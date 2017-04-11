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

#include "internals.h"

#define SHMEM_MAJOR_VERSION             1
#define SHMEM_MINOR_VERSION             3
#define SHMEM_MAX_NAME_LEN              64
#define SHMEM_VENDOR_STRING             "ARL OpenSHMEM for Epiphany, version 1.3"
#define SHMEM_INTERNAL_F2C_SCALE        __INTERNAL_F2C_SCALE
#define SHMEM_BCAST_SYNC_SIZE           __BCAST_SYNC_SIZE
#define SHMEM_BARRIER_SYNC_SIZE         __BARRIER_SYNC_SIZE
#define SHMEM_REDUCE_SYNC_SIZE          __REDUCE_SYNC_SIZE
#define SHMEM_REDUCE_MIN_WRKDATA_SIZE   __REDUCE_MIN_WRKDATA_SIZE
#define SHMEM_SYNC_VALUE                __SYNC_VALUE
#define SHMEM_COLLECT_SYNC_SIZE         __COLLECT_SYNC_SIZE
#define SHMEM_ALLTOALL_SYNC_SIZE        __ALLTOALL_SYNC_SIZE
#define SHMEM_ALLTOALLS_SYNC_SIZE       __ALLTOALLS_SYNC_SIZE
#define _SHMEM_MAJOR_VERSION            SHMEM_MAJOR_VERSION
#define _SHMEM_MINOR_VERSION            SHMEM_MINOR_VERSION
#define _SHMEM_MAX_NAME_LEN             SHMEM_MAX_NAME_LEN
#define _SHMEM_VENDOR_STRING            SHMEM_VENDOR_STRING
#define _SHMEM_INTERNAL_F2C_SCALE       SHMEM_INTERNAL_F2C_SCALE
#define _SHMEM_BCAST_SYNC_SIZE          SHMEM_BCAST_SYNC_SIZE
#define _SHMEM_BARRIER_SYNC_SIZE        SHMEM_BARRIER_SYNC_SIZE
#define _SHMEM_REDUCE_SYNC_SIZE         SHMEM_REDUCE_SYNC_SIZE
#define _SHMEM_REDUCE_MIN_WRKDATA_SIZE  SHMEM_REDUCE_MIN_WRKDATA_SIZE
#define _SHMEM_SYNC_VALUE               SHMEM_SYNC_VALUE
#define _SHMEM_COLLECT_SYNC_SIZE        SHMEM_COLLECT_SYNC_SIZE
#define _my_pe(...)                     shmem_my_pe(__VA_ARGS__)
#define _num_pes(...)                   shmem_n_pes(__VA_ARGS__)
#define shmalloc(...)                   shmem_malloc(__VA_ARGS__)
#define shfree(...)                     shmem_free(__VA_ARGS__)
#define shrealloc(...)                  shmem_realloc(__VA_ARGS__)
#define shmemalign(...)                 shmem_align(__VA_ARGS__)
#define start_pes(...)                  shmem_init()
#define shmem_clear_cache_inv(...)      do{}while(0)
#define shmem_set_cache_inv(...)        do{}while(0)
#define shmem_clear_cache_line_inv(...) do{}while(0)
#define shmem_set_cache_line_inv(...)   do{}while(0)
#define shmem_udcflush(...)             do{}while(0)
#define shmem_udcflush_line(...)        do{}while(0)


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

SHMEM_SCOPE void* shmem_ptr(const void* dest, int pe);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_malloc(size_t size);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_align(size_t alignment, size_t size);
SHMEM_SCOPE void shmem_free(const void *ptr);
SHMEM_SCOPE void* shmem_realloc(const void* ptr, size_t size);

#define shmem_fence(...) shmem_quiet(__VA_ARGS__)
SHMEM_SCOPE void shmem_quiet(void);

