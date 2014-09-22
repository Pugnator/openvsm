SYS=$(shell gcc -dumpmachine)
ifneq (, $(findstring mingw, $(SYS)))
	MAKE=mingw32-make
	CC:=mingw32-gcc
	AR:=ar
	OBJCOPY:=objcopy
	STRIP:=strip
	WINRES:=windres
else ifneq (, $(findstring linux, $(SYS)))
	MAKE=make
	CC:=i586-mingw32msvc-gcc
	AR:=i586-mingw32msvc-ar
	OBJCOPY:=i586-mingw32msvc-objcopy
	STRIP:=i586-mingw32msvc-strip
	WINRES:=i586-mingw32msvc-windres
else
	exit 0
endif

LIB?=vsm
SRC=vsm_api.c binding/c_bind.c binding/lua_bind.c win32.c

CFLAGS:=-O0 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3 -W -Wall -Iinclude -Icurses \
-Ilua52/include

SHLIB_CFLAGS:=-Wl,--export-all-symbols,--enable-auto-import

LDFLAGS:=lua52/liblua52.a

OBJ=$(SRC:%.c=%.o) my.res

%.o: %.c 	
	@$(WINRES) my.rc -O coff -o my.res
	@$(CC) -c -o $@ $< $(CFLAGS) 

$(LIB).dll: $(OBJ)	
#	$(AR) rcs $@ $^
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(SHLIB_CFLAGS) 
#	@$(OBJCOPY) --only-keep-debug $(LIB).dll $(LIB).dwarf
#	@$(STRIP) -s $(LIB).dll	
#	@$(OBJCOPY) --add-gnu-debuglink=$(LIB).dwarf $(LIB).dll
#	@upx -q9 $(LIB).dll	

clean:
	@find -maxdepth 1 -type f -regex ".*/.*\.\(o\|res\|dll\|lib\|def\|dwarf\|unc-backup~\|md5~\\)" -delete
