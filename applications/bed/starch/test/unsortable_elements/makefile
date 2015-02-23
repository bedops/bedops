#
# makefile
#

TMP             = /tmp
USER           := $(shell whoami)
THIS_DIR       := ${shell pwd}
RESULTS         = ${TMP}/${USER}/starch_regression_test/results/unsortable_elements
SRC             = ${THIS_DIR}/src
START_INPUT     = ${THIS_DIR}/data/test_start.bed
STOP_INPUT      = ${THIS_DIR}/data/test_stop.bed
STARCH_BZIP2    = ${RESULTS}/test_output.bz2starch
STARCH_GZIP     = ${RESULTS}/test_output.gzstarch
PLATFORM       := ${shell uname}
BIN_DIR         = ${THIS_DIR}/../binaries
LINUX_BINDIR    = ${BIN_DIR}/linux
OSX_BINDIR      = ${BIN_DIR}/osx
VERSION         = v2.1
VERSIONP        = 2p1p0

all: tests

tests: tests_bzip2 tests_gzip

tests_bzip2: tests_start_bzip2 tests_stop_bzip2

tests_start_bzip2:
	@echo "[STARCH] testing (bzip2-starch) start elements..."
	if [ ! -d ${RESULTS} ]; then mkdir -p ${RESULTS}; fi
ifeq (${PLATFORM},Linux)
	@${SRC}/isUnsortable.py ${LINUX_BINDIR}/${VERSION}/bin/starch ${STARCH_BZIP2} ${START_INPUT} > ${RESULTS}/start_bz2.starch
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/isUnsortable.py ${OSX_BINDIR}/${VERSION}/bin/starch ${STARCH_BZIP2} ${START_INPUT} > ${RESULTS}/start_bz2.starch
endif
	@echo "[STARCH] (bzip2-starch) element start test passed!"

tests_stop_bzip2:
	@echo "[STARCH] testing (bzip2-starch) stop elements..."
	if [ ! -d ${RESULTS} ]; then mkdir -p ${RESULTS}; fi
ifeq (${PLATFORM},Linux)
	@${SRC}/isUnsortable.py ${LINUX_BINDIR}/${VERSION}/bin/starch ${STARCH_BZIP2} ${STOP_INPUT} > ${RESULTS}/stop_bz2.starch
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/isUnsortable.py ${OSX_BINDIR}/${VERSION}/bin/starch ${STARCH_BZIP2} ${STOP_INPUT} > ${RESULTS}/stop_bz2.starch
endif
	@echo "[STARCH] (bzip2-starch) element stop test passed!"

tests_gzip: tests_start_gzip tests_stop_gzip

tests_start_gzip:
	@echo "[STARCH] testing (gzip-starch) start elements..."
	if [ ! -d ${RESULTS} ]; then mkdir -p ${RESULTS}; fi
ifeq (${PLATFORM},Linux)
	@${SRC}/isUnsortable.py ${LINUX_BINDIR}/${VERSION}/bin/starch ${STARCH_GZIP} ${START_INPUT} > ${RESULTS}/start_gz.starch
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/isUnsortable.py ${OSX_BINDIR}/${VERSION}/bin/starch ${STARCH_GZIP} ${START_INPUT} > ${RESULTS}/start_gz.starch
endif
	@echo "[STARCH] (gzip-starch) element start test passed!"

tests_stop_gzip:
	@echo "[STARCH] testing (gzip-starch) stop elements..."
	if [ ! -d ${RESULTS} ]; then mkdir -p ${RESULTS}; fi
ifeq (${PLATFORM},Linux)
	@${SRC}/isUnsortable.py ${LINUX_BINDIR}/${VERSION}/bin/starch ${STARCH_GZIP} ${STOP_INPUT} > ${RESULTS}/stop_gz.starch
endif
ifeq (${PLATFORM},Darwin)
	@${SRC}/isUnsortable.py ${OSX_BINDIR}/${VERSION}/bin/starch ${STARCH_GZIP} ${STOP_INPUT} > ${RESULTS}/stop_gz.starch
endif
	@echo "[STARCH] (gzip-starch) element stop test passed!"

clean:
	@echo "[STARCH] cleaning up intermediate stuff..."
	@rm -rf ${RESULTS}
	@echo "[STARCH] all clean!"
