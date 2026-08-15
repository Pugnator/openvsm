#include <iostream>

#include <lua.hpp>

namespace
{
void setPath(lua_State *lua, const char *name, const char *path)
{
    lua_pushstring(lua, path);
    lua_setglobal(lua, name);
}
} // namespace

int main(int argumentCount, char **arguments)
{
    if (argumentCount != 4)
    {
        std::cerr << "Expected CHIP-8 core, framebuffer, and test script paths\n";
        return 1;
    }

    auto *lua = luaL_newstate();
    if (lua == nullptr)
    {
        return 1;
    }
    luaL_openlibs(lua);

    setPath(lua, "CHIP8_CORE_PATH", arguments[1]);
    setPath(lua, "CHIP8_FRAMEBUFFER_PATH", arguments[2]);

    if (luaL_dofile(lua, arguments[3]) != LUA_OK)
    {
        std::cerr << lua_tostring(lua, -1) << '\n';
        lua_close(lua);
        return 1;
    }

    lua_close(lua);
    return 0;
}
