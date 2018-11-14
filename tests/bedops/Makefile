APP = bedops
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
BIN = $(CWD)/../../bin/bedops
TMP := $(shell mktemp -d)
DATA = $(CWD)/data
SHELL := /bin/bash

all: 
	@echo "Testing binary [$(APP)] and build type [$(BUILDTYPE)]"
	@$(MAKE) tests

tests: bedops_prep merge complement intersection difference symdiff element-of not-element-of union partition chrom chop stdin named-pipe ec
	@echo "Removing [$(TMP)]"
	@rm -rf $(TMP)

bedops_prep:
	@[ -f $(BIN) ] || echo "Missing binary [$(APP)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(BIN) --version

merge:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -m $(DATA)/001.merge.001.test > $(TMP)/001.merge.001.observed
	@diff $(TMP)/001.merge.001.observed $(DATA)/001.merge.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -m $(DATA)/001.merge.001.test $(DATA)/001.complement.001.expected > $(TMP)/002.merge.002.observed
	@diff $(TMP)/002.merge.002.observed $(DATA)/002.merge.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -m $(DATA)/003.merge.003.test > $(TMP)/003.merge.003.observed
	@diff $(TMP)/003.merge.003.observed $(DATA)/003.merge.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -m $(DATA)/001.merge.001.test $(DATA)/003.merge.003.test > $(TMP)/004.merge.004.observed
	@diff $(TMP)/004.merge.004.observed $(DATA)/004.merge.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -m $(DATA)/005.merge.005.test $(DATA)/004.merge.004.expected > $(TMP)/005.merge.005.observed
	@diff $(TMP)/005.merge.005.observed $(DATA)/005.merge.005.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 006
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 006]"
	@$(BIN) -m $(DATA)/006.merge.006a.test $(DATA)/006.merge.006b.test $(DATA)/006.merge.006c.test > $(TMP)/006.merge.006.observed
	@diff $(TMP)/006.merge.006.observed $(DATA)/006.merge.006.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 007
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 007]"
	@$(BIN) -m $(DATA)/007.merge.007a.test $(DATA)/007.merge.007b.test > $(TMP)/007.merge.007.observed
	@diff $(TMP)/007.merge.007.observed $(DATA)/007.merge.007.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 008
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 008]"
	@$(BIN) -m $(DATA)/004.complement.004a.test $(DATA)/004.complement.004b.test $(DATA)/004.complement.004c.test > $(TMP)/008.merge.008.observed
	@diff $(TMP)/008.merge.008.observed $(DATA)/008.merge.008.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 009
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 009]"
	@$(BIN) -m $(DATA)/004.difference.004.expected $(DATA)/005.difference.005.expected > $(TMP)/009.merge.009.observed
	@diff $(TMP)/009.merge.009.observed $(DATA)/009.merge.009.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

complement:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -c $(DATA)/001.merge.001.test > $(TMP)/001.complement.001.observed
	@diff $(TMP)/001.complement.001.observed $(DATA)/001.complement.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -c $(DATA)/002.merge.002.expected > $(TMP)/002.complement.002.observed
	@diff $(TMP)/002.complement.002.observed $(DATA)/002.complement.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -c $(DATA)/005.merge.005.expected > $(TMP)/003.complement.003.observed
	@diff $(TMP)/003.complement.003.observed $(DATA)/003.complement.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -c $(DATA)/004.complement.004a.test $(DATA)/004.complement.004b.test $(DATA)/004.complement.004c.test > $(TMP)/004.complement.004.observed
	@diff $(TMP)/004.complement.004.observed $(DATA)/004.complement.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -c $(DATA)/005.complement.005.test > $(TMP)/005.complement.005.observed
	@diff $(TMP)/005.complement.005.observed $(DATA)/005.complement.005.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 006
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 006]"
	@$(BIN) -c -L $(DATA)/001.merge.001.test > $(TMP)/006.complement.006.observed
	@diff $(TMP)/006.complement.006.observed $(DATA)/006.complement.006.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 007
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 007]"
	@$(BIN) -c -L $(DATA)/002.merge.002.expected > $(TMP)/007.complement.007.observed
	@diff $(TMP)/007.complement.007.observed $(DATA)/007.complement.007.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 008
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 008]"
	@$(BIN) -c -L $(DATA)/005.merge.005.expected > $(TMP)/008.complement.008.observed
	@diff $(TMP)/008.complement.008.observed $(DATA)/008.complement.008.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 009
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 009]"
	@$(BIN) -c -L $(DATA)/004.complement.004a.test $(DATA)/004.complement.004b.test $(DATA)/004.complement.004c.test > $(TMP)/009.complement.009.observed
	@diff $(TMP)/009.complement.009.observed $(DATA)/009.complement.009.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

