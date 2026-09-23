#pragma once

#include <lua.hpp>
#include <openvsm.hpp>

namespace DeviceSimulator
{
void registerPinLibrary(lua_State *luaContext, const char *name, int number);
void registerPinLibrary(lua_State *luaContext, const char *name, int number, IDSIMPIN *nativePin, IDSIMCKT *dsim);
} // namespace DeviceSimulator
