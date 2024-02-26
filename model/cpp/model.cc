#include <log/log.hpp>
#include <format>
#include "model.hpp"
#include "lua_context.hpp"
#include "lua_script_executor.hpp"
#include <windows.h>
#include <combaseapi.h>

namespace DeviceSimulator
{
#define PIN_NAME "name"
#define PIN_NUM "number"
#define PIN_OFF_TIME "off_time"
#define PIN_ON_TIME "on_time"

  INT VirtualDevice::isdigital(CHAR *pinname)
  {
    (void)pinname;
    return 1;
  }

  void VirtualDevice::setup(IINSTANCE *instance, IDSIMCKT *dsim)
  {
    GUID guid;
    CoCreateGuid(&guid);
    // GUID to string
    std::string guidStr = std::format("{{{0:08X}-{1:04X}-{2:04X}-{3:02X}{4:02X}-"
                                      "{5:02X}{6:02X}{7:02X}{8:02X}{9:02X}{10:02X}}}",
                                      guid.Data1, guid.Data2, guid.Data3,
                                      guid.Data4[0], guid.Data4[1], guid.Data4[2],
                                      guid.Data4[3], guid.Data4[4], guid.Data4[5],
                                      guid.Data4[6], guid.Data4[7]);

    LOG_DEBUG("Setting up the device {}\n", guidStr);
    instance_ = instance;
    try
    {
      luactx = LuaScripting::LuaContextManager::getInstance().getLuaState();
    }
    catch (const LuaScripting::LuaException &e)
    {
      LOG_DEBUG("Error setting up the Lua context: {}\n", e.what());
    }
    auto scripter = std::make_unique<LuaScripting::ScriptExecutor>();
    bool result = scripter->loadScriptFromTextFile("C:\\Dev\\proteus_lua_script\\nand.lua");
    if (!result)
    {
      LOG_DEBUG("Failed to load the script\n");
      return;
    }
    scripter->execute();

    lua_getglobal(luactx, "device_init");
    if (lua_isfunction(luactx, lua_gettop(luactx)))
    {
      LOG_DEBUG("Initialization function found\n");
    }

    lua_getglobal(luactx, "timer_callback");
    if (lua_isfunction(luactx, lua_gettop(luactx)))
    {
      LOG_DEBUG("Timer callback function found\n");
    }

    lua_getglobal(luactx, "device_simulate");
    if (lua_isfunction(luactx, lua_gettop(luactx)))
    {
      LOG_DEBUG("Simulation function found\n");
    }

    lua_getglobal(luactx, "device_pins");

    if (!lua_istable(luactx, lua_gettop(luactx)))
    {
      LOG_DEBUG("Fatal error, no pin assignments found in script\n");
      return;
    }

    int pinsNum = luaL_len(luactx, -1);
    LOG_DEBUG("Number of pins: {}\n", pinsNum);

    for (int i = 1; i <= pinsNum; ++i)
    {
      lua_rawgeti(luactx, -1, i);

      if (!lua_istable(luactx, -1))
      {
        LOG_DEBUG("Invalid pin entry at index {}\n", i);
        lua_pop(luactx, 1);
        continue;
      }

      lua_getfield(luactx, -1, "name");
      const char *name = lua_tostring(luactx, -1);
      lua_pop(luactx, 1);

      lua_getfield(luactx, -1, "on_time");
      int on_time = lua_tointeger(luactx, -1);
      lua_pop(luactx, 1);

      lua_getfield(luactx, -1, "off_time");
      int off_time = lua_tointeger(luactx, -1);
      lua_pop(luactx, 1);

      LOG_DEBUG("Pin {}: Name={}, On Time={}, Off Time={}\n", i, name, on_time, off_time);
      lua_pop(luactx, 1);
    }

    lua_pop(luactx, 1);
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
    lua_getglobal(luactx, "device_simulate");
    if (lua_isfunction(luactx, lua_gettop(luactx)))
    {
      if (lua_pcall(luactx, 0, 0, 0))
      {
        const char *err = lua_tostring(luactx, -1);
        LOG_DEBUG("Simulation failed with \"{}\"", err);
      }
    }
  }

  void VirtualDevice::callback(ABSTIME time, EVENTID eventid)
  {
  }
}