intersection:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -i $(DATA)/001.intersection.001a.test $(DATA)/001.intersection.001b.test > $(TMP)/001.intersection.001.observed
	@diff $(TMP)/001.intersection.001.observed $(DATA)/001.intersection.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -i $(DATA)/001.intersection.001a.test $(DATA)/001.intersection.001b.test $(DATA)/002.intersection.002.test > $(TMP)/002.intersection.002.observed
	@diff $(TMP)/002.intersection.002.observed $(DATA)/002.intersection.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -i $(DATA)/008.merge.008.expected $(DATA)/004.complement.004.expected > $(TMP)/003.intersection.003.observed
	@diff $(TMP)/003.intersection.003.observed $(DATA)/003.intersection.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -i $(DATA)/004.intersection.004a.test $(DATA)/004.intersection.004b.test $(DATA)/004.intersection.004c.test > $(TMP)/004.intersection.004.observed
	@diff $(TMP)/004.intersection.004.observed $(DATA)/004.intersection.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

difference:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -d $(DATA)/004.intersection.004a.test $(DATA)/004.intersection.004b.test $(DATA)/004.intersection.004c.test > $(TMP)/001.difference.001.observed
	@diff $(TMP)/001.difference.001.observed $(DATA)/001.difference.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -d $(DATA)/002.difference.002a.test $(DATA)/002.difference.002b.test > $(TMP)/002.difference.002.observed
	@diff $(TMP)/002.difference.002.observed $(DATA)/002.difference.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -d $(DATA)/002.difference.002b.test $(DATA)/002.difference.002a.test > $(TMP)/003.difference.003.observed
	@diff $(TMP)/003.difference.003.observed $(DATA)/003.difference.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -d $(DATA)/004.difference.004a.test $(DATA)/004.difference.004b.test > $(TMP)/004.difference.004.observed
	@diff $(TMP)/004.difference.004.observed $(DATA)/004.difference.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -d $(DATA)/004.difference.004b.test $(DATA)/004.difference.004a.test > $(TMP)/005.difference.005.observed
	@diff $(TMP)/005.difference.005.observed $(DATA)/005.difference.005.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 006
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 006]"
	@$(BIN) -d $(DATA)/004.symdiff.004.expected $(DATA)/009.merge.009.expected > $(TMP)/006.difference.006.observed
	@diff $(TMP)/006.difference.006.observed $(DATA)/006.difference.006.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 007
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 007]"
	@$(BIN) -d $(DATA)/009.merge.009.expected $(DATA)/004.symdiff.004.expected > $(TMP)/007.difference.007.observed
	@diff $(TMP)/007.difference.007.observed $(DATA)/007.difference.007.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

symdiff:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -s $(DATA)/004.intersection.004a.test $(DATA)/004.intersection.004b.test > $(TMP)/001.symdiff.001.observed
	@diff $(TMP)/001.symdiff.001.observed $(DATA)/001.symdiff.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -s $(DATA)/004.intersection.004a.test $(DATA)/004.intersection.004b.test $(DATA)/004.intersection.004c.test > $(TMP)/002.symdiff.002.observed
	@diff $(TMP)/002.symdiff.002.observed $(DATA)/002.symdiff.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -s $(DATA)/003.symdiff.003a.test $(DATA)/003.symdiff.003b.test $(DATA)/003.symdiff.003c.test $(DATA)/003.symdiff.003d.test > $(TMP)/003.symdiff.003.observed
	@diff $(TMP)/003.symdiff.003.observed $(DATA)/003.symdiff.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -s $(DATA)/004.difference.004a.test $(DATA)/004.difference.004b.test > $(TMP)/004.symdiff.004.observed
	@diff $(TMP)/004.symdiff.004.observed $(DATA)/004.symdiff.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -s $(DATA)/009.merge.009.expected $(DATA)/004.symdiff.004.expected > $(TMP)/005.symdiff.005.observed
	@diff $(TMP)/005.symdiff.005.observed $(DATA)/005.symdiff.005.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