SHMEM_SCOPE void shmem_clear_lock (volatile long* lock);
SHMEM_SCOPE void shmem_set_lock (volatile long* lock);
SHMEM_SCOPE int shmem_test_lock (volatile long* lock);

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 20112L) // Use C11 Generic syntax
#define DECL_GENERIC_TYPE(X,T,F,...) T: F, __VA_ARGS__
#define DECL_ARG1(X,T,F) T: F
#define DECL_GENERIC(X,...) _Generic((X),__VA_ARGS__)
#elif (defined (__GNUC__) && !defined(__cplusplus)) // Use GCC C builtin syntax
#define DECL_GENERIC_TYPE(X,T,F,...) __builtin_choose_expr(__builtin_types_compatible_p(typeof(&(*X)),T),F,__VA_ARGS__)
#define DECL_ARG1(X,T,F) DECL_GENERIC_TYPE(X,T,F,(void)0)
#define DECL_GENERIC(X,...) __VA_ARGS__
#else
#define DECL_GENERIC(...) (void)0 // Error, function unsupported/unavailable
#endif
#define DECL_ARG2(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG1(X,__VA_ARGS__))
#define DECL_ARG3(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG2(X,__VA_ARGS__))
#define DECL_ARG4(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG3(X,__VA_ARGS__))
#define DECL_ARG5(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG4(X,__VA_ARGS__))
#define DECL_ARG6(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG5(X,__VA_ARGS__))
#define DECL_ARG7(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG6(X,__VA_ARGS__))
#define DECL_ARG8(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG7(X,__VA_ARGS__))

#define DECL_STANDARD_AMO(F) \
F(int,int) \
F(long,long) \
F(longlong,long long)
#define DECL_EXTENDED_AMO(F) \
F(float,float) \
F(double,double) \
DECL_STANDARD_AMO(F)
#define DECL_STANDARD_RMA(F) \
F(longdouble,long double) \
F(char,char) \
F(short,short) \
DECL_EXTENDED_AMO(F)

#define DECL_GENERIC_STANDARD_AMO(A,F) \
DECL_GENERIC((A), \
DECL_ARG3((A), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong) \
))
#define DECL_GENERIC_EXTENDED_AMO(A,F) \
DECL_GENERIC((A), \
DECL_ARG5((A), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong), \
	float*, F(float), \
	double*, F(double) \
))
#define DECL_GENERIC_STANDARD_RMA(A,F) \
DECL_GENERIC((A), \
DECL_ARG8((A), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong), \
	float*, F(float), \
	double*, F(double), \
	long double*, F(longdouble) \
	char*, F(char), \
	short*, F(short) \
))

#define SHMEM_FINC(N,T) SHMEM_SCOPE T shmem_##N##_finc (T* dest, int pe);
#define SHMEM_INC(N,T) SHMEM_SCOPE void shmem_##N##_inc (T* dest, int pe);
#define SHMEM_FADD(N,T) SHMEM_SCOPE T shmem_##N##_fadd (T* dest, T value, int pe);
#define SHMEM_ADD(N,T) SHMEM_SCOPE void shmem_##N##_add (T* dest, T value, int pe);
#define SHMEM_CSWAP(N,T) SHMEM_SCOPE T shmem_##N##_cswap (T* dest, T cond, T value, int pe);
#define SHMEM_SWAP(N,T) SHMEM_SCOPE T shmem_##N##_swap (T* dest, T value, int pe);
#define SHMEM_FETCH(N,T) SHMEM_SCOPE T shmem_##N##_fetch (const T* dest, int pe);
#define SHMEM_SET(N,T) SHMEM_SCOPE void shmem_##N##_set (T* dest, T value, int pe);

DECL_STANDARD_AMO(SHMEM_FINC)
DECL_STANDARD_AMO(SHMEM_INC)
DECL_STANDARD_AMO(SHMEM_FADD)
DECL_STANDARD_AMO(SHMEM_ADD)
DECL_STANDARD_AMO(SHMEM_CSWAP)
DECL_EXTENDED_AMO(SHMEM_SWAP)
DECL_EXTENDED_AMO(SHMEM_FETCH)
DECL_EXTENDED_AMO(SHMEM_SET)

