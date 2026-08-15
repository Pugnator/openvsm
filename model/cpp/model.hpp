#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <vsm.hpp>
#include <lua.hpp>

namespace DeviceSimulator
{
class LuaEventDispatcher;

/** Configuration for one Lua-declared digital pin. Timing values use Proteus
 * absolute-time ticks. */
struct model_pin
{
    std::string name;
    int number;
    RELTIME on_time;
    RELTIME off_time;
};

/** A single Proteus component backed by an independent Lua state.
 *
 * Proteus owns the host interface pointers passed to setup(). VirtualDevice
 * owns luactx_ and closes it when the concrete model is destroyed.
 */
class VirtualDevice : public IDSIMMODEL, public ICPU
{
  public:
    /** Allocate and initialize the model's Lua state. */
    VirtualDevice();
    /** Close the owned Lua state. */
    ~VirtualDevice();

    /** Report that a component terminal uses the digital simulation engine. */
    INT isdigital(CHAR *pinname) override;
    /** Attach host interfaces and load the Lua model for this component. */
    void setup(IINSTANCE *instance, IDSIMCKT *dsim) override;
    /** Receive a change to the Proteus run or debugger mode. */
    void runctrl(RUNMODES mode) override;
    /** Receive an interactive actuator state change. */
    void actuate(REALTIME time, ACTIVESTATE newstate) override;
    /** Update interactive indicator state. */
    BOOL indicate(REALTIME time, ACTIVEDATA *newstate) override;
    /** Execute the Lua simulation hook for one host simulation tick. */
    void simulate(ABSTIME time, DSIMMODES mode) override;
    /** Deliver a scheduled host event to the Lua model. */
    void callback(ABSTIME time, EVENTID eventid) override;

    LRESULT vdmhlr(VDM_COMMAND *command, BYTE *data) override;
    void loaddata(INT format, INT segment, ADDRESS address, BYTE *data, INT numbytes) override;
    void disassemble(ADDRESS address, INT numbytes) override;
    BOOL getvardata(VARITEM *item, VARDATA *data) override;

    /** Return the Lua state owned by this model. The pointer is non-owning. */
    lua_State *getLuaContext() const
    {
        return luactx_;
    }

    /** Return the Proteus component instance supplied to setup(). */
    IINSTANCE *getInstance() const
    {
        return instance_;
    }

    /** Resolve a required digital pin by its component terminal name. */
    IDSIMPIN *getPin(CHAR *name) const
    {
        return instance_->getdsimpin(name, true);
    }

    /** Return the digital circuit interface supplied to setup(). */
    IDSIMCKT *getDSIM() const
    {
        return dsim_;
    }

    LuaEventDispatcher &getEventDispatcher() const
    {
        return *eventDispatcher_;
    }

  private:
    void registerPin(const model_pin &pin);
    bool registerBuses();

    std::vector<model_pin> devicePins_;
    lua_State *luactx_;
    IINSTANCE *instance_;
    IDSIMCKT *dsim_;
    std::string deviceID_;
    std::string deviceGUID_;
    bool simulationCallbackEnabled_ = true;
    bool modelReady_ = false;
    std::unique_ptr<LuaEventDispatcher> eventDispatcher_;
};

} // namespace DeviceSimulator
