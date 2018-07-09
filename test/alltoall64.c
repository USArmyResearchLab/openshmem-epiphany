/*
 * Copyright (c) 2016-2018 U.S. Army Research laboratory. All rights reserved.
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
 * Performance test for shmem_alltoall64
 *
 */

#include <host_stdio.h>
#include <shmem.h>
#include "ctimer.h"

#define MAXBYTES 8192

#ifndef NLOOP
#define NLOOP 1000
#endif

int main (void)
{
	int i, j, nelement;
	static int ti, tsum;
	static long pSyncA[SHMEM_ALLTOALL_SYNC_SIZE];
	static long pSyncB[SHMEM_ALLTOALL_SYNC_SIZE];
	static int pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	static long pSync[SHMEM_REDUCE_SYNC_SIZE];
	for (i = 0; i < SHMEM_ALLTOALL_SYNC_SIZE; i++) {
		pSyncA[i] = SHMEM_SYNC_VALUE;
		pSyncB[i] = SHMEM_SYNC_VALUE;
	}
	for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
		pSync[i] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();
	int maxelement = MAXBYTES / (npes * sizeof(long long));
	int sz = maxelement * npes * sizeof(long long);

	long long* source = (long long*)shmem_malloc(sz);
	long long* target = (long long*)shmem_malloc(sz);
	for (i = 0; i < maxelement * npes; i++) {
		source[i] = me;
		target[i] = -90;
	}

	if (me == 0) {
		host_printf("# SHMEM AlltoAll64 times for NPES = %d\n" \
			"# Bytes\tLatency (nanoseconds)\n", npes);
	}

	for (nelement = 1; nelement <= maxelement; nelement <<= 1)
	{
		shmem_barrier_all();

		ctimer_start();
		unsigned int t = ctimer();
		for (i = 0; i < NLOOP; i += 2) {

			shmem_alltoall64 (target, source, nelement, 0, 0, npes, pSyncA);
			shmem_alltoall64 (target, source, nelement, 0, 0, npes, pSyncB);
		}

		t -= ctimer();
		ti = (int)t;
		shmem_int_sum_to_all(&tsum, &ti, 1, 0, 0, npes, pWrk, pSync);

		if (me == 0) {
			unsigned int bytes = nelement * sizeof(*source);
			unsigned int nsec = ctimer_nsec(tsum / (npes * NLOOP));
			host_printf("%5d %7u\n", bytes, nsec);
		}
		unsigned int err = 0;
		for (j = 0; j < npes; j++) {
			for (i = 0; i < nelement; i++) {
				if (target[j*nelement + i] != j) err++;
			}
		}
		if (err) host_printf("%d: %d ERRORS\n", me, err);
	}

	shmem_free(target);
	shmem_free(source);

	shmem_finalize();

	return 0;
}
