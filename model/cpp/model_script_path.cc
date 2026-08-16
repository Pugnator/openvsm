#include "model_script_path.hpp"

#include <algorithm>
#include <fstream>
#include <vector>

namespace
{
enum class CandidateState
{
    missing,
    ready,
    invalid
};

struct CandidateCheck
{
    CandidateState state;
    std::string error;
};

CandidateCheck inspectCandidate(const std::filesystem::path &path)
{
    std::error_code statusError;
    const bool exists = std::filesystem::exists(path, statusError);
    if (statusError)
    {
        return {CandidateState::invalid,
                "the model script path cannot be inspected: '" + path.string() + "' (" + statusError.message() + ")"};
    }
    if (!exists)
    {
        return {CandidateState::missing, {}};
    }

    const bool regularFile = std::filesystem::is_regular_file(path, statusError);
    if (statusError)
    {
        return {CandidateState::invalid,
                "the model script path cannot be inspected: '" + path.string() + "' (" + statusError.message() + ")"};
    }
    if (!regularFile)
    {
        return {CandidateState::invalid, "the model script is not a regular file: '" + path.string() + "'"};
    }

    std::ifstream script{path, std::ios::binary};
    if (!script.is_open())
    {
        return {CandidateState::invalid, "the model script cannot be opened for reading: '" + path.string() + "'"};
    }

    return {CandidateState::ready, {}};
}

std::string missingScriptError(std::string_view configuredScript, const std::vector<std::filesystem::path> &candidates)
{
    std::string error = "the model script '" + std::string{configuredScript} + "' was not found; searched:";
    for (const auto &candidate : candidates)
    {
        error += " '" + candidate.string() + "'";
    }
    return error;
}
} // namespace

namespace DeviceSimulator
{
ModelScriptPath resolveModelScriptPath(std::string_view configuredScript, const std::filesystem::path &projectRoot,
                                       const std::filesystem::path &scriptRoot)
{
    if (configuredScript.empty())
    {
        return {{}, "the model's LUA property is empty"};
    }

    const std::filesystem::path configuredPath{configuredScript};
    std::vector<std::filesystem::path> candidates;
    if (configuredPath.is_absolute())
    {
        candidates.push_back(configuredPath.lexically_normal());
    }
    else
    {
        const auto addCandidate = [&](const std::filesystem::path &root)
        {
            if (root.empty())
            {
                return;
            }

            const auto candidate = (root / configuredPath).lexically_normal();
            if (std::find(candidates.begin(), candidates.end(), candidate) == candidates.end())
            {
                candidates.push_back(candidate);
            }
        };

        addCandidate(projectRoot);
        addCandidate(scriptRoot);
    }

    if (candidates.empty())
    {
        return {{},
                "the relative model script '" + configuredPath.string() +
                    "' cannot be resolved because neither the Proteus project directory nor LUAVSM is available"};
    }

    for (const auto &candidate : candidates)
    {
        const auto check = inspectCandidate(candidate);
        if (check.state == CandidateState::ready)
        {
            return {candidate, {}};
        }
        if (check.state == CandidateState::invalid)
        {
            return {candidate, check.error};
        }
    }

    return {candidates.front(), missingScriptError(configuredScript, candidates)};
}
} // namespace DeviceSimulator
