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

#ifndef _def_shmem_x_to_all_h
#define _def_shmem_x_to_all_h

#define SUM_OP  += pWrk[j]
#define PROD_OP *= pWrk[j]
#define AND_OP  &= pWrk[j]
#define OR_OP   |= pWrk[j]
#define XOR_OP  ^= pWrk[j]
#define MAX_OP  = (dest[i+j]<pWrk[j])?pWrk[j]:dest[i+j]
#define MIN_OP  = (dest[i+j]<pWrk[j])?dest[i+j]:pWrk[j]

#define SHMEM_X_TO_ALL(N,T,SZ,OP) \
SHMEM_SCOPE void \
shmem_##N##_to_all(T *dest, const T *source, int nreduce, int PE_start, int logPE_stride, int PE_size, T *pWrk, long *pSync) \
{ \
	int PE_size_stride = PE_size << logPE_stride; \
	int PE_step = 1 << logPE_stride; \
	int PE_end = PE_size_stride + PE_start; \
	int nreduced2p1 = (nreduce >> 1) + 1; \
	int nwrk = (nreduced2p1 > SHMEM_REDUCE_MIN_WRKDATA_SIZE) ? nreduced2p1 : SHMEM_REDUCE_MIN_WRKDATA_SIZE; \
	volatile long* vSync = (volatile long*)(pSync + SHMEM_REDUCE_SYNC_SIZE - 2); \
	int i, j, r; \
	shmemx_memcpy##SZ(dest, source, nreduce); \
	vSync[0] = SHMEM_SYNC_VALUE; /* XXX */ \
	vSync[1] = SHMEM_SYNC_VALUE; /* XXX */ \
	shmem_sync(PE_start, logPE_stride, PE_size, pSync); /* XXX */ \
	int start = 1 << logPE_stride; \
	int end = PE_size_stride; \
	int step = start; \
	int to = __shmem.my_pe; \
	T* data = dest; \
	if (PE_size & (PE_size - 1)) { /* Use ring algorithm for non-powers of 2 */ \
		start = 1; \
		end = PE_size; \
		step = PE_step; \
		data = (T*)source; \
	} \
	for (r = start; r < end;) { \
		to += step; \
		if (to >= PE_end) to -= PE_size_stride; \
		uintptr_t remote_ptr = (uintptr_t)shmem_ptr(0, to); \
		T* remote_work = (T*)(remote_ptr | (uintptr_t)pWrk); \
		long* remote_locks = (long*)(remote_ptr | (uintptr_t)vSync); \
		for (i = 0; i < nreduce; i += nwrk) { \
			int nrem = nreduce - i; \
			nrem = (nrem < nwrk) ? nrem : nwrk; \
			__shmem_set_lock(remote_locks); \
			shmemx_memcpy##SZ(remote_work, data + i, nrem); \
			remote_locks[1] = 1; /* XXX assumes SHMEM_SYNC_VALUE != 1 */\
			while (vSync[1] == SHMEM_SYNC_VALUE); \
			for (j = 0; j < nrem; j++) dest[i+j] OP; \
			vSync[1] = SHMEM_SYNC_VALUE; \
			vSync[0] = SHMEM_SYNC_VALUE; \
		} \
		if ((PE_size & (PE_size - 1))) { \
			r++; \
		} else { \
			r <<= 1; \
			step <<= 1; \
		} \
	} \
}

#endif
