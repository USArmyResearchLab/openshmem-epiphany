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
#include "shmem.h"

#ifdef SHMEM_USE_WAND_BARRIER

void __attribute__((aligned(8))) __shmem_wand_isr (void)
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

void __shmem_wand_barrier_init(void)
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

void __shmem_wand_barrier(void)
{
	__asm__ __volatile__ (
		"gid               \n" // disable interrupts
		"wand              \n" // wait on AND
		".balignw 8,0x01a2 \n" // nop align gie/idle pair to block
		"gie               \n" // enable interrupts
		"idle              \n" // to go sleep
	);
}

void shmem_barrier_all(void)
{
	shmem_quiet();
	__shmem_wand_barrier();
	__shmem.dma_used = 0; // reset
}

#else

void __shmem_dissemination_barrier_init(void)
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

void __shmem_dissemination_barrier(void)
{
	int c;
	for (c = 0; c < __shmem.n_pes_log2; c++)
	{
		volatile long* lock = (volatile long*)(__shmem.barrier_sync + c);
		*(__shmem.barrier_psync[c]) = 1;
		while (*lock == SHMEM_SYNC_VALUE);
		*lock = SHMEM_SYNC_VALUE;
	}
}

void shmem_barrier_all(void)
{
	shmem_quiet();
	__shmem_dissemination_barrier();
	__shmem.dma_used = 0; // reset
}

#endif

void 
__shmem_barrier_lte2(int PE_start, int logPE_stride, int PE_size, long *pSync)
{ /* Routine for PE_size <= 2. Looping over shmem_barrier() for npes = 2 may
	* not work correctly.  Solution requires using testset because only
	* sychronization stage may not be reset before subsequent call */
	if (PE_size == 1) return;
	int PE_step = 0x1 << logPE_stride;
	if (__shmem.my_pe != PE_start) PE_step *= -1;
	int to = __shmem.my_pe + PE_step;
	volatile long* lock = (volatile long*)pSync;
	__shmem_set_lock((long*)shmem_ptr((void*)lock, to));
	while (*lock == SHMEM_SYNC_VALUE);
	*lock = 0;
}

void
shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync)
{
	if (PE_size < 3) return __shmem_barrier_lte2(PE_start, logPE_stride, PE_size, pSync);
	int PE_size_stride = PE_size << logPE_stride;
	int PE_step = 0x1 << logPE_stride;
	int PE_end = PE_size_stride + PE_start;

	int c, r;
	for (c = 0, r = (1 << logPE_stride); r < PE_size_stride; c++, r <<= 1)
	{
		int to = __shmem.my_pe + r;
		if (to >= PE_end) to -= PE_size_stride;
		volatile long* lock = (volatile long*)(pSync + c);
		long * remote_lock = (long*)shmem_ptr((void*)lock, to);
		*remote_lock = 1;
		while (*lock == SHMEM_SYNC_VALUE);
		*lock = SHMEM_SYNC_VALUE;
	}
}

