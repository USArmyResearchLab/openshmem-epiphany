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

#ifndef _shmem_h
#define _shmem_h

#include "internals.h"

#define SHMEM_MAJOR_VERSION             1
#define SHMEM_MINOR_VERSION             4
#define SHMEM_MAX_NAME_LEN              64
#if defined(SHMEM_USE_HEADER_ONLY)
#define SHMEM_VENDOR_STRING             "ARL OpenSHMEM for Epiphany, version 1.4 (header-only)"
#else
#define SHMEM_VENDOR_STRING             "ARL OpenSHMEM for Epiphany, version 1.4 (shared library)"
#endif
#define SHMEM_INTERNAL_F2C_SCALE        __INTERNAL_F2C_SCALE
#define SHMEM_BCAST_SYNC_SIZE           __BCAST_SYNC_SIZE
#define SHMEM_BARRIER_SYNC_SIZE         __BARRIER_SYNC_SIZE
#define SHMEM_REDUCE_SYNC_SIZE          __REDUCE_SYNC_SIZE
#define SHMEM_REDUCE_MIN_WRKDATA_SIZE   __REDUCE_MIN_WRKDATA_SIZE
#define SHMEM_SYNC_VALUE                __SYNC_VALUE
#define SHMEM_COLLECT_SYNC_SIZE         __COLLECT_SYNC_SIZE
#define SHMEM_ALLTOALL_SYNC_SIZE        __ALLTOALL_SYNC_SIZE
#define SHMEM_ALLTOALLS_SYNC_SIZE       __ALLTOALLS_SYNC_SIZE
#define SHMEM_SYNC_SIZE                 __SYNC_SIZE
#define SHMEM_THREAD_SINGLE             __THREAD_SINGLE
#define SHMEM_THREAD_FUNNELED           __THREAD_FUNNELED
#define SHMEM_THREAD_SERIALIZED         __THREAD_SERIALIZED
#define SHMEM_THREAD_MULTIPLE           __THREAD_MULTIPLE
#define SHMEM_CTX_DEFAULT               __CTX_DEFAULT
#define SHMEM_CTX_SERIALIZED            __CTX_SERIALIZED
#define SHMEM_CTX_PRIVATE               __CTX_PRIVATE
#define SHMEM_CTX_NOSTORE               __CTX_NOSTORE
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

typedef enum shmem_cmp
{
	SHMEM_CMP_EQ = 0,
	SHMEM_CMP_NE,
	SHMEM_CMP_GT,
	SHMEM_CMP_GE,
	SHMEM_CMP_LT,
	SHMEM_CMP_LE
} shmem_cmp_t;

typedef struct shmem_ctx
{
	long options;
} shmem_ctx_t;

SHMEM_SCOPE void* shmem_ptr(const void* dest, int pe);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_calloc(size_t count, size_t size);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_malloc(size_t size);
SHMEM_SCOPE void* __attribute__((malloc)) shmem_align(size_t alignment, size_t size);
SHMEM_SCOPE void shmem_free(const void *ptr);
SHMEM_SCOPE void* shmem_realloc(const void* ptr, size_t size);

#define shmem_fence(...) shmem_quiet(__VA_ARGS__)
#define shmem_ctx_fence(ctx) shmem_ctx_quiet(ctx)
SHMEM_SCOPE void shmem_quiet(void);
static void shmem_ctx_quiet(shmem_ctx_t ctx)
{ shmem_quiet(); }

SHMEM_SCOPE void shmem_clear_lock (long* lock);
SHMEM_SCOPE void shmem_set_lock (long* lock);
SHMEM_SCOPE int shmem_test_lock (long* lock);

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
#define DECL_ARG9(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG8(X,__VA_ARGS__))
#define DECL_ARG10(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG9(X,__VA_ARGS__))
#define DECL_ARG11(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG10(X,__VA_ARGS__))
#define DECL_ARG12(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG11(X,__VA_ARGS__))
#define DECL_ARG13(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG12(X,__VA_ARGS__))
#define DECL_ARG14(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG13(X,__VA_ARGS__))
#define DECL_ARG15(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG14(X,__VA_ARGS__))
#define DECL_ARG16(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG15(X,__VA_ARGS__))
#define DECL_ARG17(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG16(X,__VA_ARGS__))
#define DECL_ARG18(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG17(X,__VA_ARGS__))
#define DECL_ARG19(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG18(X,__VA_ARGS__))
#define DECL_ARG20(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG19(X,__VA_ARGS__))
#define DECL_ARG21(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG20(X,__VA_ARGS__))
#define DECL_ARG22(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG21(X,__VA_ARGS__))
#define DECL_ARG23(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG22(X,__VA_ARGS__))
#define DECL_ARG24(X,T,F,...) DECL_GENERIC_TYPE(X,T,F,DECL_ARG23(X,__VA_ARGS__))

