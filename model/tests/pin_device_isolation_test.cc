#include "luabind/device/pin.hpp"

#include <cstring>
#include <iostream>

namespace
{
class FakeDsim final : public IDSIMCKT
{
  public:
    explicit FakeDsim(ABSTIME simulationTime) : time(simulationTime)
    {
    }

    ABSTIME time;

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

class FakePin final : public IDSIMPIN
{
  public:
    STATE state = SLO;
    ABSTIME lastTime = -1;
    RELTIME lastDelay = -1;

    BOOL invert() override
    {
        state = ishigh(state) ? SLO : SHI;
        return TRUE;
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
    EVENT *setstate(ABSTIME time, RELTIME delay, STATE newState) override
    {
        lastTime = time;
        lastDelay = delay;
        state = newState;
        return nullptr;
    }
    void setstate(STATE newState) override
    {
        state = newState;
    }
    void sethandler(IDSIMMODEL *, PINHANDLERFN) override
    {
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
} // namespace

int main()
{
    auto *luaA = luaL_newstate();
    auto *luaB = luaL_newstate();
    if (luaA == nullptr || luaB == nullptr)
    {
        if (luaA != nullptr)
        {
            lua_close(luaA);
        }
        if (luaB != nullptr)
        {
            lua_close(luaB);
        }
        return 1;
    }
    luaL_openlibs(luaA);
    luaL_openlibs(luaB);

    FakeDsim dsimA{101};
    FakeDsim dsimB{202};
    FakePin pinA;
    FakePin pinB;
    pinB.state = SHI;

    DeviceSimulator::registerPinLibrary(luaA, "IO", 1, &pinA, &dsimA);
    DeviceSimulator::registerPinLibrary(luaB, "IO", 1, &pinB, &dsimB);

    const bool firstDeviceOk = run(luaA, "assert(IO:get() == 0); IO:hi()") && pinA.state == TSTATE &&
                               pinA.lastTime == dsimA.time && pinB.state == SHI && pinB.lastTime == -1;
    const bool secondDeviceOk = run(luaB, "assert(IO:get() == 1); IO:lo()") && pinB.state == FSTATE &&
                                pinB.lastTime == dsimB.time && pinA.state == TSTATE;
    const bool interleavedOk = run(luaA, "IO:lo()") && run(luaB, "IO:hi()") && pinA.state == FSTATE &&
                               pinB.state == TSTATE && pinA.lastTime == dsimA.time && pinB.lastTime == dsimB.time;

    lua_close(luaA);
    lua_close(luaB);
    return firstDeviceOk && secondDeviceOk && interleavedOk ? 0 : 2;
}
