.PHONY: all test clean

EXECUTABLES =VertexBasedMonolayerSimulator
EXECUTABLES+=Simulator
EXECUTABLES+=PersistentSimulator
EXECUTABLES+=uniform-sampler
EXECUTABLES+=normal-sampler
EXECUTABLES+=normal-pdf

all: $(EXECUTABLES)

VertexBasedMonolayerSimulator:
	[ -z $${CHASTE_BUILD+x} ] && exit ; \
	$(MAKE) -C $(CHASTE_BUILD) VertexBasedMonolayerSimulator ; \
	cp $(CHASTE_BUILD)/projects/cell-chaste/apps/VertexBasedMonolayerSimulator .

Simulator:
	[ -z $${CHASTE_BUILD+x} ] && exit ; \
	$(MAKE) -C $(CHASTE_BUILD) Simulator ; \
	cp $(CHASTE_BUILD)/projects/cell-chaste/apps/Simulator .

PersistentSimulator:
	[ -z $${CHASTE_BUILD+x} ] && exit ; \
	$(MAKE) -C $(CHASTE_BUILD) PersistentSimulator ; \
	cp $(CHASTE_BUILD)/projects/cell-chaste/apps/PersistentSimulator .

test:
	./test.sh

clean:
	rm -rf $(EXECUTABLES)
