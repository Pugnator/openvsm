# OpenVSM is Lua bindings for Proteus 7/8 CAD
[![Release][release-image]][releases] [![Wiki][wiki-img]][wiki]


[release-image]: https://img.shields.io/badge/release-0.6.213-blue.svg?style=flat
[releases]: https://github.com/Pugnator/openvsm/releases
[wiki-img]: https://img.shields.io/badge/docs-Wiki-blue.svg
[wiki]: https://github.com/Pugnator/openvsm/wiki


Tested on Proteus 8.9

Please report all issues you face in order to make this tool better

![Lua logo](http://www.lua.org/images/powered-by-lua.gif)

Powered by Lua http://www.lua.org/

Documentation can be found at http://pugnator.github.io/openvsm

Prebuilt DLL and symbols or installer are in [Release](https://github.com/Pugnator/openvsm/releases) section

The v0.7 native model is written in C++20 and currently builds with 32-bit MSVC.

  - You don't need to recompile anything - one DLL for all models in Lua
  - You can create your model as a standalone DLL or use DLL and Lua script together while prototyping
  - You can write your own Lua scripts that will be precompiled and built-in into DLL
  - Function prototypes have similar syntax in C and Lua API
  - Designed with hope to make simulation as simple as possible for electronics enthusiasts


Visit 'examples' directory for sample project files. There is no tutorial yet but I'm working on it

Please kindly send all your remarks and ideas to my mail [o o kami (at) ma il.ru] or submit a bug or feature request

There are plenty to do!

Generally you need to compile DLL from the sources only if you want to include custom scripts.

## Native buses

Lua models declare native Proteus buses alongside `device_pins`:

```lua
device_buses = {
    {name = "D", base = 0, width = 8, on_time = 100000,
     off_time = 120000, tristate_time = 50000}
}
```

Widths from 1 through 32 bits are supported. Each declaration creates a global
object (for example `D`) with `set`/`drive`, `tristate`, `drivebit`, `get`,
`getdrive`, `getbitstate`, `settiming`, and `setstates` methods. Bit indices are
zero-based, and drive values must fit the declared width. The optional
`on_state`, `off_state`, `tristate_state`, and `required` fields configure the
corresponding SDK behavior. Call methods with Lua's object syntax, such as
`D:drive(0x5a)` or `D:drivebit(0, SHI)`.

# Installation
--------------

  - Download OPenVSM MSI installer from `release` section
  - Run installer and install it
  - Visit `exmples` for some example projects  

## How to build

### Requirements

- Visual Studio 2022 with the **Desktop development with C++** workload and
  Win32 build tools;
- CMake 3.21 or newer (CMake 4.x is supported);
- the Proteus VSM SDK headers supplied with your Proteus installation.

CMake is the only supported project build entry point. Makefiles inside
`externals/Lua` belong to that upstream submodule and are not used by OpenVSM.

### Checkout

Clone with the Lua and TinyLog submodules:

```powershell
git clone --recurse-submodules https://github.com/Pugnator/openvsm.git
cd openvsm
```

For an existing checkout, initialize them with:

```powershell
git submodule update --init --recursive
```

### Proteus SDK

Create `externals/sdk` and copy the VSM SDK headers into it. At minimum, the
directory must contain `vsm.hpp`; model-specific VDM headers can be placed next
to it. The directory is ignored by Git because the SDK is distributed with
Proteus rather than this project.

### Configure and build

The checked-in preset selects Visual Studio 2022 and its 32-bit platform:

```powershell
cmake --preset vs2022-win32
cmake --build --preset debug
cmake --build --preset release
```

For a manual Visual Studio configuration, select the same target with
`-A Win32`; MSVC does not use the GCC `-m32` option.

Build outputs are written below `build/vs2022-win32/bin/<configuration>`.

# License
----

GPL 2
