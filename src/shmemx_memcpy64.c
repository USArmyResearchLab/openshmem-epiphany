/*
 * Copyright (c) 2017-2018 U.S. Army Research laboratory. All rights reserved.
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
 * This routine handles aligned memory copying of 64-bit aligned data in a high
 * performance manner.
 * -JAR
 *
 * Register Key:
 *  r0  = dst, destination pointer
 *  r1  = src, source pointer
 *  r2  = nelem, number of elements
 *  r3  = temporary register
 *  r16 = r16-r17 used as dword src data
 *  r18 = r18-r19 used as dword src data
 *  r20 = r20-r21 used as dword src data
 *  r22 = r22-r23 used as dword src data
 *
*/

SHMEM_SCOPE void
shmemx_memcpy64(void* dst, const void* src, size_t nelem)
{ // assumes dst and src are 64-bit aligned
	__asm__ __volatile__(
	"lsr r3, %[nelem], #2             \n" // Checking number dwords >= 4
	"beq .LDremainder%=               \n"
	"gid                              \n"
	"movts lc, r3                     \n"
	"mov r3, %%low(.LDstart%=)        \n"
	"movts ls, r3                     \n"
	"mov r3, %%low(.LDend%=-4)        \n"
	"movts le, r3                     \n"
	".balignw 8,0x01a2                \n" // If alignment is correct, no need for nops
	".LDstart%=:                      \n"
	"ldrd r16, [%[src]], #1           \n"
	"ldrd r18, [%[src]], #1           \n"
	"strd r16, [%[dst]], #1           \n"
	"ldrd r20, [%[src]], #1           \n"
	"strd r18, [%[dst]], #1           \n"
	"ldrd r22, [%[src]], #1           \n"
	"strd r20, [%[dst]], #1           \n"
	"strd r22, [%[dst]], #1           \n"
	".LDend%=:                        \n"
	"gie                              \n"
	".LDremainder%=:                  \n"
	"lsl %[nelem], %[nelem], #30      \n"
	"lsr %[nelem], %[nelem], #30      \n"
	"beq .LDdone%=                    \n"
	".LDloop%=:                       \n"
	"sub %[nelem], %[nelem], #1       \n"
	"ldrd r16, [%[src]], #1           \n"
	"strd r16, [%[dst]], #1           \n"
	"bne .LDloop%=                    \n"
	".LDdone%=:                       \n"
			: [dst] "+r" (dst), [src] "+r" (src), [nelem] "+r" (nelem)
			:
			: "r3",
			  "r16", "r17", "r18", "r19",
			  "r20", "r21", "r22", "r23",
			  "ls", "le", "lc", "memory"
		);
}

#ifdef __cplusplus
}
#endif
