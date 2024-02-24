#pragma once
#include <lua.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <map>

#define LSTRING (&lua_isstring)
#define LINT (&lua_isnumber)
#define LUSER (&lua_islightuserdata)
#define LTABLE (&lua_istable)
#define LNONE (NULL)

#define LUA_REGISTER_FUNCTION(ctx, funcName, cFunc) \
  lua_pushcfunction(ctx, cFunc);                    \
  lua_setglobal(ctx, funcName);

#define LUA_REGISTER_METHOD(ctx, methodName, cFunc) \
  lua_pushcfunction(ctx, cFunc);                    \
  lua_setfield(ctx, -2, methodName);

typedef struct
{
  const char *lua_func_name;
  int (*lua_c_api)(lua_State *);
  int (*args[16])(lua_State *, int index);
} luaBindFunc;

namespace LuaScripting
{
  bool preloadModule(lua_State *L, const std::string &moduleName);
  std::string loadBytecodeFromResource(const std::string &resourceName);

  std::vector<char32_t> luaPopChar32Array(lua_State *L, int argPosition = 1);
  std::u32string luaPopU32String(lua_State *L, int argPosition = 1);
  std::string luaPopString(lua_State *L, int argPosition = 1);
  int luaPushU32String(lua_State *L, const std::u32string &str);
  int luaPushString(lua_State *L, const std::string &str);
  int luaPushVectorInt(lua_State *L, const std::vector<uint32_t> &vec);
  int luaPushVectorString32(lua_State *L, const std::vector<std::u32string> &vec);
  int luaPushVectorString(lua_State *L, const std::vector<std::string> &vec);
  int luaPushMapStringInt(lua_State *L, const std::map<std::string, int> &map);

  void registerDictManagement(lua_State *ctx);
  void registerKanaProcessor(lua_State *ctx);
  void registerDictSearch(lua_State *ctx);
  void registerKanjiSearch(lua_State *L);
  void registerJlptSearch(lua_State *L);
  void registerExamplesSearch(lua_State *L);
  void registerKanjiVgSearch(lua_State *L);

  void registerUserNotes(lua_State *L);
}
