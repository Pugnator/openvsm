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
static int32_t lua_set_callback(lua_State *L);
static int32_t lua_create_debug_popup (lua_State *L);
static int32_t lua_create_memory_popup (lua_State *L);
static int32_t lua_create_source_popup (lua_State *L);
static int32_t lua_create_status_popup (lua_State *L);
static int32_t lua_create_var_popup (lua_State *L);
static int32_t lua_delete_popup (lua_State *L);
static int32_t lua_print_popup (lua_State *L);
static int32_t lua_set_popup_memory (lua_State *L);

//It is temp ugly function, should create function/name array
//and loop through it in order to register functions
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
   	/* Lua bind */
    lua_pushcfunction(L, lua_set_callback);
    lua_setglobal(L, "set_callback");
    /* Lua bind */
    lua_pushcfunction(L, lua_create_debug_popup);
    lua_setglobal(L, "create_debug_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_create_memory_popup);
    lua_setglobal(L, "create_memory_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_create_source_popup);
    lua_setglobal(L, "create_source_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_create_status_popup);
    lua_setglobal(L, "create_status_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_create_var_popup);
    lua_setglobal(L, "create_var_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_delete_popup);
    lua_setglobal(L, "delete_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_print_popup);
    lua_setglobal(L, "print_popup");
    /* Lua bind */
    lua_pushcfunction(L, lua_set_popup_memory);
    lua_setglobal(L, "set_popup_memory");

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

void lua_load_script (const char *function)
{	
	(void) function;
	int32_t lua_err = 0;       
    char spath[512] = {0};
    if ( 0 == GetEnvironmentVariable("LUAVSM", spath, sizeof spath))
    {
    	out_error("LUAVSM env variable was not set");
    }
    char script[512]={0};
    sprintf(script, "%s\\test.lua", spath);
    
    lua_err = luaL_loadfile ( luactx, script );
    if(0 != lua_err)
    {
    	switch(lua_err)
    	{
    		case LUA_ERRSYNTAX:
    		out_error("Syntax error in Lua script");
    		return;
    		case LUA_ERRMEM:
    		out_error("Not enough memory to load script");
    		return;
    		case LUA_ERRFILE:
    		out_error("Error loading script file");
    		return;
    		default:
    		out_error("Unknown error, shouldn't happen");
    		assert(0);    		
    	}
    }
    /* Primer run, if not run it - nothing works, need for parse */
    if ( 0 != lua_pcall(luactx, 0, 0, 0))
    {
    	out_error("Failed to load the script");
    	return;
    }
      
    out_log("Successfully loaded Lua script");
}

void lua_run_function (const char *func_name)
{
	 /* Declare function to run */
    lua_getglobal(luactx, func_name);
    /* First argument */
    lua_pcall(luactx, 0, LUA_MULTRET, 0);
}

static int32_t lua_console_log (lua_State *L) 
{
  //const char *text = lua_tostring(L, -1);  /* get argument */  
  //out_error("%s", text);
  return 0;  /* number of results */
}

static int32_t lua_delete_popup (lua_State *L) 
{  
  int id = lua_tonumber(L, -1);
  delete_popup(id);
  return 0;  /* number of results */
}

static int32_t lua_create_debug_popup (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  IPOPUP *popup = create_debug_popup (text, popup_id++);
  lua_pushlightuserdata(L, popup);
  return 0;  /* number of results */
}

static int32_t lua_create_source_popup (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  IDEBUGPOPUP *popup = create_source_popup (text, popup_id++);
  lua_pushlightuserdata(L, popup);
  return 0;  /* number of results */
}
static int32_t lua_create_status_popup (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  IDEBUGPOPUP *popup = create_status_popup (text, popup_id++);
  lua_pushlightuserdata(L, popup);
  return 0;  /* number of results */
}
static int32_t lua_create_var_popup (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  IDEBUGPOPUP *popup = create_var_popup (text, popup_id++);
  lua_pushlightuserdata(L, popup);
  return 0;  /* number of results */
}

static int32_t lua_create_memory_popup (lua_State *L) 
{
  const char *text = lua_tostring(L, -1);  /* get argument */  
  memory_popup = create_memory_popup (text, popup_id++);
  lua_pushinteger(L, popup_id);
  return 0;  /* number of results */
}

static int32_t lua_set_pin_state (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (2 != argnum)
  {
  	out_error("Function %s expects 2 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
	return 0;  
  }
  int32_t pin_num = lua_tonumber(L, 1);
  int32_t pin_state = lua_tonumber(L, -1);
  set_pin_state(device_pins[pin_num], pin_state);
  return 0;  /* number of results */
}

static int32_t lua_get_pin_state (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
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
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
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
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
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
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
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
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_error(text); 
  return 0;  
}

static int32_t lua_out_message (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
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
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_warning(text); 
  return 0;  
}

static int32_t lua_print_popup (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  //IPOPUP *popup = lua_touserdata(L, -1); 
  //const char *text = lua_tostring(L, -1); 
  //print_popup(popup, "%s", text); 
  return 0;  
}

static int32_t lua_set_popup_memory (lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (3 != argnum)
  {
  	out_error("Function %s expects 3 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  
  
	if (0 == lua_istable(luactx, -1))
	{
	    out_error("No device model found, it is fatal error");
	    return;
	}
	lua_gettable (luactx, -1);
	for (int i=1;;i++)
	{    
	    lua_rawget(luactx, i);
	    if (lua_isnil(luactx,-1)) 
	    	break;	    
	    out_log ("Iterate");
	    lua_pop(luactx, 2);
	}


  int32_t offset = lua_tonumber(L, 2); 
  int32_t size = lua_tonumber(L, 3);
  //set_popup_memory(memory_popup, offset, buffer, size);
  return 0;  
}

static int32_t lua_out_error(lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (1 != argnum)
  {
  	out_error("Function %s expects 1 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  const char *text = lua_tostring(L, -1); 
  out_error(text); 
  return 0;  
}

static int32_t lua_set_callback(lua_State *L) 
{  
  int32_t argnum = lua_gettop(L);
  if (2 != argnum)
  {
  	out_error("Function %s expects 2 arguments got %d\n", __PRETTY_FUNCTION__, argnum);
  	return 0;
  }
  ///TODO: Add check integer type
  size_t picotime = lua_tonumber(L, 1); 
  int32_t eventid = lua_tointeger(L, -1);
  set_callback(picotime, eventid);
  return 0;  
}

