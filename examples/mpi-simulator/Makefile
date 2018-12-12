.PHONY: all test clean

TARGETS=mpi-simulator
CC=mpicc

all: $(TARGETS)

test: all
	./test-sweep.sh
	./test-rejection.sh
	./test-smc.sh

clean:
	rm -rf $(TARGETS)
