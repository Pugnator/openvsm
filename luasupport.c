#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

const char *lua_execute_script (const char *command, const char *delimiter)
{	
    /* Main state of Lua */
    lua_State *L;
    /* Init the Lua */
    L = luaL_newstate();
    /* Open libraries */
    luaL_openlibs(L);
    /* Load script but do not execute it */
    luaL_loadfile ( L, "test.lua" );
    /* Primer run, if not run it - nothing works, need for parse */
    if ( 0 != lua_pcall(L, 0, 0, 0))
    {
    	
    }
    /* Declare function to run */
    lua_getglobal(L, "foobar");
    /* First argument */
    lua_pushstring(L, command);
    /* Second argument */
    lua_pushstring(L, delimiter);
    /* Execute script */
    lua_pcall(L, 2, LUA_MULTRET, 0);
    /* Return result */
    const char *res = lua_tostring(L, -1);
    /* Print result if needed */
    printf("Result: %s\n",res);
    /* Close Lua */
    lua_close(L);
    /* Return result */
    return res;
}