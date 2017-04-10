/*
 * Copyright (c) 2016-2017, James A. Ross
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * The OpenSHMEM programming model is based on Single Program, Multiple Data
 * stream (SPMD), however, it is possible to execute different code paths and
 * have certain cores synchronize within a workgroup. This is an example of how
 * one might run code on Epiphany in a Multiple Programs, Multiple Data streams
 * (MPMD)-like manner.
 *
 * Below, 16 different subroutines are defined. Each of them has the same input
 * arguments and result for convenience. They also include the function
 * attribute '__dynamic_call', which sets up the code to dynamically load each
 * function at runtime, thereby decreasing the total program size. With 16
 * different subroutines, the total program size may be prohibitively large if
 * you do not use this dynamic call method. The compilation must also include
 * the '-fdynamic-calls' flag, otherwise code will remain and execute in DRAM.
 *
 * A function pointer table is also created to simplify the launch of each
 * routine. The first 5 PEs will launch a function which shares data by
 * rotating it around in the following manner:
 *
 * (0 -> 1), (1 -> 2), (2 -> 3), (3 -> 4), (4 -> 0)
 *
 * Each of the first 5 PEs also include a symmetric barrier operation,
 * demonstrating synchronization between subroutines. If the shmem_barrier
 * operation is removed, the data may not have completed copying by the time
 * the result is printed.
 *
 * Macros are used to reduce the amount of code below.
 *
 * -JAR
 */

#include <coprthr.h>
#include <host_stdio.h>
#include <shmem.h>

#define N 512

// global arrays to use in separate subroutines
long pSync[SHMEM_BARRIER_SYNC_SIZE];
int shared_data0[N];
int shared_data1[N];

#define FNX(X) \
void __dynamic_call fn##X(void) \
{ host_printf("fn%d\n", X); }
#define FNXB(X,START,SIZE) \
void __dynamic_call fn##X(void) \
{ \
	int next_pe = (shmem_my_pe() + 1) % 5; \
	shmem_put(shared_data1, shared_data0, N, next_pe); \
	shmem_barrier(START,0,SIZE,pSync); \
	host_printf("fn%d shared_data1[%d] = %d\n", X, N-1, shared_data1[N-1]); \
}

FNXB(0,0,5) FNXB(1,0,5) FNXB(2,0,5) FNXB(3,0,5) FNXB(4,0,5)
FNX(5) FNX(6) FNX(7) FNX(8) FNX(9) FNX(10)
FNX(11) FNX(12) FNX(13) FNX(14) FNX(15)

void (*pfn[])(void) = {
	&fn0, &fn1, &fn2, &fn3, &fn4, &fn5, &fn6, &fn7,
	&fn8, &fn9, &fn10, &fn11, &fn12, &fn13, &fn14, &fn15
};

int main(void)
{
	int i;
	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();
	if (npes < 5) {
		host_printf("ERROR: you should run this with 5 or more cores\n");
		goto done;
	}

	// initializing data
	for (i = 0; i < SHMEM_BARRIER_SYNC_SIZE; i++)
		pSync[i] = SHMEM_SYNC_VALUE;
	int val = (me < 5) ? me : 0;
	for (i = 0; i < N; i++) {
		shared_data0[i] = val;
		shared_data1[i] = 0;
	}

	// waiting for all PEs to initialize data
	shmem_barrier_all();

	// call the various subroutines
	pfn[me]();

	done:
		shmem_finalize();
		return 0;
}
