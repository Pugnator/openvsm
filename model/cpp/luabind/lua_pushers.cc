#include "luabind/lua_bind.hpp"
#include "metrics/profiler.hpp"

namespace LuaScripting
{
  std::vector<char32_t> luaPopChar32Array(lua_State *L, int argPosition)
  {
    RECORD_CALL();
    std::vector<char32_t> charArray;

    if (!lua_istable(L, argPosition))
    {
      luaL_error(L, "Expected a table as the argument");
      return charArray;
    }

    lua_pushnil(L); // Push the first key to start traversal
    while (lua_next(L, argPosition))
    {
      char32_t character = static_cast<char32_t>(lua_tointeger(L, -1));
      charArray.push_back(character);
      lua_pop(L, 1); // Pop the value, keep the key for the next iteration
    }

    return charArray;
  }

  std::u32string luaPopU32String(lua_State *L, int argPosition)
  {
    RECORD_CALL();
    if (!lua_istable(L, argPosition))
    {
      luaL_error(L, "Expected a table as the argument");
      return std::u32string();
    }
    int tableLength = lua_rawlen(L, 1);
    std::u32string input;
    for (int i = 1; i <= tableLength; i++)
    {
      lua_pushinteger(L, i);
      lua_gettable(L, argPosition);
      char32_t character = static_cast<char32_t>(luaL_checkinteger(L, -1));
      input.push_back(character);
      lua_pop(L, 1);
    }
    return input;
  }

  std::string luaPopString(lua_State *L, int argPosition)
  {
    RECORD_CALL();
    if (!lua_istable(L, argPosition))
    {
      luaL_error(L, "Expected a table as the argument");
      return std::string();
    }

    int tableLength = lua_rawlen(L, argPosition);
    std::string input;
    for (int i = 1; i <= tableLength; i++)
    {
      lua_pushinteger(L, i);
      lua_gettable(L, argPosition);
      const char *character = luaL_checkstring(L, -1);
      input += character;
      lua_pop(L, 1); // Corrected to pop one element from the stack
    }

    return input;
  }

  int luaPushU32String(lua_State *L, const std::u32string &str)
  {
    RECORD_CALL();
    lua_newtable(L);
    int index = 1;
    int tableStackPos = lua_gettop(L);
    for (const char32_t c : str)
    {
      lua_pushinteger(L, static_cast<lua_Integer>(c));
      lua_rawseti(L, tableStackPos, index);
      index++;
    }
    return 1;
  }

  int luaPushString(lua_State *L, const std::string &str)
  {
    RECORD_CALL();
    lua_pushlstring(L, str.c_str(), str.length()); // Push the entire string as a single Lua string
    return 1;
  }

  int luaPushVectorInt(lua_State *L, const std::vector<uint32_t> &vec)
  {
    RECORD_CALL();
    if (vec.size() == 0)
    {
      lua_pushnil(L);
      return 1;
    }

    lua_createtable(L, vec.size(), 0);
    int newTable = lua_gettop(L);

    for (size_t index = 0; index < vec.size(); ++index)
    {
      lua_pushinteger(L, vec[index]);
      lua_rawseti(L, newTable, index + 1);
    }
    return 1;
  }

  int luaPushMapStringInt(lua_State *L, const std::map<std::string, int> &map)
  {
    RECORD_CALL();
    if (map.empty())
    {
      lua_pushnil(L);
      return 1;
    }

    lua_createtable(L, 0, map.size());

    for (const auto &pair : map)
    {
      lua_pushstring(L, pair.first.c_str()); // Push the key
      lua_pushinteger(L, pair.second);       // Push the value
      lua_settable(L, -3);                   // Set the key-value pair in the table
    }

    return 1; // The table is on top of the stack
  }

  int luaPushVectorString32(lua_State *L, const std::vector<std::u32string> &strVector)
  {
    RECORD_CALL();
    if (strVector.size() == 0)
    {
      lua_pushnil(L);
      return 1;
    }

    lua_newtable(L);
    int index = 1;
    int tableStackPos = lua_gettop(L);

    for (const std::u32string &str : strVector)
    {
      luaPushU32String(L, str);
      lua_rawseti(L, tableStackPos, index);
      index++;
    }

    return 1;
  }

  int luaPushVectorString(lua_State *L, const std::vector<std::string> &strVector)
  {
    RECORD_CALL();
    if (strVector.size() == 0)
    {
      lua_pushnil(L);
      return 1;
    }

    lua_newtable(L);
    int index = 1;
    int tableStackPos = lua_gettop(L);

    for (const std::string &str : strVector)
    {
      luaPushString(L, str);
      lua_rawseti(L, tableStackPos, index);
      index++;
    }

    return 1;
  }
}