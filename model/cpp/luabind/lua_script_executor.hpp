#pragma once
#include <lua.hpp>
#include <string>

namespace LuaScripting
{
class ScriptExecutor
{
  public:
    // The caller retains ownership of the Lua state and must keep it alive for
    // the lifetime of this executor.
    explicit ScriptExecutor(lua_State *ctx);
    ~ScriptExecutor() = default;

    bool loadScriptFromString(const char *script);
    bool loadScriptFromTextFile(const char *fileName);
    bool loadScriptFromBinaryFile(const char *fileName);
    bool loadScriptFromResource(const unsigned char *start, const unsigned char *end);
    bool execute();

    const std::string &lastError() const noexcept;

  private:
    bool captureLuaError(const char *operation);

    lua_State *luaContext_;
    std::string lastError_;
};
} // namespace LuaScripting
