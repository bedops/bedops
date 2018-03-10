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

PROG                = sort-bed-${BINARY_TYPE}
BINDIR              = ../bin
OBJDIR              = objects-${BINARY_TYPE}
WARNINGS            = -Wall -Wextra -pedantic
BLDFLAGS            = ${WARNINGS} -O3 -std=c++11 ${MEGAFLAGS}
SFLAGS              = -static

dependency_names    = starchConstants starchFileHelpers starchHelpers starchMetadataHelpers unstarchHelpers starchSha1Digest starchBase64Coding SortDetails Sort CheckSort
dependencies        = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(dependency_names)))
debug_dependencies  = $(addprefix $(OBJDIR)/, $(addsuffix .do, $(dependency_names)))

FLAGS               = $(SFLAGS) ${MEGAFLAGS} -s ${BLDFLAGS} ${LIBLOCATION} ${INCLUDES}
DFLAGS              = $(SFLAGS) ${MEGAFLAGS} -g -O0 -std=c++11 -Wall -fno-inline -pedantic ${LIBLOCATION} ${INCLUDES}
GPROFFLAGS          = $(SFLAGS) ${MEGAFLAGS} -O -std=c++11 -Wall -pedantic -pg ${LIBLOCATION} ${INCLUDES}

ifneq ($(shell uname -s),CYGWIN_NT-6.1)
	WARNINGS += -ansi
else
	WARNINGS += -U__STRICT_ANSI__
endif

build: $(dependencies) $(BINDIR)/$(PROG) update-sort-bed-slurm-${BINARY_TYPE} update-sort-bed-starch-slurm-${BINARY_TYPE} update-sort-bed-migrate-candidates-${BINARY_TYPE}
	rm -rf ${OBJDIR}

build_gprof: $(BINDIR)/gprof.$(PROG)
	rm -rf ${OBJDIR}

build_debug: $(BINDIR)/debug.$(PROG)
	rm -rf ${OBJDIR}

dependencies: $(dependencies)

debug_dependencies: $(debug_dependencies)

update-sort-bed-slurm-${BINARY_TYPE}:
	mkdir -p $(BINDIR) && cp update-sort-bed-slurm.py $(BINDIR)/update-sort-bed-slurm-${BINARY_TYPE}

update-sort-bed-starch-slurm-${BINARY_TYPE}:
	mkdir -p $(BINDIR) && cp update-sort-bed-starch-slurm.py $(BINDIR)/update-sort-bed-starch-slurm-${BINARY_TYPE}

update-sort-bed-migrate-candidates-${BINARY_TYPE}:
	mkdir -p $(BINDIR) && cp update-sort-bed-migrate-candidates.py $(BINDIR)/update-sort-bed-migrate-candidates-${BINARY_TYPE}

$(BINDIR)/$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(FLAGS) $^ ${LIBRARIES}

$(BINDIR)/gprof.$(PROG) : $(dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(GPROFFLAGS) $^ ${LIBRARIES}

$(BINDIR)/debug.$(PROG) : $(debug_dependencies)
	mkdir -p $(BINDIR) && $(CXX) -o $@ $(DFLAGS) $^ ${LIBRARIES}
	rm -rf ${OBJDIR}

$(OBJDIR)/%.o : $(LIB1)/%.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c $(BLDFLAGS) $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.o : $(LIB3)/%.c
	mkdir -p $(OBJDIR) && $(CXX) -c ${BLDFLAGS} $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.o : %.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c ${BLDFLAGS} $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.do : $(LIB1)/%.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c $(DFLAGS) $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.do : $(LIB3)/%.c
	mkdir -p $(OBJDIR) && $(CXX) -c ${DFLAGS} $^ -o $@ ${INCLUDES}

$(OBJDIR)/%.do : %.cpp
	mkdir -p $(OBJDIR) && $(CXX) -c ${DFLAGS} $^ -o $@ ${INCLUDES}

clean:
	rm -rf ${OBJDIR}
	rm -rf $(BINDIR)
