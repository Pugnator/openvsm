#include <vsm_api.h>
IPOPUP *memory_popup=NULL;
IPOPUP *debug_popup=NULL;
IPOPUP *source_popup=NULL;
IPOPUP *status_popup=NULL;
IPOPUP *var_popup=NULL;
lua_State *luactx = NULL;

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

IDSIMMODEL * __cdecl createdsimmodel (CHAR *device, ILICENCESERVER *ils )
{
	(void) device;
	if(0 == vsm_register(ils))
	{
		return NULL;
	}
	/* Init Lua */
	luactx = luaL_newstate();
	/* Open libraries */
    luaL_openlibs(luactx);
    register_functions(luactx);
    
	return &VSM_DEVICE;
}

VOID __cdecl deletedsimmodel ( IDSIMMODEL *model)
{
	(void) model;
	/* Close Lua */
    lua_close(luactx);
}

INT __attribute__((fastcall)) vsm_isdigital ( IDSIMMODEL *this, DWORD edx, CHAR *pinname)
{
	(void) this;
	(void) edx;
	(void) pinname;
	return 1;
}

VOID __attribute__((fastcall)) vsm_setup ( IDSIMMODEL *this, DWORD edx, IINSTANCE *instance, IDSIMCKT *dsimckt)
{
	(void) this;
	(void) edx;
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

	lua_load_script("test");
	lua_getglobal (luactx, "device_pins");
	if (0 == lua_istable(luactx, -1))
	{
	    out_log("No device model found, it is fatal error");
	    return;
	}

	for (int i=1;;i++)
	{    
	    lua_rawgeti(luactx,-1, i);
	    if (lua_isnil(luactx,-1)) 
	    	break;
	    lua_getfield(luactx,-1, PIN_NAME);
	    const char *pin_name = lua_tostring(luactx,-1);
	    device_pins[i].pin = get_pin((char *)pin_name);	       
	    //Set global variable that holds pin name and its number
	    lua_pushinteger(luactx, i);
	    lua_setglobal(luactx, pin_name);
	    lua_pop(luactx, 2);
	}
	out_log("OpenVSM model loaded, engine version 0.1a");
	lua_run_function("device_init");
}

VOID __attribute__((fastcall)) vsm_runctrl (  IDSIMMODEL *this, DWORD edx, RUNMODES mode)
{
	(void) this;
	(void) edx;

	switch(mode)
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

VOID __attribute__((fastcall)) vsm_actuate  (  IDSIMMODEL *this, DWORD edx, REALTIME time, ACTIVESTATE newstate)
{
	(void) this;
	(void) edx;
	(void) time;
	(void) newstate;
}

BOOL __attribute__((fastcall)) vsm_indicate (  IDSIMMODEL *this, DWORD edx, REALTIME time, ACTIVEDATA *newstate)
{
	(void) this;
	(void) edx;
	(void) time;
	(void) newstate;
	return FALSE;
}

VOID __attribute__((fastcall)) vsm_simulate (  IDSIMMODEL *this, DWORD edx, ABSTIME time, DSIMMODES mode)
{
	(void) this;
	(void) edx;
	(void) time;
	(void) mode;
	lua_run_function("device_simulate");
	device_simulate();
}

VOID __attribute__((fastcall)) vsm_callback (  IDSIMMODEL *this, DWORD edx, ABSTIME time, EVENTID eventid)
{
	(void) this;
	(void) edx;
	(void) time;
	lua_run_function("timer_callback");

	switch(eventid)
	{
	default:
	break;
	}
}

BOOL APIENTRY DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	(void) hInstDLL; /* Unused param */
	(void) lpvReserved; /* Unused param */

	switch (fdwReason)
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
		if(CONSOLE_ALLOCATED)
		{
			//debug_console_free();
		}

		break;
	}
	return TRUE;
}
