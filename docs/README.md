# OpenVSM documentation

Start with the [user guide](USER-GUIDE.md) to install OpenVSM and create a Lua
device. The other documents cover specialized APIs and project development.

## Using OpenVSM

- [User guide](USER-GUIDE.md): installation, component setup, Lua models,
  script lookup, and troubleshooting.
- [Lua graphics and animation API](GRAPHICS-LUA-API.md): active schematic
  displays, drawing, repainting, and input.
- [Lua VDM target bridge](VDM-LUA-API.md): expose a CPU-like model to the
  Proteus Virtual Debug Monitor.
- [Third-party control](THIRD-PARTY-CONTROL.md): expose bounded model state to
  external tools through VDM.
- [Lua v0.7 compatibility](V0.7-LUA-COMPATIBILITY.md): supported legacy names
  and migration rules.

## Developing OpenVSM

- [Building from source](BUILDING.md): prerequisites, CMake, tests, installer,
  and generated documentation.
- [Developer guide](DEVELOPER-GUIDE.md): runtime architecture, ownership, and
  API contribution guidance.
- [Code style](CODE-STYLE.md): formatting commands and third-party exclusions.

Runnable scripts are collected in the repository's [examples](../examples)
directory.
