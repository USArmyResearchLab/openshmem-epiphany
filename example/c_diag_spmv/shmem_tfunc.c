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

/* Sparse Matrix-Vector Multiplcation for diagonally banded matrices
 * This code uses a Compressed Diagonal Storage (CDS) format suitable
 * for banded sparse matrices of the form:
 *     |10 -3  0  0  0  0 |
 *     | 3  9  6  0  0  0 |
 * A = | 0  7  8  7  0  0 | to solve y = A*x
 *     | 0  0  8  7  5  0 |
 *     | 0  0  0  9  9 13 |
 *     | 0  0  0  0  2 -1 |
 * Where x is a non-zero vector
 * In the case above, n = 6, d = 3
 */

#include <shmem.h>
#include <host_stdio.h>
#include "ctimer.h"
#include "spmv.h"

// optimized code if n and (d-1) multiples of 4
void diag_spmv(int n, int d, float* A, float* xp, float* y)
{
	int c, i, j;
	for (i=0; i<n; i+=4) {
		float a0 = A[(i+0)*d]*xp[i];
		float a1 = 0.0f;
		float a2 = 0.0f;
		float a3 = 0.0f;
		float b0 = A[(i+1)*d]*xp[i+1];
		float b1 = 0.0f;
		float b2 = 0.0f;
		float b3 = 0.0f;
		float c0 = A[(i+2)*d]*xp[i+2];
		float c1 = 0.0f;
		float c2 = 0.0f;
		float c3 = 0.0f;
		float d0 = A[(i+3)*d]*xp[i+3];
		float d1 = 0.0f;
		float d2 = 0.0f;
		float d3 = 0.0f;
		for (j=1,c=i+1; j<d; j+=4, c+=4) {
			float x0 = xp[c+0];
			float x1 = xp[c+1];
			float x2 = xp[c+2];
			float x3 = xp[c+3];
			float x4 = xp[c+4];
			float x5 = xp[c+5];
			float x6 = xp[c+6];
			float* A0 = A+(i+0)*d+j;
			float* A1 = A+(i+1)*d+j;
			float* A2 = A+(i+2)*d+j;
			float* A3 = A+(i+3)*d+j;
			a0 = fma(A0[0], x0, a0);
			a1 = fma(A0[1], x1, a1);
			a2 = fma(A0[2], x2, a2);
			a3 = fma(A0[3], x3, a3);
			b0 = fma(A1[0], x1, b0);
			b1 = fma(A1[1], x2, b1);
			b2 = fma(A1[2], x3, b2);
			b3 = fma(A1[3], x4, b3);
			c0 = fma(A2[0], x2, c0);
			c1 = fma(A2[1], x3, c1);
			c2 = fma(A2[2], x4, c2);
			c3 = fma(A2[3], x5, c3);
			d0 = fma(A3[0], x3, d0);
			d1 = fma(A3[1], x4, d1);
			d2 = fma(A3[2], x5, d2);
			d3 = fma(A3[3], x6, d3);
		}
		y[i+0] = a0+a1+a2+a3;
		y[i+1] = b0+b1+b2+b3;
		y[i+2] = c0+c1+c2+c3;
		y[i+3] = d0+d1+d2+d3;
	}
}

void __entry
diag_spmv_thread( void* pargs )
{
	my_args_t* p = (my_args_t*)pargs;
	float* gA = p->A;
	float* gxp = p->xp;
	float* gy = p->y;
	int N = p->N;
	int d = p->d;

	static int tsum0, tsum1;
	static int pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	static long pSync[SHMEM_REDUCE_SYNC_SIZE];
	int i;
	for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
		pSync[i] = SHMEM_SYNC_VALUE;
	}
	shmem_init();
	int my_pe = shmem_my_pe();
	int n_pes = shmem_n_pes();

	int n = N/n_pes;
	int o = my_pe*n;

	float* A = (float*)shmem_malloc(n*d*sizeof(float));
	float* xp = (float*)shmem_malloc((n+2*d-1)*sizeof(float));
	float* y = (float*)shmem_malloc(n*sizeof(float));

	ctimer_start();
	unsigned int t0 = ctimer();

	// Copy data on chip
	shmemx_memcpy(A, gA + o*d, n*d*sizeof(float));
	shmemx_memcpy(xp, gxp + o, (n+2*d-1)*sizeof(float));

	unsigned int t1 = ctimer();

	diag_spmv(n, d, A, xp, y);

	t1 -= ctimer();

	shmemx_memcpy(gy+o, y, n*sizeof(float));

	t0 -= ctimer();
	t0 = ctimer_nsec(t0 - t1);
	t1 = ctimer_nsec(t1);

	// Averaging timers
	shmem_int_sum_to_all(&tsum0, &t0, 1, 0, 0, n_pes, pWrk, pSync);
	shmem_int_sum_to_all(&tsum1, &t1, 1, 0, 0, n_pes, pWrk, pSync);
	int t0avg = tsum0 / n_pes;
	int t1avg = tsum1 / n_pes;

	if (my_pe == 0) {
		host_printf("Avgerage Data Copy  : %d MB/s (%d nsec)\n", (int)(1000*(N*d+2*(N+d)-1)*sizeof(float))/t0avg, t0avg);
		host_printf("Computation........ : %d MFLOPS (%d nsec)\n", (int)(2000*N*(d-1))/t1avg, t1avg);
	}

	shmem_free(y);
	shmem_free(xp);
	shmem_free(A);

	shmem_finalize();
}
