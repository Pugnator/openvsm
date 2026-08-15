#pragma once

#include <lua.hpp>

namespace DeviceSimulator
{
void registerPinLibrary(lua_State *luaContext, const char *name, int number);
} // namespace DeviceSimulator
