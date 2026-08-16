#include "luabind/device/pin_timing.hpp"
#include "model.hpp"

#include <iostream>

int main()
{
    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }

    if (luaL_dostring(lua, "return {name='Q', on_time=125000, off_time=375000}") != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }

    const auto pin = DeviceSimulator::readPinDefinition(lua, -1, 3);
    lua_pop(lua, 1);
    if (pin.name != "Q" || pin.number != 3 || DeviceSimulator::transitionDelay(TSTATE, pin) != 125000 ||
        DeviceSimulator::transitionDelay(SHI, pin) != 125000 ||
        DeviceSimulator::transitionDelay(FSTATE, pin) != 375000 ||
        DeviceSimulator::transitionDelay(SLO, pin) != 375000 || DeviceSimulator::transitionDelay(FLT, pin) != 0)
    {
        std::cerr << "Pin transition delays were not parsed or selected correctly\n";
        lua_close(lua);
        return 1;
    }

    if (luaL_dostring(lua, "return {name='A', on_time=-1, off_time='invalid'}") != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }

    const auto invalid = DeviceSimulator::readPinDefinition(lua, -1, 1);
    lua_pop(lua, 1);
    lua_close(lua);
    if (invalid.on_time != 0 || invalid.off_time != 0)
    {
        std::cerr << "Invalid pin timings were not normalized to zero\n";
        return 1;
    }

    return 0;
}
