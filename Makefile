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
	$(MAKE) -C bin2source clean
	$(MAKE) -C bin2source
	$(MAKE) -C externals/lua-5.3.1 linux
	cp externals/lua-5.3.1/src/luac tools/
	$(MAKE) -C src
clean:
	$(MAKE) -C src clean
