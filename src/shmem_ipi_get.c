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
#include "def_shmem_x_get.h"

#ifdef SHMEM_USE_IPI_GET

shmem_ipi_args_t shmem_ipi_args = {
	.lock = 0,
	.source = 0,
	.dest = 0,
	.nbytes = 0,
	.pe = -1,
	.complete = 0
};

void __attribute__((interrupt ("swi"))) 
__shmem_user_isr(int signum)
{
	shmem_putmem(shmem_ipi_args.dest, shmem_ipi_args.source, 
		shmem_ipi_args.nbytes, shmem_ipi_args.pe);
	volatile int* remote_complete = shmem_ptr(&(shmem_ipi_args.complete), 
		shmem_ipi_args.pe);
	*remote_complete = 1; // inform remote PE
}

void 
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

