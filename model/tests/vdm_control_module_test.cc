#include "luabind/vdm_lua_api.hpp"

#include <array>
#include <cstring>
#include <iostream>

namespace
{
bool expect(bool condition, const char *message)
{
    if (!condition)
    {
        std::cerr << message << '\n';
    }
    return condition;
}

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

LRESULT dispatch(lua_State *lua, BYTE commandCode, BYTE memspace, DWORD address, BYTE *data, DWORD size)
{
    VDM_COMMAND command = {commandCode, memspace, address, size};
    return DeviceSimulator::dispatchLuaVdmCommand(lua, &command, data);
}
} // namespace

int main(int argumentCount, char **arguments)
{
    if (argumentCount != 2)
    {
        std::cerr << "Expected the vdm_control Lua module path\n";
        return 1;
    }

    lua_State *lua = luaL_newstate();
    if (!expect(lua != nullptr, "Lua state creation failed"))
    {
        return 1;
    }
    luaL_openlibs(lua);
    DeviceSimulator::registerVdmLuaApi(lua);

    if (luaL_dofile(lua, arguments[1]) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }
    lua_setglobal(lua, "Control");

    bool ok = run(lua, R"(
        control = Control.new({
            id = "TEST-TARGET",
            regclass = VDM51_CLASS,
            regsize = 4,
            registers = string.char(1, 2, 3, 4),
            clock = 12000000,
            pc = 0x12345678
        })
        control:register_space(2, 8, string.char(0x10, 0x20))
        control:on(VDM_PLAY, function()
            play_count = (play_count or 0) + 1
            return ERR_VDM_OK
        end)
        control:install()
    )");

    const int initialTop = lua_gettop(lua);
    std::array<BYTE, sizeof(VDM_TARGETINFO)> infoBytes = {};
    ok = expect(dispatch(lua, VDM_INIT, 0, 0, infoBytes.data(), static_cast<DWORD>(infoBytes.size())) == ERR_VDM_OK,
                "VDM_INIT failed") &&
         ok;
    VDM_TARGETINFO info = {};
    std::memcpy(&info, infoBytes.data(), sizeof(info));
    ok = expect(std::strcmp(info.id, "TEST-TARGET") == 0, "target ID was not packed") &&
         expect(info.regclass == VDM51_CLASS, "register class was not packed") &&
         expect(info.regsize == 4, "register size was not packed") &&
         expect(info.apiver == VDM_API_VERSION, "VDM API version was not packed") &&
         expect(info.clock == 12000000, "target clock was not packed") && ok;

    std::array<BYTE, 4> memory = {};
    ok = expect(dispatch(lua, VDM_READDATA, 2, 0, memory.data(), static_cast<DWORD>(memory.size())) == ERR_VDM_OK,
                "memory read failed") &&
         expect(memory == std::array<BYTE, 4>{0x10, 0x20, 0x00, 0x00}, "initial memory contents changed") && ok;

    std::array<BYTE, 2> writeData = {0xaa, 0x00};
    ok =
        expect(dispatch(lua, VDM_WRITEDATA, 2, 2, writeData.data(), static_cast<DWORD>(writeData.size())) == ERR_VDM_OK,
               "binary memory write failed") &&
        ok;
    memory = {};
    ok = expect(dispatch(lua, VDM_READDATA, 2, 0, memory.data(), static_cast<DWORD>(memory.size())) == ERR_VDM_OK,
                "memory reread failed") &&
         expect(memory == std::array<BYTE, 4>{0x10, 0x20, 0xaa, 0x00}, "binary memory write was not retained") && ok;

    std::array<BYTE, 2> outside = {};
    ok = expect(dispatch(lua, VDM_READDATA, 2, 7, outside.data(), static_cast<DWORD>(outside.size())) ==
                    ERR_VDM_BADADDRESS,
                "out-of-range memory read was accepted") &&
         ok;

    std::array<BYTE, 4> registers = {};
    ok = expect(dispatch(lua, VDM_READREGS, 0, 0, registers.data(), static_cast<DWORD>(registers.size())) == ERR_VDM_OK,
                "register read failed") &&
         expect(registers == std::array<BYTE, 4>{1, 2, 3, 4}, "initial registers changed") && ok;
    registers = {9, 8, 0, 6};
    ok =
        expect(dispatch(lua, VDM_WRITEREGS, 0, 0, registers.data(), static_cast<DWORD>(registers.size())) == ERR_VDM_OK,
               "binary register write failed") &&
        ok;

    ok = expect(dispatch(lua, VDM_SETPC, 0, 0x89abcdef, nullptr, 0) == ERR_VDM_OK, "VDM_SETPC failed") && ok;
    std::array<BYTE, 4> pc = {};
    ok = expect(dispatch(lua, VDM_GETPC, 0, 0, pc.data(), static_cast<DWORD>(pc.size())) == ERR_VDM_OK,
                "VDM_GETPC failed") &&
         expect(pc == std::array<BYTE, 4>{0xef, 0xcd, 0xab, 0x89}, "program counter encoding changed") && ok;

    ok = expect(dispatch(lua, VDM_SETBP, 0, 0x1000, nullptr, 0) == ERR_VDM_OK, "VDM_SETBP failed") &&
         expect(run(lua, "assert(control.breakpoints[0x1000])"), "breakpoint was not recorded") &&
         expect(dispatch(lua, VDM_CLRBP, 0, 0x1000, nullptr, 0) == ERR_VDM_OK, "VDM_CLRBP failed") &&
         expect(run(lua, "assert(control.breakpoints[0x1000] == nil)"), "breakpoint was not removed") && ok;

    ok = expect(dispatch(lua, VDM_PLAY, 0, 0, nullptr, 0) == ERR_VDM_OK, "custom play callback failed") &&
         expect(run(lua, "assert(play_count == 1)"), "custom play callback was not invoked") &&
         expect(lua_gettop(lua) == initialTop, "control profile changed the Lua stack") && ok;

    std::array<BYTE, 64> targetId = {};
    ok = expect(dispatch(lua, VDM_GETTID, 0, 0, targetId.data(), static_cast<DWORD>(targetId.size())) == ERR_VDM_OK,
                "VDM_GETTID failed") &&
         expect(std::strcmp(reinterpret_cast<const char *>(targetId.data()), "TEST-TARGET") == 0,
                "VDM_GETTID returned the wrong target") &&
         ok;

    ok = expect(run(lua, "assert(not pcall(function() Control.new({regsize = 2, registers = 'abc'}) end))"),
                "oversized register initialization was accepted") &&
         ok;

    lua_close(lua);
    return ok ? 0 : 1;
}
