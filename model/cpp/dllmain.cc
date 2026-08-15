#include <windows.h>
#include <fstream>
#include <vsm.hpp>
#include <log/log.hpp>
#include "active_model.hpp"
#include "model.hpp"

namespace
{
bool vsmRegister(ILICENCESERVER *ils)
{
    return ils && ils->authorize(0, VSM_API_VERSION);
}

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
#ifdef OPENVSM_DEBUG
            Log::get().set_level(TraceSeverity::debug);
#endif
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
} // namespace

extern "C"
{
    IACTIVEMODEL __declspec(dllexport) * createactivemodel(char *device, ILICENCESERVER *ils)
    {
        initializeRuntimeLogging();
        (void)device;
        if (!ils || !vsmRegister(ils))
        {
            return nullptr;
        }
        return new DeviceSimulator::LuaActiveModel;
    }

    void __declspec(dllexport) deleteactivemodel(IACTIVEMODEL *model)
    {
        delete static_cast<DeviceSimulator::LuaActiveModel *>(model);
    }

    IDSIMMODEL __declspec(dllexport) * createdsimmodel(char *device, ILICENCESERVER *ils)
    {
        initializeRuntimeLogging();
        (void)device;
        if (!vsmRegister(ils))
        {
            LOG_DEBUG("VSM authorization failed; model creation refused\n");
            return nullptr;
        }
        return new DeviceSimulator::VirtualDevice;
    }

    void __declspec(dllexport) deletedsimmodel(IDSIMMODEL *model)
    {
        if (!model)
        {
            return;
        }

        auto *device = dynamic_cast<DeviceSimulator::VirtualDevice *>(model);
        if (!device)
        {
            LOG_DEBUG("Refusing to delete an unknown IDSIMMODEL implementation\n");
            return;
        }
        delete device;
    }
}
