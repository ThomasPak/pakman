.PHONY: all test clean

DIRS=$(shell echo */)

all:
	echo */ | tr " \n\t" "\0" | xargs -0 -I {} $(MAKE) -C {} all

test:
	echo */ | tr " \n\t" "\0" | xargs -0 -I {} $(MAKE) -C {} test

clean:
	echo */ | tr " \n\t" "\0" | xargs -0 -I {} $(MAKE) -C {} clean
