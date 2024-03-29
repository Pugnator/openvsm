RELEASE=`../relgen.sh`

MAKE=make
CC:= i686-w64-mingw32-cc
AR:= i686-w64-mingw32-ar
OBJCOPY:= objcopy
STRIP:= strip
WINRES:= windres

LIBDIR=../dll

MOD=modules
DEV=device
XXD=../tools/bin2source.exe
LUAC=../tools/luac.exe
OPENVSMLIB?=$(LIBDIR)/openvsm
LUADIR=lua

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
$(MOD)/module_fifo.lua \
$(MOD)/module_uart.lua \
$(MOD)/module_pin.lua

DEVICE=\
$(DEV)/device.lua

CFLAGS:=-O3 -m32 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3 -Wno-unused-variable -static-libgcc -static-libstdc++ -I../include -Imodules \
-I../externals/$(LUADIR)/src -D__VERSION=\"$(shell ../relgen.sh)\"

SLIB_CFLAGS:=-Wl,--export-all-symbols,--enable-auto-import

LDFLAGS:=-m32 ../externals/$(LUADIR)/src/liblua.a -lm -static-libgcc -static-libstdc++

OBJ=$(SRC:%.c=%.o) openvsm.res \
$(MOD)/module_bus.c.o $(MOD)/module_custom.c.o \
$(MOD)/module_events.c.o $(MOD)/module_pin.c.o \
$(MOD)/module_fifo.c.o $(MOD)/module_uart.c.o \
$(DEV)/device.c.o

LOBJ:=$(MODULES:%.lua=%.mod)
LOBJ+=$(DEVICE:%.lua=%.mod)

all: modules $(OPENVSMLIB).dll

%.mod: %.lua
	$(LUAC) -s -o $@ $<
	$(XXD) $@ > $@.c

openvsm.res:
	$(WINRES) openvsm.rc --output-format=coff --target=pe-i386 -o openvsm.res

$(DEV)/device.c.o:
	$(CC) -c -o $@ $(DEV)/device.mod.c $(CFLAGS)

$(MOD)/module_bus.c.o:
	$(CC) -c -o $@ $(MOD)/module_bus.mod.c $(CFLAGS)
	
$(MOD)/module_fifo.c.o:
	$(CC) -c -o $@ $(MOD)/module_fifo.mod.c $(CFLAGS)

$(MOD)/module_custom.c.o:
	$(CC) -c -o $@ $(MOD)/module_custom.mod.c $(CFLAGS)

$(MOD)/module_uart.c.o:
	$(CC) -c -o $@ $(MOD)/module_uart.mod.c $(CFLAGS)
	
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

SUBDIRS:= . modules device bindings ../externals/$(LUADIR)/src/

clean:	
	rm -f $(OPENVSMLIB).*
	for dir in $(SUBDIRS); do \
		rm -f $$dir/*.o; \
		rm -f $$dir/*.res; \
		rm -f $$dir/*.mod; \
		rm -f $$dir/*.mod.c; \
	done