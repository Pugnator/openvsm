OpenVSM
=========

![Openvsm logo by 4eRt0vKa](logo.png)

OpenVSM is a model sumulation engine with C and Lua API for Proteus 7/8 CAD

Documentation can be found at http://pugnator.github.io/openvsm

Prebuild DLL and symbols can be found in Release section

Written in C and gcc-ready (mingw/cygwin) for Linux and Windows

  - You don't need to recompile anything - one DLL for all models in Lua
  - You can create your model as a standalone DLL or use DLL and Lua script together while prototyping
  - You can write your own Lua scripts that will be precompiled and built-in into DLL
  - Function prototypes have similar syntax in C and Lua API
  - Designed with hope to make simulation as simple as possible for electronics enthusiasts


Visit 'examples' directory for sample project files. There is no tutorial yet but I'm working on it

Please kindly send all your remarks and ideas to my mail [o o kami (at) ma il.ru] or submit a bug or feature request

There are plenty to do!

Version
----
0.2

Generally you need to compile DLL from the sources only if you want to include custom scripts.

How to build
--------------

  - Install mingw32 and cygwin for you platform
  - Install Lua 5.3 or higher compiler
  - Clone: https://github.com/Pugnator/openvsm.git openvsm
  - Change to the openvsm
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
