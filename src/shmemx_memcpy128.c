/*
 * Copyright (c) 2017 U.S. Army Research laboratory. All rights reserved.
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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This routine handles aligned memory copying of 64-bit aligned data, handled
 * in 128-bit chunks, in a high performance manner.
 * -JAR
 *
 * Register Key:
 *  r0  = dst, destination pointer
 *  r1  = src, source pointer
 *  r2  = nelem, number of elements
 *  r3  = temporary register
 *  r56 = r56-r57 used as dword src data
 *  r58 = r58-r59 used as dword src data
 *  r60 = r60-r61 used as dword src data
 *  r62 = r62-r63 used as dword src data
 *
*/

SHMEM_SCOPE void
shmemx_memcpy128(void* dst, const void* src, size_t nelem)
{ // assumes dst and src are 64-bit aligned
	__asm__ __volatile__(
	"lsr r3, %[nelem], #1             \n" // Checking number dwords >= 4
	"beq .LQremainder%=               \n"
	"gid                              \n"
	"movts lc, r3                     \n"
	"mov r3, %%low(.LDstart%=)        \n"
	"movts ls, r3                     \n"
	"mov r3, %%low(.LDend%=-4)        \n"
	"movts le, r3                     \n"
	".balignw 8,0x01a2                \n" // If alignment is correct, no need for nops
	".LDstart%=:                      \n"
	"ldrd r56, [%[src]], #1           \n"
	"ldrd r58, [%[src]], #1           \n"
	"strd r56, [%[dst]], #1           \n"
	"ldrd r60, [%[src]], #1           \n"
	"strd r58, [%[dst]], #1           \n"
	"ldrd r62, [%[src]], #1           \n"
	"strd r60, [%[dst]], #1           \n"
	"strd r62, [%[dst]], #1           \n"
	".LDend%=:                        \n"
	"gie                              \n"
	".LQremainder%=:                  \n"
	"lsl %[nelem], %[nelem], #31      \n"
	"beq .LDdone%=                    \n"
	"ldrd r56, [%[src]], #1           \n"
	"ldrd r58, [%[src]], #1           \n"
	"strd r56, [%[dst]], #1           \n"
	"strd r58, [%[dst]], #1           \n"
	".LDdone%=:                       \n"
			: [dst] "+r" (dst), [src] "+r" (src), [nelem] "+r" (nelem)
			:
			: "r3",
			  "r56", "r57", "r58", "r59",
			  "r60", "r61", "r62", "r63"
		);
}

#ifdef __cplusplus
}
#endif
