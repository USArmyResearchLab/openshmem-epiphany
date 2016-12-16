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

void* __shmem_free_mem;

int __shmemx_brk(const void* ptr)
{
	__shmem_free_mem = (void*)ptr;
	return 0;
}

void* __attribute__((malloc)) __shmemx_sbrk(size_t size)
{
	void* ptr = __shmem_free_mem;
	__shmem_free_mem += (size + 7) & 0xfffffff8; // Double-word alignment
	return ptr;
}


void* shmem_ptr(const void* dest, int pe)
{
	unsigned int row = pe >> SHMEM_ROW_SHIFT;
	unsigned int col = pe & SHMEM_ROW_MASK;
	unsigned int coreid = (row*0x40 + col) + e_group_config.group_id;
	void* remote = (void*)((coreid << 20) | (unsigned int) dest);
	return remote;
}

void* __attribute__((malloc)) shmem_malloc(size_t size)
{
	return shmemx_sbrk(size);
}

void* __attribute__((malloc)) shmem_align(size_t alignment, size_t size)
{
	unsigned int minus1 = alignment - 1;
	unsigned int mask = ~(minus1);
	unsigned int x = (unsigned int)shmemx_sbrk(0);
	unsigned int y = ((x + minus1) & mask) - x;
	shmemx_sbrk(y); // advance to alignment address
	return shmemx_sbrk(size);
}

void shmem_free(const void *ptr)
{
	if ((unsigned int)ptr < (unsigned int)shmemx_sbrk(0))
	shmemx_brk(ptr);
}

void* shmem_realloc(const void* ptr, size_t size)
{
	shmemx_brk(ptr);
	return shmemx_sbrk(size);
}

