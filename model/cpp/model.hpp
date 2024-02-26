#pragma once
#include <vsm.hpp>
#include <lua.hpp>

namespace DeviceSimulator
{
  class VirtualDevice : public IDSIMMODEL
  {
  public:
    INT isdigital(CHAR *pinname) override;
    void setup(IINSTANCE *instance, IDSIMCKT *dsim) override;
    void runctrl(RUNMODES mode) override;
    void actuate(REALTIME time, ACTIVESTATE newstate) override;
    BOOL indicate(REALTIME time, ACTIVEDATA *newstate) override;
    void simulate(ABSTIME time, DSIMMODES mode) override;
    void callback(ABSTIME time, EVENTID eventid) override;

  private:
    lua_State *luactx;
    IINSTANCE *instance_;
  };
}