#include <vsm_api.h>

static int l_sin (lua_State *L);

const char *lua_execute_script (const char *function)
{	
	/* Main state of Lua */
	lua_pushcfunction(luactx, l_sin);
    lua_setglobal(luactx, "mytest");
	int32_t lua_err = 0;       
    /* Load script but do not execute it */
    lua_err = luaL_loadfile ( luactx, "c:\\test.lua" );
    if(0 != lua_err)
    {
    	switch(lua_err)
    	{
    		case LUA_ERRSYNTAX:
    		console_log("Syntax error in Lua script\n");
    		break;
    		case LUA_ERRMEM:
    		console_log("Not enough memory to load script\n");
    		break;
    		case LUA_ERRFILE:
    		console_log("Error loading script file\n");
    		break;
    		default:
    		console_log("Unknown error, shouldn't happen\n");
    		assert(0);
    		break;
    	}
    }
    /* Primer run, if not run it - nothing works, need for parse */
    if ( 0 != lua_pcall(luactx, 0, 0, 0))
    {
    	console_log("Failed to load the script");
    }
    /* Declare function to run */
    lua_getglobal(luactx, function);
    /* First argument */
    //lua_pushstring(luactx, command);
    
    /* Execute script */
    lua_pcall(luactx, 0, LUA_MULTRET, 0);
    /* Return result */
    //const char *res = lua_tostring(luactx, -1);
    /* Print result if needed */
    //console_log("Result: %s\n",res);
    
    /* Return result */
    return "";
}

static int l_sin (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  console_log("%s", text);
  return 0;  /* number of results */
}