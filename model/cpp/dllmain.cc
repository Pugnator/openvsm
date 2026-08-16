#include <windows.h>
#include <vsm.hpp>
#include <log/log.hpp>
#include "active_model.hpp"
#include "model.hpp"
#include "runtime_logging.hpp"

namespace
{
bool vsmRegister(ILICENCESERVER *ils)
{
    return ils && ils->authorize(0, VSM_API_VERSION);
}

const char *deviceName(const char *device)
{
    return device && *device ? device : "<unknown>";
}

} // namespace

extern "C"
{
    IACTIVEMODEL __declspec(dllexport) * createactivemodel(char *device, ILICENCESERVER *ils)
    {
        DeviceSimulator::initializeRuntimeLogging();
        LOG_DEBUG("Creating active model for device {}\n", deviceName(device));
        if (!ils || !vsmRegister(ils))
        {
            LOG_DEBUG("VSM authorization failed; active model creation refused\n");
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
        DeviceSimulator::initializeRuntimeLogging();
        LOG_DEBUG("Creating digital model for device {}\n", deviceName(device));
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
