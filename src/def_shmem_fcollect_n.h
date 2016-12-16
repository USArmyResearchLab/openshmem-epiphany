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

#ifndef _def_shmem_fcollect_n_h
#define _def_shmem_fcollect_n_h

#define SHMEM_FCOLLECT_N(N,T) \
void \
__shmem_fcollect##N##_0 (void *dest, const void *source, size_t nelems, int PE_start, int PE_size, long *pSync) \
{ \
	int PE = __shmem.my_pe; \
	int PE_size_stride = PE_size; \
	int step = 1; \
	int PE_end = PE_size_stride + PE_start; \
	int PEx = PE - PE_start; \
	T* psrc = (T*)source; \
	T* pdst = (T*)dest; \
	for (int i = 0; i < nelems; i++) { \
		pdst[PEx*nelems + i] = psrc[i]; \
	} \
	if (__builtin_expect(PE_size & (PE_size - 1),0)) { /* Use ring algorithm for non-powers of 2 */ \
		for (int j = 1; j < PE_size; j++) { \
			int PE_to = __shmem.my_pe + j*step; \
			if (PE_to >= PE_end) PE_to -= PE_size_stride; \
			int offset = (PE - PE_start) *nelems; \
			pdst = (T*)shmem_ptr((void*)dest, PE_to); \
			for (int i = 0; i < nelems; i++) { \
				pdst[offset + i] = psrc[i]; \
			} \
		} \
		shmem_barrier(PE_start, 0, PE_size, pSync); \
	} else { /* recursive doubling (butterfly) algorithm broadcasts increasingly more */ \
		int Nx = nelems * PE_size; \
		int x = PE_size-1; \
		x |= (x >> 1); \
		x |= (x >> 2); /* up to 16 PEs */ \
		/*x |= (x >> 4);*/ /* up to 1024 PEs */ \
		/*x |= (x >> 8);*/ /* up to 65536 PEs */ \
		x += 1; /* the next largest power of 2 of the largest PE number */ \
		for (int c = 0, r = (PE_size_stride >> 1); r >= 1; c++, x >>= 1, r>>=1) { \
			int PE_to = __shmem.my_pe + r; \
			if (PE_to >= PE_end) PE_to -= PE_size_stride; \
			volatile long* lock = (volatile long*)(pSync + c); \
			long* remote_lock = (long*)shmem_ptr((void*)lock, PE_to); \
			T* remote_dest = (T*)shmem_ptr((void*)dest, PE_to); \
			for (int j = 0; j < (1<<c); j++) { \
				int offset = (PEx + x*j )*nelems; \
				if (offset >= Nx) offset -= Nx; \
				for (int i = 0; i < nelems; i++) { \
					remote_dest[offset + i] =  pdst[offset + i]; \
				} \
			} \
			*remote_lock = 1; \
			while (*lock == SHMEM_SYNC_VALUE); \
			*lock = SHMEM_SYNC_VALUE; \
		} \
	} \
} \
void \
shmem_fcollect##N (void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync) \
{ \
	if (logPE_stride==0) return \
		__shmem_fcollect##N##_0(dest,source,nelems,PE_start,PE_size,pSync); \
	int PE = __shmem.my_pe; \
	int PE_size_stride = PE_size << logPE_stride; \
	int step = 1 << logPE_stride; \
	int PE_end = PE_size_stride + PE_start; \
	int PEx = (PE - PE_start) >> logPE_stride; \
	T* psrc = (T*)source; \
	T* pdst = (T*)dest; \
	for (int i = 0; i < nelems; i++) { \
		pdst[PEx*nelems + i] = psrc[i]; \
	} \
	if (__builtin_expect(PE_size & (PE_size - 1),0)) { /* Use ring algorithm for non-powers of 2 */ \
		for (int j = 1; j < PE_size; j++) { \
			int PE_to = __shmem.my_pe + j*step; \
			if (PE_to >= PE_end) PE_to -= PE_size_stride; \
			int offset = ((PE - PE_start) >> logPE_stride)*nelems; \
			pdst = (T*)shmem_ptr((void*)dest, PE_to); \
			for (int i = 0; i < nelems; i++) { \
				pdst[offset + i] = psrc[i]; \
			} \
		} \
		shmem_barrier(PE_start, logPE_stride, PE_size, pSync); \
	} else { /* recursive doubling (butterfly) algorithm broadcasts increasingly more */ \
		int Nx = nelems * PE_size; \
		int x = PE_size-1; \
		x |= (x >> 1); \
		x |= (x >> 2); /* up to 16 PEs */ \
		/*x |= (x >> 4);*/ /* up to 1024 PEs */ \
		/*x |= (x >> 8);*/ /* up to 65536 PEs */ \
		x += 1; /* the next largest power of 2 of the largest PE number */ \
		for (int c = 0, r = (PE_size_stride >> 1); r >= (1 << logPE_stride); c++, x >>= 1, r>>=1) { \
			int PE_to = __shmem.my_pe + r; \
			if (PE_to >= PE_end) PE_to -= PE_size_stride; \
			volatile long* lock = (volatile long*)(pSync + c); \
			long* remote_lock = (long*)shmem_ptr((void*)lock, PE_to); \
			T* remote_dest = (T*)shmem_ptr((void*)dest, PE_to); \
			for (int j = 0; j < (1<<c); j++) { \
				int offset = (PEx + x*j )*nelems; \
				if (offset >= Nx) offset -= Nx; \
				for (int i = 0; i < nelems; i++) { \
					remote_dest[offset + i] =  pdst[offset + i]; \
				} \
			} \
			*remote_lock = 1; \
			while (*lock == SHMEM_SYNC_VALUE); \
			*lock = SHMEM_SYNC_VALUE; \
		} \
	} \
}

#endif