#define DECL_STANDARD_AMO(F) \
F(int,int) \
F(long,long) \
F(longlong,long long) \
F(uint,unsigned int) \
F(ulong,unsigned long) \
F(ulonglong,unsigned long long) \
F(int32,int32_t) \
F(int64,int64_t) \
F(uint32,uint32_t) \
F(uint64,uint64_t) \
F(size,size_t) \
F(ptrdiff,ptrdiff_t)

#define DECL_EXTENDED_AMO(F) \
F(float,float) \
F(double,double) \
DECL_STANDARD_AMO(F)

#define DECL_BITWISE_AMO(F) \
F(uint,unsigned int) \
F(ulong,unsigned long) \
F(ulonglong,unsigned long long) \
F(int32,int32_t) \
F(int64,int64_t) \
F(uint32,uint32_t) \
F(uint64,uint64_t)

#define DECL_STANDARD_RMA(F) \
F(longdouble,long double) \
F(char,char) \
F(short,short) \
DECL_EXTENDED_AMO(F)

#define DECL_P2P(F) \
F(short,short) \
F(ushort,unsigned short) \
DECL_STANDARD_AMO(F)

#define DECL_GENERIC_STANDARD_AMO(A,F) \
DECL_GENERIC((A), \
DECL_ARG12((A), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong), \
	unsigned int*, F(uint), \
	unsigned long*, F(ulong), \
	unsigned long long*, F(ulonglong), \
	int32_t*, F(int32), \
	int64_t*, F(int64), \
	uint32_t*, F(uint32), \
	uint64_t*, F(uint64), \
	size_t*, F(size), \
	ptrdiff_t*, F(ptrdiff) \
))

#define DECL_GENERIC_EXTENDED_AMO(A,F) \
DECL_GENERIC((A), \
DECL_ARG14((A), \
	float*, F(float), \
	double*, F(double), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong), \
	unsigned int*, F(uint), \
	unsigned long*, F(ulong), \
	unsigned long long*, F(ulonglong), \
	int32_t*, F(int32), \
	int64_t*, F(int64), \
	uint32_t*, F(uint32), \
	uint64_t*, F(uint64), \
	size_t*, F(size), \
	ptrdiff_t*, F(ptrdiff) \
))

#define DECL_GENERIC_BITWISE_AMO(A,F) \
DECL_GENERIC((A), \
DECL_ARG7((A), \
	unsigned int*, F(uint), \
	unsigned long*, F(ulong), \
	unsigned long long*, F(ulonglong), \
	int32_t*, F(int), \
	int64_t*, F(longlong), \
	uint32_t*, F(uint), \
	uint64_t*, F(ulonglong) \
))

#define DECL_GENERIC_STANDARD_RMA(A,F) \
DECL_GENERIC((A), \
DECL_ARG24((A), \
	float*, F(float), \
	double*, F(double), \
	long double*, F(longdouble), \
	char*, F(char), \
	signed char*, F(schar), \
	short*, F(short), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong), \
	unsigned char*, F(uchar), \
	unsigned short*, F(ushort), \
	unsigned int*, F(uint), \
	unsigned long*, F(ulong), \
	unsigned long long*, F(ulonglong), \
	int8_t*, F(int8), \
	int16_t*, F(int16), \
	int32_t*, F(int32), \
	int64_t*, F(int64), \
	uint8_t*, F(uint8), \
	uint16_t*, F(uint16), \
	uint32_t*, F(uint32), \
	uint64_t*, F(uint64), \
	size_t*, F(size), \
	ptrdiff_t*, F(ptrdiff) \
))

#define DECL_GENERIC_P2P(A,F) \
DECL_GENERIC((A), \
DECL_ARG14((A), \
	short*, F(short), \
	int*, F(int), \
	long*, F(long), \
	long long*, F(longlong), \
	unsigned short*, F(ushort), \
	unsigned int*, F(uint), \
	unsigned long*, F(ulong), \
	unsigned long long*, F(ulonglong), \
	int32_t*, F(int32), \
	int64_t*, F(int64), \
	uint32_t*, F(uint32), \
	uint64_t*, F(uint64), \
	size_t*, F(size), \
	ptrdiff_t*, F(ptrdiff) \
))

