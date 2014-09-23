/**
 *
 * @file   vsm_api.c
 * @Author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @brief  Brief description of file.
 *
 * This file is part of OpenVSM.
 * OpenVSM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenVSM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenVSM.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "vsm_api.h"

lua_State* luactx = NULL;
VSM_PIN device_pins[16];

IDSIMMODEL_vtable VSM_DEVICE_vtable =
{
	.isdigital      = vsm_isdigital,
	.setup          = vsm_setup,
	.runctrl        = vsm_runctrl,
	.actuate        = vsm_actuate,
	.indicate       = vsm_indicate,
	.simulate       = vsm_simulate,
	.callback       = vsm_callback,
};

IDSIMMODEL VSM_DEVICE =
{
	.vtable = &VSM_DEVICE_vtable,
};

IDSIMMODEL* __cdecl
createdsimmodel ( CHAR* device, ILICENCESERVER* ils )
{
	( void ) device;
	if ( 0 == vsm_register ( ils ) )
	{
		return NULL;
	}
	/* Init Lua */
	luactx = luaL_newstate();
	/* Open libraries */
	luaL_openlibs ( luactx );
	register_functions ( luactx );
	
	return &VSM_DEVICE;
}

VOID __cdecl
deletedsimmodel ( IDSIMMODEL* model )
{
	( void ) model;
	/* Close Lua */
	lua_close ( luactx );
	free ( memory_popup_buf );
	free ( debug_popup_buf );
}

INT __attribute__ ( ( fastcall ) )
vsm_isdigital ( IDSIMMODEL* this, DWORD edx, CHAR* pinname )
{
	( void ) this;
	( void ) edx;
	( void ) pinname;
	return 1;
}

VOID __attribute__ ( ( fastcall ) )
vsm_setup ( IDSIMMODEL* this, DWORD edx, IINSTANCE* instance, IDSIMCKT* dsimckt )
{
	( void ) this;
	( void ) edx;
	model_instance = instance;
	model_dsim = dsimckt;
	//Here I should decompose two possible scenarios:
	//1) Device is inited in C
	//Device is inited in Lua
	/*
	for (int i=0; device_pins[i].name; i++)
	{
	    //device_pins[i].pin = get_pin(device_pins[i].name);
	}
	*/
	char* moddll = get_string_param ( "moddll" );
	free ( moddll );
	
	lua_load_script ( "gps" ); ///Model name
	lua_getglobal ( luactx, "device_pins" );
	if ( 0 == lua_istable ( luactx, -1 ) )
	{
		out_log ( "No device model found, it is fatal error" );
		return;
	}
	
	lua_len ( luactx, -1 );
	int32_t pin_number = lua_tointeger ( luactx, -1 );
	lua_pop ( luactx, 1 );
	
	for ( int i=1; i<=pin_number; i++ )
	{
		lua_rawgeti ( luactx,-1, i );
		//set pin
		lua_getfield ( luactx,-1, PIN_NAME );
		const char* pin_name = lua_tostring ( luactx,-1 );
		device_pins[i].pin = get_pin ( ( char* ) pin_name );
		lua_pop ( luactx, 1 );
		//set pin on time
		lua_getfield ( luactx,-1, PIN_ON_TIME );
		device_pins[i].on_time = lua_tonumber ( luactx,-1 );
		lua_pop ( luactx, 1 );
		//set pin off time
		lua_getfield ( luactx,-1, PIN_OFF_TIME );
		device_pins[i].off_time = lua_tonumber ( luactx,-1 );
		lua_pop ( luactx, 1 );
		//Set global variable that holds pin name and its number
		lua_pushinteger ( luactx, i );
		lua_setglobal ( luactx, pin_name );
		lua_pop ( luactx, 1 );
	}
	out_log ( "OpenVSM model loaded, engine version 0.1a" );
	lua_run_function ( "device_init" );
	
}

