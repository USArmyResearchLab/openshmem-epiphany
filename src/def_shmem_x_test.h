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


#ifndef _def_shmem_x_test_h
#define _def_shmem_x_test_h

#include "internals.h"
#include "shmem.h"

#define SHMEM_X_TEST(N,T) \
SHMEM_SCOPE int \
shmem_##N##_test (T *ivar, shmem_cmp_t cmp, T cmp_value) \
{ \
	volatile T* p = (volatile T*)ivar; \
	switch (cmp) { \
		case SHMEM_CMP_EQ: \
			if (*p == cmp_value) return 1; \
			break; \
		case SHMEM_CMP_NE: \
			if (*p != cmp_value) return 1; \
			break; \
		case SHMEM_CMP_GT: \
			if (*p > cmp_value) return 1; \
			break; \
		case SHMEM_CMP_GE: \
			if (*p >= cmp_value) return 1; \
			break; \
		case SHMEM_CMP_LT: \
			if (*p < cmp_value) return 1; \
			break; \
		case SHMEM_CMP_LE: \
			if (*p <= cmp_value) return 1; \
			break; \
	} \
	return 0; \
}

#endif
