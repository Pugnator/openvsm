#include "model.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <utility>

namespace
{
class FakeInstance final : public IINSTANCE
{
  public:
    FakeInstance(std::string instanceId, std::string modelScript)
        : instanceId_(std::move(instanceId)), modelScript_(std::move(modelScript))
    {
    }

    CHAR *id() override
    {
        return &instanceId_[0];
    }
    CHAR *value() override
    {
        return nullptr;
    }
    CHAR *getstrval(CHAR *name, CHAR *defaultValue = nullptr) override
    {
        return std::strcmp(name, "LUA") == 0 ? &modelScript_[0] : defaultValue;
    }
    void getnumval(DOUBLE *result, CHAR *, DOUBLE defaultValue = 0) override
    {
        *result = defaultValue;
    }
    BOOL getboolval(CHAR *, BOOL defaultValue = FALSE) override
    {
        return defaultValue;
    }
    DWORD gethexval(CHAR *, DWORD defaultValue = 0) override
    {
        return defaultValue;
    }
    LONG getinitval(CHAR *, LONG defaultValue = 0) override
    {
        return defaultValue;
    }
    RELTIME getdelay(CHAR *, RELTIME defaultValue = 0) override
    {
        return defaultValue;
    }
    IACTIVEMODEL *getactivemodel() override
    {
        return nullptr;
    }
    IINSTANCE *getinterfacemodel() override
    {
        return nullptr;
    }
    BOOL getmoddata(BYTE **data, DWORD *size) override
    {
        *data = nullptr;
        *size = 0;
        return FALSE;
    }
    SPICENODE getspicenode(CHAR *, BOOL) override
    {
        return 0;
    }
    IDSIMPIN *getdsimpin(CHAR *, BOOL) override
    {
        return nullptr;
    }
    void log(CHAR *, ...) override
    {
    }
    void warning(CHAR *, ...) override
    {
    }
    void error(CHAR *, ...) override
    {
    }
    void fatal(CHAR *, ...) override
    {
    }
    BOOL message(CHAR *, ...) override
    {
        return TRUE;
    }
    IPOPUP *createpopup(CREATEPOPUPSTRUCT *) override
    {
        return nullptr;
    }
    void deletepopup(POPUPID) override
    {
    }
    BOOL setvdmhlr(ICPU *) override
    {
        return TRUE;
    }
    BOOL loadmemory(CHAR *, VOID *, UINT, UINT = 0, UINT = 0) override
    {
        return FALSE;
    }
    IBUSPIN *getbuspin(CHAR *, UINT, UINT, BOOL) override
    {
        return nullptr;
    }
    IBUSPIN *getbuspin(CHAR *, IDSIMPIN **, UINT) override
    {
        return nullptr;
    }

  private:
    std::string instanceId_;
    std::string modelScript_;
};

class FakeDsim final : public IDSIMCKT
{
  public:
    void sysvar(DOUBLE *result, DSIMVARS variable) override
    {
        if (variable == DSIMTIMENOW)
        {
            ABSTIME time = 0;
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

bool runLua(lua_State *luaContext, const char *script)
{
    if (luaL_dostring(luaContext, script) == LUA_OK)
    {
        return true;
    }
    std::cerr << lua_tostring(luaContext, -1) << '\n';
    lua_pop(luaContext, 1);
    return false;
}

bool stackIsStable(DeviceSimulator::VirtualDevice &device, int ticks)
{
    lua_State *luaContext = device.getLuaContext();
    const int originalTop = lua_gettop(luaContext);
    for (int tick = 0; tick < ticks; ++tick)
    {
        device.simulate(0, DSIMNORMAL);
        if (lua_gettop(luaContext) != originalTop)
        {
            return false;
        }
    }
    return true;
}

void setupDevice(DeviceSimulator::VirtualDevice &device, FakeInstance &instance, FakeDsim &dsim)
{
    device.setup(&instance, &dsim);
}
} // namespace

int main(int argumentCount, char **arguments)
{
    if (argumentCount != 2)
    {
        std::cerr << "Expected the empty device fixture path\n";
        return 1;
    }
    const std::string scriptPath = arguments[1];

    {
        DeviceSimulator::VirtualDevice device;
        if (!stackIsStable(device, 128))
        {
            return 2;
        }
    }

    {
        DeviceSimulator::VirtualDevice device;
        FakeInstance instance{"non-function", scriptPath};
        FakeDsim dsim;
        setupDevice(device, instance, dsim);
        lua_State *luaContext = device.getLuaContext();
        if (!runLua(luaContext, "device_simulate = 42") || !stackIsStable(device, 128))
        {
            return 3;
        }
    }

    {
        DeviceSimulator::VirtualDevice device;
        FakeInstance instance{"successful-callback", scriptPath};
        FakeDsim dsim;
        setupDevice(device, instance, dsim);
        lua_State *luaContext = device.getLuaContext();
        if (!runLua(luaContext,
                    "simulation_calls = 0; function device_simulate() simulation_calls = simulation_calls + 1 end") ||
            !stackIsStable(device, 128))
        {
            return 4;
        }
        lua_getglobal(luaContext, "simulation_calls");
        const bool calledEveryTick = lua_tointeger(luaContext, -1) == 128;
        lua_pop(luaContext, 1);
        if (!calledEveryTick)
        {
            return 5;
        }
    }

    {
        DeviceSimulator::VirtualDevice device;
        FakeInstance instance{"failing-callback", scriptPath};
        FakeDsim dsim;
        setupDevice(device, instance, dsim);
        lua_State *luaContext = device.getLuaContext();
        if (!runLua(luaContext,
                    "simulation_calls = 0; function device_simulate() simulation_calls = simulation_calls + 1; "
                    "error('tick failure') end") ||
            !stackIsStable(device, 128))
        {
            return 6;
        }
        lua_getglobal(luaContext, "simulation_calls");
        const bool disabledAfterFailure = lua_tointeger(luaContext, -1) == 1;
        lua_pop(luaContext, 1);
        if (!disabledAfterFailure)
        {
            return 7;
        }
    }

    return 0;
}
