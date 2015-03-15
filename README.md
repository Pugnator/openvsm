OpenVSM
=========

![Openvsm logo by 4eRt0vKa](logo.png)

OpenVSM is a model sumulation engine with C and Lua API for Proteus 7/8 CAD

Documentation is here http://pugnator.github.io/openvsm

Written in C and gcc-ready (mingw/cygwin) for Linux and Windows

  - You don't need to compile anything - one DLL for all models in Lua
  - You can create your model as a standalone DLL or use DLL and Lua script together
  - Function prototypes have similar syntax in C and Lua API
  - Designed with hope to make simulation as simple as possible for electronics enthusiasts


Visit 'example' directory for sample project files

Version
----
0.2

How to
--------------

  - Install mingw32 and cygwin for you platform
  - Clone: https://github.com/Pugnator/openvsm.git openvsm
  - Change to the openvsm/src
  - Issue "make" command in Linux or "mingw32-make" under Windows
  - Create environment variable containing path to the script directory,
issuing the following command:

```bat
setx LUAVSM "C:\script"
```
  - In this case you should place your model script to c:\script directory

License
----

BSD
