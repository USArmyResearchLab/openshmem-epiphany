#SUBDIRS_ = $(wildcard */.)
#SUBDIRS  = $(SUBDIRS_:/.=)

SHMEM_ESDK = ../src/libshmem_esdk.a
ifneq ("$(wildcard $(SHMEM_ESDK))","")
SUBDIRS = e_hello
endif

COPRCC := $(shell command -v coprcc 2> /dev/null)
ifdef COPRCC
SUBDIRS += c_conjugate_gradient c_diag_spmv c_dynamic_calls c_gups c_nbody
endif

.PHONY: all $(SUBDIRS) clean distclean

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ 

run: $(SUBDIRS)
	@$(foreach x,$(SUBDIRS), \
		printf '\n======================= ' && \
		printf '%0.30s' '$x =========================' && \
		printf '==========================\n' && \
		cd $x && \
		$(MAKE) -s run 2> /dev/null && \
		cd .. && \
	) printf '\n'


clean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d clean; \
	done

distclean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d distclean; \
	done
