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

void
shmemx_memcpy_nbi(void *dest, const void *src, size_t nbytes)
{
	static const unsigned char dma_data_shift[8] = { 3,0,1,0,2,0,1,0 };
	unsigned int data_shift = (unsigned int)dma_data_shift[(((unsigned int)dest) | ((unsigned int)src) | (unsigned int)nbytes) & 0x7];
	unsigned int data_size = (data_shift<<5);
	unsigned int stride = 0x10001 << data_shift;
	unsigned int count = 0x10000 | ((unsigned int)nbytes >> data_shift);
	unsigned int config = 0x3 | data_size;
	unsigned char* csrc = (unsigned char*)src;
	unsigned char value = ~csrc[nbytes-1];
	unsigned char* cdst = (unsigned char*)dest + nbytes - 1;
	*cdst = value;
	__shmem.dma_desc.count = count;
	__shmem.dma_desc.src_addr = (char*)src;
	__shmem.dma_desc.dst_addr = (char*)dest;
	__shmem.dma_desc.inner_stride = stride,
	__shmem.dma_desc.outer_stride = stride,
	__shmem.dma_desc.config = config;
	__shmem.dma_used = 1;
	unsigned int dmachannel;
	__asm__ __volatile__ (
		"mov r0, #15             \n"
		"mov r2, #0              \n"
		".Loop%=:                \n"
		"   movfs r1, DMA0STATUS \n"
		"   and r1,r1,r0         \n"
		"   beq .Lconfig%=       \n" // if DMA0 isn't busy, branch to start DMA0
	 	"   movfs r1, DMA1STATUS \n"
 		"   and r1,r1,r0         \n"
	 	"   bne .Loop%=          \n" // loop until one DMA engine isn't busy
 		"movts DMA1CONFIG, %[x]  \n" // start DMA1
		"mov r2, #1              \n"
		"b .Ldone%=              \n"
		".Lconfig%=:             \n"
		"movts DMA0CONFIG, %[x]  \n" // start DMA0
		".Ldone%=:               \n"
		"mov %[y], r2            \n"
		: [y] "=r" (dmachannel)
		: [x] "r" (__shmem.dma_start)
		: "r0", "r1", "r2", "memory", "cc");
		if(dmachannel) {
			__shmem.cdst1 = cdst;
			__shmem.csrc1 = value;
		}
		else {
			__shmem.cdst0 = cdst;
			__shmem.csrc0 = value;
		}
}

