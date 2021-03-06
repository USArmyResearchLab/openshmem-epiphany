CFLAGS = -O3 -Wall -Wno-unused-function
CFLAGS += -fhost -fno-unroll-loops -ffast-math -ftree-vectorize -falign-loops=8 -falign-functions=8 -fno-stack-protector
#CFLAGS += -k

CORES = 16
NP = $(shell seq 1 $(CORES))

COPRTHR = /usr/local/browndeer/coprthr2

INCS = -I. -I$(COPRTHR)/include
LIBS = -L$(COPRTHR)/lib -lcoprthr2_dev -lcoprthr_hostcall -lesyscall
LIBS += -L../src -lshmem
DEFS += -DSHMEM_USE_HEADER_ONLY # Uncomment and comment out preceeding line to use header only library
DEFS += -DSHMEM_USE_HEAP_START=0x2000
INCS += -I../src -I../common

#DEFS += -DNLOOP=4

FIGURES = ./figures/
SRC_FILES = $(wildcard *.c)
PLOT_FILES = $(notdir $(wildcard $(FIGURES)*.gnuplot))
FIGS = $(PLOT_FILES:.gnuplot=.pdf)

DEPENDS = 

TARGETS = \
	alltoall64.x atomic_add.x atomic_compare_swap_eq.x \
	atomic_compare_swap_neq.x atomic_fetch_add.x atomic_fetch.x \
	atomic_fetch_inc.x atomic_inc.x atomic_set.x atomic_swap.x barrier.x \
	broadcast32.x broadcast64.x collect32.x collect64.x fcollect32.x \
	fcollect64.x get.x get_ipi.x get_nb.x get_nb_dual.x hello.x put.x \
	put32.x put64.x put_nb.x put_nb_dual.x reduce.x

OBJ_FILES = $(DEPENDS) $(SRC_FILES:.c=.o)

all: $(TARGETS)

figures: $(FIGS)

info: all $(OBJ_FILES)

report: report.pdf

.PHONY: run test check clean install uninstall

.SUFFIXES:
.SUFFIXES: .c .o .x .S .dat .pdf .tex
#.PRECIOUS: %.dat

.c.o:
	coprcc -c $(CFLAGS) $(DEFS) $(INCS) $(LIBS) $<

%.x: %.c $(DEPENDS)
	coprcc $(CFLAGS) $(DEFS) $(INCS) $(LIBS) $< $(DEPENDS) -o $@

.S.o:
	epiphany-elf-gcc -c $<

%.o: %.x
	coprcc --extract $< -o $@
	coprcc-info -j -l 100 $@

.x.dat:
	coprsh -r err -np $(CORES) ./$< 1> $@

version.dat: hello.x
	./$< | head -n 1 > $@

git.dat:
	echo -n "git commit: " > $@
	git rev-parse HEAD >> $@

pes.dat:
	echo -n $(CORES) > $@

%.pdf: %.dat $(FIGURES)%.gnuplot
	gnuplot -e "filename='$@'; set loadpath '$(FIGURES)'; load '$*.gnuplot'"

atomics.pdf: atomic_add.dat atomic_compare_swap_neq.dat atomic_compare_swap_eq.dat atomic_swap.dat atomic_fetch_inc.dat atomic_inc.dat atomic_fetch_add.dat atomic_fetch.dat atomic_set.dat $(FIGURES)atomics.gnuplot
	gnuplot -e "filename='$@'; set loadpath '$(FIGURES)'; load 'atomics.gnuplot'"

report.pdf: figures version.dat git.dat pes.dat report.tex
	pdflatex report.tex
	pdflatex report.tex # re-running to get indexes right
	pdflatex report.tex # re-running to get indexes right

run: $(TARGETS)
	@$(foreach x,$(TARGETS),coprsh -np $(CORES) ./$(x) 2> /dev/null &&) echo "Complete"

test: check
check: $(TARGETS)
	@$(foreach x,$(TARGETS), \
		$(foreach n,$(NP), \
			printf "%-5d%-27.27s " $(n) "$(x) .........................." && coprsh -np $(n) ./$(x) 2> /dev/null | \
				grep -i ERROR &> /dev/null && \
					echo -e "[\\033[1;31mFAIL\\033[0m]" || echo -e "[\\033[1;32mpass\\033[0m]" && \
		) \
	) \
	echo "Complete"

clean:
	rm -f *.aux
	rm -f *.log
	rm -f *.o
	rm -f *.dat
	rm -f *.bin
	rm -f *.out

distclean: clean
	rm -f *.x
	rm -f *.pdf
