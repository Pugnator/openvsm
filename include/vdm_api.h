/**
 *
 * @file   vdm_api.h
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

#ifndef VDMAPI_GLOBAL
#define VDMAPI_GLOBAL

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "vsm_api.h"
#include "defines.h"
#include "enums.h"
#include "vsm_classes.h"

// VDM Protocol command strcuture:
typedef struct VDM_COMMAND
{
	BYTE command;
	BYTE memspace;
	DWORD address;
	DWORD datalength;
} VDM_COMMAND;


#endif
