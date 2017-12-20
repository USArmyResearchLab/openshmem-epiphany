SUBDIRS_ = $(wildcard */.)
SUBDIRS  = $(SUBDIRS_:/.=)

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
