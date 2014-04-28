AR      = ar

ifneq ($(wildcard .forcestatic),)
	SFLAGS = -static -s
else
	SFLAGS =
endif

HEAD	                  = ../../../..
INTERFACES                = $(HEAD)/interfaces
MAIN	                  = ${INTERFACES}/general-headers
MAINAPPDIR                = ../..
PARTY3                    = $(HEAD)/third-party
CFLAGS                    = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O2 -Wformat -Wall -Wextra -Wswitch-enum -std=c99 ${SFLAGS}
CXXFLAGS                  = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O2 -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 ${SFLAGS}
CDFLAGS                   = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O0 -g -Wformat -Wall -Wextra -Wswitch-enum -std=c99 -fgnu89-inline -DDEBUG=1 -static
CXXDFLAGS                 = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O0 -g -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 -DDEBUG=1 -static
CGFLAGS                   = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O -Wformat -Wall -Wextra -Wswitch-enum -std=c99 -fgnu89-inline -DDEBUG=1 -static -pg
CXXGFLAGS                 = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 -DDEBUG=1 -static -pg
BINDIR                       = ../bin
THISDIR	                  = ${shell pwd}
LIBSTARCH                 = libstarch.a
LIBSTARCHDEBUG            = libstarch_debug.a
LIBSTARCHGPROF            = libstarch_gprof.a
LIBJANSSON                = libjansson.a
LIBBZIP2                  = libbz2.a
LIBZLIB                   = libz.a
LOCALSTARCHLIBDIR         = ../lib
LOCALSTARCHLIBPATH        = ${LOCALSTARCHLIBDIR}/${LIBSTARCH}
LOCALSTARCHLIBDEBUGPATH   = ${LOCALSTARCHLIBDIR}/${LIBSTARCHDEBUG}
LOCALSTARCHLIBGPROFPATH   = ${LOCALSTARCHLIBDIR}/${LIBSTARCHGPROF}
LOCALJANSSONDIR           = ${THISDIR}/${PARTY3}/jansson
LOCALJANSSONLIBDIR        = ${LOCALJANSSONDIR}/lib
LOCALJANSSONLIBPATH       = ${LOCALJANSSONLIBDIR}/${LIBJANSSON}
LOCALJANSSONINCDIR        = ${LOCALJANSSONDIR}/include
LOCALBZIP2DIR             = ${THISDIR}/${PARTY3}/bzip2
LOCALBZIP2LIBDIR          = ${LOCALBZIP2DIR}
LOCALBZIP2LIBPATH         = ${LOCALBZIP2LIBDIR}/${LIBBZIP2}
LOCALBZIP2INCDIR          = ${LOCALBZIP2DIR}
LOCALZLIBDIR              = ${THISDIR}/${PARTY3}/zlib
LOCALZLIBLIBDIR           = ${LOCALZLIBDIR}
LOCALZLIBLIBPATH          = ${LOCALZLIBLIBDIR}/${LIBZLIB}
LOCALZLIBINCDIR           = ${LOCALZLIBDIR}
OBJDIR                    = ${INTERFACES}/src/data/starch
LOCALOBJDIR                 = objects
PLATFORM                  = ${shell uname}
ARCH_VERSION              = v2.0
BIN_VERSION               = v2.4.2
TEST                      = ../test
TEST_LINUX_BINDIR         = ${TEST}/binaries/linux/${ARCH_VERSION}/bin
TEST_OSX_BINDIR           = ${TEST}/binaries/osx/${ARCH_VERSION}/bin

.PHONY: starchcluster

build: clean_force_flags dependencies starchLibrary starch unstarch starchcat starchcluster
	rm -f *~

dependencies:
	rm -f ${LOCALOBJDIR}
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchConstants.c -o ${LOCALOBJDIR}/starchConstants.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchMetadataHelpers.c -o  ${LOCALOBJDIR}/starchMetadataHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/unstarchHelpers.c -o  ${LOCALOBJDIR}/unstarchHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchHelpers.c -o  ${LOCALOBJDIR}/starchHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchFileHelpers.c -o  ${LOCALOBJDIR}/starchFileHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchSha1Digest.c -o  ${LOCALOBJDIR}/starchSha1Digest.o -iquote${MAIN} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} -c ${OBJDIR}/starchBase64Coding.c -o  ${LOCALOBJDIR}/starchBase64Coding.o -iquote${MAIN} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}

