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
