#include "vsm_lua_api.hpp"

#include <initializer_list>
#include <limits>
#include <string>

namespace DeviceSimulator
{
namespace
{
constexpr lua_Integer picosecondsPerNanosecond = 1000;
constexpr lua_Integer picosecondsPerMicrosecond = 1000000;
constexpr lua_Integer picosecondsPerMillisecond = 1000000000;
constexpr lua_Integer picosecondsPerSecond = 1000000000000;

IINSTANCE *instance(lua_State *lua)
{
    return static_cast<IINSTANCE *>(lua_touserdata(lua, lua_upvalueindex(1)));
}

IDSIMCKT *dsim(lua_State *lua)
{
    return static_cast<IDSIMCKT *>(lua_touserdata(lua, lua_upvalueindex(2)));
}

IDSIMMODEL *model(lua_State *lua)
{
    return static_cast<IDSIMMODEL *>(lua_touserdata(lua, lua_upvalueindex(3)));
}

void registerHostFunction(lua_State *lua, const char *name, lua_CFunction function, IINSTANCE *hostInstance,
                          IDSIMCKT *hostDsim, IDSIMMODEL *hostModel)
{
    lua_pushlightuserdata(lua, hostInstance);
    lua_pushlightuserdata(lua, hostDsim);
    lua_pushlightuserdata(lua, hostModel);
    lua_pushcclosure(lua, function, 3);
    lua_setglobal(lua, name);
}

void setInteger(lua_State *lua, const char *name, lua_Integer value)
{
    lua_pushinteger(lua, value);
    lua_setglobal(lua, name);
}

const char *stateName(STATE state)
{
    switch (state)
    {
    case UNDEFINED:
        return "UNDEFINED";
    case TSTATE:
        return "TSTATE";
    case FSTATE:
        return "FSTATE";
    case PLO:
        return "PLO";
    case ILO:
        return "ILO";
    case SLO:
        return "SLO";
    case WLO:
        return "WLO";
    case FLT:
        return "FLT";
    case WHI:
        return "WHI";
    case SHI:
        return "SHI";
    case IHI:
        return "IHI";
    case PHI:
        return "PHI";
    case WUD:
        return "WUD";
    case SUD:
        return "SUD";
    default:
        return "wrong state";
    }
}

int stateToString(lua_State *lua)
{
    const auto value = luaL_checkinteger(lua, 1);
    if (value < (std::numeric_limits<INT>::min)() || value > (std::numeric_limits<INT>::max)())
    {
        lua_pushstring(lua, "wrong state");
        return 1;
    }
    const auto state = static_cast<STATE>(value);
    lua_pushstring(lua, stateName(state));
    return 1;
}

int info(lua_State *lua)
{
    instance(lua)->log(const_cast<CHAR *>("%s"), luaL_checkstring(lua, 1));
    return 0;
}

int message(lua_State *lua)
{
    instance(lua)->message(const_cast<CHAR *>("%s"), luaL_checkstring(lua, 1));
    return 0;
}

int warning(lua_State *lua)
{
    instance(lua)->warning(const_cast<CHAR *>("%s"), luaL_checkstring(lua, 1));
    return 0;
}

int hostError(lua_State *lua)
{
    instance(lua)->error(const_cast<CHAR *>("%s"), luaL_checkstring(lua, 1));
    return 0;
}

CHAR *parameterName(lua_State *lua)
{
    return const_cast<CHAR *>(luaL_checkstring(lua, 1));
}

int getStringParameter(lua_State *lua)
{
    lua_pushstring(lua, instance(lua)->getstrval(parameterName(lua)));
    return 1;
}

int getNumberParameter(lua_State *lua)
{
    DOUBLE value = 0;
    instance(lua)->getnumval(&value, parameterName(lua));
    lua_pushnumber(lua, value);
    return 1;
}

int getBooleanParameter(lua_State *lua)
{
    lua_pushboolean(lua, instance(lua)->getboolval(parameterName(lua)));
    return 1;
}

int getInitialParameter(lua_State *lua)
{
    lua_pushinteger(lua, instance(lua)->getinitval(parameterName(lua)));
    return 1;
}

int getHexParameter(lua_State *lua)
{
    lua_pushinteger(lua, instance(lua)->gethexval(parameterName(lua)));
    return 1;
}

int systemTime(lua_State *lua)
{
    ABSTIME time = 0;
    dsim(lua)->systime(&time);
    lua_pushinteger(lua, time);
    return 1;
}

int setCallback(lua_State *lua)
{
    const auto time = luaL_checkinteger(lua, 1);
    if (time < 0)
    {
        return luaL_argerror(lua, 1, "callback time cannot be negative");
    }

    const auto event = luaL_checkinteger(lua, 2);
    if (event < (std::numeric_limits<EVENTID>::min)() || event > (std::numeric_limits<EVENTID>::max)())
    {
        return luaL_argerror(lua, 2, "event ID is outside the SDK integer range");
    }

    dsim(lua)->setcallback(time, model(lua), static_cast<EVENTID>(event));
    return 0;
}

int unsupported(lua_State *lua)
{
    const char *name = lua_tostring(lua, lua_upvalueindex(1));
    const char *replacement = lua_tostring(lua, lua_upvalueindex(2));
    return luaL_error(lua, "%s is not supported by the v0.7 Lua API; %s", name, replacement);
}

void registerUnsupported(lua_State *lua, const char *name, const char *replacement)
{
    lua_pushstring(lua, name);
    lua_pushstring(lua, replacement);
    lua_pushcclosure(lua, unsupported, 2);
    lua_setglobal(lua, name);
}
} // namespace

void registerVsmLuaApi(lua_State *lua, IINSTANCE *hostInstance, IDSIMCKT *hostDsim, IDSIMMODEL *hostModel)
{
    const struct
    {
        const char *name;
        lua_Integer value;
    } constants[] = {{"UNDEFINED", UNDEFINED},
                     {"TSTATE", TSTATE},
                     {"FSTATE", FSTATE},
                     {"PLO", PLO},
                     {"ILO", ILO},
                     {"SLO", SLO},
                     {"WLO", WLO},
                     {"FLT", FLT},
                     {"WHI", WHI},
                     {"SHI", SHI},
                     {"IHI", IHI},
                     {"PHI", PHI},
                     {"WUD", WUD},
                     {"SUD", SUD},
                     {"NOW", 0},
                     {"NSEC", picosecondsPerNanosecond},
                     {"USEC", picosecondsPerMicrosecond},
                     {"MSEC", picosecondsPerMillisecond},
                     {"SEC", picosecondsPerSecond},
                     {"TTL", 0},
                     {"CMOS", 1},
                     {"I2L", 2}};
    for (const auto &constant : constants)
    {
        setInteger(lua, constant.name, constant.value);
    }

    registerHostFunction(lua, "state_to_string", stateToString, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "info", info, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "message", message, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "warning", warning, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "vsm_error", hostError, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "get_string_param", getStringParameter, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "get_num_param", getNumberParameter, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "get_bool_param", getBooleanParameter, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "get_init_param", getInitialParameter, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "get_hex_param", getHexParameter, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "systime", systemTime, hostInstance, hostDsim, hostModel);
    registerHostFunction(lua, "set_callback", setCallback, hostInstance, hostDsim, hostModel);

    const char *popupReplacement = "popup and source-debug helpers remain Proteus-host-only";
    for (const char *name : {"create_debug_popup", "create_memory_popup", "create_source_popup", "create_status_popup",
                             "create_var_popup", "delete_popup", "set_memory_popup", "repaint_memory_popup",
                             "print_to_debug_popup", "dump_to_debug_popup", "add_source_file"})
    {
        registerUnsupported(lua, name, popupReplacement);
    }
    registerUnsupported(lua, "set_bus", "declare device_buses and use the native bus object's methods");
    registerUnsupported(lua, "get_bus", "declare device_buses and use the native bus object's methods");
}
} // namespace DeviceSimulator
