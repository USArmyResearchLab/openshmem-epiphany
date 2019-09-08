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

#ifndef _def_shmem_collect_n_h
#define _def_shmem_collect_n_h

/* XXX I can't imagine a better algorithm than linear where each PE passes
 * their chunk to all other PEs. This is a horrible routine with variable
 * nelems and you should avoid using it */
#define SHMEM_COLLECT_N(N,T) \
SHMEM_SCOPE void \
shmem_collect##N (void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync) \
{ \
	const int my_pe = __shmem.my_pe; \
	const int pe_step = 1 << logPE_stride; \
	const int pe_end = PE_start + ((PE_size - 1) << logPE_stride); \
	int my_offset = 0; \
	/* The SHMEM_COLLECT_SYNC_SIZE is padded with two extra values for \
	 * syncronization and are not used in the shmem_sync */ \
	volatile long* lsync = pSync + SHMEM_COLLECT_SYNC_SIZE - 2; \
	if (my_pe != PE_start) { /* spin until neighbor sets offset */ \
		while (!*lsync) my_offset = lsync[1]; \
		*lsync = SHMEM_SYNC_VALUE; \
	} \
	int pe = my_pe + pe_step; \
	if (my_pe != pe_end) { \
		volatile long* rsync = (volatile long*)shmem_ptr((void*)lsync, pe); \
		rsync[1] = my_offset + nelems; /* XXX casting size_t to long */ \
		__shmem_set_lock(rsync); /* XXX this method isn't ideal */ \
	} \
	dest = (T*)dest + my_offset; \
	shmemx_memcpy##N(dest, source, nelems); \
	for (; pe <= pe_end; pe += pe_step) \
		shmemx_memcpy##N(shmem_ptr(dest, pe), source, nelems); \
	for (pe = PE_start; pe < my_pe; pe += pe_step) \
		shmemx_memcpy##N(shmem_ptr(dest, pe), source, nelems); \
	shmem_sync(PE_start, logPE_stride, PE_size, pSync); \
}

#endif
