# OpenVSM

OpenVSM lets [Proteus VSM](https://www.labcenter.com/) components implement
their simulation behavior and optional schematic graphics in Lua. One 32-bit
`openvsm.dll` can serve many device types and instances; every component loads
its configured script into an isolated Lua state.

[Download a release](https://github.com/Pugnator/openvsm/releases) ·
[Read the user guide](docs/USER-GUIDE.md) ·
[Browse the documentation](docs/README.md) ·
[Report an issue](https://github.com/Pugnator/openvsm/issues)

## A minimal model

This script implements a two-input NAND gate. The names in `device_pins` match
the terminal names on the Proteus component.

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

Install OpenVSM, configure the component with `MODDLL=openvsm.DLL` and
`LUA=device.lua`, then place the script beside the Proteus project. The
[user guide](docs/USER-GUIDE.md) covers device properties, script lookup,
pins, buses, callbacks, graphics, and troubleshooting.

## Examples

| Example | What it demonstrates |
| --- | --- |
| [NAND](examples/NAND) | The smallest useful digital model |
| [Active display](examples/graphics) | Lua drawing and mouse input on the schematic |
| [CHIP-8](examples/chip8) | A complete VM with a display, keypad, timers, and per-instance ROM selection |
| [Optional modules](model/lua/modules) | UART and third-party control helpers |

## Documentation

- [Using OpenVSM](docs/USER-GUIDE.md)
- [Building from source](docs/BUILDING.md)
- [Lua graphics API](docs/GRAPHICS-LUA-API.md)
- [Lua v0.7 compatibility](docs/V0.7-LUA-COMPATIBILITY.md)
- [Developer guide](docs/DEVELOPER-GUIDE.md)

OpenVSM v0.7 uses Lua 5.4 and builds as a 32-bit Windows DLL with C++20 and
MSVC. It is licensed under [GPL-2.0-or-later](LICENSE).
