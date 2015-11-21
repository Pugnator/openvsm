/**
 *
 * @file   vsm_api.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#include "vsmapi.h"

#ifndef __VERSION
#define __VERSION 0
#endif

IDSIMMODEL_vtable VSM_DEVICE_vtable =
{
	.isdigital      = vsm_isdigital,
	.setup          = vsm_setup,
	.runctrl        = vsm_runctrl,
	.actuate        = vsm_actuate,
	.indicate       = vsm_indicate,
	.simulate       = vsm_simulate,
	.callback       = vsm_timer_callback,
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

#ifdef _MSC_VER
__declspec ( dllexport )
#endif
IDSIMMODEL* __CDECL__
createdsimmodel ( char* device, ILICENCESERVER* ils )
{
	( void ) device;
	srand ( time ( 0 ) );
	if ( 0 == vsm_register ( ils ) )
	{
		return NULL;
	}
	
	IDSIMMODEL* vdev = malloc ( sizeof *vdev );
	/* Assign virtual table to the object */
	vdev->vtable = &VSM_DEVICE_vtable;
	/* Init Lua */
	vdev->luactx = luaL_newstate();
	/* Open Lua libraries */
	vdev->trace = NULL;
	vdev->popup_id = 0;
	luaL_openlibs ( vdev->luactx );
	register_functions ( vdev, vdev->luactx );
	return vdev;
}

#ifdef _MSC_VER
__declspec ( dllexport )
#endif
void __CDECL__
deletedsimmodel ( IDSIMMODEL* model )
{
	/* Close Lua */
	lua_close ( model->luactx );
	/* Remove device object */
	free ( model );
}

int32_t __FASTCALL__
vsm_isdigital ( IDSIMMODEL* this, uint32_t edx, char* pinname )
{
	( void ) this;
	( void ) edx;
	( void ) pinname;
	/* Always true at the moment */
	return 1;
}

void __FASTCALL__
pinhandler ( IDSIMPIN* pin, uint32_t edx )
{
	
}

/**
 * [Set up the model]
 * @param this  [description]
 */
