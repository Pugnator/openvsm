#pragma once

#include <lua.hpp>
#include <vsm.hpp>

namespace DeviceSimulator
{
void registerVsmLuaApi(lua_State *lua, IINSTANCE *instance, IDSIMCKT *dsim, IDSIMMODEL *model);
} // namespace DeviceSimulator
