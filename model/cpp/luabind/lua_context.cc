#if defined(__GNUC__) && (__GNUC__ >= 4)
#if not defined(_WIN32) and not defined(__MINGW32__) and not defined(__MINGW64__)
#include <dlfcn.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#endif
#endif

#include "lua_context.hpp"
//#include "logger/log.hpp"

namespace
{
#if defined(__GNUC__) && (__GNUC__ >= 4)
#if not defined(_WIN32) and not defined(__MINGW32__) and not defined(__MINGW64__)
  std::string getWakaLibPath()
  {
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len == -1)
    {
      LOG_DEBUG("Error finding executable path\n");
      return std::string();
    }
    exePath[len] = '\0';
    char *exeDir = dirname(strdup(exePath));
    char libPath[PATH_MAX];
    snprintf(libPath, sizeof(libPath), "%s/waka.so", exeDir);
    return libPath;
  }
#endif
#endif
}

namespace LuaScripting
{
  lua_State *LuaContextManager::getLuaState() const
  {
    return L_;
  }

  LuaContextManager::LuaContextManager()
  {
    //LOG_DEBUG("Creating Lua context\n");
    L_ = luaL_newstate();
    if (!L_)
      throw LuaException("Failed to create a new Lua state");

    luaL_openlibs(L_);

#if defined(__GNUC__) && (__GNUC__ >= 4)
#if not defined(_WIN32) and not defined(__MINGW32__) and not defined(__MINGW64__)
    std::string wakaPath = getWakaLibPath();
    LOG_DEBUG("Waka library is located at {}\n", wakaPath);
    lua_getglobal(L_, "package");
    lua_getfield(L_, -1, "cpath");
    lua_pushfstring(L_, ";%s", wakaPath.c_str());
    lua_concat(L_, 2);
    lua_setfield(L_, -2, "cpath");
    lua_pop(L_, 1);
#endif
#endif

    lua_getglobal(L_, "require");
    lua_pushstring(L_, "waka");
    if (lua_pcall(L_, 1, 1, 0) != 0)
    {
      //LOG_DEBUG("Error requiring 'waka': {}\n", lua_tostring(L_, -1));
      lua_pop(L_, 1);
      throw LuaException("Error requiring 'waka'");
    }

    //LOG_DEBUG("Lua context created\n");
  }

  LuaContextManager::~LuaContextManager()
  {
    lua_close(L_);
  }
}