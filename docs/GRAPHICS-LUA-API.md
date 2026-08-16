# Lua graphics and animation API

OpenVSM can export a Proteus active model and a digital model from the same C++
object and Lua state. Set the component's `LUA` property as for an ordinary v0.7
model; relative paths are resolved beside the Proteus project first and below
`LUAVSM` second. Proteus calls the active model for drawing and user input while
its digital side continues to receive pin simulation events.

For devices that share the installed `openvsm.dll`, configure Active Component
Properties with the `OPENVSM` name stem, one state, and DLL linking enabled.
Proteus derives the graphical DLL name from this stem rather than from the
`MODDLL` property, so each such device uses the reusable `OPENVSM_0` active
sprite. The library part name and its `LUA` property remain device-specific.

An active model may define any of these callbacks:

```lua
function device_graphics_init()
end

function device_graphics_plot(state)
end

function device_graphics_animate(element, data_type, value)
end

function device_graphics_actuate(key, x, y, flags)
    return false -- true when the input was handled
end
```

`device_graphics_init` runs once after both the script and Proteus component
surface are available. `device_graphics_animate` receives primitive values
directly. Wire information is `{ voltage=..., current=... }`; SPICE and digital
sample blocks contain `num_timepoints`, `num_pins`, `timepoints`, and a flat
`nodes` or `states` array. User-pointer animation data is deliberately `nil`.

The global `graphics` table provides:

- drawing: `draw_line`, `draw_box`, `draw_circle`, `draw_symbol`, `draw_state`,
  and format-safe `draw_text`;
- vector style: `set_draw_scale`, `set_pen_width`, `set_pen_colour`, and
  `set_brush_colour`;
- text style: `set_text_font`, `set_text_size`, `set_bold`, `set_italic`,
  `set_underline`, and `set_text_colour`;
- component services: `get_symbol_area`, `get_marker`, `repaint`, and
  `set_timestep`;
- Proteus colour, text-justification, actuation-flag, and active-data constants.

Coordinates and sizes use the active component's drawing units. Timesteps use
seconds, matching `ICOMPONENT::settimestep`. `draw_text` always passes model
text as data rather than as a native format string, so percent characters are
safe.

The pointer-valued cache, style, and popup handles are not exposed to portable
Lua models. They require an ownership/lifetime design before they can be added
safely.
