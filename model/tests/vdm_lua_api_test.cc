#include "luabind/vdm_lua_api.hpp"

#include <array>
#include <iostream>
#include <string>

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

std::string globalString(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    size_t size = 0;
    const char *value = lua_tolstring(lua, -1, &size);
    std::string result = value ? std::string(value, size) : std::string();
    lua_pop(lua, 1);
    return result;
}

lua_Integer globalInteger(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    const lua_Integer value = lua_tointeger(lua, -1);
    lua_pop(lua, 1);
    return value;
}
} // namespace

int main()
{
    lua_State *lua = luaL_newstate();
    if (!expect(lua != nullptr, "Lua state creation failed"))
    {
        return 1;
    }
    luaL_openlibs(lua);

    const int initialTop = lua_gettop(lua);
    DeviceSimulator::registerVdmLuaApi(lua);
    bool ok = expect(lua_gettop(lua) == initialTop, "constant registration changed the Lua stack") &&
              expect(globalInteger(lua, "VDM_READDATA") == VDM_READDATA, "VDM_READDATA was not registered") &&
              expect(globalInteger(lua, "ERR_VDM_OK") == ERR_VDM_OK, "ERR_VDM_OK was not registered") &&
              expect(!DeviceSimulator::hasLuaVdmHandler(lua), "missing handler was reported as present");

    ok = run(lua, R"(
        function device_vdm_command(command, memspace, address, length, payload)
            last_command = command
            last_memspace = memspace
            last_address = address
            last_length = length
            last_payload = payload
            if command == VDM_READDATA then
                return ERR_VDM_OK, string.char(0x11, 0x00, 0xff)
            elseif command == VDM_WRITEDATA then
                return ERR_VDM_OK
            elseif command == VDM_STEP then
                error("step failed")
            elseif command == VDM_GETPC then
                return ERR_VDM_OK, string.rep("x", length + 1)
            end
            return ERR_VDM_BADCOMMAND
        end

        function device_vdm_load(format, segment, address, payload)
            load_format = format
            load_segment = segment
            load_address = address
            load_payload = payload
        end

        function device_vdm_disassemble(address, length)
            disassemble_address = address
            disassemble_length = length
        end
    )") &&
         ok;

    ok = expect(DeviceSimulator::hasLuaVdmHandler(lua), "VDM handler was not detected") && ok;

    std::array<BYTE, 3> readBuffer = {};
    VDM_COMMAND readCommand = {VDM_READDATA, 2, 0x1234, static_cast<DWORD>(readBuffer.size())};
    ok = expect(DeviceSimulator::dispatchLuaVdmCommand(lua, &readCommand, readBuffer.data()) == ERR_VDM_OK,
                "read command failed") &&
         expect(readBuffer == std::array<BYTE, 3>{0x11, 0x00, 0xff}, "read payload was not copied") &&
         expect(readCommand.datalength == 3, "read payload length was not returned") &&
         expect(globalInteger(lua, "last_memspace") == 2, "memory space was not forwarded") &&
         expect(globalInteger(lua, "last_address") == 0x1234, "address was not forwarded") && ok;

    std::array<BYTE, 4> writeBuffer = {0xde, 0xad, 0x00, 0xbe};
    VDM_COMMAND writeCommand = {VDM_WRITEDATA, 4, 0x88, static_cast<DWORD>(writeBuffer.size())};
    ok = expect(DeviceSimulator::dispatchLuaVdmCommand(lua, &writeCommand, writeBuffer.data()) == ERR_VDM_OK,
                "write command failed") &&
         expect(globalString(lua, "last_payload") == std::string("\xde\xad\x00\xbe", 4),
                "binary write payload was not forwarded") &&
         ok;

    VDM_COMMAND stepCommand = {VDM_STEP, 0, 0, 0};
    ok = expect(DeviceSimulator::dispatchLuaVdmCommand(lua, &stepCommand, nullptr) == ERR_VDM_SIMFAILED,
                "Lua command error was not isolated") &&
         expect(lua_gettop(lua) == initialTop, "VDM command changed the Lua stack") && ok;

    std::array<BYTE, 4> pcBuffer = {};
    VDM_COMMAND pcCommand = {VDM_GETPC, 0, 0, static_cast<DWORD>(pcBuffer.size())};
    ok = expect(DeviceSimulator::dispatchLuaVdmCommand(lua, &pcCommand, pcBuffer.data()) == ERR_VDM_BADDATALEN,
                "oversized VDM response was accepted") &&
         ok;

    std::array<BYTE, 3> loadData = {1, 0, 3};
    DeviceSimulator::dispatchLuaVdmLoad(lua, 7, 8, 0x200, loadData.data(), static_cast<INT>(loadData.size()));
    DeviceSimulator::dispatchLuaVdmDisassemble(lua, 0x300, 16);
    ok = expect(globalString(lua, "load_payload") == std::string("\x01\x00\x03", 3),
                "loader payload was not forwarded") &&
         expect(globalInteger(lua, "load_format") == 7, "loader format was not forwarded") &&
         expect(globalInteger(lua, "disassemble_address") == 0x300, "disassembly address was not forwarded") &&
         expect(globalInteger(lua, "disassemble_length") == 16, "disassembly length was not forwarded") &&
         expect(lua_gettop(lua) == initialTop, "optional VDM callbacks changed the Lua stack") && ok;

    lua_close(lua);

    lua = luaL_newstate();
    ok = expect(DeviceSimulator::dispatchLuaVdmCommand(lua, &stepCommand, nullptr) == ERR_VDM_NOTARGET,
                "missing command handler did not return ERR_VDM_NOTARGET") &&
         ok;
    lua_close(lua);

    return ok ? 0 : 1;
}
