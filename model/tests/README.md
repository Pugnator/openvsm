# Model tests

The test suite runs without Proteus by replacing SDK-facing pins with small
in-process fakes. `fixtures/test_ic.lua` is a complete three-pin NAND model used
to check the Lua model contract, lifecycle calls, timing declarations, truth
table, and timer arguments.

Configure and run the suite with Visual Studio's 32-bit generator:

```powershell
cmake -S . -B build/tests -A Win32 -DBUILD_TESTS=ON
cmake --build build/tests --config Debug --target lua_test_ic_test
ctest --test-dir build/tests -C Debug --output-on-failure -R lua_test_ic
```
