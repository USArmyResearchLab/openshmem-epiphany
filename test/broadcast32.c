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
 * Performance test for shmem_broadcast32
 */

#include <shmem.h>

long pSyncA[SHMEM_BCAST_SYNC_SIZE] = { SHMEM_SYNC_VALUE };
long pSyncB[SHMEM_BCAST_SYNC_SIZE] = { SHMEM_SYNC_VALUE };

#define NELEMENT 2048 /* Data size chosen to be able to capture time required */
#define NLOOP 10000
#define INV_GHZ 1.66666667f // 1/0.6 GHz

int main (void)
{
	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();

	int* source = (int*)shmem_malloc(NELEMENT * sizeof (*source));
	int* target = (int*)shmem_malloc(NELEMENT * sizeof (*target));

	if (me == 0) {
		printf("# SHMEM Broadcast32 times for NPES = %d\n" \
			"# Bytes\tLatency (nanoseconds)\n", npes);
	}

	for (int elements = 1; elements <= NELEMENT; elements <<= 1)
	{
		// reset values for each iteration
		for (int i = 0; i < NELEMENT; i++) {
			source[i] = i + 1;
			target[i] = -90;
		}

		shmem_barrier_all();
		unsigned int t = __shmem_get_ctimer();
		for (int i = 0; i < NLOOP; i += 2) {
			/* alternate between 2 pSync arrays to synchronize consequent
			collectives of even and odd iterations */
			shmem_broadcast32 (target, source, elements, 0, 0, 0, npes, pSyncA);
			shmem_broadcast32 (target, source, elements, 0, 0, 0, npes, pSyncB);
		}
		t -= __shmem_get_ctimer();

		shmem_barrier_all();

		if (me == 0) {
			unsigned int bytes = elements * sizeof(*source);
			unsigned int cycles = t / NLOOP;
			float fcycles = (float)cycles;
			int nsec = (int)(fcycles * INV_GHZ);
			printf("%5d %7d\n", bytes, nsec);
		}
		else {
			int err = 0;
			for (int i = 0; i < elements; i++) if (target[i] != source[i]) err++;
			for (int i = elements; i < NELEMENT; i++) if (target[i] != -90) err++;
			if (err) printf("# %d: ERROR: %d incorrect value(s) copied\n", me, err);
		}

		shmem_barrier_all();
	}

	shmem_free(target);
	shmem_free(source);

	shmem_finalize();

	return 0;
}
