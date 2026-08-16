#include <log/log.hpp>

#include "lua_event_dispatcher.hpp"

#include <cstdint>
#include <limits>

namespace DeviceSimulator
{
namespace
{
constexpr const char *dispatcherField = "__event_dispatcher";
constexpr const char *nativePinField = "__callback_pin";
constexpr const char *nativeBusField = "__callback_bus";

template <typename Pointer> Pointer getPointer(lua_State *lua, const char *field)
{
    luaL_checktype(lua, 1, LUA_TTABLE);
    lua_getfield(lua, 1, field);
    auto pointer = static_cast<Pointer>(lua_touserdata(lua, -1));
    lua_pop(lua, 1);
    if (pointer == nullptr)
    {
        luaL_error(lua, "invalid callback object");
    }
    return pointer;
}

LuaEventDispatcher *getDispatcher(lua_State *lua)
{
    return getPointer<LuaEventDispatcher *>(lua, dispatcherField);
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

UINT getBusWidth(lua_State *lua)
{
    lua_getfield(lua, 1, "width");
    const auto value = luaL_checkinteger(lua, -1);
    lua_pop(lua, 1);
    if (value < 1 || value > MAXBUSBITS)
    {
        luaL_argerror(lua, 1, "invalid native bus width");
    }
    return static_cast<UINT>(value);
}

DWORD checkedBusValue(lua_State *lua, int argument)
{
    const auto value = luaL_checkinteger(lua, argument);
    const auto width = getBusWidth(lua);
    const std::uint64_t maximum =
        width == MAXBUSBITS ? (std::numeric_limits<DWORD>::max)() : (std::uint64_t{1} << width) - 1;
    if (value < 0 || static_cast<std::uint64_t>(value) > maximum)
    {
        luaL_argerror(lua, argument, "callback value does not fit the bus width");
    }
    return static_cast<DWORD>(value);
}

int pinOnChange(lua_State *lua)
{
    luaL_checktype(lua, 2, LUA_TFUNCTION);
    getDispatcher(lua)->addPinCallback(getPointer<IDSIMPIN *>(lua, nativePinField), 2);
    return 0;
}

int pinOnState(lua_State *lua)
{
    const auto state = checkedState(lua, 2);
    luaL_checktype(lua, 3, LUA_TFUNCTION);
    getDispatcher(lua)->addPinCallback(getPointer<IDSIMPIN *>(lua, nativePinField), 3, state);
    return 0;
}

int busOnChange(lua_State *lua)
{
    luaL_checktype(lua, 2, LUA_TFUNCTION);
    getDispatcher(lua)->addBusCallback(getPointer<IBUSPIN *>(lua, nativeBusField), 2);
    return 0;
}

int busOnValue(lua_State *lua)
{
    const auto value = checkedBusValue(lua, 2);
    luaL_checktype(lua, 3, LUA_TFUNCTION);
    getDispatcher(lua)->addBusCallback(getPointer<IBUSPIN *>(lua, nativeBusField), 3, value);
    return 0;
}

void setPointer(lua_State *lua, int tableIndex, const char *field, void *pointer)
{
    lua_pushlightuserdata(lua, pointer);
    lua_setfield(lua, tableIndex, field);
}
} // namespace

LuaEventDispatcher::LuaEventDispatcher(lua_State *lua, IDSIMMODEL *model) : lua_(lua), model_(model)
{
}

LuaEventDispatcher::~LuaEventDispatcher()
{
    for (auto &callback : pinCallbacks_)
    {
        disable(callback.functionReference);
    }
    for (auto &callback : busCallbacks_)
    {
        disable(callback.functionReference);
    }
}

void LuaEventDispatcher::addPinCallback(IDSIMPIN *pin, int functionIndex, std::optional<STATE> expectedState)
{
    functionIndex = lua_absindex(lua_, functionIndex);
    lua_pushvalue(lua_, functionIndex);
    const int reference = luaL_ref(lua_, LUA_REGISTRYINDEX);
    pinCallbacks_.push_back({pin, reference, expectedState, pin->istate()});
    pin->sethandler(model_, &IDSIMMODEL::simulate);
}

void LuaEventDispatcher::addBusCallback(IBUSPIN *bus, int functionIndex, std::optional<DWORD> expectedValue)
{
    functionIndex = lua_absindex(lua_, functionIndex);
    lua_pushvalue(lua_, functionIndex);
    const int reference = luaL_ref(lua_, LUA_REGISTRYINDEX);
    busCallbacks_.push_back({bus, reference, expectedValue, bus->getbusvalue()});
    bus->sethandler(model_, &IDSIMMODEL::simulate);
}

bool LuaEventDispatcher::invoke(int functionReference, ABSTIME time, DSIMMODES mode, lua_Integer value)
{
    const int stackTop = lua_gettop(lua_);
    lua_rawgeti(lua_, LUA_REGISTRYINDEX, functionReference);
    lua_pushinteger(lua_, time);
    lua_pushinteger(lua_, mode);
    lua_pushinteger(lua_, value);
    if (lua_pcall(lua_, 3, 0, 0) != LUA_OK)
    {
        const char *message = lua_tostring(lua_, -1);
        LOG_DEBUG("Lua object callback failed with \"{}\"\n", message == nullptr ? "unknown Lua error" : message);
        lua_settop(lua_, stackTop);
        return false;
    }
    lua_settop(lua_, stackTop);
    return true;
}

void LuaEventDispatcher::disable(int &functionReference)
{
    if (functionReference != LUA_NOREF)
    {
        luaL_unref(lua_, LUA_REGISTRYINDEX, functionReference);
        functionReference = LUA_NOREF;
    }
}

void LuaEventDispatcher::dispatch(ABSTIME time, DSIMMODES mode)
{
    const auto pinCount = pinCallbacks_.size();
    for (std::size_t index = 0; index < pinCount; ++index)
    {
        auto &callback = pinCallbacks_[index];
        if (callback.functionReference == LUA_NOREF)
        {
            continue;
        }
        const auto state = callback.pin->istate();
        if (state == callback.lastState)
        {
            continue;
        }
        callback.lastState = state;
        if (callback.expectedState.has_value() && callback.expectedState.value() != state)
        {
            continue;
        }
        const int reference = callback.functionReference;
        if (!invoke(reference, time, mode, state))
        {
            disable(pinCallbacks_[index].functionReference);
        }
    }

    const auto busCount = busCallbacks_.size();
    for (std::size_t index = 0; index < busCount; ++index)
    {
        auto &callback = busCallbacks_[index];
        if (callback.functionReference == LUA_NOREF)
        {
            continue;
        }
        const auto value = callback.bus->getbusvalue();
        if (value == callback.lastValue)
        {
            continue;
        }
        callback.lastValue = value;
        if (callback.expectedValue.has_value() && callback.expectedValue.value() != value)
        {
            continue;
        }
        const int reference = callback.functionReference;
        if (!invoke(reference, time, mode, value))
        {
            disable(busCallbacks_[index].functionReference);
        }
    }
}

void attachPinCallbackApi(lua_State *lua, int tableIndex, IDSIMPIN *pin, LuaEventDispatcher *dispatcher)
{
    tableIndex = lua_absindex(lua, tableIndex);
    setPointer(lua, tableIndex, nativePinField, pin);
    setPointer(lua, tableIndex, dispatcherField, dispatcher);
    lua_pushcfunction(lua, pinOnChange);
    lua_setfield(lua, tableIndex, "onchange");
    lua_pushcfunction(lua, pinOnState);
    lua_setfield(lua, tableIndex, "onstate");
}

void attachBusCallbackApi(lua_State *lua, int tableIndex, IBUSPIN *bus, LuaEventDispatcher *dispatcher)
{
    tableIndex = lua_absindex(lua, tableIndex);
    setPointer(lua, tableIndex, nativeBusField, bus);
    setPointer(lua, tableIndex, dispatcherField, dispatcher);
    lua_pushcfunction(lua, busOnChange);
    lua_setfield(lua, tableIndex, "onchange");
    lua_pushcfunction(lua, busOnValue);
    lua_setfield(lua, tableIndex, "onvalue");
}
} // namespace DeviceSimulator
