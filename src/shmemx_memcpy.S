/*
 * void shmemx_memcpy(unsigned char* dest, unsigned char* source, unsigned int nbytes)
 *
 * This routine handles aligned memory copying in a high performance manner. If
 * the source and desination arrays have byte offsets but are dword aligned at
 * some point, the routine copies the initial offset amounts, performs dword
 * copies in the middle, and then copies the remaining bytes. If the arrays are
 * misaligned, it performs a slow copy.
 * -JAR
 */

#define D  r0           // destination
#define R  r1           // source, which we copy and then use as remainder
#define B  r2           // bytes and temporary register
#define T  r3           // temporary register
#define T2 r54          // temporary register
#define S  r55          // store the source here to free up r1
#define S0 r56          // r56-r57 used as dword source data
#define S1 r58          // r58-r59 used as dword source data
#define S2 r60          // r60-r61 used as dword source data
#define S3 r62          // r62-r63 used as dword source data

#if __GNUC__ > 4
.global shmemx_memcpy
shmemx_memcpy:
#else // underscore fix
.global _shmemx_memcpy
_shmemx_memcpy:
#endif
	mov S, R             // this saves program space at cost of one instruction
	mov R, B
	sub T, B, #15
	blte .LByteHandler
	mov B, #7
	orr T, D, S
	and T, T, B          // misalignment = ( D | S ) & 7
	beq .LDwordHandler
	mov B, #8
	sub B, B, T          // Correction for alignment
	add T, D, B
	add T2, S, B
	orr T, T, T2
	lsl T, T, #29        // Can the array alignment be corrected?
	beq .LByteHandler
	mov B, R             // perform full slow copy

	.LByteHandler:
	sub R, R, B
	sub B, B, #0
	blte .LDwordHandler
	.LBloop:
	ldrb S0, [S], #1
	strb S0, [D], #1
	sub B, B, #1
	bne .LBloop

	.LDwordHandler:
	mov B, #7
	and B, R, B
	lsr T, R, #5         // Checking number dwords >= 4
	beq .LDremainder
	ldrd S0, [S], #1
	ldrd S1, [S], #1
	ldrd S2, [S], #1
	ldrd S3, [S], #1
	sub T, T, #1         // subtract 1 for outer loop
	beq .LDfooter
	gid
	movts lc, T
	mov T, %low(.LDstart)
	movts ls, T
	mov T, %low(.LDend-4)
	movts le, T
	.balignw 8,0x01a2
	.LDstart:
	strd.l S0, [D], #1
	ldrd.l S0, [S], #1
	strd.l S1, [D], #1
	ldrd.l S1, [S], #1
	strd.l S2, [D], #1
	ldrd.l S2, [S], #1
	strd.l S3, [D], #1
	ldrd.l S3, [S], #1
	.LDend:
	gie
	.LDfooter:
	strd S0, [D], #1
	strd S1, [D], #1
	strd S2, [D], #1
	strd S3, [D], #1

	.LDremainder:
	lsl T, R, #27
	lsr T, T, #30
	beq .LDdone
	.LDloop:
	ldrd S0, [S], #1
	strd S0, [D], #1
	sub T, T, #1
	bne .LDloop

	.LDdone:
	sub R, B, #0
	bgt .LByteHandler

	.LReturn:
	rts
