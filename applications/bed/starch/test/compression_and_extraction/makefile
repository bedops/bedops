#
# makefile
#

TMP           =  /tmp
USER          := $(shell whoami)
THIS_DIR      := $(shell pwd)
RESULTS       =  ${TMP}/${USER}/starch_regression_test/results/compression_and_extraction
ORIGINAL_DIR  =  ${THIS_DIR}/../original_data
ORIGINAL_DATA =  ${TMP}/${USER}/starch_regression_test
BED_INPUT     =  ${ORIGINAL_DATA}/data/random.bed
PLATFORM      := $(shell uname)
LINUX_BINDIR  =  ${THIS_DIR}/../binaries/linux
OSX_BINDIR    =  ${THIS_DIR}/../binaries/osx

all: results_dir archives tests

results_dir:
	@echo "[STARCH] making results directory..."
	@mkdir -p ${RESULTS}

current_archives: random_2p2p0_bzip2.starch random_2p2p0_gzip.starch
	@echo "[STARCH] finished making current archives!"

archives: random_1p2p0_bzip2.starch random_1p2p0_gzip.starch random_1p5p0_bzip2.starch random_1p5p0_gzip.starch random_2p0p0_bzip2.starch random_2p0p0_gzip.starch random_2p1p0_bzip2.starch random_2p1p0_gzip.starch current_archives
	@echo "[STARCH] finished making archives!"

tests: concurrent_tests current_tests
	@echo "[STARCH] finished testing diffs of archives and original input!"

concurrent_tests: diff_random_1p2p0_bzip2_concurrent diff_random_1p2p0_gzip_concurrent diff_random_1p5p0_bzip2_concurrent diff_random_1p5p0_gzip_concurrent diff_random_2p0p0_bzip2_concurrent diff_random_2p0p0_gzip_concurrent diff_random_2p1p0_bzip2_concurrent diff_random_2p1p0_gzip_concurrent

current_tests: diff_random_1p2p0_bzip2_current diff_random_1p2p0_gzip_current diff_random_1p5p0_bzip2_current diff_random_1p5p0_gzip_current diff_random_2p0p0_bzip2_current diff_random_2p0p0_gzip_current diff_random_2p1p0_bzip2_current diff_random_2p1p0_gzip_current diff_random_2p2p0_bzip2_current diff_random_2p2p0_gzip_current

bed_input:
	@test -s ${BED_INPUT} || make -C ${ORIGINAL_DIR} -f makefile random.bed

# ---------------------------------------------

random_1p2p0_bzip2.starch: results_dir bed_input
	@echo "[STARCH] creating v1.2 bzip2-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v1.2/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v1.2/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_1p2p0_bzip2_concurrent: random_1p2p0_bzip2.starch
	@echo "[STARCH] testing difference between v1.2 bzip2-based starch file and random BED input using concurrent (v1.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v1.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v1.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

diff_random_1p2p0_bzip2_current: random_1p2p0_bzip2.starch
	@echo "[STARCH] testing difference between v1.2 bzip2-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_1p2p0_gzip.starch: results_dir bed_input
	@echo "[STARCH] creating v1.2 gzip-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v1.2/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v1.2/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_1p2p0_gzip_concurrent: random_1p2p0_gzip.starch
	@echo "[STARCH] testing difference between v1.2 gzip-based starch file and random BED input using concurrent (v1.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v1.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v1.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

diff_random_1p2p0_gzip_current: random_1p2p0_gzip.starch
	@echo "[STARCH] testing difference between v1.2 gzip-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_1p5p0_bzip2.starch: results_dir bed_input
	@echo "[STARCH] creating v1.5 bzip2-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v1.5/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v1.5/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_1p5p0_bzip2_concurrent: random_1p5p0_bzip2.starch
	@echo "[STARCH] testing difference between v1.5 bzip2-based starch file and random BED input using concurrent (v1.5) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v1.5/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v1.5/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

