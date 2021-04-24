APPGROUP = conversion
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
CONVERT2BED = $(CWD)/../../bin/convert2bed
CONVERT2BEDBIN = convert2bed
TMP := $(shell mktemp -d)
DATA = $(CWD)/data
SHELL := /bin/bash

all: 
	@echo "Testing binary group [$(APPGROUP)] and build type [$(BUILDTYPE)]"
	@$(MAKE) tests

tests: wig2bed
	@echo "Removing [$(TMP)]"
	@rm -rf $(TMP)

wig2bed: wig2bed_prep wig2bed_with_chr_prefix_no_header wig2bed_no_chr_prefix_no_header wig2bed_fixedStep_with_chr_prefix_no_header wig2bed_fixedStep_no_chr_prefix_no_header wig2bed_variableStep_with_chr_prefix_no_header wig2bed_variableStep_no_chr_prefix_no_header wig2bed_multipleTracks_with_chr_prefix_with_header wig2bed_multipleTracks_no_chr_prefix_with_header

wig2bed_prep:
	@[ -f $(CONVERT2BED) ] || echo "Missing binary [$(CONVERT2BED)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(CONVERT2BED) --input=wig --version

wig2bed_with_chr_prefix_no_header:
#	Test 001
#	convert2bed --input=wig < tests/conversion/data/001.wig2bed_with_chr_prefix_no_header.001.wig > tests/conversion/data/001.wig2bed_with_chr_prefix_no_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 001]"
	@$(CONVERT2BED) --input=wig < $(DATA)/001.wig2bed_with_chr_prefix_no_header.001.wig > $(TMP)/001.wig2bed_with_chr_prefix_no_header.001.observed
	@diff $(TMP)/001.wig2bed_with_chr_prefix_no_header.001.observed $(DATA)/001.wig2bed_with_chr_prefix_no_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_no_chr_prefix_no_header:
#	Test 002
#	convert2bed --input=wig < tests/conversion/data/002.wig2bed_no_chr_prefix_no_header.001.wig > tests/conversion/data/002.wig2bed_no_chr_prefix_no_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 002]"
	@$(CONVERT2BED) --input=wig < $(DATA)/002.wig2bed_no_chr_prefix_no_header.001.wig > $(TMP)/002.wig2bed_no_chr_prefix_no_header.001.observed
	@diff $(TMP)/002.wig2bed_no_chr_prefix_no_header.001.observed $(DATA)/002.wig2bed_no_chr_prefix_no_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_fixedStep_with_chr_prefix_no_header:
#	Test 003
#	convert2bed --input=wig < tests/conversion/data/003.wig2bed_fixedStep_with_chr_prefix_no_header.001.wig > tests/conversion/data/003.wig2bed_fixedStep_with_chr_prefix_no_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 003 - step = 100, span = 1 (default)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/003.wig2bed_fixedStep_with_chr_prefix_no_header.001.wig > $(TMP)/003.wig2bed_fixedStep_with_chr_prefix_no_header.001.observed
	@diff $(TMP)/003.wig2bed_fixedStep_with_chr_prefix_no_header.001.observed $(DATA)/003.wig2bed_fixedStep_with_chr_prefix_no_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 003 - step = 100, span = 5 (custom)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/003.wig2bed_fixedStep_with_chr_prefix_no_header.002.wig > $(TMP)/003.wig2bed_fixedStep_with_chr_prefix_no_header.002.observed
	@diff $(TMP)/003.wig2bed_fixedStep_with_chr_prefix_no_header.002.observed $(DATA)/003.wig2bed_fixedStep_with_chr_prefix_no_header.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_fixedStep_no_chr_prefix_no_header:
#	Test 004
#	convert2bed --input=wig < tests/conversion/data/004.wig2bed_fixedStep_no_chr_prefix_no_header.001.wig > tests/conversion/data/004.wig2bed_fixedStep_no_chr_prefix_no_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 004 - step = 100, span = 1 (default)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/004.wig2bed_fixedStep_no_chr_prefix_no_header.001.wig > $(TMP)/004.wig2bed_fixedStep_no_chr_prefix_no_header.001.observed
	@diff $(TMP)/004.wig2bed_fixedStep_no_chr_prefix_no_header.001.observed $(DATA)/004.wig2bed_fixedStep_no_chr_prefix_no_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 004 - step = 100, span = 5 (custom)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/004.wig2bed_fixedStep_no_chr_prefix_no_header.002.wig > $(TMP)/004.wig2bed_fixedStep_no_chr_prefix_no_header.002.observed
	@diff $(TMP)/004.wig2bed_fixedStep_no_chr_prefix_no_header.002.observed $(DATA)/004.wig2bed_fixedStep_no_chr_prefix_no_header.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_variableStep_with_chr_prefix_no_header:
