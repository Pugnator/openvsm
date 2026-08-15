# OpenVSM installer

The Inno Setup script is built through CMake so it receives the selected
configuration's self-contained `openvsm.dll` path and the version from
`model/VERSION.txt`.

```powershell
cmake -S . -B .build/installer -A Win32 -DDLL_WITH_INSTALLER=ON
cmake --build .build/installer --config Release --target BuildInstaller
```

The installer is written below `.build/installer/installer`. A normal OpenVSM
configure/build leaves `DLL_WITH_INSTALLER` disabled and does not require Inno
Setup.

Setup creates the machine-wide `LUAVSM` environment variable only when it is
absent. A pre-existing value is left unchanged. On uninstall, Setup removes
`LUAVSM` only when it created the value and the user has not changed it since.

Release builds can sign both `openvsm.dll` and the generated installer with a
certificate from the Windows certificate store:

```powershell
cmake -S . -B .build/signed -A Win32 -DDLL_WITH_INSTALLER=ON `
  -DOPENVSM_SIGN_CERTIFICATE_SHA1=<certificate-thumbprint>
cmake --build .build/signed --config Release --target BuildInstaller
```

Set `OPENVSM_SIGNTOOL` when `signtool.exe` is not on `PATH` or in the standard
Windows SDK directory. `OPENVSM_SIGN_TIMESTAMP_URL` selects the RFC 3161
timestamp service and defaults to DigiCert.

Setup first checks the shared Proteus 8 Models directory below
`{commonappdata}\Labcenter Electronics\Proteus 8 Professional\MODELS`, then
falls back to detecting Proteus 7 or 8 Professional from the registry. The
resulting directory is displayed for confirmation. If detection fails, the user
must select an existing Proteus Models directory; Setup does not silently
install model DLLs into the OpenVSM application directory.

Lua examples are copied below `{app}\LuaScripts` with their repository
subdirectories preserved. Existing or user-modified example files are never
overwritten or removed during uninstall.
