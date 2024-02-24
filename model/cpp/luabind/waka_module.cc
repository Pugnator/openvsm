#include <lua.hpp>
#include "luabind/lua_bind.hpp"
#include "logger/log.hpp"
#include "waka_api.hpp"
#include "settings/manager.hpp"
#include "module_config.hpp"
#include "lua_context.hpp"

static int lua_print(lua_State *L)
{
  int nargs = lua_gettop(L);
  for (int i = 1; i <= nargs; i++)
  {
    if (lua_isstring(L, i))
    {
      auto str = lua_tostring(L, i);
      if (str)
      {
        LOG_INFO("{}", str);
      }
    }

    if (i < nargs)
    {
      LOG_INFO("\t");
    }
  }
  LOG_INFO("\n");
  return 0;
}

static void requireAllModules(lua_State *L)
{
  for (const auto &moduleName : modulesToPreload)
  {
    lua_getglobal(L, "require");
    lua_pushstring(L, moduleName.c_str());
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
      LOG_DEBUG("Error requiring 'waka': {}\n", lua_tostring(L, -1));
      lua_pop(L, 1);
      throw LuaScripting::LuaException("Error requiring the module");
    }
    LOG_DEBUG("Module {} required successfully\n", moduleName);
  }
}

extern "C" WAKA_API int luaopen_waka(lua_State *L)
{
  LOG_DEBUG("Loading Waka module\n");
  luaL_openlibs(L);
  // Logging from Lua function
  lua_pushcfunction(L, lua_print);
  lua_setglobal(L, "print");

  LuaScripting::registerDictManagement(L);
  LuaScripting::registerKanaProcessor(L);
  LuaScripting::registerDictSearch(L);
  LuaScripting::registerUserNotes(L);

  for (const auto &moduleName : modulesToPreload)
  {
    if (!LuaScripting::preloadModule(L, moduleName))
    {
      LOG_DEBUG("Could not preload module {}\n", moduleName);
      return 0;
    }
  }

  // Seed the random generator

  lua_getglobal(L, "os");
  lua_getfield(L, -1, "time");
  if (lua_pcall(L, 0, 1, 0) != LUA_OK)
  {
    LOG_DEBUG("Error calling os.time: {}\n", lua_tostring(L, -1));
    return 0;
  }

  lua_getglobal(L, "math");
  lua_getfield(L, -1, "randomseed");
  lua_pushvalue(L, -3);

  if (lua_pcall(L, 1, 0, 0) != LUA_OK)
  {
    LOG_DEBUG("Error calling math.randomseed: {}\n", lua_tostring(L, -1));
    return 0;
  }

  LOG_DEBUG("Waka module loaded\n");
  return 1;
}