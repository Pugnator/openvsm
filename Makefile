CURENV=$(shell gcc -dumpmachine)
ifneq (, $(findstring mingw, $(CURENV)))
	MAKE=mingw32-make	
else ifneq (, $(findstring linux, $(CURENV)))
	MAKE=make	
else
	exit 0
endif

all:
	$(MAKE) -p dll
	$(MAKE) -C src
clean:
	$(MAKE) -C src clean
