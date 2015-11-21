/**
 *
 * @file   lua_bind.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#include <vsmapi.h>

static const lua_bind_var lua_var_api_list[]=
{
	/* Logic states */
	{.var_name="SHI", .var_value=SHI},
	{.var_name="SLO", .var_value=SLO},
	{.var_name="FLT", .var_value=FLT},
	{.var_name="PLO", .var_value=PLO},
	{.var_name="ILO", .var_value=ILO},
	{.var_name="WLO", .var_value=WLO},
	{.var_name="WHI", .var_value=WHI},
	{.var_name="IHI", .var_value=IHI},
	{.var_name="PHI", .var_value=PHI},
	{.var_name="WUD", .var_value=WUD},
	{.var_name="SUD", .var_value=SUD},
	{.var_name="TSTATE", .var_value=TSTATE},
	{.var_name="FSTATE", .var_value=FSTATE},
	{.var_name="UNDEFINED", .var_value=UNDEFINED},
	/* Time suffixes */
	{.var_name="MSEC", .var_value=1000000000L},
	{.var_name="NSEC", .var_value=100000000L},
	{.var_name="USEC", .var_value=1000000L},
	{.var_name="SEC", .var_value=1000000000000L},
	{.var_name="NOW", .var_value=0L},
	/* Logic types */
	{.var_name="TTL", .var_value=TTL},
	{.var_name="CMOS", .var_value=CMOS},
	{.var_name="I2L", .var_value=I2L},
	{.var_name=NULL},
};

static const lua_bind_func lua_c_api_list[] =
{
	{.lua_func_name="state_to_string", .lua_c_api=&lua_state_to_string, .args={LSTRING}},
	{.lua_func_name="info", .lua_c_api=&lua_print_info, .args={LSTRING}},
	{.lua_func_name="message", .lua_c_api=&lua_print_message, .args={LSTRING}},
	{.lua_func_name="warning", .lua_c_api=&lua_print_warning, .args={LSTRING}},
	{.lua_func_name="error", .lua_c_api=&lua_print_error, .args={LSTRING}},
	{.lua_func_name="set_callback", .lua_c_api=&lua_set_callback, .args={LINT, LINT}},
	{.lua_func_name="create_debug_popup", .lua_c_api=&lua_create_debug_popup, .args={LSTRING}},
	{.lua_func_name="create_memory_popup", .lua_c_api=&lua_create_memory_popup, .args={LSTRING}},
	{.lua_func_name="create_source_popup", .lua_c_api=&lua_create_source_popup, .args={LSTRING}},
	{.lua_func_name="create_status_popup", .lua_c_api=&lua_create_status_popup, .args={LSTRING}},
	{.lua_func_name="create_var_popup", .lua_c_api=&lua_create_var_popup, .args={LSTRING}},
	{.lua_func_name="delete_popup", .lua_c_api=&lua_delete_popup, .args={LINT}},
	{.lua_func_name="set_memory_popup", .lua_c_api=&lua_set_memory_popup},
	{.lua_func_name="repaint_memory_popup", .lua_c_api=&lua_repaint_memory_popup},
	{.lua_func_name="print_to_debug_popup", .lua_c_api=&lua_print_to_debug_popup},
	{.lua_func_name="dump_to_debug_popup", .lua_c_api=&lua_dump_to_debug_popup},
	{.lua_func_name="get_string_param", .lua_c_api=&lua_get_string_param},
	{.lua_func_name="get_num_param", .lua_c_api=&lua_get_num_param},
	{.lua_func_name="get_bool_param", .lua_c_api=&lua_get_bool_param},
	{.lua_func_name="get_init_param", .lua_c_api=&lua_get_init_param},
	{.lua_func_name="get_hex_param", .lua_c_api=&lua_get_hex_param},
	{.lua_func_name="add_source_file", .lua_c_api=&lua_add_source_file},
	{.lua_func_name="systime", .lua_c_api=&lua_get_systime },
	{.lua_func_name="set_bus", .lua_c_api=&lua_set_bus },
	{.lua_func_name="get_bus", .lua_c_api=&lua_get_bus},
	{ NULL, NULL},
};

void lua_load_modules (IDSIMMODEL* this)
{
	if ( luaL_loadbufferx(this->luactx, module_bus_lua, module_bus_lua_len, "bus_class", NULL))
	{
		PRINT(this, "Failed to load module");
	}
	if ( luaL_loadbufferx(this->luactx, module_events_lua, module_events_lua_len, "events_class", NULL))
	{
		PRINT(this, "Failed to load module");
	}	
	if ( luaL_loadbufferx(this->luactx, device_lua, device_lua_len, "precompiled_device", NULL))
	{
		PRINT(this, "Failed to load module");
	}
	lua_pcall(this->luactx, 0, 0, 0);
}