VOID __attribute__ ( ( fastcall ) )
vsm_runctrl (  IDSIMMODEL* this, DWORD edx, RUNMODES mode )
{
	( void ) this;
	( void ) edx;
	
	switch ( mode )
	{
		case RM_BATCH:
			lua_run_function ( "RM_BATCH" );
			break;
		case RM_START:
			lua_run_function ( "RM_START" );
			break;
		case RM_STOP:
			lua_run_function ( "RM_STOP" );
			break;
		case RM_SUSPEND:
			lua_run_function ( "RM_SUSPEND" );
			break;
		case RM_ANIMATE:
			lua_run_function ( "RM_ANIMATE" );
			break;
		case RM_STEPTIME:
			lua_run_function ( "RM_STEPTIME" );
			break;
		case RM_STEPOVER:
			lua_run_function ( "RM_STEPOVER" );
			break;
		case RM_STEPINTO:
			lua_run_function ( "RM_STEPINTO" );
			break;
		case RM_STEPOUT:
			lua_run_function ( "RM_STEPOUT" );
			break;
		case RM_STEPTO:
			lua_run_function ( "RM_STEPTO" );
			break;
		case RM_META:
			lua_run_function ( "RM_META" );
			break;
		case RM_DUMP:
			lua_run_function ( "RM_DUMP" );
			break;
	}
}

VOID __attribute__ ( ( fastcall ) )
vsm_actuate  (  IDSIMMODEL* this, DWORD edx, REALTIME time, ACTIVESTATE newstate )
{
	( void ) this;
	( void ) edx;
	( void ) time;
	( void ) newstate;
}

BOOL __attribute__ ( ( fastcall ) )
vsm_indicate (  IDSIMMODEL* this, DWORD edx, REALTIME time, ACTIVEDATA* newstate )
{
	( void ) this;
	( void ) edx;
	( void ) time;
	( void ) newstate;
	return FALSE;
}

VOID __attribute__ ( ( fastcall ) )
vsm_simulate (  IDSIMMODEL* this, DWORD edx, ABSTIME time, DSIMMODES mode )
{
	( void ) this;
	( void ) edx;
	( void ) time;
	( void ) mode;
	lua_run_function ( "device_simulate" );
	device_simulate();
}

VOID __attribute__ ( ( fastcall ) )
vsm_callback (  IDSIMMODEL* this, DWORD edx, ABSTIME time, EVENTID eventid )
{
	( void ) this;
	( void ) edx;
	( void ) time;
	/// Pass event id to lua
	lua_getglobal ( luactx, "timer_callback" );
	lua_pushinteger ( luactx, eventid );
	lua_pcall ( luactx, 1, 0, 0 );
	
	switch ( eventid )
	{
		default:
			break;
	}
}
/*
ICPU_vtable ICPU_DEVICE_vtable =
{
    .vdmhlr = icpu_vdmhlr,
    .loaddata = NULL,
    .disassemble = NULL,
    .getvardata = NULL,
};

ICPU ICPU_DEVICE =
{
    .vtable = &ICPU_DEVICE_vtable,
};

LRESULT __attribute__ ( ( fastcall ) ) icpu_vdmhlr (  ICPU* this, DWORD edx, VDM_COMMAND* cmd, BYTE* data )
{
    ( void ) this;
    ( void ) edx;
    ( void ) cmd;
    ( void ) data;
    return 0;
}
*/
BOOL APIENTRY
DllMain ( HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	( void ) hInstDLL; /* Unused param */
	( void ) lpvReserved; /* Unused param */
	
	switch ( fdwReason )
	{
		case DLL_PROCESS_ATTACH:
			//debug_console_alloc();
			break;
		case DLL_THREAD_ATTACH:
			//debug_console_alloc();
			break;
		case DLL_THREAD_DETACH:
			//debug_console_free();
			break;
		case DLL_PROCESS_DETACH:
		
			break;
	}
	return TRUE;
}
