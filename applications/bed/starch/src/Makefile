HEAD                      = ../../../..
INTERFACES                = $(HEAD)/interfaces
MAIN                      = ${INTERFACES}/general-headers
MAINAPPDIR                = ../..
PARTY3                    = $(HEAD)/third-party
BINDIR                    = ../bin
THISDIR                   = ${shell pwd}
LIBSTARCH                 = libstarch${POSTFIX}.a
LIBSTARCHDEBUG            = libstarch_debug${POSTFIX}.a
LIBSTARCHGPROF            = libstarch_gprof${POSTFIX}.a
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
LOCALOBJDIR               = objects${POSTFIX}
INCLUDES                  = -iquote${MAIN} -iquote${HEAD} -iquote${PARTY3} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
LIBRARIES                 = ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB}
ARCH_VERSION              = v2.2
BIN_VERSION               = v2.4.27
TEST                      = ../test
TEST_LINUX_BINDIR         = ${TEST}/binaries/linux/${ARCH_VERSION}/bin
TEST_OSX_BINDIR           = ${TEST}/binaries/osx/${ARCH_VERSION}/bin
AR                        = ar
SFLAGS                    = -static
CXXFLAGS                  = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O2 -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 ${SFLAGS} -s
CXXDFLAGS                 = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O0 -g -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 -DDEBUG_VERBOSE=1 ${SFLAGS} -DDEBUG=1
CXXGFLAGS                 = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -DUSE_ZLIB -DUSE_BZLIB -O -Wformat -Wall -Wextra -Wswitch-enum -std=c++11 -DDEBUG_VERBOSE=1 ${SFLAGS} -DDEBUG=1 -pg

STARCH_NAMES              = starchConstants unstarchHelpers starchHelpers starchMetadataHelpers starchFileHelpers starchSha1Digest starchBase64Coding
STARCH_OBJECTS            = $(addprefix $(LOCALOBJDIR)/, $(addsuffix .o, $(STARCH_NAMES)))

.PHONY: starchcluster starch unstarch starchcat build build_debug build_gprof starch-diff starchstrip

build: starch unstarch starchcat starchcluster starch-diff starchstrip
	rm -rf ${LOCALOBJDIR}
build_debug: $(BINDIR)/debug.starch $(BINDIR)/debug.unstarch $(BINDIR)/debug.starchcat $(BINDIR)/debug.starchstrip starchcluster starch-diff
	rm -rf ${LOCALOBJDIR}
build_gprof: $(BINDIR)/gprof.starch $(BINDIR)/gprof.unstarch $(BINDIR)/gprof.starchcat $(BINDIR)/gprof.starchstrip starchcluster starch-diff
	rm -rf ${LOCALOBJDIR}

# Cancel implicit compilation rules
% : %.o
% : %.c

starch: $(BINDIR)/starch-$(BINARY_TYPE)
unstarch: $(BINDIR)/unstarch-$(BINARY_TYPE)
starchcat: $(BINDIR)/starchcat-$(BINARY_TYPE)
starchcluster: $(BINDIR)/starchcluster_gnuParallel-$(BINARY_TYPE) $(BINDIR)/starchcluster_sge-$(BINARY_TYPE) $(BINDIR)/starchcluster_slurm-$(BINARY_TYPE)
starch-diff: $(BINDIR)/starch-diff-$(BINARY_TYPE)
starchstrip: $(BINDIR)/starchstrip-$(BINARY_TYPE)

$(BINDIR)/%-$(BINARY_TYPE) : %.c $(LOCALSTARCHLIB) $(LIBRARIES)
	mkdir -p $(BINDIR)
	${CXX} ${CXXFLAGS} ${MEGAFLAGS} -c $*.c -o $(LOCALOBJDIR)/$*.o ${INCLUDES}
	${CXX} ${CXXFLAGS} ${MEGAFLAGS} $(LOCALOBJDIR)/$*.o -o $@ ${LOCALSTARCHLIB} ${LIBRARIES}

$(BINDIR)/debug.% : %.c $(LOCALSTARCHLIB) $(LIBRARIES)
	mkdir -p $(BINDIR)
	${CXX} ${CXXDFLAGS} ${MEGAFLAGS} -c $*.c -o $(LOCALOBJDIR)/$*.o ${INCLUDES}
	${CXX} ${CXXDFLAGS} ${MEGAFLAGS} $(LOCALOBJDIR)/$*.o -o $@-$(BINARY_TYPE) ${LOCALSTARCHLIB} ${LIBRARIES}

$(BINDIR)/gprof.% : %.c $(LOCALSTARCHLIB) $(LIBRARIES)
	mkdir -p $(BINDIR)
	${CXX} ${CXXGFLAGS} ${MEGAFLAGS} -c $*.c -o $(LOCALOBJDIR)/$*.o ${INCLUDES}
	${CXX} ${CXXGFLAGS} ${MEGAFLAGS} $(LOCALOBJDIR)/$*.o -o $@ ${LOCALSTARCHLIB} ${LIBRARIES}

$(BINDIR)/% : %.tcsh
	mkdir -p $(BINDIR)
	cp $< $@

$(BINDIR)/%-$(BINARY_TYPE) : %.tcsh
	mkdir -p $(BINDIR)
	cp $< $@

$(BINDIR)/% : %.py
	mkdir -p $(BINDIR)
	cp $< $@

$(BINDIR)/%-$(BINARY_TYPE) : %.py
	mkdir -p $(BINDIR)
	cp $< $@

starchLibrary: $(LOCALSTARCHLIB)

$(LOCALSTARCHLIB) : $(STARCH_OBJECTS)
	mkdir -p $(LOCALSTARCHLIBDIR)
	${AR} rcs $@ $^

$(LOCALOBJDIR)/%.o : $(OBJDIR)/%.c $(LIBRARIES)
	mkdir -p $(LOCALOBJDIR)
	${CXX} ${CXXFLAGS} ${MEGAFLAGS} -c $< -o $@ ${INCLUDES}

test: starch unstarch starchcat
	cp ${BINDIR}/starch-${BINARY_TYPE} ${TEST_LINUX_BINDIR}/starch-${BINARY_TYPE}
	cp ${BINDIR}/unstarch-${BINARY_TYPE} ${TEST_LINUX_BINDIR}/unstarch-${BINARY_TYPE}
	cp ${BINDIR}/starchcat-${BINARY_TYPE} ${TEST_LINUX_BINDIR}/starchcat-${BINARY_TYPE}
	make -C ${TEST} all

clean:
	rm -rf $(LOCALSTARCHLIBDIR)
	rm -f ${LOCALSTARCHLIBDEBUG}
	rm -f ${LOCALSTARCHLIBGPROF}
	rm -rf ${LOCALOBJDIR}
	rm -rf ${BINDIR}
