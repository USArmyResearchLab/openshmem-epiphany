#include <stdint.h>
#include "ctimer.h"

#define NDEFAULT 16 // T[] is 2^N
#define NCORESLOG2DEFAULT 4 // cores = 2^C
#define A(i) (1 + (i) + (i) * (i))

typedef struct my_args
{
	uint32_t* T;
	uint32_t N;
	uint32_t time;
} my_args_t;

inline __attribute__((__always_inline__))
uint32_t INDEX(uint32_t i) // pseudo-random index access
{
	uint32_t a = i;
	a *= 1234567891;
	a ^= a << 13;
	a ^= a >> 17;
	a ^= a << 5;
	return a;
}
