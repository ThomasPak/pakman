.PHONY: all test clean

DIRS=$(shell echo */)

ll:
	echo -n */ | xargs -I{} -n 1 -d" " $(MAKE) -C {} all

test:
	echo -n */ | xargs -I{} -n 1 -d" " $(MAKE) -C {} test

clean:
	echo -n */ | xargs -I{} -n 1 -d" " $(MAKE) -C {} clean
