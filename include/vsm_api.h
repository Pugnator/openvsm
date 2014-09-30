/**
 *
 * @file   vsm_api.h
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

#ifndef VSMAPI_H
#define VSMAPI_H

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <defines.h>
#include <enums.h>
#include <vsm_classes.h>
#include <device.h>
#include <c_bind.h>
#include <lua_bind.h>

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#define VSM_API_VERSION  110
#define model_key 0x00000000

extern bool private_device_init;
extern bool private_device_simulate;
extern bool private_timer_callback;
extern bool private_on_stop;
extern bool private_on_suspend;

#ifdef __WIN32
int32_t vasprintf ( char** sptr, char* fmt, va_list argv );
int32_t asprintf ( char** sptr, char* fmt, ... );
#endif

INT __attribute__ ( ( fastcall ) )
vsm_isdigital ( IDSIMMODEL* this, DWORD edx, CHAR* pinname );
VOID __attribute__ ( ( fastcall ) )
vsm_setup ( IDSIMMODEL* this, DWORD edx, IINSTANCE* instance, IDSIMCKT* dsim );
VOID __attribute__ ( ( fastcall ) )
vsm_runctrl (  IDSIMMODEL* this, DWORD edx, RUNMODES mode );
VOID __attribute__ ( ( fastcall ) )
vsm_actuate  (  IDSIMMODEL* this, DWORD edx, REALTIME atime, ACTIVESTATE newstate );
BOOL __attribute__ ( ( fastcall ) )
vsm_indicate (  IDSIMMODEL* this, DWORD edx, REALTIME atime, ACTIVEDATA* newstate );
VOID __attribute__ ( ( fastcall ) )
vsm_simulate (  IDSIMMODEL* this, DWORD edx, ABSTIME atime, DSIMMODES mode );
VOID __attribute__ ( ( fastcall ) )
vsm_callback (  IDSIMMODEL* this, DWORD edx, ABSTIME atime, EVENTID eventid );

LRESULT __attribute__ ( ( fastcall ) )
icpu_vdmhlr (  ICPU* this, DWORD edx, VDM_COMMAND* cmd, BYTE* data );
VOID __attribute__ ( ( fastcall ) )
icpu_loaddata ( ICPU* this, EDX, INT format, INT seg, ADDRESS address, BYTE* data, INT numbytes );
VOID __attribute__ ( ( fastcall ) ) icpu_disassemble ( ICPU* this, EDX, ADDRESS address, INT numbytes );
BOOL __attribute__ ( ( fastcall ) ) icpu_getvardata ( ICPU* this, EDX, VARITEM* vip, VARDATA* vdp );

struct SPICEDATA
{
	DWORD numtimepoints;
	DWORD numpins;
	REALTIME* timepoints;
	DOUBLE* nodedata;
};

struct DSIMDATA
{
	DWORD numtimepoints;
	DWORD numpins;
	ABSTIME* timepoints;
	STATE* nodedata;
};

struct CREATEPOPUPSTRUCT
{
	POPUPID id;
	POPUPTYPES type;
	CHAR* caption;
	INT width, height;
	DWORD flags;
};

struct ACTIVEDATA
{
	ACTIVEDATATYPES type;
	union
	{
		INT intval;
		DOUBLE realval;
		STATE stateval;
		DOUBLE wireinfo[2]; // Voltage and current
		SPICEDATA spicedata;
		DSIMDATA dsimdata;
		VOID* userinfo; // Pointer to user data
	};
};

struct VARITEM
{
	CHAR name[WATCHITEM_NAME_SIZE];
	DWORD loader, seg;
	ADDRESS address;
	DATATYPES type;
	DISPFORMATS format;
	DWORD size;
	ADDRESS scope_begin;
	ADDRESS scope_end;
};

// Variable Data structure
struct VARDATA
{
	CHAR addr[WATCHITEM_ADDR_SIZE];
	DATATYPES type;
	BYTE* memory;
	DWORD memsize;
	DWORD offset; // from start of memory block.
};

struct ILICENCESERVER
{

	ILICENCESERVER_vtable* vtable;
	
};

struct EVENT
{
	EVENT_vtable* vtable;
};

struct ICPU
{
	ICPU_vtable* vtable;
	
};

struct IDSIMPIN2
{
	IDSIMPIN2_vtable* vtable;
};

struct IDSIMPIN1
{

	IDSIMPIN1_vtable* vtable;
	
};

struct IDSIMMODEL
{

	IDSIMMODEL_vtable* vtable;
};

struct IBUSPIN
{

	ICPU_vtable* vtable;
	
};

struct IINSTANCE
{

	IINSTANCE_vtable* vtable;
	
};

struct IDSIMCKT
{

	IDSIMCKT_vtable* vtable;
	
};

struct IUSERPOPUP
{

	IUSERPOPUP_vtable* vtable;
	
};

struct IDEBUGPOPUP
{

	IDEBUGPOPUP_vtable* vtable;
	
};

struct IMEMORYPOPUP
{

	IMEMORYPOPUP_vtable* vtable;
	
};

struct ISTATUSPOPUP
{

	ISTATUSPOPUP_vtable* vtable;
	
};

struct ISOURCEPOPUP
{

	ISOURCEPOPUP_vtable* vtable;
	
};

struct IMSGHLR
{
	IMSGHLR_vtable* vtable;
};

IDSIMMODEL_vtable VSM_DEVICE_vtable;
IDSIMMODEL VSM_DEVICE;

ICPU_vtable ICPU_DEVICE_vtable;
ICPU ICPU_DEVICE;

#endif

