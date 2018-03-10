BLDFLAGS                  = -Wall -Wextra -pedantic -std=c99
COMMONFLAGS               = -D__STDC_CONSTANT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1
CFLAGS                    = -O3
CDFLAGS                   = -DDEBUG=1 -g -O0 -fno-inline
CPFLAGS                   = -pg
LIBS                      = -lpthread
MAIN                      = ../../../..
INTERFACES                = $(MAIN)/interfaces
HEAD                      = ${INTERFACES}/general-headers
INCLUDES                 := -iquote"${CURDIR}" -iquote"${HEAD}"
DISTDIR                   = ../bin
OBJDIR                    = objects${POSTFIX}
WRAPPERDIR                = wrappers
PROG                      = convert2bed
SOURCE                    = convert2bed.c
SFLAGS                    = -static

all: build

.PHONY: build build_debug build_gprof clean

build: $(DISTDIR)/$(PROG)-${BINARY_TYPE}
	rm -rf $(OBJDIR)
build_debug: $(DISTDIR)/debug.$(PROG)-${BINARY_TYPE}
	rm -rf $(OBJDIR)
build_gprof: $(DISTDIR)/gprof.$(PROG)-${BINARY_TYPE}
	rm -rf $(OBJDIR)

$(DISTDIR)/$(PROG)-${BINARY_TYPE}: $(PROG).c
	mkdir -p $(DISTDIR) && $(CC) $(BLDFLAGS) ${MEGAFLAGS} $(COMMONFLAGS) $(CFLAGS) $(SFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

$(DISTDIR)/%-${BINARY_TYPE}: %.c
	mkdir -p $(DISTDIR) && $(CC) $(BLDFLAGS) ${MEGAFLAGS} $(COMMONFLAGS) $(CFLAGS) $(SFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

$(DISTDIR)/debug.$(PROG): $(PROG).c
	mkdir -p $(DISTDIR) && $(CC) $(BLDFLAGS) ${MEGAFLAGS} $(COMMONFLAGS) $(CDFLAGS) $(SFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

$(DISTDIR)/debug.%-${BINARY_TYPE}: %.c
	mkdir -p $(DISTDIR) && $(CC) $(BLDFLAGS) ${MEGAFLAGS} $(COMMONFLAGS) $(CDFLAGS) $(SFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

$(DISTDIR)/gprof.$(PROG): $(PROG).c gprof-helper.so
	mkdir -p $(DISTDIR) && $(CC) $(BLDFLAGS) ${MEGAFLAGS} $(COMMONFLAGS) $(CPFLAGS) $(SFLAGS) $(INCLUDES) $< -o $@ $(LIBS)
	@echo -e "\nNote: To profile convert2bed with gprof/pthreads, run:\n\t$$ LD_PRELOAD=/path/to/gprof-helper.so convert2bed"

$(DISTDIR)/gprof.%-${BINARY_TYPE}: %.c gprof-helper.so
	mkdir -p $(DISTDIR) && $(CC) $(BLDFLAGS) ${MEGAFLAGS} $(COMMONFLAGS) $(CPFLAGS) $(SFLAGS) $(INCLUDES) $< -o $@ $(LIBS)
	@echo -e "\nNote: To profile convert2bed with gprof/pthreads, run:\n\t$$ LD_PRELOAD=/path/to/gprof-helper.so convert2bed"

%.so: %.c
	$(CC) -shared -fPIC $^ -o $@ $(LIBS) -ldl

clean:
	rm -rf $(DISTDIR)
	rm -rf $(OBJDIR)
	rm -rf  *.so *~
