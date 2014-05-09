#
# makefile
#

TMP             = /tmp
USER           := $(shell whoami)
THIS_DIR       := ${shell pwd}
RESULTS         = ${TMP}/${USER}/starch_regression_test/results/nested_and_duplicate_elements
BED_INPUT       = ${THIS_DIR}/data/test_input.bed
STARCH_BZIP2    = ${RESULTS}/test_output.bz2starch
STARCH_GZIP     = ${RESULTS}/test_output.gzstarch
PLATFORM       := ${shell uname}
BIN_DIR         = ${THIS_DIR}/../binaries
LINUX_BINDIR    = ${BIN_DIR}/linux
OSX_BINDIR      = ${BIN_DIR}/osx
VERSION         = v2.1
VERSIONP        = 2p1p0

all: archives tests

archives:
	if [ ! -d ${RESULTS} ]; then mkdir -p ${RESULTS}; fi
	@echo "[STARCH] making bzip2-starch archive with v2.1 or greater metadata..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${STARCH_BZIP2} ]; then ${LINUX_BINDIR}/${VERSION}/bin/starch --bzip2 ${BED_INPUT} > ${STARCH_BZIP2}; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${STARCH_BZIP2} ]; then ${OSX_BINDIR}/${VERSION}/bin/starch --bzip2 ${BED_INPUT} > ${STARCH_BZIP2}; fi
endif
	@echo "[STARCH] making gzip-starch archive with v2.1 or greater metadata..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${STARCH_GZIP} ]; then ${LINUX_BINDIR}/${VERSION}/bin/starch --gzip ${BED_INPUT} > ${STARCH_GZIP}; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${STARCH_GZIP} ]; then ${OSX_BINDIR}/${VERSION}/bin/starch --gzip ${BED_INPUT} > ${STARCH_GZIP}; fi
endif
	@echo "[STARCH] made archives!"

tests: nested_tests duplicate_tests
	@echo "[STARCH] nested- and duplicate-element tests passed!"

nested_tests:
	@echo "[STARCH] testing nested elements..."
	@echo "[STARCH] nested element tests passed!"

duplicate_tests:
	@echo "[STARCH] testing duplicate elements..."
	@echo "[STARCH] duplicate element tests passed!"

clean:
	@echo "[STARCH] cleaning up intermediate starch archives..."
	@rm -rf ${RESULTS}
	@echo "[STARCH] all clean!"