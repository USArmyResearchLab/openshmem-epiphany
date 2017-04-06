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

#ifndef _shmem_h
#define _shmem_h

#include <stdint.h>
#include <stdlib.h>
#include <complex.h>
#include <sys/types.h>
#include "internals.h"

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

#define SHMEM_MAJOR_VERSION            1
#define SHMEM_MINOR_VERSION            3
#define SHMEM_MAX_NAME_LEN             64
#define SHMEM_VENDOR_STRING            "ARL OpenSHMEM for Epiphany, version 1.3"
#define SHMEM_INTERNAL_F2C_SCALE       ( sizeof (long) / sizeof (int) )
#define SHMEM_BCAST_SYNC_SIZE          ( SHMEM_MAX_PES_LOG2 / SHMEM_INTERNAL_F2C_SCALE )
#define SHMEM_BARRIER_SYNC_SIZE        ( SHMEM_MAX_PES_LOG2 / SHMEM_INTERNAL_F2C_SCALE )
#define SHMEM_REDUCE_SYNC_SIZE         ( 2*SHMEM_MAX_PES_LOG2 / SHMEM_INTERNAL_F2C_SCALE )
#define SHMEM_REDUCE_MIN_WRKDATA_SIZE  ( 16 / SHMEM_INTERNAL_F2C_SCALE )
#define SHMEM_SYNC_VALUE               ( 0 )
#define SHMEM_COLLECT_SYNC_SIZE        ( SHMEM_MAX_PES_LOG2 / SHMEM_INTERNAL_F2C_SCALE )
#define SHMEM_ALLTOALL_SYNC_SIZE       ( SHMEM_MAX_PES_LOG2 / SHMEM_INTERNAL_F2C_SCALE )
#define SHMEM_ALLTOALLS_SYNC_SIZE      ( SHMEM_MAX_PES_LOG2 / SHMEM_INTERNAL_F2C_SCALE )
#define _SHMEM_MAJOR_VERSION           SHMEM_MAJOR_VERSION
#define _SHMEM_MINOR_VERSION           SHMEM_MINOR_VERSION
#define _SHMEM_MAX_NAME_LEN            SHMEM_MAX_NAME_LEN
#define _SHMEM_VENDOR_STRING           SHMEM_VENDOR_STRING
#define _SHMEM_INTERNAL_F2C_SCALE      SHMEM_INTERNAL_F2C_SCALE
#define _SHMEM_BCAST_SYNC_SIZE         SHMEM_BCAST_SYNC_SIZE
#define _SHMEM_BARRIER_SYNC_SIZE       SHMEM_BARRIER_SYNC_SIZE
#define _SHMEM_REDUCE_SYNC_SIZE        SHMEM_REDUCE_SYNC_SIZE
#define _SHMEM_REDUCE_MIN_WRKDATA_SIZE SHMEM_REDUCE_MIN_WRKDATA_SIZE
#define _SHMEM_SYNC_VALUE              SHMEM_SYNC_VALUE
#define _SHMEM_COLLECT_SYNC_SIZE       SHMEM_COLLECT_SYNC_SIZE
#define _my_pe(...)                    shmem_my_pe(__VA_ARGS__) 
#define _num_pes(...)                  shmem_n_pes(__VA_ARGS__)
#define shmalloc(...)                  shmem_malloc(__VA_ARGS__)
#define shfree(...)                    shmem_free(__VA_ARGS__)
#define shrealloc(...)                 shmem_realloc(__VA_ARGS__)
#define shmemalign(...)                shmem_align(__VA_ARGS__)
#define start_pes(...)                 shmem_init()

#ifdef __cplusplus
extern "C" {
#endif

enum shmem_cmp_constants
{
	SHMEM_CMP_EQ = 0,
	SHMEM_CMP_NE,
	SHMEM_CMP_GT,
	SHMEM_CMP_LE,
	SHMEM_CMP_LT,
	SHMEM_CMP_GE
};

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
	volatile long barrier_sync[SHMEM_BARRIER_SYNC_SIZE];
#ifndef SHMEM_USE_WAND_BARRIER
	long* barrier_psync[SHMEM_BARRIER_SYNC_SIZE];
#endif
	shmem_dma_desc_t dma_desc;
} shmem_internals_t;

extern shmem_internals_t __shmem;