debug: clean_force_flags dependencies_debug starchLibrary_debug starch_debug unstarch_debug starchcat_debug starchcluster
	rm -f *~

dependencies_debug:
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchConstants.c -o ${LOCALOBJDIR}/starchConstants.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/unstarchHelpers.c -o  ${LOCALOBJDIR}/unstarchHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchHelpers.c -o  ${LOCALOBJDIR}/starchHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchMetadataHelpers.c -o  ${LOCALOBJDIR}/starchMetadataHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchFileHelpers.c -o  ${LOCALOBJDIR}/starchFileHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchSha1Digest.c -o  ${LOCALOBJDIR}/starchSha1Digest.o -iquote${MAIN} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} -c ${OBJDIR}/starchBase64Coding.c -o  ${LOCALOBJDIR}/starchBase64Coding.o -iquote${MAIN} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}

gprof: clean_force_flags dependencies_gprof starchLibrary_gprof starch_gprof unstarch_gprof starchcat_gprof starchcluster
	rm -f *~

dependencies_gprof:
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchConstants.c -o ${LOCALOBJDIR}/starchConstants.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/unstarchHelpers.c -o  ${LOCALOBJDIR}/unstarchHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchHelpers.c -o  ${LOCALOBJDIR}/starchHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchMetadataHelpers.c -o  ${LOCALOBJDIR}/starchMetadataHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchFileHelpers.c -o  ${LOCALOBJDIR}/starchFileHelpers.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchSha1Digest.c -o  ${LOCALOBJDIR}/starchSha1Digest.o -iquote${MAIN} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} -c ${OBJDIR}/starchBase64Coding.c -o  ${LOCALOBJDIR}/starchBase64Coding.o -iquote${MAIN} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}

clean_force_flags:
ifneq ($(wildcard .forcestatic),)
	rm -f .forcestatic
endif

starchLibrary: dependencies
	mkdir -p ${LOCALSTARCHLIBDIR}
	${AR} rcs ${LOCALSTARCHLIBPATH} ${LOCALOBJDIR}/starchConstants.o  ${LOCALOBJDIR}/unstarchHelpers.o  ${LOCALOBJDIR}/starchHelpers.o  ${LOCALOBJDIR}/starchMetadataHelpers.o  ${LOCALOBJDIR}/starchFileHelpers.o ${LOCALOBJDIR}/starchSha1Digest.o ${LOCALOBJDIR}/starchBase64Coding.o

starchLibrary_debug: dependencies_debug
	mkdir -p ${LOCALSTARCHLIBDIR}
	${AR} rcs ${LOCALSTARCHLIBDEBUGPATH} ${LOCALOBJDIR}/starchConstants.o  ${LOCALOBJDIR}/unstarchHelpers.o  ${LOCALOBJDIR}/starchHelpers.o  ${LOCALOBJDIR}/starchMetadataHelpers.o  ${LOCALOBJDIR}/starchFileHelpers.o ${LOCALOBJDIR}/starchSha1Digest.o ${LOCALOBJDIR}/starchBase64Coding.o

starchLibrary_gprof: dependencies_gprof
	mkdir -p ${LOCALSTARCHLIBDIR}
	${AR} rcs ${LOCALSTARCHLIBGPROFPATH} ${LOCALOBJDIR}/starchConstants.o  ${LOCALOBJDIR}/unstarchHelpers.o  ${LOCALOBJDIR}/starchHelpers.o  ${LOCALOBJDIR}/starchMetadataHelpers.o  ${LOCALOBJDIR}/starchFileHelpers.o ${LOCALOBJDIR}/starchSha1Digest.o ${LOCALOBJDIR}/starchBase64Coding.o

starch: starchLibrary
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(BINDIR)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXFLAGS} -c starch.c -o $(LOCALOBJDIR)/starch.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/starch.o -o ${BINDIR}/starch ${LOCALSTARCHLIBPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}
	rm -f *~

starch_debug: starchLibrary_debug
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(BINDIR)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXDFLAGS} -c starch.c -o $(LOCALOBJDIR)/debug.starch.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/debug.starch.o -o ${BINDIR}/debug.starch ${LOCALSTARCHLIBDEBUGPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}
	rm -f *~