#define SHMEM_ATOMIC_COMPARE_SWAP(N,T) SHMEM_SCOPE T shmem_##N##_atomic_compare_swap (T* dest, T cond, T value, int pe);
#define SHMEM_ATOMIC_FETCH_INC(N,T) SHMEM_SCOPE T shmem_##N##_atomic_fetch_inc (T* dest, int pe);
#define SHMEM_ATOMIC_INC(N,T) SHMEM_SCOPE void shmem_##N##_atomic_inc (T* dest, int pe);
#define SHMEM_ATOMIC_FETCH_ADD(N,T) SHMEM_SCOPE T shmem_##N##_atomic_fetch_add (T* dest, T value, int pe);
#define SHMEM_ATOMIC_ADD(N,T) SHMEM_SCOPE void shmem_##N##_atomic_add (T* dest, T value, int pe);

#define SHMEM_ATOMIC_FETCH(N,T) SHMEM_SCOPE T shmem_##N##_atomic_fetch (const T* dest, int pe);
#define SHMEM_ATOMIC_SET(N,T) SHMEM_SCOPE void shmem_##N##_atomic_set (T* dest, T value, int pe);
#define SHMEM_ATOMIC_SWAP(N,T) SHMEM_SCOPE T shmem_##N##_atomic_swap (T* dest, T value, int pe);

#define SHMEM_ATOMIC_FETCH_AND(N,T) SHMEM_SCOPE T shmem_##N##_atomic_fetch_and (T* dest, T value, int pe);
#define SHMEM_ATOMIC_AND(N,T) SHMEM_SCOPE void shmem_##N##_atomic_and (T* dest, T value, int pe);
#define SHMEM_ATOMIC_FETCH_OR(N,T) SHMEM_SCOPE T shmem_##N##_atomic_fetch_or (T* dest, T value, int pe);
#define SHMEM_ATOMIC_OR(N,T) SHMEM_SCOPE void shmem_##N##_atomic_or (T* dest, T value, int pe);
#define SHMEM_ATOMIC_FETCH_XOR(N,T) SHMEM_SCOPE T shmem_##N##_atomic_fetch_xor (T* dest, T value, int pe);
#define SHMEM_ATOMIC_XOR(N,T) SHMEM_SCOPE void shmem_##N##_atomic_xor (T* dest, T value, int pe);

DECL_STANDARD_AMO(SHMEM_ATOMIC_COMPARE_SWAP)
DECL_STANDARD_AMO(SHMEM_ATOMIC_FETCH_INC)
DECL_STANDARD_AMO(SHMEM_ATOMIC_INC)
DECL_STANDARD_AMO(SHMEM_ATOMIC_FETCH_ADD)
DECL_STANDARD_AMO(SHMEM_ATOMIC_ADD)

DECL_EXTENDED_AMO(SHMEM_ATOMIC_FETCH)
DECL_EXTENDED_AMO(SHMEM_ATOMIC_SET)
DECL_EXTENDED_AMO(SHMEM_ATOMIC_SWAP)

DECL_BITWISE_AMO(SHMEM_ATOMIC_FETCH_AND)
DECL_BITWISE_AMO(SHMEM_ATOMIC_AND)
DECL_BITWISE_AMO(SHMEM_ATOMIC_FETCH_OR)
DECL_BITWISE_AMO(SHMEM_ATOMIC_OR)
DECL_BITWISE_AMO(SHMEM_ATOMIC_FETCH_XOR)
DECL_BITWISE_AMO(SHMEM_ATOMIC_XOR)

#define DECL_SHMEM_X_WAIT(N,T) SHMEM_SCOPE void shmem_##N##_wait (T *ivar, T cmp_value) __attribute__ ((deprecated));
#define DECL_SHMEM_X_WAIT_UNTIL(N,T) SHMEM_SCOPE void shmem_##N##_wait_until (T *ivar, shmem_cmp_t cmp, T cmp_value);
#define DECL_SHMEM_X_TEST(N,T) SHMEM_SCOPE int shmem_##N##_test (T *ivar, shmem_cmp_t cmp, T cmp_value);

DECL_P2P(DECL_SHMEM_X_WAIT)
DECL_P2P(DECL_SHMEM_X_WAIT_UNTIL)
DECL_P2P(DECL_SHMEM_X_TEST)

SHMEM_SCOPE void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync);
SHMEM_SCOPE void shmem_barrier_all(void);

#define shmem_sync(...) shmem_barrier(__VA_ARGS__)
#define shmem_sync_all(...) shmem_barrier_all(__VA_ARGS__)

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

