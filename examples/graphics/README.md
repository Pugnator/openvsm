# Active display example

`display.lua` is a minimal active-model script. Configure an active Proteus
component to load `openvsm.dll`, set its `LUA` property to `graphics/display.lua`,
and set `LUAVSM` to the repository's `examples` directory. The model draws a
two-digit display and increments it when the active area receives a left click.

The example intentionally declares no digital pins. A real screen model can add
ordinary `device_pins` or `device_buses` and update its drawing state from the
same Lua state. Proteus project/library metadata is version-specific and is not
included; use the component editor for the installed Proteus version.
