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

#ifndef _def_shmem_alltoall_x_h
#define _def_shmem_alltoall_x_h

#define SHMEM_ALLTOALL_X(N,T) \
void \
shmem_alltoall##N(void* dest, const void* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync) \
{ \
	int i, j; \
	int dst_offset = ((__shmem.my_pe - PE_start) >> logPE_stride)*nelems; \
	int PE_size_stride = PE_size << logPE_stride; \
	int step = 1 << logPE_stride; \
	int PE_end = PE_size_stride + PE_start; \
	T* psrc = (T*)source; \
	T* pdsto = (T*)dest + dst_offset; \
	shmemx_memcpy((void*)pdsto, (void*)psrc, nelems * (N >> 3)); \
	for (j = 1; j < PE_size; j++) { \
		int PE_to = __shmem.my_pe + j*step; \
		if (PE_to >= PE_end) PE_to -= PE_size_stride; \
		T* pdst = (T*)shmem_ptr(pdsto, PE_to); \
		shmemx_memcpy((void*)pdst, (void*)psrc, nelems * (N >> 3)); \
	} \
	shmem_barrier(PE_start, logPE_stride, PE_size, pSync); \
}

#endif

