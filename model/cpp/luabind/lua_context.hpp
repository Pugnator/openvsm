#pragma once
#include <stdexcept>
#include <lua.hpp>
#include "lua_bind.hpp"
#include "module_config.hpp"

namespace LuaScripting
{
  class LuaException : public std::runtime_error
  {
  public:
    LuaException(const std::string &message)
        : std::runtime_error("LuaException: " + message) {}
  };

  class LuaContextManager
  {
  public:
    static LuaContextManager &getInstance()
    {
      static LuaContextManager instance;
      return instance;
    }

    lua_State *getLuaState() const;

  private:
    LuaContextManager();
    ~LuaContextManager();

    lua_State *L_ = nullptr;
    LuaContextManager(const LuaContextManager &) = delete;
    LuaContextManager &operator=(const LuaContextManager &) = delete;
  };
}