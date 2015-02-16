/**
 *
 * @file   vsm_api.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @brief  The main C wrapper of official API
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

ICPU_vtable ICPU_DEVICE_vtable =
{
	.vdmhlr = icpu_vdmhlr,
	.loaddata = icpu_loaddata,
	.disassemble = icpu_disassemble,
	.getvardata = icpu_getvardata,
};

ICPU ICPU_DEVICE =
{
	.vtable = &ICPU_DEVICE_vtable,	
};

typedef struct lua_global_func
{
	char* func_name;
	bool* exist;
} lua_global_func;

bool global_device_init = false;
bool global_device_simulate = false;
bool global_timer_callback = false;
bool global_on_stop = false;
bool global_on_suspend = false;

static lua_global_func lua_global_func_list[] =
{
	{.func_name="device_init", .exist=&global_device_init},
	{.func_name="device_simulate", .exist=&global_device_simulate},
	{.func_name="timer_callback", .exist=&global_timer_callback},
	{.func_name="on_stop", .exist=&global_on_stop},
	{.func_name="on_suspend", .exist=&global_on_suspend},
	{.func_name=0},
};

static void check_global_functions ( IDSIMMODEL* model )
{
	for ( int i=0; lua_global_func_list[i].func_name; i++ )
	{
		lua_getglobal (model->luactx,lua_global_func_list[i].func_name );
		if ( lua_isfunction (model->luactx,lua_gettop (model->luactx ) ) )
			*lua_global_func_list[i].exist = true;
	}
}

IDSIMMODEL* __cdecl
createdsimmodel ( char* device, ILICENCESERVER* ils )
{
	( void ) device;
	if ( 0 == vsm_register ( ils ) )
	{
		return NULL;
	}	
	
	IDSIMMODEL *vdev = malloc(sizeof *vdev);
	/* Assign virtual table to the object */
	vdev->vtable = &VSM_DEVICE_vtable;
	/* Init Lua */
	vdev->luactx = luaL_newstate();
	/* Open Lua libraries */
	luaL_openlibs (vdev->luactx );
	register_functions (vdev, vdev->luactx );
	return vdev;
}

void __cdecl
deletedsimmodel ( IDSIMMODEL* model )
{
	( void ) model;
	/* Close Lua */
	//lua_close (model->luactx );
	/* Remove device object */
	//free(model);
}

int32_t __attribute__ ( ( fastcall ) )
vsm_isdigital ( IDSIMMODEL* this, uint32_t edx, char* pinname )
{
	( void ) this;
	( void ) edx;
	( void ) pinname;
	/* Always true at the moment */
	/**TODO*/
	return 1;
}

void __attribute__ ( ( fastcall ) )
vsm_setup ( IDSIMMODEL* this, uint32_t edx, IINSTANCE* instance, IDSIMCKT* dsimckt )
{	
	( void ) edx;	
	
	this->model_instance = instance;
	this->model_dsim = dsimckt;

	char* moddll = get_string_param ( this, "moddll" );
	char luascript[MAX_PATH]= {0};
	/**FIXIT: Change it to have format DeviceDLL.lua without .dll.*/
	snprintf ( luascript, sizeof luascript, "%s.lua", moddll );
	if(load_device_script ( this, luascript )) ///Model name
	{
		out_log ( this, "%s started [OpenVSM %s]", get_device_id(this), __VERSION);		
	}

	free ( moddll );

	lua_getglobal ( this->luactx, "device_pins" );
	if ( 0 == lua_istable (this->luactx, -1 ) )
	{
		out_error ( this, "No device model found, it is fatal error" );
		return;
	}

	lua_len (this->luactx, -1 );
	int32_t pin_number = lua_tointeger (this->luactx, -1 );
	lua_pop (this->luactx, 1 );

	for ( int i=1; i<=pin_number; i++ )
	{
		lua_rawgeti (this->luactx,-1, i );
		//////////////
		//set pin //
		//////////////
		lua_getfield (this->luactx,-1, PIN_NAME );
		const char* pin_name = lua_tostring (this->luactx,-1 );
		this->device_pins[i].pin = get_pin ( this, ( char* ) pin_name );
		lua_pop (this->luactx, 1 );
		//////////////////////
		//set pin on time //
		//////////////////////
		lua_getfield (this->luactx,-1, PIN_ON_TIME );
		this->device_pins[i].on_time = lua_tonumber (this->luactx,-1 );
		lua_pop (this->luactx, 1 );
		///////////////////////
		//set pin off time //
		///////////////////////
		lua_getfield (this->luactx,-1, PIN_OFF_TIME );
		this->device_pins[i].off_time = lua_tonumber (this->luactx,-1 );
		lua_pop (this->luactx, 1 );
		/////////////////////////////////////////////////////////////
		//Set global variable that holds pin name and its number //
		/////////////////////////////////////////////////////////////
		lua_pushinteger (this->luactx, i );
		lua_setglobal (this->luactx, pin_name );
		lua_pop (this->luactx, 1 );
	}
	
	check_global_functions(this);
	if ( global_device_init )
	{
		lua_getglobal ( this->luactx, "device_init" );
		/* First argument - the model itself (this*) */
		lua_pushlightuserdata ( this->luactx, this );
		lua_pcall ( this->luactx, 1, 0, 0 );	
	}
}

