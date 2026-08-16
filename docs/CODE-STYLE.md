# OpenVSM C and C++ style

OpenVSM's project-owned C and C++ sources are formatted with the repository's
`.clang-format` file. The current baseline is clang-format 21.

## Scope

Formatting applies to C and C++ files under `model/cpp` and `model/include`,
except for code maintained outside this project:

- Git submodules, including `model/cpp/log` and `externals/Lua`;
- the locally supplied Proteus SDK under `externals/sdk`;
- the third-party single-header library `model/include/incbin.h`.

Lua, CMake, PowerShell, project data, and generated files are outside the
clang-format scope.

## Running the formatter

From the repository root in PowerShell, format all in-scope files with:

```powershell
./tools/format.ps1
```

Check formatting without changing files with:

```powershell
./tools/format.ps1 -Check
```

## Main rules

- Use four spaces and never tabs.
- Keep lines at or below 120 columns where clang-format can do so.
- Use Allman braces.
- Always brace control-statement bodies.
- Preserve include order; include sorting is intentionally disabled.

Formatting-only changes must be kept separate from behavioral changes so that
reviews and history remain readable.
