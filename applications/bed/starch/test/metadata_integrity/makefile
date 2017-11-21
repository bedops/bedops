#
# makefile
#

TMP            = /tmp
USER          := $(shell whoami)
THIS_DIR      := $(shell pwd)
RESULTS        = ${TMP}/${USER}/starch_regression_test/results/metadata_integrity
SRC            = $(THIS_DIR)/src
COMPEXT_DIR    = $(THIS_DIR)/../compression_and_extraction
COMPEXTRES_DIR = ${TMP}/${USER}/starch_regression_test/results/compression_and_extraction
ORIGINAL_DIR   = $(THIS_DIR)/../original_data
ORIGINAL_DATA  = ${TMP}/${USER}/starch_regression_test
BED_INPUT      = $(ORIGINAL_DATA)/data/random.bed
PLATFORM       = $(shell uname)
BIN_DIR        = $(THIS_DIR)/../binaries
LINUX_BINDIR   = $(BIN_DIR)/linux
OSX_BINDIR     = $(BIN_DIR)/osx
USR_BINDIR     = /usr/bin
OPT_BINDIR     = /opt/local/bin
LOCAL_BINDIR   = /usr/local/bin
BINDIR         = /bin
VERSION        = v2.2
VERSIONP       = 2p2p0

all: dependent_binaries archives tests

dependent_binaries: gnu_sort gnu_awk gnu_sha1sum

gnu_sort:
ifeq ($(PLATFORM),Darwin)
	@test -s $(LOCAL_BINDIR)/gsort || { echo "[STARCH] could not find GNU sort -- consider using Homebrew to install GNU Coreutils!"; exit 1; }
endif

gnu_awk:
ifeq ($(PLATFORM),Darwin)
	@test -s $(LOCAL_BINDIR)/gawk || { echo "[STARCH] could not find GNU awk -- consider using Homebrew to install gawk package!"; exit 1; }
endif

gnu_sha1sum:
ifeq ($(PLATFORM),Linux)
	@test -s $(USR_BINDIR)/sha1sum || { echo "[STARCH] could not find GNU sha1sum -- consider installing GNU Coreutils!"; exit 1; }
endif
ifeq ($(PLATFORM),Darwin)
	@test -s $(LOCAL_BINDIR)/sha1sum || { echo "[STARCH] could not find sha1sum -- consider using Homebrew to install md5sha1sum package!"; exit 1; }
endif

archives: compext_archives

tests: hash_integrity_test element_count_tests base_count_tests

hash_integrity_test: dependent_binaries metadata_hash_integrity_test
	@echo "[STARCH] hash integrity validated!"

element_count_tests: dependent_binaries metadata_element_count_all_chromosomes_integrity_test metadata_element_count_random_chromosome_integrity_test
	@echo "[STARCH] element count integrity validated (all and random chromosomes)!"

base_count_tests: dependent_binaries metadata_base_count_all_chromosomes_integrity_test metadata_base_count_random_chromosome_integrity_test metadata_unique_base_count_all_chromosomes_integrity_test metadata_unique_base_count_random_chromosome_integrity_test
	@echo "[STARCH] base count integrity validated (all and random chromosomes)!"

compext_archives:
	@echo "[STARCH] making compression/extraction archives..."
	@test -s $(COMPEXTRES_DIR) || make -C $(COMPEXT_DIR) -f makefile archives
	@echo "[STARCH] archives are ready!"

results_dir:
	@echo "[STARCH] making results directory..."
	@mkdir -p $(RESULTS)

# ---------------------------------------------