diff_random_1p5p0_bzip2_current: random_1p5p0_bzip2.starch
	@echo "[STARCH] testing difference between v1.5 bzip2-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_1p5p0_gzip.starch: results_dir bed_input
	@echo "[STARCH] creating v1.5 gzip-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v1.5/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v1.5/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_1p5p0_gzip_concurrent: random_1p5p0_gzip.starch
	@echo "[STARCH] testing difference between v1.5 gzip-based starch file and random BED input using concurrent (v1.5) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v1.5/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v1.5/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

diff_random_1p5p0_gzip_current: random_1p5p0_gzip.starch
	@echo "[STARCH] testing difference between v1.5 gzip-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_2p0p0_bzip2.starch: results_dir bed_input
	@echo "[STARCH] creating v2.0 bzip2-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v2.0/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v2.0/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_2p0p0_bzip2_concurrent: random_2p0p0_bzip2.starch
	@echo "[STARCH] testing difference between v2.0 bzip2-based starch file and random BED input using concurrent (v2.0) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.0/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.0/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

diff_random_2p0p0_bzip2_current: random_2p0p0_bzip2.starch
	@echo "[STARCH] testing difference between v2.0 bzip2-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_2p0p0_gzip.starch: results_dir bed_input
	@echo "[STARCH] creating v2.0 gzip-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v2.0/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v2.0/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_2p0p0_gzip_concurrent: random_2p0p0_gzip.starch
	@echo "[STARCH] testing difference between v2.0 gzip-based starch file and random BED input using concurrent (v2.0) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.0/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.0/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

diff_random_2p0p0_gzip_current: random_2p0p0_gzip.starch
	@echo "[STARCH] testing difference between v2.0 gzip-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_2p1p0_bzip2.starch: results_dir bed_input
	@echo "[STARCH] creating v2.1 bzip2-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v2.1/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v2.1/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_2p1p0_bzip2_concurrent: random_2p1p0_bzip2.starch
	@echo "[STARCH] testing difference between v2.1 bzip2-based starch file and random BED input using concurrent (v2.1) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.1/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.1/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif

diff_random_2p1p0_bzip2_current: random_2p1p0_bzip2.starch
	@echo "[STARCH] testing difference between v2.1 bzip2-based starch file and random BED input using concurrent (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_2p1p0_gzip.starch: results_dir bed_input
	@echo "[STARCH] creating v2.1 gzip-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v2.1/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v2.1/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_2p1p0_gzip_concurrent: random_2p1p0_gzip.starch
	@echo "[STARCH] testing difference between v2.1 gzip-based starch file and random BED input using concurrent (v2.1) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.1/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.1/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif

diff_random_2p1p0_gzip_current: random_2p1p0_gzip.starch
	@echo "[STARCH] testing difference between v2.1 gzip-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_2p2p0_bzip2.starch: results_dir bed_input
	@echo "[STARCH] creating v2.2 bzip2-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v2.2/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v2.2/bin/starch --bzip2 ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_2p2p0_bzip2_current: random_2p2p0_bzip2.starch
	@echo "[STARCH] testing difference between v2.2 bzip2-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

random_2p2p0_gzip.starch: results_dir bed_input
	@echo "[STARCH] creating v2.2 gzip-based starch file from random BED input..."
ifeq (${PLATFORM},Linux)
	test -s ${RESULTS}/$@ || ${LINUX_BINDIR}/v2.2/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif
ifeq (${PLATFORM},Darwin)
	test -s ${RESULTS}/$@ || ${OSX_BINDIR}/v2.2/bin/starch --gzip ${BED_INPUT} > ${RESULTS}/$@
endif

diff_random_2p2p0_gzip_current: random_2p2p0_gzip.starch
	@echo "[STARCH] testing difference between v2.2 gzip-based starch file and random BED input using current (v2.2) binary..."
ifeq (${PLATFORM},Linux)
	${LINUX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
ifeq (${PLATFORM},Darwin)
	${OSX_BINDIR}/v2.2/bin/unstarch ${RESULTS}/$^ | diff --brief - ${BED_INPUT}
endif
	@echo "[STARCH] no differences found!"

# ---------------------------------------------

clean:
	@echo "[STARCH] cleaning up intermediate starch archives..."
	@rm -rf ${RESULTS}
	@echo "[STARCH] all clean!"