void __FASTCALL__
vsm_setup ( IDSIMMODEL* this, uint32_t edx, IINSTANCE* instance, IDSIMCKT* dsimckt )
{
	( void ) edx;
	
	this->model_instance = instance;
	this->model_dsim = dsimckt;
	/*Init xor shift PRNG*/
	this->xorseed[0] = rand() << 31 | rand() & 0xFFFFFFFF;
	this->xorseed[1] = rand() << 31 | rand() & 0xFFFFFFFF;
	
	char* device_script = get_string_param ( this, "lua" );
	lua_load_modules ( this );
	/* If user uses precompiled device script - don't load external script */
	lua_getglobal ( this->luactx,"__USE_PRECOMPILED" );
	if ( lua_isinteger ( this->luactx,lua_gettop ( this->luactx ) ) )
	{
		print_info ( this, "%s started [OpenVSM %s, precompiled device script] %s", get_device_id ( this ), __VERSION, LUA_RELEASE );
	}
	else
	{
		load_device_script ( this, device_script );
		print_info ( this, "%s started [OpenVSM %s, %s] %s", get_device_id ( this ), __VERSION, device_script, LUA_RELEASE );
	}
	
	
	lua_getglobal ( this->luactx,"device_init" );
	if ( lua_isfunction ( this->luactx,lua_gettop ( this->luactx ) ) )
	{
		this->device_init_declared = true;
	}
	
	lua_getglobal ( this->luactx,"timer_callback" );
	if ( lua_isfunction ( this->luactx,lua_gettop ( this->luactx ) ) )
	{
		this->timer_callback_declared = true;
	}
	
	lua_getglobal ( this->luactx,"device_simulate" );
	if ( lua_isfunction ( this->luactx,lua_gettop ( this->luactx ) ) )
	{
		this->device_simulate_declared = true;
	}
	
	lua_getglobal ( this->luactx, "device_pins" );
	if ( 0 == lua_istable ( this->luactx, -1 ) )
	{
		print_error ( this, "Fatal error, no pin assignments found in script" );
		return;
	}
	lua_len ( this->luactx, -1 );
	int32_t pin_number = lua_tointeger ( this->luactx, -1 );
	if ( !pin_number )
	{
		print_warning ( this, "IC has no pins" );
	}
	lua_pop ( this->luactx, 1 );
	/* Pins initialization */
	for ( int i=1; i<=pin_number; i++ )
	{
		lua_rawgeti ( this->luactx,-1, i );
		//////////////
		//set pin   //
		//////////////
		lua_getfield ( this->luactx,-1, PIN_NAME );
		char* pin_name = ( char* ) lua_tostring ( this->luactx,-1 );
		char name_orig[64] = {0};
		strcpy ( name_orig, pin_name );
		/* Replace leading AND trailing underscore with $ sign */
		if ( '$' == pin_name[0] || '$' == pin_name[strlen ( pin_name )] )
		{
			memmove ( name_orig, name_orig + 1, sizeof name_orig - 1 );
			name_orig[strlen ( pin_name )-2]=0;
		}
		this->device_pins[i].pin = get_pin ( this, pin_name );
		strncpy ( this->device_pins[i].name, pin_name , sizeof *this->device_pins[i].name );
		lua_pop ( this->luactx, 1 );
		//////////////////////
		//set pin on time   //
		//////////////////////
		lua_getfield ( this->luactx,-1, PIN_ON_TIME );
		this->device_pins[i].on_time = lua_tointeger ( this->luactx,-1 );
		lua_pop ( this->luactx, 1 );
		///////////////////////
		//set pin off time   //
		///////////////////////
		lua_getfield ( this->luactx,-1, PIN_OFF_TIME );
		this->device_pins[i].off_time = lua_tointeger ( this->luactx,-1 );
		lua_pop ( this->luactx, 1 );
		/*
		 *  At this point we will create table, named after the pin name
		 *  that will contain pin index in device table and several methods
		 */
		register_pin_obj ( this->luactx, i, name_orig );
	}
	
	/* Check and set IC type (TTL/CMOS/I2L) */
	lua_getglobal ( this->luactx, "LOGIC_TYPE" );
	if ( 0 == lua_isinteger ( this->luactx, -1 ) )
	{
		print_warning ( this, "No or wrong IC type specified. TTL type will be set" );
		set_logic_type ( this, TTL );
	}
	else
	{
		int ltype = lua_tointeger ( this->luactx,-1 );
#ifdef __DEBUG
		print_info ( this, "IC type was set to %s", logic_type_to_string ( ltype ) );
#endif
		set_logic_type ( this, ltype );
	}
	
	/* Does safe mode should be enabled? */
	lua_getglobal ( this->luactx, "SAFE_MODE" );
	if ( lua_isboolean ( this->luactx, -1 ) )
	{
		this->safe_mode = lua_toboolean ( this->luactx, -1 );
#ifdef __DEBUG
		print_info ( this, "Safe mode was %s", this->safe_mode ? "enabled" : "disabled" );
#endif
	}
	else
	{
		this->safe_mode = false;
#ifdef __DEBUG
		print_info ( this, "Safe mode was disabled or incorrectly set" );
#endif
	}
	
	/* Pass model object pointer to Lua - it is safer there ;) */
	lua_pushliteral ( this->luactx, "__this" );
	lua_pushlightuserdata ( this->luactx, this );
	lua_settable ( this->luactx, LUA_REGISTRYINDEX );
	
#ifdef __DEBUG
	print_info ( this, "User defined functions: %s%s%s",
				 this->device_init_declared ? "device_init() " : "",
				 this->timer_callback_declared ? "timer_callback() " : "",
				 this->device_simulate_declared ? "device_simulate()" : "" );
#endif
				 
	/* Call device_init() function in Lua if it's exists */
	
	if ( this->device_init_declared )
	{
		lua_getglobal ( this->luactx, "device_init" );
		if ( lua_isfunction ( this->luactx,lua_gettop ( this->luactx ) ) )
		{
			if ( lua_pcall ( this->luactx, 0, 0, 0 ) )
			{
				const char* err = lua_tostring ( this->luactx, -1 );
				print_error ( this, "Error during device init: %s", err );
			}
		}
	}
	
	//this->device_pins[1].pin->vtable->sethandler(this->device_pins[1].pin, 0, this, &pinhandler);
	
}

