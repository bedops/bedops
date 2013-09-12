CC               = g++
NONSTATICFLAGS   = -ansi -Wall -pedantic -O3 -s
ifeq ($(wildcard .forcestatic),)
	SFLAGS = $(NONSTATICFLAGS)
else
	SFLAGS = -static $(NONSTATICFLAGS)
endif
FLAGS	         = $(SFLAGS) -std=c++11
DFLAGS	         = -Wall -pedantic -O0 -g -std=c++11
GPROFFLAGS       = -Wall -pedantic -O -pg -std=c++11
MAIN             = ../../../../..
HEAD             = ${MAIN}/interfaces/general-headers
SOURCE1	         = Wig2Bed.cpp
BIN              = ../../bin
PROG             = wig2bed_bin

.cpp.o:; $(CC) -c $(FLAGS) $<

run: dependencies
	mkdir -p $(BIN) && $(CC) -o $(BIN)/$(PROG) $(FLAGS) $(SOURCE1) -iquote${HEAD}

gprof: dependencies
	mkdir -p $(BIN) && $(CC) -o $(BIN)/gprof.$(PROG) $(GPROFFLAGS) $(SOURCE1) -iquote${HEAD}

debug: dependencies
	mkdir -p $(BIN) && $(CC) -o $(BIN)/debug.$(PROG) $(DFLAGS) $(SOURCE1) -iquote${HEAD}

dependencies:
ifneq ($(wildcard .forcestatic),)
	rm -rf .forcestatic
endif

clean:
	rm -f $(BIN)/$(PROG) $(BIN)/debug.$(PROG) $(BIN)/gprof.$(PROG)
