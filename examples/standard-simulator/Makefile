.PHONY: all test clean

TARGETS=standard-simulator

all: $(TARGETS)

test: all
	./test-sweep.sh
	./test-rejection.sh
	./test-smc.sh

clean:
	rm -rf $(TARGETS)
