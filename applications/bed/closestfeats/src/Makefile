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
OBJDIR              = objects
INCLUDES            = -iquote$(HEAD) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
LIBLOCATION         = -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBDIR}
LIBRARIES           = ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB}
BLDFLAGS            = -Wall -pedantic -O3 -std=c++11
SFLAGS              = -static

FLAGS               = $(SFLAGS) -s $(BLDFLAGS) $(OBJDIR)/NaN.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o ${LIBLOCATION} ${INCLUDES}
DFLAGS              = $(SFLAGS) -g -O0 -std=c++11 -Wall -fno-inline -pedantic $(OBJDIR)/NaN.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o ${LIBLOCATION} ${INCLUDES}
GPROFFLAGS          = $(SFLAGS) -O -std=c++11 -Wall -pedantic -pg $(OBJDIR)/NaN.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o ${LIBLOCATION} ${INCLUDES}
SOURCE1             = ClosestFeature.cpp
BINDIR              = ../bin
PROG                = closest-features

build: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/$(PROG) $(FLAGS) ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB} $(SOURCE1) 

build_gprof: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/gprof.$(PROG) $(FLAGS) ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB} $(SOURCE1)

build_debug: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/debug.$(PROG) $(DFLAGS) ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB} $(SOURCE1)

dependencies:
	rm -rf $(OBJDIR)
	mkdir -p $(OBJDIR)
	$(CXX) -c $(BLDFLAGS) $(LIB1)/NaN.cpp -o $(OBJDIR)/NaN.o -iquote${HEAD}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchConstants.c -o $(OBJDIR)/starchConstants.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchFileHelpers.c -o $(OBJDIR)/starchFileHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchHelpers.c -o $(OBJDIR)/starchHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchMetadataHelpers.c -o $(OBJDIR)/starchMetadataHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/unstarchHelpers.c -o $(OBJDIR)/unstarchHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c ${BLDFLAGS} ${LIB3}/starchSha1Digest.c -o  ${OBJDIR}/starchSha1Digest.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c ${BLDFLAGS} ${LIB3}/starchBase64Coding.c -o  ${OBJDIR}/starchBase64Coding.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}

clean:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(BINDIR)/*.$(PROG)
	rm -rf $(OBJDIR)