SHMEM_SCOPE void* shmem_ptr(const void* dest, int pe);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_malloc(size_t size);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_align(size_t alignment, size_t size);
SHMEM_SCOPE void shmem_free(const void *ptr);
SHMEM_SCOPE void* shmem_realloc(const void* ptr, size_t size);


#define shmem_clear_cache_inv(...)      do{}while(0)
#define shmem_set_cache_inv(...)        do{}while(0)
#define shmem_clear_cache_line_inv(...) do{}while(0)
#define shmem_set_cache_line_inv(...)   do{}while(0)
#define shmem_udcflush(...)             do{}while(0)
#define shmem_udcflush_line(...)        do{}while(0)


#define shmem_fence(...) shmem_quiet(__VA_ARGS__)

SHMEM_SCOPE void shmem_quiet(void);

SHMEM_SCOPE long* __shmem_lock_ptr (const long* p);
SHMEM_SCOPE void __shmem_clear_lock (volatile long* x);
SHMEM_SCOPE void __shmem_set_lock (volatile long* x);
SHMEM_SCOPE void __shmem_set_lock_self (long* x);
SHMEM_SCOPE int __shmem_test_lock (volatile long* x);
SHMEM_SCOPE void shmem_clear_lock (volatile long* lock);
SHMEM_SCOPE void shmem_set_lock (volatile long* lock);
SHMEM_SCOPE int shmem_test_lock (volatile long* lock);


#define DECL_SHMEM_X_FINC(N,T) \
SHMEM_SCOPE T \
shmem_##N##_finc (T *ptr, int pe);

DECL_SHMEM_X_FINC(int,int)
DECL_SHMEM_X_FINC(long,long)
DECL_SHMEM_X_FINC(longlong,long long)

#define shmem_finc(dest,pe) \
_Generic((dest), \
	int*:       shmem_int_finc, \
	long*:      shmem_long_finc, \
	long long*: shmem_longlong_finc \
)(dest,pe)


#define DECL_SHMEM_X_INC(N,T) \
SHMEM_SCOPE void \
shmem_##N##_inc (T* ptr, int pe);

DECL_SHMEM_X_INC(int,int)
DECL_SHMEM_X_INC(long,long)
DECL_SHMEM_X_INC(longlong,long long)

#define shmem_inc(dest,pe) \
_Generic((dest), \
	int*:       shmem_int_inc, \
	long*:      shmem_long_inc, \
	long long*: shmem_longlong_inc \
)(dest,pe)


#define DECL_SHMEM_X_FADD(N,T) \
SHMEM_SCOPE T \
shmem_##N##_fadd (T *ptr, T value, int pe);

DECL_SHMEM_X_FADD(int,int)
DECL_SHMEM_X_FADD(long,long)
DECL_SHMEM_X_FADD(longlong,long long)

#define shmem_fadd(dest,value,pe) \
_Generic((dest), \
	int*:       shmem_int_fadd, \
	long*:      shmem_long_fadd, \
	long long*: shmem_longlong_fadd \
)(dest,value,pe)


#define DECL_SHMEM_X_ADD(N,T) \
SHMEM_SCOPE void \
shmem_##N##_add (T* ptr, T value, int pe);

DECL_SHMEM_X_ADD(int,int)
DECL_SHMEM_X_ADD(long,long)
DECL_SHMEM_X_ADD(longlong,long long)

#define shmem_add(dest,value,pe) \
_Generic((dest), \
	int*:       shmem_int_add, \
	long*:      shmem_long_add, \
	long long*: shmem_longlong_add \
)(dest,value,pe)


#define DECL_SHMEM_X_CSWAP(N,T) \
SHMEM_SCOPE T \
shmem_##N##_cswap (T *ptr, T cond, T value, int pe);

DECL_SHMEM_X_CSWAP(int,int)
DECL_SHMEM_X_CSWAP(long,long)
DECL_SHMEM_X_CSWAP(longlong,long long)

#define shmem_cswap(dest,cond,value,pe) \
_Generic((dest), \
	int*:       shmem_int_cswap, \
	long*:      shmem_long_cswap, \
	long long*: shmem_longlong_cswap \
)(dest,cond,value,pe)


#define DECL_SHMEM_X_SWAP(N,T) \
SHMEM_SCOPE T \
shmem_##N##_swap (T *ptr, T value, int pe);

