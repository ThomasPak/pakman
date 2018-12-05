.PHONY: all test clean

EXECUTABLES =binomial-sampler
EXECUTABLES+=uniform-sampler

all: $(EXECUTABLES)

test: all
	./test.sh

clean:
	rm -rf $(EXECUTABLES)
