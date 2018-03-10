MAIN                = ../../../..
HEAD                = $(MAIN)/interfaces/general-headers
LIB1                = $(MAIN)/interfaces/src/data/measurement
LIB2                = $(MAIN)/interfaces/src/utility
LIB3                = $(MAIN)/interfaces/src/data/starch
THISDIR             = ${shell pwd}
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
OBJDIR              = objects_${BINARY_TYPE}
INCLUDES            = -iquote$(HEAD) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
LIBLOCATION         = -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBDIR}
LIBRARIES           = ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB}
BLDFLAGS            = -Wall -pedantic -O3 -std=c++11
SFLAGS              = -static

dependency_names    = NaN starchConstants starchFileHelpers starchHelpers starchMetadataHelpers unstarchHelpers starchSha1Digest starchBase64Coding
dependencies        = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(dependency_names)))
FLAGS               = $(SFLAGS) ${MEGAFLAGS} -s $(BLDFLAGS) $(dependencies) ${LIBLOCATION} ${INCLUDES}
DFLAGS              = $(SFLAGS) ${MEGAFLAGS} -g -O0 -std=c++11 -Wall -fno-inline -pedantic $(dependencies) ${LIBLOCATION} ${INCLUDES}
GPROFFLAGS          = $(SFLAGS) ${MEGAFLAGS} -O -std=c++11 -Wall -pedantic -pg $(dependencies) ${LIBLOCATION} ${INCLUDES}
SOURCE1             = ClosestFeature.cpp
BINDIR              = ../bin
PROG                = closest-features-${BINARY_TYPE}

build: $(BINDIR)/$(PROG)
	rm -rf $(OBJDIR)

build_gprof: $(BINDIR)/gprof.$(PROG)
	rm -rf $(OBJDIR)

build_debug: $(BINDIR)/debug.$(PROG)
	rm -rf $(OBJDIR)

dependencies: $(dependencies)

$(BINDIR)/$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(FLAGS) ${LIBRARIES} $(SOURCE1)

$(BINDIR)/gprof.$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(GPROFFLAGS) ${LIBRARIES} $(SOURCE1)

$(BINDIR)/debug.$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(DFLAGS) ${LIBRARIES} $(SOURCE1)

$(OBJDIR)/%.o : $(LIB1)/%.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c $(BLDFLAGS) $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.o : $(LIB3)/%.c
	mkdir -p $(OBJDIR) && $(CXX) -c ${BLDFLAGS} $^ -o $@ ${INCLUDES}

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)
