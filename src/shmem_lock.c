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

long*
__shmem_lock_ptr (const long* p)
{
	if ((unsigned int)p <= 0x100000) // addr > 1 MB
		return (long*)(__shmem.lock_high_bits | (unsigned int)p);
	return (long*)p;
}

void
__shmem_clear_lock (volatile long* x)
{
	shmem_quiet();
	*x = 0;
}

void
__shmem_set_lock (volatile long* x)
{
	__asm__ __volatile__(
		"mov r63, #0                 \n" // zero lock pointer offset
		"mov r62, #1                 \n" // value to write to lock
		".Loop%=:                    \n"
		"   mov r61, r62             \n" // copying value to write to lock
		"   testset r61, [%[x], r63] \n" // test set
		"   sub r61, r61, #0         \n" // checking result
		"   bne .Loop%=              \n" // if zero, loop until we acquire lock
		:
		: [x] "r" (x)
		: "r61", "r62", "r63"
	);
}

void
__shmem_set_lock_self (long* x)
{
	long* gx = (long*)(e_emem_config.base | (unsigned int)x);
	__shmem_set_lock(gx);
}

int
__shmem_test_lock (volatile long* x)
{
	long r = 1; // attempting to acquire the lock
	__asm__ __volatile__(
		"mov r63, #0               \n" // zero lock pointer offset
		"testset %[r], [%[x], r63] \n" // test set
		: [r] "+r" (r)
		: [x] "r" (x)
		: "r63"
	); // return 0 if the lock was originally cleared and call set lock
	return (r ? 0 : 1); // return 1 of the lock had already been set.
}

void
shmem_clear_lock (volatile long* lock)
{
	long* x = __shmem_lock_ptr((const long*)lock);
	__shmem_clear_lock(x);
}

void
shmem_set_lock (volatile long* lock)
{
	long* x = __shmem_lock_ptr((const long*)lock);
	__shmem_set_lock(x);
}

int
shmem_test_lock (volatile long* lock)
{
	long* x = __shmem_lock_ptr((const long*)lock);
	return __shmem_test_lock(x);
}

