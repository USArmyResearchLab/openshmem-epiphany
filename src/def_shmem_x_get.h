/*
 * Copyright (c) 2016-2017 U.S. Army Research laboratory. All rights reserved.
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

#ifndef _def_shmem_x_get_h
#define _def_shmem_x_get_h
#include "shmemx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SHMEM_X_GET_NBI(N,T,S) \
SHMEM_SCOPE void \
shmem_##N##_nbi (T *dest, const T *source, size_t nelems, int pe) \
{ shmemx_memcpy_nbi((void*)dest, shmem_ptr(source,pe), nelems << S); }

#ifdef SHMEM_USE_IPI_GET

typedef struct
{
	volatile long   lock;
	volatile void (*pmemcpy)(void*,const void*,size_t);
	volatile void*  source;
	volatile void*  dest;
	volatile size_t nelems;
	volatile int*   pcomplete;
	volatile int    complete;
} shmem_ipi_args_t;

extern shmem_ipi_args_t shmem_ipi_args;

#define SHMEM_X_GET(N,T,S) \
SHMEM_SCOPE void \
shmem_##N (T *dest, const T *source, size_t nelems, int pe) \
{ \
	if (nelems < (1024/S)) { \
		shmemx_memcpy##S((void*)dest, shmem_ptr(source,pe), nelems); \
	} \
	else { \
		volatile unsigned int* remote_ilatst = shmem_ptr((void*)0xf042c, pe); \
		shmem_ipi_args_t* remote_args = (shmem_ipi_args_t*)shmem_ptr((void*)&shmem_ipi_args, pe); \
		__shmem_set_lock(&(remote_args->lock)); /* spin until prior transfers complete */ \
		remote_args->pmemcpy = (volatile void (*)(void*,const void*,size_t))shmemx_memcpy##S; /* setting parameters */ \
		remote_args->source = (volatile void*)source; \
		remote_args->dest = dest; \
		remote_args->nelems = nelems; \
		remote_args->pcomplete = (void*)((__shmem.coreid << 20) | (unsigned int) &(shmem_ipi_args.complete)); \
		*remote_ilatst = 0x0200; /* cause remote user interrupt */ \
		while(!(shmem_ipi_args.complete)); /* spin until remote core signals it's done */ \
		shmem_ipi_args.complete = 0; /* reset interrupt completion */ \
	} \
}

#else

#define SHMEM_X_GET(N,T,S) \
SHMEM_SCOPE void \
shmem_##N (T *dest, const T *source, size_t nelems, int pe) \
{ shmemx_memcpy##S((void*)dest, shmem_ptr((void*)source,pe), nelems); }

#endif

#ifdef __cplusplus
}
#endif

#endif
