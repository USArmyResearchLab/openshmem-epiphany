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

#define HOT __attribute__((hot))
//#define HOT

void HOT
shmemx_memcpy(unsigned char *pdst, unsigned char *psrc, unsigned int nbytes)
{
	if (nbytes == 0) return;
	int r = nbytes & 0x7; // byte remainder
	int alignment = (((unsigned int) pdst) | ((unsigned int) psrc)) & 0x7;
	if (!alignment) { // aligned
		__asm__ __volatile__(
			"lsr r53, %[nbytes], #5    \n" // >>3 for DWORDs, >>2 for 4x unrolled
			"beq .Ldone%=              \n"
			"ldrd r56, [%[psrc]], #1   \n"
			"ldrd r58, [%[psrc]], #1   \n"
			"ldrd r60, [%[psrc]], #1   \n"
			"ldrd r62, [%[psrc]], #1   \n"
			"sub r53, r53, #1          \n" // subtract 1 for outer loop
			"beq .Lfooter%=            \n"
			"mov r54, %%low(.Lstart%=) \n"
			"mov r55, %%low(.Lend%=)   \n"
			"movts ls, r54             \n"
			"movts le, r55             \n"
			"movts lc, r53             \n"
			"gid                       \n"
			".balignw 8,0x01a2         \n"
			".Lstart%=:                \n"
			"strd.l r56, [%[pdst]], #1 \n"
			"ldrd.l r56, [%[psrc]], #1 \n"
			"strd.l r58, [%[pdst]], #1 \n"
			"ldrd.l r58, [%[psrc]], #1 \n"
			"strd.l r60, [%[pdst]], #1 \n"
			"ldrd.l r60, [%[psrc]], #1 \n"
			"strd.l r62, [%[pdst]], #1 \n"
			".Lend%=:                  \n"
			"ldrd.l r62, [%[psrc]], #1 \n"
			"gie                       \n"
			".Lfooter%=:               \n"
			"strd r56, [%[pdst]], #1   \n"
			"strd r58, [%[pdst]], #1   \n"
			"strd r60, [%[pdst]], #1   \n"
			"strd r62, [%[pdst]], #1   \n"
			".Ldone%=:                 \n"
			: [pdst] "+r" (pdst), [psrc] "+r" (psrc)
			: [nbytes] "r" (nbytes)
			: "r53", "r54", "r55",
			  "r56", "r57", "r58", "r59",
			  "r60", "r61", "r62", "r63",
			  "ls", "le", "lc", "memory"
		);
		int dr = (nbytes << 27) >> 30; // DWORD remainder
		switch (dr) {
			case 3: __asm__ __volatile__("ldrd r56, [%[psrc]], #1\nstrd r56, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r56", "r57", "memory");
			case 2: __asm__ __volatile__("ldrd r58, [%[psrc]], #1\nstrd r58, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r58", "r59", "memory");
			case 1: __asm__ __volatile__("ldrd r60, [%[psrc]], #1\nstrd r60, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r60", "r61", "memory");
		}
	}
	else {
		__asm__ __volatile__(
			"lsr r55, %[nbytes], #2    \n" // >>2 for 4x unrolled
			"beq .Ldone%=              \n"
			"mov r58, %%low(.Lstart%=) \n"
			"mov r59, %%low(.Lend%=)   \n"
			"movts ls, r58             \n"
			"movts le, r59             \n"
			"movts lc, r55             \n"
			"gid                       \n"
			".balignw 8,0x01a2         \n"
			".Lstart%=:                \n"
			"ldrb.l r56, [%[psrc]], #1 \n"
			"ldrb.l r57, [%[psrc]], #1 \n"
			"ldrb.l r58, [%[psrc]], #1 \n"
			"ldrb.l r59, [%[psrc]], #1 \n"
			"strb.l r56, [%[pdst]], #1 \n"
			"strb.l r57, [%[pdst]], #1 \n"
			"strb.l r58, [%[pdst]], #1 \n"
			".Lend%=:                  \n"
			"strb.l r59, [%[pdst]], #1 \n"
			"gie                       \n"
			".Ldone%=:                 \n"
			: [pdst] "+r" (pdst), [psrc] "+r" (psrc)
			: [nbytes] "r" (nbytes)
			: "r55", "r56", "r57", "r58", "r59",
			  "ls", "le", "lc", "memory"
		);
	}
	switch (r) {
		case 7: __asm__ __volatile__("ldrb r56, [%[psrc]], #1\nstrb r56, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r56", "memory");
		case 6: __asm__ __volatile__("ldrb r57, [%[psrc]], #1\nstrb r57, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r57", "memory");
		case 5: __asm__ __volatile__("ldrb r58, [%[psrc]], #1\nstrb r58, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r58", "memory");
		case 4: __asm__ __volatile__("ldrb r59, [%[psrc]], #1\nstrb r59, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r59", "memory");
		case 3: __asm__ __volatile__("ldrb r56, [%[psrc]], #1\nstrb r56, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r56", "memory");
		case 2: __asm__ __volatile__("ldrb r57, [%[psrc]], #1\nstrb r57, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r57", "memory");
		case 1: __asm__ __volatile__("ldrb r58, [%[psrc]], #1\nstrb r58, [%[pdst]], #1" : [pdst] "+r" (pdst), [psrc] "+r" (psrc) : : "r58", "memory");
	}
}

