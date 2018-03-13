DEFS = -DEDIM=4 -DSIZE=256
EDEFS = -DSHMEM_USE_HEADER_ONLY -fno-unroll-loops
ASFLAGS = #-fPIC

CFLAGS += -O3 $(DEFS)

COPRTHR = /usr/local/browndeer/coprthr2
SHMEM = ../../src

INCS = -I. -I$(COPRTHR)/include -I$(SHMEM) -I../../common
LIBS = -L$(COPRTHR)/lib -lcoprthr -lcoprthrcc -lm

ELIBS = -lcoprthr2_dev -L$(SHMEM) -lshmem_coprthr

KERNEL = shmem_tfunc.e32

TARGETS = main.x $(KERNEL)

all: $(TARGETS)

run: main.x
	./$<

.PHONY: clean distclean

.SUFFIXES:
.SUFFIXES: .c .o .x .e32 .S

main.x: main.c
	$(CC) $(CFLAGS) $(INCS) main.c -o main.x $(LIBS)

foo.x: foo.c
	$(CC) $(CFLAGS) $(INCS) foo.c -o foo.x $(LIBS)

shmem_tfunc.e32: shmem_tfunc.c dotprod.o
	coprcc $(INCS) $(EDEFS) $(ELIBS) shmem_tfunc.c dotprod.o -o shmem_tfunc.e32

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<

.S.o:
	epiphany-elf-gcc $(ASFLAGS) -c $<

info: $(KERNEL)
	coprcc-info -j -l 100 shmem_tfunc.e32 

clean:
	rm -f *.o 
	rm -f $(TARGETS)

distclean: clean 