DECL_SHMEM_X_SWAP(int,int)
DECL_SHMEM_X_SWAP(float,float)
DECL_SHMEM_X_SWAP(long,long)
DECL_SHMEM_X_SWAP(double,double)
DECL_SHMEM_X_SWAP(longlong,long long)

#define shmem_swap(dest,value,pe) \
_Generic((dest), \
	int*:       shmem_int_swap, \
	float*:     shmem_float_swap, \
	long*:      shmem_long_swap, \
	double*:    shmem_double_swap, \
	long long*: shmem_longlong_swap \
)(dest,value,pe)

#define DECL_SHMEM_X_FETCH(N,T) \
SHMEM_SCOPE T \
shmem_##N##_fetch (const T *ptr, int pe);

DECL_SHMEM_X_FETCH(int,int)
DECL_SHMEM_X_FETCH(float,float)
DECL_SHMEM_X_FETCH(long,long)
DECL_SHMEM_X_FETCH(double,double)
DECL_SHMEM_X_FETCH(longlong,long long)

#define shmem_fetch(dest,pe) \
_Generic((dest), \
	int*:       shmem_int_fetch, \
	float*:     shmem_float_fetch, \
	long*:      shmem_long_fetch, \
	double*:    shmem_double_fetch, \
	long long*: shmem_longlong_fetch \
)(dest,pe)


#define DECL_SHMEM_X_SET(N,T) \
SHMEM_SCOPE void \
shmem_##N##_set (T* dest, T value, int pe);

DECL_SHMEM_X_SET(int,int)
DECL_SHMEM_X_SET(float,float)
DECL_SHMEM_X_SET(long,long)
DECL_SHMEM_X_SET(double,double)
DECL_SHMEM_X_SET(longlong,long long)

#define shmem_set(dest,value,pe) \
_Generic((dest), \
	int*:       shmem_int_set, \
	float*:     shmem_float_set, \
	long*:      shmem_long_set, \
	double*:    shmem_double_set, \
	long long*: shmem_longlong_set \
)(dest,value,pe)


#define DECL_SHMEM_X_WAIT(N,T) \
SHMEM_SCOPE void \
shmem_##N (volatile T *ivar, T cmp_value);

DECL_SHMEM_X_WAIT(int_wait,int)
DECL_SHMEM_X_WAIT(long_wait,long)
DECL_SHMEM_X_WAIT(longlong_wait,long long)
DECL_SHMEM_X_WAIT(short_wait,short)
DECL_SHMEM_X_WAIT(wait,long)

#define DECL_SHMEM_X_WAIT_UNTIL(N,T) \
SHMEM_SCOPE void \
shmem_##N (volatile T *ivar, int cmp, T cmp_value);

DECL_SHMEM_X_WAIT_UNTIL(int_wait_until,int)
DECL_SHMEM_X_WAIT_UNTIL(long_wait_until,long)
DECL_SHMEM_X_WAIT_UNTIL(longlong_wait_until,long long)
DECL_SHMEM_X_WAIT_UNTIL(short_wait_until,short)
DECL_SHMEM_X_WAIT_UNTIL(wait_until,long)


#ifdef SHMEM_USE_WAND_BARRIER

SHMEM_SCOPE void __attribute__((aligned(8))) __shmem_wand_isr(void);
SHMEM_SCOPE void SHMEM_INLINE __shmem_wand_barrier_init(void);
SHMEM_SCOPE void SHMEM_INLINE __shmem_wand_barrier(void);

#else

SHMEM_SCOPE void SHMEM_INLINE __shmem_dissemination_barrier_init(void);
SHMEM_SCOPE void SHMEM_INLINE __shmem_dissemination_barrier(void);

#endif

SHMEM_SCOPE void SHMEM_INLINE
__shmem_barrier_lte2(int PE_start, int logPE_stride, int PE_size, long *pSync);

SHMEM_SCOPE void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_barrier_all(void);


#define DECL_SHMEM_X_TO_ALL(N,T,OP) \
SHMEM_SCOPE void \
shmem_##N##_to_all(T *dest, const T *source, int nreduce, int PE_start, int logPE_stride, int PE_size, T *pWrk, long *pSync);

