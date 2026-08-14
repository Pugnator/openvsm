# Lua VDM target bridge

The Proteus SDK already supplies the Virtual Debug Monitor protocol in
`vdm.hpp`; reverse engineering another model DLL is not required. A CPU-like
OpenVSM model can opt in by defining `device_vdm_command`. OpenVSM then registers
the model through `IINSTANCE::setvdmhlr` and forwards host VDM commands to Lua.

## Command callback

```lua
function device_vdm_command(command, memspace, address, length, payload)
    -- Return a VDM status and, for reads, an optional binary string.
    return ERR_VDM_OK, response
end
```

The five arguments are the fields of the SDK `VDM_COMMAND` plus the request
payload. `payload` is a binary-safe Lua string for `VDM_WRITEDATA` and
`VDM_WRITEREGS`; it is empty for other commands. The optional response must not
exceed the requested `length`. OpenVSM copies it to the SDK buffer and reports
its actual length. A Lua error becomes `ERR_VDM_SIMFAILED`, an invalid result
becomes `ERR_VDM_FAILED`, and an oversized result becomes
`ERR_VDM_BADDATALEN`.

Lua 5.4 `string.pack` and `string.unpack` are suitable for register blocks and
the fixed-layout SDK structures. For example, a four-byte program counter can
be returned with `string.pack("<I4", pc)`.

The bridge registers `VDM_API_VERSION`, the `VDM*_CLASS` target classes, all
`VDM_*` command constants, and the host-facing `ERR_VDM_*` result constants.
Target-specific memory-space numbers and register layouts remain the model's
responsibility; the SDK headers `vdm11.hpp`, `vdm51.hpp`, and `vdmpic.hpp`
define the standard layouts.

## Loader and disassembler callbacks

CPU models may also define:

```lua
function device_vdm_load(format, segment, address, payload)
    -- Consume a binary chunk supplied by the Proteus loader.
end

function device_vdm_disassemble(address, length)
    -- Update model-specific disassembly state if required.
end
```

Both callbacks are optional and isolated from the C++ stack. The SDK
`getvardata` service is deliberately reported as unsupported for now because it
requires stable, model-owned memory and type descriptors; returning temporary
Lua storage would create dangling host pointers.

## External client

The other half of the SDK is `vdmapi.dll`. A third-party debugger opens a
handle with `vdm_open`, initializes it with `vdm_init`, and then uses the
documented play, pause, memory, register, breakpoint, and PC functions. The
default SDK transport port is 8000. Distribution of Labcenter binaries and
headers remains subject to the Proteus SDK licence and is outside OpenVSM.
