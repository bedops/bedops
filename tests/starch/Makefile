APPGROUP = starch
CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
STARCH = $(CWD)/../../bin/starch
STARCHBIN = starch
UNSTARCH = $(CWD)/../../bin/unstarch
UNSTARCHBIN = unstarch
STARCHCAT = $(CWD)/../../bin/starchcat
STARCHCATBIN = starchcat
STARCHSTRIP = $(CWD)/../../bin/starchstrip
STARCHSTRIPBIN = starchstrip
SORTBED = $(CWD)/../../bin/sort-bed
BEDMAP = $(CWD)/../../bin/bedmap
TMP := $(shell mktemp -d)
DATA = $(CWD)/data
SHELL := /bin/bash
SAMPLES = 3
MAXLENGTH = 10000
RANDOMINTERVALS = $(TMP)/random_intervals.bed

all: 
	@echo "Testing binary group [$(APPGROUP)] and build type [$(BUILDTYPE)]"
	@$(MAKE) tests

tests: unstarch starch_api starch
	@echo "Removing [$(TMP)]"
	@rm -rf $(TMP)

unstarch: unstarch_prep signature

unstarch_prep:
	@[ -f $(UNSTARCH) ] || echo "Missing binary [$(UNSTARCH)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(UNSTARCH) --version

signature:
#	Test 001
#	starch tests/bedops/data/002.difference.002a.test > tests/starch/data/001.unstarch.signature.001.test
	@printf "[$(APPGROUP)-$(UNSTARCHBIN)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(UNSTARCH) --signature $(DATA)/001.unstarch.signature.001.test > $(TMP)/001.unstarch.signature.001.observed
	@diff $(TMP)/001.unstarch.signature.001.observed $(DATA)/001.unstarch.signature.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
#	Test 002
#	starch tests/bedops/data/002.difference.002a.test > tests/starch/data/002.unstarch.signature.001.test
	@printf "[$(APPGROUP)-$(UNSTARCHBIN)-$(BUILDTYPE) --$@] - [Test 002]"
	@$(UNSTARCH) --signature $(DATA)/002.unstarch.signature.001.test > $(TMP)/002.unstarch.signature.001.observed
	@diff $(TMP)/002.unstarch.signature.001.observed $(DATA)/002.unstarch.signature.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	
starch_api: starch_api_prep starch_api_bedmap

starch_api_prep:
	@[ -f $(STARCH) ] || echo "Missing binary [$(STARCH)] for build type [$(BUILDTYPE)]"
	$(STARCH) --version
	@echo "Writing to [$(TMP)]"
	@[ -f $(BEDMAP) ] || echo "Missing binary [$(BEDMAP)] for build type [$(BUILDTYPE)]"
	$(BEDMAP) --help
	
starch_api_bedmap:
# Test 001
	@printf "[$(APPGROUP)-$(STARCHBIN)-API-$(BUILDTYPE) --$@] - [Test 001]"
	@$(BEDMAP) --echo --echo-map-id $(DATA)/001.starch_api.bedmap.001.test > $(TMP)/001.starch_api.bedmap.001.observed
	@diff $(TMP)/001.starch_api.bedmap.001.observed $(DATA)/001.starch_api.bedmap.001.expected || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

starch: starch_prep deflate_and_inflate

starch_prep:
	@[ -f $(STARCH) ] || echo "Missing binary [$(STARCH)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(STARCH) --version
	@echo "Generating random intervals..."
	./generate_random_intervals.sh $(DATA)/hg38.bed $(SAMPLES) $(MAXLENGTH) | $(SORTBED) - > $(RANDOMINTERVALS)

deflate_and_inflate: deflate_and_inflate_bzip2 deflate_and_inflate_gz

deflate_and_inflate_bzip2:
#	Test 001
	@printf "[$(APPGROUP)-$(STARCHBIN)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(STARCH) --bzip2 $(RANDOMINTERVALS) > $(TMP)/001.starch.deflate_and_inflate.bzip2.starch
	@$(UNSTARCH) $(TMP)/001.starch.deflate_and_inflate.bzip2.starch > $(TMP)/001.starch.deflate_and_inflate.bzip2.bed
	@diff $(TMP)/001.starch.deflate_and_inflate.bzip2.bed $(RANDOMINTERVALS) || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

deflate_and_inflate_gz:
#	Test 001
	@printf "[$(APPGROUP)-$(STARCHBIN)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(STARCH) --gzip $(RANDOMINTERVALS) > $(TMP)/001.starch.deflate_and_inflate.gz.starch
	@$(UNSTARCH) $(TMP)/001.starch.deflate_and_inflate.gz.starch > $(TMP)/001.starch.deflate_and_inflate.gz.bed
	@diff $(TMP)/001.starch.deflate_and_inflate.gz.bed $(RANDOMINTERVALS) || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"

starchcat: starchcat_prep starchcat_disjoint_chrs

starchcat_prep:
	@[ -f $(STARCHCAT) ] || echo "Missing binary [$(STARCHCAT)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(STARCHCAT) --version
	
starchcat_disjoint_chrs:
# Test 001
	@printf "[$(APPGROUP)-$(STARCHCATBIN)-$(BUILDTYPE) --$@] - [Test 001]"
	@$(STARCHCAT) $(DATA)/001.starchcat.disjoint_chrs.chr1.001.test $(DATA)/001.starchcat.disjoint_chrs.chr2.001.test > $(TMP)/001.starchcat.disjoint_chrs.chr1andChr2.001.observed
	@diff <($(UNSTARCH) $(TMP)/001.starchcat.disjoint_chrs.chr1andChr2.001.observed) <($(UNSTARCH) $(DATA)/001.starchcat.disjoint_chrs.chr1andChr2.001.expected) || (printf " ...failed!\n" && exit 1)
	@printf " ...passed!\n"
	
starchstrip: starchstrip_prep

starchstrip_prep:
	@[ -f $(STARCHSTRIP) ] || echo "Missing binary [$(STARCHSTRIP)] for build type [$(BUILDTYPE)]"
	@echo "Writing to [$(TMP)]"
	$(STARCHSTRIP) --version