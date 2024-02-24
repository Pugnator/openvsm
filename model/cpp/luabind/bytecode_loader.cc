#include <lua.hpp>
#include "luabind/lua_bind.hpp"
#include "logger/log.hpp"
#include "module_config.hpp"

namespace LuaScripting
{
  std::string loadBytecodeFromResource(const std::string &resourceName)
  {
    auto it = resourceMap.find(resourceName);
    if (it != resourceMap.end())
    {
      return std::string(it->second.data, it->second.size);
    }
    LOG_DEBUG("Resource {} not found in map\n", resourceName);
    return std::string();
  }

  bool preloadModule(lua_State *L, const std::string &moduleName)
  {
    bool result = false;
    std::string code = loadBytecodeFromResource(moduleName);
    if (code.empty())
    {
      LOG_DEBUG("Could not load bytecode of {} from resources\n", moduleName);
      return result;
    }
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    if (luaL_loadbuffer(L, code.c_str(), code.size(), moduleName.c_str()) == 0)
    {
      lua_setfield(L, -2, moduleName.c_str());
      result = true;
    }
    else
    {
      LOG_DEBUG("Error preloading module {}: {}\n", moduleName, lua_tostring(L, -1));
      lua_pop(L, 1);
    }

    lua_pop(L, 2);
    return result;
  }
}