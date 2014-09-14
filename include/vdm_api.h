#ifndef VDMAPI_GLOBAL
#define VDMAPI_GLOBAL

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vsm_api.h>
#include <defines.h>
#include <enums.h>
#include <vsm_classes.h>

// VDM Protocol command strcuture:
typedef struct VDM_COMMAND
{
	BYTE command;
	BYTE memspace;
	DWORD address;
	DWORD datalength;
}VDM_COMMAND;


#endif
