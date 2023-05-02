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

Written in C99 and gcc-ready (mingw/cygwin) for Linux and Windows

  - You don't need to recompile anything - one DLL for all models in Lua
  - You can create your model as a standalone DLL or use DLL and Lua script together while prototyping
  - You can write your own Lua scripts that will be precompiled and built-in into DLL
  - Function prototypes have similar syntax in C and Lua API
  - Designed with hope to make simulation as simple as possible for electronics enthusiasts


Visit 'examples' directory for sample project files. There is no tutorial yet but I'm working on it

Please kindly send all your remarks and ideas to my mail [o o kami (at) ma il.ru] or submit a bug or feature request

There are plenty to do!

Generally you need to compile DLL from the sources only if you want to include custom scripts.

# Installation
--------------

  - Download OPenVSM MSI installer from `release` section
  - Run installer and install it
  - Visit `exmples` for some example projects  

How to build
--------------

  - Install mingw32 and cygwin for you platform
  - Install Lua 5.3 or higher
  - Clone: https://github.com/Pugnator/openvsm.git openvsm
  - Navigate to the openvsm directory
  - Issue "make" command in Linux or "mingw32-make" under Windows
  - Create environment variable containing path to the script directory,
issuing the following command:

```bat
setx LUAVSM "C:\script"
```
  - In this case you should place your model script to c:\script directory

# License
----

GPL 2
