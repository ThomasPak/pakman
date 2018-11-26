.PHONY: all clean

EXECUTABLES=$(shell ls *.cc | awk -F. '{print $$1}')

CXX=mpic++

all: $(EXECUTABLES)

clean:
	rm -rf $(EXECUTABLES)
