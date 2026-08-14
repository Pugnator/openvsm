#include <log/log.hpp>
#include <cstdint>
#include <limits>
#include "model.hpp"
#include "bus.hpp"
#include "lua.hpp"

namespace DeviceSimulator
{
namespace
{
constexpr const char *nativeBusField = "__native_bus";
constexpr const char *nativeDsimField = "__native_dsim";
constexpr const char *widthField = "width";

bool readIntegerField(lua_State *lua, int tableIndex, const char *field, lua_Integer defaultValue, lua_Integer minimum,
                      lua_Integer maximum, lua_Integer &value, std::string &error)
{
    lua_getfield(lua, tableIndex, field);
    if (lua_isnil(lua, -1))
    {
        value = defaultValue;
        lua_pop(lua, 1);
        return true;
    }

    int isInteger = 0;
    value = lua_tointegerx(lua, -1, &isInteger);
    lua_pop(lua, 1);
    if (isInteger == 0 || value < minimum || value > maximum)
    {
        error = "bus field '" + std::string{field} + "' must be an integer from " + std::to_string(minimum) + " to " +
                std::to_string(maximum);
        return false;
    }
    return true;
}

template <typename Pointer> Pointer readPointer(lua_State *lua, const char *field)
{
    luaL_checktype(lua, 1, LUA_TTABLE);
    lua_getfield(lua, 1, field);
    auto pointer = static_cast<Pointer>(lua_touserdata(lua, -1));
    lua_pop(lua, 1);
    if (pointer == nullptr)
    {
        luaL_error(lua, "invalid native bus object");
    }
    return pointer;
}

UINT readWidth(lua_State *lua)
{
    lua_getfield(lua, 1, widthField);
    const auto value = luaL_checkinteger(lua, -1);
    lua_pop(lua, 1);
    if (value < 1 || value > MAXBUSBITS)
    {
        luaL_argerror(lua, 1, "invalid native bus width");
    }
    return static_cast<UINT>(value);
}

ABSTIME currentTime(lua_State *lua)
{
    auto *dsim = readPointer<IDSIMCKT *>(lua, nativeDsimField);
    ABSTIME time = 0;
    dsim->systime(&time);
    return time;
}

lua_Integer checkedNonNegative(lua_State *lua, int argument, const char *description)
{
    const auto value = luaL_checkinteger(lua, argument);
    if (value < 0)
    {
        luaL_argerror(lua, argument, description);
    }
    return value;
}

STATE checkedState(lua_State *lua, int argument)
{
    const auto value = luaL_checkinteger(lua, argument);
    if (value < (std::numeric_limits<INT>::min)() || value > (std::numeric_limits<INT>::max)())
    {
        luaL_argerror(lua, argument, "state is outside the SDK integer range");
    }
    return static_cast<STATE>(value);
}

int driveBus(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    const auto width = readWidth(lua);
    const auto value = checkedNonNegative(lua, 2, "bus value cannot be negative");
    const std::uint64_t maximum =
        width == MAXBUSBITS ? (std::numeric_limits<DWORD>::max)() : (std::uint64_t{1} << width) - 1;
    if (static_cast<std::uint64_t>(value) > maximum)
    {
        return luaL_argerror(lua, 2, "value does not fit the bus width");
    }

    bus->drivebusvalue(currentTime(lua), static_cast<DWORD>(value));
    return 0;
}

int driveTristate(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    bus->drivetristate(currentTime(lua));
    return 0;
}

UINT checkedBit(lua_State *lua, int argument)
{
    const auto bit = checkedNonNegative(lua, argument, "bit index cannot be negative");
    if (static_cast<std::uint64_t>(bit) >= readWidth(lua))
    {
        luaL_argerror(lua, argument, "bit index is outside the bus width");
    }
    return static_cast<UINT>(bit);
}

int driveBitState(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    const auto bit = checkedBit(lua, 2);
    const auto state = checkedState(lua, 3);
    bus->drivebitstate(currentTime(lua), bit, state);
    return 0;
}

int getBusValue(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    lua_pushinteger(lua, bus->getbusvalue());
    return 1;
}

int getBusDrive(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    lua_pushinteger(lua, bus->getbusdrive());
    return 1;
}

int getBitState(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    lua_pushinteger(lua, bus->getbitstate(checkedBit(lua, 2)));
    return 1;
}

int setTiming(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    const auto onTime = checkedNonNegative(lua, 2, "on timing cannot be negative");
    const auto offTime = checkedNonNegative(lua, 3, "off timing cannot be negative");
    const auto tristateTime = checkedNonNegative(lua, 4, "tristate timing cannot be negative");
    bus->settiming(onTime, offTime, tristateTime);
    return 0;
}

int setStates(lua_State *lua)
{
    auto *bus = readPointer<IBUSPIN *>(lua, nativeBusField);
    const auto onState = checkedState(lua, 2);
    const auto offState = checkedState(lua, 3);
    const auto tristateState = checkedState(lua, 4);
    bus->setstates(onState, offState, tristateState);
    return 0;
}

const luaL_Reg busMethods[] = {
    {"set", driveBus},        {"drive", driveBus},       {"tristate", driveTristate},  {"drivebit", driveBitState},
    {"get", getBusValue},     {"getdrive", getBusDrive}, {"getbitstate", getBitState}, {"settiming", setTiming},
    {"setstates", setStates}, {nullptr, nullptr}};
} // namespace

bool readBusDefinition(lua_State *lua, int tableIndex, BusDefinition &definition, std::string &error)
{
    tableIndex = lua_absindex(lua, tableIndex);
    if (!lua_istable(lua, tableIndex))
    {
        error = "bus definition must be a table";
        return false;
    }

    lua_getfield(lua, tableIndex, "name");
    const bool nameIsString = lua_type(lua, -1) == LUA_TSTRING;
    const char *name = lua_tostring(lua, -1);
    if (!nameIsString || name == nullptr || *name == '\0')
    {
        lua_pop(lua, 1);
        error = "bus field 'name' must be a non-empty string";
        return false;
    }
    definition.name = name;
    lua_pop(lua, 1);

    lua_Integer value = 0;
    if (!readIntegerField(lua, tableIndex, "base", 0, 0, (std::numeric_limits<UINT>::max)(), value, error))
    {
        return false;
    }
    definition.base = static_cast<UINT>(value);

    if (!readIntegerField(lua, tableIndex, "width", 0, 1, MAXBUSBITS, value, error))
    {
        return false;
    }
    definition.width = static_cast<UINT>(value);

    const auto maximumTime = (std::numeric_limits<RELTIME>::max)();
    if (!readIntegerField(lua, tableIndex, "on_time", 0, 0, maximumTime, value, error))
    {
        return false;
    }
    definition.on_time = value;
    if (!readIntegerField(lua, tableIndex, "off_time", 0, 0, maximumTime, value, error))
    {
        return false;
    }
    definition.off_time = value;
    if (!readIntegerField(lua, tableIndex, "tristate_time", 0, 0, maximumTime, value, error))
    {
        return false;
    }
    definition.tristate_time = value;

    if (!readIntegerField(lua, tableIndex, "on_state", TSTATE, (std::numeric_limits<INT>::min)(),
                          (std::numeric_limits<INT>::max)(), value, error))
    {
        return false;
    }
    definition.on_state = static_cast<STATE>(value);
    if (!readIntegerField(lua, tableIndex, "off_state", FSTATE, (std::numeric_limits<INT>::min)(),
                          (std::numeric_limits<INT>::max)(), value, error))
    {
        return false;
    }
    definition.off_state = static_cast<STATE>(value);
    if (!readIntegerField(lua, tableIndex, "tristate_state", FLT, (std::numeric_limits<INT>::min)(),
                          (std::numeric_limits<INT>::max)(), value, error))
    {
        return false;
    }
    definition.tristate_state = static_cast<STATE>(value);

    lua_getfield(lua, tableIndex, "required");
    if (!lua_isnil(lua, -1) && !lua_isboolean(lua, -1))
    {
        lua_pop(lua, 1);
        error = "bus field 'required' must be a boolean";
        return false;
    }
    definition.required = lua_isnil(lua, -1) ? TRUE : lua_toboolean(lua, -1);
    lua_pop(lua, 1);
    return true;
}

void registerBusObject(lua_State *lua, const BusDefinition &definition, IBUSPIN *bus, IDSIMCKT *dsim)
{
    bus->settiming(definition.on_time, definition.off_time, definition.tristate_time);
    bus->setstates(definition.on_state, definition.off_state, definition.tristate_state);

    luaL_newlib(lua, busMethods);
    lua_pushlightuserdata(lua, bus);
    lua_setfield(lua, -2, nativeBusField);
    lua_pushlightuserdata(lua, dsim);
    lua_setfield(lua, -2, nativeDsimField);
    lua_pushinteger(lua, definition.base);
    lua_setfield(lua, -2, "base");
    lua_pushinteger(lua, definition.width);
    lua_setfield(lua, -2, widthField);
    lua_setglobal(lua, definition.name.c_str());
}

bool VirtualDevice::registerBuses()
{
    const int stackTop = lua_gettop(luactx_);
    lua_getglobal(luactx_, "device_buses");
    if (lua_isnil(luactx_, -1))
    {
        lua_settop(luactx_, stackTop);
        return true;
    }
    if (!lua_istable(luactx_, -1))
    {
        LOG_DEBUG("device_buses must be a table\n");
        lua_settop(luactx_, stackTop);
        return false;
    }

    const int busCount = luaL_len(luactx_, -1);
    for (int index = 1; index <= busCount; ++index)
    {
        lua_rawgeti(luactx_, -1, index);
        BusDefinition definition;
        std::string error;
        if (!readBusDefinition(luactx_, -1, definition, error))
        {
            LOG_DEBUG("Invalid bus definition {}: {}\n", index, error);
            lua_settop(luactx_, stackTop);
            return false;
        }

        auto *bus =
            instance_->getbuspin(definition.name.data(), definition.base, definition.width, definition.required);
        if (bus == nullptr)
        {
            LOG_DEBUG("Proteus did not provide bus {} (base {}, width {})\n", definition.name, definition.base,
                      definition.width);
            lua_settop(luactx_, stackTop);
            return false;
        }

        registerBusObject(luactx_, definition, bus, dsim_);
        lua_pop(luactx_, 1);
    }

    lua_settop(luactx_, stackTop);
    return true;
}
} // namespace DeviceSimulator
