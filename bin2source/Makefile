CURENV=$(shell gcc -dumpmachine)
ifneq (, $(findstring mingw, $(CURENV)))
	MAKE=make
	CC:=gcc
	AR:=ar
	OBJCOPY:=objcopy
	STRIP:=strip
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

CFLAGS:=-O0 -ggdb -g3 -std=gnu99 

OBJ=$(SRC:%.c=%.o)

all: $(BIN2SRC)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN2SRC): $(OBJ)
	$(CC) -o $@ $^

clean:
	@-rm -f $(BIN2SRC) *.o	
