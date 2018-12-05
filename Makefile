FOLDERS =samplers
FOLDERS+=simulators
FOLDERS+=methods
FOLDERS+=densities

.PHONY: all very_clean clean release debug

all:
	echo $(FOLDERS) \
	| tr " \n\t" "\0" | xargs -0 -I {} $(MAKE) -C {} all

very_clean:
	echo $(FOLDERS) \
	| tr " \n\t" "\0" | xargs -0 -I {} $(MAKE) -C {} clean

release:
	$(MAKE) -C methods NDEBUG=true

debug:
	$(MAKE) -C methods

clean:
	$(MAKE) -C methods clean