DECL_SHMEM_X_TO_ALL(complexd_sum,complex double,SUM_OP)
DECL_SHMEM_X_TO_ALL(complexf_sum,complex float,SUM_OP)
DECL_SHMEM_X_TO_ALL(float_sum,float,SUM_OP)
DECL_SHMEM_X_TO_ALL(double_sum,double,SUM_OP)
DECL_SHMEM_X_TO_ALL(longdouble_sum,long double,SUM_OP)
DECL_SHMEM_X_TO_ALL(short_sum,short,SUM_OP)
DECL_SHMEM_X_TO_ALL(int_sum,int,SUM_OP)
DECL_SHMEM_X_TO_ALL(long_sum,long,SUM_OP)
DECL_SHMEM_X_TO_ALL(longlong_sum,long long,SUM_OP)

DECL_SHMEM_X_TO_ALL(complexd_prod,complex double,PROD_OP)
DECL_SHMEM_X_TO_ALL(complexf_prod,complex float,PROD_OP)
DECL_SHMEM_X_TO_ALL(float_prod,float,PROD_OP)
DECL_SHMEM_X_TO_ALL(double_prod,double,PROD_OP)
DECL_SHMEM_X_TO_ALL(longdouble_prod,long double,PROD_OP)
DECL_SHMEM_X_TO_ALL(short_prod,short,PROD_OP)
DECL_SHMEM_X_TO_ALL(int_prod,int,PROD_OP)
DECL_SHMEM_X_TO_ALL(long_prod,long,PROD_OP)
DECL_SHMEM_X_TO_ALL(longlong_prod,long long,PROD_OP)

DECL_SHMEM_X_TO_ALL(short_and,short,AND_OP)
DECL_SHMEM_X_TO_ALL(int_and,int,AND_OP)
DECL_SHMEM_X_TO_ALL(long_and,long,AND_OP)
DECL_SHMEM_X_TO_ALL(longlong_and,long long,AND_OP)

DECL_SHMEM_X_TO_ALL(short_or,short,OR_OP)
DECL_SHMEM_X_TO_ALL(int_or,int,OR_OP)
DECL_SHMEM_X_TO_ALL(long_or,long,OR_OP)
DECL_SHMEM_X_TO_ALL(longlong_or,long long,OR_OP)

DECL_SHMEM_X_TO_ALL(short_xor,short,XOR_OP)
DECL_SHMEM_X_TO_ALL(int_xor,int,XOR_OP)
DECL_SHMEM_X_TO_ALL(long_xor,long,XOR_OP)
DECL_SHMEM_X_TO_ALL(longlong_xor,long long,XOR_OP)

DECL_SHMEM_X_TO_ALL(float_max,float,MAX_OP)
DECL_SHMEM_X_TO_ALL(double_max,double,MAX_OP)
DECL_SHMEM_X_TO_ALL(longdouble_max,long double,MAX_OP)
DECL_SHMEM_X_TO_ALL(short_max,short,MAX_OP)
DECL_SHMEM_X_TO_ALL(int_max,int,MAX_OP)
DECL_SHMEM_X_TO_ALL(long_max,long,MAX_OP)
DECL_SHMEM_X_TO_ALL(longlong_max,long long,MAX_OP)

DECL_SHMEM_X_TO_ALL(float_min,float,MIN_OP)
DECL_SHMEM_X_TO_ALL(double_min,double,MIN_OP)
DECL_SHMEM_X_TO_ALL(longdouble_min,long double,MIN_OP)
DECL_SHMEM_X_TO_ALL(short_min,short,MIN_OP)
DECL_SHMEM_X_TO_ALL(int_min,int,MIN_OP)
DECL_SHMEM_X_TO_ALL(long_min,long,MIN_OP)
DECL_SHMEM_X_TO_ALL(longlong_min,long long,MIN_OP)


#define DECL_SHMEM_BROADCASTX(N,T) \
SHMEM_SCOPE void \
shmem_broadcast##N (void *dest, const void *source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long *pSync); 

DECL_SHMEM_BROADCASTX(32,int)
DECL_SHMEM_BROADCASTX(64,long long)


#define DECL_SHMEM_FCOLLECT_N(N,T) \
SHMEM_SCOPE void \
shmem_fcollect##N (void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync); 

DECL_SHMEM_FCOLLECT_N(32,int)
DECL_SHMEM_FCOLLECT_N(64,long long)


