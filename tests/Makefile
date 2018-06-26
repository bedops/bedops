CWD := $(shell pwd)
SWITCH = $(CWD)/../bin/switch-BEDOPS-binary-type
ALL_BINARY_TYPES = typical megarow float128
SHELL := /bin/bash

all: tests

.PHONY: tests bedops

tests:
	$(MAKE) bedops -C $(CWD)/tests
	$(MAKE) clean -C $(CWD)/tests

bedops: 
	for btype in ${ALL_BINARY_TYPES}; do \
		(cd $(CWD)/../bin && $(SWITCH) --$$btype . && cd $(CWD)) || exit $$?; \
		$(MAKE) all -C $(CWD)/bedops BUILDTYPE=$$btype; \
	done

clean:
	(cd $(CWD)/../bin && $(SWITCH) --typical . && cd $(CWD)) || exit $$?
	@rm -rf /tmp/tmp.*
