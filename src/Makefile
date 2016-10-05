RELEASE=`../relgen.sh`
ifdef SystemRoot
	MAKE=mingw32-make
	CC:=mingw32-gcc
	AR:=mingw32-ar
	OBJCOPY:=objcopy
	STRIP:=mingw32-strip
	WINRES:=windres
else
	MAKE=make
	CC:=i586-mingw32msvc-gcc
	AR:=i586-mingw32msvc-ar
	OBJCOPY:=i586-mingw32msvc-objcopy
	STRIP:=i586-mingw32msvc-strip
	WINRES:=i586-mingw32msvc-windres
endif

LIBDIR=../dll

MOD=modules
DEV=device
XXD=../tools/bin2source.exe
LUAC=../tools/luac.exe
OPENVSMLIB?=$(LIBDIR)/openvsm

SRC:=vsmapi.c \
lua_pin.c \
lua_bus.c \
lua_vsmobj.c \
utils.c \
wincompat.c \
bindings/cbind.c \
bindings/luabind.c

MODULES=\
$(MOD)/module_bus.lua \
$(MOD)/module_custom.lua \
$(MOD)/module_events.lua \
$(MOD)/module_pin.lua

DEVICE=\
$(DEV)/device.lua

CFLAGS:=-O3 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3 \
-Wno-unused-variable -I../include -Imodules \
-I../externals/lua-5.3.1/src -D__VERSION=\"$(shell ../relgen.sh)\"

SLIB_CFLAGS:=-Wl,--export-all-symbols,--enable-auto-import

LDFLAGS:=../externals/lua-5.3.1/src/liblua.a -lm

OBJ=$(SRC:%.c=%.o) openvsm.res \
$(MOD)/module_bus.c.o $(MOD)/module_custom.c.o \
$(MOD)/module_events.c.o $(MOD)/module_pin.c.o \
$(DEV)/device.c.o

LOBJ:=$(MODULES:%.lua=%.mod)
LOBJ+=$(DEVICE:%.lua=%.mod)

all: modules $(OPENVSMLIB).dll

%.mod: %.lua
	$(LUAC) -s -o $@ $<
	$(XXD) $@ > $@.c

openvsm.res:
	$(WINRES) openvsm.rc -O coff -o openvsm.res

$(DEV)/device.c.o:
	$(CC) -c -o $@ $(DEV)/device.mod.c $(CFLAGS)

$(MOD)/module_bus.c.o:
	$(CC) -c -o $@ $(MOD)/module_bus.mod.c $(CFLAGS)

$(MOD)/module_custom.c.o:
	$(CC) -c -o $@ $(MOD)/module_custom.mod.c $(CFLAGS)

$(MOD)/module_pin.c.o:
	$(CC) -c -o $@ $(MOD)/module_pin.mod.c $(CFLAGS)

$(MOD)/module_events.c.o:
	$(CC) -c -o $@ $(MOD)/module_events.mod.c $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OPENVSMLIB).dll: $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(SLIB_CFLAGS)
	$(OBJCOPY) --only-keep-debug $(OPENVSMLIB).dll $(OPENVSMLIB).dwarf
	$(STRIP) -s $(OPENVSMLIB).dll
	$(OBJCOPY) --add-gnu-debuglink=$(OPENVSMLIB).dwarf $(OPENVSMLIB).dll

modules: $(LOBJ)
	@echo "Lua modules compiled"

SUBDIRS:=modules device

clean:	
	rm -f $(OPENVSMLIB).*	
	for dir in $(SUBDIRS); do \
		rm -f $$dir/*.o; \
		rm -f $$dir/*.mod; \
		rm -f $$dir/*.mod.c; \
	done