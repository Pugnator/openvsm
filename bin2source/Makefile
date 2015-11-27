CURENV=$(shell gcc -dumpmachine)
ifneq (, $(findstring mingw, $(CURENV)))
	MAKE=mingw32-make
	CC:=mingw32-gcc
	AR:=mingw32-ar
	OBJCOPY:=objcopy
	STRIP:=mingw32-strip
	WINRES:=windres
else ifneq (, $(findstring linux, $(CURENV)))
	MAKE=make
	CC:=gcc
	AR:=ar
	OBJCOPY:=objcopy
	STRIP:=strip
else
	exit 0
endif

BIN2SRC:=../tools/bin2source.exe

SRC:=main.cpp

CFLAGS:=-O3 -gdwarf-2 -fgnu89-inline -std=gnu99 -g3

OBJ=$(SRC:%.c=%.o)

all: $(BIN2SRC)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN2SRC): $(OBJ)
	$(CC) -o $@ $^

clean:
	@-rm -f $(BIN2SRC)
	@find -maxdepth 2 -type f -regex ".*/.*\.\(o\|res\|dll\|lib\|dwarf\\)" -delete
