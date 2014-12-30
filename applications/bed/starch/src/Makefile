HEAD                      = ../../../..
INTERFACES                = $(HEAD)/interfaces
MAIN                      = ${INTERFACES}/general-headers
MAINAPPDIR                = ../..
PARTY3                    = $(HEAD)/third-party
BINDIR                    = ../bin
THISDIR                   = ${shell pwd}
LIBSTARCH                 = libstarch.a
LIBSTARCHDEBUG            = libstarch_debug.a
LIBSTARCHGPROF            = libstarch_gprof.a
LIBJANSSON                = libjansson.a
LIBBZIP2                  = libbz2.a
LIBZLIB                   = libz.a
LOCALSTARCHLIBDIR         = ../lib
LOCALSTARCHLIB            = ${LOCALSTARCHLIBDIR}/${LIBSTARCH}
LOCALSTARCHLIBDEBUG       = ${LOCALSTARCHLIBDIR}/${LIBSTARCHDEBUG}
LOCALSTARCHLIBGPROF       = ${LOCALSTARCHLIBDIR}/${LIBSTARCHGPROF}
LOCALJANSSONDIR           = ${THISDIR}/${PARTY3}/jansson
LOCALJANSSONLIBDIR        = ${LOCALJANSSONDIR}/lib
LOCALJANSSONLIB           = ${LOCALJANSSONLIBDIR}/${LIBJANSSON}
LOCALJANSSONINCDIR        = ${LOCALJANSSONDIR}/include
LOCALBZIP2DIR             = ${THISDIR}/${PARTY3}/bzip2
LOCALBZIP2LIBDIR          = ${LOCALBZIP2DIR}
LOCALBZIP2LIB             = ${LOCALBZIP2LIBDIR}/${LIBBZIP2}
LOCALBZIP2INCDIR          = ${LOCALBZIP2DIR}
LOCALZLIBDIR              = ${THISDIR}/${PARTY3}/zlib
LOCALZLIBLIBDIR           = ${LOCALZLIBDIR}
LOCALZLIBLIB              = ${LOCALZLIBLIBDIR}/${LIBZLIB}
LOCALZLIBINCDIR           = ${LOCALZLIBDIR}
OBJDIR                    = ${INTERFACES}/src/data/starch
LOCALOBJDIR               = objects
INCLUDES                  = -iquote${MAIN} -iquote${HEAD} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
LIBRARIES                 = ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB}
ARCH_VERSION              = v2.1
BIN_VERSION               = v2.4.3
TEST                      = ../test
TEST_LINUX_BINDIR         = ${TEST}/binaries/linux/${ARCH_VERSION}/bin
TEST_OSX_BINDIR           = ${TEST}/binaries/osx/${ARCH_VERSION}/bin
AR                        = ar
SFLAGS                    = -static
CXXFLAGS                  = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O2 -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 ${SFLAGS} -s
CXXDFLAGS                 = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O0 -g -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 -DDEBUG=1 ${SFLAGS}
CXXGFLAGS                 = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 -DDEBUG=1 ${SFLAGS} -pg


.PHONY: starchcluster

build: dependencies starchLibrary starch unstarch starchcat starchcluster
	rm -f *~

# dependencies-related recipes all have mkdirs prereqs
#   and pretty much everything else has dependencies-related prereqs at some level
#   -> let make figure out hierarchy and eliminate mkdir/rm race conditions
dependencies: mkdirs
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchConstants.c -o ${LOCALOBJDIR}/starchConstants.o ${INCLUDES}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchMetadataHelpers.c -o  ${LOCALOBJDIR}/starchMetadataHelpers.o ${INCLUDES}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/unstarchHelpers.c -o  ${LOCALOBJDIR}/unstarchHelpers.o ${INCLUDES}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchHelpers.c -o  ${LOCALOBJDIR}/starchHelpers.o ${INCLUDES}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchFileHelpers.c -o  ${LOCALOBJDIR}/starchFileHelpers.o ${INCLUDES}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchSha1Digest.c -o  ${LOCALOBJDIR}/starchSha1Digest.o ${INCLUDES}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchBase64Coding.c -o  ${LOCALOBJDIR}/starchBase64Coding.o ${INCLUDES}

debug: dependencies_debug starchLibrary_debug starch_debug unstarch_debug starchcat_debug starchcluster
	rm -f *~

dependencies_debug: mkdirs
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchConstants.c -o ${LOCALOBJDIR}/starchConstants.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/unstarchHelpers.c -o  ${LOCALOBJDIR}/unstarchHelpers.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchHelpers.c -o  ${LOCALOBJDIR}/starchHelpers.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchMetadataHelpers.c -o  ${LOCALOBJDIR}/starchMetadataHelpers.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchFileHelpers.c -o  ${LOCALOBJDIR}/starchFileHelpers.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchSha1Digest.c -o  ${LOCALOBJDIR}/starchSha1Digest.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchBase64Coding.c -o  ${LOCALOBJDIR}/starchBase64Coding.o ${INCLUDES}

gprof: dependencies_gprof starchLibrary_gprof starch_gprof unstarch_gprof starchcat_gprof starchcluster
	rm -f *~

dependencies_gprof: mkdirs
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchConstants.c -o ${LOCALOBJDIR}/starchConstants.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/unstarchHelpers.c -o  ${LOCALOBJDIR}/unstarchHelpers.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchHelpers.c -o  ${LOCALOBJDIR}/starchHelpers.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchMetadataHelpers.c -o  ${LOCALOBJDIR}/starchMetadataHelpers.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchFileHelpers.c -o  ${LOCALOBJDIR}/starchFileHelpers.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchSha1Digest.c -o  ${LOCALOBJDIR}/starchSha1Digest.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchBase64Coding.c -o  ${LOCALOBJDIR}/starchBase64Coding.o ${INCLUDES}