#define SHMEM_PUT_NBI(N,T) \
SHMEM_SCOPE void shmem_##N##_put_nbi (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N##_put_nbi (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_PUT_NBI)
#define DECL_SHMEM_X_PUT_NBI(N,T) \
SHMEM_SCOPE void shmem_##N##_nbi (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N##_nbi (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_SHMEM_X_PUT_NBI(putmem,void)
DECL_SHMEM_X_PUT_NBI(put8,void)
DECL_SHMEM_X_PUT_NBI(put16,void)
DECL_SHMEM_X_PUT_NBI(put32,void)
DECL_SHMEM_X_PUT_NBI(put64,void)
DECL_SHMEM_X_PUT_NBI(put128,void)

#define SHMEM_GET_NBI(N,T) \
SHMEM_SCOPE void shmem_##N##_get_nbi (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N##_get_nbi (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_GET_NBI)
#define DECL_SHMEM_X_GET_NBI(N,T) \
SHMEM_SCOPE void shmem_##N##_nbi (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N##_nbi (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_SHMEM_X_GET_NBI(getmem,void)
DECL_SHMEM_X_GET_NBI(get8,void)
DECL_SHMEM_X_GET_NBI(get16,void)
DECL_SHMEM_X_GET_NBI(get32,void)
DECL_SHMEM_X_GET_NBI(get64,void)
DECL_SHMEM_X_GET_NBI(get128,void)

#define SHMEM_PUT(N,T) \
SHMEM_SCOPE void shmem_##N##_put (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N##_put (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_PUT)
#define DECL_SHMEM_X_PUT(N,T) \
SHMEM_SCOPE void shmem_##N (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_SHMEM_X_PUT(putmem,void)
DECL_SHMEM_X_PUT(put8,void)
DECL_SHMEM_X_PUT(put16,void)
DECL_SHMEM_X_PUT(put32,void)
DECL_SHMEM_X_PUT(put64,void)
DECL_SHMEM_X_PUT(put128,void)

#define SHMEM_GET(N,T) \
SHMEM_SCOPE void shmem_##N##_get (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N##_get (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_GET)
#define DECL_SHMEM_X_GET(N,T) \
SHMEM_SCOPE void shmem_##N (T *dest, const T *source, size_t nelems, int pe); \
static void shmem_ctx_##N (shmem_ctx_t ctx, T *dest, const T *source, size_t nelems, int pe);
DECL_SHMEM_X_GET(getmem,void)
DECL_SHMEM_X_GET(get8,void)
DECL_SHMEM_X_GET(get16,void)
DECL_SHMEM_X_GET(get32,void)
DECL_SHMEM_X_GET(get64,void)
DECL_SHMEM_X_GET(get128,void)

#define SHMEM_P(X,T) \
static void shmem_##X##_p (T *dest, T value, int pe) \
{ T* ptr = (T*)shmem_ptr((void*)dest, pe); *ptr = value; } \
static void shmem_ctx_##X##_p (shmem_ctx_t ctx, T *dest, T value, int pe);
DECL_STANDARD_RMA(SHMEM_P)

#define SHMEM_G(X,T) \
static T shmem_##X##_g (T *addr, int pe) \
{ return *((T*)shmem_ptr((void*)addr, pe)); } \
static T shmem_ctx_##X##_g (T *addr, int pe);
DECL_STANDARD_RMA(SHMEM_G)

#define SHMEM_IPUT(N,T) \
SHMEM_SCOPE void shmem_##N##_iput (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe); \
static void shmem_ctx_##N##_iput (shmem_ctx_t ctx, T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_IPUT)
#define DECL_SHMEM_X_IPUT(N,T) \
SHMEM_SCOPE void shmem_##N (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe); \
static void shmem_ctx_##N (shmem_ctx_t ctx, T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_SHMEM_X_IPUT(iput8,void)
DECL_SHMEM_X_IPUT(iput16,void)
DECL_SHMEM_X_IPUT(iput32,void)
DECL_SHMEM_X_IPUT(iput64,void)
DECL_SHMEM_X_IPUT(iput128,void)

#define SHMEM_IGET(N,T) \
SHMEM_SCOPE void shmem_##N##_iget (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe); \
static void shmem_ctx_##N##_iget (shmem_ctx_t ctx, T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_STANDARD_RMA(SHMEM_IGET)
#define DECL_SHMEM_X_IGET(N,T) \
SHMEM_SCOPE void shmem_##N (T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe); \
static void shmem_ctx_##N (shmem_ctx_t ctx, T *dest, const T *source, ptrdiff_t dst, ptrdiff_t sst, size_t nelems, int pe);
DECL_SHMEM_X_IGET(iget8,void)
DECL_SHMEM_X_IGET(iget16,void)
DECL_SHMEM_X_IGET(iget32,void)
DECL_SHMEM_X_IGET(iget64,void)
DECL_SHMEM_X_IGET(iget128,void)

#ifndef __cplusplus

