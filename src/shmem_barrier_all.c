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

#include "internals.h"
#include "shmem.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SHMEM_USE_WAND_BARRIER

SHMEM_SCOPE void
shmem_barrier_all(void)
{
	shmem_quiet();
	__asm__ __volatile__ (
		"gid               \n" // disable interrupts
		"wand              \n" // wait on AND
		".balignw 8,0x01a2 \n" // nop align gie/idle pair to block
		"gie               \n" // enable interrupts
		"idle              \n" // to go sleep
	);
	__shmem.dma_used = 0; // reset
}

#else

SHMEM_SCOPE void
shmem_barrier_all(void)
{
	shmem_quiet();
	int c;
	for (c = 0; c < __shmem.n_pes_log2; c++)
	{
		volatile long* lock = (volatile long*)(__shmem.barrier_sync + c);
		*(__shmem.barrier_psync[c]) = 1;
		while (*lock == SHMEM_SYNC_VALUE);
		*lock = SHMEM_SYNC_VALUE;
	}
	__shmem.dma_used = 0; // reset
}

#endif

#ifdef __cplusplus
}
#endif
