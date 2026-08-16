#include "lua_script_executor.hpp"
#include <lua.hpp>
#include <string>

int main()
{
    lua_State *luaContext = luaL_newstate();
    if (!luaContext)
    {
        return 1;
    }
    luaL_openlibs(luaContext);

    const int originalTop = lua_gettop(luaContext);
    {
        LuaScripting::ScriptExecutor executor(luaContext);

        const std::string resource = "resource_value = 41";
        const auto *start = reinterpret_cast<const unsigned char *>(resource.data());
        if (!executor.loadScriptFromResource(start, start + resource.size()))
        {
            lua_close(luaContext);
            return 2;
        }
        if (lua_gettop(luaContext) != originalTop)
        {
            lua_close(luaContext);
            return 3;
        }

        const std::string syntaxError = "this is not valid Lua";
        start = reinterpret_cast<const unsigned char *>(syntaxError.data());
        if (executor.loadScriptFromResource(start, start + syntaxError.size()) || executor.lastError().empty() ||
            lua_gettop(luaContext) != originalTop)
        {
            lua_close(luaContext);
            return 4;
        }

        const std::string runtimeError = "error('resource failure')";
        start = reinterpret_cast<const unsigned char *>(runtimeError.data());
        if (executor.loadScriptFromResource(start, start + runtimeError.size()) ||
            executor.lastError().find("resource failure") == std::string::npos || lua_gettop(luaContext) != originalTop)
        {
            lua_close(luaContext);
            return 5;
        }
    }

    if (luaL_dostring(luaContext, "resource_value = resource_value + 1") != LUA_OK)
    {
        lua_close(luaContext);
        return 6;
    }
    lua_getglobal(luaContext, "resource_value");
    const bool stateIsUsable = lua_tointeger(luaContext, -1) == 42;
    lua_pop(luaContext, 1);

    lua_close(luaContext);
    return stateIsUsable ? 0 : 7;
}
