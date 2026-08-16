#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace DeviceSimulator
{
struct ModelScriptPath
{
    std::filesystem::path path;
    std::string error;

    explicit operator bool() const
    {
        return error.empty();
    }
};

ModelScriptPath resolveModelScriptPath(std::string_view configuredScript, const std::filesystem::path &projectRoot,
                                       const std::filesystem::path &scriptRoot);
} // namespace DeviceSimulator
