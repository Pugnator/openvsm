#include "luabind/device/bus.hpp"

#include <array>
#include <cstring>
#include <iostream>
#include <string>

namespace
{
class FakeDsim final : public IDSIMCKT
{
  public:
    ABSTIME time = 0x123456789LL;

    void sysvar(DOUBLE *result, DSIMVARS variable) override
    {
        if (variable == DSIMTIMENOW)
        {
            std::memcpy(result, &time, sizeof(time));
        }
    }

    EVENT *setcallback(ABSTIME, IDSIMMODEL *, EVENTID) override
    {
        return nullptr;
    }

    BOOL cancelcallback(EVENT *, IDSIMMODEL *) override
    {
        return FALSE;
    }

    void setbreak(ABSTIME) override
    {
    }

    void suspend(IINSTANCE *, CHAR *) override
    {
    }

    EVENT *setcallbackex(ABSTIME, IDSIMMODEL *, CALLBACKHANDLERFN, EVENTID) override
    {
        return nullptr;
    }

    DSIMNODE newnode(CHAR *, CHAR *) override
    {
        return nullptr;
    }

    IDSIMPIN *newpin(IINSTANCE *, DSIMNODE, CHAR *, DWORD) override
    {
        return nullptr;
    }

    EVENT *setclockcallback(ABSTIME, RELTIME, IDSIMMODEL *, CALLBACKHANDLERFN, EVENTID) override
    {
        return nullptr;
    }
};

class FakeBus final : public IBUSPIN
{
  public:
    RELTIME onTime = 0;
    RELTIME offTime = 0;
    RELTIME tristateTime = 0;
    STATE onState = UNDEFINED;
    STATE offState = UNDEFINED;
    STATE tristateState = UNDEFINED;
    ABSTIME lastTime = 0;
    DWORD value = 0;
    DWORD drive = 0;
    bool tristated = false;
    UINT lastBit = 0;
    STATE lastBitState = UNDEFINED;
    std::array<STATE, MAXBUSBITS> bitStates{};

    void settiming(RELTIME on, RELTIME off, RELTIME tristate) override
    {
        onTime = on;
        offTime = off;
        tristateTime = tristate;
    }

    void setstates(STATE on, STATE off, STATE tristate) override
    {
        onState = on;
        offState = off;
        tristateState = tristate;
    }

    void sethandler(IDSIMMODEL *, PINHANDLERFN) override
    {
    }

    void drivebusvalue(ABSTIME time, DWORD newValue) override
    {
        lastTime = time;
        value = newValue;
        drive = newValue;
        tristated = false;
    }

    void drivetristate(ABSTIME time) override
    {
        lastTime = time;
        tristated = true;
    }

    void drivebitstate(ABSTIME time, UINT bit, STATE state) override
    {
        lastTime = time;
        lastBit = bit;
        lastBitState = state;
        bitStates[bit] = state;
    }

    DWORD getbusvalue() override
    {
        return value;
    }

    DWORD getbusdrive() override
    {
        return drive;
    }

    STATE getbitstate(UINT bit) override
    {
        return bitStates[bit];
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

bool readDefinition(lua_State *lua, const char *script, DeviceSimulator::BusDefinition &definition, std::string &error)
{
    if (!run(lua, script))
    {
        return false;
    }
    const bool valid = DeviceSimulator::readBusDefinition(lua, -1, definition, error);
    lua_pop(lua, 1);
    return valid;
}
} // namespace

int main()
{
    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    DeviceSimulator::BusDefinition definition;
    std::string error;
    if (!readDefinition(lua,
                        "return {name='D', base=4, width=4, on_time=11, off_time=22, tristate_time=33, "
                        "on_state=7, off_state=8, tristate_state=9, required=false}",
                        definition, error))
    {
        std::cerr << error << '\n';
        lua_close(lua);
        return 1;
    }
    if (definition.name != "D" || definition.base != 4 || definition.width != 4 || definition.required != FALSE)
    {
        std::cerr << "Bus identity fields were not parsed correctly\n";
        lua_close(lua);
        return 1;
    }

    FakeDsim dsim;
    FakeBus bus;
    DeviceSimulator::registerBusObject(lua, definition, &bus, &dsim);
    if (bus.onTime != 11 || bus.offTime != 22 || bus.tristateTime != 33 || bus.onState != 7 || bus.offState != 8 ||
        bus.tristateState != 9)
    {
        std::cerr << "Bus timing or state configuration was not applied\n";
        lua_close(lua);
        return 1;
    }

    const char *exerciseBus = R"(
        assert(D.base == 4 and D.width == 4)
        D:drive(10)
        assert(D:get() == 10 and D:getdrive() == 10)
        D:drivebit(2, 15)
        assert(D:getbitstate(2) == 15)
        D:tristate()
        D:settiming(41, 42, 43)
        D:setstates(11, 12, 13)
        assert(not pcall(function() D:drive(16) end))
        assert(not pcall(function() D:drive(-1) end))
        assert(not pcall(function() D:drivebit(4, 1) end))
        assert(not pcall(function() D:drivebit(-1, 1) end))
        assert(not pcall(function() D:drivebit(0, 0x100000000) end))
        assert(not pcall(function() D:settiming(-1, 0, 0) end))
        assert(not pcall(function() D:setstates(0x100000000, 0, 0) end))
        assert(not pcall(function() D.width = 0; D:drive(0) end))
        D.width = 4
    )";
    if (!run(lua, exerciseBus) || bus.value != 10 || bus.lastBit != 2 || bus.lastBitState != 15 || !bus.tristated ||
        bus.lastTime != dsim.time || bus.onTime != 41 || bus.offTime != 42 || bus.tristateTime != 43 ||
        bus.onState != 11 || bus.offState != 12 || bus.tristateState != 13)
    {
        std::cerr << "Native bus operations were not forwarded correctly\n";
        lua_close(lua);
        return 1;
    }

    DeviceSimulator::BusDefinition invalid;
    if (readDefinition(lua, "return {name='bad', width=0}", invalid, error) ||
        readDefinition(lua, "return {name='bad', width=33}", invalid, error))
    {
        std::cerr << "An invalid bus width was accepted\n";
        lua_close(lua);
        return 1;
    }

    DeviceSimulator::BusDefinition wide;
    if (!readDefinition(lua, "return {name='W', width=32}", wide, error))
    {
        std::cerr << error << '\n';
        lua_close(lua);
        return 1;
    }
    FakeBus wideBus;
    DeviceSimulator::registerBusObject(lua, wide, &wideBus, &dsim);
    if (!run(lua, "W:drive(0xffffffff); assert(not pcall(function() W:drive(0x100000000) end))") ||
        wideBus.value != 0xffffffffu)
    {
        std::cerr << "The 32-bit bus value range was handled incorrectly\n";
        lua_close(lua);
        return 1;
    }

    lua_close(lua);
    return 0;
}
