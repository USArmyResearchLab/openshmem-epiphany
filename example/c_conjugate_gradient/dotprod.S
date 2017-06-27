/* float dotprod(const float* a, const float* b, int n) {
	int i;
	float c = 0.0f;
	for (i = 0; i < n; i++) {
		c += a[i] * b[i];
	}
	return c;
} */

#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b
#define SYM(s) CONCAT1(__USER_LABEL_PREFIX__, s)
#define SIZE(s) .size SYM(s), .-SYM(s)

#define A  r0
#define B  r1
#define N  r2
#define s0 r24
#define s1 r25
#define s2 r26
#define s3 r27
#define s4 r44
#define s5 r45
#define s6 r46
#define s7 r47
#define a0 r48
#define a1 r49
#define b0 r50
#define b1 r51
#define a2 r52
#define a3 r53
#define b2 r54
#define b3 r55
#define a4 r56
#define a5 r57
#define b4 r58
#define b5 r59
#define a6 r60
#define a7 r61
#define b6 r62
#define b7 r63

.global SYM(dotprod)
SYM(dotprod):
	sub   N,  N  , #0   ; N = N - 0
	blte  .Lfastrts     ; fast return
#if __PIC__            // position independent code path
	movfs r3, pc
	.Lpreamble:
	add   r3, r3 , (.Lstart-.Lpreamble)
	movts ls, r3
	add   r3, r3 , (.Lend-.Lstart-4)
#else
	mov   r3, %low(.Lstart)
	movts ls, r3
	mov   r3, %low(.Lend-4)
#endif
	movts le, r3        ;  set end of hardware loop
	orr   r3, A  , B    ;  alignment = (a | b)
	lsl   r3, r3 , #29
	mov   s4, #0        ;  s4 = 0.0f
	mov   s0, #0        ;  s0 = 0.0f
	bne .Lremainder     ;  take slow path if !alignment
	lsr   r3, N  , #3   ;╮ n8 = n/8
	fsub  s1, s0 , s0   ;╯ s1 = 0.0f
	sub   r3, r3 , #1   ;╮ n8 = n8 - 1
	fsub  s2, s0 , s0   ;╯ s2 = 0.0f
	blte .Lremainder    ;  if n < 16, take slow path
	movts lc, r3        ;  hardware loop count
	.balignw 8,0x01a2   ;  ensuring loop alignment below with padded nops
	ldrd  a0, [A], #1   ;╮ {a0,a1} <= *A++
	fsub  s3, s0 , s0   ;╯ s3 = 0.0f
	ldrd  b0, [B], #1   ;╮ {b0,b1} <= *B++
	fsub  s5, s0 , s0   ;╯ s5 = 0.0f
	ldrd  a2, [A], #1   ;╮ {a2,a3} <= *A++
	fsub  s6, s0 , s0   ;╯ s6 = 0.0f
	ldrd  b2, [B], #1   ;╮ {b2,b3} <= *B++
	fsub  s7, s0 , s0   ;╯ s7 = 0.0f
	ldrd  a4, [A], #1   ;╮ {a4,a5} <= *A++
	fmadd s0, a0 , b0   ;╯ s0 += a0 * b0
	ldrd  b4, [B], #1   ;╮ {b4,b5} <= *B++
	fmadd s1, a1 , b1   ;╯ s1 += a1 * b1
	ldrd  a6, [A], #1   ;╮ {a6,a7} <= *A++
	fmadd s2, a2 , b2   ;╯ s2 += a2 * b2
	ldrd  b6, [B], #1   ;╮ {b6,b7} <= *B++
	fmadd s3, a3 , b3   ;╯ s3 += a3 * b3
	.Lstart:            ;  hardware loop start
	ldrd  a0, [A], #1   ;╮ {a0,a1} <= *A++
	fmadd s4, a4 , b4   ;╯ s4 += a4 * b4
	ldrd  b0, [B], #1   ;╮ {b0,b1} <= *B++
	fmadd s5, a5 , b5   ;╯ s5 += a5 * b5
	ldrd  a2, [A], #1   ;╮ {a2,a3} <= *A++
	fmadd s6, a6 , b6   ;╯ s6 += a6 * b6
	ldrd  b2, [B], #1   ;╮ {b2,b3} <= *B++
	fmadd s7, a7 , b7   ;╯ s7 += a7 * b7
	ldrd  a4, [A], #1   ;╮ {a4,a5} <= *A++
	fmadd s0, a0 , b0   ;╯ s0 += a0 * b0
	ldrd  b4, [B], #1   ;╮ {b4,b5} <= *B++
	fmadd s1, a1 , b1   ;╯ s1 += a1 * b1
	ldrd  a6, [A], #1   ;╮ {a6,a7} <= *A++
	fmadd s2, a2 , b2   ;╯ s2 += a2 * b2
	ldrd  b6, [B], #1   ;╮ {b6,b7} <= *B++
	fmadd s3, a3 , b3   ;╯ s3 += a3 * b3
	.Lend:              ;  hardware loop end
	fmadd s4, a4 , b4   ;  s4 += a4 * b4
	fmadd s5, a5 , b5   ;  s5 += a5 * b5
	fmadd s6, a6 , b6   ;  s6 += a6 * b6
	fmadd s7, a7 , b7   ;  s7 += a7 * b7
	fadd  s0, s0 , s1   ;  s0 += s1
	fadd  s2, s2 , s3   ;  s2 += s3
	fadd  s4, s4 , s5   ;  s4 += s5
	fadd  s6, s6 , s7   ;╮ s6 += s7
	add   r3, r3 , #1   ;╯ +1 to loop count
	fadd  s0, s0 , s2   ;╮ s0 += s2 
	lsl   r3, r3 , #3   ;╯ handle remainder
	fadd  s4, s4 , s6   ;╮ s4 += s6
	sub   N , N  , r3   ;╯ the remainder
	beq .Ldone          ;  almost done if no remainder
	.Lremainder:        ;  loop to handle remainders and unaligned arrays
	ldr   a0, [A], #1   ;  a0 = *A++
	ldr   b0, [B], #1   ;  b0 = *B++
	fmadd s4, a0 , b0   ;╮ s4 += a0 * b0
	sub   N , N  , #1   ;╯ decrement remainder
	bgt .Lremainder     ;  loop if not done
	.Ldone:             ;  no more work, final summation
	fadd  r0, s0 , s4   ;  result = s0 + s4
	rts                 ;  return
	.Lfastrts:          ;  fast return
	mov   r0, #0        ;  result = 0.0f
	rts                 ;  return
	SIZE(dotprod)
