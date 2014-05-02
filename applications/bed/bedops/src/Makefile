MAIN                = ../../../..
HEAD                = $(MAIN)/interfaces/general-headers
CC                  = gcc
CXX                 = g++
LIB1                = $(MAIN)/interfaces/src/data/measurement
LIB2                = $(MAIN)/interfaces/src/utility
LIB3                = $(MAIN)/interfaces/src/data/starch
TESTDIR             = ../test
TMPTESTDIR          = $(TESTDIR)/regression.tests
THISDIR	            = ${shell pwd}
PARTY3              = ${THISDIR}/$(MAIN)/third-party
BOOSTLIBDIR         = $(PARTY3)/boost
LIBJANSSON          = libjansson.a
LIBBZIP2            = libbz2.a
LIBZLIB             = libz.a
LOCALJANSSONDIR     = ${PARTY3}/jansson
LOCALJANSSONLIBDIR  = ${LOCALJANSSONDIR}/lib
LOCALJANSSONINCDIR  = ${LOCALJANSSONDIR}/include
LOCALJANSSONLIBPATH = ${LOCALJANSSONLIBDIR}/${LIBJANSSON}
LOCALBZIP2DIR       = ${PARTY3}/bzip2
LOCALBZIP2LIBDIR    = ${LOCALBZIP2DIR}
LOCALBZIP2LIBPATH   = ${LOCALBZIP2LIBDIR}/${LIBBZIP2}
LOCALBZIP2INCDIR    = ${LOCALBZIP2DIR}
LOCALZLIBDIR        = ${PARTY3}/zlib
LOCALZLIBLIBDIR     = ${LOCALZLIBDIR}
LOCALZLIBLIBPATH    = ${LOCALZLIBLIBDIR}/${LIBZLIB}
LOCALZLIBINCDIR     = ${LOCALZLIBDIR}
OBJDIR              = objects
INCLUDES            = -iquote$(HEAD) -I$(BOOSTLIBDIR)
NONSTATICFLAGS      = -Wall -pedantic -O3 ${INCLUDES} -std=c++11
SFLAGS              = -static -s -I$(HEAD)

FLAGS               = $(SFLAGS) $(NONSTATICFLAGS) $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBLIBDIR} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
DFLAGS              = $(SFLAGS) -g -O0 -std=c++11 -Wall -fno-inline -pedantic $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBLIBDIR} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
GPROFFLAGS          = $(SFLAGS) -O -std=c++11 -Wall -pedantic -pg $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o $(OBJDIR)/starchConstants.o $(OBJDIR)/starchFileHelpers.o $(OBJDIR)/starchHelpers.o $(OBJDIR)/starchMetadataHelpers.o $(OBJDIR)/unstarchHelpers.o $(OBJDIR)/starchSha1Digest.o $(OBJDIR)/starchBase64Coding.o -L${LOCALJANSSONLIBDIR} -L${LOCALBZIP2LIBDIR} -L${LOCALZLIBLIBDIR} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
SOURCE1	            = Bedops.cpp
BINDIR              = ../bin
PROG                = bedops

build: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/$(PROG) $(FLAGS) ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH} $(SOURCE1) 

gprof: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/gprof.$(PROG) $(GPROFFLAGS) ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH} $(SOURCE1) 

nonstatic: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/nonstatic.$(PROG) $(NONSTATICFLAGS) $(SOURCE1) $(OBJDIR)/NaN.o $(OBJDIR)/Formats.o

dependencies:
	rm -rf $(OBJDIR)
	mkdir -p $(OBJDIR)
	$(CXX) -c $(NONSTATICFLAGS) $(LIB1)/NaN.cpp -o $(OBJDIR)/NaN.o -iquote${HEAD}
	$(CXX) -c $(NONSTATICFLAGS) $(LIB2)/Formats.cpp -o $(OBJDIR)/Formats.o -iquote${HEAD}
	$(CXX) -c $(NONSTATICFLAGS) $(LIB3)/starchConstants.c -o $(OBJDIR)/starchConstants.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(NONSTATICFLAGS) $(LIB3)/starchFileHelpers.c -o $(OBJDIR)/starchFileHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(NONSTATICFLAGS) $(LIB3)/starchHelpers.c -o $(OBJDIR)/starchHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(NONSTATICFLAGS) $(LIB3)/starchMetadataHelpers.c -o $(OBJDIR)/starchMetadataHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c $(NONSTATICFLAGS) $(LIB3)/unstarchHelpers.c -o $(OBJDIR)/unstarchHelpers.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c ${NONSTATICFLAGS} ${LIB3}/starchSha1Digest.c -o  ${OBJDIR}/starchSha1Digest.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}
	$(CXX) -c ${NONSTATICFLAGS} ${LIB3}/starchBase64Coding.c -o  ${OBJDIR}/starchBase64Coding.o -iquote${HEAD} -I${LOCALJANSSONINCDIR} -I${LOCALBZIP2INCDIR} -I${LOCALZLIBINCDIR}

debug: dependencies
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/debug.$(PROG) $(DFLAGS) ${LOCALJANSSONLIBPATH} ${LOCALBZIP2LIBPATH} ${LOCALZLIBLIBPATH} $(SOURCE1)

test:
	mkdir -p $(TMPTESTDIR)
	cp $(TESTDIR)/TestPlan.xml $(TESTDIR)/Regression.java $(TMPTESTDIR)/
	cd $(TMPTESTDIR)/ && javac Regression.java
	printf "\n\nTo run regression tests: cd $(TMPTESTDIR) && java Regression TestPlan.xml\n"

clean:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(BINDIR)/*.$(PROG)
	rm -rf $(OBJDIR)

clean_test:
	rm -rf $(TMPTESTDIR)
