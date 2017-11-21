#
# makefile
#

TMP            =  /tmp
USER           := $(shell whoami)
THIS_DIR       := $(shell pwd)
RESULTS        =  ${TMP}/${USER}/starch_regression_test/results/concatenation
SRC            =  $(THIS_DIR)/src
COMPEXT_DIR    =  $(THIS_DIR)/../compression_and_extraction
COMPEXTRES_DIR =  ${TMP}/${USER}/starch_regression_test/results/compression_and_extraction
ORIGINAL_DIR   =  $(THIS_DIR)/../original_data
ORIGINAL_DATA  =  ${TMP}/${USER}/starch_regression_test/data
BED_INPUT      =  $(ORIGINAL_DATA)/random.bed
PLATFORM       := $(shell uname)
BIN_DIR        =  $(THIS_DIR)/../binaries
LINUX_BINDIR   =  $(BIN_DIR)/linux
OSX_BINDIR     =  $(BIN_DIR)/osx

all: archives tests

archives: compext_archives

tests: disjoint_by_variable_size_compext_tests disjoint_by_chromosome_compext_tests disjoint_by_fixed_size_compext_tests

compext_archives:
	@echo "[STARCH] making compression/extraction archives..."
	@test -s $(COMPEXTRES_DIR) || make -C $(COMPEXT_DIR) -f makefile archives
	@echo "[STARCH] archives are ready!"

bed_input:
	@test -s $(BED_INPUT) || make -C $(ORIGINAL_DIR) -f makefile random.bed

results_dir:
	@echo "[STARCH] making results directory..."
	@mkdir -p $(RESULTS)

# ---------------------------------------------

disjoint_by_variable_size_compext_tests: compext_archives results_dir
	@echo "[STARCH] splitting input into disjoint split-by-variable-size versions..."
ifeq ($(PLATFORM),Linux)
	@$(SRC)/split_random_inputs_into_disjoint_archives.pl $(COMPEXTRES_DIR) $(RESULTS) $(LINUX_BINDIR)
endif
ifeq ($(PLATFORM),Darwin)
	@$(SRC)/split_random_inputs_into_disjoint_archives.pl $(COMPEXTRES_DIR) $(RESULTS) $(OSX_BINDIR)
endif
	@echo "[STARCH] merging disjoint versions into one archive..."
ifeq ($(PLATFORM),Linux)
	$(LINUX_BINDIR)/v2.2/bin/starchcat --bzip2 $(RESULTS)/version_test_bzip2_*.starch > $(RESULTS)/version_test_bzip2.starch
	$(LINUX_BINDIR)/v2.2/bin/starchcat --gzip $(RESULTS)/version_test_gzip_*.starch > $(RESULTS)/version_test_gzip.starch
endif
ifeq ($(PLATFORM),Darwin)
	@$(OSX_BINDIR)/v2.2/bin/starchcat --bzip2 $(RESULTS)/version_test_bzip2_*.starch > $(RESULTS)/version_test_bzip2.starch
	@$(OSX_BINDIR)/v2.2/bin/starchcat --gzip $(RESULTS)/version_test_gzip_*.starch > $(RESULTS)/version_test_gzip.starch
endif
	@echo "[STARCH] comparing element counts of disjoint archives with merged archive..."
ifeq ($(PLATFORM),Linux)
	@$(SRC)/compare_disjoint_element_counts_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/version_test_bzip2.starch $(RESULTS)/version_test_bzip2_*.starch
endif
ifeq ($(PLATFORM),Darwin)
	@$(SRC)/compare_disjoint_element_counts_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/version_test_bzip2.starch $(RESULTS)/version_test_bzip2_*.starch
endif
	@echo "[STARCH] comparing diffs of manually concatenated disjoint archives with merged archive..."
ifeq ($(PLATFORM),Linux)
	@$(SRC)/diff_disjoint_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/version_test_bzip2.starch $(RESULTS)/version_test_bzip2_*.starch
endif
ifeq ($(PLATFORM),Darwin)
	@$(SRC)/diff_disjoint_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/version_test_bzip2.starch $(RESULTS)/version_test_bzip2_*.starch
endif

# ---------------------------------------------

disjoint_by_chromosome_compext_tests: compext_archives results_dir
	@echo "[STARCH] splitting input into disjoint split-by-chromosome versions..."
ifeq ($(PLATFORM),Linux)
	@$(SRC)/split_random_inputs_into_disjoint_archives_by_chromosome.pl $(COMPEXTRES_DIR) $(RESULTS) $(LINUX_BINDIR)
