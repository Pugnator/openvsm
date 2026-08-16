#include "luabind/lua_callback.hpp"

#include <iostream>
#include <string>

namespace
{
bool run(lua_State *lua, const char *script)
{
    if (luaL_dostring(lua, script) == LUA_OK)
    {
        return true;
    }

    std::cerr << lua_tostring(lua, -1) << '\n';
    lua_pop(lua, 1);
    return false;
}

bool checkLifecycleEvents(lua_State *lua)
{
    lua_getglobal(lua, "events");
    if (!lua_istable(lua, -1) || lua_rawlen(lua, -1) != 3)
    {
        lua_pop(lua, 1);
        return false;
    }

    lua_rawgeti(lua, -1, 1);
    const bool initializedFirst = std::string{lua_tostring(lua, -1)} == "init";
    lua_pop(lua, 1);
    lua_rawgeti(lua, -1, 2);
    const bool timeForwarded = lua_tointeger(lua, -1) == 0x123456789LL;
    lua_pop(lua, 1);
    lua_rawgeti(lua, -1, 3);
    const bool eventForwarded = lua_tointeger(lua, -1) == 73;
    lua_pop(lua, 2);
    return initializedFirst && timeForwarded && eventForwarded;
}
} // namespace

int main()
{
    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    const char *lifecycleScript = R"(
        events = {}
        function device_init()
            assert(A ~= nil)
            assert(TSTATE == 1)
            table.insert(events, "init")
        end
        function timer_callback(time, event_id)
            table.insert(events, time)
            table.insert(events, event_id)
        end
    )";
    if (!run(lua, lifecycleScript))
    {
        lua_close(lua);
        return 1;
    }

    lua_newtable(lua);
    lua_setglobal(lua, "A");
    lua_pushinteger(lua, 1);
    lua_setglobal(lua, "TSTATE");

    const int lifecycleStackTop = lua_gettop(lua);
    const auto initialized = LuaScripting::invokeCallback(lua, "device_init");
    const auto timer = LuaScripting::invokeCallback(lua, "timer_callback", {0x123456789LL, 73});
    if (initialized.status != LuaScripting::CallbackStatus::succeeded ||
        timer.status != LuaScripting::CallbackStatus::succeeded || lua_gettop(lua) != lifecycleStackTop ||
        !checkLifecycleEvents(lua))
    {
        std::cerr << "Lifecycle callbacks were not dispatched in order with their arguments\n";
        lua_close(lua);
        return 1;
    }

    lua_pushstring(lua, "stack sentinel");
    const int errorStackTop = lua_gettop(lua);
    if (!run(lua, "function timer_callback() error('timer exploded') end"))
    {
        lua_close(lua);
        return 1;
    }
    const auto failed = LuaScripting::invokeCallback(lua, "timer_callback", {1, 2});
    const auto missing = LuaScripting::invokeCallback(lua, "not_defined");
    if (failed.status != LuaScripting::CallbackStatus::failed ||
        failed.error.find("timer exploded") == std::string::npos ||
        missing.status != LuaScripting::CallbackStatus::notDefined || lua_gettop(lua) != errorStackTop ||
        std::string{lua_tostring(lua, -1)} != "stack sentinel")
    {
        std::cerr << "Callback errors or missing functions corrupted the Lua stack\n";
        lua_close(lua);
        return 1;
    }

    lua_pop(lua, 1);
    lua_close(lua);
    return 0;
}
