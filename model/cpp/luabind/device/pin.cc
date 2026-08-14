#include <log/log.hpp>
#include "model.hpp"
#include "pin_timing.hpp"
#include "lua_event_dispatcher.hpp"
#include "lua.hpp"
#include "lua_stack_guard.hpp"
#include "luabind/device/pin.hpp"

namespace DeviceSimulator
{
model_pin getPinSelf(lua_State *L)
{
    if (!lua_istable(L, -lua_gettop(L)))
    {
        LOG_DEBUG("Error: not a table\n");
        lua_pop(L, 1); // Pop the invalid value from the stack
        return model_pin{};
    }

    lua_pushstring(L, "pinName");
    lua_gettable(L, -lua_gettop(L));
    std::string pinName = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_pushstring(L, "pinNumber");
    lua_gettable(L, -lua_gettop(L));
    int pinNumber = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "onTime");
    lua_gettable(L, -lua_gettop(L));
    const auto onTime = static_cast<RELTIME>(luaL_checkinteger(L, -1));
    lua_pop(L, 1);

    lua_pushstring(L, "offTime");
    lua_gettable(L, -lua_gettop(L));
    const auto offTime = static_cast<RELTIME>(luaL_checkinteger(L, -1));
    lua_pop(L, 1);

    model_pin pin;
    pin.name = pinName;
    pin.number = pinNumber;
    pin.on_time = onTime;
    pin.off_time = offTime;
    return pin;
}

static void setPinState(const model_pin &pin, STATE state)
{
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    ABSTIME time = 0;
    device->getDSIM()->systime(&time);
    pinInstance->setstate(time, transitionDelay(state, pin), state);
}

static int l_pin_set(lua_State *L)
{
    auto pin = getPinSelf(L);

    int level = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    setPinState(pin, level == 1 ? TSTATE : FSTATE);
    return 0;
}

static int l_pin_set_state(lua_State *L)
{
    auto pin = getPinSelf(L);

    STATE state = (STATE)luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    setPinState(pin, state);
    return 0;
}

static int l_pin_get(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));

    auto state = pinInstance->istate();
    lua_pushinteger(L, ishigh(state) ? 1 : 0);
    return 1;
}

static int l_pin_set_hi(lua_State *L)
{
    auto pin = getPinSelf(L);
    setPinState(pin, TSTATE);
    return 0;
}

static int l_pin_set_lo(lua_State *L)
{
    auto pin = getPinSelf(L);
    setPinState(pin, FSTATE);
    return 0;
}

static int l_pin_invert(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    const auto state = pinInstance->istate();
    if (ishigh(state))
    {
        setPinState(pin, FSTATE);
    }
    else if (islow(state))
    {
        setPinState(pin, TSTATE);
    }
    else
    {
        pinInstance->invert();
    }
    return 0;
}

static int l_pint_issteady(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushboolean(L, pinInstance->issteady());
    return 1;
}

static int l_pin_activity(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushinteger(L, pinInstance->activity());
    return 1;
}

static int l_pin_isactive(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushboolean(L, pinInstance->isactive());
    return 1;
}

static int l_pin_isinactive(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushboolean(L, pinInstance->isinactive());
    return 1;
}

static int l_pin_isposedge(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushboolean(L, pinInstance->isposedge());
    return 1;
}

static int l_pin_isnegedge(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushboolean(L, pinInstance->isnegedge());
    return 1;
}

static int l_pin_isedge(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    lua_pushboolean(L, pinInstance->isedge());
    return 1;
}

static int l_pin_islow(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushboolean(L, islow(state));
    return 1;
}

static int l_pin_ishigh(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushboolean(L, ishigh(state));
    return 1;
}

static int l_pin_isfloating(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushboolean(L, isfloating(state));
    return 1;
}

static int l_pin_iscontention(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushboolean(L, iscontention(state));
    return 1;
}

static int l_pin_isdefined(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushboolean(L, isdefined(state));
    return 1;
}

static int l_pin_ishighlow(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushboolean(L, ishighlow(state));
    return 1;
}

static int l_pin_polarity(lua_State *L)
{
    auto pin = getPinSelf(L);
    auto device = VirtualContextManager::getInstance().getDevice();
    auto pinInstance = device->getPin(const_cast<CHAR *>(pin.name.c_str()));
    auto state = pinInstance->istate();
    lua_pushinteger(L, polarity(state));
    return 1;
}

static const luaL_Reg VsmPinMethodsLib[] = {{"set", l_pin_set},
                                            {"setstate", l_pin_set_state},
                                            {"get", l_pin_get},
                                            {"hi", l_pin_set_hi},
                                            {"lo", l_pin_set_lo},
                                            {"invert", l_pin_invert},
                                            {"issteady", l_pint_issteady},
                                            {"activity", l_pin_activity},
                                            {"isactive", l_pin_isactive},
                                            {"isinactive", l_pin_isinactive},
                                            {"isposedge", l_pin_isposedge},
                                            {"isnegedge", l_pin_isnegedge},
                                            {"isedge", l_pin_isedge},
                                            {"islow", l_pin_islow},
                                            {"ishigh", l_pin_ishigh},
                                            {"isfloating", l_pin_isfloating},
                                            {"iscontention", l_pin_iscontention},
                                            {"isdefined", l_pin_isdefined},
                                            {"ishighlow", l_pin_ishighlow},
                                            {"polarity", l_pin_polarity},
                                            {NULL, NULL}};

namespace
{
void registerPinLibrary(lua_State *luaContext, const model_pin &pin, IDSIMPIN *nativePin = nullptr,
                        LuaEventDispatcher *dispatcher = nullptr)
{
    LuaScripting::LuaStackGuard stackGuard(luaContext);
    luaL_newlib(luaContext, VsmPinMethodsLib);

    if (nativePin != nullptr && dispatcher != nullptr)
    {
        attachPinCallbackApi(luaContext, -1, nativePin, dispatcher);
    }

    lua_pushinteger(luaContext, pin.number);
    lua_setfield(luaContext, -2, "pinNumber");

    lua_pushstring(luaContext, pin.name.c_str());
    lua_setfield(luaContext, -2, "pinName");

    lua_pushinteger(luaContext, pin.on_time);
    lua_setfield(luaContext, -2, "onTime");

    lua_pushinteger(luaContext, pin.off_time);
    lua_setfield(luaContext, -2, "offTime");

    lua_setglobal(luaContext, pin.name.c_str());
}
} // namespace

void registerPinLibrary(lua_State *luaContext, const char *name, int number)
{
    registerPinLibrary(luaContext, model_pin{name, number, 0, 0});
}

void VirtualDevice::registerPin(const model_pin &pin)
{
    LOG_DEBUG("Registering pin {}-{}\n", pin.name, pin.number);
    registerPinLibrary(luactx_, pin, getPin(const_cast<CHAR *>(pin.name.c_str())), eventDispatcher_.get());
}
} // namespace DeviceSimulator
