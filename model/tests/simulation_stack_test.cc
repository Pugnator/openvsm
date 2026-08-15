#include <string>
#include <vector>
#include "model.hpp"
#include <lua.hpp>

namespace
{
bool runLua(lua_State *luaContext, const char *script)
{
    if (luaL_dostring(luaContext, script) == LUA_OK)
    {
        return true;
    }
    lua_pop(luaContext, 1);
    return false;
}

bool stackIsStable(DeviceSimulator::VirtualDevice &device, int ticks)
{
    lua_State *luaContext = device.getLuaContext();
    const int originalTop = lua_gettop(luaContext);
    for (int tick = 0; tick < ticks; ++tick)
    {
        device.simulate(0, DSIMNORMAL);
        if (lua_gettop(luaContext) != originalTop)
        {
            return false;
        }
    }
    return true;
}
} // namespace

int main()
{
    {
        DeviceSimulator::VirtualDevice device;
        if (!stackIsStable(device, 128))
        {
            return 1;
        }
    }

    {
        DeviceSimulator::VirtualDevice device;
        lua_State *luaContext = device.getLuaContext();
        if (!runLua(luaContext, "device_simulate = 42") || !stackIsStable(device, 128))
        {
            return 2;
        }
    }

    {
        DeviceSimulator::VirtualDevice device;
        lua_State *luaContext = device.getLuaContext();
        if (!runLua(luaContext,
                    "simulation_calls = 0; function device_simulate() simulation_calls = simulation_calls + 1 end") ||
            !stackIsStable(device, 128))
        {
            return 3;
        }
        lua_getglobal(luaContext, "simulation_calls");
        const bool calledEveryTick = lua_tointeger(luaContext, -1) == 128;
        lua_pop(luaContext, 1);
        if (!calledEveryTick)
        {
            return 4;
        }
    }

    {
        DeviceSimulator::VirtualDevice device;
        lua_State *luaContext = device.getLuaContext();
        if (!runLua(luaContext,
                    "simulation_calls = 0; function device_simulate() simulation_calls = simulation_calls + 1; "
                    "error('tick failure') end") ||
            !stackIsStable(device, 128))
        {
            return 5;
        }
        lua_getglobal(luaContext, "simulation_calls");
        const bool disabledAfterFailure = lua_tointeger(luaContext, -1) == 1;
        lua_pop(luaContext, 1);
        if (!disabledAfterFailure)
        {
            return 6;
        }
    }

    return 0;
}
