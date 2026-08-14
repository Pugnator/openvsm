#pragma once

#include <initializer_list>
#include <string>

#include <lua.hpp>

namespace LuaScripting
{
enum class CallbackStatus
{
    notDefined,
    succeeded,
    failed
};

struct CallbackResult
{
    CallbackStatus status;
    std::string error;
};

CallbackResult invokeCallback(lua_State *lua, const char *name, std::initializer_list<lua_Integer> arguments = {});
} // namespace LuaScripting
