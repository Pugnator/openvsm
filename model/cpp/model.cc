#include <log/log.hpp>
#include <cassert>
#include <cstdlib>
#include <format>
#include "model.hpp"
#include "lua_stack_guard.hpp"
#include "model_script_path.hpp"
#include "lua_callback.hpp"
#include "luabind/device/pin_timing.hpp"
#include "luabind/device/lua_event_dispatcher.hpp"
#include "lua_script_executor.hpp"
#include "vsm_lua_api.hpp"
#include <windows.h>
#include <combaseapi.h>

namespace
{
int lua_print(lua_State *L)
{
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; i++)
    {
        if (lua_isstring(L, i))
        {
            auto str = lua_tostring(L, i);
            if (str)
            {
                LOG_INFO("{}", str);
            }
        }

        if (i < nargs)
        {
            LOG_INFO("\t");
        }
    }
    return 0;
}
} // namespace

namespace DeviceSimulator
{
#define PIN_NAME "name"
#define PIN_NUM "number"
#define PIN_OFF_TIME "off_time"
#define PIN_ON_TIME "on_time"

VirtualContextManager &VirtualContextManager::getInstance()
{
    static VirtualContextManager instance;
    return instance;
}

VirtualDevice::VirtualDevice()
{
    LOG_DEBUG("Creating the device\n");
    luactx_ = luaL_newstate();
    if (!luactx_)
    {
        throw std::runtime_error("Failed to create a new Lua state");
    }
    eventDispatcher_ = std::make_unique<LuaEventDispatcher>(luactx_, this);

    LuaScripting::LuaStackGuard stackGuard(luactx_);
    luaL_openlibs(luactx_);
    lua_pushcfunction(luactx_, lua_print);
    lua_setglobal(luactx_, "print");
    lua_getglobal(luactx_, "os");
    lua_getfield(luactx_, -1, "time");
    if (lua_pcall(luactx_, 0, 1, 0) != LUA_OK)
    {
        LOG_DEBUG("Error calling os.time: {}\n", lua_tostring(luactx_, -1));
    }

    lua_getglobal(luactx_, "math");
    lua_getfield(luactx_, -1, "randomseed");
    lua_pushvalue(luactx_, -3);

    if (lua_pcall(luactx_, 1, 0, 0) != LUA_OK)
    {
        LOG_DEBUG("Error calling math.randomseed: {}\n", lua_tostring(luactx_, -1));
    }
    lua_settop(luactx_, stackGuard.top());
    LOG_DEBUG("Lua context created\n");
}

VirtualDevice::~VirtualDevice()
{
    LOG_DEBUG("Destroying the device\n");
    VirtualContextManager::getInstance().unregisterDevice(*this);
    eventDispatcher_.reset();
    lua_close(luactx_);
}

INT VirtualDevice::isdigital(CHAR *pinname)
{
    (void)pinname;
    return 1;
}

void VirtualDevice::setup(IINSTANCE *instance, IDSIMCKT *dsim)
{
    modelReady_ = false;
    LuaScripting::LuaStackGuard stackGuard(luactx_);
    auto &mgr = VirtualContextManager::getInstance();
    dsim_ = dsim;
    instance_ = instance;
    mgr.registerDevice(instance_->id(), *this);
    deviceID_ = instance_->id();
    GUID guid;
    CoCreateGuid(&guid);
    deviceGUID_ = std::format("{{{0:08X}-{1:04X}-{2:04X}-{3:02X}{4:02X}-"
                              "{5:02X}{6:02X}{7:02X}{8:02X}{9:02X}{10:02X}}}",
                              guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
                              guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    LOG_DEBUG("Setting up the device {} {}\n", deviceID_, deviceGUID_);
    constexpr char scriptProperty[] = "LUA";
    const char *configuredScript = instance_->getstrval(const_cast<char *>(scriptProperty));
    const char *scriptRoot = std::getenv("LUAVSM");
    const auto scriptPath = resolveModelScriptPath(configuredScript == nullptr ? "" : configuredScript,
                                                   scriptRoot == nullptr ? "" : scriptRoot);
    if (!scriptPath)
    {
        LOG_DEBUG("Failed to select the model script: {}\n", scriptPath.error);
        return;
    }

    const auto scriptPathText = scriptPath.path.string();
    LOG_DEBUG("Loading model script {}\n", scriptPathText);
    registerVsmLuaApi(luactx_, instance_, dsim_, this);
    auto scripter = std::make_unique<LuaScripting::ScriptExecutor>(luactx_);
    bool result = scripter->loadScriptFromTextFile(scriptPathText.c_str());
    if (!result)
    {
        LOG_DEBUG("Failed to load the script\n");
        lua_settop(luactx_, stackGuard.top());
        return;
    }
    scripter->execute();
    lua_settop(luactx_, stackGuard.top());

    lua_getglobal(luactx_, "device_pins");

    if (!lua_istable(luactx_, lua_gettop(luactx_)))
    {
        LOG_DEBUG("Fatal error, no pin assignments found in script\n");
        lua_pop(luactx_, 1);
        return;
    }

    int pinsNum = luaL_len(luactx_, -1);
    LOG_DEBUG("Number of pins: {}\n", pinsNum);

    for (int i = 1; i <= pinsNum; ++i)
    {
        lua_rawgeti(luactx_, -1, i);

        if (!lua_istable(luactx_, -1))
        {
            LOG_DEBUG("Invalid pin entry at index {}\n", i);
            lua_pop(luactx_, 1);
            continue;
        }

        auto pin = readPinDefinition(luactx_, -1, i);
        LOG_DEBUG("Pin {}: Name={}, On Time={}, Off Time={}\n", i, pin.name, pin.on_time, pin.off_time);
        devicePins_.push_back(pin);
        registerPin(pin);
        lua_pop(luactx_, 1);
    }

    lua_pop(luactx_, 1);

    if (!registerBuses())
    {
        LOG_DEBUG("Failed to register model buses\n");
        return;
    }

    mgr.setCurrentDevice(deviceID_);
    const auto initialization = LuaScripting::invokeCallback(luactx_, "device_init");
    if (initialization.status == LuaScripting::CallbackStatus::succeeded)
    {
        LOG_DEBUG("Device initialization completed\n");
    }
    else if (initialization.status == LuaScripting::CallbackStatus::failed)
    {
        LOG_DEBUG("Device initialization failed with \"{}\"\n", initialization.error);
    }
    modelReady_ = true;
}

void VirtualDevice::runctrl(RUNMODES mode)
{
    switch (mode)
    {
    case RM_BATCH:

        break;
    case RM_START:

        break;
    case RM_STOP:

        break;
    case RM_SUSPEND:

        break;
    case RM_ANIMATE:

        break;
    case RM_STEPTIME:

        break;
    case RM_STEPOVER:

        break;
    case RM_STEPINTO:

        break;
    case RM_STEPOUT:

        break;
    case RM_STEPTO:

        break;
    case RM_META:

        break;
    case RM_DUMP:

        break;
    default:
        LOG_DEBUG("Unknown mode\n");
        break;
    }
}

void VirtualDevice::actuate(REALTIME time, ACTIVESTATE newstate)
{
}

BOOL VirtualDevice::indicate(REALTIME time, ACTIVEDATA *newstate)
{
    return true;
}

void VirtualDevice::simulate(ABSTIME time, DSIMMODES mode)
{
    (void)time;
    (void)mode;
    const int originalStackTop = lua_gettop(luactx_);
    if (!modelReady_ || !simulationCallbackEnabled_)
    {
        assert(lua_gettop(luactx_) == originalStackTop);
        return;
    }

    auto &vinstance = VirtualContextManager::getInstance();
    vinstance.setCurrentDevice(deviceID_);
    eventDispatcher_->dispatch(time, mode);

    lua_getglobal(luactx_, "device_simulate");
    if (!lua_isfunction(luactx_, -1))
    {
        lua_pop(luactx_, 1);
        assert(lua_gettop(luactx_) == originalStackTop);
        return;
    }

    if (lua_pcall(luactx_, 0, 0, 0) != LUA_OK)
    {
        const char *err = lua_tostring(luactx_, -1);
        LOG_DEBUG("Simulation failed with \"{}\"; disabling callback\n", err ? err : "Unknown Lua error");
        lua_pop(luactx_, 1);
        simulationCallbackEnabled_ = false;
    }
    assert(lua_gettop(luactx_) == originalStackTop);
}

void VirtualDevice::callback(ABSTIME time, EVENTID eventid)
{
    if (!modelReady_)
    {
        return;
    }

    auto &vinstance = VirtualContextManager::getInstance();
    vinstance.setCurrentDevice(deviceID_);

    const auto callback = LuaScripting::invokeCallback(
        luactx_, "timer_callback", {static_cast<lua_Integer>(time), static_cast<lua_Integer>(eventid)});
    if (callback.status == LuaScripting::CallbackStatus::failed)
    {
        LOG_DEBUG("Timer callback failed with \"{}\"\n", callback.error);
    }
}

const lua_State *VirtualContextManager::getDeviceLuaContext(const std::string &id)
{
    if (devices_.find(id) != devices_.end())
    {
        VirtualDevice *device = devices_[id];
        return device->getLuaContext();
    }
    return nullptr;
}

const VirtualDevice *VirtualContextManager::getDevice(const std::string &id)
{
    if (devices_.find(id) != devices_.end())
    {
        return devices_[id];
    }
    return nullptr;
}

const VirtualDevice *VirtualContextManager::getDevice()
{
    if (devices_.find(currentDevice_) != devices_.end())
    {
        return devices_[currentDevice_];
    }
    return nullptr;
}

void VirtualContextManager::registerDevice(std::string id, VirtualDevice &device)
{
    devices_[id] = &device;
}
void VirtualContextManager::unregisterDevice(const VirtualDevice &device)
{
    for (auto deviceIt = devices_.begin(); deviceIt != devices_.end();)
    {
        if (deviceIt->second == &device)
        {
            if (currentDevice_ == deviceIt->first)
            {
                currentDevice_.clear();
            }
            deviceIt = devices_.erase(deviceIt);
        }
        else
        {
            ++deviceIt;
        }
    }
}
} // namespace DeviceSimulator
