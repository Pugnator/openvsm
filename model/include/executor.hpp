#pragma once
#include <lua.hpp>
#include "waka_api.hpp"

namespace LuaScripting
{
  class ScriptExecutor
  {
  public:
    ScriptExecutor();
    explicit ScriptExecutor(lua_State *);
    ~ScriptExecutor();

    bool loadScriptFromString(const char *script);
    bool loadScriptFromTextFile(const char *fileName);
    bool loadScriptFromBinaryFile(const char *fileName);
    void loadScriptFromResource(const unsigned char *start, const unsigned char *end);
    void execute();

  private:
    lua_State *luaPrepare();

    bool isFuncExists(const char *funcName);
    void safeExecute(void *curfunc);

    lua_State *luactx;
  };    
}

WAKA_API bool runScriptFromTextFile(const char *fileName);