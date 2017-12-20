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

#ifndef _ctimer_h
#define _ctimer_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __epiphany__ // Specific to Epiphany
#define INV_GHZ 1.66666667f // 1/0.6 GHz

static void
ctimer_start(void)
{
	__asm__ __volatile__ (
		"movfs r1, CONFIG            \n" // read CONFIG register
		"mov r0, %%low(0xFFFFFF0F)   \n" // low bits of CONFIG mask
		"movt r0, %%high(0xFFFFFF0F) \n" // and top bits
//		"add r0, r0, 0xF0            \n" // XXX uncomment if you need MAX value...
		"movts CTIMER0, r0           \n" // ...or lazy...set ctimer0 almost to MAX value (0xFFFFFFFF)
		"mov r2, 0x10                \n" // CTIMER0 CONFIG starts at bit 4 for CLK (0001 << 4)
		"and r1, r1, r0              \n" // apply mask to clear TIMERMODE bits from previous CONFIG
		"movts CONFIG, r1            \n" // turn off ctimer0
		"orr r1, r1, r2              \n" // add new TIMERMODE to CONFIG
		"movts CONFIG, r1            \n" // start the ctimer counter
		: : : "r0", "r1", "r2", "cc"
	);
}

static unsigned int
ctimer_nsec(unsigned int cycles)
{
	float fcycles = (float)cycles;
	unsigned int nsec = (unsigned int)(fcycles * INV_GHZ);
	return nsec;
}

static unsigned int inline __attribute__((__always_inline__)) 
ctimer(void)
{
	register unsigned int tmp;
	__asm__ __volatile(
		"movfs %[tmp], CTIMER0 \n"
		: [tmp] "=r" (tmp)
	);
	return tmp;
}

#else // Generic timer

#include <sys/time.h>

struct timeval t0; // initialized with ctimer_start()

static void
ctimer_start(void)
{
	gettimeofday(&t0, 0);
}

static uint64_t
ctimer_nsec(uint64_t t)
{
	return t;
}

static uint64_t inline __attribute__((__always_inline__)) 
ctimer(void)
{
	const static uint64_t r = 0xffffffffffffffff;
	uint64_t ns; // nanoseconds passed since calling ctimer_start()
	struct timeval t1;
	gettimeofday(&t1,0);
	ns = 1e9 * (t1.tv_sec - t0.tv_sec)
		+ 1e3 * (t1.tv_usec - t0.tv_usec);
	return (r - ns); // this counts down from r
}

#endif

#ifdef __cplusplus
}
#endif

#endif
