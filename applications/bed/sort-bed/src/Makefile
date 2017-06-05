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
BINDIR              = ../bin
OBJDIR             = objects
WARNINGS            = $(MEGAFLAGS) -Wall -Wextra -pedantic
BLDFLAGS            = ${WARNINGS} -O3 -std=c++11
SFLAGS              = -static

dependency_names    = starchConstants starchFileHelpers starchHelpers starchMetadataHelpers unstarchHelpers starchSha1Digest starchBase64Coding SortDetails Sort CheckSort
dependencies        = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(dependency_names)))

FLAGS               = $(SFLAGS) -s ${BLDFLAGS} ${LIBLOCATION} ${INCLUDES}
DFLAGS              = $(SFLAGS) -g -O0 -std=c++11 -Wall -fno-inline -pedantic ${LIBLOCATION} ${INCLUDES}
GPROFFLAGS          = $(SFLAGS) -O -std=c++11 -Wall -pedantic -pg ${LIBLOCATION} ${INCLUDES}

ifneq ($(shell uname -s),CYGWIN_NT-6.1)
	WARNINGS += -ansi
else
	WARNINGS += -U__STRICT_ANSI__
endif

build: $(BINDIR)/$(PROG) $(dependencies) update-sort-bed-slurm update-sort-bed-starch-slurm update-sort-bed-migrate-candidates

build_gprof: $(BINDIR)/gprof.$(PROG)

build_debug: $(BINDIR)/debug.$(PROG)

dependencies: $(dependencies)

update-sort-bed-slurm:
	mkdir -p $(BINDIR) && cp update-sort-bed-slurm.py $(BINDIR)/update-sort-bed-slurm

update-sort-bed-starch-slurm:
	mkdir -p $(BINDIR) && cp update-sort-bed-starch-slurm.py $(BINDIR)/update-sort-bed-starch-slurm

update-sort-bed-migrate-candidates:
	mkdir -p $(BINDIR) && cp update-sort-bed-migrate-candidates.py $(BINDIR)/update-sort-bed-migrate-candidates

$(BINDIR)/$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(FLAGS) $^ ${LIBRARIES}

$(BINDIR)/gprof.$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(GPROFFLAGS) $^ ${LIBRARIES}

$(BINDIR)/debug.$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(DFLAGS) $^ ${LIBRARIES}

$(OBJDIR)/%.o : $(LIB1)/%.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c $(BLDFLAGS) $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.o : $(LIB3)/%.c
	mkdir -p $(OBJDIR) && $(CXX) -c ${BLDFLAGS} $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.o : %.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c ${BLDFLAGS} $^ -o $@ ${INCLUDES}

clean:
	rm -rf ${OBJDIR}
	rm -f $(BINDIR)/$(PROG)
	rm -f $(BINDIR)/*.$(PROG)
	rm -f $(BINDIR)/update-sort-bed-slurm
	rm -f $(BINDIR)/update-sort-bed-starch-slurm
	rm -f $(BINDIR)/update-sort-bed-migrate-candidates