void __attribute__ ( ( fastcall ) )
vsm_runctrl (  IDSIMMODEL* this, uint32_t edx, RUNMODES mode )
{
	( void ) this;
	( void ) edx;
	( void ) mode;

	switch ( mode )
	{
		case RM_BATCH:

			break;
		case RM_START:

			break;
		case RM_STOP:
			/*if ( global_on_stop )
				lua_run_function ( this, "on_stop" );*/
			break;
		case RM_SUSPEND:
			/*if ( global_on_suspend )
				lua_run_function ( this, "on_suspend" );*/
			break;
		case RM_ANIMATE:
			break;
		case RM_STEPTIME:

			break;
		case RM_STEPOVER:

			break;
		case RM_STEPINTO:

			break;
		case RM_STEPOUT:

			break;
		case RM_STEPTO:

			break;
		case RM_META:

			break;
		case RM_DUMP:

			break;
	}

}

void __attribute__ ( ( fastcall ) )
vsm_actuate  (  IDSIMMODEL* this, uint32_t edx, REALTIME atime, ACTIVESTATE newstate )
{
	( void ) this;
	( void ) edx;
	( void ) atime;
	( void ) newstate;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param l [description]
 * @param edx [description]
 * @param atime [description]
 * @param newstate [description]
 * @return [description]
 */
bool __attribute__ ( ( fastcall ) )
vsm_indicate (  IDSIMMODEL* this, uint32_t edx, REALTIME atime, ACTIVEDATA* newstate )
{
	( void ) this;
	( void ) edx;
	( void ) atime;
	( void ) newstate;
	return false;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param l [description]
 * @param edx [description]
 * @param atime [description]
 * @param mode [description]
 */
void __attribute__ ( ( fastcall ) )
vsm_simulate (  IDSIMMODEL* this, uint32_t edx, ABSTIME atime, DSIMMODES mode )
{
	( void ) this;
	( void ) edx;
	( void ) atime;
	( void ) mode;	

	if ( global_device_simulate )
	{		
		lua_getglobal ( this->luactx, "device_simulate" );
		/* First argument - the model itself (this*) */
		lua_pushlightuserdata ( this->luactx, this );
		lua_pcall ( this->luactx, 1, 0, 0 );		
	}
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param l [description]
 * @param edx [description]
 * @param atime [description]
 * @param eventid [description]
 */
void __attribute__ ( ( fastcall ) )
vsm_callback (  IDSIMMODEL* this, uint32_t edx, ABSTIME atime, EVENTID eventid )
{	
	( void ) edx;	

	if ( false == global_timer_callback )
	{
		return;	
	}

	lua_getglobal (this->luactx, "timer_callback" );
	/* First argument - the model itself (this*) */
	lua_pushlightuserdata ( this->luactx, this );
	lua_pushunsigned (this->luactx, atime );
	lua_pushunsigned (this->luactx, eventid );
	lua_pcall (this->luactx, 3, 0, 0 );
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param hInstDLL [description]
 * @param fdwReason [description]
 * @param lpvReserved [description]
 * @return [description]
 */
bool APIENTRY
DllMain ( HINSTANCE hInstDLL, uint32_t fdwReason, LPVOID lpvReserved )
{
	( void ) hInstDLL;
	( void ) fdwReason;
	( void ) lpvReserved;	 	
	return true;
}

LRESULT __attribute__ ( ( fastcall ) ) icpu_vdmhlr (  ICPU* this, uint32_t edx, VDM_COMMAND* cmd, uint8_t* data )
{
	( void ) this;
	( void ) edx;
	( void ) cmd;
	( void ) data;
	return 0;
}

void __attribute__ ( ( fastcall ) ) icpu_loaddata ( ICPU* this, uint32_t edx, int32_t format, int32_t seg, ADDRESS address, uint8_t* data, int32_t numbytes )
{
	( void ) this;
	( void ) edx;
	( void ) format;
	( void ) seg;
	( void ) address;
	( void ) data;
	( void ) numbytes;
}

void __attribute__ ( ( fastcall ) ) icpu_disassemble ( ICPU* this, uint32_t edx, ADDRESS address, int32_t numbytes )
{
	( void ) this;
	( void ) edx;
	( void ) address;
	( void ) numbytes;
}

bool __attribute__ ( ( fastcall ) ) icpu_getvardata ( ICPU* this, uint32_t edx, VARITEM* vip, VARDATA* vdp )
{
	( void ) this;
	( void ) edx;
	( void ) vip;
	( void ) vdp;
	return true;
}