metadata_hash_integrity_test: results_dir
	@echo "[STARCH] checking metadata hash integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] signing bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-json-no-trailing-newline $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(USR_BINDIR)/sha1sum | sed 's/ .*//' | $(USR_BINDIR)/xxd -r -p | $(USR_BINDIR)/base64 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.sha1.observed
	@echo "[STARCH] extracting signature of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --sha1-signature $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.sha1.expected
	@echo "[STARCH] signing gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-json-no-trailing-newline $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(USR_BINDIR)/sha1sum | sed 's/ .*//' | $(USR_BINDIR)/xxd -r -p | $(USR_BINDIR)/base64 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.sha1.observed
	@echo "[STARCH] extracting signature of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --sha1-signature $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.sha1.expected
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] signing bzip2 archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-json-no-trailing-newline $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(LOCAL_BINDIR)/sha1sum | sed 's/ .*//' | $(USR_BINDIR)/xxd -r -p | $(USR_BINDIR)/base64 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.sha1.observed
	@echo "[STARCH] extracting signature of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --sha1-signature $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.sha1.expected
	@echo "[STARCH] signing gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-json-no-trailing-newline $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(LOCAL_BINDIR)/sha1sum | sed 's/ .*//' | $(USR_BINDIR)/xxd -r -p | $(USR_BINDIR)/base64 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.sha1.observed
	@echo "[STARCH] extracting signature of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --sha1-signature $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.sha1.expected
endif
	@echo "[STARCH] comparing bzip2 signatures..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.sha1.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.sha1.observed
	@echo "[STARCH] no differences!"
	@echo "[STARCH] comparing gzip signatures..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.sha1.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.sha1.observed
	@echo "[STARCH] no differences!"
	@echo "[STARCH] all done!"

metadata_element_count_all_chromosomes_integrity_test: results_dir
	@echo "[STARCH] checking metadata element count (all chromosomes) integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] extracting all-chromosome element count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.observed
	@echo "[STARCH] computing all-chromosome element count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | wc -l | awk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.expected
	@echo "[STARCH] comparing all-chromosome element counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.observed
	@echo "[STARCH] extracting all-chromosome element count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.observed
	@echo "[STARCH] computing all-chromosome element count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | wc -l | awk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.expected
	@echo "[STARCH] comparing all-chromosome element counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.observed
	@echo "[STARCH] no differences!"
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] extracting all-chromosome element count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.observed
	@echo "[STARCH] computing all-chromosome element count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | wc -l | gawk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.expected
	@echo "[STARCH] comparing all-chromosome element counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.all.observed
	@echo "[STARCH] extracting all-chromosome element count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.observed
	@echo "[STARCH] computing all-chromosome element count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | wc -l | gawk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.expected
	@echo "[STARCH] comparing all-chromosome element counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.all.observed
	@echo "[STARCH] no differences!"
endif
	@echo "[STARCH] all done!"

metadata_element_count_random_chromosome_integrity_test: results_dir
	@echo "[STARCH] checking metadata element count (random chromosome) integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | sort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome)
	@echo "[STARCH] extracting random-chromosome element count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome) --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome element count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | wc -l | awk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome element counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).observed
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | sort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome)
	@echo "[STARCH] extracting random-chromosome element count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome) --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome element count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | wc -l | awk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome element counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).observed
	@echo "[STARCH] no differences!"
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | gsort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome)
	@echo "[STARCH] extracting random-chromosome element count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome) --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome element count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | wc -l | gawk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome element counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.elements.random_chromosome).observed
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | gsort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome)
	@echo "[STARCH] extracting random-chromosome element count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome) --elements $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome element count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | wc -l | gawk '{sub(/^[ \t]+/, ""); print}' - > $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome element counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.elements.random_chromosome).observed
	@echo "[STARCH] no differences!"
endif

# ---------------------------------------------