#define DECL_SHMEM_COLLECT_N(N,T) \
SHMEM_SCOPE void \
shmem_collect##N (void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

DECL_SHMEM_COLLECT_N(32,int)
DECL_SHMEM_COLLECT_N(64,long long)


#define DECL_SHMEM_ALLTOALL_X(N,T) \
SHMEM_SCOPE void \
shmem_alltoall##N(void* dest, const void* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

DECL_SHMEM_ALLTOALL_X(32,int)
DECL_SHMEM_ALLTOALL_X(64,long long)


#define DECL_SHMEM_ALLTOALLS_X(N,T) \
SHMEM_SCOPE void \
shmem_alltoalls##N(void* dest, const void* source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

DECL_SHMEM_ALLTOALLS_X(32,int)
DECL_SHMEM_ALLTOALLS_X(64,long long)


#define DECL_SHMEM_X_PUT_NBI(N,T,S) \
SHMEM_SCOPE void \
shmem_##N##_nbi (T *dest, const T *src, size_t nelems, int pe);

DECL_SHMEM_X_PUT_NBI(char_put,char,0)
DECL_SHMEM_X_PUT_NBI(short_put,short,1)
DECL_SHMEM_X_PUT_NBI(int_put,int,2)
DECL_SHMEM_X_PUT_NBI(long_put,long,3)
DECL_SHMEM_X_PUT_NBI(longlong_put,long long,3)
DECL_SHMEM_X_PUT_NBI(longdouble_put,long double,4)
DECL_SHMEM_X_PUT_NBI(double_put,double,3)
DECL_SHMEM_X_PUT_NBI(float_put,float,2)
DECL_SHMEM_X_PUT_NBI(putmem,void,0)
DECL_SHMEM_X_PUT_NBI(put8,void,0)
DECL_SHMEM_X_PUT_NBI(put16,void,1)
DECL_SHMEM_X_PUT_NBI(put32,void,2)
DECL_SHMEM_X_PUT_NBI(put64,void,3)
DECL_SHMEM_X_PUT_NBI(put128,void,4)

#define shmem_put_nbi(dest,src,nelems,pe) \
_Generic((dest), \
	float*:       shmem_float_put_nbi, \
	double*:      shmem_double_put_nbi, \
	long double*: shmem_longdouble_put_nbi, \
	char*:        shmem_char_put_nbi, \
	short*:       shmem_short_put_nbi, \
	int*:         shmem_int_put_nbi, \
	long*:        shmem_long_put_nbi, \
	long long*:   shmem_longlong_put_nbi \
)(dest,src,nelems,pe)


#define DECL_SHMEM_X_GET_NBI(N,T,S) \
SHMEM_SCOPE void \
shmem_##N##_nbi (T *dest, const T *src, size_t nelems, int pe);

DECL_SHMEM_X_GET_NBI(char_get,char,0)
DECL_SHMEM_X_GET_NBI(short_get,short,1)
DECL_SHMEM_X_GET_NBI(int_get,int,2)
DECL_SHMEM_X_GET_NBI(long_get,long,3)
DECL_SHMEM_X_GET_NBI(longlong_get,long long,3)
DECL_SHMEM_X_GET_NBI(longdouble_get,long double,4)
DECL_SHMEM_X_GET_NBI(double_get,double,3)
DECL_SHMEM_X_GET_NBI(float_get,float,2)
DECL_SHMEM_X_GET_NBI(getmem,void,0)
DECL_SHMEM_X_GET_NBI(get8,void,0)
DECL_SHMEM_X_GET_NBI(get16,void,1)
DECL_SHMEM_X_GET_NBI(get32,void,2)
DECL_SHMEM_X_GET_NBI(get64,void,3)
DECL_SHMEM_X_GET_NBI(get128,void,4)

#define shmem_get_nbi(dest,src,nelems,pe) \
_Generic((dest), \
	float*:       shmem_float_get_nbi, \
	double*:      shmem_double_get_nbi, \
	long double*: shmem_longdouble_get_nbi, \
	char*:        shmem_char_get_nbi, \
	short*:       shmem_short_get_nbi, \
	int*:         shmem_int_get_nbi, \
	long*:        shmem_long_get_nbi, \
	long long*:   shmem_longlong_get_nbi \
)(dest,src,nelems,pe)


