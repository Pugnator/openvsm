#pragma once
#include <lua.hpp>

namespace LuaScripting
{
  class ScriptExecutor
  {
  public:
    ScriptExecutor(lua_State *ctx);

    bool loadScriptFromString(const char *script);
    bool loadScriptFromTextFile(const char *fileName);
    bool loadScriptFromBinaryFile(const char *fileName);
    void loadScriptFromResource(const unsigned char *start, const unsigned char *end);
    void execute();

  private:
    bool isFuncExists(const char *funcName);
    lua_State *luactx;
  };
}