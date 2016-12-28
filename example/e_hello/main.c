/*

Copyright (C) 2012 Adapteva, Inc.
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

Epiphany SDK + ARL OpenSHMEM for Epiphany
-------------------------------------------------------------------------------
This example demonstrates how to use Adapteva's host interface to Epiphany with
the device-side ARL OpenSHMEM for Epiphany interface. The device code
distributes a vector task of size N to #CORES. Each core works on a vector unit
of size N/CORES. The shmem_init, shmem_my_pe, shmem_malloc, shmem_free, and
shmem_finalize routines are used to demonstrate setup, memory allocation and
release.  The shmem_int_sum_to_all interface is used to perform a non-trivial
parallel collective sum operation. The host program accepts one command line
argument for the global size, N, which should be a multiple of #CORES.

-JAR

*/

#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>

#define CORES 16
#define NDEFAULT 64
#define NMAX 1024 // greater than this causes integer overflow 

int main(int argc, char *argv[])
{
	e_platform_t platform;
	e_epiphany_t dev;

	int c[CORES];
	int done[CORES],all_done;
	int sop;
	int i,j,n,N = NDEFAULT;
	if (argc > 1) N = atoi(argv[1]);
	if (N > NMAX) N = NMAX;
	if (N < CORES) N = CORES;
	n = N/CORES; // assumes N % CORES = 0
	N = n*CORES; // but we correct anyway
	int result = N*(N-1)*(2*N-1)/6;
	unsigned clr = 0;

	// Calculation being done
	printf("Calculating sum of products of two integer vectors of length ");
	printf("%d initalized from (0..%d) using %d Cores.\n",N,N-1,CORES);

	// Initalize Epiphany device
	e_init(NULL);
	e_reset_system(); // reset Epiphany
	e_get_platform_info(&platform);
	e_open(&dev, 0, 0, platform.rows, platform.cols); // open all cores

	// Load program to cores
	e_load_group("e_task.elf", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);

	// Clear the "done" flag for every core and set "n"
	for (i=0; i<platform.rows; i++) {
		for (j=0; j<platform.cols;j++) {
			e_write(&dev, i, j, 0x7000, &clr, sizeof(clr));
			e_write(&dev, i, j, 0x7004, &n, sizeof(n));
		}
	}

	// start cores
	e_start_group(&dev);

	// Check if all cores are done
	do {
		all_done = 0;
		for (i=0; i<platform.rows; i++) {
			for (j=0; j<platform.cols;j++) {
				e_read(&dev, i, j, 0x7000, &done[i*platform.cols+j], sizeof(int));
				all_done += done[i*platform.cols+j];
			}
		}
	} while (all_done != CORES);

	// Copy all Epiphany results to host memory space
	for (i=0; i<platform.rows; i++) {
		for (j=0; j<platform.cols;j++) {
			e_read(&dev, i, j, 0x6000, &c[i*platform.cols+j], sizeof(int));
		}
	}
	printf("Sum of Product Is %d!\n", c[0]);

	// Close down Epiphany device
	e_close(&dev);
	e_finalize();

	for (i=0; i<platform.rows; i++) {
		for (j=0; j<platform.cols;j++) {
			sop = c[i*platform.cols+j];
			if (sop != result) {
				printf("core %d reports bad result (%d)\n", i*platform.cols+j, sop);
				//return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}
