# Portable CHIP-8 core

This example keeps the emulator core independent from its Proteus-specific
adapter. `core.lua` implements the classic 4 KiB, 64-by-32 CHIP-8 machine and
depends only on Lua 5.4. `device.lua` supplies a small original demonstration
program when no external ROM is configured.

The core accepts a display object with two operations:

```lua
display:clear()
local collision = display:xor_pixel(x, y)
```

`framebuffer.lua` is the reference headless implementation. It owns the pixels,
tracks dirty state and revisions, and exposes snapshots for renderers. A future
OpenVSM adapter can draw this framebuffer through the existing `graphics` API;
an ImGui adapter can consume the same pixels without changing the VM.

## Core API

```lua
local Chip8 = dofile("core.lua")
local Framebuffer = dofile("framebuffer.lua")

local display = Framebuffer.new(Chip8.DISPLAY_WIDTH, Chip8.DISPLAY_HEIGHT)
local vm = Chip8.new({
    display = display,
    profile = "modern",
    random_byte = function()
        return math.random(0, 255)
    end
})

vm:load_rom(rom_bytes)
vm:run(instructions_per_slice)
vm:tick_timers(1) -- call at 60 Hz
vm:set_key(0xa, true)
```

The default `modern` profile shifts `VX` and leaves `I` unchanged after
`FX55`/`FX65`. The `original` profile shifts `VY` and increments `I`. Individual
quirks can be overridden through `quirks`: `clip_sprites`, `jump_uses_vx`,
`load_store_increment_i`, and `shift_uses_vy`.

Execution is deterministic when `random_byte` is injected. Timers are separated
from instruction execution so a Proteus callback, an ImGui event loop, or a
headless test can drive the same core at an exact 60 Hz.

## Proteus device

`device.txt` follows the same Proteus library-description structure as the NAND
example and loads `chip8/device.lua` through `openvsm.DLL`. It is a
self-contained virtual console with no electrical pins. Its DIL8 package is a
temporary metadata placeholder inherited from the NAND example; the component
is not intended for PCB placement.

The device is a one-state active component with the shared symbol name stem
`OPENVSM` and DLL linking enabled. Proteus uses that stem both for the active
sprite (`OPENVSM_0`) and for the active-model DLL (`openvsm.dll`). When
recreating it with Make Device, create or reuse the `OPENVSM_0` sprite and set
those values in Active Component Properties. A device-specific stem such as
`LUA_CHIP8` would make Proteus look for `LUA_CHIP8.dll`, bypassing the active
entry point in the installed `openvsm.dll` even though its digital model still
starts normally.

Leave the component's `ROM` property empty to run the built-in hexadecimal font
demo. Set `ROM` to a binary CHIP-8 image to run another program. Relative paths
are resolved by Proteus from the project directory, while absolute paths work
unchanged. Because the property belongs to the component instance, multiple
CHIP-8 devices can run different ROMs through the same `openvsm.dll`.

The active schematic face is a compact horizontal console:

```text
+--------------------------------------------------+
| CHIP-8                                       DEMO |
| +----------------------------+   [1] [2] [3] [C] |
| |                            |   [4] [5] [6] [D] |
| |       framebuffer          |   [7] [8] [9] [E] |
| |                            |   [A] [0] [B] [F] |
| +----------------------------+                   |
| RUN  PC:0224  60 Hz  700 ips                     |
+--------------------------------------------------+
```

The 192-by-96 display area preserves the CHIP-8 2:1 aspect ratio and gives every
64-by-32 framebuffer pixel an exact 3-by-3 drawing cell. Pixels are lime on
black. The adapter executes 700 instructions per second, updates CHIP-8 timers
at exactly 60 Hz, and repaints only after framebuffer or keypad changes. Mouse
clicks use the drawn hexadecimal keypad. Keyboard input uses the conventional
`1234`, `QWER`, `ASDF`, `ZXCV` layout.

## Host adapters

- OpenVSM: `device.lua` maps the framebuffer and keypad to the graphics API and
  drives CPU/timer slices from Proteus absolute-time callbacks.
- ImGui: render the same 64-by-32 snapshot as a scaled texture or draw list and
  map desktop keys to `set_key`.
- Headless: use `framebuffer.lua`, as the native CTest harness does today.
