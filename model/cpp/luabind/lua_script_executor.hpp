#pragma once
#include <lua.hpp>
#include <string>

namespace LuaScripting
{
/** Loads and executes chunks in a caller-owned Lua state.
 *
 * The non-owning contract is being enforced by issue #47. Until that fix is
 * integrated, loadScriptFromResource() must not be called because the current
 * implementation closes the borrowed state. A successful source/file load
 * leaves the loaded chunk on the Lua stack for execute().
 */
class ScriptExecutor
{
  public:
    /** Bind an executor to a non-null, caller-owned Lua state.
     * The caller must keep the state alive for the executor's lifetime. */
    explicit ScriptExecutor(lua_State *ctx);
    ~ScriptExecutor() = default;

    /** Compile a null-terminated Lua source string. */
    bool loadScriptFromString(const char *script);
    /** Compile a Lua source file from disk. */
    bool loadScriptFromTextFile(const char *fileName);
    /** Load a precompiled Lua chunk from disk. */
    bool loadScriptFromBinaryFile(const char *fileName);
    /** Load and execute the bytecode in [start, end). */
    bool loadScriptFromResource(const unsigned char *start, const unsigned char *end);
    /** Execute the chunk currently at the top of the Lua stack. */
    bool execute();

    /** Return the most recent load or execution error. */
    const std::string &lastError() const noexcept;

  private:
    bool captureLuaError(const char *operation);

    lua_State *luaContext_;
    std::string lastError_;
};
} // namespace LuaScripting
