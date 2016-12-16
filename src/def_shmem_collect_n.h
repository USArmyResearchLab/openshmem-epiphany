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

#ifndef _def_shmem_collect_n_h
#define _def_shmem_collect_n_h

/* XXX I can't imagine a better algorithm than linear where each PE passes
 * their chunk to all other PEs. This is a horrible routine with variable
 * nelems and you should avoid using it */
#define SHMEM_COLLECT_N(N,T) \
static void \
__shmem_collect##N##_0 (void *dest, const void *source, size_t nelems, int PE_start, int PE_size, long *pSync) \
{ \
	int PE = __shmem.my_pe; \
	int PE_step = 1; \
	int orig_PE_end = PE_start + (PE_size - 1); \
	int PE_end = PE_start + (PE_size - 1); \
	int my_offset = 0; \
	/* We know the value here is 0 and we can use it and reset it before \
    * it's needed in the barrier */ \
	volatile long *offset = pSync + SHMEM_COLLECT_SYNC_SIZE - 1; \
	int* neighbor_offset = (int*)shmem_ptr((void*)offset, PE + PE_step); \
	if (__builtin_expect((PE == PE_start),0)) { \
		*neighbor_offset = my_offset + nelems; \
	} else { \
		/* spin until neighbor sets offset */ \
		while (!(my_offset = *offset)); \
		*offset = SHMEM_SYNC_VALUE; \
		if (__builtin_expect((PE != PE_end),1)) \
			*neighbor_offset = my_offset + nelems; \
	} \
	for (int i = PE_start; i <= PE_end; i += PE_step) { \
		T* dst = (T*)shmem_ptr((void*)dest, i) + my_offset; \
		for (int j = 0; j < nelems; j++) dst[j] = ((T*)source)[j]; \
	} \
	shmem_barrier(PE_start, 0, PE_size, pSync); \
} \
void \
shmem_collect##N (void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync) \
{ \
	if (logPE_stride==0) return \
		__shmem_collect##N##_0(dest,source,nelems,PE_start,PE_size,pSync); \
	int PE = __shmem.my_pe; \
	int PE_step = 0x1 << logPE_stride; \
	int PE_end = PE_start + PE_step * (PE_size - 1); \
	int my_offset = 0; \
	/* We know the value here is 0 and we can use it and reset it before \
    * it's needed in the barrier */ \
	volatile long *offset = pSync + SHMEM_COLLECT_SYNC_SIZE - 1; \
	int* neighbor_offset = (int*)shmem_ptr((void*)offset, PE + PE_step); \
	if (__builtin_expect((PE == PE_start),0)) { \
		*neighbor_offset = my_offset + nelems; \
	} else { \
		/* spin until neighbor sets offset */ \
		while (!(my_offset = *offset)); \
		*offset = SHMEM_SYNC_VALUE; \
		if (__builtin_expect((PE != PE_end),1)) \
			*neighbor_offset = my_offset + nelems; \
	} \
	for (int i = PE_start; i <= PE_end; i += PE_step) { \
		T* dst = (T*)shmem_ptr((void*)dest, i) + my_offset; \
		for (int j = 0; j < nelems; j++) dst[j] = ((T*)source)[j]; \
	} \
	shmem_barrier(PE_start, logPE_stride, PE_size, pSync); \
}

#endif

