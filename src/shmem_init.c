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


#include "shmem.h"
#include "internals.h"

shmem_internals_t __shmem = { 0 };

void shmem_init(void)
{
	__asm__ __volatile__ (
		"movfs %[id], COREID \n" // storing COREID register value
		: [id] "=r" (__shmem.coreid)
	);
#if defined(__coprthr_device__)
	__shmem.n_pes = coprthr_get_num_threads();
#else
	__shmem.n_pes = e_group_config.group_rows * e_group_config.group_cols;
#endif
	__shmem.n_pes_log2 = __log2_ceil16(__shmem.n_pes);
	unsigned int coreid = __shmem.coreid - e_group_config.group_id;
	unsigned int row = (coreid >> 6) & 0x3f;
	unsigned int col = (coreid) & 0x3f;
	__shmem.my_pe = row*e_group_config.group_rows + col;
	__shmem.dma_start = ((int)(&__shmem.dma_desc) << 16) | 0x8;
#ifdef SHMEM_USE_WAND_BARRIER
	__shmem_wand_barrier_init();
#else
	__shmem_dissemination_barrier_init();
#endif
#ifdef SHMEM_USE_IPI_GET
	__shmem_ipi_get_init();
#endif
	__shmem.lock_high_bits = (unsigned int)shmem_ptr(NULL, 0); // using PE 0 for all global locks
#ifdef SHMEM_USE_CTIMER
	__shmem_ctimer_start();
#endif
#if !defined(__coprthr_device__)
	extern char _end;
	__shmem.free_mem = (void*)&_end; // This should already be double-word aligned
#endif
	__shmem.local_mem_base = (void*)shmemx_sbrk(0);
	int stride = SHMEM_HEAP_START - (int)__shmem.local_mem_base;
	if (stride > 0) shmemx_sbrk(stride); // advance to SHMEM_HEAP_START address
	shmem_barrier (0, 0, __shmem.n_pes, (long*)__shmem.barrier_sync);
}