#define DECL_SHMEM_X_WAIT(N,T) \
SHMEM_SCOPE void shmem_##N (volatile T *ivar, T cmp_value);
DECL_SHMEM_X_WAIT(int_wait,int)
DECL_SHMEM_X_WAIT(long_wait,long)
DECL_SHMEM_X_WAIT(longlong_wait,long long)
DECL_SHMEM_X_WAIT(short_wait,short)
DECL_SHMEM_X_WAIT(wait,long)

#define DECL_SHMEM_X_WAIT_UNTIL(N,T) \
SHMEM_SCOPE void shmem_##N (volatile T *ivar, int cmp, T cmp_value);
DECL_SHMEM_X_WAIT_UNTIL(int_wait_until,int)
DECL_SHMEM_X_WAIT_UNTIL(long_wait_until,long)
DECL_SHMEM_X_WAIT_UNTIL(longlong_wait_until,long long)
DECL_SHMEM_X_WAIT_UNTIL(short_wait_until,short)
DECL_SHMEM_X_WAIT_UNTIL(wait_until,long)

SHMEM_SCOPE void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_barrier_all(void);

#define DECL_SHMEM_X_TO_ALL(N,T) \
SHMEM_SCOPE void shmem_##N##_to_all(T *dest, const T *source, int nreduce, int PE_start, int logPE_stride, int PE_size, T *pWrk, long *pSync);

DECL_SHMEM_X_TO_ALL(complexd_sum,double _Complex)
DECL_SHMEM_X_TO_ALL(complexf_sum,float _Complex)
DECL_SHMEM_X_TO_ALL(float_sum,float)
DECL_SHMEM_X_TO_ALL(double_sum,double)
DECL_SHMEM_X_TO_ALL(longdouble_sum,long double)
DECL_SHMEM_X_TO_ALL(short_sum,short)
DECL_SHMEM_X_TO_ALL(int_sum,int)
DECL_SHMEM_X_TO_ALL(long_sum,long)
DECL_SHMEM_X_TO_ALL(longlong_sum,long long)

DECL_SHMEM_X_TO_ALL(complexd_prod,double _Complex)
DECL_SHMEM_X_TO_ALL(complexf_prod,float _Complex)
DECL_SHMEM_X_TO_ALL(float_prod,float)
DECL_SHMEM_X_TO_ALL(double_prod,double)
DECL_SHMEM_X_TO_ALL(longdouble_prod,long double)
DECL_SHMEM_X_TO_ALL(short_prod,short)
DECL_SHMEM_X_TO_ALL(int_prod,int)
DECL_SHMEM_X_TO_ALL(long_prod,long)
DECL_SHMEM_X_TO_ALL(longlong_prod,long long)

DECL_SHMEM_X_TO_ALL(short_and,short)
DECL_SHMEM_X_TO_ALL(int_and,int)
DECL_SHMEM_X_TO_ALL(long_and,long)
DECL_SHMEM_X_TO_ALL(longlong_and,long long)

DECL_SHMEM_X_TO_ALL(short_or,short)
DECL_SHMEM_X_TO_ALL(int_or,int)
DECL_SHMEM_X_TO_ALL(long_or,long)
DECL_SHMEM_X_TO_ALL(longlong_or,long long)

DECL_SHMEM_X_TO_ALL(short_xor,short)
DECL_SHMEM_X_TO_ALL(int_xor,int)
DECL_SHMEM_X_TO_ALL(long_xor,long)
DECL_SHMEM_X_TO_ALL(longlong_xor,long long)

DECL_SHMEM_X_TO_ALL(float_max,float)
DECL_SHMEM_X_TO_ALL(double_max,double)
DECL_SHMEM_X_TO_ALL(longdouble_max,long double)
DECL_SHMEM_X_TO_ALL(short_max,short)
DECL_SHMEM_X_TO_ALL(int_max,int)
DECL_SHMEM_X_TO_ALL(long_max,long)
DECL_SHMEM_X_TO_ALL(longlong_max,long long)

