CURENV=$(shell gcc -dumpmachine)
RELEASE=`../relgen.sh`
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

LIBDIR=../dll

MOD=modules
XXD=xxd
OPENVSMLIB?=$(LIBDIR)/openvsm

SRC=vsmapi.c pin.c bus.c vsmobj.c utils.c win32.c \
bindings/cbind.c bindings/luabind.c

LUAC=luac
MODULES=\
$(MOD)/bus.lua \
$(MOD)/custom.lua

CFLAGS:=-O0 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3 \
-Wno-unused-variable -I../include -Imodules \
-I../lua53/include -D__VERSION=\"$(shell ../relgen.sh)\"

SLIB_CFLAGS:=-Wl,--export-all-symbols,--enable-auto-import

LDFLAGS:=../lua53/liblua.a -lm

OBJ=$(SRC:%.c=%.o) openvsm.res \
$(MOD)/bus.mod.c.o $(MOD)/custom.mod.c.o

LOBJ=$(MODULES:%.lua=%.mod)

all: modules $(OPENVSMLIB).dll

%.mod: %.lua
	$(LUAC) -s -o $@ $<
	$(XXD) -i $< > $@.c

openvsm.res:
	$(WINRES) openvsm.rc -O coff -o openvsm.res

$(MOD)/bus.mod.c.o:
	$(CC) -c -o $@ $(MOD)/bus.mod.c $(CFLAGS)

$(MOD)/custom.mod.c.o:
	$(CC) -c -o $@ $(MOD)/custom.mod.c $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OPENVSMLIB).dll: $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(SLIB_CFLAGS)
	@#$(OBJCOPY) --only-keep-debug $(OPENVSMLIB).dll $(OPENVSMLIB).dwarf
	@#$(STRIP) -s $(OPENVSMLIB).dll
	@#$(OBJCOPY) --add-gnu-debuglink=$(OPENVSMLIB).dwarf $(OPENVSMLIB).dll

modules: $(LOBJ)
	@echo "Lua modules compiled"

install:

clean:
	@rm -f $(OPENVSMLIB).*
	@rm -f modules/*.mod modules/*.c modules/*.o
	@find -maxdepth 2 -type f -regex ".*/.*\.\(o\|res\|dll\|lib\|dwarf\\)" -delete
