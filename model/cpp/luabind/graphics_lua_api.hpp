#pragma once

#include <lua.hpp>
#include <vsm.hpp>

namespace DeviceSimulator
{
void registerGraphicsLuaApi(lua_State *lua, ICOMPONENT *component);
bool dispatchLuaGraphicsInit(lua_State *lua);
void dispatchLuaGraphicsPlot(lua_State *lua, ACTIVESTATE state);
void dispatchLuaGraphicsAnimate(lua_State *lua, INT element, const ACTIVEDATA *data);
bool dispatchLuaGraphicsActuate(lua_State *lua, WORD key, INT x, INT y, DWORD flags);
} // namespace DeviceSimulator
