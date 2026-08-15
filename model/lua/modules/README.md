# Optional Lua modules

These ordinary Lua 5.4 source modules can be copied beneath
`%LUAVSM%\modules`. A model can load them directly or add the model root to
Lua's module search path:

```lua
local root = assert(os.getenv("LUAVSM"), "LUAVSM is not set")
package.path = root .. "/?.lua;" .. package.path
local Control = require("modules.vdm_control")
```

- `uart.lua` sends 8-N-1 frames through a Lua pin object.
- `vdm_control.lua` implements a generic, bounded third-party control profile
  over the Proteus Virtual Debug Monitor transport.

## UART example

Construct the UART module after pin objects exist:

```lua
local script_root = assert(os.getenv("LUAVSM"))
local Uart = dofile(script_root .. "/modules/uart.lua")
local uart

function device_init()
    uart = Uart.new(TX, {baud = 9600, event_id = 42})
    uart:send_string("Hello")
    uart:send(systime())
end

function timer_callback(time, event_id)
    if event_id == uart.event_id then
        uart:send(time)
    end
end
```

`Uart.new` drives the transmit pin high immediately, before any start bit. The
module sends 8-N-1 frames, least-significant bit first, and uses the v0.7
`set_callback` API for bit timing. `bit_time` may be supplied directly for
nonstandard clocks; otherwise it is rounded from `SEC / baud`.
