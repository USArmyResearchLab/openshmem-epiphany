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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

#include "coprthr.h"
#include "coprthr_cc.h"
#include "coprthr_thread.h"
#include "ctimer.h"

typedef struct {
	int N, k, d, t;
	float *gA, *gx, *gb;
} args_t;

#define ERROR(...) do{fprintf(stderr,__VA_ARGS__); exit(-1);}while(0)

// Solves x for A*x = b where A is an N x N matrix is x and b are vectors of size N

// calculates b for A*x = b
void matvecmult(int n, float* b, float* A, float* x)
{
	int i, j;
	for (j = 0; j < n; j++) {
		float t = 0.0f;
		for (i = 0; i < n; i++) {
			t += A[j*n+i] * x[i];
		}
		b[j] = t;
	}
}

int main(int argc, char* argv[])
{
	int i, j, k;
	int row;
	int N = SIZE;
	int d = EDIM;
	int v = 0;

	i = 1;
	while (i < argc) {
		if (!strcmp(argv[i], "-n")) N = atoi(argv[++i]);
		else if (!strcmp(argv[i], "-d")) d = atoi(argv[++i]);
		else if (!strcmp(argv[i], "-v")) v = 1;
		else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) goto help;
		else {
			fprintf(stderr, "unrecognized option: %s\n", argv[i]);
			help:
			ERROR("use -n [on-chip matrix dimension] -d [eCore dimension]\n");
		}
		++i;
	}

	printf("# Using -n=%d, -d=%d\n", N, d);

	// open device for threads
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32, COPRTHR_O_THREAD);
	if (dd < 0) ERROR("device open failed\n");

	// get thread function
	coprthr_program_t prg = coprthr_cc_read_bin("./shmem_tfunc.e32", 0);
	if (!prg) ERROR("cannot open binary\n");
	coprthr_sym_t thr = coprthr_getsym(prg, "cg_thread");
	if (!thr) ERROR("cannot read kernel\n");

	// allocate host memory
	float* x_soln = (float*)malloc(N*sizeof(float));

	// allocate host-device addressable memory, setup pointers
	coprthr_mem_t gA_mem = coprthr_dmalloc(dd, N * N * sizeof(float), 0);
	coprthr_mem_t gx_mem = coprthr_dmalloc(dd, N * sizeof(float), 0);
	coprthr_mem_t gb_mem = coprthr_dmalloc(dd, N * sizeof(float), 0);
	coprthr_mem_t argmem = coprthr_dmalloc(dd, sizeof(args_t), 0);
	float* gA = coprthr_memptr(gA_mem, 0);
	float* gx = coprthr_memptr(gx_mem, 0);
	float* gb = coprthr_memptr(gb_mem, 0);
	args_t* args = (args_t*)coprthr_memptr(argmem, 0);

	// set thread function arguments
	*args = (args_t){ .N = N, .k = N, .d = d, .gb = gb, .gA = gA, .gx = gx };

	// initialize A matrix and x solution vector (used to verify result)
	for (i = 0; i < N; i++) {
		x_soln[i] = (float)(i + 1) * 0.25f;
		for (j = 0; j < N; j++) {
			float dij = fabs((float)(j - i));
			float dij2 = dij*dij;
			float t = 1.0f / (dij2 + 0.1f);
			float t2 = t * t;
			gA[i * N + j] = t2;
		}
	}

	// calculating Ax = b, where b is used to initialize r0, p0
	matvecmult(N, gb, gA, x_soln);

	ctimer_start();
	unsigned int t = ctimer();

	// launch threads
	coprthr_attr_t attr;
	coprthr_attr_init(&attr);
	coprthr_attr_setdetachstate(&attr, COPRTHR_CREATE_JOINABLE);
	coprthr_attr_setdevice(&attr, dd);
	coprthr_td_t td;
	int cores = d*d;
	coprthr_ncreate(cores, &td, &attr, thr, (void*)&argmem);
	void* status;
	coprthr_join(td, &status);

	t -= ctimer();

	// checking for errors
	int errors = 0;
	for (i = 0; i < N; i++) {
			int e = 0;
			float x = gx[i];
			float r = x_soln[i];
			float diff = fabsf(x / r - 1.0f);
			if(diff > 0.01f || isnan(x)) e = 1;
			if (v) printf("%3d: %f %f %s\n", i, x, r, e ? "X" : "");
			errors += e;
	}

	double gflops = 2.0 * args->k * N * (N + 2.0) / args->t;
	printf("# offload time: %f msec\n", 1.0e-6 * ctimer_nsec(t));
	printf("# on-chip time: %f msec\n", 1.0e-6 * args->t);
	printf("# iterations  : %d\n", args->k);
	printf("# GFLOPS      : %f\n", gflops);
	printf("# errors      : %d (%0.2f%%)\n", errors, 100.0f*errors/N);

	// clean up
	coprthr_attr_destroy(&attr);
	coprthr_dfree(dd, argmem);
	coprthr_dfree(dd, gb_mem);
	coprthr_dfree(dd, gx_mem);
	coprthr_dfree(dd, gA_mem);
	coprthr_dclose(dd);
}
