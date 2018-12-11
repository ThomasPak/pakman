.PHONY: all clean

CXXFLAGS = -std=c++11

ifeq ($(NDEBUG),true)
    CXXFLAGS+= -O3
    CXXFLAGS+= -DNDEBUG
else
    CXXFLAGS+= -g
    CXXFLAGS+= -rdynamic
endif

ifeq ($(PROF),true)
    CXXFLAGS+= -pg
    CXXFLAGS+= -DNDEBUG
    CXXFLAGS+= -g
    CXXFLAGS+= -rdynamic
endif

MPICXX ?= mpic++

MPIFLAGS = -pthread

EXECUTABLES = rejection
EXECUTABLES+= mcmc
EXECUTABLES+= smc
EXECUTABLES+= sweep

EXECUTABLES+= rejection-mpi
EXECUTABLES+= mcmc-mpi
EXECUTABLES+= smc-mpi
EXECUTABLES+= sweep-mpi

MAINOBJS = $(EXECUTABLES:%=%.o)

CXXFILES = $(shell ls *.cc)
OBJFILES = $(CXXFILES:%.cc=%.o)
OBJFILES:= $(filter-out $(MAINOBJS), $(OBJFILES))

MPICXXFILES = $(shell ls mpi/*.cc | xargs -n 1 basename)
MPIOBJFILES = $(MPICXXFILES:%.cc=%.o)
MPIOBJFILES:= $(filter-out $(MAINOBJS), $(MPIOBJFILES))

DEPFILES = $(OBJFILES:.o=.d)
DEPFILES+= $(MPIOBJFILES:.o=.d)
DEPFILES+= $(MAINOBJS:.o=.d)

all: $(EXECUTABLES)

install: all
	[ $(INSTALLDIR) ] || { echo "Please set environment variable INSTALLDIR"; exit 2; }
	cp pakman $(EXECUTABLES) $(INSTALLDIR)/bin

softinstall: all
	[ $(INSTALLDIR) ] || { echo "Please set environment variable INSTALLDIR"; exit 2; }
	ln -s `echo pakman $(EXECUTABLES) | xargs realpath` $(INSTALLDIR)/bin

uninstall:
	[ $(INSTALLDIR) ] || { echo "Please set environment variable INSTALLDIR"; exit 2; }
	cd $(INSTALLDIR)/bin && rm -rf pakman $(EXECUTABLES)

-include $(DEPFILES)

# Serial executables
rejection: $(OBJFILES) rejection.o
	$(CXX) $(CXXFLAGS) -o $@ $^

mcmc: $(OBJFILES) mcmc.o
	$(CXX) $(CXXFLAGS) -o $@ $^

smc: $(OBJFILES) smc.o
	$(CXX) $(CXXFLAGS) -o $@ $^

sweep: $(OBJFILES) sweep.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Serial dependencies
%.d: %.cc
	@$(CXX) -MM $(CXXFLAGS) $< | \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

# MPI object files
%.o: mpi/%.cc
	$(MPICXX) $(CXXFLAGS) $(MPIFLAGS) -c -o $@ $<

# MPI executables
rejection-mpi: $(OBJFILES) $(MPIOBJFILES) rejection-mpi.o
	$(MPICXX) $(CXXFLAGS) $(MPIFLAGS) -o $@ $^

mcmc-mpi: $(OBJFILES) $(MPIOBJFILES) mcmc-mpi.o
	$(MPICXX) $(CXXFLAGS) $(MPIFLAGS) -o $@ $^

smc-mpi: $(OBJFILES) $(MPIOBJFILES) smc-mpi.o
	$(MPICXX) $(CXXFLAGS) $(MPIFLAGS) -o $@ $^

sweep-mpi: $(OBJFILES) $(MPIOBJFILES) sweep-mpi.o
	$(MPICXX) $(CXXFLAGS) $(MPIFLAGS) -o $@ $^

# MPI dependencies
%.d: mpi/%.cc
	@$(MPICXX) -MM $(CXXFLAGS) $(MPIFLAGS) $< | \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

clean:
	find . -name "*.o" | xargs rm -rf
	find . -name "*.d" | xargs rm -rf
	rm -rf $(EXECUTABLES)
