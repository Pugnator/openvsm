# Portable CHIP-8 core

This example starts with the emulator core rather than a Proteus-specific
component. `core.lua` implements the classic 4 KiB, 64-by-32 CHIP-8 machine and
depends only on Lua 5.4. ROMs are deliberately not included.

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

## Planned host adapters

- OpenVSM: map `device_graphics_plot` to the framebuffer, translate actuation
  into the 16 CHIP-8 keys, call `graphics.repaint` only when dirty, and schedule
  CPU/timer slices with simulation time.
- ImGui: render the same 64-by-32 snapshot as a scaled texture or draw list and
  map desktop keys to `set_key`.
- Headless: use `framebuffer.lua`, as the native CTest harness does today.
