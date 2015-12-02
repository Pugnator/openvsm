ifdef SystemRoot
   MAKE=make
   RM = rm -f
   CP = copy
   FixPath = $(subst /,\,$1)
else
   ifeq ($(shell uname), Linux)
	  RM = rm -f
	  CP = cp
      MAKE=make
      FixPath = $1
   endif
endif

all:
	mkdir -p tools
	mkdir -p dll
	$(MAKE) tools
	$(MAKE) lua
	$(MAKE) -C src

clean:
	$(MAKE) -C bin2source clean
	$(MAKE) -C src clean
	$(MAKE) -C externals/lua-5.3.1/src clean
	$(RM) tools/*

tools:
	$(MAKE) -C bin2source

lua:
	$(MAKE) -C externals/lua-5.3.1/src linux
	cp externals/lua-5.3.1/src/luac.exe tools/
	$(MAKE) -C externals/lua-5.3.1/src clean
	$(MAKE) -C externals/lua-5.3.1/src mingw

.PHONY: all clean tools lua