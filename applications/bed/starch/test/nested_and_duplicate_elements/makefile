#
# makefile
#

TMP             = /tmp
USER           := $(shell whoami)
THIS_DIR       := ${shell pwd}
RESULTS         = ${TMP}/${USER}/starch_regression_test/results/nested_and_duplicate_elements
SRC             = ${THIS_DIR}/src
BED_INPUT       = ${THIS_DIR}/data/test_input.bed
STARCH_BZIP2    = ${RESULTS}/test_output.bz2starch
STARCH_GZIP     = ${RESULTS}/test_output.gzstarch
PLATFORM       := ${shell uname}
BIN_DIR         = ${THIS_DIR}/../binaries
LINUX_BINDIR    = ${BIN_DIR}/linux
OSX_BINDIR      = ${BIN_DIR}/osx
VERSION         = v2.2
VERSIONP        = 2p2p0

all: archives tests

archives:
	if [ ! -d ${RESULTS} ]; then mkdir -p ${RESULTS}; fi
	@echo "[STARCH] making bzip2-starch archive with v2.2 or greater metadata..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${STARCH_BZIP2} ]; then ${LINUX_BINDIR}/${VERSION}/bin/starch --bzip2 ${BED_INPUT} > ${STARCH_BZIP2}; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${STARCH_BZIP2} ]; then ${OSX_BINDIR}/${VERSION}/bin/starch --bzip2 ${BED_INPUT} > ${STARCH_BZIP2}; fi
endif
	@echo "[STARCH] making gzip-starch archive with v2.2 or greater metadata..."
ifeq (${PLATFORM},Linux)
	if [ ! -s ${STARCH_GZIP} ]; then ${LINUX_BINDIR}/${VERSION}/bin/starch --gzip ${BED_INPUT} > ${STARCH_GZIP}; fi
endif
ifeq (${PLATFORM},Darwin)
	if [ ! -s ${STARCH_GZIP} ]; then ${OSX_BINDIR}/${VERSION}/bin/starch --gzip ${BED_INPUT} > ${STARCH_GZIP}; fi
endif
	@echo "[STARCH] made archives!"

tests: nested_tests duplicate_tests
	@echo "[STARCH] nested- and duplicate-element tests passed!"

nested_tests: nested_tests_bzip2 nested_tests_gzip
	@echo "[STARCH] nested element tests passed!"

nested_tests_bzip2:
	@echo "[STARCH] testing (bzip2-starch) nested elements..."
ifeq (${PLATFORM},Linux)
	@${SRC}/hasNestedElements.py ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_BZIP2}
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/hasNestedElements.py ${OSX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_BZIP2}
endif
	@echo "[STARCH] (bzip2-starch) nested element tests passed!"

nested_tests_gzip:
	@echo "[STARCH] testing (gzip-starch) nested elements..."
ifeq (${PLATFORM},Linux)
	@${SRC}/hasNestedElements.py ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_GZIP}
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/hasNestedElements.py ${OSX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_GZIP}
endif
	@echo "[STARCH] (gzip-starch) nested element tests passed!"

duplicate_tests: duplicate_tests_bzip2 duplicate_tests_gzip
	@echo "[STARCH] duplicate element tests passed!"

duplicate_tests_bzip2:
	@echo "[STARCH] testing (bzip2-starch) duplicate elements..."
ifeq (${PLATFORM},Linux)
	@${SRC}/hasDuplicateElements.py ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_BZIP2}
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/hasDuplicateElements.py ${OSX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_BZIP2}
endif
	@echo "[STARCH] (bzip2-starch) duplicate element tests passed!"

duplicate_tests_gzip:
	@echo "[STARCH] testing (gzip-starch) duplicate elements..."
ifeq (${PLATFORM},Linux)
	@${SRC}/hasDuplicateElements.py ${LINUX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_GZIP}
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/hasDuplicateElements.py ${OSX_BINDIR}/${VERSION}/bin/unstarch ${STARCH_GZIP}
endif
	@echo "[STARCH] (gzip-starch) duplicate element tests passed!"

clean:
	@echo "[STARCH] cleaning up intermediate starch archives..."
	@rm -rf ${RESULTS}
	@echo "[STARCH] all clean!"
