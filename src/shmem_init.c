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

#include "shmem.h"
#include "internals.h"

#ifdef __cplusplus
extern "C" {
#endif

shmem_internals_t __shmem = { 0 };

#ifdef SHMEM_USE_WAND_BARRIER

SHMEM_SCOPE void __attribute__((aligned(8)))
__shmem_wand_isr (void)
{
	__asm__ __volatile__ (
		"gid                 \n" // disable further interrupts
		"str r0, [sp, -0x1]  \n" // push r0 on the stack
		"str r1, [sp, -0x2]  \n" // push r0 on the stack
		"movfs r0, STATUS    \n" // read STATUS register
		"mov r1, 0xFFF7      \n" // low bits of NOT WAND bit
		"movt r1, 0xFFFF     \n" // and high bits
		"and r0, r0, r1      \n" // clearing WAND bit
		"movts STATUS, r0    \n" // setting STATUS register
		"ldr r0, [sp, -0x1]  \n" // pop r0 from the stack
		"ldr r1, [sp, -0x2]  \n" // pop r1 from the stack
		"gie                 \n" // enable interrupts
		"rti                 \n" // PC = IRET, which returns to after WAND instr
		: : : "cc"
	);
	__builtin_unreachable();
}

SHMEM_SCOPE void SHMEM_INLINE
__shmem_wand_barrier_init(void)
{
	unsigned int *ivt = (unsigned int*)0x20;
	*ivt = ((((unsigned int)__shmem_wand_isr - (unsigned int)ivt) >> 1) << 8) | 0xe8; // e8 = B<*> Branch Opcode
	__asm__ __volatile__ (
		"gie             \n" // enables interrupts in ILAT register
		"mov r1, 0xFEFF  \n" // low bits of NOT IRQ mask
		"movt r1, 0xFFFF \n" // and top bits
		"movfs r0, IMASK \n" // read IMASK register
		"and r0, r0, r1  \n" // clearing WAND bit
		"movts IMASK, r0 \n" // setting IMASK register
		: : : "r0", "r1", "cc"
	);
}

#else

SHMEM_SCOPE void SHMEM_INLINE
__shmem_dissemination_barrier_init(void)
{
	int c, r;
	for (c = 0, r = 1; r < __shmem.n_pes; c++, r <<= 1)
	{
		int to = __shmem.my_pe + r;
		if (to >= __shmem.n_pes) to -= __shmem.n_pes;
		long* lock = (long*)(__shmem.barrier_sync + c);
		long* remote_lock = (long*)shmem_ptr((void*)lock, to);
		__shmem.barrier_psync[c] = remote_lock;
	}
}

#endif

#ifdef SHMEM_USE_IPI_GET

shmem_ipi_args_t shmem_ipi_args = {
	.lock = 0,
	.source = 0,
	.dest = 0,
	.nbytes = 0,
	.pe = -1,
	.complete = 0
};

SHMEM_SCOPE void __attribute__((interrupt ("swi"))) 
__shmem_user_isr(int signum)
{
	shmem_putmem(shmem_ipi_args.dest, shmem_ipi_args.source, 
		shmem_ipi_args.nbytes, shmem_ipi_args.pe);
	volatile int* remote_complete = shmem_ptr(&(shmem_ipi_args.complete), 
		shmem_ipi_args.pe);
	*remote_complete = 1; // inform remote PE
}

SHMEM_SCOPE void SHMEM_INLINE
__shmem_ipi_get_init (void)
{
	unsigned int *ivt = (unsigned int*)0x24;
	*ivt = ((((unsigned int)__shmem_user_isr - (unsigned int)ivt) >> 1) << 8) | 0xe8; // e8 = B<*> Branch Opcode
	__asm__ __volatile__ (
		"gie              \n" // enable interrupts
		"mov r1, 0xFDFF   \n" // low bits of NOT USER_INTERRUPT mask
		"movt r1, 0xFFFF  \n" // and top bits
		"movfs r0, IMASK  \n" // read IMASK register
		"and r0, r0, r1   \n" // clearing user interrupt mask bit
		"movts IMASK, r0  \n" // setting IMASK register
		: : : "r0", "r1", "cc"
	);
}

#endif

SHMEM_SCOPE void
shmem_init(void)
{
	__asm__ __volatile__ (
		"movfs %[id], COREID \n" // storing COREID register value
		: [id] "=r" (__shmem.coreid)
	);
#if defined(__coprthr_device__)
	__shmem.n_pes = coprthr_get_num_threads();
	__shmem.my_pe = coprthr_get_thread_id();
#else
	__shmem.n_pes = e_group_config.group_rows * e_group_config.group_cols;
	unsigned int coreid = __shmem.coreid - e_group_config.group_id;
	unsigned int row = (coreid >> 6) & 0x3f;
	unsigned int col = (coreid) & 0x3f;
	__shmem.my_pe = row*e_group_config.group_rows + col;
#endif
	// log2_ceil of n_pes precalculated once
	unsigned int x = __shmem.n_pes - 1;
	while (x > 0) {
		__shmem.n_pes_log2++;
		x >>= 1;
	}
	__shmem.dma_start = ((int)(&__shmem.dma_desc) << 16) | 0x8;
#ifdef SHMEM_USE_WAND_BARRIER
	__shmem_wand_barrier_init();
#else
	__shmem_dissemination_barrier_init();
#endif
#ifdef SHMEM_USE_IPI_GET
	__shmem_ipi_get_init();
#endif
	__shmem.lock_high_bits = (unsigned int)shmem_ptr(NULL, 0); // using PE 0 for all global locks
#if !defined(__coprthr_device__)
	extern char _end;
	__shmem.free_mem = (intptr_t)&_end; // This should already be double-word aligned
#endif
	__shmem.local_mem_base = (intptr_t)shmemx_sbrk(0);
	int stride = SHMEM_HEAP_START - (int)__shmem.local_mem_base;
	if (stride > 0) shmemx_sbrk(stride); // advance to SHMEM_HEAP_START address
	shmem_barrier (0, 0, __shmem.n_pes, (long*)__shmem.barrier_sync);
}

#ifdef __cplusplus
}
#endif
