SUBDIRS = src example test

COPRCC := $(shell command -v coprcc 2> /dev/null)
ifndef COPRCC
$(warning Building libshmem_esdk.a without COPRTHR-2 SDK!)
SUBDIRS = src
endif

.PHONY: all $(SUBDIRS) check run clean distclean

all: $(SUBDIRS)

src:
ifndef COPRCC
	$(MAKE) -C src -f Makefile.esdk
else
	$(MAKE) -C src
endif

# dependencies on libshmem
example: src
test: src

check: src
	$(MAKE) -C test check;

run: all
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d run; \
	done

clean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d clean; \
	done

distclean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d distclean; \
	done
