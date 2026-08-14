#pragma once

#include <cassert>
#include <lua.hpp>

namespace LuaScripting
{
class LuaStackGuard
{
  public:
    explicit LuaStackGuard(lua_State *state) : state_(state), top_(lua_gettop(state))
    {
    }

    ~LuaStackGuard()
    {
        assert(lua_gettop(state_) == top_);
    }

    LuaStackGuard(const LuaStackGuard &) = delete;
    LuaStackGuard &operator=(const LuaStackGuard &) = delete;

    int top() const
    {
        return top_;
    }

  private:
    lua_State *state_;
    int top_;
};
} // namespace LuaScripting
