# Building OpenVSM

Most users should install a binary from
[GitHub releases](https://github.com/Pugnator/openvsm/releases). Build from
source when changing the runtime, running its native tests, or producing an
installer.

## Requirements

- Visual Studio 2022 with the **Desktop development with C++** workload and
  Win32 build tools
- CMake 3.21 or newer
- the Proteus VSM SDK headers supplied with Proteus

The v0.7 runtime is C++20 and supports MSVC Win32 builds. CMake is the supported
build entry point; makefiles inside the Lua submodule belong to upstream Lua
and are not used by OpenVSM.

## Checkout and SDK

Clone the repository with its submodules:

```powershell
git clone --recurse-submodules https://github.com/Pugnator/openvsm.git
cd openvsm
```

For an existing checkout:

```powershell
git submodule update --init --recursive
```

Create `externals/sdk` and copy the VSM SDK headers into it. At minimum it must
contain `vsm.hpp`; model-specific VDM headers may be placed beside it. The SDK
directory is ignored because those headers are distributed with Proteus, not
with OpenVSM.

## Configure, build, and test

The checked-in preset selects Visual Studio 2022 and the 32-bit platform:

```powershell
cmake --preset vs2022-win32 -DBUILD_TESTS=ON
cmake --build --preset debug
ctest --test-dir build/vs2022-win32 -C Debug --output-on-failure
```

Build the release configuration with:

```powershell
cmake --build --preset release
```

Outputs are written below `build/vs2022-win32/bin/<configuration>`. For a
manual Visual Studio configuration, select `-A Win32`; MSVC does not use the
GCC `-m32` option.

## Build the installer

Install Inno Setup and make `ISCC.exe` available on `PATH`, then run:

```powershell
cmake -S . -B .build/installer -A Win32 -DDLL_WITH_INSTALLER=ON
cmake --build .build/installer --config Release --target BuildInstaller
```

The installer is written below `.build/installer/installer`. Signing options
and installation behavior are documented in the
[installer README](../externals/installer/README.md).

## Build the developer reference

Install Doxygen, then enable its opt-in target:

```powershell
cmake -S . -B .build/docs -A Win32 -DBUILD_DOCS=ON
cmake --build .build/docs --target openvsm-docs
```

Open `.build/docs/docs/html/index.html`. See the
[developer guide](DEVELOPER-GUIDE.md) before changing the native API and use
the repository's [code-style instructions](CODE-STYLE.md) for C++ changes.