static void
safe_execute ( lua_State* L, void* curfunc )
{
	/* Call trace */
	/*calltrace* new = NULL;
	HASH_FIND_INT ( this->trace, &curfunc, new );
	if ( !new )
	{
	    new = malloc ( sizeof *new );
	    new->func_addr = curfunc;
	    HASH_ADD_INT ( this->trace, func_addr, new );
	}*/
	/*=============*/
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	if ( !this->safe_mode )
		return;

	int argnum = lua_gettop ( L );
	for ( int i=0; lua_c_api_list[i].lua_func_name; i++ )
	{
		if ( curfunc == lua_c_api_list[i].lua_c_api )
		{
			int argcount=0;
			for ( argcount=0; lua_c_api_list[i].args[argcount]; argcount++ )
			{
				if ( argnum < argcount+1 )
				{
					lua_Debug ar;
					lua_getstack ( L, 1, &ar );
					lua_getinfo ( L, "nSl", &ar );
					int line = ar.currentline;

					print_error ( this, "Line %d: Too few arguments passed to the function \"%s\"", line, lua_c_api_list[i].lua_func_name );
				}
				else if ( !lua_c_api_list[i].args[argcount] ( L, argcount+1 ) )
				{
					lua_Debug ar;
					lua_getstack ( L, 1, &ar );
					lua_getinfo ( L, "nSl", &ar );
					int line = ar.currentline;
					print_error ( this, "Line %d: Argument %d of \"%s\" is of wrong type [%s]", line, argcount+1, lua_c_api_list[i].lua_func_name, lua_typename ( L, argcount+1 ) );
				}
			}
			if ( lua_c_api_list[i].args[argcount+2] )
			{
				lua_Debug ar;
				lua_getstack ( L, 1, &ar );
				lua_getinfo ( L, "nSl", &ar );
				int line = ar.currentline;
				print_error ( this, "Line %d: extra arguments passed to \"%s\"", line, argcount+1, lua_c_api_list[i].lua_func_name );
			}
		}
	}
}

void
register_functions ( IDSIMMODEL* this, lua_State* L )
{
	/*  Declare functions */
	for ( int i=0; lua_c_api_list[i].lua_func_name; i++ )
	{
		lua_pushcfunction ( L, lua_c_api_list[i].lua_c_api );
		lua_setglobal ( L, lua_c_api_list[i].lua_func_name );
	}
	/* Declare variables */
	for ( int i=0; lua_var_api_list[i].var_name ; i++ )
	{
		lua_pushinteger ( L, lua_var_api_list[i].var_value );
		lua_setglobal ( L, lua_var_api_list[i].var_name );
	}
}

bool
load_device_script ( IDSIMMODEL* this, const char* device_name )
{
	char spath[512] = {0};
	if ( 0 == GetEnvironmentVariable ( "LUAVSM", spath, sizeof spath ) )
	{
		print_error ( this, "LUAVSM environment variable is not set" );
		return false;
	}
	char* script=NULL;
	asprintf ( &script, "%s%s%s", spath, '\\' == spath[strlen ( spath )-1]? "":"\\", device_name );

	int lua_err = luaL_loadfile ( this->luactx, script );

	if ( 0 != lua_err )
	{
		const char* mess = NULL;
		switch ( lua_err )
		{
			case LUA_ERRSYNTAX:
				mess = lua_tostring ( this->luactx, -1 );
				print_error ( this, "Syntax error in Lua script\n%s", mess );
				return false;
			case LUA_ERRMEM:
				print_error ( this, "Not enough memory to load script \"%s\"", script );
				return false;
			case LUA_ERRFILE:
				print_error ( this, "Error loading script file \"%s\"", script );
				return false;
			default:
				print_error ( this, "Unknown error, shouldn't happen" );
				assert ( 0 );
		}
	}
	/* Primer run, if not run it - nothing works, need for parse */
	if ( 0 != lua_pcall ( this->luactx, 0, 0, 0 ) )
	{
		print_error ( this, "Failed to load the script \"%s\"", script );
		free ( script );
		return false;
	}
	free ( script );
	return true;
}

static int
lua_get_string_param ( lua_State* L )
{
	safe_execute ( L, &lua_get_string_param );
	char* str = ( char* ) lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushstring ( L, get_string_param ( this, str ) );
	return 1;
}

static int
lua_get_bool_param ( lua_State* L )
{
	safe_execute ( L, &lua_get_bool_param );
	char* str = ( char* ) lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushinteger ( L, get_bool_param ( this, str ) );
	return 1;
}

static int
lua_get_num_param ( lua_State* L )
{
	safe_execute ( L, &lua_get_num_param );
	char* str = ( char* ) lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushinteger ( L, get_num_param ( this, str )  );
	return 1;
}

static int
lua_get_hex_param ( lua_State* L )
{
	safe_execute ( L, &lua_get_hex_param );
	char* str = ( char* ) lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushinteger ( L, get_hex_param ( this,str ) );
	return 1;
}

static int
lua_get_init_param ( lua_State* L )
{
	safe_execute ( L, &lua_get_init_param );
	char* str = ( char* ) lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushinteger ( L, get_init_param ( this, str ) );
	return 1;
}

