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
 * Performance test for shmem_put (latency and bandwidth)
 */

#include <host_stdio.h>
#include <shmem.h>
#include "ctimer.h"

#define NELEMENT 1024

#ifndef NLOOP
#define NLOOP 1000
#endif

int main (void)
{
	int i, nelement;
	static int ti, tsum;
	static int pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	static long pSync[SHMEM_REDUCE_SYNC_SIZE];
	for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
		pSync[i] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();

	int nxtpe = me + 1;
	if (nxtpe >= npes) nxtpe -= npes;

	long long* source = (long long*)shmem_align(NELEMENT * sizeof(long long), 0x2000);
	long long* target = (long long*)shmem_align(NELEMENT * sizeof(long long), 0x2000);
	for (i = 0; i < NELEMENT; i++) {
		source[i] = i + 1;
	}

	if (me == 0) {
		host_printf("# SHMEM Put64 times for variable message size\n" \
			"# Bytes\tLatency (nanoseconds)\n");
	}

	/* For int put we take average of all the times realized by a pair of PEs,
	thus reducing effects of physical location of PEs */
	for (nelement = 1; nelement <= NELEMENT; nelement <<= 1)
	{
		for (i = 0; i < NELEMENT; i++) { // reset values for each iteration
			target[i] = -90;
		}
		shmem_barrier_all();

		ctimer_start();
		unsigned int t = ctimer();

		for (i = 0; i < NLOOP; i++) {
			shmem_put64(target, source, nelement, nxtpe);
		}

		t -= ctimer();
		ti = (int)t;
		shmem_int_sum_to_all(&tsum, &ti, 1, 0, 0, npes, pWrk, pSync);

		if (me == 0) {
			int bytes = nelement * sizeof(*source);
			unsigned int nsec = ctimer_nsec(tsum / (npes * NLOOP));
			host_printf("%6d %7u\n", bytes, nsec);
		}

		int err = 0;
		for (i = 0; i < nelement; i++) if (target[i] != source[i]) err++;
		for (i = nelement; i < NELEMENT; i++) if (target[i] != -90) err++;
		if (err) host_printf("# %d: ERROR: %d incorrect value(s) copied\n", me, err);
	}

	shmem_free(target);
	shmem_free(source);

	shmem_finalize();

	return 0;
}
