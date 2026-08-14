#include "model_script_path.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <lua.hpp>

namespace
{
class TemporaryDirectory
{
  public:
    TemporaryDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() / ("openvsm model scripts " + std::to_string(suffix));
        std::filesystem::create_directories(path_);
    }

    ~TemporaryDirectory()
    {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }

    const std::filesystem::path &path() const
    {
        return path_;
    }

  private:
    std::filesystem::path path_;
};

bool writeScript(const std::filesystem::path &path, int modelId)
{
    std::ofstream script{path};
    script << "model_id = " << modelId << '\n';
    return script.good();
}

bool loadAndCheck(lua_State *lua, const std::filesystem::path &path, int expectedModelId)
{
    if (luaL_dofile(lua, path.string().c_str()) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_pop(lua, 1);
        return false;
    }

    lua_getglobal(lua, "model_id");
    const bool matches = lua_isinteger(lua, -1) && lua_tointeger(lua, -1) == expectedModelId;
    lua_pop(lua, 1);
    return matches;
}
} // namespace

int main()
{
    TemporaryDirectory scripts;
    const auto firstScript = scripts.path() / "first model.lua";
    const auto secondScript = scripts.path() / "second model.lua";
    if (!writeScript(firstScript, 1) || !writeScript(secondScript, 2))
    {
        std::cerr << "Failed to create the test scripts\n";
        return 1;
    }

    const auto first = DeviceSimulator::resolveModelScriptPath("first model.lua", scripts.path().string());
    const auto second = DeviceSimulator::resolveModelScriptPath(secondScript.string(), "unused root");
    if (!first || !second || first.path != firstScript.lexically_normal() ||
        second.path != secondScript.lexically_normal())
    {
        std::cerr << "Failed to resolve distinct relative and absolute model scripts\n";
        return 1;
    }

    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        std::cerr << "Failed to create a Lua state\n";
        return 1;
    }

    const bool scriptsLoaded = loadAndCheck(lua, first.path, 1) && loadAndCheck(lua, second.path, 2);
    lua_close(lua);
    if (!scriptsLoaded)
    {
        std::cerr << "Failed to load the independently selected model scripts\n";
        return 1;
    }

    const auto noRoot = DeviceSimulator::resolveModelScriptPath("relative.lua", {});
    const auto missing = DeviceSimulator::resolveModelScriptPath("missing.lua", scripts.path().string());
    if (noRoot || missing)
    {
        std::cerr << "Invalid script configuration was accepted\n";
        return 1;
    }

    return 0;
}
