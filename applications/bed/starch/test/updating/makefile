#
# makefile
#

TMP             = /tmp
USER           := $(shell whoami)
THIS_DIR       := ${shell pwd}
RESULTS         = ${TMP}/${USER}/starch_regression_test/results/updating
SRC             = ${THIS_DIR}/src
COMPEXT_DIR     = ${THIS_DIR}/../compression_and_extraction
COMPEXTRES_DIR  = ${TMP}/${USER}/starch_regression_test/results/compression_and_extraction
ORIGINAL_DIR    = ${THIS_DIR}/../original_data
ORIGINAL_DATA   = ${TMP}/${USER}/starch_regression_test/data
BED_INPUT       = ${ORIGINAL_DATA}/data/random.bed
FIMO_INPUT      = ${ORIGINAL_DIR}/data/fimo.combined.xfac.1e-5.parsed.1M.starch
PLATFORM       := ${shell uname}
BIN_DIR         = ${THIS_DIR}/../binaries
LINUX_BINDIR    = ${BIN_DIR}/linux
OSX_BINDIR      = ${BIN_DIR}/osx
VERSION         = v2.2
VERSIONP        = 2p2p0

all: archives tests

archives: compext_archives

tests: random_updating_tests
	@echo "[STARCH] randomized archive update tests passed!"

compext_archives:
	@echo "[STARCH] making compression/extraction archives..."
	@test -s ${COMPEXTRES_DIR} || make -C ${COMPEXT_DIR} -f makefile archives
	@echo "[STARCH] archives are ready!"

bed_input:
	@test -s ${BED_INPUT} || make -C ${ORIGINAL_DIR} -f makefile random.bed

results_dir:
	@echo "[STARCH] making results directory..."
	@mkdir -p ${RESULTS}

# ---------------------------------------------

fimo_updating_tests: results_dir
	@echo "[STARCH] extracting v1.2 (bzip2-ed) FIMO archive..."
ifeq (${PLATFORM},Linux) 
	if [ ! -s ${RESULTS}/fimo.bzip2.expected ]; then ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${FIMO_INPUT} > ${RESULTS}/fimo.bzip2.expected; fi 
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo.bzip2.expected ]; then ${OSX_BINDIR}/${VERSION}/bin/unstarch ${FIMO_INPUT} > ${RESULTS}/fimo.bzip2.expected; fi
endif
	@echo "[STARCH] making v1.2 (gzip) FIMO archive from bzip2-ed data..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${RESULTS}/fimo.gzip.starch ]; then ${LINUX_BINDIR}/v1.2/bin/starch --gzip ${RESULTS}/fimo.bzip2.expected > ${RESULTS}/fimo.gzip.starch; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo.gzip.starch ]; then ${OSX_BINDIR}/v1.2/bin/starch --gzip ${RESULTS}/fimo.bzip2.expected > ${RESULTS}/fimo.gzip.starch; fi
endif
	@echo "[STARCH] extracting v1.2 (gzip-ed) FIMO archive..."
