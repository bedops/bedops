APP = bedops
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
BIN = $(CWD)/../../bin/bedops
TMP := $(shell mktemp -d)
DATA = $(CWD)/data

all: 
	@echo "Testing binary [$(APP)] and build type [$(BUILDTYPE)]"
	@$(MAKE) operations

operations: prep merge complement intersection

prep:
	@[ -f $(BIN) ] || echo "Missing binary [$(APP)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(BIN) --version

merge:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -m $(DATA)/001.merge.001.test > $(TMP)/001.merge.001.observed
	@diff $(TMP)/001.merge.001.observed $(DATA)/001.merge.001.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -m $(DATA)/001.merge.001.test $(DATA)/001.complement.001.expected > $(TMP)/002.merge.002.observed
	@diff $(TMP)/002.merge.002.observed $(DATA)/002.merge.002.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -m $(DATA)/003.merge.003.test > $(TMP)/003.merge.003.observed
	@diff $(TMP)/003.merge.003.observed $(DATA)/003.merge.003.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -m $(DATA)/001.merge.001.test $(DATA)/003.merge.003.test > $(TMP)/004.merge.004.observed
	@diff $(TMP)/004.merge.004.observed $(DATA)/004.merge.004.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 005
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 005]"
	@$(BIN) -m $(DATA)/005.merge.005.test $(DATA)/004.merge.004.expected > $(TMP)/005.merge.005.observed
	@diff $(TMP)/005.merge.005.observed $(DATA)/005.merge.005.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 006
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 006]"
	@$(BIN) -m $(DATA)/006.merge.006A.test $(DATA)/006.merge.006B.test $(DATA)/006.merge.006C.test > $(TMP)/006.merge.006.observed
	@diff $(TMP)/006.merge.006.observed $(DATA)/006.merge.006.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 007
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 007]"
	@$(BIN) -m $(DATA)/007.merge.007A.test $(DATA)/007.merge.007B.test > $(TMP)/007.merge.007.observed
	@diff $(TMP)/007.merge.007.observed $(DATA)/007.merge.007.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 008
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 008]"
	@$(BIN) -m $(DATA)/004.complement.004A.test $(DATA)/004.complement.004B.test $(DATA)/004.complement.004C.test > $(TMP)/008.merge.008.observed
	@diff $(TMP)/008.merge.008.observed $(DATA)/008.merge.008.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"

complement:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -c $(DATA)/001.merge.001.test > $(TMP)/001.complement.001.observed
	@diff $(TMP)/001.complement.001.observed $(DATA)/001.complement.001.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -c $(DATA)/002.merge.002.expected > $(TMP)/002.complement.002.observed
	@diff $(TMP)/002.complement.002.observed $(DATA)/002.complement.002.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -c $(DATA)/005.merge.005.expected > $(TMP)/003.complement.003.observed
	@diff $(TMP)/003.complement.003.observed $(DATA)/003.complement.003.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 004
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 004]"
	@$(BIN) -c $(DATA)/004.complement.004A.test $(DATA)/004.complement.004B.test $(DATA)/004.complement.004C.test > $(TMP)/004.complement.004.observed
	@diff $(TMP)/004.complement.004.observed $(DATA)/004.complement.004.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"

intersection:
#	Test 001
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BIN) -i $(DATA)/001.intersection.001A.test $(DATA)/001.intersection.001B.test > $(TMP)/001.intersection.001.observed
	@diff $(TMP)/001.intersection.001.observed $(DATA)/001.intersection.001.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 002
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(BIN) -i $(DATA)/001.intersection.001A.test $(DATA)/001.intersection.001B.test $(DATA)/002.intersection.002.test > $(TMP)/002.intersection.002.observed
	@diff $(TMP)/002.intersection.002.observed $(DATA)/002.intersection.002.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
#	Test 003
	@printf "[$(APP)-$(BUILDTYPE) --$@] - [Test 003]"
	@$(BIN) -i $(DATA)/008.merge.008.expected $(DATA)/004.complement.004.expected > $(TMP)/003.intersection.003.observed
	@diff $(TMP)/003.intersection.003.observed $(DATA)/003.intersection.003.expected || (printf "... failed!\n" && exit 1)
	@printf "... passed!\n"
