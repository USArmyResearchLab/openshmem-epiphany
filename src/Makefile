DEFS = -fdynamic-calls -DSHMEM_USE_CTIMER -std=c99
INCS =
LIBS =

TARGETS = libshmem.a

COPRTHR = /usr/local/browndeer/coprthr2

OBJS = shmem_ctimer.o shmem_memcpy.o shmem_quiet.o shmem_init.o \
	shmem_int_finc.o shmem_long_finc.o shmem_longlong_finc.o \
	shmem_int_inc.o shmem_long_inc.o shmem_longlong_inc.o \
	shmem_int_fadd.o shmem_long_fadd.o shmem_longlong_fadd.o \
	shmem_int_add.o shmem_long_add.o shmem_longlong_add.o \
	shmem_int_cswap.o shmem_long_cswap.o shmem_longlong_cswap.o \
	shmem_int_swap.o shmem_long_swap.o shmem_longlong_swap.o \
	shmem_float_swap.o shmem_double_swap.o \
	shmem_int_fetch.o shmem_long_fetch.o shmem_longlong_fetch.o \
	shmem_float_fetch.o shmem_double_fetch.o \
	shmem_int_set.o shmem_long_set.o shmem_longlong_set.o \
	shmem_float_set.o shmem_double_set.o \
	shmem_wait.o shmem_wait_until.o \

OBJS += shmem_short_and_to_all.o shmem_int_and_to_all.o \
	shmem_long_and_to_all.o shmem_longlong_and_to_all.o \
	shmem_float_max_to_all.o shmem_double_max_to_all.o \
	shmem_longdouble_max_to_all.o shmem_short_max_to_all.o \
	shmem_int_max_to_all.o shmem_long_max_to_all.o \
	shmem_longlong_max_to_all.o \
	shmem_float_min_to_all.o shmem_double_min_to_all.o \
	shmem_longdouble_min_to_all.o shmem_short_min_to_all.o \
	shmem_int_min_to_all.o shmem_long_min_to_all.o \
	shmem_longlong_min_to_all.o \
	shmem_short_or_to_all.o shmem_int_or_to_all.o \
	shmem_long_or_to_all.o shmem_longlong_or_to_all.o \
	shmem_complexd_prod_to_all.o shmem_complexf_prod_to_all.o \
	shmem_float_prod_to_all.o shmem_double_prod_to_all.o \
	shmem_longdouble_prod_to_all.o shmem_short_prod_to_all.o \
	shmem_int_prod_to_all.o shmem_long_prod_to_all.o \
	shmem_longlong_prod_to_all.o \
	shmem_complexd_sum_to_all.o shmem_complexf_sum_to_all.o \
	shmem_float_sum_to_all.o shmem_double_sum_to_all.o \
	shmem_longdouble_sum_to_all.o shmem_short_sum_to_all.o \
	shmem_int_sum_to_all.o shmem_long_sum_to_all.o \
	shmem_longlong_sum_to_all.o \
	shmem_short_xor_to_all.o shmem_int_xor_to_all.o \
	shmem_long_xor_to_all.o shmem_longlong_xor_to_all.o \
	shmem_broadcast32.o shmem_broadcast64.o \
	shmem_fcollect32.o shmem_fcollect64.o \
	shmem_collect32.o shmem_collect64.o \
	shmem_alltoall32.o shmem_alltoall64.o \
	shmem_alltoalls32.o shmem_alltoalls64.o \
	shmemx_memcpy_nbi.o \
	shmem_put8_nbi.o shmem_put16_nbi.o shmem_put32_nbi.o shmem_put64_nbi.o \
	shmem_put128_nbi.o \
	shmem_get8_nbi.o shmem_get16_nbi.o shmem_get32_nbi.o shmem_get64_nbi.o \
	shmem_get128_nbi.o \
	shmem_put8.o shmem_put16.o shmem_put32.o shmem_put64.o shmem_put128.o \
	shmem_get8.o shmem_get16.o shmem_get32.o shmem_get64.o shmem_get128.o \
	shmem_ipi_get.o 

OBJS += shmem_char_iput.o shmem_short_iput.o shmem_int_iput.o \
	shmem_long_iput.o shmem_longlong_iput.o \
	shmem_float_iput.o shmem_double_iput.o shmem_longdouble_iput.o \
	shmem_char_iget.o shmem_short_iget.o shmem_int_iget.o \
	shmem_long_iget.o shmem_longlong_iget.o \
	shmem_float_iget.o shmem_double_iget.o shmem_longdouble_iget.o

OBJS += shmem_lock.o shmem_barrier.o shmem_aux.o shmem_mman.o

OBJS += shmem_char_p.o shmem_short_p.o shmem_int_p.o shmem_long_p.o \
	shmem_longlong_p.o shmem_float_p.o shmem_double_p.o shmem_longdouble_p.o

all: $(TARGETS)

.PHONY: clean install uninstall

.SUFFIXES:
.SUFFIXES: .S .c .o 

.c.o:
	coprcc -c $(DEFS) $(INCS) $(LIBS) $<

libshmem.a: $(OBJS)
	ar rcv libshmem.a $(OBJS)

.S.o:
	e-gcc -c $<

clean:
	rm -f *.o

distclean: clean 
	rm -f libshmem.a

