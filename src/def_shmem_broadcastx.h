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

#ifndef _def_shmem_broadcastx_h
#define _def_shmem_broadcastx_h
#include "shmemx.h"

#define SHMEM_BROADCASTX(N,T) \
void \
shmem_broadcast##N (void *dest, const void *source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long *pSync) \
{ \
	int PE = __shmem.my_pe; \
	int PE_root_stride = PE_root << logPE_stride; \
	T* psrc = (T*)dest; \
	int PEx = PE - PE_start - PE_root_stride; \
	int PE_size_stride = PE_size << logPE_stride; \
	if (PEx < 0) PEx += PE_size_stride; \
	if (PEx == 0) psrc = (T*)source; \
	int PE_end = PE_size_stride + PE_start; \
	int maskx = PE_size_stride - 1; \
	maskx |= (maskx >> 1); \
	maskx |= (maskx >> 2); /* up to 16 PEs */ \
	/*maskx |= (maskx >> 4);*/ /* up to 1024 PEs */ \
	/*maskx |= (maskx >> 8);*/ /* up to 65536 PEs */ \
	maskx += 1; /* the next largest power of 2 of the largest PE number */ \
	maskx >>= 1; \
	int mask2 = PEx & -PEx; /* least significant 1 bit */ \
	if (PEx==0) mask2 = maskx; \
	int mask1 = (mask2 << 1) - 1; \
	do { \
		if ((PEx & mask1) == 0) { \
			int PE_to = (PEx | mask2) + PE_start + PE_root_stride; \
			if (PE_to >= PE_end) PE_to -= PE_size_stride; \
			long* remote_lock_receive_finished = (long*)shmem_ptr((void*)&__shmem.lock_receive_finished, PE_to); \
			T* remote_dest = (T*)shmem_ptr((void*)dest, PE_to); \
			shmemx_memcpy((void*)remote_dest, (void*)psrc, nelems * (N >> 3)); \
			*remote_lock_receive_finished = 1; /* clear lock_receive_finished on receiving PE */ \
		} \
		else { \
			while (!__shmem.lock_receive_finished); \
			__shmem.lock_receive_finished = 0; \
		} \
		mask1 >>= 1; \
		mask2 >>= 1; \
	} while (mask1 >> logPE_stride); \
}

#endif

