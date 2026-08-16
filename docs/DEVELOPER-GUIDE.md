# OpenVSM developer guide {#mainpage}

OpenVSM is a 32-bit Windows DLL that lets a Proteus VSM digital model delegate
its behavior to a Lua script. Proteus owns the simulation interfaces, OpenVSM
owns one Lua state per model instance, and the script describes the device pins
and lifecycle functions.

This guide documents the v0.7 C++ rewrite. The released v0.6 API and examples
may differ while the rewrite is being completed.

## Architecture

The runtime path has four layers:

1. Proteus loads `openvsm.dll` and calls the exported `createdsimmodel` factory.
2. `DeviceSimulator::VirtualDevice` implements the SDK's `IDSIMMODEL` interface
   and owns the Lua state for one schematic component.
3. `LuaScripting::ScriptExecutor` loads the selected script into that state.
4. Native Lua bindings translate script operations to Proteus pin and circuit
   interfaces. The `LuaBytecode` target compiles optional module sources into
   `.luac` artifacts; packaging or loading those artifacts is a separate step.

`DeviceSimulator::VirtualContextManager` connects Lua C functions with the model
instance currently being simulated. It does not own the registered devices.

The proprietary Labcenter SDK is deliberately not copied into the repository.
The build expects its headers under `externals/sdk`; see
[`BUILDING.md`](BUILDING.md) for the checkout and build procedure.

## Model lifecycle

Proteus creates an independent `VirtualDevice` for every component instance.
The important host calls are:

| Host call | OpenVSM responsibility |
| --- | --- |
| `setup` | Save host interfaces, load the component's Lua script, register pins, and initialize the model. |
| `isdigital` | Tell Proteus that a named terminal is handled as a digital pin. |
| `runctrl` | Observe simulator start, stop, animation, and debugger modes. |
| `simulate` | Run per-tick Lua behavior when the host requests it. |
| `callback` | Deliver a previously scheduled event to Lua. |
| `actuate` / `indicate` | Handle interactive schematic state when a model implements it. |

The DLL must not retain host objects beyond their documented lifetime. Lua stack
operations at a C API boundary must have a documented, balanced stack effect.
The `VirtualDevice` destructor is the single owner that closes its Lua state.
The current resource-loader ownership defect is tracked separately in issue
[#47](https://github.com/Pugnator/openvsm/issues/47); do not use that path until
its fix is integrated.

## Source map

- `model/cpp/dllmain.cc`: Windows DLL entry point and Proteus model factory.
- `model/cpp/model.*`: model lifecycle and per-instance state.
- `model/cpp/luabind/`: Lua loading, embedded resources, and native bindings.
- `model/cpp/log/`: logging DLL used by the model.
- `model/lua/`: optional Lua modules and the bytecode source list.
- `examples/`: Proteus projects and their Lua model scripts.
- `model/tests/`: host-independent regression tests.

Generated documentation includes browsable source for these implementation
files. Third-party code and the proprietary SDK are excluded.

## Building this reference

Install Doxygen, then enable the opt-in documentation target in an otherwise
normal build directory:

```powershell
cmake -S . -B .build/docs -A Win32 -DBUILD_DOCS=ON
cmake --build .build/docs --target openvsm-docs
```

Open `.build/docs/docs/html/index.html`. Configuration without `BUILD_DOCS`
does not require Doxygen. If documentation is enabled and Doxygen is missing,
CMake reports the missing prerequisite during configuration.

## Adding or changing an API

Document ownership, nullability, units, valid ranges, and Lua stack effects at
the declaration. Add a focused regression test when behavior can be exercised
without Proteus. For host-dependent behavior, add a minimal example model and
record the Proteus version and manual verification steps.

Format C++ changes with the checked-in `.clang-format`; the exact commands and
excluded third-party paths are in `docs/CODE-STYLE.md`.
