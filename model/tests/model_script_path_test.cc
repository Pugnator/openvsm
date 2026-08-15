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
    const auto projectRoot = scripts.path() / "project";
    const auto installedRoot = scripts.path() / "installed";
    std::filesystem::create_directories(projectRoot);
    std::filesystem::create_directories(installedRoot);

    const auto projectScript = projectRoot / "shared model.lua";
    const auto shadowedInstalledScript = installedRoot / "shared model.lua";
    const auto fallbackScript = installedRoot / "fallback model.lua";
    const auto absoluteScript = scripts.path() / "absolute model.lua";
    if (!writeScript(projectScript, 1) || !writeScript(shadowedInstalledScript, 2) || !writeScript(fallbackScript, 3) ||
        !writeScript(absoluteScript, 4))
    {
        std::cerr << "Failed to create the test scripts\n";
        return 1;
    }

    const auto projectFirst = DeviceSimulator::resolveModelScriptPath("shared model.lua", projectRoot, installedRoot);
    const auto installedFallback =
        DeviceSimulator::resolveModelScriptPath("fallback model.lua", projectRoot, installedRoot);
    const auto absolute =
        DeviceSimulator::resolveModelScriptPath(absoluteScript.string(), "unused project", "unused root");
    const auto projectOnly =
        DeviceSimulator::resolveModelScriptPath("shared model.lua", projectRoot, std::filesystem::path{});
    if (!projectFirst || !installedFallback || !absolute || !projectOnly ||
        projectFirst.path != projectScript.lexically_normal() ||
        installedFallback.path != fallbackScript.lexically_normal() ||
        absolute.path != absoluteScript.lexically_normal() || projectOnly.path != projectScript.lexically_normal())
    {
        std::cerr << "Failed to apply project-first model script resolution\n";
        return 1;
    }

    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        std::cerr << "Failed to create a Lua state\n";
        return 1;
    }

    const bool scriptsLoaded = loadAndCheck(lua, projectFirst.path, 1) &&
                               loadAndCheck(lua, installedFallback.path, 3) && loadAndCheck(lua, absolute.path, 4);
    lua_close(lua);
    if (!scriptsLoaded)
    {
        std::cerr << "Failed to load the independently selected model scripts\n";
        return 1;
    }

    const auto missing = DeviceSimulator::resolveModelScriptPath("missing.lua", projectRoot, installedRoot);
    const auto missingProjectPath = (projectRoot / "missing.lua").lexically_normal().string();
    const auto missingInstalledPath = (installedRoot / "missing.lua").lexically_normal().string();
    if (missing || missing.error.find(missingProjectPath) == std::string::npos ||
        missing.error.find(missingInstalledPath) == std::string::npos)
    {
        std::cerr << "Missing-script diagnostics did not list every searched path\n";
        return 1;
    }

    std::filesystem::create_directory(projectRoot / "blocked.lua");
    if (!writeScript(installedRoot / "blocked.lua", 5))
    {
        std::cerr << "Failed to create the masked fallback script\n";
        return 1;
    }
    const auto invalidProjectOverride =
        DeviceSimulator::resolveModelScriptPath("blocked.lua", projectRoot, installedRoot);
    const auto noRoot = DeviceSimulator::resolveModelScriptPath("relative.lua", {}, {});
    const auto emptyProperty = DeviceSimulator::resolveModelScriptPath({}, projectRoot, installedRoot);
    if (invalidProjectOverride || invalidProjectOverride.path != (projectRoot / "blocked.lua").lexically_normal() ||
        noRoot || emptyProperty)
    {
        std::cerr << "Invalid script configuration was accepted\n";
        return 1;
    }

    return 0;
}