#define DECL_SHMEM_X_PUT(N,T,S) \
SHMEM_SCOPE void \
shmem_##N (T *dest, const T *src, size_t nelems, int pe);

DECL_SHMEM_X_PUT(char_put,char,0)
DECL_SHMEM_X_PUT(short_put,short,1)
DECL_SHMEM_X_PUT(int_put,int,2)
DECL_SHMEM_X_PUT(long_put,long,3)
DECL_SHMEM_X_PUT(longlong_put,long long,3)
DECL_SHMEM_X_PUT(longdouble_put,long double,4)
DECL_SHMEM_X_PUT(double_put,double,3)
DECL_SHMEM_X_PUT(float_put,float,2)
DECL_SHMEM_X_PUT(putmem,void,0)
DECL_SHMEM_X_PUT(put8,void,0)
DECL_SHMEM_X_PUT(put16,void,1)
DECL_SHMEM_X_PUT(put32,void,2)
DECL_SHMEM_X_PUT(put64,void,3)
DECL_SHMEM_X_PUT(put128,void,4)

#define shmem_put(dest,src,nelems,pe) \
_Generic((dest), \
	float*:       shmem_float_put, \
	double*:      shmem_double_put, \
	long double*: shmem_longdouble_put, \
	char*:        shmem_char_put, \
	short*:       shmem_short_put, \
	int*:         shmem_int_put, \
	long*:        shmem_long_put, \
	long long*:   shmem_longlong_put \
)(dest,src,nelems,pe)


#define DECL_SHMEM_X_GET(N,T,S) \
SHMEM_SCOPE void \
shmem_##N (T *dest, const T *src, size_t nelems, int pe);

DECL_SHMEM_X_GET(char_get,char,0)
DECL_SHMEM_X_GET(short_get,short,1)
DECL_SHMEM_X_GET(int_get,int,2)
DECL_SHMEM_X_GET(long_get,long,3)
DECL_SHMEM_X_GET(longlong_get,long long,3)
DECL_SHMEM_X_GET(longdouble_get,long double,4)
DECL_SHMEM_X_GET(double_get,double,3)
DECL_SHMEM_X_GET(float_get,float,2)
DECL_SHMEM_X_GET(getmem,void,0)
DECL_SHMEM_X_GET(get8,void,0)
DECL_SHMEM_X_GET(get16,void,1)
DECL_SHMEM_X_GET(get32,void,2)
DECL_SHMEM_X_GET(get64,void,3)
DECL_SHMEM_X_GET(get128,void,4)

#define shmem_get(dest,src,nelems,pe) \
_Generic((dest), \
	float*:       shmem_float_get, \
	double*:      shmem_double_get, \
	long double*: shmem_longdouble_get, \
	char*:        shmem_char_get, \
	short*:       shmem_short_get, \
	int*:         shmem_int_get, \
	long*:        shmem_long_get, \
	long long*:   shmem_longlong_get \
)(dest,src,nelems,pe)


#define DECL_SHMEM_X_P(X,T) \
SHMEM_SCOPE void \
shmem_##X##_p (T *addr, T value, int pe);

DECL_SHMEM_X_P(char,char)
DECL_SHMEM_X_P(short,short)
DECL_SHMEM_X_P(int,int)
DECL_SHMEM_X_P(long,long)
DECL_SHMEM_X_P(longlong,long long)
DECL_SHMEM_X_P(float,float)
DECL_SHMEM_X_P(double,double)
DECL_SHMEM_X_P(longdouble,long double)

#define shmem_p(addr,value,pe) \
_Generic((addr), \
	float*:       shmem_float_p, \
	double*:      shmem_double_p, \
	long double*: shmem_longdouble_p, \
	char*:        shmem_char_p, \
	short*:       shmem_short_p, \
	int*:         shmem_int_p, \
	long*:        shmem_long_p, \
	long long*:   shmem_longlong_p \
)(addr,value,pe)


#define SHMEM_X_G(X,T) \
static T \
shmem_##X##_g (T *addr, int pe) \
{ return *((T*)shmem_ptr((void*)addr, pe)); }

SHMEM_X_G(char,char)
SHMEM_X_G(short,short)
SHMEM_X_G(int,int)
SHMEM_X_G(long,long)
SHMEM_X_G(longlong,long long)
SHMEM_X_G(float,float)
SHMEM_X_G(double,double)
SHMEM_X_G(longdouble,long double)