element-of:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -e $(DATA)/007.merge.007b.test $(DATA)/007.merge.007b.test > $(TMP)/001.element-of.001.observed
	@diff $(TMP)/001.element-of.001.observed $(DATA)/001.element-of.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -e 0 $(DATA)/003.symdiff.003a.test $(DATA)/003.symdiff.003b.test > $(TMP)/002.element-of.002.observed
	@diff $(TMP)/002.element-of.002.observed $(DATA)/002.element-of.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -e 1 $(DATA)/003.not-element-of.003a.test $(DATA)/003.not-element-of.003b.test > $(TMP)/003.element-of.003.observed
	@diff $(TMP)/003.element-of.003.observed $(DATA)/003.element-of.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -e $(DATA)/004.element-of.004a.test $(DATA)/004.element-of.004b.test > $(TMP)/004.element-of.004.observed
	@diff $(TMP)/004.element-of.004.observed $(DATA)/004.element-of.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

not-element-of:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -n $(DATA)/007.merge.007b.test $(DATA)/007.merge.007b.test > $(TMP)/001.not-element-of.001.observed
	@diff $(TMP)/001.not-element-of.001.observed $(DATA)/001.not-element-of.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -n 0 $(DATA)/003.symdiff.003b.test $(DATA)/003.symdiff.003a.test > $(TMP)/002.not-element-of.002.observed
	@diff $(TMP)/002.not-element-of.002.observed $(DATA)/002.not-element-of.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -n 1 $(DATA)/003.not-element-of.003a.test $(DATA)/003.not-element-of.003b.test > $(TMP)/003.not-element-of.003.observed
	@diff $(TMP)/003.not-element-of.003.observed $(DATA)/003.not-element-of.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

union:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -u --range 100 $(DATA)/001.union.001.test > $(TMP)/001.union.001.observed
	@diff $(TMP)/001.union.001.observed $(DATA)/001.union.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -u --range -100 $(DATA)/001.union.001.expected > $(TMP)/002.union.002.observed
	@diff $(TMP)/002.union.002.observed $(DATA)/002.union.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -u --range 500 $(DATA)/003.union.003.test > $(TMP)/003.union.003.observed
	@diff $(TMP)/003.union.003.observed $(DATA)/003.union.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -u --range -5:-8 $(DATA)/004.union.004.test > $(TMP)/004.union.004.observed
	@diff $(TMP)/004.union.004.observed $(DATA)/004.union.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -u --range 0:-79 $(DATA)/004.union.004.test > $(TMP)/005.union.005.observed
	@diff $(TMP)/005.union.005.observed $(DATA)/005.union.005.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 006
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 006]"
	@$(BIN) -u --range 0:-80 $(DATA)/004.union.004.test > $(TMP)/006.union.006.observed
	@diff $(TMP)/006.union.006.observed $(DATA)/006.union.006.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 007
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 007]"
	@$(BIN) -u --range -5:5 $(DATA)/004.union.004.test > $(TMP)/007.union.007.observed
	@diff $(TMP)/007.union.007.observed $(DATA)/007.union.007.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 008
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 008]"
	@$(BIN) -u --range 5 $(DATA)/004.union.004.test > $(TMP)/008.union.008.observed
	@diff $(TMP)/008.union.008.observed $(DATA)/008.union.008.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 009
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 009]"
	@$(BIN) -u --range -5:-8 $(DATA)/009.union.009.test > $(TMP)/009.union.009.observed
	@diff $(TMP)/009.union.009.observed $(DATA)/009.union.009.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

