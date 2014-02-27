CC               = gcc
CXX              = g++
WARNINGS         = -Wall -Wextra -pedantic
NONSTATICFLAGS   = -ansi ${WARNINGS} -O3
ifeq ($(wildcard .forcestatic),)
	SFLAGS = $(NONSTATICFLAGS)
else
	SFLAGS = -static -s $(NONSTATICFLAGS)
endif
FLAGS	         = $(SFLAGS) -std=c++11
DFLAGS	         = ${WARNINGS} -O0 -g -fno-inline -std=c++11
GPROFFLAGS       = ${WARNINGS} -O -pg -std=c++11
MAIN             = ../../../../..
HEAD             = ${MAIN}/interfaces/general-headers
SOURCE1	         = Wig2Bed.cpp
BIN              = ../../bin
PROG             = wig2bed_bin

.cpp.o:; $(CXX) -c $(FLAGS) $<

run: dependencies
	mkdir -p $(BIN) && $(CXX) -o $(BIN)/$(PROG) $(FLAGS) $(SOURCE1) -iquote${HEAD}

gprof: dependencies
	mkdir -p $(BIN) && $(CXX) -o $(BIN)/gprof.$(PROG) $(GPROFFLAGS) $(SOURCE1) -iquote${HEAD}

debug: dependencies
	mkdir -p $(BIN) && $(CXX) -o $(BIN)/debug.$(PROG) $(DFLAGS) $(SOURCE1) -iquote${HEAD}

dependencies:
ifneq ($(wildcard .forcestatic),)
	rm -rf .forcestatic
endif

clean:
	rm -f $(BIN)/$(PROG) $(BIN)/debug.$(PROG) $(BIN)/gprof.$(PROG)
