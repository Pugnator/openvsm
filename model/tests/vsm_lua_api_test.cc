#include "luabind/vsm_lua_api.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

namespace
{
std::string formatMessage(CHAR *format, va_list arguments)
{
    char buffer[512] = {};
    std::vsnprintf(buffer, sizeof(buffer), format, arguments);
    return buffer;
}

class FakeInstance final : public IINSTANCE
{
  public:
    std::string logText;
    std::string messageText;
    std::string warningText;
    std::string errorText;
    std::string stringValue = "configured value";

    CHAR *id() override
    {
        return const_cast<CHAR *>("test");
    }

    CHAR *value() override
    {
        return nullptr;
    }

    CHAR *getstrval(CHAR *name, CHAR *defaultValue = nullptr) override
    {
        return std::strcmp(name, "TEXT") == 0 ? stringValue.data() : defaultValue;
    }

    void getnumval(DOUBLE *result, CHAR *name, DOUBLE defaultValue = 0) override
    {
        *result = std::strcmp(name, "NUMBER") == 0 ? 3.25 : defaultValue;
    }

    BOOL getboolval(CHAR *name, BOOL defaultValue = FALSE) override
    {
        return std::strcmp(name, "ENABLED") == 0 ? TRUE : defaultValue;
    }

    DWORD gethexval(CHAR *name, DWORD defaultValue = 0) override
    {
        return std::strcmp(name, "MASK") == 0 ? 0xa5 : defaultValue;
    }

    LONG getinitval(CHAR *name, LONG defaultValue = 0) override
    {
        return std::strcmp(name, "INITIAL") == 0 ? -12 : defaultValue;
    }

    RELTIME getdelay(CHAR *, RELTIME = 0) override
    {
        return 0;
    }

    IACTIVEMODEL *getactivemodel() override
    {
        return nullptr;
    }

    IINSTANCE *getinterfacemodel() override
    {
        return nullptr;
    }

    BOOL getmoddata(BYTE **, DWORD *) override
    {
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

    void log(CHAR *format, ...) override
    {
        va_list arguments;
        va_start(arguments, format);
        logText = formatMessage(format, arguments);
        va_end(arguments);
    }

    void warning(CHAR *format, ...) override
    {
        va_list arguments;
        va_start(arguments, format);
        warningText = formatMessage(format, arguments);
        va_end(arguments);
    }

    void error(CHAR *format, ...) override
    {
        va_list arguments;
        va_start(arguments, format);
        errorText = formatMessage(format, arguments);
        va_end(arguments);
    }

    void fatal(CHAR *, ...) override
    {
    }

    BOOL message(CHAR *format, ...) override
    {
        va_list arguments;
        va_start(arguments, format);
        messageText = formatMessage(format, arguments);
        va_end(arguments);
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
        return FALSE;
    }

    BOOL loadmemory(CHAR *, void *, UINT, UINT = 0, UINT = 0) override
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

class FakeDsim final : public IDSIMCKT
{
  public:
    ABSTIME time = 0x123456789LL;
    ABSTIME callbackTime = -1;
    EVENTID callbackId = -1;
    IDSIMMODEL *callbackModel = nullptr;

    void sysvar(DOUBLE *result, DSIMVARS variable) override
    {
        if (variable == DSIMTIMENOW)
        {
            std::memcpy(result, &time, sizeof(time));
        }
    }

    EVENT *setcallback(ABSTIME eventTime, IDSIMMODEL *eventModel, EVENTID eventId) override
    {
        callbackTime = eventTime;
        callbackModel = eventModel;
        callbackId = eventId;
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
} // namespace

int main()
{
    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    FakeInstance instance;
    FakeDsim dsim;
    FakeModel model;
    DeviceSimulator::registerVsmLuaApi(lua, &instance, &dsim, &model);

    const char *contract = R"(
        local constants = {
            UNDEFINED, TSTATE, FSTATE, PLO, ILO, SLO, WLO, FLT,
            WHI, SHI, IHI, PHI, WUD, SUD, NOW, NSEC, USEC, MSEC,
            SEC, TTL, CMOS, I2L
        }
        for _, value in ipairs(constants) do assert(type(value) == "number") end
        assert(NOW == 0 and NSEC == 1000 and USEC == 1000000)
        assert(MSEC == 1000000000 and SEC == 1000000000000)
        assert(TTL == 0 and CMOS == 1 and I2L == 2)

        local states = {
            [UNDEFINED] = "UNDEFINED", [TSTATE] = "TSTATE", [FSTATE] = "FSTATE",
            [PLO] = "PLO", [ILO] = "ILO", [SLO] = "SLO", [WLO] = "WLO",
            [FLT] = "FLT", [WHI] = "WHI", [SHI] = "SHI", [IHI] = "IHI",
            [PHI] = "PHI", [WUD] = "WUD", [SUD] = "SUD"
        }
        for state, name in pairs(states) do assert(state_to_string(state) == name) end
        assert(state_to_string(123456) == "wrong state")

        local supported = {
            "state_to_string", "info", "message", "warning", "vsm_error",
            "get_string_param", "get_num_param", "get_bool_param",
            "get_init_param", "get_hex_param", "systime", "set_callback"
        }
        for _, name in ipairs(supported) do assert(type(_G[name]) == "function") end

        assert(get_string_param("TEXT") == "configured value")
        assert(get_num_param("NUMBER") == 3.25)
        assert(get_bool_param("ENABLED") == true)
        assert(get_init_param("INITIAL") == -12)
        assert(get_hex_param("MASK") == 0xa5)
        assert(systime() == 0x123456789)

        info("100% info")
        message("100% message")
        warning("100% warning")
        vsm_error("100% error")
        set_callback(0x23456789a, 73)
        assert(not pcall(function() set_callback(-1, 1) end))
        assert(not pcall(function() set_callback(0, 0x100000000) end))

        -- The standard Lua error function is deliberately not shadowed.
        assert(not pcall(function() error("ordinary Lua error") end))

        local unsupported = {
            "create_debug_popup", "create_memory_popup", "create_source_popup",
            "create_status_popup", "create_var_popup", "delete_popup",
            "set_memory_popup", "repaint_memory_popup", "print_to_debug_popup",
            "dump_to_debug_popup", "add_source_file", "set_bus", "get_bus"
        }
        for _, name in ipairs(unsupported) do
            assert(type(_G[name]) == "function")
            local ok, reason = pcall(_G[name])
            assert(not ok and reason:find("not supported"))
        end
    )";
    if (luaL_dostring(lua, contract) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }

    const bool hostCallsMatch = instance.logText == "100% info" && instance.messageText == "100% message" &&
                                instance.warningText == "100% warning" && instance.errorText == "100% error";
    const bool callbackMatches =
        dsim.callbackTime == 0x23456789aLL && dsim.callbackId == 73 && dsim.callbackModel == &model;
    lua_close(lua);
    if (!hostCallsMatch || !callbackMatches)
    {
        std::cerr << "Lua VSM calls were not forwarded to the SDK interfaces\n";
        return 1;
    }
    return 0;
}
