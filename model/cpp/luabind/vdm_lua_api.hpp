#pragma once

#include <lua.hpp>
#include <vsm.hpp>
#include <vdm.hpp>

namespace DeviceSimulator
{
void registerVdmLuaApi(lua_State *lua);
bool hasLuaVdmHandler(lua_State *lua);
LRESULT dispatchLuaVdmCommand(lua_State *lua, VDM_COMMAND *command, BYTE *data);
void dispatchLuaVdmLoad(lua_State *lua, INT format, INT segment, ADDRESS address, const BYTE *data, INT numbytes);
void dispatchLuaVdmDisassemble(lua_State *lua, ADDRESS address, INT numbytes);
} // namespace DeviceSimulator