ifeq (${PLATFORM},Linux) 
	if [ ! -s ${RESULTS}/fimo.gzip.expected ]; then ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo.gzip.starch > ${RESULTS}/fimo.gzip.expected; fi 
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo.gzip.expected ]; then ${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo.gzip.starch > ${RESULTS}/fimo.gzip.expected; fi
endif
	@echo "[STARCH] taking v1.2 (gzip) FIMO archive and starchcat-ing it to new v2.2 bzip2 and gzip archives..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_bzip2.starch ]; then ${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${RESULTS}/fimo.gzip.starch > ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_bzip2.starch; fi
	if [ ! -s ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_gzip.starch ]; then ${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${RESULTS}/fimo.gzip.starch > ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_gzip.starch; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_bzip2.starch ]; then ${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${RESULTS}/fimo.gzip.starch > ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_bzip2.starch; fi
	if [ ! -s ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_gzip.starch ]; then ${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${RESULTS}/fimo.gzip.starch > ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_gzip.starch; fi
endif
	@echo "[STARCH] extracting (gzip) v1.2-to-v2.2 bzip2 and gzip FIMO archive..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${RESULTS}/fimo.gzip_to_bzip2.observed ]; then ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_bzip2.starch > ${RESULTS}/fimo.gzip_to_bzip2.observed; fi
	if [ ! -s ${RESULTS}/fimo.gzip_to_gzip.observed ]; then ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_gzip.starch > ${RESULTS}/fimo.gzip_to_gzip.observed; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo.gzip_to_bzip2.observed ]; then ${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_bzip2.starch > ${RESULTS}/fimo.gzip_to_bzip2.observed; fi
	if [ ! -s ${RESULTS}/fimo.gzip_to_gzip.observed ]; then ${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_gzip_to_${VERSIONP}_gzip.starch > ${RESULTS}/fimo.gzip_to_gzip.observed; fi
endif
	@echo "[STARCH] diff-ing v1.2 (gzip, extracted) and v1.2-to-v2.2 (bzip2, observed)..."
	diff --brief ${RESULTS}/fimo.gzip.expected ${RESULTS}/fimo.gzip_to_bzip2.observed
	@echo "[STARCH] diff-ing v1.2 (gzip, extracted) and v1.2-to-v2.2 (gzip, observed)..."
	diff --brief ${RESULTS}/fimo.gzip.expected ${RESULTS}/fimo.gzip_to_gzip.observed
	@echo "[STARCH] taking v1.2 (bzip2) FIMO archive and starchcat-ing it to new v2.2 bzip2 and gzip archives..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch ]; then ${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${FIMO_INPUT} > ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch; fi
	if [ ! -s ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_gzip.starch ]; then ${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${FIMO_INPUT} > ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_gzip.starch; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch ]; then ${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${FIMO_INPUT} > ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch; fi
	if [ ! -s ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_gzip.starch ]; then ${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${FIMO_INPUT} > ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_gzip.starch; fi
endif
	@echo "[STARCH] extracting (bzip2) v1.2-to-v2.2 bzip2 and gzip FIMO archive..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${RESULTS}/fimo.bzip2_to_bzip2.observed ]; then ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch > ${RESULTS}/fimo.bzip2_to_bzip2.observed; fi
	if [ ! -s ${RESULTS}/fimo.bzip2_to_gzip.observed ]; then ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_gzip.starch > ${RESULTS}/fimo.bzip2_to_gzip.observed; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${RESULTS}/fimo.bzip2_to_bzip2.observed ]; then ${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch > ${RESULTS}/fimo.bzip2_to_bzip2.observed; fi
	if [ ! -s ${RESULTS}/fimo.bzip2_to_gzip.observed ]; then ${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/fimo_1p2p0_bzip2_to_${VERSIONP}_gzip.starch > ${RESULTS}/fimo.bzip2_to_gzip.observed; fi
endif
	@echo "[STARCH] diff-ing v1.2 (bzip2, extracted) and v1.2-to-v2.2 (bzip2, observed)..."
	diff --brief ${RESULTS}/fimo.bzip2.expected ${RESULTS}/fimo.bzip2_to_bzip2.observed
	diff --brief ${RESULTS}/fimo.bzip2.expected ${RESULTS}/fimo.bzip2_to_gzip.observed
	@echo "[STARCH] no differences found!"

random_updating_tests: compext_archives results_dir
	@echo "[STARCH] taking v1.2 archive and starchcat-ing it to a new v2.2 archive..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p2p0_bzip2.starch > ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p2p0_bzip2.starch > ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_gzip.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p2p0_gzip.starch > ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_bzip2.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p2p0_gzip.starch > ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_gzip.starch
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p2p0_bzip2.starch > ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p2p0_bzip2.starch > ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_gzip.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p2p0_gzip.starch > ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_bzip2.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p2p0_gzip.starch > ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_gzip.starch
endif
	@echo "[STARCH] taking v1.5 archive and starchcat-ing it to a new v2.2 archive"
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p5p0_bzip2.starch > ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_bzip2.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p5p0_bzip2.starch > ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_gzip.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p5p0_gzip.starch > ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_bzip2.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p5p0_gzip.starch > ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_gzip.starch
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p5p0_bzip2.starch > ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_bzip2.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p5p0_bzip2.starch > ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_gzip.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_1p5p0_gzip.starch > ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_bzip2.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_1p5p0_gzip.starch > ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_gzip.starch
endif
	@echo "[STARCH] taking v2.0 archive and starchcat-ing it to a new v2.2 archive"
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_2p0p0_bzip2.starch > ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_bzip2.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_2p0p0_bzip2.starch > ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_gzip.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_2p0p0_gzip.starch > ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_bzip2.starch
	${LINUX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_2p0p0_gzip.starch > ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_gzip.starch
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_2p0p0_bzip2.starch > ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_bzip2.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_2p0p0_bzip2.starch > ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_gzip.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --bzip2 ${COMPEXTRES_DIR}/random_2p0p0_gzip.starch > ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_bzip2.starch
	${OSX_BINDIR}/${VERSION}/bin/starchcat --gzip ${COMPEXTRES_DIR}/random_2p0p0_gzip.starch > ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_gzip.starch
endif
	@echo "[STARCH] diff-comparing elements from v1.2 and v2.2 archives"
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p2p0_bzip2.starch > ${RESULTS}/random_1p2p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p2p0_gzip.starch > ${RESULTS}/random_1p2p0_gzip.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p2p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p2p0_gzip.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p2p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p2p0_gzip.bed
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p2p0_bzip2.starch > ${RESULTS}/random_1p2p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p2p0_gzip.starch > ${RESULTS}/random_1p2p0_gzip.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p2p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p2p0_gzip.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_bzip2_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p2p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p2p0_gzip_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p2p0_gzip.bed
endif
	@echo "[STARCH] no differences found!"
	@echo "[STARCH] diff-comparing elements from v1.5 and v2.2 archives"
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p5p0_bzip2.starch > ${RESULTS}/random_1p5p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p5p0_gzip.starch > ${RESULTS}/random_1p5p0_gzip.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p5p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p5p0_gzip.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p5p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p5p0_gzip.bed
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p5p0_bzip2.starch > ${RESULTS}/random_1p5p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_1p5p0_gzip.starch > ${RESULTS}/random_1p5p0_gzip.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p5p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_1p5p0_gzip.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_bzip2_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p5p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_1p5p0_gzip_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_1p5p0_gzip.bed
endif
	@echo "[STARCH] no differences found!"
	@echo "[STARCH] diff-comparing elements from v2.0 and v2.2 archives"
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_2p0p0_bzip2.starch > ${RESULTS}/random_2p0p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_2p0p0_gzip.starch > ${RESULTS}/random_2p0p0_gzip.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_2p0p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_2p0p0_gzip.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_2p0p0_bzip2.bed
	${LINUX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_2p0p0_gzip.bed
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_2p0p0_bzip2.starch > ${RESULTS}/random_2p0p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${COMPEXTRES_DIR}/random_2p0p0_gzip.starch > ${RESULTS}/random_2p0p0_gzip.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_2p0p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_bzip2.starch | diff --brief - ${RESULTS}/random_2p0p0_gzip.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_bzip2_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_2p0p0_bzip2.bed
	${OSX_BINDIR}/${VERSION}/bin/unstarch ${RESULTS}/random_2p0p0_gzip_to_${VERSIONP}_gzip.starch | diff --brief - ${RESULTS}/random_2p0p0_gzip.bed
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

clean:
	@echo "[STARCH] cleaning up intermediate starch archives..."
	@rm -rf ${RESULTS}
	@echo "[STARCH] all clean!"
