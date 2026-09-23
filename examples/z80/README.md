# Z80

A Z80 single-board computer on a Proteus schematic, running NASCOM BASIC
through a serial terminal. The CPU is the
[Z80core](https://github.com/Pugnator/Z80core) library and the serial port is
a 6850 ACIA, both modelled in Lua through OpenVSM. The EPROM, RAM and glue
logic are ordinary Proteus parts.

```
Z80 SBC By Grant Searle

Memory top?
Z80 BASIC Ver 4.7b
Copyright (C) 1978 by Microsoft
32382 Bytes free
Ok
```

The CPU model holds no memory and no devices. It is clocked from the
schematic and asks for every byte through its pins, so the memory map lives in
the address decoding, where it belongs. That makes the board a real test of
the core: a machine cycle of the wrong length, a bus released at the wrong
moment or an address the decoder does not expect, and real memory notices.

| File | What it is |
| --- | --- |
| `z80.pdsprj` | The board |
| `z80_device.lua` | The Z80: pins, clock edge and bus, around Z80core |
| `acia.lua` | A 6850-compatible ACIA |
| `device.txt`, `acia.txt` | Library definitions for the two parts |
| `uarttest.asm` / `.bin` | Serial smoke test: prints `UART TEST`, echoes what you type |
| `mathtest.asm` / `.bin` | Arithmetic checked against known answers, reported on I/O ports |

`z80core.dll` and the BASIC ROM are not in the repository. The first is built
from Z80core, the second comes from Grant Searle; both are covered below.

## Quick start

1. Build `z80core.dll` as described below and put it beside `z80_device.lua`.
2. Put `ROM_32K.HEX` from Grant Searle's NASCOM BASIC for his Simple Z80 in
   `sbc_NascomBasic32k/`.
3. Open `z80.pdsprj` and run. The terminal window should show the banner above.
   Press Enter at `Memory top?` to take all the RAM.

Type in capitals or lower case, but with an English keyboard layout: the
terminal sends raw bytes, and on another layout the letter keys send
characters BASIC does not know.

## The board

| Range | Part | Selected by |
| --- | --- | --- |
| `0000`-`7FFF` | 27C256 EPROM | `A15` low |
| `8000`-`FFFF` | 62256 static RAM | `A15` high, through an inverter |
| ports `80`-`FF` | ACIA: `80` control and status, `81` data | `A7` high, `IORQ` low, `M1` high |

The ROM image is 8 K. The EPROM's `A13` and `A14` are tied to ground, so the
image repeats four times through the lower half of the map. BASIC keeps its
workspace and stack in the RAM from `8000` up.

| Signal | Driven from |
| --- | --- |
| EPROM `/CE` | `A15` |
| RAM `/CE` | `A15` through one gate of a 74HCT04 |
| EPROM and RAM `/OE` | `MREQ` OR `RD`, one gate of a 74LS32 |
| RAM `/WE` | `MREQ` OR `WR`, the other gate |
| `RESET` | 10 k pull-up and a push button to ground |
| `WAIT`, `INT`, `NMI`, `BUSRQ` | pulled up; the ACIA also drives `INT` |

**Only ports `00`-`7F` are free.** The ACIA decodes nothing below `A7`, so every
port from `80` to `FF` reaches it, with `A0` choosing the register. A program
that reports progress on, say, port `F2` is writing its progress into the
ACIA's control register. The test programs here report on `70`-`77` for that
reason.

### Clocks and baud rate

| Generator | Frequency | Drives |
| --- | --- | --- |
| CPU | 100 kHz | Z80 `CLK` |
| Serial | 19200 Hz | ACIA `RXCLK` and `TXCLK` |

The baud rate is the serial clock divided by the divider the program writes to
the ACIA control register, and the terminal has to match it:

| Program | Control word | Divider | Terminal |
| --- | --- | --- | --- |
| NASCOM BASIC | `96` | 64 | 300 baud, 8N1 |
| `uarttest.bin` | `15` | 16 | 1200 baud, 8N1 |

To run BASIC at 1200, raise the serial clock to 76800 Hz instead.

The CPU clock can go higher. Every edge is a Proteus event and a Lua call, and
that boundary is what limits speed, not the core. Keep the clock period long
next to the memories' access time: the model hands the core whatever the data
bus carries at the edge that needs it.

## The Z80 component

Import `device.txt` in the library editor, or set these properties on a
component whose pins match the names below.

| Property | Value | Purpose |
| --- | --- | --- |
| `PRIMITIVE` | `DIGITAL` | Proteus digital model |
| `MODDLL` | `openvsm.DLL` | The OpenVSM model |
| `LUA` | `z80_device.lua` | This script, relative to the project |
| `TRACE` | `0` or `1` | Log bus cycles; see below |

Pins: `A0`-`A15`, `D0`-`D7`, `M1`, `MREQ`, `IORQ`, `RD`, `WR`, `RFSH`, `HALT`,
`WAIT`, `INT`, `NMI`, `RESET`, `BUSRQ`, `BUSAK`, `CLK`. Each active-low pin is
declared as `NAME,$NAME$`, so the symbol may draw it plain or overbarred.
`VCC` and `GND` may be on the symbol and are not declared.

`TRACE=1` logs every bus cycle until the first 20000, then keeps logging I/O
cycles only. That is enough to watch a program come up, and it makes
`log.txt` large, so leave it at `0` otherwise.

## The ACIA component

`acia.lua` follows the 6850 datasheet for everything the program can see:
control and status at register select 0, data at 1, the divider and
word-format fields, the status bits and the receive and transmit interrupts.
It differs in how it meets the bus. It takes the Z80's `RD` and `WR` directly
instead of a 6800 enable clock, and its bit timing comes only from its clock
pin and the divider.

It reuses the pin names of the stock MC6850 symbol, so the simplest way to
make the part is to place an MC6850, Decompose it, and Make Device with the
properties from `acia.txt`. Remaking it is necessary because the stock part's
`MODDLL` is read-only.

| Pin | Net | Note |
| --- | --- | --- |
| `D[0..7]` | data bus | the bus must carry the name `D[0..7]`; see below |
| `RS` | `A0` | register select |
| `CS0`, `CS1`, `/CS2` | `A7`, `M1`, `IORQ` | selects I/O cycles to `80`-`FF`, never an interrupt acknowledge |
| `E` | `RD` | **the read strobe**, not an enable clock |
| `R/W` | `WR` | the write strobe |
| `RXCLK`, `TXCLK` | serial generator | only `RXCLK` is read; tie them together |
| `RXDATA` | terminal `TXD` | |
| `TXDATA` | terminal `RXD` | |
| `/IRQ` | `INT` | pulled low while an enabled interrupt is pending |

`/CTS`, `/DCD` and `/RTS` are not used and may be left unconnected.

Set `TRACE = true` at the top of `acia.lua` to log every control write, every
received byte and every transmitted frame, with the clock ticks it took. A
healthy frame at divide by 16 logs 160 ticks and 10 bits.

## When it does not work

**The terminal shows garbage.** Check the baud rate before anything else: the
serial clock divided by the program's divider must equal the terminal
setting. Then check that nothing writes to a port from `80` to `FF` except the
ACIA driver itself.

**`Pin 'X' not found` when the simulation starts.** Proteus gives a model only
the pins that are connected to a net. The pin exists on the symbol but is
unwired.

**Reads come back as `00` with nothing driving the bus.** The Z80 model says so
in `log.txt`, and names the address:

```
z80: read 0080 = 00 | contended D:00 floating D:FF | this model is not driving
```

`floating D:FF` means no device answered. A part connected to the data bus
through a bus pin, like the ACIA's `D[0..7]`, only joins the nets `D0`-`D7` if
the bus it connects to carries that name. On an unnamed bus its data lines
are private nets that nothing else is on. Label the bus `D[0..7]`.

`contended` bits mean two devices answered at once, which is a decoding fault.

## How it works

`device_init` resolves the pins, creates the CPU and registers a change handler
on `CLK` and on the five control inputs. The data bus has no handler: it is
read at the clock edge that needs it rather than chased as it settles.

On each clock edge the model hands the core the data bus if a read is in
flight, advances the core with `cpu:tick(level)`, and publishes only the pins
the core reports as changed. The data bus is driven on a write, released on a
read, and floats otherwise, including during refresh, which asserts `MREQ`
with neither strobe.

**The address never moves on the same edge that a strobe is released.** At the
end of an opcode fetch the core releases `MREQ` and `RD` and puts the refresh
address out in the same step. If both reach the schematic together, the EPROM
is still selected and still enabled when its address changes, and it goes on
driving the byte at the refresh address into the next cycle, where it collides
with the RAM. Ordering the two publishes, or delaying the address by a few
hundred nanoseconds, does not separate them far enough. So when a strobe is
released, the model holds the new address back and publishes it on the next
edge, half a clock period later.

**An interrupt acknowledge is treated as a read.** It is the one cycle where
`M1` and `IORQ` are asserted together and neither `RD` nor `WR` is, so a model
that only releases the bus on `RD` would drive it while a device is supplying
its vector. BASIC uses mode 1, where the byte is discarded; the same handling
is what mode 2 needs, though nothing on this board supplies a vector.

The clock level crosses to the core as a boolean. The binding tests its
argument with `lua_toboolean`, and the number `0` is true in Lua, so
`cpu:tick(0)` would raise the clock instead of dropping it.

## Build `z80core.dll`

The binding ships with Z80core as `lua_z80core.c`. It has to be **32-bit**, to
match Proteus, and it has to resolve its `lua_*` symbols against the same Lua
that OpenVSM uses.

`openvsm.dll` does not export those symbols, because
`CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS` exports a target's own objects and not the
static libraries it links. So the module links OpenVSM's `lua_static.lib`
instead. Both then run identical copies of the same Lua build over one
`lua_State`, which is safe because the Lua library keeps no mutable state
outside that state. Rebuild the module whenever OpenVSM's Lua is rebuilt.

Build OpenVSM once so `lua_static.lib` exists, then from the **x86** Native
Tools Command Prompt for VS 2022:

```bat
set Z80=C:\path\to\Z80core\src\emucore\z80core
set OVSM=C:\path\to\openvsm

cl /nologo /LD /O2 /MT ^
   /I "%Z80%\include" ^
   /I "%OVSM%\externals\Lua\lua-5.4.6\include" ^
   /DZ80CORE_SHARED /DZ80CORE_BUILD ^
   "%Z80%\z80core.c" "%Z80%\lua_z80core.c" ^
   /Fez80core.dll ^
   /link "%OVSM%\build\vs2022-win32\lib\Release\lua_static.lib"
```

`/MT` matches the static runtime of OpenVSM's release build. Put the result
**beside `z80_device.lua`**; the script adds its own directory to
`package.cpath` before requiring it.

### Build the module, not the shared core

Z80core has two targets that both produce a file called `z80core.dll`:

| Target | Sources | Exports |
| --- | --- | --- |
| `z80core_shared` | `z80core.c` | `z80_*` only, **not loadable by Lua** |
| `z80core_lua` | `z80core.c` + `lua_z80core.c` | `luaopen_z80core` as well |

Loading the first gives

```
error loading module 'z80core' from file '...z80core.dll':
	The specified procedure could not be found.
```

which is `require` finding the DLL and then not finding `luaopen_z80core` in
it. Check what you built before blaming the path:

```bat
dumpbin /exports z80core.dll | findstr luaopen
```

The command above is also simpler than configuring Z80core's CMake project,
which needs bison and flex for its assembler.

### Match OpenVSM's compiler

The module runs Lua code against a `lua_State` that `openvsm.dll` created, and
Lua raises errors with `longjmp`. A jump buffer filled by one compiler's
`setjmp` and taken by another's is undefined, so the rule is to **build the
module with the same compiler family as the `openvsm.dll` it loads into**. The
OpenVSM release builds are MSVC.

Two mismatches name themselves: a 64-bit build gives `%1 is not a valid Win32
application`, and a MinGW module against an MSVC OpenVSM shows a dependency on
`msvcrt.dll` in `dumpbin /imports z80core.dll`. A matched build imports
`KERNEL32.dll` and nothing else.

## Test programs

Both sources are written for Z80core's `zasm`, and the `.bin` files beside them
are prebuilt:

```sh
zasm -s uarttest.asm -o uarttest.bin
```

Load a `.bin` into the EPROM's *Image File* property in place of the BASIC ROM.

`uarttest` master-resets the ACIA, prints `UART TEST` in a loop and echoes
anything typed. Run the terminal at 1200 baud. It reports its progress on
ports `70`-`74`, so with `TRACE=1` the log shows how far it got even if nothing
reaches the terminal.

`mathtest` runs three calculations with their working values in RAM, checks
each against a known answer, and reports on ports `70`-`77`. It needs no
terminal. A passing run writes `AA` then `55` to port `70`, and `01`, `02`,
`03` to port `77`, and halts.

## Limits

- The model reads the data bus at clock edges only, so a device that drives
  the bus for less than half a clock period is not seen.
- Every message the models log ends in `\n`. OpenVSM's `print` adds no
  terminator, so a message without one runs into the next.
