MAIN            = ../../../..
HEAD            = $(MAIN)/interfaces/general-headers
CC              = g++
PROG            = sort-bed
DIST_DIR        = ../bin
OBJ_DIR         = objects
OPTIMIZE        = -O3 -std=c++11
GPROFFLAGS      = -O -std=c++11 -pg
WARNINGS        = -Wall -Wextra -ansi -pedantic
MAIN            = ../../../..
HEAD            = ${MAIN}/interfaces/general-headers

ifneq ($(wildcard .forcestatic),)
	SFLAGS	= -static -s
endif

all: sort

gprof: gprofbuild
	${CC} ${SFLAGS} ${WARNINGS} ${GPROFFLAGS} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/gprof.${PROG}

sort: sortbuild
	${CC} ${SFLAGS} ${WARNINGS} ${OPTIMIZE} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/${PROG}

gprofbuild: prep
	${CC} ${WARNINGS} ${GPROFFLAGS} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CC} ${WARNINGS} ${GPROFFLAGS} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD} 

sortbuild: prep
	${CC} ${WARNINGS} ${OPTIMIZE} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CC} ${WARNINGS} ${OPTIMIZE} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

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
	${CC} ${WARNINGS} ${OPTIMIZE} -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CC} ${WARNINGS} ${OPTIMIZE} -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}
	${CC} ${WARNINGS} ${OPTIMIZE} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/${PROG}.nonstatic

debug: prep
	${CC} -g -O0 -std=c++11 -c SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CC} -g -O0 -std=c++11 -c Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}
	${CC} -g ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o -o ${DIST_DIR}/debug.${PROG}

