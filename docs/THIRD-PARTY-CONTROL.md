# Third-party model control

OpenVSM uses the Proteus Virtual Debug Monitor as its third-party transport. It
already supplies connection management, play/pause/step/reset commands, memory
and register transfers, breakpoints, and program-counter access. Reusing it
avoids opening an unauthenticated custom socket from every simulated model.

The `vdm_control` Lua module is a generic target profile on top of the Lua VDM
bridge. A model can expose bounded byte-addressable regions with a few lines:

```lua
local root = assert(os.getenv("LUAVSM"), "LUAVSM is not set")
package.path = root .. "/?.lua;" .. package.path
local Control = require("modules.vdm_control")

control = Control.new({
    id = "MY-MODEL",
    regclass = 0,
    regsize = 8,
    clock = 1000000
})

control:register_space(0, 4096)
control:on(VDM_RESET, function(target)
    target.pc = 0
    return ERR_VDM_OK
end)
control:install()
```

`register_space(id, size, initial)` allocates a fixed-size binary region. Reads
and writes are range checked and preserve embedded NUL bytes. The profile also
implements target discovery, target information, registers, PC operations, and
breakpoint bookkeeping. `on(command, callback)` can override or extend any VDM
command; returning `nil` delegates to the profile's default behavior.

A third-party program uses Labcenter's `vdmapi.dll` client API: open and
initialize a handle, then call the documented VDM memory/register/control
functions. The client selects the target using the Proteus VDM configuration;
the SDK's default transport port is 8000. OpenVSM does not redistribute that DLL
or its import library, and their use remains subject to the Proteus SDK licence.

The profile runs on the simulator/VDM callback path. Command callbacks should be
short and deterministic; long-running network or device I/O should be handled
by the external tool, not while Proteus is waiting for a model response.