#define SHMEM_ATOMIC_FETCH_INC_GENERIC(N)    shmem_##N##_atomic_fetch_inc
#define SHMEM_ATOMIC_INC_GENERIC(N)          shmem_##N##_atomic_inc
#define SHMEM_ATOMIC_FETCH_ADD_GENERIC(N)    shmem_##N##_atomic_fetch_add
#define SHMEM_ATOMIC_ADD_GENERIC(N)          shmem_##N##_atomic_add
#define SHMEM_ATOMIC_COMPARE_SWAP_GENERIC(N) shmem_##N##_atomic_compare_swap
#define SHMEM_ATOMIC_SWAP_GENERIC(N)         shmem_##N##_atomic_swap
#define SHMEM_ATOMIC_FETCH_GENERIC(N)        shmem_##N##_atomic_fetch
#define SHMEM_ATOMIC_SET_GENERIC(N)          shmem_##N##_atomic_set
#define SHMEM_ATOMIC_FETCH_AND_GENERIC(N)    shmem_##N##_atomic_fetch_and
#define SHMEM_ATOMIC_AND_GENERIC(N)          shmem_##N##_atomic_and
#define SHMEM_ATOMIC_FETCH_OR_GENERIC(N)     shmem_##N##_atomic_fetch_or
#define SHMEM_ATOMIC_OR_GENERIC(N)           shmem_##N##_atomic_or
#define SHMEM_ATOMIC_FETCH_XOR_GENERIC(N)    shmem_##N##_atomic_fetch_xor
#define SHMEM_ATOMIC_XOR_GENERIC(N)          shmem_##N##_atomic_xor

#define SHMEM_CTX_ATOMIC_FETCH_INC_GENERIC(N)    shmem_ctx_##N##_atomic_fetch_inc
#define SHMEM_CTX_ATOMIC_INC_GENERIC(N)          shmem_ctx_##N##_atomic_inc
#define SHMEM_CTX_ATOMIC_FETCH_ADD_GENERIC(N)    shmem_ctx_##N##_atomic_fetch_add
#define SHMEM_CTX_ATOMIC_ADD_GENERIC(N)          shmem_ctx_##N##_atomic_add
#define SHMEM_CTX_ATOMIC_COMPARE_SWAP_GENERIC(N) shmem_ctx_##N##_atomic_compare_swap
#define SHMEM_CTX_ATOMIC_SWAP_GENERIC(N)         shmem_ctx_##N##_atomic_swap
#define SHMEM_CTX_ATOMIC_FETCH_GENERIC(N)        shmem_ctx_##N##_atomic_fetch
#define SHMEM_CTX_ATOMIC_SET_GENERIC(N)          shmem_ctx_##N##_atomic_set
#define SHMEM_CTX_ATOMIC_FETCH_AND_GENERIC(N)    shmem_ctx_##N##_atomic_fetch_and
#define SHMEM_CTX_ATOMIC_AND_GENERIC(N)          shmem_ctx_##N##_atomic_and
#define SHMEM_CTX_ATOMIC_FETCH_OR_GENERIC(N)     shmem_ctx_##N##_atomic_fetch_or
#define SHMEM_CTX_ATOMIC_OR_GENERIC(N)           shmem_ctx_##N##_atomic_or
#define SHMEM_CTX_ATOMIC_FETCH_XOR_GENERIC(N)    shmem_ctx_##N##_atomic_fetch_xor
#define SHMEM_CTX_ATOMIC_XOR_GENERIC(N)          shmem_ctx_##N##_atomic_xor

#define SHMEM_WAIT_GENERIC(N)       shmem_##N##_wait
#define SHMEM_WAIT_UNTIL_GENERIC(N) shmem_##N##_wait_until
#define SHMEM_TEST_GENERIC(N)       shmem_##N##_test

#define SHMEM_PUT_NBI_GENERIC(N)    shmem_##N##_put_nbi
#define SHMEM_GET_NBI_GENERIC(N)    shmem_##N##_get_nbi
#define SHMEM_PUT_GENERIC(N)        shmem_##N##_put
#define SHMEM_GET_GENERIC(N)        shmem_##N##_get
#define SHMEM_P_GENERIC(N)          shmem_##N##_p
#define SHMEM_G_GENERIC(N)          shmem_##N##_g
#define SHMEM_IPUT_GENERIC(N)       shmem_##N##_iput
#define SHMEM_IGET_GENERIC(N)       shmem_##N##_iget

