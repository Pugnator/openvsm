#if defined(__GNUC__) && (__GNUC__ >= 4)
#if not defined(_WIN32) and not defined(__MINGW32__) and not defined(__MINGW64__)
#include <dlfcn.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#endif
#endif

#include "lua_context.hpp"
#include "log/log.hpp"

namespace LuaScripting
{
  lua_State *LuaContextManager::getLuaState() const
  {
    return L_;
  }

  LuaContextManager::LuaContextManager()
  {
    LOG_DEBUG("Creating Lua context\n");
    L_ = luaL_newstate();
    if (!L_)
      throw LuaException("Failed to create a new Lua state");

    luaL_openlibs(L_);    

    LOG_DEBUG("Lua context created\n");
  }

  LuaContextManager::~LuaContextManager()
  {
    lua_close(L_);
  }
}