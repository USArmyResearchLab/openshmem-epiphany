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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "coprthr.h"
#include "coprthr_cc.h"
#include "coprthr_thread.h"
#include "coprthr_mpi.h"
#include "spmv.h"
#include "ctimer.h"

#define ERROR(...) do{fprintf(stderr,__VA_ARGS__); exit(-1);}while(0)

void diag_spmv_epiphany(float* A, float* xp, float* y, int n, int d, int cores)
{
	// open device for threads
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_THREAD);
	if (dd < 0) ERROR("device open failed\n");

	coprthr_program_t prg;
	prg = coprthr_cc_read_bin("./shmem_tfunc.e32", 0);
	if (!prg) ERROR("invalid program\n");
	coprthr_sym_t thr = coprthr_getsym(prg, "diag_spmv_thread");
	if (!thr) ERROR("invalid symbol\n");

	// allocating on shared memory
	coprthr_mem_t A_mem = coprthr_dmalloc(dd, n*d*sizeof(float),0);
	coprthr_mem_t xp_mem = coprthr_dmalloc(dd, n*(2*d-1)*sizeof(float),0);
	coprthr_mem_t y_mem = coprthr_dmalloc(dd, n*sizeof(float),0);

	// write data to shared DRAM
	coprthr_dwrite(dd, A_mem, 0, A, n*d*sizeof(float), COPRTHR_E_WAIT);
	coprthr_dwrite(dd, xp_mem, 0, xp, n*(2*d-1)*sizeof(float), COPRTHR_E_WAIT);

	my_args_t args = {
		.N = n, 
		.d = d,
		.A = coprthr_memptr(A_mem,0),
		.xp = coprthr_memptr(xp_mem,0),
		.y = coprthr_memptr(y_mem,0)
	};

	coprthr_mpiexec(dd, cores, thr, &args, sizeof(args),0);

	// read back data from memory on device
	coprthr_dread(dd, y_mem, 0, y, n*sizeof(float), COPRTHR_E_WAIT);

	coprthr_dclose(dd);
}

void diag_spmv(float* A, float* xp, float* y, int n, int d)
{
	int c, i, j;
	for (i=0; i<n; i++) {
		float a = 0.0f;
		for (j=0,c=i; j<d; j++, c++) {
			a += A[i*d + j] * xp[c];
		}
		y[i] = a;
	}
}

int main(int argc, char** argv)
{
	int i,j,k;
	double mflops;
	int validate = 0;

	int n = SIZE;
	int d = DIAGS;
	int c = ECORE;

	i = 1;
	while (i < argc) {
		if (!strcmp(argv[i],"-n")) n = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-d")) d = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-c")) c = atoi(argv[++i]);
		else if (!strcmp(argv[i],"--validate")) validate = 1;
		else if (!strcmp(argv[i],"--help") || !strcmp(argv[i],"-h")) goto help;
		else {
			fprintf(stderr,"unrecognized option: %s\n",argv[i]);
			help:
			ERROR("use -n [length of diagonal] -d [number of diagonals] -c [number of Epiphany cores/threads] --validate\n");
		}
		++i;
	}

	printf("Using -n=%d, -d=%d, -c=%d\n", n, d, c);

	if (n % c) ERROR("Length of diagonal (%d) must be a multiple of number of cores (%d)\n", n, c);
	if ((d-1) & 0x3) ERROR("Number of diagonals (%d) must be factor of 4 + 1 (5,9,13,etc) from unrolling\n", d);

	// allocate memory on device and write a value
	int o = (d-1)/2;
	float* A = (float*)malloc(n*d*sizeof(float));
	float* xp = (float*)calloc((n+d-1),sizeof(float)); // padded x-vector
	float* x = xp + o; // x vector with offset
	float* y = (float*)calloc(n,sizeof(float));

	// initializing
	for (i=0; i<n; i++) {
		x[i] = (float)(i+1);
		for (j=0; j<d; j++) {
			A[i*d + j] = (float)(i*d+j+1.0f);
		}
	}
	for (i=0; i<o; i++) {
		for (j=0; j<o-i; j++) {
			A[i*d + j] = 0.0f;
			A[(n-i)*d - j - 1] = 0.0f;
		}
	}

	ctimer_start();
	unsigned int t0 = ctimer();
	diag_spmv_epiphany(A, xp, y, n, d, c);
	t0 -= ctimer();

	double time = (double)ctimer_nsec(t0)/1.0e9;
	printf("Cold Start Time.... : %f sec\n", time);

	if (validate) {
		printf("Validating on CPU host....\n");
		float* y_cpu = (float*)calloc(n,sizeof(float));

		t0 = ctimer();
		diag_spmv(A, xp, y_cpu, n, d);
		t0 -= ctimer();
		double time = (double)ctimer_nsec(t0)/1.0e9;

		float max_diff = 0.0f;
		int errors = 0;
		for (i=0; i<n; i++) {
			float diff = fabs((y_cpu[i]-y[i])/y_cpu[i]);
			max_diff = (diff>max_diff) ? diff : max_diff;
			if (diff > 1e-6f) errors++;
		}

		mflops = ((double)(1000*2*d-1)*n)/t0;
		printf("ARM CPU Performance : %3.2f MFLOPS\n",mflops);
		printf("Execution time..... : %f sec\n", time);
		printf("Errors............. : %d (%3.2f%%)\n", errors, (float)errors*100.0f/n);
		printf("Max Difference..... : %3.2e%%\n", max_diff*100.0f);
	}
}