endif
ifeq ($(PLATFORM),Darwin)
	@$(SRC)/split_random_inputs_into_disjoint_archives_by_chromosome.pl $(COMPEXTRES_DIR) $(RESULTS) $(OSX_BINDIR)
endif
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] comparing diffs of manually concatenated per-chromosome disjoint archives with merged bzip2 archive..."
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p2p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p2p0_bzip2.starch.chr*
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p5p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p5p0_bzip2.starch.chr*
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p0p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p0p0_bzip2.starch.chr*
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p1p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p1p0_bzip2.starch.chr*
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p2p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p2p0_bzip2.starch.chr*
	@echo "[STARCH] comparing diffs of manually concatenated per-chromosome disjoint archives with merged gzip archive..."
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p2p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p2p0_gzip.starch.chr*
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p5p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p5p0_gzip.starch.chr*
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p0p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p0p0_gzip.starch.chr*
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p1p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p1p0_gzip.starch.chr*
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p2p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p2p0_gzip.starch.chr*
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] comparing diffs of manually concatenated per-chromosome disjoint archives with merged bzip2 archive..."
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p2p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p2p0_bzip2.starch.chr*
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p5p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p5p0_bzip2.starch.chr*
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p0p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p0p0_bzip2.starch.chr*
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p1p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p1p0_bzip2.starch.chr*
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p2p0_bzip2.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p2p0_bzip2.starch.chr*
	@echo "[STARCH] comparing diffs of manually concatenated per-chromosome disjoint archives with merged gzip archive..."
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p2p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p2p0_gzip.starch.chr*
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_1p5p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_1p5p0_gzip.starch.chr*
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p0p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p0p0_gzip.starch.chr*
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p1p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p1p0_gzip.starch.chr*
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_chromosome_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(RESULTS)/random_2p2p0_gzip.starch.merge_test $(RESULTS)/disjoint_by_chr.random_2p2p0_gzip.starch.chr*
endif

# ---------------------------------------------

disjoint_by_fixed_size_compext_tests: compext_archives results_dir
	@echo "[STARCH] splitting input into disjoint split-by-fixed-size versions..."
ifeq ($(PLATFORM),Linux)
	@$(SRC)/split_random_inputs_into_disjoint_archives_by_fixed_size.pl $(COMPEXTRES_DIR) $(RESULTS) $(LINUX_BINDIR)
endif
ifeq ($(PLATFORM),Darwin)
	@$(SRC)/split_random_inputs_into_disjoint_archives_by_fixed_size.pl $(COMPEXTRES_DIR) $(RESULTS) $(OSX_BINDIR)
endif
	@echo "[STARCH] comparing diffs of manually concatenated fixed-size disjoint archives with merged bzip2 archive"
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p2p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p2p0_bzip2.starch
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p5p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p5p0_bzip2.starch
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p0p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p0p0_bzip2.starch
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p1p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p1p0_bzip2.starch
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p2p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p2p0_bzip2.starch
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p2p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p2p0_bzip2.starch
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p5p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p5p0_bzip2.starch
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p0p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p0p0_bzip2.starch
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p1p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p1p0_bzip2.starch
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p2p0_bzip2.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p2p0_bzip2.starch
endif
	@echo "[STARCH] comparing diffs of manually concatenated fixed-size disjoint archives with merged gzip archive"
ifeq ($(PLATFORM),Linux)
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p2p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p2p0_gzip.starch
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p5p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p5p0_gzip.starch
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p0p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p0p0_gzip.starch
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p1p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p1p0_gzip.starch
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(LINUX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p2p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p2p0_gzip.starch
endif
ifeq ($(PLATFORM),Darwin)
	@echo "[STARCH] v1.2 disjoint <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p2p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p2p0_gzip.starch
	@echo "[STARCH] v1.5 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_1p5p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_1p5p0_gzip.starch
	@echo "[STARCH] v2.0 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p0p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p0p0_gzip.starch
	@echo "[STARCH] v2.1 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p1p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p1p0_gzip.starch
	@echo "[STARCH] v2.2 <-> v2.2"
	@$(SRC)/diff_disjoint_by_fixed_size_archives_with_merged_archive.pl $(OSX_BINDIR)/v2.2/bin/unstarch $(COMPEXTRES_DIR)/random_2p2p0_gzip.starch $(RESULTS)/fixed_merged_merged_fixed_size_random_2p2p0_gzip.starch
endif
# ---------------------------------------------

clean:
	@echo "[STARCH] cleaning up intermediate starch archives..."
	@rm -rf $(RESULTS)
	@echo "[STARCH] all clean!"
