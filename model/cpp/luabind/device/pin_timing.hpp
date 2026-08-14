#pragma once

#include <lua.hpp>
#include <vsm.hpp>

namespace DeviceSimulator
{
struct model_pin;

RELTIME normalizePinTiming(lua_Integer timing);
model_pin readPinDefinition(lua_State *lua, int tableIndex, int pinNumber);
RELTIME transitionDelay(STATE state, const model_pin &pin);
} // namespace DeviceSimulator
