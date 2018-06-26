APP = bedops
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
BIN = $(CWD)/../../bin/bedops
TMP := $(shell mktemp -d)

all: 
	@echo "Testing binary [$(APP)] and build type [$(BUILDTYPE)]"
	@$(MAKE) operations

operations: prep merge

prep:
	@[ -f $(BIN) ] || echo "Missing binary [$(APP)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(BIN) --version

merge:
#	Test 001
	@echo "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -m $(CWD)/data/001.merge.001.test > $(TMP)/001.merge.001.observed
	@diff -s $(TMP)/001.merge.001.observed $(CWD)/data/001.merge.001.expected
#	Test 002
	@echo "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -m $(CWD)/data/001.merge.001.test $(CWD)/data/002.complement.001.expected > $(TMP)/006.merge.002.observed
	@diff -s $(TMP)/006.merge.002.observed $(CWD)/data/006.merge.002.expected
