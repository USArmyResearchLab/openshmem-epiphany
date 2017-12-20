#include <stdio.h>
#include <string.h>
#include "gups.h"
#include "coprthr_cc.h"
#include "coprthr_thread.h"

uint32_t RandomAccessHost(uint32_t* T, uint32_t N)
{
	ctimer_start();
	uint64_t t0 = ctimer();

	uint32_t kshift = 32 - N;
	uint32_t I = 1 << (N + 2);
	for (uint32_t i = 0; i < I; i++)
	{
		uint32_t a = INDEX(i);
		uint32_t k = a >> kshift;
		uint32_t t = T[k];
		uint32_t t_new = t ^ a;
		T[k] = t_new;
	}

	t0 -= ctimer();
	return ctimer_nsec(t0);
}

uint32_t RandomAccessEpiphany(uint32_t* T, uint32_t N, uint32_t C)
{
	uint32_t K = 1 << N;
	size_t Ksz = K * sizeof(uint32_t);

	// open device for threads
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32, COPRTHR_O_THREAD);
	if (dd < 0) {
		fprintf(stderr, "device open failed\n");
		return 0;
	}

	// Allocate shared DRAM
	coprthr_mem_t T_mem = coprthr_dmalloc(dd, Ksz, 0);
	coprthr_mem_t argmem = coprthr_dmalloc(dd, sizeof(my_args_t), 0);

	// Write to shared DRAM
	coprthr_dwrite(dd, T_mem, 0, T, Ksz, COPRTHR_E_WAIT);

	my_args_t* args = (my_args_t*)coprthr_memptr(argmem, 0);
	*args = (my_args_t){.N = N, .T = coprthr_memptr(T_mem,0)};

	// Select program and thread function
	coprthr_program_t prg = coprthr_cc_read_bin("./shmem_tfunc.e32", 0);
	coprthr_sym_t thr = coprthr_getsym(prg, "SHMEMRandomAccessPowerOf2");

	// launch threads
	coprthr_attr_t attr;
	coprthr_attr_init(&attr);
	coprthr_attr_setdetachstate(&attr, COPRTHR_CREATE_JOINABLE);
	coprthr_attr_setdevice(&attr, dd);
	coprthr_td_t td;
	coprthr_ncreate(C, &td, &attr, thr, (void*)&argmem);
	void* status;
	coprthr_join(td, &status);

	// read back data from memory on device
	coprthr_dread(dd, T_mem, 0, T, Ksz, COPRTHR_E_WAIT);

	// cleanup
	coprthr_attr_destroy(&attr);
	coprthr_dfree(dd, T_mem);
	coprthr_dfree(dd, argmem);

	return args->time;
}

int main(int argc, char* argv[])
{
	uint32_t time_cpu, time_e32;
	uint32_t N = NDEFAULT;
	uint32_t NC = NCORESLOG2DEFAULT;

	int i = 1;
	while (i < argc) {
		if (!strncmp(argv[i],"-n",2)) N  = atoi(argv[++i]);
		else if (!strncmp(argv[i],"-c",2)) NC = atoi(argv[++i]);
		else if (!strncmp(argv[i],"-h",2) || !strncmp(argv[i],"--help",6)) goto help;
		else {
			fprintf(stderr, "unrecognized option: %s\n",argv[i]);
			help:
			printf("Usage: %s [OPTION]...\n"
				"Parallel 32-bit on-chip implementation of Random Access measuring Giga Updates Per Second (GUPS)\n"
				"  -n           log2 of size. Default: %u (size = %u)\n"
				"  -c           log2 of number of Epiphany cores. Default: %u (cores = %u)\n"
				"  -h, --help   print this\n",
				argv[0], NDEFAULT, 1 << NDEFAULT,
				NCORESLOG2DEFAULT, 1 << NCORESLOG2DEFAULT);
			return 0;
		}
		++i;
	}
	if (NC > NCORESLOG2DEFAULT) {
		fprintf(stderr, "Try a smaller number of cores, C (%u)\n", NCORESLOG2DEFAULT);
		return 1;
	}
	uint32_t Nlimit = NDEFAULT - (NCORESLOG2DEFAULT - NC);
	if (N > Nlimit) {
		fprintf(stderr, "Try a smaller size, N (%u)\n", Nlimit);
		return 1;
	}

	uint32_t K = 1 << N;
	uint32_t I = 1 << (N + 2);
	uint32_t C = 1 << NC;

	uint32_t* T = calloc(K, sizeof(*T));
	uint32_t* Thost = calloc(K, sizeof(*Thost));
	if (!T || !Thost) {
		fprintf(stderr, "Could not allocate memory\n");
		return 1;
	}

	// initializing T
	srand(12345);
	for (int k = 0; k < K; k++) T[k] = rand();
	memcpy(Thost, T, K * sizeof(*Thost));

	time_e32 = RandomAccessEpiphany(T, N, C);
	time_cpu = RandomAccessHost(Thost, N);

	int err = 0;
	for (int k = 0; k < K; k++) if (T[k] != Thost[k]) err++;

	printf("N = %u (2^N = %u), cores = %u, err = %d (\x1b[3%dm%0.2f%%\x1b[0m)\n",
		N, K, C, err, (err*100 >= K) ? 1 : 2, (double)err*100.0/(double)K);
	printf("GUPS = %0.4f (host = %0.4f)\n", (double)I/(double)time_e32, (double)I/(double)time_cpu);

	return 0;
}
