OpenVSM
=========

OpenVSM is a sumulation model engine with C and Lua API
It is created to be ready for build with gcc (mingw/cygwin) on Linux and Windows machines

  - You can create your model as standalone DLL or use DLL and Lua script
  - You can freely mix up C language and Lua
  - Function prototypes have the same syntax in both API
  - It is created with hope to amke modelling as simple as possible even for people who
  don't know C
  - It is recommended to use only Lua API

OpenVSM is written in C.
For example usage visit 'example' directory

Version
----

0.1

Tech
-----------

Xenomatic uses Lua 5.2 library

* [Liblua52] - awesome scripting language for embedded use

Build instructions

--------------

```sh
git clone https://Pugnator@bitbucket.org/Pugnator/openvsm.git openvsm
cd openvsm
make
```


License
----

GPL v2


**Free Software!**
