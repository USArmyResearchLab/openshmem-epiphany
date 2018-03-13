CC = coprcc
AR = epiphany-elf-ar
CFLAGS = -Wall -Wno-unused-function
DEFS = -fno-unroll-loops -mfp-mode=caller
INCS =
LIBS =

TARGETS = libshmem_coprthr.a libshmem.a

SRC_FILES = $(wildcard *.c)
ASM_FILES = $(wildcard *.S)

OBJS = $(SRC_FILES:.c=.o) $(ASM_FILES:.S=.o)

all: $(TARGETS)

run:

.PHONY: clean install uninstall

.SUFFIXES:
.SUFFIXES: .S .c .o

.c.o:
	$(CC) $(CFLAGS) $(DEFS) $(INCS) $(LIBS) -c $<

libshmem_coprthr.a: $(OBJS)
	$(AR) rcv $@ $(OBJS)

libshmem.a: libshmem_coprthr.a
	ln -sf $< $@

.S.o:
	epiphany-elf-gcc -c $<

clean:
	rm -f *.elf *.o

distclean: clean 
	rm -f *.a
