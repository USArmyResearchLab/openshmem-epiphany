CC = coprcc
AR = epiphany-elf-ar
READELF = epiphany-elf-readelf
CFLAGS = -Wall -fno-unroll-loops -mfp-mode=caller
DEFS =
INCS =
LIBS =

TARGETS = libshmem_coprthr.a libshmem.a

SRC_FILES = $(wildcard *.c)
ASM_FILES = $(wildcard *.S)

OBJS = $(SRC_FILES:.c=.o) $(ASM_FILES:.S=.o)

all: $(TARGETS)

run:

info: $(TARGETS)
	$(READELF) -s --wide libshmem_coprthr.a | \
	awk '/FUNC|OBJECT/{printf "%7s %-6s %s\n",$$3,$$4,$$8;sum+=$$3}; \
	END{printf "===================\n%7s TOTAL BYTES\n", sum}'

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
