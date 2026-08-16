#pragma once

#include <string_view>

namespace DeviceSimulator
{
void initializeRuntimeLogging() noexcept;
void logRuntimeDebug(std::string_view message) noexcept;
} // namespace DeviceSimulator
