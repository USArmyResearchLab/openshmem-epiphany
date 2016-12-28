/*
Copyright (C) 2013 Adapteva, Inc.
Contributed by Andreas Olofsson <support@adapteva.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program, see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.
*/

/*

This function calculates the local sum of product of two vectors located in the
Epiphany local memory. It then performs a non-trivial global summation
reduction operation and stores the final sum of products in 'c' for every core.

This serial task (with N = n*CORES) ...
for (i=0; i<N; i++) *c += a[i] * b[i];

.. may be distributed across #CORES by splitting the work ...
for (i=0; i<n; i++) *c += a[i] * b[i];

... and collecting the final summation of 'c' ...
shmem_int_sum_to_all(c, c, 1, 0, 0, CORES, pWrk, pSync);

*/

#include <shmem.h>

// syncrhonization and work arrays defined per OpenSHMEM specification
long pSync[SHMEM_REDUCE_SYNC_SIZE] = { SHMEM_SYNC_VALUE };
int pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];

int main(void)
{
	int* c = (int*) 0x6000; // address of 'c' result
	int* d = (int*) 0x7000; // done flag
	int* h = (int*) 0x7004; // host value of 'n'

	shmem_init(); // Initiailize SHMEM
	int my_pe = shmem_my_pe(); // The number of this processing element (core)
	int n_pes = shmem_n_pes(); // The total number of processing elements
	int n = *h; // copying the host-value passed in at address 'h'.

	int* a = (int*) shmem_malloc(n*sizeof(int)); // allocate 'a' vector
	int* b = (int*) shmem_malloc(n*sizeof(int)); // allocate 'b' vector

	// Initialize data
	for (int i = 0; i < n; i++) {
		a[i] = n*my_pe + i;
		b[i] = n*my_pe + i;
	}

	// Sum of product calculation
	int tmpc = 0;
	for (int i = 0; i < n; i++) {
		tmpc += a[i] * b[i];
	}
	*c = tmpc;

	// Perform global sum reduction operation. Each core receives result
	shmem_int_sum_to_all(c, c, 1, 0, 0, n_pes, pWrk, pSync);

	// Raising "done" flag
	(*(d)) = 0x00000001;

	shmem_free(b);
	shmem_free(a);
	shmem_finalize();

	// Put core in idle state
	__asm__ __volatile__("idle");
}
