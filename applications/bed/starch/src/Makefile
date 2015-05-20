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

STARCH_NAMES              = starchConstants unstarchHelpers starchHelpers starchMetadataHelpers starchFileHelpers starchSha1Digest starchBase64Coding
STARCH_OBJECTS            = $(addprefix $(LOCALOBJDIR)/, $(addsuffix .o, $(STARCH_NAMES)))

.PHONY: starchcluster starch unstarch starchcat build build_debug build_gprof

build: starch unstarch starchcat starchcluster
build_debug: $(BINDIR)/debug.starch $(BINDIR)/debug.unstarch $(BINDIR)/debug.starchcat
build_gprof: $(BINDIR)/gprof.starch $(BINDIR)/gprof.unstarch $(BINDIR)/gprof.starchcat

# Cancel implicit compilation rules
% : %.o
% : %.c

starch: $(BINDIR)/starch
unstarch: $(BINDIR)/unstarch
starchcat: $(BINDIR)/starchcat
starchcluster: $(BINDIR)/starchcluster_gnuParallel.tcsh $(BINDIR)/starchcluster_sge.tcsh

$(BINDIR)/% : %.c $(LOCALSTARCHLIB) $(LIBRARIES)
	mkdir -p $(BINDIR)
	${CXX} ${CXXFLAGS} -c $*.c -o $(LOCALOBJDIR)/$*.o ${INCLUDES}
	${CXX} ${CXXFLAGS} $(LOCALOBJDIR)/$*.o -o $@ ${LOCALSTARCHLIB} ${LIBRARIES}

$(BINDIR)/debug.% : %.c $(LOCALSTARCHLIB) $(LIBRARIES)
	mkdir -p $(BINDIR)
	${CXX} ${CXXDFLAGS} -c $*.c -o $(LOCALOBJDIR)/$*.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} $(LOCALOBJDIR)/$*.o -o $@ ${LOCALSTARCHLIB} ${LIBRARIES}

$(BINDIR)/gprof.% : %.c $(LOCALSTARCHLIB) $(LIBRARIES)
	mkdir -p $(BINDIR)
	${CXX} ${CXXGFLAGS} -c $*.c -o $(LOCALOBJDIR)/$*.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} $(LOCALOBJDIR)/$*.o -o $@ ${LOCALSTARCHLIB} ${LIBRARIES}

$(BINDIR)/%.tcsh : %.tcsh
	mkdir -p $(BINDIR)
	cp $< $@

starchLibrary: $(LOCALSTARCHLIB)

$(LOCALSTARCHLIB) : $(STARCH_OBJECTS)
	mkdir -p $(LOCALSTARCHLIBDIR)
	${AR} rcs $@ $^

$(LOCALOBJDIR)/%.o : $(OBJDIR)/%.c $(LIBRARIES)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXFLAGS} -c $< -o $@ ${INCLUDES}

test: starch unstarch starchcat
	cp ${BINDIR}/starch ${TEST_LINUX_BINDIR}/starch
	cp ${BINDIR}/unstarch ${TEST_LINUX_BINDIR}/unstarch
	cp ${BINDIR}/starchcat ${TEST_LINUX_BINDIR}/starchcat
	make -C ${TEST} all

clean:
	rm -f $(LOCALSTARCHLIB)
	rm -f ${LOCALSTARCHLIBDEBUG}
	rm -f ${LOCALSTARCHLIBGPROF}
	rm -f ${BINDIR}/*starch*
	rm -rf ${LOCALOBJDIR}
