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

#ifndef _internal_h
#define _internal_h

#if defined(__coprthr_device__) // Using COPRTHR

#define SHMEM_LOW_PRIORITY __dynamic_call
#define shmemx_brk(ptr) coprthr_tls_brk(ptr)
#define shmemx_sbrk(size) coprthr_tls_sbrk(size)

#else // Using eSDK

#include "e_coreid.h"
#include "shmem_mman.h"
#define SHMEM_LOW_PRIORITY __attribute__((section(".shared_dram")))
#define shmemx_brk(ptr) __shmemx_brk(ptr)
#define shmemx_sbrk(size) __shmemx_sbrk(size)

#endif

// Values specific to Epiphany-III architecture
#define SHMEM_MAX_PES      16
#define SHMEM_MAX_PES_LOG2 4
#define SHMEM_ROW_SHIFT    2
#define SHMEM_ROW_MASK     0x3
#if defined(SHMEM_USE_HEAP_START)
#define SHMEM_HEAP_START   (SHMEM_USE_HEAP_START)
#else
#define SHMEM_HEAP_START   0x2000
#endif
#define SHMEM_INLINE       inline __attribute__((__always_inline__))

#if defined(SHMEM_USE_HEADER_ONLY)
#define SHMEM_SCOPE static
#else
#define SHMEM_SCOPE
#endif

#if __cplusplus
extern "C" {
#endif

SHMEM_SCOPE long* __shmem_lock_ptr (const long* p);
SHMEM_SCOPE void __shmem_clear_lock (volatile long* x);
SHMEM_SCOPE void __shmem_set_lock (volatile long* x);
SHMEM_SCOPE void __shmem_set_lock_self (long* x);
SHMEM_SCOPE int __shmem_test_lock (volatile long* x);

#if __cplusplus
}
#endif

#endif
