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

#ifndef _def_shmem_x_to_all_h
#define _def_shmem_x_to_all_h

//#define HOT  __attribute__((hot))
#define HOT 

#define SUM_OP  += pWrk[j]
#define PROD_OP *= pWrk[j]
#define AND_OP  &= pWrk[j]
#define OR_OP   |= pWrk[j]
#define XOR_OP  ^= pWrk[j]
#define MAX_OP  = (dest[i+j]>pWrk[j])?dest[i+j]:pWrk[j]
#define MIN_OP  = (dest[i+j]<pWrk[j])?dest[i+j]:pWrk[j]

#define SHMEM_X_TO_ALL(N,T,OP) \
void HOT \
shmem_##N##_to_all(T *dest, const T *source, int nreduce, int PE_start, int logPE_stride, int PE_size, T *pWrk, long *pSync) \
{ \
	int PE_size_stride = PE_size << logPE_stride; \
	int PE_step = 0x1 << logPE_stride; \
	int PE_end = PE_size_stride + PE_start; \
	int nreduced2p1 = (nreduce >> 1) + 1; \
	int nwrk = (nreduced2p1 > SHMEM_REDUCE_MIN_WRKDATA_SIZE) ? nreduced2p1 : SHMEM_REDUCE_MIN_WRKDATA_SIZE; \
	volatile long* vSync = (volatile long*)pSync; \
	for (int i = 0; i < nreduce; i++) { \
		dest[i] = source[i]; \
	} \
	if (PE_size & (PE_size - 1)) { /* Use ring algorithm for non-powers of 2 */ \
		int to = __shmem.my_pe; \
		for (int r = 1; r < PE_size; r++) { \
			to += PE_step; \
			if (to >= PE_end) to -= PE_size_stride; \
			T* remote_work = (T*)shmem_ptr((void*)pWrk, to); \
			long* remote_lock0 = (long*)shmem_ptr((void*)vSync, to); \
			long* remote_lock1 = (long*)shmem_ptr((void*)vSync+1, to); \
			for (int i = 0; i < nreduce; i += nwrk) { \
				int nrem = nreduce - i; \
				nrem = (nrem > nwrk) ? nwrk : nrem; \
				__shmem_set_lock(remote_lock0); \
				for (int j = 0; j < nrem; j++) { \
					remote_work[j] = source[i+j]; \
				} \
				__shmem_set_lock(remote_lock1); \
				while (!vSync[1]); \
				for (int j = 0; j < nrem; j++) { \
					dest[i+j] OP; \
				} \
				vSync[0] = SHMEM_SYNC_VALUE; \
				vSync[1] = SHMEM_SYNC_VALUE; \
			} \
		} \
	} else { /* Use dissemination algorithm for powers of 2 */ \
		for (int r = (1 << logPE_stride); r < PE_size_stride; r <<= 1) { \
			int to = __shmem.my_pe + r; \
			if (to >= PE_end) to -= PE_size_stride; \
			T* remote_work = (T*)shmem_ptr((void*)pWrk, to); \
			long* remote_lock0 = (long*)shmem_ptr((void*)vSync, to); \
			long* remote_lock1 = (long*)shmem_ptr((void*)(vSync+1), to); \
			for (int i = 0; i < nreduce; i += nwrk) { \
				int nrem = nreduce - i; \
				nrem = (nrem > nwrk) ? nwrk : nrem; \
				__shmem_set_lock(remote_lock0); \
				for (int j = 0; j < nrem; j++) { \
					remote_work[j] = dest[i+j]; \
				} \
				__shmem_set_lock(remote_lock1); \
				while (!vSync[1]); \
				for (int j = 0; j < nrem; j++) { \
					dest[i+j] OP; \
				} \
				vSync[0] = SHMEM_SYNC_VALUE; \
				vSync[1] = SHMEM_SYNC_VALUE; \
			} \
		} \
	} \
}

#endif

