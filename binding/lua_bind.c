#include <vsm_api.h>

static int32_t lua_console_log (lua_State *L);
static int32_t lua_set_pin_state (lua_State *L);
static int32_t lua_get_pin_state (lua_State *L);
static int32_t lua_is_pin_low (lua_State *L);
static int32_t lua_is_pin_high (lua_State *L);
static int32_t lua_is_pin_floating (lua_State *L);
static int32_t lua_out_log(lua_State *L);
static int32_t lua_out_message(lua_State *L);
static int32_t lua_out_warning(lua_State *L);
static int32_t lua_out_error(lua_State *L);

void register_functions (lua_State *L)
{
	/* console_log Lua bind */
	lua_pushcfunction(L, lua_console_log);
    lua_setglobal(L, "console_log");
    /* Lua bind */
    lua_pushcfunction(L, lua_set_pin_state);
    lua_setglobal(L, "set_pin_state");
    /* Lua bind */
    lua_pushcfunction(L, lua_get_pin_state);
    lua_setglobal(L, "get_pin_state");
    /* Lua bind */
    lua_pushcfunction(L, lua_is_pin_low);
    lua_setglobal(L, "is_pin_low");
    /* Lua bind */
    lua_pushcfunction(L, lua_is_pin_high);
    lua_setglobal(L, "is_pin_high");
    /* Lua bind */
    lua_pushcfunction(L, lua_is_pin_floating);
    lua_setglobal(L, "is_pin_floating");
    /* Lua bind */
    lua_pushcfunction(L, lua_out_log);
    lua_setglobal(L, "out_log");
    /* Lua bind */
    lua_pushcfunction(L, lua_out_message);
    lua_setglobal(L, "out_message");
    /* Lua bind */
    lua_pushcfunction(L, lua_out_warning);
    lua_setglobal(L, "out_warning");
    /* Lua bind */
    lua_pushcfunction(L, lua_out_error);
    lua_setglobal(L, "out_error");
   

    lua_pushinteger(L, SHI);
	lua_setglobal(L, "SHI");
	lua_pushinteger(L, SLO);
	lua_setglobal(L, "SLO");
	lua_pushinteger(L, FLT);
	lua_setglobal(L, "FLT");
	lua_pushinteger(L, UNDEFINED);
	lua_setglobal(L, "UNDEFINED");

	for (int i=0; device_pins[i].name; i++)
	{
		lua_pushinteger(L, i);
		lua_setglobal(L, device_pins[i].name);		
	}
}

const char *lua_execute_script (const char *function)
{	
	/* Main state of Lua */
	
	int32_t lua_err = 0;       
    /* Load script but do not execute it */
    char spath[512] = {0};
    if ( 0 == GetEnvironmentVariable("LUAVSM", spath, sizeof spath))
    {
    	console_log("LUAVSM env variable was not set");
    }
    char script[512]={0};
    sprintf(script, "%s\\test.lua", spath);
    
    lua_err = luaL_loadfile ( luactx, script );
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

static int32_t lua_console_log (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  console_log("%s", text);
  return 0;  /* number of results */
}

static int32_t lua_set_pin_state (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (2 != argnum)
  {
  	console_log("Function %s expects 2 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
	return 0;  
  }
  int32_t pin_num = lua_tonumber(L, 1);
  int32_t pin_state = lua_tonumber(L, -1);
  //console_log("Pin %d, state: %d\n", pin_num, pin_state);
  set_pin_state(device_pins[pin_num], pin_state);
  return 0;  /* number of results */
}

static int32_t lua_get_pin_state (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  int32_t pin_num = lua_tonumber(L, -1);
  
  if(TRUE == is_pin_high(device_pins[pin_num].pin))
  {
  	lua_pushnumber(L, SHI);  
  	return 1;
  }
  else if(TRUE == is_pin_low(device_pins[pin_num].pin))
  {
  	lua_pushnumber(L, SLO);
  	return 1;
  }
  else if (TRUE == is_pin_floating(device_pins[pin_num].pin))
  {
  	lua_pushnumber(L, FLT);
  	return 1;	
  }
  else
  {
  	lua_pushnumber(L, UNDEFINED);
  	return 1;	
  } 
}

static int32_t lua_is_pin_low (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  int32_t pin_num = lua_tonumber(L, -1);  
  lua_pushboolean(L, is_pin_low(device_pins[pin_num].pin));
  return 1;  
}

static int32_t lua_is_pin_high (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  int32_t pin_num = lua_tonumber(L, -1);  
  lua_pushboolean(L, is_pin_high(device_pins[pin_num].pin));
  return 1;  
}

static int32_t lua_is_pin_floating (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  int32_t pin_num = lua_tonumber(L, -1);  
  lua_pushboolean(L, is_pin_floating(device_pins[pin_num].pin));
  return 1;  
}

static int32_t lua_out_log (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_log(text); 
  return 0;  
}

static int32_t lua_out_message (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_message(text); 
  return 0;  
}

static int32_t lua_out_warning (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_warning(text); 
  return 0;  
}

static int32_t lua_out_error(lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	console_log("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_error(text); 
  return 0;  
}