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

/*
 * SUPPLEMENTAL ROUTINES (non-standard)
 */

//#ifdef SHMEM_USE_CTIMER

void
__shmem_ctimer_start(void)
{
	__asm__ __volatile__ (
		"mov r0, 0xFFFF              \n" // load MAX value
		"movt r0, 0xFFFF             \n" // into r0
		"movts CTIMER0, r0           \n" // set ctimer0 to MAX
		"movfs r0, CONFIG            \n" // read CONFIG register
		"mov r1, %%low(0xFFFFFF0F)   \n" // low bits of CONFIG mask
		"movt r1, %%high(0xFFFFFF0F) \n" // and top bits
		"mov r2, 0x10                \n" // 
		"and r0, r0, r1              \n" // apply mask to clear TIMERMODE bits from previous config
		"movts CONFIG, r0            \n" // turn off ctimer0
		"orr r0, r0, r2              \n" // addd new TIMERMODE to config
		"movts CONFIG, r0            \n" // start the ctimer counter
		: : : "r0", "r1", "r2", "cc"
	);
}

//SHMEM_SCOPE unsigned int SHMEM_INLINE
//__shmem_get_ctimer(void)
//{
//	register unsigned int tmp;
//	__asm__ __volatile(
//		"movfs %[tmp], CTIMER0 \n"
//		: [tmp] "=r" (tmp)
//	);
//	return tmp;
//}

//#endif // SHMEM_USE_CTIMER

