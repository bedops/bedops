MAIN               = ../../../..
HEAD               = $(MAIN)/interfaces/general-headers
CC                 = gcc
CXX                = g++
PROG               = sort-bed
DIST_DIR           = ../bin
OBJ_DIR            = objects
OPTIMIZE           = -O3 -std=c++11
GPROFFLAGS         = -O -std=c++11 -pg
WARNINGS           = -Wall -Wextra -pedantic
MAIN               = ../../../..
HEAD               = ${MAIN}/interfaces/general-headers
PARTY3             = $(MAIN)/third-party
THISDIR	           = ${shell pwd}
LIBLIBMT           = libmt.a
LOCALLIBMTDIR      = ${THISDIR}/${PARTY3}/libmt
LOCALLIBMTLIBDIR   = ${LOCALLIBMTDIR}/lib
LOCALLIBMTLIBPATH  = ${LOCALLIBMTLIBDIR}/${LIBLIBMT}
LOCALLIBMTINCDIR   = ${LOCALLIBMTDIR}/include

ifneq ($(wildcard .forcestatic),)
	SFLAGS	= -static -s
endif

ifneq ($(shell uname -s),CYGWIN_NT-6.1)
	WARNINGS += -ansi
else
	WARNINGS += -U__STRICT_ANSI__
endif

all: sort

gprof: gprofbuild
	${CXX} -o ${DIST_DIR}/gprof.${PROG} ${SFLAGS} ${WARNINGS} ${GPROFFLAGS} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o 

sort: sortbuild
	${CXX} -o ${DIST_DIR}/${PROG} -pthread ${SFLAGS} ${WARNINGS} ${OPTIMIZE} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -L${LOCALLIBMTLIBDIR} ${LOCALLIBMTLIBPATH}

gprofbuild: prep
	${CXX} ${WARNINGS} ${GPROFFLAGS} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD} -I${LOCALLIBMTINCDIR}
	${CXX} ${WARNINGS} ${GPROFFLAGS} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

sortbuild: prep dependencies
	${CXX} ${WARNINGS} ${OPTIMIZE} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD} -I${LOCALLIBMTINCDIR}
	${CXX} ${WARNINGS} ${OPTIMIZE} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

dependencies: libmt

libmt:
	test -d ${LOCALLIBMTDIR} || { cd ${MAIN} && make libmt_support_c && cd ${THISDIR}; }
	cd ${LOCALLIBMTDIR} && ${LOCALLIBMTDIR}/configure --prefix=${LOCALLIBMTDIR} && make clean && make && make install && cd ${THISDIR}

prep:
ifneq ($(wildcard .forcestatic),)
	rm -rf .forcestatic
endif
	mkdir -p ${DIST_DIR}
	mkdir -p ${OBJ_DIR}

clean:
	rm -rf ${OBJ_DIR}
	rm -rf ${DIST_DIR}/${PROG}
	-test -d ${LOCALLIBMTDIR} && { cd ${LOCALLIBMTDIR} && make clean && cd ${THISDIR}; }

nonstatic: sortbuild
	${CXX} ${WARNINGS} ${OPTIMIZE} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} ${WARNINGS} ${OPTIMIZE} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}
	${CXX} ${WARNINGS} ${OPTIMIZE} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/${PROG}.nonstatic

debug: prep
	${CXX} -g -O0 -std=c++11 -fno-inline -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} -g -O0 -std=c++11 -fno-inline -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}
	${CXX} -g ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/debug.${PROG}

