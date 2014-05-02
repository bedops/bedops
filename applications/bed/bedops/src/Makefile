MAIN                = ../../../..
HEAD                = $(MAIN)/interfaces/general-headers
LIB1                = $(MAIN)/interfaces/src/data/measurement
LIB2                = $(MAIN)/interfaces/src/utility
LIB3                = $(MAIN)/interfaces/src/data/starch
TESTDIR             = ../test
TMPTESTDIR          = $(TESTDIR)/regression.tests
THISDIR             = ${shell pwd}
PARTY3              = ${THISDIR}/$(MAIN)/third-party
BOOSTLIBDIR         = $(PARTY3)/boost
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
INCLUDES            = -iquote$(HEAD) -I$(BOOSTLIBDIR) -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
LIBLOCATION         = -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBDIR}
LIBRARIES           = ${LOCALJANSSONLIB} ${LOCALBZIP2LIB} ${LOCALZLIBLIB}
BLDFLAGS            = -Wall -pedantic -O3 -std=c++11
SFLAGS              = -static -s

FLAGS               = $(SFLAGS) $(BLDFLAGS) $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o ${LIBLOCATION} ${INCLUDES}
DFLAGS              = $(SFLAGS) -g -O0 -std=c++11 -Wall -fno-inline -pedantic $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o ${LIBLOCATION} ${INCLUDES}
GPROFFLAGS          = $(SFLAGS) -O -std=c++11 -Wall -pedantic -pg $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o ${LIBLOCATION} ${INCLUDES}
SOURCE1             = Bedops.cpp
BINDIR              = ../bin
PROG                = bedops

build: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/$(PROG) $(FLAGS) ${LIBRARIES} $(SOURCE1) 

gprof: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/gprof.$(PROG) $(GPROFFLAGS) ${LIBRARIES} $(SOURCE1) 

debug: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/debug.$(PROG) $(DFLAGS) ${LIBRARIES} $(SOURCE1)

dependencies:
	rm -rf $(OBJDIR)
	mkdir -p $(OBJDIR)
	$(CXX) -c $(BLDFLAGS) $(LIB1)/NaN.cpp -o $(OBJDIR)/NaN.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB2)/Formats.cpp -o $(OBJDIR)/Formats.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchConstants.c -o $(OBJDIR)/starchConstants.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchFileHelpers.c -o $(OBJDIR)/starchFileHelpers.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchHelpers.c -o $(OBJDIR)/starchHelpers.o -iquote${HEAD} ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/starchMetadataHelpers.c -o $(OBJDIR)/starchMetadataHelpers.o ${INCLUDES}
	$(CXX) -c $(BLDFLAGS) $(LIB3)/unstarchHelpers.c -o $(OBJDIR)/unstarchHelpers.o ${INCLUDES}
	$(CXX) -c ${BLDFLAGS} ${LIB3}/starchSha1Digest.c -o  ${OBJDIR}/starchSha1Digest.o ${INCLUDES}
	$(CXX) -c ${BLDFLAGS} ${LIB3}/starchBase64Coding.c -o  ${OBJDIR}/starchBase64Coding.o ${INCLUDES}

test:
	mkdir -p $(TMPTESTDIR)
	cp $(TESTDIR)/TestPlan.xml $(TESTDIR)/Regression.java $(TMPTESTDIR)/
	cd $(TMPTESTDIR)/ && javac Regression.java
	printf "\n\nTo run regression tests:\n  cd $(TMPTESTDIR) && java Regression TestPlan.xml\n"

clean:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(BINDIR)/*.$(PROG)
	rm -rf $(OBJDIR)

clean_test:
	rm -rf $(TMPTESTDIR)
