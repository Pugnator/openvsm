#pragma once
#include <tuple>
#include <optional>
#include <memory>

#include "lua_context.hpp"

namespace LuaScripting
{
  using TableReference = int;
  class LuaTableFacade
  {
  public:
    using Ptr = std::unique_ptr<LuaTableFacade>;

    LuaTableFacade(const std::string &module);
    ~LuaTableFacade();

    LuaTableFacade(const LuaTableFacade &) = delete;
    LuaTableFacade &operator=(const LuaTableFacade &) = delete;

    template <typename Ret, typename... Args>
    Ret callMethod(const std::string &methodName, Args... args)
    {
      // Push the Lua table onto the stack using the stored reference
      lua_rawgeti(L_, LUA_REGISTRYINDEX, tableRef_);
      if (!lua_istable(L_, -1))
      {
        throw LuaException("Not a table");
      }

      // Push the method onto the stack
      lua_getfield(L_, -1, methodName.c_str());
      if (!lua_isfunction(L_, -1))
      {
        throw LuaException("Not a function");
      }

      lua_pushvalue(L_, -2);
      (luaPushArgs(args), ...);
      if (lua_pcall(L_, sizeof...(Args) + 1, 1, 0) != 0)
      {
        std::string message;
        int errorType = lua_type(L_, -1);
        switch (errorType)
        {
        case LUA_TSTRING:
          // Error is a string
          message = lua_tostring(L_, -1);
          break;
        case LUA_TTABLE:
          // Handle table error object
          // [Your table extraction logic here]
          break;
        // Add cases for other types as needed
        case LUA_TNUMBER:
          message = "Error object is a number: " + std::to_string(lua_tonumber(L_, -1));
          break;
        case LUA_TBOOLEAN:
          message = "Error object is a boolean: " + std::string(lua_toboolean(L_, -1) ? "true" : "false");
          break;
        // Add more cases for other Lua types
        default:
          message = "Unknown error type: " + std::string(lua_typename(L_, errorType));
          break;
        }

        luaL_traceback(L_, L_, message.c_str(), 1);
        const char *traceback = lua_tostring(L_, -1);
        std::string error = traceback ? traceback : "Error with no traceback";
        lua_pop(L_, 2); // Pop error message and traceback
        throw LuaException(error);
      }
      Ret returnValue = getReturnValue<Ret>(-1);
      lua_pop(L_, 2);
      return returnValue;
    }

    void newSelfInstance(int newReference);

  private:
    lua_State *L_ = nullptr;
    int tableRef_ = LUA_NOREF;

    template <typename T>
    void luaPushArgs(T value)
    {
      if constexpr (std::is_integral_v<T>)
      {
        lua_pushinteger(L_, static_cast<lua_Integer>(value));
      }
      else if constexpr (std::is_floating_point_v<T>)
      {
        lua_pushnumber(L_, static_cast<lua_Number>(value));
      }
      else if constexpr (std::is_same_v<T, const char *>)
      {
        lua_pushstring(L_, value);
      }
      else if constexpr (std::is_same_v<T, std::string>)
      {
        lua_pushstring(L_, value.c_str());
      }
      else
      {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> ||
                          std::is_same_v<T, const char *> || std::is_same_v<T, std::string>,
                      "Unsupported argument type for luaPushArgs");
      }
    }

    template <typename T>
    T getReturnValue(int index)
    {
      if (lua_isnil(L_, index))
      {
        return T();
      }

      if constexpr (std::is_same_v<T, std::string>)
      {
        size_t len;
        const char *cstr = lua_tolstring(L_, index, &len);
        return std::string(cstr, len);
      }
      else if constexpr (std::is_same_v<T, std::vector<std::string>>)
      {
        std::vector<std::string> result;

        luaL_checktype(L_, index, LUA_TTABLE);
        lua_pushnil(L_);
        while (lua_next(L_, index - 1) != 0)
        {
          if (lua_isstring(L_, -1))
          {
            result.emplace_back(lua_tostring(L_, -1));
          }
          lua_pop(L_, 1);
        }

        return result;
      }
      else if constexpr (std::is_same_v<T, std::vector<unsigned>>)
      {
        std::vector<unsigned> result;

        luaL_checktype(L_, index, LUA_TTABLE);
        lua_pushnil(L_);
        while (lua_next(L_, index - 1) != 0)
        {
          if (lua_isinteger(L_, -1))
          {
            result.emplace_back(lua_tointeger(L_, -1));
          }
          lua_pop(L_, 1);
        }

        return result;
      }
      else if constexpr (std::is_same_v<T, TableReference>)
      {
        lua_pushvalue(L_, index);
        int ref = luaL_ref(L_, LUA_REGISTRYINDEX);
        return static_cast<T>(ref);
      }
      else if constexpr (std::is_integral_v<T>)
      {
        return static_cast<T>(lua_tointeger(L_, index));
      }
      else if constexpr (std::is_floating_point_v<T>)
      {
        return static_cast<T>(lua_tonumber(L_, index));
      }
      else if constexpr (std::is_same_v<T, bool>)
      {
        return static_cast<T>(lua_toboolean(L_, index));
      }

      throw LuaException("Unsupported return type");
    }

    bool checkLuaTable(lua_State *L, int tableRef);
  };
}