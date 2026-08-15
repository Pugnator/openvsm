# Lua modules

The source modules in this directory can be copied beneath `%LUAVSM%\modules`.
Load the UART module from a model script and construct it after pin objects
exist:

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
