#pragma once

#include <optional>
#include <vector>

#include <lua.hpp>
#include <vsm.hpp>

namespace DeviceSimulator
{
class LuaEventDispatcher
{
  public:
    LuaEventDispatcher(lua_State *lua, IDSIMMODEL *model);
    ~LuaEventDispatcher();

    LuaEventDispatcher(const LuaEventDispatcher &) = delete;
    LuaEventDispatcher &operator=(const LuaEventDispatcher &) = delete;

    void addPinCallback(IDSIMPIN *pin, int functionIndex, std::optional<STATE> expectedState = std::nullopt);
    void addBusCallback(IBUSPIN *bus, int functionIndex, std::optional<DWORD> expectedValue = std::nullopt);
    void dispatch(ABSTIME time, DSIMMODES mode);

  private:
    struct PinCallback
    {
        IDSIMPIN *pin;
        int functionReference;
        std::optional<STATE> expectedState;
        STATE lastState;
    };

    struct BusCallback
    {
        IBUSPIN *bus;
        int functionReference;
        std::optional<DWORD> expectedValue;
        DWORD lastValue;
    };

    bool invoke(int functionReference, ABSTIME time, DSIMMODES mode, lua_Integer value);
    void disable(int &functionReference);

    lua_State *lua_;
    IDSIMMODEL *model_;
    std::vector<PinCallback> pinCallbacks_;
    std::vector<BusCallback> busCallbacks_;
};

void attachPinCallbackApi(lua_State *lua, int tableIndex, IDSIMPIN *pin, LuaEventDispatcher *dispatcher);
void attachBusCallbackApi(lua_State *lua, int tableIndex, IBUSPIN *bus, LuaEventDispatcher *dispatcher);
} // namespace DeviceSimulator