void __FASTCALL__
vsm_runctrl (  IDSIMMODEL* this, uint32_t edx, RUNMODES mode )
{
	( void ) this;
	( void ) edx;
	( void ) mode;
	
	///FIXIT: Create full set of flags for each simulator step
	switch ( mode )
	{
		case RM_BATCH:
		
			break;
		case RM_START:
		
			break;
		case RM_STOP:
			break;
		case RM_SUSPEND:
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

void __FASTCALL__
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
 * @param this [description]
 * @param edx [description]
 * @param atime [description]
 * @param newstate [description]
 * @return [description]
 */
bool __FASTCALL__
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
 * @param this [description]
 * @param edx [description]
 * @param atime [description]
 * @param mode [description]
 */
void __FASTCALL__
vsm_simulate (  IDSIMMODEL* this, uint32_t edx, ABSTIME atime, DSIMMODES mode )
{
	( void ) edx;
	( void ) atime;
	( void ) mode;
#if 0
	/* C model speed benchmark as for NAND gate */
	set_pin_bool ( this, this->device_pins[3], ( 1- ( get_pin_bool ( this->device_pins[1] ) * get_pin_bool ( this->device_pins[2] ) ) ) );
#else
	if ( this->device_simulate_declared )
	{
		lua_getglobal ( this->luactx, "device_simulate" );
		if ( lua_isfunction ( this->luactx,lua_gettop ( this->luactx ) ) )
		{
			if ( lua_pcall ( this->luactx, 0, 0, 0 ) )
			{
				const char* err = lua_tostring ( this->luactx, -1 );
				print_error ( this, "Simulation failed with \"%s\"", err );
			}
		}
	}
#endif
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param this [description]
 * @param edx [description]
 * @param atime [description]
 * @param eventid [description]
 */
void __FASTCALL__
vsm_timer_callback (  IDSIMMODEL* this, uint32_t edx, ABSTIME atime, EVENTID eventid )
{
	( void ) edx;
	
	/* Ignore Lua call if it not declared in the script */
	if ( this->timer_callback_declared )
	{
		/*callback_events* curevent = NULL;
		HASH_FIND_INT ( this->events, &eventid, curevent );
		if ( curevent )
		{*/
		lua_getglobal ( this->luactx, "timer_callback" );
		if ( lua_isfunction ( this->luactx,lua_gettop ( this->luactx ) ) )
		{
			lua_pushinteger ( this->luactx, atime );
			lua_pushinteger ( this->luactx, eventid );
			if ( lua_pcall ( this->luactx, 2, 0, 0 ) )
			{
				const char* err = lua_tostring ( this->luactx, -1 );
				print_error ( this, "Timer callback failed with \"%s\"", err );
			}
		}
		//}
	}
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param hInstDLL [description]
 * @param fdwReason [description]
 * @param thispvReserved [description]
 * @return [description]
 */
#ifndef __MINGW32__
bool APIENTRY
DllMain ( HINSTANCE hInstDLL, uint32_t fdwReason, LPVOID lpvReserved )
{
	( void ) hInstDLL;
	( void ) fdwReason;
	( void ) lpvReserved;
	return true;
}
#endif

LRESULT __FASTCALL__ icpu_vdmhlr (  ICPU* this, uint32_t edx, VDM_COMMAND* cmd, uint8_t* data )
{
	( void ) this;
	( void ) edx;
	( void ) cmd;
	( void ) data;
	return 0;
}

void __FASTCALL__ icpu_loaddata ( ICPU* this, uint32_t edx, int32_t format, int32_t seg, ADDRESS address, uint8_t* data, int32_t numbytes )
{
	( void ) this;
	( void ) edx;
	( void ) format;
	( void ) seg;
	( void ) address;
	( void ) data;
	( void ) numbytes;
}

void __FASTCALL__ icpu_disassemble ( ICPU* this, uint32_t edx, ADDRESS address, int32_t numbytes )
{
	( void ) this;
	( void ) edx;
	( void ) address;
	( void ) numbytes;
}

bool __FASTCALL__ icpu_getvardata ( ICPU* this, uint32_t edx, VARITEM* vip, VARDATA* vdp )
{
	( void ) this;
	( void ) edx;
	( void ) vip;
	( void ) vdp;
	return true;
}
