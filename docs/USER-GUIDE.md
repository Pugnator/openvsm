# Using OpenVSM

OpenVSM is a shared model DLL for Proteus VSM. A schematic component selects a
Lua script, and that script declares the component's pins or buses and handles
simulation events. Active components may also draw their own schematic face
and receive mouse input.

OpenVSM is currently a 32-bit Windows model using Lua 5.4. Each component
instance owns a separate Lua state, so devices can share `openvsm.dll` while
using different scripts, parameters, and runtime state.

## Install OpenVSM

Download and run the latest `.exe` installer from the
[GitHub releases](https://github.com/Pugnator/openvsm/releases) page. The
installer locates the Proteus Models directory and installs `openvsm.dll`
there. It also installs the example scripts into an OpenVSM `LuaScripts`
directory and creates the machine-wide `LUAVSM` environment variable when one
does not already exist.

Restart Proteus after installing or changing `LUAVSM` so it inherits the
updated environment.

## Create a digital device

The exact library-editor screens differ between Proteus versions, but the
model needs these component properties:

| Property | Value | Purpose |
| --- | --- | --- |
| `PRIMITIVE` | `DIGITAL` | Selects the Proteus digital simulation interface |
| `MODDLL` | `openvsm.DLL` | Loads the shared OpenVSM model |
| `LUA` | For example, `device.lua` | Selects the script for this component instance |

Create terminals whose names match the Lua pin declarations. A minimal NAND
model saved as `device.lua` is:

```lua
device_pins = {
    {name = "A", on_time = 100000, off_time = 100000},
    {name = "B", on_time = 100000, off_time = 100000},
    {name = "Q", on_time = 100000, off_time = 100000}
}

function device_simulate()
    Q:set(1 - (A:get() * B:get()))
end
```

Place `device.lua` beside the Proteus project, add the configured part to the
schematic, and start the simulation. The complete small example is in
[`examples/NAND`](../examples/NAND).

## How scripts are found

The `LUA` property belongs to the component instance and supports either form:

- An absolute path is used directly.
- A relative path is searched beside the Proteus project first, then below
  the directory in `LUAVSM`.

For example, `LUA=chip8/device.lua` can load a project-local copy or fall back
to `%LUAVSM%\chip8\device.lua`. Project-first lookup makes it possible to edit
or override a model without replacing the installed example. Paths may contain
spaces, and `LUAVSM` does not need a trailing slash.

If a script cannot be loaded, `log.txt` records the configured value and every
path that OpenVSM searched.

## Model lifecycle

A script may define these functions:

| Function | When it runs |
| --- | --- |
| `device_init()` | Once, after the script's pins and buses have been registered |
| `device_simulate()` | Whenever Proteus activates the digital model for simulation |
| `timer_callback(time, event_id)` | When a callback scheduled with `set_callback` is due |

Only define the callbacks the model needs. Callback errors are logged and the
Lua stack is restored. A failing `device_simulate` or pin/bus object callback
is disabled to prevent the same error from recurring on every host event.

Simulation times are integer picoseconds. The constants `NSEC`, `USEC`, `MSEC`,
and `SEC` make scheduling readable:

```lua
local TICK = 1

function device_init()
    set_callback(systime() + MSEC, TICK)
end

function timer_callback(time, event_id)
    if event_id == TICK then
        -- Update the device here.
        set_callback(time + MSEC, TICK)
    end
end
```

## Pins

Every model must declare `device_pins`, even when it is an empty table. Each
entry creates a global object with the same name. `name` identifies the Proteus
terminal; `on_time` and `off_time` are optional transition delays in
picoseconds and default to zero.

Common operations are:

```lua
local level = A:get() -- 0 for low, 1 for high
Q:set(1)              -- drive high
Q:lo()                -- drive low
Q:invert()            -- invert the current drive
Q:setstate(FLT)       -- drive a Proteus logic state directly
```

The API also provides state queries such as `ishigh()`, `islow()`, `isedge()`,
`isposedge()`, `isnegedge()`, `isfloating()`, and `iscontention()`.

Register object callbacks from `device_init` when a model should respond only
to changes:

```lua
function device_init()
    A:onchange(function(time, mode, state)
        info("A changed to " .. state_to_string(state))
    end)

    B:onstate(SHI, function(time, mode, state)
        info("B became strongly high")
    end)
end
```

## Buses

Declare a native Proteus bus in `device_buses`. Widths from 1 through 32 bits
are supported, and bit indices are zero-based.

```lua
device_buses = {
    {
        name = "D",
        base = 0,
        width = 8,
        on_time = 100000,
        off_time = 120000,
        tristate_time = 50000
    }
}

function device_init()
    D:onvalue(0x55, function(time, mode, value)
        info("D received 0x55")
    end)
end

function device_simulate()
    D:drive(0x5a)
end
```

Bus objects provide `set`/`drive`, `tristate`, `drivebit`, `get`, `getdrive`,
`getbitstate`, `settiming`, and `setstates`. Use `onchange(function)` for every
value change or `onvalue(value, function)` for one value. Optional declaration
fields include `on_state`, `off_state`, `tristate_state`, and `required`.

## Component parameters and logging

Use these functions to read values from the component instance:

- `get_string_param(name)`
- `get_num_param(name)`
- `get_bool_param(name)`
- `get_init_param(name)`
- `get_hex_param(name)`

This is how multiple instances of one device can select different ROMs or
settings while sharing the same DLL and Lua source.

`info(text)`, `message(text)`, `warning(text)`, and `vsm_error(text)` forward
messages to the corresponding Proteus instance service. OpenVSM's runtime
diagnostics are appended to `log.txt` in the Proteus process working directory
when that location is writable. The log identifies the component, selected
script, pin count, initialization result, and callback errors.

## Active graphics

A device that draws or accepts input on the schematic needs the normal digital
properties plus active-component metadata:

- use the active name stem `OPENVSM`;
- configure one state and enable DLL linking;
- create or reuse the active sprite named `OPENVSM_0`;
- keep the device-specific `LUA` property on the component.

The equivalent active declaration in a text device description is:

```text
{ACTIVE=OPENVSM,1,DLL}
```

Proteus derives the active DLL name from the active stem. A stem such as the
part name `LUA_CHIP8` would make it search for `LUA_CHIP8.dll` instead of the
shared `openvsm.dll`.

The Lua script can implement `device_graphics_init`, `device_graphics_plot`,
`device_graphics_animate`, and `device_graphics_actuate`. Call
`graphics.repaint()` after initializing or changing visible state. See the
[graphics API](GRAPHICS-LUA-API.md), the minimal
[active display](../examples/graphics), and the complete
[CHIP-8 console](../examples/chip8).

## Troubleshooting

### The model does not start

Open `log.txt` and look for `Loading model script`. A missing script error lists
every searched path. Check the component's `LUA` property, the Proteus project
location, and `LUAVSM`.

### `log.txt` is empty or missing

OpenVSM writes the file in the Proteus process working directory. If that
directory is not writable, file logging is disabled. Confirm that the installed
DLL is the expected build and that the process can create `log.txt` there.

### Pins are not registered

Pin declaration names must match the component terminal names exactly. The log
reports the number of pins registered during setup.

### An active device starts but remains blank

Confirm that Proteus created an active model, not only a digital model. Check
the `OPENVSM` active stem, the `OPENVSM_0` sprite, DLL linking, and the graphics
callbacks. The initialization callback should request a repaint after setting
its drawing state.

### Lua changes are not visible

Stop and restart the Proteus simulation so the component creates a new Lua
state and reloads its script.
