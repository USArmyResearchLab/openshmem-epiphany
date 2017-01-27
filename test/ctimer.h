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

void ctimer_start(void);
unsigned int ctimer_nsec(unsigned int);

#ifdef __epiphany__ // Specific to Epiphany

#define INV_GHZ 1.66666667f // 1/0.6 GHz

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

static unsigned int inline __attribute__((__always_inline__)) 
ctimer(void)
{
	const static unsigned int r = 0xffffffff;
	unsigned int ns; // nanoseconds passed since calling ctimer_start()
	struct timeval t1;
	gettimeofday(&t1,0);
	ns = 1e9 * (t1.tv_sec - t0.tv_sec)
		+ 1e3 * (t1.tv_usec - t0.tv_usec);
	return (r - ns); // this counts down from r
}

#endif

#endif
