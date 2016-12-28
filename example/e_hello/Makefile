ESDK = ${EPIPHANY_HOME}
ELIBS = -L $(ESDK)/tools/host/lib -le-hal -le-loader
EINCS = -I $(ESDK)/tools/host/include
ELDF = $(ESDK)/bsps/current/internal.ldf

SHMEM =../../src
SLIBS = -L$(SHMEM) -lshmem
SINCS = -I$(SHMEM)

LIBS = $(ELIBS) $(SLIBS)

.SUFFIXES: .c .x .o .elf

OBJS = main.x e_task.elf

all: $(OBJS)

# Build HOST side application
.c.x:
	gcc -O3 $< -o $@ ${EINCS} ${ELIBS} -lpthread

# Build DEVICE side program
.c.elf:
	e-gcc -O3 -T ${ELDF} $< -o $@ ${SINCS} ${SLIBS} -le-lib -lm -ffast-math

clean:
	rm -f $(OBJS)

distclean: clean
