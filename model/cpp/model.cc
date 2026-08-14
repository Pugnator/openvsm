#include <log/log.hpp>
#include <format>
#include "model.hpp"
#include "lua_script_executor.hpp"
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
}

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
      throw std::runtime_error("Failed to create a new Lua state");

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
    LOG_DEBUG("Lua context created\n");
  }

  VirtualDevice::~VirtualDevice()
  {
    LOG_DEBUG("Destroying the device\n");
    lua_close(luactx_);
  }

  INT VirtualDevice::isdigital(CHAR *pinname)
  {
    (void)pinname;
    return 1;
  }

  void VirtualDevice::setup(IINSTANCE *instance, IDSIMCKT *dsim)
  {
    auto &mgr = VirtualContextManager::getInstance();
    dsim_ = dsim;
    instance_ = instance;
    mgr.registerDevice(instance_->id(), *this);
    deviceID_ = instance_->id();
    GUID guid;
    CoCreateGuid(&guid);
    deviceGUID_ = std::format("{{{0:08X}-{1:04X}-{2:04X}-{3:02X}{4:02X}-"
                              "{5:02X}{6:02X}{7:02X}{8:02X}{9:02X}{10:02X}}}",
                              guid.Data1, guid.Data2, guid.Data3,
                              guid.Data4[0], guid.Data4[1], guid.Data4[2],
                              guid.Data4[3], guid.Data4[4], guid.Data4[5],
                              guid.Data4[6], guid.Data4[7]);

    LOG_DEBUG("Setting up the device {} {}\n", deviceID_, deviceGUID_);
    auto scripter = std::make_unique<LuaScripting::ScriptExecutor>(luactx_);
    bool result = scripter->loadScriptFromTextFile("C:\\Dev\\proteus_lua_script\\mcu.lua");
    if (!result)
    {
      LOG_DEBUG("Failed to load the script\n");
      return;
    }
    scripter->execute();

    lua_getglobal(luactx_, "device_init");
    if (lua_isfunction(luactx_, lua_gettop(luactx_)))
    {
      LOG_DEBUG("Initialization function found\n");
      if (lua_pcall(luactx_, 0, 0, 0))
      {
        const char *err = lua_tostring(luactx_, -1);
        LOG_DEBUG("Simulation failed with \"{}\"\n", err);
      }
    }

    lua_getglobal(luactx_, "timer_callback");
    if (lua_isfunction(luactx_, lua_gettop(luactx_)))
    {
      LOG_DEBUG("Timer callback function found\n");
    }

    lua_getglobal(luactx_, "device_simulate");
    if (lua_isfunction(luactx_, lua_gettop(luactx_)))
    {
      LOG_DEBUG("Simulation function found\n");
    }

    lua_getglobal(luactx_, "device_pins");

    if (!lua_istable(luactx_, lua_gettop(luactx_)))
    {
      LOG_DEBUG("Fatal error, no pin assignments found in script\n");
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

      lua_getfield(luactx_, -1, "name");
      const char *name = lua_tostring(luactx_, -1);
      lua_pop(luactx_, 1);

      lua_getfield(luactx_, -1, "on_time");
      int on_time = lua_tointeger(luactx_, -1);
      lua_pop(luactx_, 1);

      lua_getfield(luactx_, -1, "off_time");
      int off_time = lua_tointeger(luactx_, -1);
      lua_pop(luactx_, 1);

      LOG_DEBUG("Pin {}: Name={}, On Time={}, Off Time={}\n", i, name, on_time, off_time);
      devicePins_.push_back({name, i, on_time, off_time});
      registerPin(name, i);
      lua_pop(luactx_, 1);
    }

    lua_pop(luactx_, 1);

    lua_pushinteger(luactx_, UNDEFINED);
    lua_setglobal(luactx_, "UNDEFINED");
    lua_pushinteger(luactx_, TSTATE);
    lua_setglobal(luactx_, "TSTATE");
    lua_pushinteger(luactx_, FSTATE);
    lua_setglobal(luactx_, "FSTATE");
    lua_pushinteger(luactx_, PLO);
    lua_setglobal(luactx_, "PLO");
    lua_pushinteger(luactx_, ILO);
    lua_setglobal(luactx_, "ILO");
    lua_pushinteger(luactx_, SLO);
    lua_setglobal(luactx_, "SLO");
    lua_pushinteger(luactx_, WLO);
    lua_setglobal(luactx_, "WLO");
    lua_pushinteger(luactx_, FLT);
    lua_setglobal(luactx_, "FLT");
    lua_pushinteger(luactx_, WHI);
    lua_setglobal(luactx_, "WHI");
    lua_pushinteger(luactx_, SHI);
    lua_setglobal(luactx_, "SHI");
    lua_pushinteger(luactx_, IHI);
    lua_setglobal(luactx_, "IHI");
    lua_pushinteger(luactx_, PHI);
    lua_setglobal(luactx_, "PHI");
    lua_pushinteger(luactx_, WUD);
    lua_setglobal(luactx_, "WUD");
    lua_pushinteger(luactx_, SUD);
    lua_setglobal(luactx_, "SUD");    

    instance_->getbuspin((CHAR*)"D[4..5]", 2, 2, true);
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
    auto &vinstance = VirtualContextManager::getInstance();
    vinstance.setCurrentDevice(deviceID_);

    lua_getglobal(luactx_, "device_simulate");
    if (lua_isfunction(luactx_, lua_gettop(luactx_)))
    {
      if (lua_pcall(luactx_, 0, 0, 0))
      {
        const char *err = lua_tostring(luactx_, -1);
        LOG_DEBUG("Simulation failed with \"{}\"\n", err);
      }
    }
  }

  void VirtualDevice::callback(ABSTIME time, EVENTID eventid)
  {
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
}