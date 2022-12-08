LUATOOLS=""

ISCC := iscc

ifdef SystemRoot
	MAKE := mingw32-make
	RM := rm -f
	CP := copy
	LUATOOLS := win32lua
	FixPath = $(subst /,\,$1)
else
	ifeq ($(shell uname), Linux)
		RM = rm -f
		CP = cp
    	MAKE=make
    	FixPath = $1
    	LUATOOLS=linlua   
	else ifeq ($(OS), Windows_NT)
    	MAKE := make
   		RM := rm -f
   		CP := copy		
   		LUATOOLS := win32lua
   		FixPath := $(subst /,\,$1)
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
	$(MAKE) -C externals/lua/src clean
	$(RM) tools/*

tools:
	$(MAKE) -C bin2source

linlua:
	$(MAKE) -C externals/lua/src linux
	$(CP) externals/lua/src/luac.exe tools/
	$(MAKE) -C externals/lua/src clean
	$(MAKE) -C externals/lua/src mingw

win32lua:
	$(MAKE) -C externals/lua/src mingw
	cp externals/lua/src/luac.exe tools

innosetup:
	$(ISCC) externals/installer/openvsm.iss

.PHONY: all clean tools innosetup win32lua linlua
