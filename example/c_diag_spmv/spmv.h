#ifndef _spmv_h
#define _spmv_h
#define fma(x,y,z) __builtin_fmaf(x,y,z)

typedef struct {
	float* A;
	float* xp;
	float* y;
	int N;
	int d;
} my_args_t;


#endif
