#include "pin_timing.hpp"

#include "model.hpp"

namespace DeviceSimulator
{
namespace
{
RELTIME readTiming(lua_State *lua, int tableIndex, const char *field)
{
    lua_getfield(lua, tableIndex, field);
    int isInteger = 0;
    const auto timing = lua_tointegerx(lua, -1, &isInteger);
    lua_pop(lua, 1);
    return isInteger == 0 ? 0 : normalizePinTiming(timing);
}
} // namespace

RELTIME normalizePinTiming(lua_Integer timing)
{
    return timing < 0 ? 0 : static_cast<RELTIME>(timing);
}

model_pin readPinDefinition(lua_State *lua, int tableIndex, int pinNumber)
{
    tableIndex = lua_absindex(lua, tableIndex);

    lua_getfield(lua, tableIndex, "name");
    const char *name = lua_tostring(lua, -1);
    model_pin pin{name == nullptr ? "" : name, pinNumber, 0, 0};
    lua_pop(lua, 1);

    pin.on_time = readTiming(lua, tableIndex, "on_time");
    pin.off_time = readTiming(lua, tableIndex, "off_time");
    return pin;
}

RELTIME transitionDelay(STATE state, const model_pin &pin)
{
    // TSTATE and FSTATE are Proteus logical-state tokens rather than ordinary
    // polarity values, so handle them before inspecting the polarity bits.
    if (state == TSTATE)
    {
        return pin.on_time;
    }
    if (state == FSTATE)
    {
        return pin.off_time;
    }
    if (ishigh(state))
    {
        return pin.on_time;
    }
    if (islow(state))
    {
        return pin.off_time;
    }
    return 0;
}
} // namespace DeviceSimulator
