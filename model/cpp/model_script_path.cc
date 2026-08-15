#include "model_script_path.hpp"

#include <fstream>

namespace DeviceSimulator
{
ModelScriptPath resolveModelScriptPath(std::string_view configuredScript, std::string_view scriptRoot)
{
    if (configuredScript.empty())
    {
        return {{}, "the model's LUA property is empty"};
    }

    std::filesystem::path path{configuredScript};
    if (path.is_relative())
    {
        if (scriptRoot.empty())
        {
            return {{}, "LUAVSM is not set for the relative script path '" + path.string() + "'"};
        }
        path = std::filesystem::path{scriptRoot} / path;
    }

    path = path.lexically_normal();

    std::error_code fileError;
    if (!std::filesystem::is_regular_file(path, fileError))
    {
        return {path, "the model script is not a readable regular file: '" + path.string() + "'"};
    }

    std::ifstream script{path, std::ios::binary};
    if (!script.is_open())
    {
        return {path, "the model script cannot be opened for reading: '" + path.string() + "'"};
    }

    return {path, {}};
}
} // namespace DeviceSimulator