#define SHMEM_CTX_PUT_NBI_GENERIC(N)    shmem_ctx_##N##_put_nbi
#define SHMEM_CTX_GET_NBI_GENERIC(N)    shmem_ctx_##N##_get_nbi
#define SHMEM_CTX_PUT_GENERIC(N)        shmem_ctx_##N##_put
#define SHMEM_CTX_GET_GENERIC(N)        shmem_ctx_##N##_get
#define SHMEM_CTX_P_GENERIC(N)          shmem_ctx_##N##_p
#define SHMEM_CTX_G_GENERIC(N)          shmem_ctx_##N##_g
#define SHMEM_CTX_IPUT_GENERIC(N)       shmem_ctx_##N##_iput
#define SHMEM_CTX_IGET_GENERIC(N)       shmem_ctx_##N##_iget

#define __atomic_fetch_inc(dest,pe)               DECL_GENERIC_STANDARD_AMO(dest,SHMEM_ATOMIC_FETCH_INC_GENERIC)(dest,pe)
#define __atomic_inc(dest,pe)                     DECL_GENERIC_STANDARD_AMO(dest,SHMEM_ATOMIC_INC_GENERIC)(dest,pe)
#define __atomic_fetch_add(dest,value,pe)         DECL_GENERIC_STANDARD_AMO(dest,SHMEM_ATOMIC_FETCH_ADD_GENERIC)(dest,value,pe)
#define __atomic_add(dest,value,pe)               DECL_GENERIC_STANDARD_AMO(dest,SHMEM_ATOMIC_ADD_GENERIC)(dest,value,pe)
#define __atomic_compare_swap(dest,cond,value,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_ATOMIC_COMPARE_SWAP_GENERIC)(dest,cond,value,pe)
#define __atomic_swap(dest,value,pe)              DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_ATOMIC_SWAP_GENERIC)(dest,value,pe)
#define __atomic_fetch(dest,pe)                   DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_ATOMIC_FETCH_GENERIC)(dest,pe)
#define __atomic_set(dest,value,pe)               DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_ATOMIC_SET_GENERIC)(dest,value,pe)
#define __atomic_fetch_and(dest,value,pe)         DECL_GENERIC_BITWISE_AMO(dest,SHMEM_ATOMIC_FETCH_AND_GENERIC)(dest,value,pe)
#define __atomic_and(dest,value,pe)               DECL_GENERIC_BITWISE_AMO(dest,SHMEM_ATOMIC_AND_GENERIC)(dest,value,pe)
#define __atomic_fetch_or(dest,value,pe)          DECL_GENERIC_BITWISE_AMO(dest,SHMEM_ATOMIC_FETCH_OR_GENERIC)(dest,value,pe)
#define __atomic_or(dest,value,pe)                DECL_GENERIC_BITWISE_AMO(dest,SHMEM_ATOMIC_OR_GENERIC)(dest,value,pe)
#define __atomic_fetch_xor(dest,value,pe)         DECL_GENERIC_BITWISE_AMO(dest,SHMEM_ATOMIC_FETCH_XOR_GENERIC)(dest,value,pe)
#define __atomic_xor(dest,value,pe)               DECL_GENERIC_BITWISE_AMO(dest,SHMEM_ATOMIC_XOR_GENERIC)(dest,value,pe)

#define __ctx_atomic_fetch_inc(ctx,dest,pe)               DECL_GENERIC_STANDARD_AMO(dest,SHMEM_CTX_ATOMIC_FETCH_INC_GENERIC)(ctx,dest,pe)
#define __ctx_atomic_inc(ctx,dest,pe)                     DECL_GENERIC_STANDARD_AMO(dest,SHMEM_CTX_ATOMIC_INC_GENERIC)(ctx,dest,pe)
#define __ctx_atomic_fetch_add(ctx,dest,value,pe)         DECL_GENERIC_STANDARD_AMO(dest,SHMEM_CTX_ATOMIC_FETCH_ADD_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_add(ctx,dest,value,pe)               DECL_GENERIC_STANDARD_AMO(dest,SHMEM_CTX_ATOMIC_ADD_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_compare_swap(ctx,dest,cond,value,pe) DECL_GENERIC_STANDARD_AMO(dest,SHMEM_CTX_ATOMIC_COMPARE_SWAP_GENERIC)(ctx,dest,cond,value,pe)
#define __ctx_atomic_swap(ctx,dest,value,pe)              DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_CTX_ATOMIC_SWAP_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_fetch(ctx,dest,pe)                   DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_CTX_ATOMIC_FETCH_GENERIC)(ctx,dest,pe)
#define __ctx_atomic_set(ctx,dest,value,pe)               DECL_GENERIC_EXTENDED_AMO(dest,SHMEM_CTX_ATOMIC_SET_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_fetch_and(ctx,dest,value,pe)         DECL_GENERIC_BITWISE_AMO(dest,SHMEM_CTX_ATOMIC_FETCH_AND_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_and(ctx,dest,value,pe)               DECL_GENERIC_BITWISE_AMO(dest,SHMEM_CTX_ATOMIC_AND_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_fetch_or(ctx,dest,value,pe)          DECL_GENERIC_BITWISE_AMO(dest,SHMEM_CTX_ATOMIC_FETCH_OR_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_or(ctx,dest,value,pe)                DECL_GENERIC_BITWISE_AMO(dest,SHMEM_CTX_ATOMIC_OR_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_fetch_xor(ctx,dest,value,pe)         DECL_GENERIC_BITWISE_AMO(dest,SHMEM_CTX_ATOMIC_FETCH_XOR_GENERIC)(ctx,dest,value,pe)
#define __ctx_atomic_xor(ctx,dest,value,pe)               DECL_GENERIC_BITWISE_AMO(dest,SHMEM_CTX_ATOMIC_XOR_GENERIC)(ctx,dest,value,pe)