#	Test 005
#	convert2bed --input=wig < tests/conversion/data/005.wig2bed_variableStep_with_chr_prefix_no_header.001.wig > tests/conversion/data/005.wig2bed_variableStep_with_chr_prefix_no_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 005 - variable step, span = 1 (default)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/005.wig2bed_variableStep_with_chr_prefix_no_header.001.wig > $(TMP)/005.wig2bed_variableStep_with_chr_prefix_no_header.001.observed
	@diff $(TMP)/005.wig2bed_variableStep_with_chr_prefix_no_header.001.observed $(DATA)/005.wig2bed_variableStep_with_chr_prefix_no_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 005 - variable step, span = 5 (custom)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/005.wig2bed_variableStep_with_chr_prefix_no_header.002.wig > $(TMP)/005.wig2bed_variableStep_with_chr_prefix_no_header.002.observed
	@diff $(TMP)/005.wig2bed_variableStep_with_chr_prefix_no_header.002.observed $(DATA)/005.wig2bed_variableStep_with_chr_prefix_no_header.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_variableStep_no_chr_prefix_no_header:
#	Test 006
#	convert2bed --input=wig < tests/conversion/data/006.wig2bed_variableStep_no_chr_prefix_no_header.001.wig > tests/conversion/data/006.wig2bed_variableStep_no_chr_prefix_no_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 006 - variable step, span = 1 (default)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/006.wig2bed_variableStep_no_chr_prefix_no_header.001.wig > $(TMP)/006.wig2bed_variableStep_no_chr_prefix_no_header.001.observed
	@diff $(TMP)/006.wig2bed_variableStep_no_chr_prefix_no_header.001.observed $(DATA)/006.wig2bed_variableStep_no_chr_prefix_no_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 006 - variable step, span = 5 (custom)]"
	@$(CONVERT2BED) --input=wig < $(DATA)/006.wig2bed_variableStep_no_chr_prefix_no_header.002.wig > $(TMP)/006.wig2bed_variableStep_no_chr_prefix_no_header.002.observed
	@diff $(TMP)/006.wig2bed_variableStep_no_chr_prefix_no_header.002.observed $(DATA)/006.wig2bed_variableStep_no_chr_prefix_no_header.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_multipleTracks_with_chr_prefix_with_header:
#	Test 007
#   convert2bed --input=wig < tests/conversion/data/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.wig > tests/conversion/data/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 007 - multiple tracks]"
	@$(CONVERT2BED) --input=wig < $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.wig > $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.observed
	@diff $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.observed $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 007 - multiple tracks, with --multisplit <str> option]"
	@$(CONVERT2BED) --input=wig --multisplit foo < $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.wig > $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.002.observed
	@diff $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.002.observed $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 007 - multiple tracks, preserve header]"
	@$(CONVERT2BED) --input=wig --keep-header < $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.wig > $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.003.observed
	@diff $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.003.observed $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 007 - multiple tracks, with --multisplit <str> option, preserve header]"
	@$(CONVERT2BED) --input=wig --multisplit foo --keep-header < $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.001.wig > $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.004.observed
	@diff $(TMP)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.004.observed $(DATA)/007.wig2bed_multipleTracks_with_chr_prefix_with_header.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

wig2bed_multipleTracks_no_chr_prefix_with_header:
#	Test 008
#   convert2bed --input=wig < tests/conversion/data/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.wig > tests/conversion/data/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.expected
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 008 - multiple tracks, no chr prefix]"
	@$(CONVERT2BED) --input=wig < $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.wig > $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.observed
	@diff $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.observed $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 008 - multiple tracks, no chr prefix, with --multisplit <str> option]"
	@$(CONVERT2BED) --input=wig --multisplit foo < $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.wig > $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.002.observed
	@diff $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.002.observed $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.002.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 008 - multiple tracks, no chr prefix, preserve header]"
	@$(CONVERT2BED) --input=wig --keep-header < $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.wig > $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.003.observed
	@diff $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.003.observed $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.003.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	@printf "[$(APPGROUP)-$(CONVERT2BEDBIN)-$(BUILDTYPE) | $@ ] - [Test 008 - multiple tracks, no chr prefix, with --multisplit <str> option, preserve header]"
	@$(CONVERT2BED) --input=wig --multisplit foo --keep-header < $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.001.wig > $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.004.observed
	@diff $(TMP)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.004.observed $(DATA)/008.wig2bed_multipleTracks_no_chr_prefix_with_header.004.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"