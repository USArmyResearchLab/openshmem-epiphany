DEFS = -DECORE=16 -DNPARTICLE=4096 -DSTEPS=32 -DSIZE=1 -std=c99

CCFLAGS += -O3 $(DEFS)
EFLAGS += -ffast-math -ftree-vectorize -funroll-loops -ffp-contract=fast -mfp-mode=truncate -falign-loops=8 -falign-functions=8 -fno-stack-protector

COPRTHR = /usr/local/browndeer/coprthr2
SHMEM = ../../src

INCS = -I. -I$(COPRTHR)/include -I$(SHMEM)
LIBS = -L$(COPRTHR)/lib -lcoprthr -lcoprthrcc -lm

ELIBS = -L$(COPRTHR)/lib -lcoprthr_mpi -lcoprthr2_dev -L$(SHMEM) -lshmem_coprthr

TARGET = main.x shmem_tfunc.e32 shmem_tfunc2.e32

all: $(TARGET)

run: main.x
	./$<

.PHONY: clean install uninstall $(SUBDIRS)

.SUFFIXES:
.SUFFIXES: .c .o .x .e32

main.x: main.o
	$(CC) -o main.x main.o $(LIBS)

.c.e32:
	coprcc $(INCS) $(DEFS) $(EFLAGS) $(ELIBS) $< -o $@

.c.o:
	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o 
	rm -f $(TARGET)

distclean: clean 


