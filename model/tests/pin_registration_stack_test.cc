#include <string>
#include <vector>
#include "luabind/device/pin.hpp"
#include "model.hpp"
#include <lua.hpp>

int main()
{
    {
        DeviceSimulator::VirtualDevice device;
        if (lua_gettop(device.getLuaContext()) != 0)
        {
            return 1;
        }
    }

    lua_State *luaContext = luaL_newstate();
    if (!luaContext)
    {
        return 2;
    }

    for (int number = 1; number <= 8; ++number)
    {
        const int originalTop = lua_gettop(luaContext);
        const std::string name = "PIN" + std::to_string(number);

        DeviceSimulator::registerPinLibrary(luaContext, name.c_str(), number);
        if (lua_gettop(luaContext) != originalTop)
        {
            lua_close(luaContext);
            return 3;
        }

        lua_getglobal(luaContext, name.c_str());
        const bool isTable = lua_istable(luaContext, -1);
        lua_getfield(luaContext, -1, "pinNumber");
        const bool hasNumber = lua_tointeger(luaContext, -1) == number;
        lua_pop(luaContext, 1);
        lua_getfield(luaContext, -1, "pinName");
        const char *registeredName = lua_tostring(luaContext, -1);
        const bool hasName = registeredName && name == registeredName;
        lua_settop(luaContext, originalTop);

        if (!isTable || !hasNumber || !hasName)
        {
            lua_close(luaContext);
            return 4;
        }
    }

    lua_close(luaContext);
    return 0;
}