DECL_SHMEM_X_TO_ALL(float_min,float)
DECL_SHMEM_X_TO_ALL(double_min,double)
DECL_SHMEM_X_TO_ALL(longdouble_min,long double)
DECL_SHMEM_X_TO_ALL(short_min,short)
DECL_SHMEM_X_TO_ALL(int_min,int)
DECL_SHMEM_X_TO_ALL(long_min,long)
DECL_SHMEM_X_TO_ALL(longlong_min,long long)


SHMEM_SCOPE void shmem_broadcast32(void *dest, const void *source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_broadcast64(void *dest, const void *source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long *pSync);

SHMEM_SCOPE void shmem_fcollect32(void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_fcollect64(void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

SHMEM_SCOPE void shmem_collect32(void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_collect64(void *dest, const void *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

SHMEM_SCOPE void shmem_alltoall32(void* dest, const void* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_alltoall64(void* dest, const void* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

SHMEM_SCOPE void shmem_alltoalls32(void* dest, const void* source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_alltoalls64(void* dest, const void* source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int PE_start, int logPE_stride, int PE_size, long *pSync);

#define SHMEM_PUT_NBI(N,T) SHMEM_SCOPE void shmem_##N##_put_nbi (T *dest, const T *src, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_PUT_NBI)
#define DECL_SHMEM_X_PUT_NBI(N,T) SHMEM_SCOPE void shmem_##N##_nbi (T *dest, const T *src, size_t nelems, int pe);
DECL_SHMEM_X_PUT_NBI(putmem,void)
DECL_SHMEM_X_PUT_NBI(put8,void)
DECL_SHMEM_X_PUT_NBI(put16,void)
DECL_SHMEM_X_PUT_NBI(put32,void)
DECL_SHMEM_X_PUT_NBI(put64,void)
DECL_SHMEM_X_PUT_NBI(put128,void)

#define SHMEM_GET_NBI(N,T) SHMEM_SCOPE void shmem_##N##_get_nbi (T *dest, const T *src, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_GET_NBI)
#define DECL_SHMEM_X_GET_NBI(N,T) SHMEM_SCOPE void shmem_##N##_nbi (T *dest, const T *src, size_t nelems, int pe);
DECL_SHMEM_X_GET_NBI(getmem,void)
DECL_SHMEM_X_GET_NBI(get8,void)
DECL_SHMEM_X_GET_NBI(get16,void)
DECL_SHMEM_X_GET_NBI(get32,void)
DECL_SHMEM_X_GET_NBI(get64,void)
DECL_SHMEM_X_GET_NBI(get128,void)

#define SHMEM_PUT(N,T) SHMEM_SCOPE void shmem_##N##_put (T *dest, const T *src, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_PUT)
#define DECL_SHMEM_X_PUT(N,T) SHMEM_SCOPE void shmem_##N (T *dest, const T *src, size_t nelems, int pe);
DECL_SHMEM_X_PUT(putmem,void)
DECL_SHMEM_X_PUT(put8,void)
DECL_SHMEM_X_PUT(put16,void)
DECL_SHMEM_X_PUT(put32,void)
DECL_SHMEM_X_PUT(put64,void)
DECL_SHMEM_X_PUT(put128,void)

#define SHMEM_GET(N,T) SHMEM_SCOPE void shmem_##N##_get (T *dest, const T *src, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_GET)
#define DECL_SHMEM_X_GET(N,T) SHMEM_SCOPE void shmem_##N (T *dest, const T *src, size_t nelems, int pe);
DECL_SHMEM_X_GET(getmem,void)
DECL_SHMEM_X_GET(get8,void)
DECL_SHMEM_X_GET(get16,void)
DECL_SHMEM_X_GET(get32,void)
DECL_SHMEM_X_GET(get64,void)
DECL_SHMEM_X_GET(get128,void)

#define SHMEM_P(X,T) \
static void shmem_##X##_p (T *dest, T value, int pe) \
{ T* ptr = (T*)shmem_ptr((void*)dest, pe); *ptr = value; }
DECL_STANDARD_RMA(SHMEM_P)

#define SHMEM_G(X,T) \
static T shmem_##X##_g (T *addr, int pe) \
{ return *((T*)shmem_ptr((void*)addr, pe)); }
DECL_STANDARD_RMA(SHMEM_G)

#define SHMEM_IPUT(N,T) \
SHMEM_SCOPE void shmem_##N##_iput (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_IPUT)
#define DECL_SHMEM_X_IPUT(N,T) \
SHMEM_SCOPE void shmem_##N (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_SHMEM_X_IPUT(iput8,void)
DECL_SHMEM_X_IPUT(iput16,void)
DECL_SHMEM_X_IPUT(iput32,void)
DECL_SHMEM_X_IPUT(iput64,void)
DECL_SHMEM_X_IPUT(iput128,void)

#define SHMEM_IGET(N,T) \
SHMEM_SCOPE void shmem_##N##_iget (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_IGET)
#define DECL_SHMEM_X_IGET(N,T) \
SHMEM_SCOPE void shmem_##N (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_SHMEM_X_IGET(iget8,void)
DECL_SHMEM_X_IGET(iget16,void)
DECL_SHMEM_X_IGET(iget32,void)
DECL_SHMEM_X_IGET(iget64,void)
DECL_SHMEM_X_IGET(iget128,void)

#ifndef __cplusplus
#define SHMEM_FINC_GENERIC(N) shmem_##N##_finc
#define SHMEM_INC_GENERIC(N) shmem_##N##_inc
#define SHMEM_FADD_GENERIC(N) shmem_##N##_fadd
#define SHMEM_ADD_GENERIC(N) shmem_##N##_add
#define SHMEM_CSWAP_GENERIC(N) shmem_##N##_cswap
#define SHMEM_SWAP_GENERIC(N) shmem_##N##_swap
#define SHMEM_FETCH_GENERIC(N) shmem_##N##_fetch
#define SHMEM_SET_GENERIC(N) shmem_##N##_set
#define SHMEM_PUT_NBI_GENERIC(N) shmem_##N##_put_nbi
#define SHMEM_GET_NBI_GENERIC(N) shmem_##N##_get_nbi
#define SHMEM_PUT_GENERIC(N) shmem_##N##_put
#define SHMEM_GET_GENERIC(N) shmem_##N##_get
#define SHMEM_P_GENERIC(N) shmem_##N##_p
#define SHMEM_G_GENERIC(N) shmem_##N##_g
#define SHMEM_IPUT_GENERIC(N) shmem_##N##_iput
#define SHMEM_IGET_GENERIC(N) shmem_##N##_iget
#define shmem_finc(dest,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_FINC_GENERIC)(dest,pe)
#define shmem_inc(dest,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_INC_GENERIC)(dest,pe)
#define shmem_fadd(dest,value,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_FADD_GENERIC)(dest,value,pe)
#define shmem_add(dest,value,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_ADD_GENERIC)(dest,value,pe)
#define shmem_cswap(dest,cond,value,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_CSWAP_GENERIC)(dest,cond,value,pe)
#define shmem_swap(dest,value,pe) DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_SWAP_GENERIC)(dest,value,pe)
#define shmem_fetch(dest,pe) DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_FETCH_GENERIC)(dest,pe)
#define shmem_set(dest,value,pe) DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_SET_GENERIC)(dest,value,pe)
#define shmem_put_nbi(dest,src,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_PUT_NBI_GENERIC)(dest,src,nelems,pe)
#define shmem_get_nbi(dest,src,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_GET_NBI_GENERIC)(dest,src,nelems,pe)
#define shmem_put(dest,src,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_PUT_GENERIC)(dest,src,nelems,pe)
#define shmem_get(dest,src,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_GET_GENERIC)(dest,src,nelems,pe)
#define shmem_p(dest,value,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_P_GENERIC)(dest,value,pe)
#define shmem_g(addr,pe) DECL_GENERIC_STANDARD_RMA(addr,SHMEM_G_GENERIC)(addr,pe)
#define shmem_iput(dest,source,dst,sst,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_IPUT_GENERIC)(dest,source,dst,sst,nelems,pe)
#define shmem_iget(dest,source,dst,sst,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_IGET_GENERIC)(dest,source,dst,sst,nelems,pe)
#endif


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
