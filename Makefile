CURENV=$(shell gcc -dumpmachine)
ifneq (, $(findstring mingw, $(CURENV)))
	MAKE=mingw32-make
	CC:=mingw32-gcc
	AR:=mingw32-ar
	OBJCOPY:=mingw32-objcopy
	STRIP:=mingw32-strip
	WINRES:=mingw32-windres
else ifneq (, $(findstring linux, $(CURENV)))
	MAKE=make
	CC:=i586-mingw32msvc-gcc
	AR:=i586-mingw32msvc-ar
	OBJCOPY:=i586-mingw32msvc-objcopy
	STRIP:=i586-mingw32msvc-strip
	WINRES:=i586-mingw32msvc-windres
else
	exit 0
endif

OPENVSMLIB?=openvsm

SRC=vsm_api.c c_bind.c lua_bind.c win32.c

CFLAGS:=-O2 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3 -W -Wall -Iinclude \
-Ilua53/include

SHOPENVSMLIB_CFLAGS:=-Wl,--export-all-symbols,--enable-auto-import

LDFLAGS:=lua53/liblua.a

OBJ=$(SRC:%.c=%.o) my.res

%.o: %.c
	@$(WINRES) my.rc -O coff -o my.res
	@$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: $(OPENVSMLIB).dll
$(OPENVSMLIB).dll: $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(SHOPENVSMLIB_CFLAGS)
	@$(OBJCOPY) --only-keep-debug $(OPENVSMLIB).dll $(OPENVSMLIB).dwarf
	@$(STRIP) -s $(OPENVSMLIB).dll
	@$(OBJCOPY) --add-gnu-debuglink=$(OPENVSMLIB).dwarf $(OPENVSMLIB).dll

.PHONY: install
install:

.PHONY: clean
clean:
	@find -maxdepth 1 -type f -regex ".*/.*\.\(o\|res\|dll\|lib\|dwarf\\)" -delete
