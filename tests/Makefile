CWD := $(shell pwd)
SWITCH = $(CWD)/../bin/switch-BEDOPS-binary-type
ALL_BINARY_TYPES = typical megarow float128

all: tests

.PHONY: tests bedops

tests:
	$(MAKE) bedops -C $(CWD)/tests
	$(MAKE) clean -C $(CWD)/tests

bedops: 
	for btype in ${ALL_BINARY_TYPES}; do \
		$(SWITCH) --$$btype $(BINDIR) || exit $$?; \
		$(MAKE) all -C $(CWD)/bedops BUILDTYPE=$$btype; \
	done

clean:
	$(SWITCH) --typical $(BINDIR)
	@rm -rf /tmp/tmp.*
