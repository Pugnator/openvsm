#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
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
} // namespace

extern "C"
{
    IACTIVEMODEL __declspec(dllexport) * createactivemodel(char *device, ILICENCESERVER *ils)
    {
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

bool APIENTRY DllMain(HINSTANCE hInstDLL, uint32_t fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        Log::get().configure(TraceType::file);
        Log::get().set_level(TraceSeverity::debug);
        LOG_DEBUG("The model is being loaded\n");
        break;
    case DLL_PROCESS_DETACH:
        LOG_DEBUG("The model is being unloaded\n");
        break;
    case DLL_THREAD_ATTACH:
        LOG_DEBUG("A thread is being created\n");
        break;
    case DLL_THREAD_DETACH:
        LOG_DEBUG("A thread is being destroyed\n");
        break;
    }
    (void)hInstDLL;
    (void)fdwReason;
    (void)lpvReserved;
    return true;
}
