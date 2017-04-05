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
 *  r54 = temporary register
 *  r55 = store the src here to free up r1 for 16-bit instructions
 *  r56 = r56-r57 used as dword src data
 *  r58 = r58-r59 used as dword src data
 *  r60 = r60-r61 used as dword src data
 *  r62 = r62-r63 used as dword src data
 *
*/
#include "internals.h"
#include "shmem.h"

SHMEM_SCOPE void
shmemx_memcpy(void* dest, const void* source, size_t nbytes)
{
	__asm__ __volatile__(
	"mov r55, %[src]                  \n" // this saves program space at cost of one instruction
	"mov %[src], %[nbytes]            \n"
	"sub r3, %[nbytes], #15           \n"
	"blte .LByteHandler%=             \n"
	"mov %[nbytes], #7                \n"
	"orr r3, %[dst], r55              \n"
	"and r3, r3, %[nbytes]            \n" // misalignment = ( %[dst] | r55 ) & 7
	"beq .LDwordHandler%=             \n"
	"mov %[nbytes], #8                \n"
	"sub %[nbytes], %[nbytes], r3     \n" // Correction for alignment
	"add r3, %[dst], %[nbytes]        \n"
	"add r54, r55, %[nbytes]          \n"
	"orr r3, r3, r54                  \n"
	"lsl r3, r3, #29                  \n" // Can the array alignment be corrected?
	"beq .LByteHandler%=              \n"
	"mov %[nbytes], %[src]            \n" // perform full misaligned copy (slow)
	".LByteHandler%=:                 \n"
	"sub %[src], %[src], %[nbytes]    \n"
	"sub %[nbytes], %[nbytes], #0     \n"
	"blte .LDwordHandler%=            \n"
	".LBloop%=:                       \n"
	"ldrb r56, [r55], #1              \n"
	"strb r56, [%[dst]], #1           \n"
	"sub %[nbytes], %[nbytes], #1     \n"
	"bne .LBloop%=                    \n"
	".LDwordHandler%=:                \n"
	"mov %[nbytes], #7                \n"
	"and %[nbytes], %[src], %[nbytes] \n"
	"lsr r3, %[src], #5               \n" // Checking number dwords >= 4
	"beq .LDremainder%=               \n"
	"ldrd r56, [r55], #1              \n"
	"ldrd r58, [r55], #1              \n"
	"ldrd r60, [r55], #1              \n"
	"ldrd r62, [r55], #1              \n"
	"sub r3, r3, #1                   \n" // subtract 1 for outer loop
	"beq .LDfooter%=                  \n"
	"gid                              \n"
	"movts lc, r3                     \n"
	"mov r3, %%low(.LDstart%=)        \n"
	"movts ls, r3                     \n"
	"mov r3, %%low(.LDend%=-4)        \n"
	"movts le, r3                     \n"
	".balignw 8,0x01a2                \n"
	".LDstart%=:                      \n"
	"strd.l r56, [%[dst]], #1         \n"
	"ldrd.l r56, [r55], #1            \n"
	"strd.l r58, [%[dst]], #1         \n"
	"ldrd.l r58, [r55], #1            \n"
	"strd.l r60, [%[dst]], #1         \n"
	"ldrd.l r60, [r55], #1            \n"
	"strd.l r62, [%[dst]], #1         \n"
	"ldrd.l r62, [r55], #1            \n"
	".LDend%=:                        \n"
	"gie                              \n"
	".LDfooter%=:                     \n"
	"strd r56, [%[dst]], #1           \n"
	"strd r58, [%[dst]], #1           \n"
	"strd r60, [%[dst]], #1           \n"
	"strd r62, [%[dst]], #1           \n"
	".LDremainder%=:                  \n"
	"lsl r3, %[src], #27              \n"
	"lsr r3, r3, #30                  \n"
	"beq .LDdone%=                    \n"
	".LDloop%=:                       \n"
	"ldrd r56, [r55], #1              \n"
	"strd r56, [%[dst]], #1           \n"
	"sub r3, r3, #1                   \n"
	"bne .LDloop%=                    \n"
	".LDdone%=:                       \n"
	"sub %[src], %[nbytes], #0        \n"
	"bgt .LByteHandler%=              \n"
			: [dst] "+r" (dest), [src] "+r" (source)
			: [nbytes] "r" (nbytes)
			: "r3", "r54", "r55",
			  "r56", "r57", "r58", "r59",
			  "r60", "r61", "r62", "r63",
			  "ls", "le", "lc", "memory"
		);
}
