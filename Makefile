CURENV=$(shell gcc -dumpmachine)
ifneq (, $(findstring mingw, $(CURENV)))
	MAKE=make
else ifneq (, $(findstring linux, $(CURENV)))
	MAKE=make
else
	exit 0
endif

all:
	mkdir -p dll
	mkdir -p tools
	$(MAKE) -C bin2source clean
	$(MAKE) -C bin2source
	$(MAKE) -C externals/lua-5.3.1/src linux
	cp externals/lua-5.3.1/src/luac.exe tools/
	$(MAKE) -C externals/lua-5.3.1/src clean
	$(MAKE) -C externals/lua-5.3.1/src mingw
	$(MAKE) -C src
clean:
	$(MAKE) -C src clean
	rm -f tools/*

.PHONY: all clean