starch_gprof: starchLibrary_gprof
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(BINDIR)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXDFLAGS} -c starch.c -o $(LOCALOBJDIR)/gprof.starch.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/gprof.starch.o -o ${BINDIR}/gprof.starch ${LOCALSTARCHLIBGPROFPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}
	rm -f *~

unstarch: starchLibrary
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(BINDIR)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXFLAGS} -c unstarch.c -o $(LOCALOBJDIR)/unstarch.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/unstarch.o -o ${BINDIR}/unstarch ${LOCALSTARCHLIBPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}
	rm -f *~

unstarch_debug: starchLibrary_debug
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(BINDIR)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXDFLAGS} -c unstarch.c -o $(LOCALOBJDIR)/debug.unstarch.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/debug.unstarch.o -o ${BINDIR}/debug.unstarch ${LOCALSTARCHLIBDEBUGPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}
	rm -f *~

unstarch_gprof: starchLibrary_gprof
	rm -rf ${LOCALOBJDIR}
	mkdir -p $(BINDIR)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXGFLAGS} -c unstarch.c -o $(LOCALOBJDIR)/gprof.unstarch.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} $(LOCALOBJDIR)/gprof.unstarch.o -o ${BINDIR}/gprof.unstarch ${LOCALSTARCHLIBGPROFPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}
	rm -f *~

starchcluster: starchcat
	mkdir -p ${BINDIR} && touch starchcluster && cp starchcluster ${BINDIR}
	mkdir -p ${BINDIR} && touch starchcluster.gnu_parallel && cp starchcluster.gnu_parallel ${BINDIR}

starchcat: starchLibrary
	rm -rf ${LOCALOBJDIR}
	mkdir -p ${BINDIR}
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXFLAGS} -c starchcat.c -o $(LOCALOBJDIR)/starchcat.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/starchcat.o -o ${BINDIR}/starchcat ${LOCALSTARCHLIBPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}

starchcat_debug: starchLibrary_debug
	rm -rf ${LOCALOBJDIR}
	mkdir -p ${BINDIR}
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXDFLAGS} -c starchcat.c -o $(LOCALOBJDIR)/debug.starchcat.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/debug.starchcat.o -o ${BINDIR}/debug.starchcat ${LOCALSTARCHLIBDEBUGPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}

starchcat_gprof: starchLibrary_gprof
	rm -rf ${LOCALOBJDIR}
	mkdir -p ${BINDIR}
	mkdir -p ${LOCALOBJDIR}
	${CXX} ${CXXGFLAGS} -c starchcat.c -o $(LOCALOBJDIR)/gprof.starchcat.o -iquote${MAIN} -iquote$(PARTY3) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	${CXX} ${CXXGFLAGS} $(LOCALOBJDIR)/gprof.starchcat.o -o ${BINDIR}/gprof.starchcat ${LOCALSTARCHLIBGPROFPATH} ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH}

test: starch unstarch starchcat
ifeq (${PLATFORM},Linux)
	cp ${BINDIR}/starch ${TEST_LINUX_BINDIR}/starch
	cp ${BINDIR}/unstarch ${TEST_LINUX_BINDIR}/unstarch
	cp ${BINDIR}/starchcat ${TEST_LINUX_BINDIR}/starchcat
endif
ifeq (${PLATFORM},Darwin)
	cp ${BINDIR}/starch ${TEST_OSX_BINDIR}/starch
	cp ${BINDIR}/unstarch ${TEST_OSX_BINDIR}/unstarch
	cp ${BINDIR}/starchcat ${TEST_OSX_BINDIR}/starchcat
endif
	make -C ${TEST} all

clean:
	rm -rf $(LOCALSTARCHLIBPATH) ${LOCALSTARCHLIBDEBUGPATH}
	rm -f ${BINDIR}/*starch*
	rm -rf ${LOCALOBJDIR}
	-test -d ${LOCALJANSSONDIR} && { cd ${LOCALJANSSONDIR} && make clean && cd ${THISDIR}; }
	-test -d ${LOCALBZIP2DIR} && { cd ${LOCALBZIP2DIR} && make clean && cd ${THISDIR}; }
	-test -d ${LOCALZLIBDIR} && { cd ${LOCALZLIBDIR} && make -f Makefile.in clean && cd ${THISDIR}; }
