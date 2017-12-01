DEFS =
EDEFS = -fno-unroll-loops

CFLAGS += -O3 -std=c11 -fopenmp $(DEFS)

COPRTHR = /usr/local/browndeer/coprthr2
SHMEM = ../../src

INCS = -I. -I$(COPRTHR)/include -I$(SHMEM) -I../../common
LIBS = -L$(COPRTHR)/lib -lcoprthr -lcoprthrcc -lm

ELIBS = -lcoprthr2_dev -L$(SHMEM) -lshmem_coprthr

KERNEL = shmem_tfunc.e32

TARGETS = main.x $(KERNEL)

all: $(TARGETS)

.PHONY: clean distclean

.SUFFIXES:
.SUFFIXES: .c .o .x .e32 .S

main.x: main.c
	$(CC) $(CFLAGS) $(INCS) main.c -o main.x $(LIBS)

shmem_tfunc.e32: shmem_tfunc.c
	coprcc $(INCS) $(EDEFS) $(ELIBS) shmem_tfunc.c -o shmem_tfunc.e32

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<

info: $(KERNEL)
	coprcc-info -j -l 100 shmem_tfunc.e32 

clean:
	rm -f *.o 
	rm -f $(TARGETS)

distclean: clean 