#define shmem_g(addr,pe) \
_Generic((addr), \
	float*:       shmem_float_g, \
	double*:      shmem_double_g, \
	long double*: shmem_longdouble_g, \
	char*:        shmem_char_g, \
	short*:       shmem_short_g, \
	int*:         shmem_int_g, \
	long*:        shmem_long_g, \
	long long*:   shmem_longlong_g \
)(addr,pe)


#define DECL_SHMEM_X_IPUT(N,T) \
SHMEM_SCOPE void \
shmem_##N (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);

DECL_SHMEM_X_IPUT(char_iput,char)
DECL_SHMEM_X_IPUT(short_iput,short)
DECL_SHMEM_X_IPUT(int_iput,int)
DECL_SHMEM_X_IPUT(long_iput,long)
DECL_SHMEM_X_IPUT(longlong_iput,long long)
DECL_SHMEM_X_IPUT(longdouble_iput,long double)
DECL_SHMEM_X_IPUT(double_iput,double)
DECL_SHMEM_X_IPUT(float_iput,float)
DECL_SHMEM_X_IPUT(iput8,void)
DECL_SHMEM_X_IPUT(iput16,void)
DECL_SHMEM_X_IPUT(iput32,void)
DECL_SHMEM_X_IPUT(iput64,void)
DECL_SHMEM_X_IPUT(iput128,void)

#define shmem_iput(dest,source,dst,sst,nelems,pe) \
_Generic((dest), \
	float*:       shmem_float_iput, \
	double*:      shmem_double_iput, \
	long double*: shmem_longdouble_iput, \
	char*:        shmem_char_iput, \
	short*:       shmem_short_iput, \
	int*:         shmem_int_iput, \
	long*:        shmem_long_iput, \
	long long*:   shmem_longlong_iput \
)(dest,source,dst,sst,nelems,pe)


#define DECL_SHMEM_X_IGET(N,T) \
SHMEM_SCOPE void \
shmem_##N (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);

DECL_SHMEM_X_IGET(char_iget,char)
DECL_SHMEM_X_IGET(short_iget,short)
DECL_SHMEM_X_IGET(int_iget,int)
DECL_SHMEM_X_IGET(long_iget,long)
DECL_SHMEM_X_IGET(longlong_iget,long long)
DECL_SHMEM_X_IGET(longdouble_iget,long double)
DECL_SHMEM_X_IGET(double_iget,double)
DECL_SHMEM_X_IGET(float_iget,float)
DECL_SHMEM_X_IGET(iget8,void)
DECL_SHMEM_X_IGET(iget16,void)
DECL_SHMEM_X_IGET(iget32,void)
DECL_SHMEM_X_IGET(iget64,void)
DECL_SHMEM_X_IGET(iget128,void)

#define shmem_iget(dest,source,dst,sst,nelems,pe) \
_Generic((dest), \
	float*:       shmem_float_iget, \
	double*:      shmem_double_iget, \
	long double*: shmem_longdouble_iget, \
	char*:        shmem_char_iget, \
	short*:       shmem_short_iget, \
	int*:         shmem_int_iget, \
	long*:        shmem_long_iget, \
	long long*:   shmem_longlong_iget \
)(dest,source,dst,sst,nelems,pe)


SHMEM_SCOPE void shmem_init(void);
SHMEM_SCOPE void shmem_finalize(void);
SHMEM_SCOPE void shmem_global_exit(int status);
SHMEM_SCOPE void shmem_info_get_version(int *major, int *minor);
SHMEM_SCOPE void shmem_info_get_name(char *name);

static int shmem_my_pe(void)
{ return __shmem.my_pe; }

static int shmem_n_pes(void)
{ return __shmem.n_pes; }

static int shmem_pe_accessible(int pe)
{ return ((pe >= 0 && pe < __shmem.n_pes) ? 1 : 0); }

static int shmem_addr_accessible(const void *addr, int pe)
{ 
	return (
		(shmem_pe_accessible(pe) 
			&& (unsigned int)addr < (unsigned int)shmemx_sbrk(0)
		) ? 1 : 0
	);
}

#ifdef __cplusplus
}
#endif

#if defined(SHMEM_USE_HEADER_ONLY)
#include "shmem_header_only.h"
#endif

#endif
