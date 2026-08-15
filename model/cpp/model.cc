#include <log/log.hpp>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include "model.hpp"
#include "lua_stack_guard.hpp"
#include "model_script_path.hpp"
#include "lua_callback.hpp"
#include "luabind/device/pin_timing.hpp"
#include "luabind/device/lua_event_dispatcher.hpp"
#include "lua_script_executor.hpp"
#include "vdm_lua_api.hpp"
#include "vsm_lua_api.hpp"
#include <windows.h>
#include <combaseapi.h>

namespace
{
constexpr char randomSeedSequenceKey[] = "openvsm.random_seed.sequence";
std::atomic<std::uint64_t> nextRandomSeedSequence{1};

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

void seedLuaRandomGenerator(lua_State *lua)
{
    const auto sequence = nextRandomSeedSequence.fetch_add(1, std::memory_order_relaxed);
    const auto timestamp = static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    const auto address = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(lua));
    const auto firstSeed = static_cast<lua_Integer>(timestamp ^ (address + 0x9e3779b97f4a7c15ULL));
    const auto secondSeed = static_cast<lua_Integer>(sequence);

    lua_getglobal(lua, "math");
    lua_getfield(lua, -1, "randomseed");
    lua_pushinteger(lua, firstSeed);
    lua_pushinteger(lua, secondSeed);
    if (lua_pcall(lua, 2, 0, 0) != LUA_OK)
    {
        LOG_DEBUG("Error calling math.randomseed: {}\n", lua_tostring(lua, -1));
        return;
    }

    lua_pushinteger(lua, secondSeed);
    lua_setfield(lua, LUA_REGISTRYINDEX, randomSeedSequenceKey);
}
} // namespace

namespace DeviceSimulator
{
#define PIN_NAME "name"
#define PIN_NUM "number"
#define PIN_OFF_TIME "off_time"
#define PIN_ON_TIME "on_time"

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
    seedLuaRandomGenerator(luactx_);
    lua_settop(luactx_, stackGuard.top());
    LOG_DEBUG("Lua context created\n");
}

VirtualDevice::~VirtualDevice()
{
    LOG_DEBUG("Destroying the device\n");
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
    dsim_ = dsim;
    instance_ = instance;
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
    registerVdmLuaApi(luactx_);
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

    if (hasLuaVdmHandler(luactx_) && !instance_->setvdmhlr(this))
    {
        LOG_DEBUG("Proteus rejected the Lua VDM handler\n");
    }

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

    const auto callback = LuaScripting::invokeCallback(
        luactx_, "timer_callback", {static_cast<lua_Integer>(time), static_cast<lua_Integer>(eventid)});
    if (callback.status == LuaScripting::CallbackStatus::failed)
    {
        LOG_DEBUG("Timer callback failed with \"{}\"\n", callback.error);
    }
}

LRESULT VirtualDevice::vdmhlr(VDM_COMMAND *command, BYTE *data)
{
    return dispatchLuaVdmCommand(luactx_, command, data);
}

void VirtualDevice::loaddata(INT format, INT segment, ADDRESS address, BYTE *data, INT numbytes)
{
    dispatchLuaVdmLoad(luactx_, format, segment, address, data, numbytes);
}

void VirtualDevice::disassemble(ADDRESS address, INT numbytes)
{
    dispatchLuaVdmDisassemble(luactx_, address, numbytes);
}

BOOL VirtualDevice::getvardata(VARITEM *item, VARDATA *data)
{
    (void)item;
    (void)data;
    return FALSE;
}

} // namespace DeviceSimulator