partition:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -p $(DATA)/001.partition.001a.test $(DATA)/001.partition.001b.test $(DATA)/001.partition.001c.test >  $(TMP)/001.partition.001.observed
	@diff $(TMP)/001.partition.001.observed $(DATA)/001.partition.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -p $(DATA)/002.partition.002a.test $(DATA)/002.partition.002b.test $(DATA)/002.partition.002c.test $(DATA)/002.partition.002d.test $(DATA)/002.partition.002e.test > $(TMP)/002.partition.002.observed
	@diff $(TMP)/002.partition.002.observed $(DATA)/002.partition.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -p $(DATA)/003.partition.003.test > $(TMP)/003.partition.003.observed
	@diff $(TMP)/003.partition.003.observed $(DATA)/003.partition.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -p $(DATA)/004.partition.004.test > $(TMP)/004.partition.004.observed
	@diff $(TMP)/004.partition.004.observed $(DATA)/004.partition.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -p $(DATA)/004.partition.004.test > $(TMP)/004.partition.004.observed
	@diff $(TMP)/004.partition.004.observed $(DATA)/004.partition.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

chrom:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) --chrom chr6 -u $(DATA)/008.complement.008.expected > $(TMP)/001.chrom.001.observed
	@diff $(TMP)/001.chrom.001.observed $(DATA)/001.chrom.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) --chrom chr4 -m $(DATA)/008.complement.008.expected > $(TMP)/002.chrom.002.observed
	@diff $(TMP)/002.chrom.002.observed $(DATA)/002.chrom.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) --chrom chr7 -c $(DATA)/008.complement.008.expected > $(TMP)/003.chrom.003.observed
	@diff $(TMP)/003.chrom.003.observed $(DATA)/003.chrom.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) --ec --chrom chr7 -c -L $(DATA)/008.complement.008.expected > $(TMP)/004.chrom.004.observed
	@diff $(TMP)/004.chrom.004.observed $(DATA)/004.chrom.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

chop:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) --chop 100 $(DATA)/001.chop.001a.test $(DATA)/001.chop.001b.test > $(TMP)/001.chop.001.observed
	@diff $(TMP)/001.chop.001.observed $(DATA)/001.chop.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) --chop 100 -x $(DATA)/001.chop.001a.test $(DATA)/001.chop.001b.test > $(TMP)/002.chop.002.observed
	@diff $(TMP)/002.chop.002.observed $(DATA)/002.chop.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) --chop 100 --stagger 53 $(DATA)/001.chop.001a.test $(DATA)/001.chop.001b.test > $(TMP)/003.chop.003.observed
	@diff $(TMP)/003.chop.003.observed $(DATA)/003.chop.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) --chop 100 --stagger 53 -x $(DATA)/001.chop.001a.test $(DATA)/001.chop.001b.test > $(TMP)/004.chop.004.observed
	@diff $(TMP)/004.chop.004.observed $(DATA)/004.chop.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) --chop $(DATA)/005.chop.005.test > $(TMP)/005.chop.005.observed
	@diff $(TMP)/005.chop.005.observed $(DATA)/005.chop.005.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

stdin:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -u $(DATA)/001.chop.001a.test $(DATA)/001.chop.001b.test | $(BIN) --chop 100 --stagger 53 -x - > $(TMP)/001.stdin.001.observed
	@diff $(TMP)/001.stdin.001.observed $(DATA)/001.stdin.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

named-pipe:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) --chop 100 --stagger 53 <($(BIN) -u $(DATA)/001.chop.001a.test $(DATA)/001.chop.001b.test) > $(TMP)/001.named-pipe.001.observed
	@diff $(TMP)/001.named-pipe.001.observed $(DATA)/001.named-pipe.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

ec:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@if $(BIN) --ec --chop 20 --chrom chr1 $(DATA)/001.ec.001a.test 2> /dev/null; then printf " ...passed! (not expected to pass)\n"; exit 1; else printf " ...failed! (but expected to fail)\n"; exit 0; fi
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@if $(BIN) --ec --chop 20 --chrom chr2 $(DATA)/001.ec.001a.test 2> /dev/null; then printf " ...passed! (not expected to pass)\n"; exit 1; else printf " ...failed! (but expected to fail)\n"; exit 0; fi
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@if $(BIN) --ec --chop 20 --chrom chr1 <(sort-bed $(DATA)/001.ec.001a.test) > /dev/null 2>&1; then printf " ...passed!\n"; exit 0; else printf " ...failed!\n"; exit 1; fi
