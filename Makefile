LUATOOLS=""

ifdef SystemRoot
   MAKE=mingw32-make
   RM = rm -f
   CP = copy
   LUATOOLS=win32lua
   FixPath = $(subst /,\,$1)
else
	ifeq ($(shell uname), Linux)
	  RM = rm -f
	  CP = cp
      MAKE=make
      FixPath = $1
      LUATOOLS=linlua   
	else ifeq ($(OS), Windows_NT)
      MAKE=mingw32-make
   	  RM = rm -f
   	  CP = copy
   	  LUATOOLS=win32lua
   	  FixPath = $(subst /,\,$1)
	endif
endif

all:
	mkdir -p tools
	mkdir -p dll
	$(MAKE) tools
	$(MAKE) $(LUATOOLS)
	$(MAKE) -C src

clean:
	$(MAKE) -C bin2source clean
	$(MAKE) -C src clean
	$(MAKE) -C externals/lua-5.3.1/src clean
	$(RM) tools/*

tools:
	$(MAKE) -C bin2source

linlua:
	$(MAKE) -C externals/lua-5.3.1/src linux
	cp externals/lua-5.3.1/src/luac.exe tools/
	$(MAKE) -C externals/lua-5.3.1/src clean
	$(MAKE) -C externals/lua-5.3.1/src mingw

win32lua:
	$(MAKE) -C externals/lua-5.3.1/src win32
	cp externals/lua-5.3.1/src/luac.exe tools/	

.PHONY: all clean tools
