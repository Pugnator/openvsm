#include <cstddef>
#include <iostream>
#include <vector>

#include <lua.hpp>

namespace
{
struct ScheduledCallback
{
    lua_Integer time;
    lua_Integer eventId;
};

struct Harness
{
    std::vector<int> pinLevels;
    std::vector<ScheduledCallback> callbacks;
};

Harness *harness(lua_State *lua)
{
    return static_cast<Harness *>(lua_touserdata(lua, lua_upvalueindex(1)));
}

int setPin(lua_State *lua)
{
    const auto value = luaL_checkinteger(lua, 2);
    if (value != 0 && value != 1)
    {
        return luaL_argerror(lua, 2, "UART test pin accepts only zero or one");
    }
    harness(lua)->pinLevels.push_back(static_cast<int>(value));
    return 0;
}

int scheduleCallback(lua_State *lua)
{
    harness(lua)->callbacks.push_back({luaL_checkinteger(lua, 1), luaL_checkinteger(lua, 2)});
    return 0;
}

bool callSend(lua_State *lua, lua_Integer time)
{
    lua_getglobal(lua, "uart");
    lua_getfield(lua, -1, "send");
    lua_pushvalue(lua, -2);
    lua_pushinteger(lua, time);
    if (lua_pcall(lua, 2, 1, 0) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_pop(lua, 2);
        return false;
    }
    const bool sent = lua_toboolean(lua, -1) != 0;
    lua_pop(lua, 2);
    return sent;
}
} // namespace

int main(int argumentCount, char **arguments)
{
    if (argumentCount != 2)
    {
        std::cerr << "Expected the UART Lua module path\n";
        return 1;
    }

    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    Harness test;
    lua_pushinteger(lua, 1000000000000LL);
    lua_setglobal(lua, "SEC");
    lua_pushlightuserdata(lua, &test);
    lua_pushcclosure(lua, scheduleCallback, 1);
    lua_setglobal(lua, "set_callback");

    lua_newtable(lua);
    lua_pushlightuserdata(lua, &test);
    lua_pushcclosure(lua, setPin, 1);
    lua_setfield(lua, -2, "set");
    lua_setglobal(lua, "TX");

    if (luaL_dofile(lua, arguments[1]) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }
    lua_setglobal(lua, "Uart");

    const char *configure = R"(
        uart = Uart.new(TX, {bit_time = 100, event_id = 77})
        assert(uart:send_string(string.char(0x55, 0xa3)) == 2)
        assert(not pcall(function() uart:send_byte(256) end))
    )";
    if (luaL_dostring(lua, configure) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }
    if (!callSend(lua, 1000))
    {
        std::cerr << "UART did not start a queued frame\n";
        lua_close(lua);
        return 1;
    }

    for (std::size_t index = 0; index < test.callbacks.size(); ++index)
    {
        if (!callSend(lua, test.callbacks[index].time))
        {
            std::cerr << "UART stopped before the queued frames completed\n";
            lua_close(lua);
            return 1;
        }
    }

    const std::vector<int> expectedLevels = {
        1,                            // idle before the first packet
        0, 1, 0, 1, 0, 1, 0, 1, 0, 1, // start, 0x55 LSB first, stop
        0, 1, 1, 0, 0, 0, 1, 0, 1, 1  // start, 0xa3 LSB first, stop
    };
    if (test.pinLevels != expectedLevels || test.callbacks.size() != 19)
    {
        std::cerr << "UART frame levels or callback count did not match 8-N-1 output\n";
        lua_close(lua);
        return 1;
    }

    for (std::size_t index = 0; index < test.callbacks.size(); ++index)
    {
        const auto expectedTime = 1100 + static_cast<lua_Integer>(index) * 100;
        if (test.callbacks[index].time != expectedTime || test.callbacks[index].eventId != 77)
        {
            std::cerr << "UART callback timing or event ID changed\n";
            lua_close(lua);
            return 1;
        }
    }

    if (luaL_dostring(lua, "assert(not uart:busy())") != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }

    lua_close(lua);
    return 0;
}