static int
lua_delete_popup ( lua_State* L )
{
	safe_execute ( L, &lua_delete_popup );
	ptrdiff_t id = lua_tointeger ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	delete_popup ( this, id );
	return 0;
}

static int
lua_create_debug_popup ( lua_State* L )
{
	safe_execute ( L, &lua_create_debug_popup );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushlightuserdata ( L, create_debug_popup ( this, text, ++this->popup_id ) );
	lua_pushinteger ( L, this->popup_id );
	return 2;
}

/**
* Prints a text string to debug popup
* @param L Lua state
* @return a pointer to popup and its ID
*/
static int
lua_print_to_debug_popup ( lua_State* L )
{
	safe_execute ( L, &lua_print_to_debug_popup );
	const char* str = lua_tostring ( L, -1 );
	void* popup = lua_touserdata ( L, -2 );
	print_to_debug_popup ( popup,  str );
	return 0;
}

/**
* Prints a text string to debug popup
* @param L Lua state
* @return a pointer to popup and its ID
*/
static int
lua_dump_to_debug_popup ( lua_State* L )
{
	safe_execute ( L, &lua_dump_to_debug_popup );
	lua_Number offset = luaL_checknumber ( L,-1 );
	lua_Number size = luaL_checknumber ( L,-2 );
	const char* buf = luaL_checkstring ( L,-3 );
	void* popup = lua_touserdata ( L, -4 );
	dump_to_debug_popup ( popup, ( BYTE* ) buf, offset, size );
	return 0;
}


static int
lua_create_source_popup ( lua_State* L )
{
	safe_execute ( L, &lua_create_source_popup );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushlightuserdata ( L, create_source_popup ( this, text, ++this->popup_id ) );
	lua_pushinteger ( L, this->popup_id );
	return 2;
}
static int
lua_create_status_popup ( lua_State* L )
{
	safe_execute ( L, &lua_create_status_popup );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushlightuserdata ( L, create_status_popup ( this, text, ++this->popup_id ) );
	lua_pushinteger ( L, this->popup_id );
	return 2;
}
static int
lua_create_var_popup ( lua_State* L )
{
	safe_execute ( L, &lua_create_var_popup );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushlightuserdata ( L, create_var_popup ( this, text, ++this->popup_id ) );
	lua_pushinteger ( L, this->popup_id );
	return 2;
}

static int
lua_create_memory_popup ( lua_State* L )
{
	safe_execute ( L, &lua_create_memory_popup );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	lua_pushlightuserdata ( L, create_memory_popup ( this, text, ++this->popup_id ) );
	lua_pushinteger ( L, this->popup_id );
	return 2;
}

static int
lua_set_memory_popup ( lua_State* L )
{
	safe_execute ( L, &lua_set_memory_popup );
	lua_Number size = luaL_checknumber ( L,-1 );
	const char* buf = luaL_checkstring ( L,-2 );
	void* popup = lua_touserdata ( L, -3 );
	set_memory_popup ( popup, 0, ( void* ) buf, size );

	return 0;
}

static int
lua_add_source_file ( lua_State* L )
{
	safe_execute ( L, &lua_add_source_file );
	if ( false == add_source_file ( lua_touserdata ( L, -3 ), ( char* ) lua_tostring ( L, -2 ), lua_tointeger ( L, -1 ) ) )
	{
		IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
		print_info ( this, "Fail to load source file" );
	}

	return 0;
}

static int
lua_repaint_memory_popup ( lua_State* L )
{
	safe_execute ( L, &lua_repaint_memory_popup );
	void* popup = lua_touserdata ( L, -1 ) ;
	repaint_memory_popup ( popup );
	return 0;
}

static int
lua_state_to_string ( lua_State* L )
{
	safe_execute ( L, &lua_state_to_string );
	ptrdiff_t state = lua_tointeger ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );

	lua_pushstring ( L,  state_to_string ( state ) );
	return 1;
}

static int
lua_print_info ( lua_State* L )
{
	safe_execute ( L, &lua_print_info );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	print_info ( this, text );
	return 0;
}

static int
lua_print_message ( lua_State* L )
{
	safe_execute ( L, &lua_print_message );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	print_message ( this, text );
	return 0;
}

static int
lua_print_warning ( lua_State* L )
{
	safe_execute ( L, &lua_print_warning );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	print_warning ( this, text );
	return 0;
}

static int
lua_print_error ( lua_State* L )
{
	safe_execute ( L, &lua_print_error );
	const char* text = lua_tostring ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	print_error ( this, text );
	return 0;
}

static int
lua_set_callback ( lua_State* L )
{
	safe_execute ( L, &lua_set_callback );
	lua_Number picotime = lua_tointeger ( L, -2 );
	lua_Number eventid = lua_tointeger ( L, -1 );
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	set_callback ( this, picotime, eventid );
	return 0;
}

static int
lua_get_systime ( lua_State* L )
{
	ABSTIME curtime = 0;
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	systime ( this, &curtime );
	lua_pushinteger ( L, curtime );
	return 1;
}

