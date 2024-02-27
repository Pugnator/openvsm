#pragma once
#include <vsm.hpp>
#include <lua.hpp>
#include <map>

namespace DeviceSimulator
{
  class VirtualDevice : public IDSIMMODEL
  {
  public:
    VirtualDevice();
    ~VirtualDevice();

    INT isdigital(CHAR *pinname) override;
    void setup(IINSTANCE *instance, IDSIMCKT *dsim) override;
    void runctrl(RUNMODES mode) override;
    void actuate(REALTIME time, ACTIVESTATE newstate) override;
    BOOL indicate(REALTIME time, ACTIVEDATA *newstate) override;
    void simulate(ABSTIME time, DSIMMODES mode) override;
    void callback(ABSTIME time, EVENTID eventid) override;
    
    lua_State *getLuaContext() const
    {
      return luactx_;
    }

    IDSIMPIN* getPin(CHAR *name) const
    {
      return instance_->getdsimpin(name, true);
    }

  private:
    void registerPin(const char *name, int num);

    lua_State *luactx_;
    IINSTANCE *instance_;
    std::string deviceID_;
    std::string deviceGUID_;
  };

  class VirtualContextManager
  {
  public:
    static VirtualContextManager &getInstance()
    {
      static VirtualContextManager instance;
      return instance;
    }

    VirtualContextManager(VirtualContextManager const &) = delete;
    void operator=(VirtualContextManager const &) = delete;

    const VirtualDevice *getDevice(const std::string &id);
    const VirtualDevice *getDevice();
    const lua_State *getDeviceLuaContext(const std::string &id);
    void registerDevice(std::string id, VirtualDevice &device);
    void setCurrentDevice(const std::string &id)
    {
      currentDevice_ = id;
    }

  private:
    VirtualContextManager(){};

    std::string currentDevice_;
    std::map<std::string, VirtualDevice *> devices_;
    lua_State *luactx_;
  };
}