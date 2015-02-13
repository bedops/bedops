MAIN                = ../../../..
HEAD                = $(MAIN)/interfaces/general-headers
LIB1                = $(MAIN)/interfaces/src/data/measurement
LIB2                = $(MAIN)/interfaces/src/utility
LIB3                = $(MAIN)/interfaces/src/data/starch
THISDIR             = ${shell pwd}
INCLUDES            = -iquote$(HEAD) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}

PARTY3              = ${THISDIR}/$(MAIN)/third-party
LIBJANSSON          = libjansson.a
LIBBZIP2            = libbz2.a
LIBZLIB             = libz.a
LOCALJANSSONDIR     = ${PARTY3}/jansson
LOCALJANSSONLIBDIR  = ${LOCALJANSSONDIR}/lib
LOCALJANSSONINCDIR  = ${LOCALJANSSONDIR}/include
LOCALJANSSONLIB     = ${LOCALJANSSONLIBDIR}/${LIBJANSSON}
LOCALBZIP2DIR       = ${PARTY3}/bzip2
LOCALBZIP2LIBDIR    = ${LOCALBZIP2DIR}
LOCALBZIP2LIB       = ${LOCALBZIP2LIBDIR}/${LIBBZIP2}
LOCALBZIP2INCDIR    = ${LOCALBZIP2DIR}
LOCALZLIBDIR        = ${PARTY3}/zlib
LOCALZLIBLIB        = ${LOCALZLIBDIR}/${LIBZLIB}
LOCALZLIBINCDIR     = ${LOCALZLIBDIR}

LIBLOCATION         = -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBDIR}
LIBRARIES           = ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB}

PROG                = sort-bed
DIST_DIR            = ../bin
OBJ_DIR             = objects
WARNINGS            = -Wall -Wextra -pedantic
BLDFLAGS            = ${WARNINGS} -O3 -std=c++11
SFLAGS              = -static

STARCHOBJS          = $(OBJ_DIR)/starchConstants.o $(OBJ_DIR)/starchFileHelpers.o $(OBJ_DIR)/starchHelpers.o $(OBJ_DIR)/starchMetadataHelpers.o $(OBJ_DIR)/unstarchHelpers.o $(OBJ_DIR)/starchSha1Digest.o $(OBJ_DIR)/starchBase64Coding.o

FLAGS               = $(SFLAGS) -s ${BLDFLAGS} ${LIBLOCATION} ${INCLUDES}
DFLAGS              = $(SFLAGS) -g -O0 -std=c++11 -Wall -fno-inline -pedantic ${LIBLOCATION} ${INCLUDES}
GPROFFLAGS          = $(SFLAGS) -O -std=c++11 -Wall -pedantic -pg ${LIBLOCATION} ${INCLUDES}

ifneq ($(shell uname -s),CYGWIN_NT-6.1)
	WARNINGS += -ansi
else
	WARNINGS += -U__STRICT_ANSI__
endif

build: sort

sort: sortbuild
	${CXX} -o ${DIST_DIR}/${PROG} ${FLAGS} ${STARCHOBJS} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o ${OBJ_DIR}/CheckSort.o ${LIBRARIES}

build_gprof: gprofbuild
	${CXX} -o ${DIST_DIR}/gprof.${PROG} ${GPROFFLAGS} ${STARCHOBJS} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o ${OBJ_DIR}/CheckSort.o ${LIBRARIES}

build_debug: dbgbuild
	${CXX} $(DFLAGS) ${STARCHOBJS} ${OBJ_DIR}/SortDetails.o ${OBJ_DIR}/Sort.o ${OBJ_DIR}/CheckSort.o -o ${DIST_DIR}/debug.${PROG} ${LIBRARIES}