starchLibrary: dependencies
	${AR} rcs ${LOCALSTARCHLIB} ${LOCALOBJDIR}/starchConstants.o  ${LOCALOBJDIR}/unstarchHelpers.o  ${LOCALOBJDIR}/starchHelpers.o  ${LOCALOBJDIR}/starchMetadataHelpers.o ${LOCALOBJDIR}/starchFileHelpers.o ${LOCALOBJDIR}/starchSha1Digest.o ${LOCALOBJDIR}/starchBase64Coding.o

starchLibrary_debug: dependencies_debug
	${AR} rcs ${LOCALSTARCHLIBDEBUG} ${LOCALOBJDIR}/starchConstants.o  ${LOCALOBJDIR}/unstarchHelpers.o  ${LOCALOBJDIR}/starchHelpers.o  ${LOCALOBJDIR}/starchMetadataHelpers.o  ${LOCALOBJDIR}/starchFileHelpers.o ${LOCALOBJDIR}/starchSha1Digest.o ${LOCALOBJDIR}/starchBase64Coding.o

starchLibrary_gprof: dependencies_gprof
	${AR} rcs ${LOCALSTARCHLIBGPROF} ${LOCALOBJDIR}/starchConstants.o  ${LOCALOBJDIR}/unstarchHelpers.o  ${LOCALOBJDIR}/starchHelpers.o  ${LOCALOBJDIR}/starchMetadataHelpers.o  ${LOCALOBJDIR}/starchFileHelpers.o ${LOCALOBJDIR}/starchSha1Digest.o ${LOCALOBJDIR}/starchBase64Coding.o

starch: starchLibrary
	${CXX} ${CXXFLAGS} -c starch.c -o $(LOCALOBJDIR)/starch.o ${INCLUDES}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/starch.o -o ${BINDIR}/starch ${LOCALSTARCHLIB} ${LIBRARIES}
	rm -f *~

starch_debug: starchLibrary_debug
	${CXX} ${CXXDFLAGS} -c starch.c -o $(LOCALOBJDIR)/debug.starch.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/debug.starch.o -o ${BINDIR}/debug.starch ${LOCALSTARCHLIBDEBUG} ${LIBRARIES}
	rm -f *~

starch_gprof: starchLibrary_gprof
	${CXX} ${CXXDFLAGS} -c starch.c -o $(LOCALOBJDIR)/gprof.starch.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/gprof.starch.o -o ${BINDIR}/gprof.starch ${LOCALSTARCHLIBGPROF} ${LIBRARIES}
	rm -f *~

unstarch: starchLibrary
	${CXX} ${CXXFLAGS} -c unstarch.c -o $(LOCALOBJDIR)/unstarch.o ${INCLUDES}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/unstarch.o -o ${BINDIR}/unstarch ${LOCALSTARCHLIB} ${LIBRARIES}
	rm -f *~

unstarch_debug: starchLibrary_debug
	${CXX} ${CXXDFLAGS} -c unstarch.c -o $(LOCALOBJDIR)/debug.unstarch.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/debug.unstarch.o -o ${BINDIR}/debug.unstarch ${LOCALSTARCHLIBDEBUG} ${LIBRARIES}
	rm -f *~

unstarch_gprof: starchLibrary_gprof
	${CXX} ${CXXGFLAGS} -c unstarch.c -o $(LOCALOBJDIR)/gprof.unstarch.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} $(LOCALOBJDIR)/gprof.unstarch.o -o ${BINDIR}/gprof.unstarch ${LOCALSTARCHLIBGPROF} ${LIBRARIES}
	rm -f *~

starchcluster: starchcat
	touch starchcluster_sge.tcsh && cp starchcluster_sge.tcsh ${BINDIR}/starchcluster_sge
	touch starchcluster_gnuParallel.tcsh && cp starchcluster_gnuParallel.tcsh ${BINDIR}/starchcluster_gnuParallel

starchcat: starchLibrary
	${CXX} ${CXXFLAGS} -c starchcat.c -o $(LOCALOBJDIR)/starchcat.o ${INCLUDES}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/starchcat.o -o ${BINDIR}/starchcat ${LOCALSTARCHLIB} ${LIBRARIES}

starchcat_debug: starchLibrary_debug
	${CXX} ${CXXDFLAGS} -c starchcat.c -o $(LOCALOBJDIR)/debug.starchcat.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/debug.starchcat.o -o ${BINDIR}/debug.starchcat ${LOCALSTARCHLIBDEBUG} ${LIBRARIES}

starchcat_gprof: starchLibrary_gprof
	${CXX} ${CXXGFLAGS} -c starchcat.c -o $(LOCALOBJDIR)/gprof.starchcat.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} $(LOCALOBJDIR)/gprof.starchcat.o -o ${BINDIR}/gprof.starchcat ${LOCALSTARCHLIBGPROF} ${LIBRARIES}

test: starch unstarch starchcat
	cp ${BINDIR}/starch ${TEST_LINUX_BINDIR}/starch
	cp ${BINDIR}/unstarch ${TEST_LINUX_BINDIR}/unstarch
	cp ${BINDIR}/starchcat ${TEST_LINUX_BINDIR}/starchcat
	make -C ${TEST} all

mkdirs:
	mkdir -p ${LOCALOBJDIR}
	mkdir -p ${LOCALSTARCHLIBDIR}
	mkdir -p ${BINDIR}

clean:
	rm -f $(LOCALSTARCHLIB)
	rm -f ${LOCALSTARCHLIBDEBUG}
	rm -f ${LOCALSTARCHLIBGPROF}
	rm -f ${BINDIR}/*starch*
	rm -rf ${LOCALOBJDIR}
