#include "luabind/device/lua_event_dispatcher.hpp"

#include <iostream>

namespace
{
class LuaState
{
  public:
    LuaState() : lua_(luaL_newstate())
    {
    }

    ~LuaState()
    {
        if (lua_ != nullptr)
        {
            lua_close(lua_);
        }
    }

    lua_State *get() const
    {
        return lua_;
    }

  private:
    lua_State *lua_;
};

class FakeModel final : public IDSIMMODEL
{
  public:
    INT isdigital(CHAR *) override
    {
        return 1;
    }

    void setup(IINSTANCE *, IDSIMCKT *) override
    {
    }

    void runctrl(RUNMODES) override
    {
    }

    void actuate(REALTIME, ACTIVESTATE) override
    {
    }

    BOOL indicate(REALTIME, ACTIVEDATA *) override
    {
        return TRUE;
    }

    void simulate(ABSTIME, DSIMMODES) override
    {
    }

    void callback(ABSTIME, EVENTID) override
    {
    }
};

class FakePin final : public IDSIMPIN
{
  public:
    STATE state = SLO;
    IDSIMMODEL *handlerModel = nullptr;
    PINHANDLERFN handler = nullptr;

    BOOL invert() override
    {
        return FALSE;
    }

    STATE istate() override
    {
        return state;
    }

    BOOL issteady() override
    {
        return TRUE;
    }

    INT activity() override
    {
        return 0;
    }

    BOOL isactive() override
    {
        return FALSE;
    }

    BOOL isinactive() override
    {
        return TRUE;
    }

    BOOL isposedge() override
    {
        return FALSE;
    }

    BOOL isnegedge() override
    {
        return FALSE;
    }

    BOOL isedge() override
    {
        return FALSE;
    }

    EVENT *setstate(ABSTIME, RELTIME, RELTIME, RELTIME, STATE) override
    {
        return nullptr;
    }

    EVENT *setstate(ABSTIME, RELTIME, STATE) override
    {
        return nullptr;
    }

    void setstate(STATE newState) override
    {
        state = newState;
    }

    void sethandler(IDSIMMODEL *model, PINHANDLERFN function) override
    {
        handlerModel = model;
        handler = function;
    }

    DSIMNODE getnode() override
    {
        return nullptr;
    }

    STATE getstate() override
    {
        return state;
    }

    void settiming(RELTIME, RELTIME, RELTIME) override
    {
    }

    void setstates(STATE, STATE, STATE) override
    {
    }

    EVENT *drivebool(ABSTIME, BOOL) override
    {
        return nullptr;
    }

    EVENT *drivestate(ABSTIME, STATE) override
    {
        return nullptr;
    }

    EVENT *drivetristate(ABSTIME) override
    {
        return nullptr;
    }
};

class FakeBus final : public IBUSPIN
{
  public:
    DWORD value = 0;
    IDSIMMODEL *handlerModel = nullptr;
    PINHANDLERFN handler = nullptr;

    void settiming(RELTIME, RELTIME, RELTIME) override
    {
    }

    void setstates(STATE, STATE, STATE) override
    {
    }

    void sethandler(IDSIMMODEL *model, PINHANDLERFN function) override
    {
        handlerModel = model;
        handler = function;
    }

    void drivebusvalue(ABSTIME, DWORD newValue) override
    {
        value = newValue;
    }

    void drivetristate(ABSTIME) override
    {
    }

    void drivebitstate(ABSTIME, UINT, STATE) override
    {
    }

    DWORD getbusvalue() override
    {
        return value;
    }

    DWORD getbusdrive() override
    {
        return value;
    }

