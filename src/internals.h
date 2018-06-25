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

#ifndef _internal_h
#define _internal_h

#include <stdint.h>

// SHMEM_MAX_PES_LOG2 should be an even number for alignment
#if defined(__coprthr_device__) // Using COPRTHR

#include "coprthr2.h"
#include "ecore.h"

#define SHMEM_MAX_PES_LOG2 10
#define SHMEM_ROW_SHIFT    (_config.corenum_row_shift)
#define SHMEM_ROW_MASK     (_config.corenum_col_mask)
#define SHMEM_BASE_COREID  (_config.core_addr_base >> 20)
#define SHMEM_LOW_PRIORITY __dynamic_call
#define shmemx_brk(ptr)    coprthr_tls_brk(ptr)
#define shmemx_sbrk(size)  coprthr_tls_sbrk(size)

#else // Using eSDK

#include "e_coreid.h"
#include "shmem_mman.h"

// Values specific to Epiphany-III architecture
#define SHMEM_MAX_PES_LOG2 4
#define SHMEM_ROW_SHIFT    2
#define SHMEM_ROW_MASK     0x3
#define SHMEM_BASE_COREID  2056
#define SHMEM_LOW_PRIORITY __attribute__((section(".shared_dram")))
#define shmemx_brk(ptr)    __shmemx_brk(ptr)
#define shmemx_sbrk(size)  __shmemx_sbrk(size)

#endif

#if defined(SHMEM_USE_HEAP_START)
#define SHMEM_HEAP_START   (SHMEM_USE_HEAP_START)
#else
#define SHMEM_HEAP_START   0x0
#endif
#define SHMEM_INLINE       inline __attribute__((__always_inline__))

#if defined(SHMEM_USE_HEADER_ONLY)
#define SHMEM_SCOPE static
#else
#define SHMEM_SCOPE
#endif

#define __PAD                     ( 2 ) // extra synchronization scratchpad
#define __F2C_SCALE               ( sizeof (long) / sizeof (int) )
#define __BCAST_SYNC_SIZE         ( SHMEM_MAX_PES_LOG2 / __F2C_SCALE )
#define __BARRIER_SYNC_SIZE       ( SHMEM_MAX_PES_LOG2 / __F2C_SCALE )
#define __REDUCE_SYNC_SIZE        ( SHMEM_MAX_PES_LOG2 / __F2C_SCALE + __PAD )
#define __REDUCE_MIN_WRKDATA_SIZE ( 16 / __F2C_SCALE )
#define __SYNC_VALUE              ( 0 )
#define __COLLECT_SYNC_SIZE       ( SHMEM_MAX_PES_LOG2 / __F2C_SCALE + __PAD )
#define __ALLTOALL_SYNC_SIZE      ( SHMEM_MAX_PES_LOG2 / __F2C_SCALE )
#define __ALLTOALLS_SYNC_SIZE     ( SHMEM_MAX_PES_LOG2 / __F2C_SCALE )
#define __SYNC_SIZE               ( __REDUCE_SYNC_SIZE ) // max. of SYNC_SIZEs

#define  __THREAD_SINGLE     0
#define  __THREAD_FUNNELED   1
#define  __THREAD_SERIALIZED 2
#define  __THREAD_MULTIPLE   3

#define  __CTX_DEFAULT    0
#define  __CTX_SERIALIZED 1
#define  __CTX_PRIVATE    2
#define  __CTX_NOSTORE    4

#if __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned config;
	unsigned inner_stride;
	unsigned count;
	unsigned outer_stride;
	void*    src_addr;
	void*    dst_addr;
} __attribute__((aligned(8))) shmem_dma_desc_t;

typedef struct {
	int my_pe;
	int n_pes;
	int n_pes_log2;
	unsigned int dma_start;
	unsigned int dma_used;
	unsigned int lock_high_bits;
	volatile long lock_atomic;
	volatile long lock_atomic_int;
	volatile long lock_atomic_long;
	volatile long lock_atomic_longlong;
	volatile long lock_atomic_uint;
	volatile long lock_atomic_ulong;
	volatile long lock_atomic_ulonglong;
	volatile long lock_atomic_int32;
	volatile long lock_atomic_int64;
	volatile long lock_atomic_uint32;
	volatile long lock_atomic_uint64;
	volatile long lock_atomic_size;
	volatile long lock_atomic_ptrdiff;
	volatile long lock_atomic_float;
	volatile long lock_atomic_double;
	volatile long lock_receive_finished;
	volatile unsigned char csrc0;
	volatile unsigned char csrc1;
	unsigned char volatile * volatile cdst0;
	unsigned char volatile * volatile cdst1;
	unsigned int coreid;
	intptr_t local_mem_base;
	intptr_t free_mem;
	volatile long barrier_sync[__BARRIER_SYNC_SIZE];
#ifndef SHMEM_USE_WAND_BARRIER
	long* barrier_psync[__BARRIER_SYNC_SIZE];
#endif
	shmem_dma_desc_t dma_desc;
} shmem_internals_t;

extern shmem_internals_t __shmem;

SHMEM_SCOPE long* __shmem_lock_ptr (const long* p);
SHMEM_SCOPE void __shmem_clear_lock (volatile long* x);
SHMEM_SCOPE void __shmem_set_lock (volatile long* x);
SHMEM_SCOPE int __shmem_test_lock (volatile long* x);

#if __cplusplus
}
#endif

#include "def_shmem_alltoall_x.h"
#include "def_shmem_alltoalls.h"
#include "def_shmem_broadcastx.h"
#include "def_shmem_collect_n.h"
#include "def_shmem_fcollect_n.h"
#include "def_shmem_x_atomic_add.h"
#include "def_shmem_x_atomic_and.h"
#include "def_shmem_x_atomic_compare_swap.h"
#include "def_shmem_x_atomic_fetch.h"
#include "def_shmem_x_atomic_fetch_add.h"
#include "def_shmem_x_atomic_fetch_and.h"
#include "def_shmem_x_atomic_fetch_inc.h"
#include "def_shmem_x_atomic_fetch_or.h"
#include "def_shmem_x_atomic_fetch_xor.h"
#include "def_shmem_x_atomic_inc.h"
#include "def_shmem_x_atomic_or.h"
#include "def_shmem_x_atomic_set.h"
#include "def_shmem_x_atomic_swap.h"
#include "def_shmem_x_atomic_xor.h"
#include "def_shmem_x_get.h"
#include "def_shmem_x_iget.h"
#include "def_shmem_x_iput.h"
#include "def_shmem_x_put.h"
#include "def_shmem_x_test.h"
#include "def_shmem_x_to_all.h"
#include "def_shmem_x_wait.h"
#include "def_shmem_x_wait_until.h"

#endif
