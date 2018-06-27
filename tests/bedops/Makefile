APP = bedops
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
BIN = $(CWD)/../../bin/bedops
TMP := $(shell mktemp -d)
DATA = $(CWD)/data

all: 
	@echo "Testing binary [$(APP)] and build type [$(BUILDTYPE)]"
	@$(MAKE) operations

operations: prep merge complement intersection difference symdiff element-of not-element-of union

prep:
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
	@$(BIN) -m $(DATA)/006.merge.006A.test $(DATA)/006.merge.006B.test $(DATA)/006.merge.006C.test > $(TMP)/006.merge.006.observed
	@diff $(TMP)/006.merge.006.observed $(DATA)/006.merge.006.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 007
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 007]"
	@$(BIN) -m $(DATA)/007.merge.007A.test $(DATA)/007.merge.007B.test > $(TMP)/007.merge.007.observed
	@diff $(TMP)/007.merge.007.observed $(DATA)/007.merge.007.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 008
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 008]"
	@$(BIN) -m $(DATA)/004.complement.004A.test $(DATA)/004.complement.004B.test $(DATA)/004.complement.004C.test > $(TMP)/008.merge.008.observed
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
	@$(BIN) -c $(DATA)/004.complement.004A.test $(DATA)/004.complement.004B.test $(DATA)/004.complement.004C.test > $(TMP)/004.complement.004.observed
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

intersection:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -i $(DATA)/001.intersection.001A.test $(DATA)/001.intersection.001B.test > $(TMP)/001.intersection.001.observed
	@diff $(TMP)/001.intersection.001.observed $(DATA)/001.intersection.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -i $(DATA)/001.intersection.001A.test $(DATA)/001.intersection.001B.test $(DATA)/002.intersection.002.test > $(TMP)/002.intersection.002.observed
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
	@$(BIN) -e $(DATA)/007.merge.007B.test $(DATA)/007.merge.007B.test > $(TMP)/001.element-of.001.observed
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
	@$(BIN) -n $(DATA)/007.merge.007B.test $(DATA)/007.merge.007B.test > $(TMP)/001.not-element-of.001.observed
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
