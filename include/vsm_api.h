#ifndef VSMAPI_H
#define VSMAPI_H

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <defines.h>
#include <enums.h>
#include <vsm_classes.h>
#include <device.h>
#include <c_bind.h>
#include <lua_bind.h>
#include <panel.h>
//#include <tui.h>

#define VSM_API_VERSION  110
#define model_key 0x00000000

extern uint8_t *memory_popup_buf;

int32_t vasprintf ( char** sptr, char* fmt, va_list argv );
int32_t asprintf ( char** sptr, char* fmt, ... );

INT __attribute__ ( ( fastcall ) ) vsm_isdigital ( IDSIMMODEL* this, DWORD edx, CHAR* pinname );
VOID __attribute__ ( ( fastcall ) ) vsm_setup ( IDSIMMODEL* this, DWORD edx, IINSTANCE* instance, IDSIMCKT* dsim );
VOID __attribute__ ( ( fastcall ) ) vsm_runctrl (  IDSIMMODEL* this, DWORD edx, RUNMODES mode );
VOID __attribute__ ( ( fastcall ) ) vsm_actuate  (  IDSIMMODEL* this, DWORD edx, REALTIME time, ACTIVESTATE newstate );
BOOL __attribute__ ( ( fastcall ) ) vsm_indicate (  IDSIMMODEL* this, DWORD edx, REALTIME time, ACTIVEDATA* newstate );
VOID __attribute__ ( ( fastcall ) ) vsm_simulate (  IDSIMMODEL* this, DWORD edx, ABSTIME time, DSIMMODES mode );
VOID __attribute__ ( ( fastcall ) ) vsm_callback (  IDSIMMODEL* this, DWORD edx, ABSTIME time, EVENTID eventid );

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

struct IMSGHLR
{
	IMSGHLR_vtable* vtable;
};

IDSIMMODEL_vtable VSM_DEVICE_vtable;
IDSIMMODEL VSM_DEVICE;

#endif

