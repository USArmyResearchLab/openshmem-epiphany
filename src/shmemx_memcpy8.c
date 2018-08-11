/*
 * Copyright (c) 2016-2018 U.S. Army Research laboratory. All rights reserved.
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
 * This routine handles aligned memory copying in a high performance manner. If
 * the src and desination arrays have byte offsets but are dword aligned at
 * some point, the routine copies the initial offset amounts, performs dword
 * copies in the middle, and then copies the remaining bytes. If the arrays are
 * misaligned, it performs a slow copy.
 * -JAR
 *
 * Register Key:
 *  r0  = dst, destination pointer
 *  r1  = src, source pointer which we copy and then use as remainder
 *  r2  = nbytes, bytes and temporary register
 *  r3  = temporary register
 *  r24 = store the src here to free up r1 for 16-bit instructions
 *  r16 = r16-r17 used as dword src data
 *  r18 = r18-r19 used as dword src data
 *  r20 = r20-r21 used as dword src data
 *  r22 = r22-r23 used as dword src data
 *
*/

SHMEM_SCOPE void
shmemx_memcpy8(void* dst, const void* src, size_t nbytes)
{
	__asm__ __volatile__(
	"mov r24, %[src]                  \n" // this saves program space at cost of one instruction
	"mov %[src], %[nbytes]            \n"
	"lsr r3, %[nbytes], #3            \n"
	"beq .LByteHandler%=              \n"
	"orr r3, %[dst], r24              \n"
	"lsl r3, r3, #29                  \n" // No bytes, just double words
	"beq .LDwordHandler%=             \n"
	"mov %[nbytes], #8                \n"
	"lsr r3, r3, #29                  \n"
	"sub %[nbytes], %[nbytes], r3     \n" // Correction for misalignment (8-r3)
	"eor r3, %[dst], r24              \n"
	"lsl r3, r3, #29                  \n" // Can the array alignment be corrected?
	"beq .LByteHandler%=              \n"
	"mov %[nbytes], %[src]            \n" // perform full misaligned copy (slow)
	".LByteHandler%=:                 \n"
	"sub %[src], %[src], %[nbytes]    \n"
	"b .LSubtractByte%=               \n"
	".LBloop%=:                       \n"
	"ldrb r3, [r24], #1               \n"
	"strb r3, [%[dst]], #1            \n"
	".LSubtractByte%=:                \n"
	"sub %[nbytes], %[nbytes], #1     \n"
	"bgte .LBloop%=                   \n"
	".LDwordHandler%=:                \n"
	"mov %[nbytes], #7                \n" // This is here for alignment and is used below
	"lsr r3, %[src], #5               \n" // Checking number dwords >= 4
	"beq .LDremainder%=               \n"
	"gid                              \n"
	"movts lc, r3                     \n"
	"mov r3, %%low(.LDstart%=)        \n"
	"movts ls, r3                     \n"
	"mov r3, %%low(.LDend%=-4)        \n"
	"movts le, r3                     \n"
	".balignw 8,0x01a2                \n" // If alignment is correct, no need for nops
	".LDstart%=:                      \n"
	"ldrd r16, [r24], #1              \n"
	"ldrd r18, [r24], #1              \n"
	"strd r16, [%[dst]], #1           \n"
	"ldrd r20, [r24], #1              \n"
	"strd r18, [%[dst]], #1           \n"
	"ldrd r22, [r24], #1              \n"
	"strd r20, [%[dst]], #1           \n"
	"strd r22, [%[dst]], #1           \n"
	".LDend%=:                        \n"
	"gie                              \n"
	".LDremainder%=:                  \n"
	"lsl r3, %[src], #27              \n"
	"lsr r3, r3, #30                  \n"
	"beq .LDdone%=                    \n"
	".LDloop%=:                       \n"
	"sub r3, r3, #1                   \n"
	"ldrd r16, [r24], #1              \n"
	"strd r16, [%[dst]], #1           \n"
	"bne .LDloop%=                    \n"
	".LDdone%=:                       \n"
	"and %[nbytes], %[src], %[nbytes] \n"
	"sub %[src], %[nbytes], #0        \n"
	"bgt .LByteHandler%=              \n"
			: [dst] "+r" (dst), [src] "+r" (src), [nbytes] "+r" (nbytes)
			:
			: "r3", "r24",
			  "r16", "r17", "r18", "r19",
			  "r20", "r21", "r22", "r23",
			  "ls", "le", "lc", "memory"
		);
}

#ifdef __cplusplus
}
#endif
