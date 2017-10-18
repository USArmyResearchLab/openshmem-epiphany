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

#define SHMEM_FCOLLECT_N(N,S) \
SHMEM_SCOPE void \
shmem_fcollect##N (void* dest, const void* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long* pSync) \
{ \
	const int step = 1 << logPE_stride; \
	const int pe_shift = PE_size << logPE_stride; \
	const int pe_end = PE_start + pe_shift; \
	const int my_pe = __shmem.my_pe; \
	const int nbytes = nelems << S; /* << 2 = 4 bytes, << 3 = 8 bytes */ \
	const ptrdiff_t offset = nbytes * ((my_pe - PE_start) >> logPE_stride); \
	const void* target = dest + offset; \
	int pe = my_pe; \
	do { \
		shmemx_memcpy##N(shmem_ptr(target,pe), source, nelems); \
		pe += step; \
	} while (pe < pe_end); \
	pe -= pe_shift; \
	while (pe < my_pe) { \
		shmemx_memcpy##N(shmem_ptr(target,pe), source, nelems); \
		pe += step; \
	} \
	shmem_barrier(PE_start, logPE_stride, PE_size, pSync); \
}

#endif
