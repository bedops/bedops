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
	${CXX} -o ${DIST_DIR}/gprof.${PROG} ${PFLAGS} ${SFLAGS} ${WARNINGS} ${GPROFFLAGS} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o 

sort: sortbuild
	${CXX} -o ${DIST_DIR}/${PROG} ${PFLAGS} ${SFLAGS} ${WARNINGS} ${OPTIMIZE} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o

gprofbuild: prep
	${CXX} ${WARNINGS} ${GPROFFLAGS} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} ${WARNINGS} ${GPROFFLAGS} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

sortbuild: prep
	${CXX} ${WARNINGS} ${OPTIMIZE} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} ${WARNINGS} ${OPTIMIZE} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

prep:
ifneq ($(wildcard .forcestatic),)
	rm -rf .forcestatic
endif
	mkdir -p ${DIST_DIR}
	mkdir -p ${OBJ_DIR}

clean:
	rm -rf ${OBJ_DIR}
	rm -rf ${DIST_DIR}/${PROG}

nonstatic: sortbuild
	${CXX} ${WARNINGS} ${OPTIMIZE} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} ${WARNINGS} ${OPTIMIZE} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}
	${CXX} ${WARNINGS} ${OPTIMIZE} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/${PROG}.nonstatic

debug: prep
	${CXX} -g -O0 -std=c++11 -fno-inline -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} -g -O0 -std=c++11 -fno-inline -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}
	${CXX} -g ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/debug.${PROG}