#define GET_MACRO2(_2,_1,_0,F,...) F
#define GET_MACRO3(...) GET_MACRO2(_3,__VA_ARGS__)
#define GET_MACRO4(...) GET_MACRO3(_4,__VA_ARGS__)
#define GET_MACRO6(...) GET_MACRO4(_6,_5,__VA_ARGS__)

#define shmem_atomic_fetch_inc(...)    GET_MACRO2(__VA_ARGS__, __ctx_atomic_fetch_inc,    __atomic_fetch_inc)(__VA_ARGS__);
#define shmem_atomic_inc(...)          GET_MACRO2(__VA_ARGS__, __ctx_atomic_inc,          __atomic_inc)(__VA_ARGS__);
#define shmem_atomic_fetch_add(...)    GET_MACRO3(__VA_ARGS__, __ctx_atomic_fetch_add,    __atomic_fetch_add)(__VA_ARGS__);
#define shmem_atomic_add(...)          GET_MACRO3(__VA_ARGS__, __ctx_atomic_add,          __atomic_add)(__VA_ARGS__);
#define shmem_atomic_compare_swap(...) GET_MACRO4(__VA_ARGS__, __ctx_atomic_compare_swap, __atomic_compare_swap)(__VA_ARGS__);
#define shmem_atomic_swap(...)         GET_MACRO3(__VA_ARGS__, __ctx_atomic_swap,         __atomic_swap)(__VA_ARGS__);
#define shmem_atomic_fetch(...)        GET_MACRO2(__VA_ARGS__, __ctx_atomic_fetch,        __atomic_fetch)(__VA_ARGS__);
#define shmem_atomic_set(...)          GET_MACRO3(__VA_ARGS__, __ctx_atomic_set,          __atomic_set)(__VA_ARGS__);
#define shmem_atomic_fetch_and(...)    GET_MACRO3(__VA_ARGS__, __ctx_atomic_fetch_and,    __atomic_fetch_and)(__VA_ARGS__);
#define shmem_atomic_and(...)          GET_MACRO3(__VA_ARGS__, __ctx_atomic_and,          __atomic_and)(__VA_ARGS__);
#define shmem_atomic_fetch_or(...)     GET_MACRO3(__VA_ARGS__, __ctx_atomic_fetch_or,     __atomic_fetch_or)(__VA_ARGS__);
#define shmem_atomic_or(...)           GET_MACRO3(__VA_ARGS__, __ctx_atomic_or,           __atomic_or)(__VA_ARGS__);
#define shmem_atomic_fetch_xor(...)    GET_MACRO3(__VA_ARGS__, __ctx_atomic_fetch_xor,    __atomic_fetch_xor)(__VA_ARGS__);
#define shmem_atomic_xor(...)          GET_MACRO3(__VA_ARGS__, __ctx_atomic_xor,          __atomic_xor)(__VA_ARGS__);

#define shmem_wait(ivar,cmp_value)           DECL_GENERIC_P2P(ivar,SHMEM_WAIT_GENERIC)(ivar,cmp_value)
#define shmem_wait_until(ivar,cmp,cmp_value) DECL_GENERIC_P2P(ivar,SHMEM_WAIT_UNTIL_GENERIC)(ivar,cmp,cmp_value)
#define shmem_test(ivar,cmp,value)           DECL_GENERIC_P2P(ivar,SHMEM_TEST_GENERIC)(ivar,cmp,value)

