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

SHMEM_SCOPE void
shmem_quiet(void)
{
	if (__shmem.dma_used) { // SHMEM doesn't guarantee value is available
		__asm__ __volatile__ (
			"mov r0, #15             \n" // setting r0 lower 4 bits on
			".LOOP%=:                \n"
			"   movfs r1, DMA0STATUS \n" // copy DMA0STATUS to r1
			"   and r1,r1,r0         \n" // check if DMA0STATUS != 0
			"   movfs r2, DMA1STATUS \n" // copy DMA1STATUS to r2
			"   and r2, r2, r0       \n" // check if DMA1STATUS != 0
			"   orr r2, r2, r1       \n" // check if either are != 0
			"   bne .LOOP%=          \n" // spin until both complete
			: : : "r0", "r1", "r2", "cc"
		);
		// XXX Spinning isn't a great way to guarantee the data has finished
		// XXX since another PE may have modified the value
		// XXX Also see shmemx_memcpy_nbi.c
		if (__shmem.cdst0) {
			//while (*(__shmem.cdst0) == __shmem.csrc0);
			__shmem.cdst0 = 0;
		}
		if (__shmem.cdst1) {
			//while (*(__shmem.cdst1) == __shmem.csrc1);
			__shmem.cdst1 = 0;
		}
		__shmem.dma_used = 0;
	}
}

#ifdef __cplusplus
}
#endif
