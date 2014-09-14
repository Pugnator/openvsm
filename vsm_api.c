#include <vsm_api.h>

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
	return &VSM_DEVICE;
}

VOID __cdecl deletedsimmodel ( IDSIMMODEL *model)
{
	(void) model;
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

	for (int i=0; device_pins[i].name; i++)
	{
		device_pins[i].pin = get_pin(device_pins[i].name);
	}

	lua_execute_script("", "");

	//set_callback(1000000000000, INC_PC);
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
	device_simulate();

}

VOID __attribute__((fastcall)) vsm_callback (  IDSIMMODEL *this, DWORD edx, ABSTIME time, EVENTID eventid)
{
	(void) this;
	(void) edx;
	(void) time;
	switch(eventid)
	{
	// case INC_PC:
	// {
	// 	if(CPU_RESET == zstate)
	// 	{
	// 		cpu_reset();
	// 		return;
	// 	}
	// 	else
	// 	{
	// 		zstate = CPU_RUN;
	// 	}

	// 	address_bus.address++;
	// 	set_address_bus();
	// }
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
		debug_console_alloc();
		break;
	case DLL_THREAD_ATTACH:
		debug_console_alloc();
		break;
	case DLL_THREAD_DETACH:
		debug_console_free();
		break;
	case DLL_PROCESS_DETACH:
		if(CONSOLE_ALLOCATED)
		{
			debug_console_free();
		}

		break;
	}
	return TRUE;
}
