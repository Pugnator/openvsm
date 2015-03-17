/**
 *
 * @file   vdm_api.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#ifndef VDMAPI_GLOBAL
#define VDMAPI_GLOBAL

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "vsmapi.h"
#include "defines.h"
#include "enums.h"
#include "vsmclasses.h"

// VDM Protocol command strcuture:
typedef struct VDM_COMMAND
{
	BYTE command;
	BYTE memspace;
	DWORD address;
	DWORD datalength;
} VDM_COMMAND;


#endif
