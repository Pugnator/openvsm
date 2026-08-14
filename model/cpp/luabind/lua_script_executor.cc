#include "lua_script_executor.hpp"
#include "log/log.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include "lua.hpp"

namespace LuaScripting
{
ScriptExecutor::ScriptExecutor(lua_State *ctx)
{
    if (!ctx)
    {
        throw std::invalid_argument("ScriptExecutor requires a Lua state");
    }
    luaContext_ = ctx;
}

bool ScriptExecutor::captureLuaError(const char *operation)
{
    const char *errorMessage = lua_tostring(luaContext_, -1);
    lastError_ = errorMessage ? errorMessage : "Unknown Lua error";
    LOG_DEBUG("{}: {}\n", operation, lastError_);
    lua_pop(luaContext_, 1);
    return false;
}

bool ScriptExecutor::loadScriptFromString(const char *script)
{
    lastError_.clear();
    int result = luaL_loadstring(luaContext_, script);
    if (result != LUA_OK)
    {
        return captureLuaError("Lua string load error");
    }
    return true;
}

bool ScriptExecutor::loadScriptFromTextFile(const char *fileName)
{
    lastError_.clear();
    int result = luaL_loadfile(luaContext_, fileName);
    if (result != LUA_OK)
    {
        return captureLuaError("Lua file load error");
    }
    return true;
}

bool ScriptExecutor::loadScriptFromBinaryFile(const char *fileName)
{
    (void)fileName;
    lastError_ = "Binary Lua file loading is not implemented";
    return false;
}

bool ScriptExecutor::loadScriptFromResource(const unsigned char *start, const unsigned char *end)
{
    lastError_.clear();
    if (!start || !end || end <= start)
    {
        lastError_ = "Lua resource range is empty or invalid";
        return false;
    }

    const std::size_t bytecodeSize = static_cast<std::size_t>(end - start);
    if (luaL_loadbuffer(luaContext_, reinterpret_cast<const char *>(start), bytecodeSize, "embedded_lua") != LUA_OK)
    {
        return captureLuaError("Lua resource load error");
    }

    if (lua_pcall(luaContext_, 0, 0, 0) != LUA_OK)
    {
        return captureLuaError("Lua resource execution error");
    }

    return true;
}

bool ScriptExecutor::execute()
{
    lastError_.clear();
    if (lua_pcall(luaContext_, 0, 0, 0) != LUA_OK)
    {
        return captureLuaError("Lua execution error");
    }
    return true;
}

const std::string &ScriptExecutor::lastError() const noexcept
{
    return lastError_;
}
} // namespace LuaScripting

bool runScriptFromTextFile(lua_State *ctx, const char *fileName)
{
    auto scripter = std::make_unique<LuaScripting::ScriptExecutor>(ctx);
    bool result = scripter->loadScriptFromTextFile(fileName);
    return result && scripter->execute();
}
