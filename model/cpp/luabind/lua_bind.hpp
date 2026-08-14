#pragma once
#include <lua.hpp>
#include <string>

namespace LuaScripting
{
bool preloadModule(lua_State *L, const std::string &moduleName);
std::string loadBytecodeFromResource(const std::string &resourceName);
} // namespace LuaScripting
