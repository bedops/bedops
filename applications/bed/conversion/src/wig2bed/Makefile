WARNINGS   = -Wall -Wextra -pedantic
BLDFLAGS   = -ansi ${WARNINGS} -O3
SFLAGS     = -static

FLAGS      = ${SFLAGS} -s ${BLDFLAGS} -std=c++11
DFLAGS     = ${SFLAGS} ${WARNINGS} -O0 -g -fno-inline -std=c++11
GPROFFLAGS = ${SFLAGS} ${WARNINGS} -O -pg -std=c++11
MAIN       = ../../../../..
HEAD       = ${MAIN}/interfaces/general-headers
SOURCE1    = Wig2Bed.cpp
BINDIR     = ../../bin
PROG       = wig2bed_bin

build:
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/$(PROG) $(FLAGS) $(SOURCE1) -iquote${HEAD}

gprof:
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/gprof.$(PROG) $(GPROFFLAGS) $(SOURCE1) -iquote${HEAD}

debug:
	mkdir -p $(BINDIR) && $(CXX) -o $(BINDIR)/debug.$(PROG) $(DFLAGS) $(SOURCE1) -iquote${HEAD}

clean:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(BINDIR)/*.$(PROG)
