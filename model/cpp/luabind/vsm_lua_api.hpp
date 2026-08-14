#pragma once

#include <lua.hpp>
#include <vsm.hpp>

namespace DeviceSimulator
{
/** Register the supported v0.7 constants and host service functions in Lua.
 *
 * The host interface pointers and model are borrowed for the lifetime of the
 * containing VirtualDevice. This function leaves the Lua stack balanced.
 */
void registerVsmLuaApi(lua_State *lua, IINSTANCE *instance, IDSIMCKT *dsim, IDSIMMODEL *model);
} // namespace DeviceSimulator
