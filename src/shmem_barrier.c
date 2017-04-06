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

#include "internals.h"
#include "shmem.h"

#ifdef __cplusplus
extern "C" {
#endif

SHMEM_SCOPE void SHMEM_INLINE
__shmem_barrier_lte2(int PE_start, int logPE_stride, int PE_size, long *pSync)
{ /* Routine for PE_size <= 2. Looping over shmem_barrier() for npes = 2 may
	* not work correctly.  Solution requires using testset because only
	* sychronization stage may not be reset before subsequent call */
	if (PE_size == 1) return;
	int PE_step = 0x1 << logPE_stride;
	if (__shmem.my_pe != PE_start) PE_step *= -1;
	int to = __shmem.my_pe + PE_step;
	volatile long* lock = (volatile long*)pSync;
	__shmem_set_lock((long*)shmem_ptr((void*)lock, to));
	while (*lock == SHMEM_SYNC_VALUE);
	*lock = 0;
}

SHMEM_SCOPE void
shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync)
{
	if (PE_size < 3) return __shmem_barrier_lte2(PE_start, logPE_stride, PE_size, pSync);
	int PE_size_stride = PE_size << logPE_stride;
	int PE_end = PE_size_stride + PE_start;

	int c, r;
	for (c = 0, r = (1 << logPE_stride); r < PE_size_stride; c++, r <<= 1)
	{
		int to = __shmem.my_pe + r;
		if (to >= PE_end) to -= PE_size_stride;
		volatile long* lock = (volatile long*)(pSync + c);
		long * remote_lock = (long*)shmem_ptr((void*)lock, to);
		*remote_lock = 1;
		while (*lock == SHMEM_SYNC_VALUE);
		*lock = SHMEM_SYNC_VALUE;
	}
}

#ifdef __cplusplus
}
#endif
