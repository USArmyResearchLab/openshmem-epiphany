/*
 * Copyright (c) 2016 U.S. Army Research laboratory. All rights reserved.
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
 * 
 * This software was developed by Brown Deer Technology, LLC. with Copyright
 * assigned to the US Army Research laboratory as required by contract.
 */

/*
 * Performance test for shmem_int_fetch
 */

#include <shmem.h>

#define NLOOP 10000
#define INV_GHZ 1.66666667f // 1/0.6 GHz

long pSync[SHMEM_REDUCE_SYNC_SIZE] = { SHMEM_SYNC_VALUE };
int pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
int dest = 0;

int main (void)
{
	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();

	int nxtpe = (me + 1) % npes;

	if (me == 0) {
		printf("# SHMEM Atomic Fetch Performance for variable NPES\n" \
			"# NPES\tLatency (nanoseconds)\n");
	}

	for (unsigned int npe = 2; npe <= npes; npe++)
	{
		unsigned int t = 0;
		shmem_barrier_all();

		if (me < npe) {
			t = __shmem_get_ctimer();
			for (int i = 0; i < NLOOP; i++) {
				shmem_int_fetch(&dest, nxtpe);
			}
			t -= __shmem_get_ctimer();
		}

		shmem_barrier_all();

		shmem_int_sum_to_all(&t, &t, 1, 0, 0, npes, pWrk, pSync);
		t /= npe;

		if (me == 0) {
			int cycles = t / NLOOP;
			float fcycles = (float)cycles;
			int nsec = (int)(fcycles * INV_GHZ);
			printf("%5d %7d\n", npe, nsec);
		}
	}
	shmem_finalize();

	return 0;
}
