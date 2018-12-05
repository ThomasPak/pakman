.PHONY: all test clean

EXECUTABLES =VertexBasedMonolayerSimulator
EXECUTABLES+=uniform-sampler

all: $(EXECUTABLES)

VertexBasedMonolayerSimulator:
	$(MAKE) -C $(CHASTE_BUILD) VertexBasedMonolayerSimulator
	cp $(CHASTE_BUILD)/projects/cell-chaste/apps/VertexBasedMonolayerSimulator .

test:
	./test.sh

clean:
	rm -rf $(EXECUTABLES)
