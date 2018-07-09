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
	int PE = __shmem.my_pe; \
	int PE_step = 0x1 << logPE_stride; \
	int PE_end = PE_start + PE_step * (PE_size - 1); \
	int my_offset; \
	/* The SHMEM_COLLECT_SYNC_SIZE is padded with one extra value for \
	 * syncronization and is not used in the shmem_barrier */ \
	volatile long* vSync = pSync + SHMEM_COLLECT_SYNC_SIZE - 2; \
	long* neighbor = (long*)shmem_ptr((void*)vSync, PE + PE_step); \
	if (PE == PE_start) { \
		my_offset = 0; \
		neighbor[0] = nelems; /* XXX casting size_t to long */ \
		neighbor[1] = 1; /* XXX must not be SHMEM_SYNC_VALUE */ \
	} else { \
		/* spin until neighbor sets offset */ \
		while (!vSync[1]); \
		my_offset = vSync[0]; \
		if (PE != PE_end) { \
			neighbor[0] = my_offset + nelems; \
			neighbor[1] = 1; \
		} \
	} \
	vSync[0] = SHMEM_SYNC_VALUE; \
	vSync[1] = SHMEM_SYNC_VALUE; \
	int i; \
	for (i = PE_start; i <= PE_end; i += PE_step) { \
		T* dst = (T*)dest + my_offset; \
		if (PE != i) dst = (T*)shmem_ptr((void*)dst, i); \
		shmemx_memcpy##N(dst, source, nelems); \
	} \
	shmem_barrier(PE_start, logPE_stride, PE_size, pSync); \
}

#endif

