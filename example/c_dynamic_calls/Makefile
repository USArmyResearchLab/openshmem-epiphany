DEFS = -fhost -fdynamic-calls

COPRTHR = /usr/local/browndeer/coprthr2
SHMEM = ../../src

INCS = -I. -I$(COPRTHR)/include
LIBS = -L$(COPRTHR)/lib -lcoprthr2_dev -lcoprthr_hostcall -lesyscall -le-lib
LIBS += -L$(SHMEM) -lshmem_coprthr
INCS += -I$(SHMEM)

SRC_FILES = $(wildcard *.c)
OBJ_FILES = $(SRC_FILES:.c=.o)
TARGETS = $(SRC_FILES:.c=.x)

all: $(TARGETS)

info: all $(OBJ_FILES)

.PHONY: clean install uninstall

.SUFFIXES:
.SUFFIXES: .c .o .x .S

.c.o:
	coprcc -c $(DEFS) $(INCS) $(LIBS) $<

%.x: %.c $(DEPENDS)
	coprcc $(DEFS) $(INCS) $(LIBS) $< $(DEPENDS) -o $@

%.o: %.x
	coprcc --extract $< -o $@
	coprcc-info -j -l 100 $@

.S.o:
	e-gcc -c $<

clean:
	rm -f *.o

distclean: clean 
	rm -f *.x
