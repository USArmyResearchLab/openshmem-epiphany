/*
 * This routine performs a distributed RandomAccess Benchmark. Instead of
 * 64-bit values, 32-bit values are used. Also, a different PRNG is used
 * because GF(2) at small indices isn't very random. It is also non-trivial to
 * calculate the n'th value.
 *
 * -JAR
 */

#include <host_stdio.h>
#include "gups.h"

#define SHMEM_USE_HEADER_ONLY

#include "shmem.h"
#include "shmemx.h"

inline __attribute__((__always_inline__))
uint32_t ctz(register uint32_t x)
{
	x &= -x;
	x -= 1;
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return (x & 0x0000003f);
}

void __entry
SHMEMRandomAccessPowerOf2( my_args_t* args )
{
	uint32_t* T = args->T;
	uint32_t N = args->N;
	uint32_t Np2 = N + 2;
	uint32_t K = 1 << N;
	uint32_t I = 1 << Np2;
	uint32_t N32 = 32 - N;

	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();

	uint32_t log2_npes = ctz(npes);
	uint32_t peshift = N - log2_npes;
	uint32_t shift = N - 32;
	uint32_t kmask0 = (0xffffffff << shift) >> shift;
	shift = 32 - peshift;
	uint32_t kmask1 = (0xffffffff << shift) >> shift;

	uint32_t npart = K >> log2_npes;
	int ipart = I >> log2_npes;
	int istart = ipart * me;
	int iend = ipart * (me + 1);
	size_t nsz = npart * sizeof(uint32_t);
	uint32_t* t = shmem_align(0x2000, nsz);
	shmemx_memcpy(t, T + npart * me, nsz);

	shmem_barrier_all();

	ctimer_start();
	uint32_t time = ctimer();

	for (int i = istart; i < iend; i++) {
		// pseudo random number 'a', can trivially calculate n'th value
		uint32_t a = INDEX(i);
		uint32_t k_global = a >> N32;
		int k = k_global & kmask1;
		int q = k_global >> peshift; // the processor number
		if (N > 11) { // The percentage of errors is high for small N...
			uint32_t* ptk = (uint32_t*)shmem_ptr(t + k, q);
			*ptk ^= a;
		} else { // ...so use atomic xor to be error-free
			shmem_uint32_atomic_xor(t + k, a, q);
		}
	}

	shmem_barrier_all();
	time -= ctimer();
	time = ctimer_nsec(time);

	// Copying local results to DRAM
	shmemx_memcpy(T + npart * me, t, nsz);
	if (!me) args->time = time;

	shmem_free(t);

	shmem_finalize();
}
