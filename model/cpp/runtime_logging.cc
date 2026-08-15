#include "runtime_logging.hpp"

#include <fstream>
#include <log/log.hpp>

namespace DeviceSimulator
{
void initializeRuntimeLogging() noexcept
{
    static const bool initialized = []() noexcept
    {
        try
        {
            std::ofstream logProbe("log.txt", std::ios::app);
            if (logProbe.is_open())
            {
                logProbe.close();
                Log::get().configure(TraceType::file);
            }
            else
            {
                Log::get().configure(TraceType::devnull);
            }
            Log::get().set_level(TraceSeverity::debug);
            LOG_DEBUG("The model is being loaded\n");
        }
        catch (...)
        {
            try
            {
                Log::get().configure(TraceType::devnull);
            }
            catch (...)
            {
            }
        }
        return true;
    }();
    (void)initialized;
}
} // namespace DeviceSimulator
