#include "lua_script_executor.hpp"
//#include "logger/log.hpp"
#include "lua_context.hpp"
#include "lua_bind.hpp"
#include <memory>

namespace LuaScripting
{
  ScriptExecutor::ScriptExecutor()
  {
    luactx = LuaContextManager::getInstance().getLuaState();
  }

  bool ScriptExecutor::loadScriptFromString(const char *script)
  {    
    int result = luaL_loadstring(luactx, script);
    if (result != LUA_OK)
    {
      const char *errorMessage = lua_tostring(luactx, -1);
      //LOG_DEBUG("Lua Load Error: {}\n", errorMessage);
      return false;
    }
    return true;
  }

  bool ScriptExecutor::loadScriptFromTextFile(const char *fileName)
  {   
    int result = luaL_loadfile(luactx, fileName);
    if (result != LUA_OK)
    {
      const char *errorMessage = lua_tostring(luactx, -1);
      //LOG_DEBUG("Lua Load Error: {}\n", errorMessage);
      return false;
    }
    return true;
  }

  bool ScriptExecutor::loadScriptFromBinaryFile(const char *fileName)
  {    
    return false;
  }

  void ScriptExecutor::loadScriptFromResource(const unsigned char *start, const unsigned char *end)
  {    
    lua_State *L = LuaContextManager::getInstance().getLuaState();

    int bytecodeSize = end - start;

    if (luaL_loadbuffer(L, reinterpret_cast<const char *>(start), bytecodeSize, "embedded_lua") == 0)
    {
      int result = lua_pcall(L, 0, 0, 0);
      if (result != 0)
      {
        const char *errorMsg = lua_tostring(L, -1);
      }
    }
    else
    {
      const char *errorMsg = lua_tostring(L, -1);
    }

    lua_close(L);
  }

  bool ScriptExecutor::isFuncExists(const char *funcName)
  {    
    lua_getglobal(luactx, funcName);
    if (lua_isfunction(luactx, lua_gettop(this->luactx)))
    {
      return true;
    }
    return false;
  }

  void ScriptExecutor::execute()
  {
    int result = lua_pcall(luactx, 0, 0, 0);
    if (result != LUA_OK)
    {
      const char *errorMessage = lua_tostring(luactx, -1);
      //LOG_DEBUG("Lua Error: {}\n", errorMessage);
    }
  }
}

bool runScriptFromTextFile(const char *fileName)
{
  auto scripter = std::make_unique<LuaScripting::ScriptExecutor>();
  bool result = scripter->loadScriptFromTextFile(fileName);
  scripter->execute();
  return result;
}