metadata_base_count_all_chromosomes_integrity_test: results_dir
	@echo "[STARCH] checking metadata base count (all chromosome) integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] extracting all-chromosome bases count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.observed
	@echo "[STARCH] computing all-chromosome bases count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.expected
	@echo "[STARCH] comparing all-chromosome bases counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.observed
	@echo "[STARCH] extracting all-chromosome bases count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.observed
	@echo "[STARCH] computing all-chromosome bases count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.expected
	@echo "[STARCH] comparing all-chromosome bases counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.observed
	@echo "[STARCH] no differences!"
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] extracting all-chromosome bases count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.observed
	@echo "[STARCH] computing all-chromosome bases count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.expected
	@echo "[STARCH] comparing all-chromosome bases counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.all.observed
	@echo "[STARCH] extracting all-chromosome bases count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.observed
	@echo "[STARCH] computing all-chromosome bases count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.expected
	@echo "[STARCH] comparing all-chromosome bases counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.all.observed
	@echo "[STARCH] no differences!"
endif

metadata_base_count_random_chromosome_integrity_test: results_dir
	@echo "[STARCH] checking metadata base count (random chromosome) integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | sort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome base count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome) --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome base count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome base counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).observed
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | sort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome base count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome) --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome base count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome base counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).observed
	@echo "[STARCH] no differences!"
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | gsort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome base count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome) --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome base count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome base counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.bases.random_chromosome).observed
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | gsort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome base count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome) --bases $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome base count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome base counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.bases.random_chromosome).observed
	@echo "[STARCH] no differences!"
endif

# ---------------------------------------------

metadata_unique_base_count_all_chromosomes_integrity_test: results_dir
	@echo "[STARCH] checking metadata unique base count (all chromosome) integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] extracting all-chromosome unique bases count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.observed
	@echo "[STARCH] computing all-chromosome unique bases count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.expected
	@echo "[STARCH] comparing all-chromosome unique bases counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.observed
	@echo "[STARCH] extracting all-chromosome unique bases count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.observed
	@echo "[STARCH] computing all-chromosome unique bases count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.expected
	@echo "[STARCH] comparing all-chromosome unique bases counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.observed
	@echo "[STARCH] no differences!"
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] extracting all-chromosome unique bases count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.observed
	@echo "[STARCH] computing all-chromosome unique bases count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.expected
	@echo "[STARCH] comparing all-chromosome unique bases counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.all.observed
	@echo "[STARCH] extracting all-chromosome unique bases count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.observed
	@echo "[STARCH] computing all-chromosome unique bases count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.expected
	@echo "[STARCH] comparing all-chromosome unique bases counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.all.observed
	@echo "[STARCH] no differences!"
endif

metadata_unique_base_count_random_chromosome_integrity_test: results_dir
	@echo "[STARCH] checking metadata unique base count (random chromosome) integrity..."
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | sort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome unique base count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome) --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome unique base count of bzip2-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome unique base counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(LINUX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | sort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome unique base count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome) --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome unique base count of gzip-backed archive..."
	@$(LINUX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome unique base counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] no differences!"
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | gsort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome base count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome) --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome base count of bzip2-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_bzip2.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome base counts of bzip2-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_bzip2.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] picking random chromosome..."
	@$(shell $(OSX_BINDIR)/$(VERSION)/bin/unstarch --list-chr $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | gsort -R | head -n1 > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome)
	@echo "[STARCH] extracting random-chromosome base count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome) --bases-uniq $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] computing random-chromosome base count of gzip-backed archive..."
	@$(OSX_BINDIR)/$(VERSION)/bin/unstarch $(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome) $(COMPEXTRES_DIR)/random_$(VERSIONP)_gzip.starch | $(SRC)/count_unique_bases.pl > $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).expected
	@echo "[STARCH] comparing random-chromosome base counts of gzip-backed archive..."
	@diff --brief $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).expected $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.$(shell cat $(RESULTS)/random_$(VERSIONP)_gzip.starch.unique_bases.random_chromosome).observed
	@echo "[STARCH] no differences!"
endif

# ---------------------------------------------

clean:
	@echo "[STARCH] cleaning up intermediate starch archives..."
	@rm -rf $(RESULTS)
	@echo "[STARCH] all clean!"