#define shmem_finc(...)  shmem_atomic_fetch_inc(__VA_ARGS__)
#define shmem_inc(...)   shmem_atomic_inc(__VA_ARGS__)
#define shmem_fadd(...)  shmem_atomic_fetch_add(__VA_ARGS__)
#define shmem_add(...)   shmem_atomic_add(__VA_ARGS__)
#define shmem_cswap(...) shmem_atomic_compare_swap(__VA_ARGS__)
#define shmem_swap(...)  shmem_atomic_swap(__VA_ARGS__)
#define shmem_fetch(...) shmem_atomic_fetch(__VA_ARGS__)
#define shmem_set(...)   shmem_atomic_set(__VA_ARGS__)

#define __put_nbi(dest,source,nelems,pe)      DECL_GENERIC_STANDARD_RMA(dest,SHMEM_PUT_NBI_GENERIC)(dest,source,nelems,pe)
#define __get_nbi(dest,source,nelems,pe)      DECL_GENERIC_STANDARD_RMA(dest,SHMEM_GET_NBI_GENERIC)(dest,source,nelems,pe)
#define __put(dest,source,nelems,pe)          DECL_GENERIC_STANDARD_RMA(dest,SHMEM_PUT_GENERIC)(dest,source,nelems,pe)
#define __get(dest,source,nelems,pe)          DECL_GENERIC_STANDARD_RMA(dest,SHMEM_GET_GENERIC)(dest,source,nelems,pe)
#define __p(dest,value,pe)                    DECL_GENERIC_STANDARD_RMA(dest,SHMEM_P_GENERIC)(dest,value,pe)
#define __g(addr,pe)                          DECL_GENERIC_STANDARD_RMA(addr,SHMEM_G_GENERIC)(addr,pe)
#define __iput(dest,source,dst,sst,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_IPUT_GENERIC)(dest,source,dst,sst,nelems,pe)
#define __iget(dest,source,dst,sst,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_IGET_GENERIC)(dest,source,dst,sst,nelems,pe)

#define __ctx_put_nbi(ctx,dest,source,nelems,pe)      DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_PUT_NBI_GENERIC)(ctx,dest,source,nelems,pe)
#define __ctx_get_nbi(ctx,dest,source,nelems,pe)      DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_GET_NBI_GENERIC)(ctx,dest,source,nelems,pe)
#define __ctx_put(ctx,dest,source,nelems,pe)          DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_PUT_GENERIC)(ctx,dest,source,nelems,pe)
#define __ctx_get(ctx,dest,source,nelems,pe)          DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_GET_GENERIC)(ctx,dest,source,nelems,pe)
#define __ctx_p(ctx,dest,value,pe)                    DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_P_GENERIC)(ctx,dest,value,pe)
#define __ctx_g(ctx,addr,pe)                          DECL_GENERIC_STANDARD_RMA(addr,SHMEM_CTX_G_GENERIC)(ctx,addr,pe)
#define __ctx_iput(ctx,dest,source,dst,sst,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_IPUT_GENERIC)(ctx,dest,source,dst,sst,nelems,pe)
#define __ctx_iget(ctx,dest,source,dst,sst,nelems,pe) DECL_GENERIC_STANDARD_RMA(dest,SHMEM_CTX_IGET_GENERIC)(ctx,dest,source,dst,sst,nelems,pe)

#define shmem_put_nbi(...) GET_MACRO4(__VA_ARGS__, __ctx_put_nbi, __put_nbi)(__VA_ARGS__)
#define shmem_get_nbi(...) GET_MACRO4(__VA_ARGS__, __ctx_get_nbi, __get_nbi)(__VA_ARGS__)
#define shmem_put(...)     GET_MACRO4(__VA_ARGS__, __ctx_put,     __put)(__VA_ARGS__)
#define shmem_get(...)     GET_MACRO4(__VA_ARGS__, __ctx_get,     __get)(__VA_ARGS__)
#define shmem_p(...)       GET_MACRO3(__VA_ARGS__, __ctx_p,       __p)(__VA_ARGS__)
#define shmem_g(...)       GET_MACRO2(__VA_ARGS__, __ctx_g,       __g)(__VA_ARGS__)
#define shmem_iput(...)    GET_MACRO6(__VA_ARGS__, __ctx_iput,    __iput)(__VA_ARGS__)
#define shmem_iget(...)    GET_MACRO6(__VA_ARGS__, __ctx_iget,    __iget)(__VA_ARGS__)

#endif

SHMEM_SCOPE void shmem_init(void);
SHMEM_SCOPE int shmem_init_thread(int requested, int *provided);
SHMEM_SCOPE void shmem_query_thread(int *provided);
SHMEM_SCOPE int shmem_ctx_create(long options, shmem_ctx_t *ctx);
SHMEM_SCOPE void shmem_ctx_destroy(shmem_ctx_t *ctx);
SHMEM_SCOPE void shmem_finalize(void);
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 20112L)
_Noreturn
#endif
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
