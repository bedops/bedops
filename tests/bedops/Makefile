APP = bedops
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
BIN = $(CWD)/../../bin/bedops
TMPDIR := $(shell mktemp -d)

all: 
	@echo "Testing [$(APP)]"
	@$(MAKE) operations
	@$(MAKE) clean

operations: prep merge

prep:
	@[ -f $(BIN) ] || echo "Missing [$(APP)]"
	$(BIN) --version

merge:
	@echo "[$(APP) --$@]"
	@echo "[$(APP) --$@] - [001]"
	@$(BIN) -m $(CWD)/data/001.merge.001.test > $(TMPDIR)/001.merge.001.observed
	@diff -s $(TMPDIR)/001.merge.001.observed $(CWD)/data/001.merge.001.expected

clean:
	@echo "Cleaning [$(APP)] test results..."
	@rm -rf $(TMPDIR)
