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
#include "def_shmem_x_to_all.h"

#ifdef __cplusplus
extern "C" {
#endif

SHMEM_SCOPE void
shmemx_memcpy_nbi(void *dst, const void *src, size_t nbytes)
{
	static const unsigned char dma_data_shift[8] = { 3,0,1,0,2,0,1,0 };
	unsigned int data_shift = (unsigned int)dma_data_shift[(((unsigned int)dst) | ((unsigned int)src) | (unsigned int)nbytes) & 0x7];
	unsigned int data_size = (data_shift<<5);
	unsigned int stride = 0x10001 << data_shift;
	unsigned int count = 0x10000 | ((unsigned int)nbytes >> data_shift);
	unsigned int config = 0x3 | data_size;
	unsigned char* csrc = (unsigned char*)src;
	unsigned char value = ~csrc[nbytes-1];
	volatile unsigned char* cdst = (unsigned char*)dst + nbytes - 1;
	__shmem.dma_desc.count = count;
	__shmem.dma_desc.src_addr = (char*)src;
	__shmem.dma_desc.dst_addr = (char*)dst;
	__shmem.dma_desc.inner_stride = stride,
	__shmem.dma_desc.outer_stride = stride,
	__shmem.dma_desc.config = config;
#ifdef SHMEM_USE_UNSAFE // XXX dual channel DMA may not work correctly without calling shmem_quiet
	// XXX this code is possibly broken
	__asm__ __volatile__ (
		"mov r0, #15                  \n"
		".LOOKUP_DMA0STATUS%=:        \n"
		"   movfs r1, DMA0STATUS      \n"
		"   and r1,r1,r0              \n"
		"   bne .LOOKUP_DMA1STATUS%=  \n" // if DMA0 isn't busy, branch to start DMA0

		"sub r0, %[cdst0], #0         \n" // check if prior destination set
		"beq .LOAD_DMA0%=             \n"
		".LOOP0%=:                    \n"
		"   ldrb r0, [%[cdst0], #0]   \n"
		"   sub r1, r0, %[csrc0]      \n"
		"   beq .LOOP0%=              \n" // loop until prior data to complete

		".LOAD_DMA0%=:                \n"
		"strb %[value], [%[cdst], #0] \n" // store trailing inverted value
		"mov %[cdst0], %[cdst]        \n" // save destination pointer
		"mov %[csrc0], %[value]       \n" // save inverted value to be overwritten
		"movts DMA0CONFIG, %[x]       \n" // start DMA0
		"b .LEAVE%=                   \n"

		".LOOKUP_DMA1STATUS%=:        \n"
		"   movfs r1, DMA1STATUS      \n"
		"   and r1,r1,r0              \n"
		"   bne .LOOKUP_DMA0STATUS%=        \n" // loop until one DMA engine isn't busy

		"sub r0, %[cdst1], #0         \n" // check if prior destination set
		"beq .LOAD_DMA1%=             \n"
		".LOOP1%=:                    \n"
		"   ldrb r0, [%[cdst1], #0]   \n"
		"   sub r1, r0, %[csrc1]      \n"
		"   beq .LOOP1%=              \n" // loop until prior data to complete

		".LOAD_DMA1%=:                \n"
		"strb %[value], [%[cdst], #0] \n" // store trailing inverted value
		"mov %[cdst1], %[cdst]        \n" // save destination pointer
		"mov %[csrc1], %[value]       \n" // save inverted value to be overwritten
		"movts DMA1CONFIG, %[x]       \n" // start DMA1

		".LEAVE%=:                    \n"
		: [cdst0] "+r" (__shmem.cdst0),
		  [cdst1] "+r" (__shmem.cdst1),
		  [csrc0] "+r" (__shmem.csrc0),
		  [csrc1] "+r" (__shmem.csrc1)
		: [x] "r" (__shmem.dma_start),
		  [cdst] "r" (cdst),
		  [value] "r" (value)
		: "r0", "r1", "memory", "cc");
#else
	__asm__ __volatile__ (
		"sub r1, %[cdst0], #0         \n" // check if prior destination set
		"beq .LOAD_DMA0%=             \n"
		"mov r0, #15                  \n"
		".LOOP_DMA0STATUS%=:          \n"
		"   movfs r1, DMA0STATUS      \n"
		"   and r1,r1,r0              \n"
		"   bne .LOOP_DMA0STATUS%=    \n" // loop until DMA0 engine isn't busy
//		".LOOP0%=:                    \n" // XXX spinning on remote data is probably not
//		"   ldrb r0, [%[cdst0], #0]   \n" // XXX safe since it could have been modified by
//		"   sub r1, r0, %[csrc0]      \n" // XXX the remote PE
//		"   beq .LOOP0%=              \n" // loop until prior data to complete
		".LOAD_DMA0%=:                \n"
		: [cdst0] "+r" (__shmem.cdst0),
		  [csrc0] "+r" (__shmem.csrc0)
		:
		: "r0", "r1", "cc"
	);
	*cdst = value; // store trailing inverted value (to be overwritten)
	__shmem.cdst0 = cdst;
	__shmem.csrc0 = value;
	__asm__ __volatile__ (
		"movts DMA0CONFIG, %[x]       \n" // start DMA0
		:
		: [x] "r" (__shmem.dma_start)
		:
	);
#endif
	__shmem.dma_used = 1;
}

#ifdef __cplusplus
}
#endif
