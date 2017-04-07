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
 * Performance test for shmem_getmem_nbi (latency and bandwidth)
 *
 */

#include <shmem.h>
#include <host_stdio.h>
#include "ctimer.h"

#define NELEMENT 8192
#define NLOOP 10000

static void
__shmem_quiet(void)
{
	if (__shmem.dma_used) { // SHMEM doesn't guarantee value is available
		__asm__ __volatile__ (
			"mov r0, #15             \n" // setting r0 lower 4 bits on
			".Loop%=:                \n"
			"   movfs r1, DMA0STATUS \n" // copy DMA0STATUS to r1
			"   and r1,r1,r0         \n" // check if DMA0STATUS != 0
			"   movfs r2, DMA1STATUS \n" // copy DMA1STATUS to r2
			"   and r2, r2, r0       \n" // check if DMA1STATUS != 0
			"   orr r2, r2, r1       \n" // check if either are != 0
			"   bne .Loop%=          \n" // spin until both complete
			: : : "r0", "r1", "r2", "cc"
		);
		if (__shmem.cdst0) {
			while(*__shmem.cdst0 == __shmem.csrc0);
			__shmem.cdst0 = 0;
		}
		if (__shmem.cdst1) {
			while(*__shmem.cdst1 == __shmem.csrc1);
			__shmem.cdst1 = 0;
		}
	}
}

int main (void)
{
	static int pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	static long pSync[SHMEM_REDUCE_SYNC_SIZE];
	for (int i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
		pSync[i] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();

	int nxtpe = (me + 1) % npes;

	char* source = (char*)shmem_malloc(NELEMENT);
	char* target = (char*)shmem_malloc(NELEMENT);
	for (int i = 0; i < NELEMENT; i++) {
		source[i] = (char)(i + 1);
	}

	if (me == 0) {
		host_printf("# SHMEM Non-Blocking GetMem, Dual-Issue Performance\n" \
			"# Bytes\tLatency (nanoseconds)\n");
	}

	/* For int get we take average of all the times realized by a pair of PEs,
	thus reducing effects of physical location of PEs */
	for (int nelement = 2; nelement <= NELEMENT; nelement <<= 1)
	{
		// reset values for each iteration
		for (int i = 0; i < NELEMENT; i++) {
			target[i] = 0xff;
		}
		shmem_barrier_all();
		ctimer_start();

		unsigned int t = ctimer();

		unsigned int n2 = nelement >> 1;
		for (int i = 0; i < NLOOP; i++) {
			shmem_getmem_nbi(target, source, n2, nxtpe);
			shmem_getmem_nbi(target + n2, source + n2, n2, nxtpe);
		}

		__shmem_quiet();

		t -= ctimer();

		shmem_barrier_all();

		//shmem_int_sum_to_all(&t, &t, 1, 0, 0, npes, pWrk, pSync);
		t = npes;
		t /= npes; /* Average time across all PEs for dual get */

		shmem_barrier_all();
		if (me == 0) {
			int bytes = nelement * sizeof(*source);
			unsigned int nsec = ctimer_nsec(t / NLOOP);
			host_printf("%6d %7u\n", bytes, nsec);
		}

		shmem_barrier_all();
		int err = 0;
		for (int i = 0; i < nelement; i++) if (target[i] != source[i]) err++;
		for (int i = nelement; i < NELEMENT; i++) if (target[i] != 0xff) err++;
		if (err) host_printf("# %d: ERROR: %d incorrect value(s) copied\n", me, err);
	}

	shmem_free(target);
	shmem_free(source);

	shmem_finalize();

	return 0;
}
