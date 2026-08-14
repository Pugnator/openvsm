#include "lua_callback.hpp"

#include <utility>

namespace LuaScripting
{
CallbackResult invokeCallback(lua_State *lua, const char *name, std::initializer_list<lua_Integer> arguments)
{
    if (lua == nullptr)
    {
        return {CallbackStatus::failed, "Lua state is not available"};
    }

    const int stackTop = lua_gettop(lua);
    lua_getglobal(lua, name);
    if (!lua_isfunction(lua, -1))
    {
        lua_settop(lua, stackTop);
        return {CallbackStatus::notDefined, {}};
    }

    for (const auto argument : arguments)
    {
        lua_pushinteger(lua, argument);
    }

    if (lua_pcall(lua, static_cast<int>(arguments.size()), 0, 0) != LUA_OK)
    {
        const char *message = lua_tostring(lua, -1);
        std::string error = message == nullptr ? "unknown Lua error" : message;
        lua_settop(lua, stackTop);
        return {CallbackStatus::failed, std::move(error)};
    }

    lua_settop(lua, stackTop);
    return {CallbackStatus::succeeded, {}};
}
} // namespace LuaScripting
