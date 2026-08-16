#include <array>
#include <initializer_list>
#include <iostream>
#include <string>

#include <lua.hpp>

namespace
{
struct Pin
{
    int value = 0;
};

Pin *pinFromUpvalue(lua_State *lua)
{
    return static_cast<Pin *>(lua_touserdata(lua, lua_upvalueindex(1)));
}

int getPin(lua_State *lua)
{
    lua_pushinteger(lua, pinFromUpvalue(lua)->value);
    return 1;
}

int setPin(lua_State *lua)
{
    const auto value = luaL_checkinteger(lua, 2);
    if (value != 0 && value != 1)
    {
        return luaL_argerror(lua, 2, "test pin accepts only zero or one");
    }
    pinFromUpvalue(lua)->value = static_cast<int>(value);
    return 0;
}

void registerPin(lua_State *lua, const char *name, Pin &pin)
{
    lua_newtable(lua);
    lua_pushlightuserdata(lua, &pin);
    lua_pushcclosure(lua, getPin, 1);
    lua_setfield(lua, -2, "get");
    lua_pushlightuserdata(lua, &pin);
    lua_pushcclosure(lua, setPin, 1);
    lua_setfield(lua, -2, "set");
    lua_setglobal(lua, name);
}

bool call(lua_State *lua, const char *name, std::initializer_list<lua_Integer> arguments = {})
{
    const int stackTop = lua_gettop(lua);
    lua_getglobal(lua, name);
    if (!lua_isfunction(lua, -1))
    {
        std::cerr << name << " is not a function\n";
        lua_settop(lua, stackTop);
        return false;
    }
    for (const auto argument : arguments)
    {
        lua_pushinteger(lua, argument);
    }
    if (lua_pcall(lua, static_cast<int>(arguments.size()), 0, 0) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_settop(lua, stackTop);
        return false;
    }
    lua_settop(lua, stackTop);
    return true;
}

bool checkPinDeclarations(lua_State *lua)
{
    const std::array<const char *, 3> names = {"A", "B", "Q"};
    const std::array<int, 3> onTimes = {100000, 200000, 300000};
    const std::array<int, 3> offTimes = {110000, 210000, 310000};

    lua_getglobal(lua, "device_pins");
    if (!lua_istable(lua, -1) || lua_rawlen(lua, -1) != names.size())
    {
        lua_pop(lua, 1);
        return false;
    }

    for (std::size_t index = 0; index < names.size(); ++index)
    {
        lua_rawgeti(lua, -1, index + 1);
        lua_getfield(lua, -1, "name");
        const bool nameMatches = std::string{luaL_checkstring(lua, -1)} == names[index];
        lua_pop(lua, 1);
        lua_getfield(lua, -1, "on_time");
        const bool onTimeMatches = luaL_checkinteger(lua, -1) == onTimes[index];
        lua_pop(lua, 1);
        lua_getfield(lua, -1, "off_time");
        const bool offTimeMatches = luaL_checkinteger(lua, -1) == offTimes[index];
        lua_pop(lua, 2);
        if (!nameMatches || !onTimeMatches || !offTimeMatches)
        {
            lua_pop(lua, 1);
            return false;
        }
    }

    lua_pop(lua, 1);
    return true;
}

bool integerGlobalEquals(lua_State *lua, const char *name, lua_Integer expected)
{
    lua_getglobal(lua, name);
    const bool matches = lua_isinteger(lua, -1) && lua_tointeger(lua, -1) == expected;
    lua_pop(lua, 1);
    return matches;
}
} // namespace

int main(int argumentCount, char **arguments)
{
    if (argumentCount != 2)
    {
        std::cerr << "Expected the test IC Lua path\n";
        return 1;
    }

    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    Pin inputA;
    Pin inputB;
    Pin outputQ;
    registerPin(lua, "A", inputA);
    registerPin(lua, "B", inputB);
    registerPin(lua, "Q", outputQ);

    if (luaL_dofile(lua, arguments[1]) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }
    if (!checkPinDeclarations(lua) || !call(lua, "device_init") || outputQ.value != 1)
    {
        std::cerr << "The test IC did not initialize with its declared pins\n";
        lua_close(lua);
        return 1;
    }

    lua_getglobal(lua, "initialized");
    const bool initialized = lua_toboolean(lua, -1) != 0;
    lua_pop(lua, 1);
    if (!initialized)
    {
        std::cerr << "device_init was not observed\n";
        lua_close(lua);
        return 1;
    }

    for (int a = 0; a <= 1; ++a)
    {
        for (int b = 0; b <= 1; ++b)
        {
            inputA.value = a;
            inputB.value = b;
            if (!call(lua, "device_simulate") || outputQ.value != 1 - (a * b))
            {
                std::cerr << "NAND truth-table regression for A=" << a << ", B=" << b << '\n';
                lua_close(lua);
                return 1;
            }
        }
    }

    constexpr lua_Integer callbackTime = 0x123456789LL;
    constexpr lua_Integer callbackId = 73;
    if (!call(lua, "timer_callback", {callbackTime, callbackId}) ||
        !integerGlobalEquals(lua, "last_callback_time", callbackTime) ||
        !integerGlobalEquals(lua, "last_callback_id", callbackId))
    {
        std::cerr << "Timer callback arguments were not retained by the test IC\n";
        lua_close(lua);
        return 1;
    }

    lua_close(lua);
    return 0;
}
