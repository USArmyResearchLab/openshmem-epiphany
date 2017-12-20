DEFS = -DECORE=16 -DDIAGS=9 -DSIZE=4096 -std=gnu11
EDEFS = -DSHMEM_USE_HEADER_ONLY 

CCFLAGS += -O3 $(DEFS)

COPRTHR = /usr/local/browndeer/coprthr2
SHMEM = ../../src

INCS = -I. -I$(COPRTHR)/include -I$(SHMEM) -I../../common
LIBS = -L$(COPRTHR)/lib -lcoprthr -lcoprthrcc -lm

ELIBS = -L$(COPRTHR)/lib -lcoprthr2_dev -L$(SHMEM) -lshmem_coprthr

TARGET = main.x shmem_tfunc.e32

all: $(TARGET)

run: main.x
	./$<

.PHONY: clean install uninstall $(SUBDIRS)

.SUFFIXES:
.SUFFIXES: .c .o .x .e32

main.x: main.o
	$(CC) -o main.x main.o $(LIBS)

.c.e32:
	coprcc $(INCS) $(EDEFS) $(ELIBS) $< -o $@

.c.o:
	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o 
	rm -f $(TARGET)

distclean: clean 


