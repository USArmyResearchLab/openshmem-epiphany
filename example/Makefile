SUBDIRS := $(wildcard */.)

.PHONY: all $(SUBDIRS) clean distclean

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ 

clean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d clean; \
	done

distclean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d distclean; \
	done
