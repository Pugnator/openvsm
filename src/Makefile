CURENV=$(shell gcc -dumpmachine)
RELEASE=`../relgen.sh`
ifneq (, $(findstring mingw, $(CURENV)))
	MAKE=mingw32-make
	CC:=mingw32-gcc
	AR:=mingw32-ar
	OBJCOPY:=objcopy
	STRIP:=mingw32-strip
	WINRES:=windres
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
DEV=device
XXD=xxd
OPENVSMLIB?=$(LIBDIR)/openvsm

SRC:=vsmapi.c \
lua_pin.c \
lua_bus.c \
lua_vsmobj.c \
utils.c \
wincompat.c \
bindings/cbind.c \
bindings/luabind.c


LUAC=luac
MODULES=\
$(MOD)/bus.lua \
$(MOD)/custom.lua \
$(MOD)/events.lua \
$(MOD)/pin.lua

DEVICE=\
$(DEV)/device.lua

CFLAGS:=-O3 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3 \
-Wno-unused-variable -I../include -Imodules \
-I../externals/lua53/include -D__VERSION=\"$(shell ../relgen.sh)\"

SLIB_CFLAGS:=-Wl,--export-all-symbols,--enable-auto-import

LDFLAGS:=../externals/lua53/liblua.a -lm

OBJ=$(SRC:%.c=%.o) openvsm.res \
$(MOD)/bus.mod.c.o $(MOD)/custom.mod.c.o \
$(MOD)/events.mod.c.o $(MOD)/pin.mod.c.o \
$(DEV)/device.mod.c.o

LOBJ:=$(MODULES:%.lua=%.mod)
LOBJ+=$(DEVICE:%.lua=%.mod)

all: modules $(OPENVSMLIB).dll

%.mod: %.lua
	$(LUAC) -s -o $@ $<
	$(XXD) -i $< > $@.c

openvsm.res:
	$(WINRES) openvsm.rc -O coff -o openvsm.res

$(DEV)/device.mod.c.o:
	$(CC) -c -o $@ $(DEV)/device.mod.c $(CFLAGS)

$(MOD)/bus.mod.c.o:
	$(CC) -c -o $@ $(MOD)/bus.mod.c $(CFLAGS)

$(MOD)/custom.mod.c.o:
	$(CC) -c -o $@ $(MOD)/custom.mod.c $(CFLAGS)

$(MOD)/pin.mod.c.o:
	$(CC) -c -o $@ $(MOD)/pin.mod.c $(CFLAGS)

$(MOD)/events.mod.c.o:
	$(CC) -c -o $@ $(MOD)/events.mod.c $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OPENVSMLIB).dll: $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(SLIB_CFLAGS)
	@$(OBJCOPY) --only-keep-debug $(OPENVSMLIB).dll $(OPENVSMLIB).dwarf
	@$(STRIP) -s $(OPENVSMLIB).dll
	@$(OBJCOPY) --add-gnu-debuglink=$(OPENVSMLIB).dwarf $(OPENVSMLIB).dll

modules: $(LOBJ)
	@echo "Lua modules compiled"

install:

clean:
	@-rm -f $(OPENVSMLIB).*
	@rm -f $(MOD)/*.mod $(MOD)/*.o $(MOD)/*.mod.c
	@rm -f $(DEV)/*.mod $(DEV)/*.o $(DEV)/*.mod.c
	@find -maxdepth 2 -type f -regex ".*/.*\.\(o\|res\|dll\|lib\|dwarf\\)" -delete