gprofbuild: prep
	$(CXX) -c ${GPROFFLAGS} $(LIB3)/starchConstants.c -o $(OBJ_DIR)/starchConstants.o ${INCLUDES}
	$(CXX) -c ${GPROFFLAGS} $(LIB3)/starchFileHelpers.c -o $(OBJ_DIR)/starchFileHelpers.o ${INCLUDES}
	$(CXX) -c ${GPROFFLAGS} $(LIB3)/starchHelpers.c -o $(OBJ_DIR)/starchHelpers.o -iquote${HEAD} ${INCLUDES}
	$(CXX) -c ${GPROFFLAGS} $(LIB3)/starchMetadataHelpers.c -o $(OBJ_DIR)/starchMetadataHelpers.o ${INCLUDES}
	$(CXX) -c ${GPROFFLAGS} $(LIB3)/unstarchHelpers.c -o $(OBJ_DIR)/unstarchHelpers.o ${INCLUDES}
	$(CXX) -c ${GPROFFLAGS} ${LIB3}/starchSha1Digest.c -o  ${OBJ_DIR}/starchSha1Digest.o ${INCLUDES}
	$(CXX) -c ${GPROFFLAGS} ${LIB3}/starchBase64Coding.c -o  ${OBJ_DIR}/starchBase64Coding.o ${INCLUDES}
	${CXX} -c ${GPROFFLAGS} CheckSort.cpp -o ${OBJ_DIR}/CheckSort.o ${INCLUDES}
	${CXX} -c ${GPROFFLAGS} SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} -c ${GPROFFLAGS} Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

sortbuild: prep
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchConstants.c -o $(OBJ_DIR)/starchConstants.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchFileHelpers.c -o $(OBJ_DIR)/starchFileHelpers.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchHelpers.c -o $(OBJ_DIR)/starchHelpers.o -iquote${HEAD} ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchMetadataHelpers.c -o $(OBJ_DIR)/starchMetadataHelpers.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/unstarchHelpers.c -o $(OBJ_DIR)/unstarchHelpers.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) ${LIB3}/starchSha1Digest.c -o  ${OBJ_DIR}/starchSha1Digest.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) ${LIB3}/starchBase64Coding.c -o  ${OBJ_DIR}/starchBase64Coding.o ${INCLUDES}
	${CXX} -c $(BLDFLAGS) CheckSort.cpp -o ${OBJ_DIR}/CheckSort.o ${INCLUDES}
	${CXX} -c $(BLDFLAGS) SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} -c $(BLDFLAGS) Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

dbgbuild: prep
	$(CXX) -c $(DFLAGS) $(LIB3)/starchConstants.c -o $(OBJ_DIR)/starchConstants.o ${INCLUDES}
	$(CXX) -c $(DFLAGS) $(LIB3)/starchFileHelpers.c -o $(OBJ_DIR)/starchFileHelpers.o ${INCLUDES}
	$(CXX) -c $(DFLAGS) $(LIB3)/starchHelpers.c -o $(OBJ_DIR)/starchHelpers.o -iquote${HEAD} ${INCLUDES}
	$(CXX) -c $(DFLAGS) $(LIB3)/starchMetadataHelpers.c -o $(OBJ_DIR)/starchMetadataHelpers.o ${INCLUDES}
	$(CXX) -c $(DFLAGS) $(LIB3)/unstarchHelpers.c -o $(OBJ_DIR)/unstarchHelpers.o ${INCLUDES}
	$(CXX) -c $(DFLAGS) ${LIB3}/starchSha1Digest.c -o  ${OBJ_DIR}/starchSha1Digest.o ${INCLUDES}
	$(CXX) -c $(DFLAGS) ${LIB3}/starchBase64Coding.c -o  ${OBJ_DIR}/starchBase64Coding.o ${INCLUDES}
	${CXX} -c $(DFLAGS) CheckSort.cpp -o ${OBJ_DIR}/CheckSort.o ${INCLUDES}
	${CXX} -c $(DFLAGS) SortDetails.cpp -o ${OBJ_DIR}/SortDetails.o -I${HEAD}
	${CXX} -c $(DFLAGS) Sort.cpp -o ${OBJ_DIR}/Sort.o -I${HEAD}

prep: clean
	mkdir -p ${DIST_DIR}
	mkdir -p ${OBJ_DIR}

clean:
	rm -rf ${OBJ_DIR}
	rm -f $(DIST_DIR)/$(PROG)
	rm -f $(DIST_DIR)/*.$(PROG)
