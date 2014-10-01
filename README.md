OpenVSM
=========

OpenVSM is a sumulation model engine with C and Lua API

It is created to be ready for build with gcc (mingw/cygwin) on Linux and Windows machines

  - You can create your model as standalone DLL or use DLL and Lua script
  - You can freely mix up C language and Lua
  - Function prototypes have the same syntax in both API
  - It is created with hope to make simulation as simple as possible even for people who
  don't know C/C++

OpenVSM is written in C and use Lua 5.3.
Visit 'example' directory for sample projects

Version
----

0.1

Used libraries
-----------

* [Liblua53] - awesome and fast scripting language

Build instructions

--------------

```sh
git clone https://Pugnator@bitbucket.org/Pugnator/openvsm.git openvsm
cd openvsm
make #mingw32-make under win32
```
Copy lua53/lua.dll to %windor%\system32

Create environment variable with path to script directory,
by issuing the following command:

```bat
setx LUAVSM "C:\script\"
```
So you should put your model script to c:\script
Note the trailing backslash
Your script should be named after the name of model DLL, for example:

```model.dll
model.dll.lua```

License
----

GPL v2


**Free Software**
