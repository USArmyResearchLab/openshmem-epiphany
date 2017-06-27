/*
 * Copyright (c) 2015-2017, James A. Ross
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This code demonstrates the dense conjugate gradient method which solves 'x'
 * for A*x = b, where 'A' is a square matrix and 'x' and 'b' are vectors. The
 * 'A' matrix must be symmetric and positive-definite and is initialized in the
 * host code. The device (Epiphany) code distributes the 'A' matrix in two
 * dimensions whereas the 'x' and 'b' vectors are split in one dimension with
 * multiple copies in the row of Epiphany cores.
 *
 * -JAR
 */

#include <shmem.h>
#include <shmemx.h>
#include <ctimer.h>

typedef struct {
	int N, k, d, t;
	float *gA, *gx, *gb;
} args_t;

long pSync[SHMEM_REDUCE_SYNC_SIZE];

float dotprod(float* a, float* b, int n); // assembly-optimized routine

static void matvecprod(float* A, float* x, float* b, float* scratch, int n, int d, float* pWrk, int row)
{
	int i;
	// partial matrix-vector multiplication
	for (i = 0; i < n; i++)
		scratch[i] = dotprod(A + i*n, x, n);

	// collecting partial results
	shmem_float_sum_to_all(b, scratch, n, d * row, 0, d, pWrk, pSync);
}

void __entry
cg_thread(args_t* pargs)
{
	int i, j, k;
	int N = pargs->N, d = pargs->d;
	int kmax = pargs->k;
	float *gb = pargs->gb, *gA = pargs->gA, *gx = pargs->gx;
	int n = N/d;
	float invd = 1.0f / d;
	static float num, denom, num_part, denom_part;

	int pWrk_elems = (n >> 1) + 1;
	pWrk_elems = (pWrk_elems > SHMEM_REDUCE_MIN_WRKDATA_SIZE) ? pWrk_elems : SHMEM_REDUCE_MIN_WRKDATA_SIZE;
	for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) pSync[i] = SHMEM_SYNC_VALUE;

	// Initialize OpenSHMEM and timer
	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();
	ctimer_start();

	// works for 1, 4, and 16 cores (d^4 cores)
	int col = me & (d - 1);
	int row = me;
	int stride = 0;
	switch (d) {
		case 4: stride++;
		case 2: stride++;
		case 1: break;
	}
	row >>= stride;

	// allocate symmetric memory
	float* A = (float*)shmem_malloc(n*n*sizeof(float));
	float* x = (float*)shmem_malloc(n*sizeof(float));
	float* b = (float*)shmem_malloc(n*sizeof(float));
	float* s = (float*)shmem_malloc(n*sizeof(float));
	float* r = (float*)shmem_malloc(n*sizeof(float));
	float* p = (float*)shmem_malloc(n*sizeof(float));
	float* pWrk = (float*)shmem_malloc(pWrk_elems*sizeof(float));

	// read input
	for (i = 0; i < n; i++) shmemx_memcpy(A + i*n, gA + (row*n + i)*N + col*n, n *sizeof(float));
	// XXX It may be more efficient to have one row read the vector and share with the rest of the column
	shmemx_memcpy(b, gb + row*n, n*sizeof(float));

	// Initialize solution and gradient vectors
	for (i=0; i<n; i++) {
		x[i] = 0.0f;
		r[i] = p[i] = b[i];
	}

	int t = ctimer();

	num_part = dotprod(r, r, n);
	shmem_float_sum_to_all(&num, &num_part, 1, col, stride, d, pWrk, pSync);

	for (k = 1; k < kmax; k++)
	{
		// b = A*p
		matvecprod(A, p, b, s, n, d, pWrk, row);
		denom_part = dotprod(p, b, n);

		shmem_float_sum_to_all(&denom, &denom_part, 1, 0, 0, npes, pWrk, pSync);
		denom *= invd;

		// checking convergence, break before unstable
		// XXX potential asymmetric break if order of operations in reduction lead to slightly different values on each PE
		if (denom < num) break;
		float alpha = num/denom;

		for (i = 0; i < n; i++) {
			x[i] += alpha * p[i];
			r[i] -= alpha * b[i];
		}

		denom = num;
		num_part = dotprod(r, r, n);

		shmem_float_sum_to_all(&num, &num_part, 1, 0, 0, npes, pWrk, pSync);
		num *= invd;

		float beta = num/denom;

		for (i = 0; i < n; i++)
			p[i] = r[i] + beta * p[i];
	}
	t -= ctimer();
	t = ctimer_nsec(t);

	if (!me) {
		pargs->k = k;
		pargs->t = t;
	}

	// write result (only need one PE per row to write)
	if (!col) shmemx_memcpy(gx + row * n, x, n * sizeof(float));

	// release resources
	shmem_free(pWrk);
	shmem_free(p);
	shmem_free(r);
	shmem_free(s);
	shmem_free(b);
	shmem_free(x);
	shmem_free(A);
	shmem_finalize();
}