    STATE getbitstate(UINT) override
    {
        return UNDEFINED;
    }
};

bool run(lua_State *lua, const char *script)
{
    if (luaL_dostring(lua, script) == LUA_OK)
    {
        return true;
    }
    std::cerr << lua_tostring(lua, -1) << '\n';
    lua_pop(lua, 1);
    return false;
}

bool checkCounters(lua_State *lua, int pinChanges, int pinMatches, int busChanges, int busMatches, int failures,
                   int lateCalls)
{
    lua_getglobal(lua, "check_counters");
    lua_pushinteger(lua, pinChanges);
    lua_pushinteger(lua, pinMatches);
    lua_pushinteger(lua, busChanges);
    lua_pushinteger(lua, busMatches);
    lua_pushinteger(lua, failures);
    lua_pushinteger(lua, lateCalls);
    if (lua_pcall(lua, 6, 0, 0) == LUA_OK)
    {
        return true;
    }
    std::cerr << lua_tostring(lua, -1) << '\n';
    lua_pop(lua, 1);
    return false;
}
} // namespace

int main()
{
    LuaState luaState;
    auto *lua = luaState.get();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    FakeModel model;
    FakePin pin;
    FakeBus bus;
    {
        DeviceSimulator::LuaEventDispatcher dispatcher{lua, &model};

        lua_newtable(lua);
        DeviceSimulator::attachPinCallbackApi(lua, -1, &pin, &dispatcher);
        lua_setglobal(lua, "P");

        lua_newtable(lua);
        lua_pushinteger(lua, 4);
        lua_setfield(lua, -2, "width");
        DeviceSimulator::attachBusCallbackApi(lua, -1, &bus, &dispatcher);
        lua_setglobal(lua, "B");

        const char *registrations = R"(
            pin_changes, pin_matches = 0, 0
            bus_changes, bus_matches = 0, 0
            failures, late_calls = 0, 0
            late_registered = false

            P:onchange(function(time, mode, state)
                assert(type(time) == "number" and type(mode) == "number" and type(state) == "number")
                pin_changes = pin_changes + 1
                if not late_registered then
                    late_registered = true
                    P:onchange(function() late_calls = late_calls + 1 end)
                end
            end)
            P:onstate(15, function(_, _, state)
                assert(state == 15)
                pin_matches = pin_matches + 1
            end)
            P:onchange(function()
                failures = failures + 1
                error("expected callback failure")
            end)

            B:onchange(function(_, _, value)
                assert(type(value) == "number")
                bus_changes = bus_changes + 1
            end)
            B:onvalue(5, function(_, _, value)
                assert(value == 5)
                bus_matches = bus_matches + 1
            end)

            assert(not pcall(function() P:onstate(0x100000000, function() end) end))
            assert(not pcall(function() B:onvalue(16, function() end) end))

            function check_counters(pc, pm, bc, bm, f, late)
                assert(pin_changes == pc and pin_matches == pm)
                assert(bus_changes == bc and bus_matches == bm)
                assert(failures == f and late_calls == late)
            end
        )";
        if (!run(lua, registrations) || pin.handlerModel != &model || pin.handler != &IDSIMMODEL::simulate ||
            bus.handlerModel != &model || bus.handler != &IDSIMMODEL::simulate)
        {
            std::cerr << "Object callbacks did not register native handlers\n";
            return 1;
        }

        const int stackTop = lua_gettop(lua);
        pin.state = SHI;
        bus.value = 5;
        dispatcher.dispatch(100, DSIMNORMAL);
        if (lua_gettop(lua) != stackTop || !checkCounters(lua, 1, 1, 1, 1, 1, 0))
        {
            return 1;
        }

        dispatcher.dispatch(101, DSIMNORMAL);
        if (!checkCounters(lua, 1, 1, 1, 1, 1, 0))
        {
            return 1;
        }

        pin.state = SLO;
        bus.value = 6;
        dispatcher.dispatch(102, DSIMSETTLE);
        if (!checkCounters(lua, 2, 1, 2, 1, 1, 1))
        {
            return 1;
        }

        pin.state = SHI;
        bus.value = 5;
        dispatcher.dispatch(103, DSIMNORMAL);
        if (!checkCounters(lua, 3, 2, 3, 2, 1, 2))
        {
            return 1;
        }
    }

    return 0;
}
