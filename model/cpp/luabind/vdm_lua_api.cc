#include "vdm_lua_api.hpp"

#include <cstring>

namespace
{
class StackRestore
{
  public:
    explicit StackRestore(lua_State *lua) : lua_(lua), top_(lua_gettop(lua))
    {
    }

    ~StackRestore()
    {
        lua_settop(lua_, top_);
    }

  private:
    lua_State *lua_;
    int top_;
};

void setInteger(lua_State *lua, const char *name, lua_Integer value)
{
    lua_pushinteger(lua, value);
    lua_setglobal(lua, name);
}

bool commandHasInput(BYTE command)
{
    return command == VDM_WRITEDATA || command == VDM_WRITEREGS;
}

bool pushOptionalCallback(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    if (lua_isfunction(lua, -1))
    {
        return true;
    }

    lua_pop(lua, 1);
    return false;
}
} // namespace

namespace DeviceSimulator
{
void registerVdmLuaApi(lua_State *lua)
{
    if (!lua)
    {
        return;
    }

    StackRestore restore(lua);
    setInteger(lua, "VDM_API_VERSION", VDM_API_VERSION);
    setInteger(lua, "VDM51_CLASS", VDM51_CLASS);
    setInteger(lua, "VDM11_CLASS", VDM11_CLASS);
    setInteger(lua, "VDMPIC16_CLASS", VDMPIC16_CLASS);
    setInteger(lua, "VDMAVR_CLASS", VDMAVR_CLASS);
    setInteger(lua, "VDMPIC18_CLASS", VDMPIC18_CLASS);
    setInteger(lua, "VDMARM7_CLASS", VDMARM7_CLASS);

    setInteger(lua, "VDM_INIT", VDM_INIT);
    setInteger(lua, "VDM_TERM", VDM_TERM);
    setInteger(lua, "VDM_PLAY", VDM_PLAY);
    setInteger(lua, "VDM_STEP", VDM_STEP);
    setInteger(lua, "VDM_PAUSE", VDM_PAUSE);
    setInteger(lua, "VDM_WRITEDATA", VDM_WRITEDATA);
    setInteger(lua, "VDM_READDATA", VDM_READDATA);
    setInteger(lua, "VDM_READREGS", VDM_READREGS);
    setInteger(lua, "VDM_WRITEREGS", VDM_WRITEREGS);
    setInteger(lua, "VDM_SETBP", VDM_SETBP);
    setInteger(lua, "VDM_CLRBP", VDM_CLRBP);
    setInteger(lua, "VDM_SETPC", VDM_SETPC);
    setInteger(lua, "VDM_GETPC", VDM_GETPC);
    setInteger(lua, "VDM_RESET", VDM_RESET);
    setInteger(lua, "VDM_GETTID", VDM_GETTID);

    setInteger(lua, "ERR_VDM_FAILED", ERR_VDM_FAILED);
    setInteger(lua, "ERR_VDM_OK", ERR_VDM_OK);
    setInteger(lua, "ERR_VDM_TIMEOUT", ERR_VDM_TIMEOUT);
    setInteger(lua, "ERR_VDM_BADCOMMAND", ERR_VDM_BADCOMMAND);
    setInteger(lua, "ERR_VDM_BADADDRESS", ERR_VDM_BADADDRESS);
    setInteger(lua, "ERR_VDM_BADDATALEN", ERR_VDM_BADDATALEN);
    setInteger(lua, "ERR_VDM_SIMFAILED", ERR_VDM_SIMFAILED);
    setInteger(lua, "ERR_VDM_NOTARGET", ERR_VDM_NOTARGET);
}

bool hasLuaVdmHandler(lua_State *lua)
{
    if (!lua)
    {
        return false;
    }

    StackRestore restore(lua);
    lua_getglobal(lua, "device_vdm_command");
    return lua_isfunction(lua, -1);
}

LRESULT dispatchLuaVdmCommand(lua_State *lua, VDM_COMMAND *command, BYTE *data)
{
    if (!lua || !command)
    {
        return ERR_VDM_FAILED;
    }

    StackRestore restore(lua);
    lua_getglobal(lua, "device_vdm_command");
    if (!lua_isfunction(lua, -1))
    {
        return ERR_VDM_NOTARGET;
    }

    lua_pushinteger(lua, command->command);
    lua_pushinteger(lua, command->memspace);
    lua_pushinteger(lua, command->address);
    lua_pushinteger(lua, command->datalength);

    if (commandHasInput(command->command) && command->datalength != 0)
    {
        if (!data)
        {
            return ERR_VDM_BADDATALEN;
        }
        lua_pushlstring(lua, reinterpret_cast<const char *>(data), command->datalength);
    }
    else
    {
        lua_pushliteral(lua, "");
    }

    if (lua_pcall(lua, 5, 2, 0) != LUA_OK)
    {
        return ERR_VDM_SIMFAILED;
    }

    int isInteger = 0;
    const lua_Integer result = lua_tointegerx(lua, -2, &isInteger);
    if (!isInteger)
    {
        return ERR_VDM_FAILED;
    }

    if (result != ERR_VDM_OK || lua_isnil(lua, -1))
    {
        return static_cast<LRESULT>(result);
    }

    size_t outputSize = 0;
    const char *output = lua_tolstring(lua, -1, &outputSize);
    if (!output)
    {
        return ERR_VDM_FAILED;
    }
    if (outputSize > command->datalength || (outputSize != 0 && !data))
    {
        return ERR_VDM_BADDATALEN;
    }

    if (outputSize != 0)
    {
        std::memcpy(data, output, outputSize);
    }
    command->datalength = static_cast<DWORD>(outputSize);
    return ERR_VDM_OK;
}

void dispatchLuaVdmLoad(lua_State *lua, INT format, INT segment, ADDRESS address, const BYTE *data, INT numbytes)
{
    if (!lua || numbytes < 0 || (numbytes != 0 && !data))
    {
        return;
    }

    StackRestore restore(lua);
    if (!pushOptionalCallback(lua, "device_vdm_load"))
    {
        return;
    }

    lua_pushinteger(lua, format);
    lua_pushinteger(lua, segment);
    lua_pushinteger(lua, address);
    const char *payload = data ? reinterpret_cast<const char *>(data) : "";
    lua_pushlstring(lua, payload, static_cast<size_t>(numbytes));
    lua_pcall(lua, 4, 0, 0);
}

void dispatchLuaVdmDisassemble(lua_State *lua, ADDRESS address, INT numbytes)
{
    if (!lua || numbytes < 0)
    {
        return;
    }

    StackRestore restore(lua);
    if (!pushOptionalCallback(lua, "device_vdm_disassemble"))
    {
        return;
    }

    lua_pushinteger(lua, address);
    lua_pushinteger(lua, numbytes);
    lua_pcall(lua, 2, 0, 0);
}
} // namespace DeviceSimulator
