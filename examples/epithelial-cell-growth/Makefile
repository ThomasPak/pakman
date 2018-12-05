.PHONY: all test clean

EXECUTABLES =VertexBasedMonolayerSimulator
EXECUTABLES+=uniform-sampler
EXECUTABLES+=normal-sampler
EXECUTABLES+=normal-pdf

all: $(EXECUTABLES)

VertexBasedMonolayerSimulator:
	$(MAKE) -C $(CHASTE_BUILD) VertexBasedMonolayerSimulator
	cp $(CHASTE_BUILD)/projects/cell-chaste/apps/VertexBasedMonolayerSimulator .

test:
	./test.sh

clean:
	rm -rf $(EXECUTABLES)
