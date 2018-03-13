ESDK = ${EPIPHANY_HOME}
ELIBS = -L $(ESDK)/tools/host/lib -le-hal -le-loader
EINCS = -I $(ESDK)/tools/host/include
ELDF = $(ESDK)/bsps/current/internal.ldf
EDEFS = -O3 -std=c99

SHMEM =../../src
SLIBS = -L$(SHMEM) -lshmem_esdk
SINCS = -I$(SHMEM)

LIBS = $(ELIBS) $(SLIBS)

.SUFFIXES: .c .x .o .elf

OBJS = main.x e_task.elf

all: $(OBJS)

run: main.x
	./$<

# Build HOST side application
.c.x:
	gcc -O3 $< -o $@ $(EINCS) $(ELIBS) -lpthread

# Build DEVICE side program
.c.elf:
	epiphany-elf-gcc $(EDEFS) -T $(ELDF) $< -o $@ $(SINCS) $(SLIBS) -le-lib -lm -ffast-math

clean:
	rm -f $(OBJS)

distclean: clean
