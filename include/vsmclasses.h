/**
 *
 * @file   vsm_classes.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */
#pragma once
#ifndef VSM_CLASSES_H
#define VSM_CLASSES_H
#include <vsmapi.h>
#include <vdmapi.h>

#define EDX uint32_t __EDX

#define IACTIVEMODEL void*
//#define EVENT void*
//#define ACTIVEDATA void*

typedef struct SPICEDATA SPICEDATA;
typedef struct DSIMDATA DSIMDATA;
typedef struct CREATEPOPUPSTRUCT CREATEPOPUPSTRUCT;
typedef struct ACTIVEDATA ACTIVEDATA;
typedef struct VARITEM VARITEM;
typedef struct VARDATA VARDATA;

/*************************************************************************/

typedef struct IINSTANCE IINSTANCE;
typedef struct IDSIMPIN2 IDSIMPIN2;
typedef struct IDSIMPIN1 IDSIMPIN1;
typedef struct IDSIMCKT IDSIMCKT;
typedef struct EVENT EVENT;
typedef struct IDSIMMODEL IDSIMMODEL;
typedef struct ILICENCESERVER ILICENCESERVER;
typedef struct ICPU ICPU;
typedef struct IBUSPIN IBUSPIN;
typedef struct IUSERPOPUP IUSERPOPUP;
typedef struct IMEMORYPOPUP IMEMORYPOPUP;
typedef struct IDEBUGPOPUP IDEBUGPOPUP;
typedef struct ISTATUSPOPUP ISTATUSPOPUP;
typedef struct ISOURCEPOPUP ISOURCEPOPUP;
typedef struct IMSGHLR IMSGHLR;


/*************************************************************************/

typedef struct IDSIMPIN2_vtable IDSIMPIN2_vtable;
typedef struct IDSIMPIN1_vtable IDSIMPIN1_vtable;
typedef struct EVENT_vtable EVENT_vtable;
typedef struct ILICENCESERVER_vtable ILICENCESERVER_vtable;
typedef struct IINSTANCE_vtable IINSTANCE_vtable;
typedef struct ICPU_vtable ICPU_vtable;
typedef struct IDSIMMODEL_vtable IDSIMMODEL_vtable;
typedef struct IBUSPIN_vtable IBUSPIN_vtable;
typedef struct IDSIMCKT_vtable IDSIMCKT_vtable;
typedef struct IMEMORYPOPUP_vtable IMEMORYPOPUP_vtable;
typedef struct IDEBUGPOPUP_vtable IDEBUGPOPUP_vtable;
typedef struct IUSERPOPUP_vtable IUSERPOPUP_vtable;
typedef struct ISOURCEPOPUP_vtable ISOURCEPOPUP_vtable;
typedef struct ISTATUSPOPUP_vtable ISTATUSPOPUP_vtable;
typedef struct IMSGHLR_vtable IMSGHLR_vtable;


typedef struct VSM_PIN
{
	BOOL is_digital; ///< Pin is a digital one
	char name[128]; ///< The name of the pin in graphical model
	ABSTIME on_time; ///< Pin switch on-time
	ABSTIME off_time; ///< Pin switch off-time
	IDSIMPIN* pin; ///< DSIM pin pointer itself
} VSM_PIN; ///< OpenVSM pin structure

struct IDSIMPIN2_vtable
{

	VOID __FASTCALL__ ( *settiming ) ( IDSIMPIN2* this, EDX, RELTIME tlh, RELTIME thl, RELTIME tgq );
	VOID __FASTCALL__ ( *setstates ) ( IDSIMPIN2* this, EDX, STATE tstate, STATE fstate, STATE zstate );
	EVENT* __FASTCALL__ ( *drivebool ) ( IDSIMPIN2* this, EDX, ABSTIME atime, bool flag );
	EVENT* __FASTCALL__ ( *drivestate ) ( IDSIMPIN2* this, EDX, ABSTIME atime, STATE state );
	EVENT* __FASTCALL__ ( *drivetristate ) ( IDSIMPIN2* this, EDX, ABSTIME atime );
	
};
/************************************************************************/
struct IDSIMPIN1_vtable
{

	bool __FASTCALL__ ( *invert ) ( IDSIMPIN1* this, EDX );
	STATE __FASTCALL__ ( *istate ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALL__ ( *issteady ) ( IDSIMPIN1* this, EDX );
	int32_t __FASTCALL__ ( *activity ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALL__ ( *isactive ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALL__ ( *isinactive ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALL__ ( *isposedge ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALL__ ( *isnegedge ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALL__ ( *isedge ) ( IDSIMPIN1* this, EDX );
	EVENT* __FASTCALL__ ( *setstate1 ) ( IDSIMPIN1* this, EDX, ABSTIME atime, RELTIME tlh, RELTIME thl, RELTIME tgq, STATE state );
	EVENT* __FASTCALL__ ( *setstate2 ) ( IDSIMPIN1* this, EDX, ABSTIME atime, RELTIME tgq, STATE state );
	VOID __FASTCALL__ ( *setstate3 ) ( IDSIMPIN1* this, EDX, STATE state );
	VOID __FASTCALL__ ( *sethandler ) ( IDSIMPIN1* this, EDX, IDSIMMODEL* model, void* phf );
	DSIMNODE __FASTCALL__ ( *getnode ) ( IDSIMPIN1* this, EDX );
	STATE __FASTCALL__ ( *getstate ) ( IDSIMPIN1* this, EDX );
};

struct EVENT_vtable
{


};
/************************************************************************/
/* License server API wrapper */
struct ILICENCESERVER_vtable
{

	bool __FASTCALL__ ( *authorize ) ( ILICENCESERVER* this, EDX, uint32_t product_id, uint32_t apiver );
	bool __FASTCALL__ ( *authorizeex ) ( ILICENCESERVER* this, EDX, uint32_t product_id, uint32_t apiver, uint32_t builddate );
	uint32_t __FASTCALL__ ( *getid ) (  ILICENCESERVER* this, EDX );
	CHAR* __FASTCALL__ ( *getname ) ( ILICENCESERVER* this, EDX );
	CHAR* __FASTCALL__ ( *getcompany ) ( ILICENCESERVER* this, EDX );
	CHAR* __FASTCALL__ ( *getexpirydate ) ( ILICENCESERVER* this, EDX );
};

/*************************************************************************/

// CPU Interface - CPU models derive off this to implement debug support.
struct ICPU_vtable
{
	LRESULT __FASTCALL__ ( *vdmhlr ) ( ICPU* this, EDX, VDM_COMMAND* cmd, BYTE* data );
	VOID __FASTCALL__ ( *loaddata ) ( ICPU* this, EDX, int32_t format, int32_t seg, ADDRESS address, BYTE* data, int32_t numbytes );
	VOID __FASTCALL__ ( *disassemble ) ( ICPU* this, EDX, ADDRESS address, int32_t numbytes );
	bool __FASTCALL__ ( *getvardata ) ( ICPU* this, EDX, VARITEM* vip, VARDATA* vdp );
};

/*************************************************************************/
struct IDSIMMODEL_vtable
{

	int32_t __FASTCALL__ ( *isdigital ) ( IDSIMMODEL* this, EDX, CHAR* pinname );
	VOID __FASTCALL__ ( *setup ) ( IDSIMMODEL* this, EDX, IINSTANCE* inst, IDSIMCKT* dsim );
	VOID __FASTCALL__ ( *runctrl ) ( IDSIMMODEL* this, EDX, RUNMODES mode );
	VOID __FASTCALL__ ( *actuate ) ( IDSIMMODEL* this, EDX, REALTIME atime, ACTIVESTATE newstate );
	bool __FASTCALL__ ( *indicate ) ( IDSIMMODEL* this, EDX, REALTIME atime, ACTIVEDATA* data );
	VOID __FASTCALL__ ( *simulate ) ( IDSIMMODEL* this, EDX, ABSTIME atime, DSIMMODES mode );
	VOID __FASTCALL__ ( *callback ) ( IDSIMMODEL* this, EDX, ABSTIME atime, EVENTID eventid );
};


/*************************************************************************/
// Interface to bus pin object - new in APIVER 1.10
struct IBUSPIN_vtable
{
	VOID __FASTCALL__ ( *settiming ) ( IBUSPIN* this, EDX, RELTIME tlh, RELTIME thl, RELTIME tz );
	VOID __FASTCALL__ ( *setstates ) ( IBUSPIN* this, EDX, STATE tstate, STATE fstate, STATE zstate );
	VOID __FASTCALL__ ( *sethandler ) ( IBUSPIN* this, EDX, IDSIMMODEL* model, void* phf );
	VOID __FASTCALL__ ( *drivebusvalue ) ( IBUSPIN* this, EDX, ABSTIME atime, uint32_t value );
	VOID __FASTCALL__ ( *drivetristate ) ( IBUSPIN* this, EDX, ABSTIME atime );
	VOID __FASTCALL__ ( *drivebitstate ) ( IBUSPIN* this, EDX, ABSTIME atime, uint32_t bit, STATE state );
	uint32_t __FASTCALL__ ( *getbusvalue ) ( IBUSPIN* this, EDX );
	uint32_t __FASTCALL__ ( *getbusdrive ) ( IBUSPIN* this, EDX );
	STATE __FASTCALL__ ( *getbitstate ) ( IBUSPIN* this, EDX, uint32_t bit );
};
/*************************************************************************/
// Public Interface to a simulator component instance.

struct IINSTANCE_vtable
{
	// Basic property access:
	CHAR* __FASTCALL__ ( *id ) ( IINSTANCE* this, EDX );
	CHAR* __FASTCALL__ ( *value ) ( IINSTANCE* this, EDX );
	CHAR* __FASTCALL__ ( *getstrval ) ( IINSTANCE* this, EDX, CHAR* name, CHAR* defval );
	VOID __FASTCALL__ ( *getnumval ) ( IINSTANCE* this, EDX, double* result, CHAR* name, double defval );
	bool __FASTCALL__ ( *getboolval ) ( IINSTANCE* this, EDX, CHAR* name, bool defval );
	uint32_t __FASTCALL__ ( *gethexval ) ( IINSTANCE* this, EDX, CHAR* name, uint32_t defval );
	int64_t __FASTCALL__ ( *getinitval ) ( IINSTANCE* this, EDX, CHAR* name, int64_t defval );
	RELTIME __FASTCALL__ ( *getdelay ) ( IINSTANCE* this, EDX, CHAR* name, RELTIME defval );
	// Special property access
	IACTIVEMODEL* __FASTCALL__ ( *getactivemodel ) (  IINSTANCE* this, EDX );
	IINSTANCE* __FASTCALL__ ( *getinterfacemodel ) ( IINSTANCE* this, EDX );
	bool __FASTCALL__ ( *getmoddata ) ( IINSTANCE* this, EDX, BYTE** data, uint32_t* size );
	// Access to the nodes and pins:
	SPICENODE __FASTCALL__ ( *getspicenode ) ( IINSTANCE* this, EDX, CHAR* namelist, bool required );
	IDSIMPIN* __FASTCALL__ ( *getdsimpin ) ( IINSTANCE* this, EDX, CHAR* namelist, bool required );
	// Logging and messaging:
	VOID __cdecl ( *log ) ( IINSTANCE* this, CHAR* msg, ... );
	VOID __cdecl ( *warning ) ( IINSTANCE* this, CHAR* msg, ... );
	VOID __cdecl ( *error ) ( IINSTANCE* this, CHAR* msg, ... );
	VOID __cdecl ( *fatal ) ( IINSTANCE* this, CHAR* msg, ... );
	bool __cdecl ( *message ) ( IINSTANCE* this, CHAR* msg, ... );
	// Pop-up window support:
	IPOPUP* __FASTCALL__ ( *createpopup ) ( IINSTANCE* this, EDX, CREATEPOPUPSTRUCT* cps );
	VOID __FASTCALL__ ( *deletepopup ) ( IINSTANCE* this, EDX, POPUPID id );
	//  debug monitor interface:
	bool __FASTCALL__ ( *setvdmhlr ) ( IINSTANCE* this, EDX, ICPU* );
	// Generic memory loader:
	bool __FASTCALL__ ( *loadmemory ) ( IINSTANCE* this, EDX, CHAR* filename, VOID* buffer, uint32_t size, uint32_t base, uint32_t shift );
	// Access to bus pins
	IBUSPIN* __FASTCALL__ ( *getbuspin1 ) ( IINSTANCE* this, EDX, CHAR* namestem, uint32_t base, uint32_t width, bool required );
	IBUSPIN __FASTCALL__ ( *getbuspin2 ) ( IINSTANCE* this, EDX, CHAR* name, IDSIMPIN** pins, uint32_t width );
};
/*************************************************************************/
struct IDSIMCKT_vtable
{
	VOID __FASTCALL__ ( *sysvar ) ( IDSIMCKT* this, EDX, double* result, DSIMVARS var );
	EVENT* __FASTCALL__ ( *setcallback ) ( IDSIMCKT* this, EDX, ABSTIME evttime, IDSIMMODEL* model, EVENTID id );
	bool __FASTCALL__ ( *cancelcallback ) ( IDSIMCKT* this, EDX, EVENT* event, IDSIMMODEL* model );
	VOID __FASTCALL__ ( *setbreak ) ( IDSIMCKT* this, EDX, ABSTIME breaktime );
	VOID __FASTCALL__ ( *suspend ) ( IDSIMCKT* this, EDX, IINSTANCE* instance, CHAR* msg );
	EVENT* __FASTCALL__ ( *setcallbackex ) ( IDSIMCKT* this, EDX, ABSTIME evttime, IDSIMMODEL* model, void* func, EVENTID id );
	DSIMNODE __FASTCALL__ ( *newnode ) ( IDSIMCKT* this, EDX, CHAR* partid, CHAR* nodename );
	IDSIMPIN* __FASTCALL__ ( *newpin ) ( IDSIMCKT* this, EDX, IINSTANCE*, DSIMNODE node, CHAR* name, uint32_t flags );
	EVENT* __FASTCALL__ ( *setclockcallback ) ( IDSIMCKT* this, EDX, ABSTIME starttime, RELTIME period, IDSIMMODEL* model, void* func, EVENTID id );
};

/*************************************************************************/
// User defined popup window:
struct IUSERPOPUP_vtable
{
	CHAR* __FASTCALL__ ( *getprop ) ( IUSERPOPUP* this, EDX, CHAR* key );
	VOID __FASTCALL__ ( *setprop ) ( IUSERPOPUP* this, EDX, CHAR* key, CHAR* value );
	VOID __FASTCALL__ ( *setmsghlr ) ( IUSERPOPUP* this, EDX, IMSGHLR* handler );
	LRESULT __FASTCALL__ ( *callwindowproc ) ( IUSERPOPUP* this, EDX, MESSAGE msg, WPARAM warg, LPARAM larg );
	
};

// Debug\logging window:
struct IDEBUGPOPUP_vtable
{
	VOID __cdecl ( *print ) ( IDEBUGPOPUP* this, CHAR* msg, ... );
	VOID __FASTCALL__ ( *dump ) ( IDEBUGPOPUP* this, EDX, const BYTE* ptr, uint32_t nbytes, uint32_t base );
};

// Memory dump window:
struct IMEMORYPOPUP_vtable
{
	VOID __FASTCALL__ ( *setmemory ) ( IMEMORYPOPUP* this, EDX, ADDRESS baseaddr, BYTE* data, uint32_t nbytes );
	VOID __FASTCALL__ ( *repaint ) ( IMEMORYPOPUP* this, EDX );
};

// Status display window:
struct ISTATUSPOPUP_vtable
{
	VOID __FASTCALL__ ( *setarea ) ( ISTATUSPOPUP* this, EDX, uint32_t columns, uint32_t rows, uint32_t border, bool copy );
	VOID __FASTCALL__ ( *setcursorto ) ( ISTATUSPOPUP* this, EDX, uint32_t pixelx, uint32_t pixely );
	uint32_t __FASTCALL__ ( *getcharwidth ) ( ISTATUSPOPUP* this, EDX );
	uint32_t __FASTCALL__ ( *getcharheight ) ( ISTATUSPOPUP* this, EDX );
	VOID __cdecl ( *print1 ) ( ISTATUSPOPUP* this, CHAR* message, ... );
	VOID __cdecl ( *print2 ) ( ISTATUSPOPUP* this, int32_t col, int32_t row, COLOUR textcolour, CHAR* msg, ... );
	VOID __FASTCALL__ ( *setctabstops1 ) ( ISTATUSPOPUP* this, EDX, const int32_t ts[], int32_t n );
	VOID __FASTCALL__ ( *setctabstops2 ) ( ISTATUSPOPUP* this, EDX, const int32_t ts[], int32_t n ); /*FIXME: Is it error?*/
	VOID __FASTCALL__ ( *clear ) ( ISTATUSPOPUP* this, EDX, COLOUR bkcolour );
	bool __FASTCALL__ ( *setredraw ) ( ISTATUSPOPUP* this, EDX, bool on_off, bool redraw_now );
	VOID __FASTCALL__ ( *repaint ) ( ISTATUSPOPUP* this, EDX );
};

// Status display window:
struct ISOURCEPOPUP_vtable
{
	bool __FASTCALL__ ( *addsdifile ) ( ISOURCEPOPUP* this, EDX, CHAR* file );
	bool __FASTCALL__ ( *setpcaddress ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALL__ ( *isbreakpoint ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALL__ ( *iscurrentline ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALL__ ( *findfirstbpt ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALL__ ( *findnextbpt ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	
	bool __FASTCALL__ ( *addsrcfile ) ( ISOURCEPOPUP* this, EDX, CHAR* file, bool lowlevel );
	VOID __FASTCALL__ ( *addcodeline ) ( ISOURCEPOPUP* this, EDX, int32_t srclinenum, ADDRESS address );
	VOID __FASTCALL__ ( *addcodelabel ) ( ISOURCEPOPUP* this, EDX, CHAR* label, ADDRESS address );
	VOID __FASTCALL__ ( *update ) ( ISOURCEPOPUP* this, EDX );
	
	bool __FASTCALL__ ( *getsteptoaddr ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	VOID __FASTCALL__ ( *setinsertpos ) ( ISOURCEPOPUP* this, EDX, int32_t fileid, int32_t linenum, bool newblock );
	VOID __FASTCALL__ ( *insertline ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr, CHAR* opcodes, CHAR* srctext );
	bool __FASTCALL__ ( *findfirstsrcline ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	bool __FASTCALL__ ( *findnextsrcline ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	CHAR* __FASTCALL__ ( *findlabel ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	
};

/*************************************************************************/

struct IMSGHLR_vtable
{
	LRESULT __FASTCALL__ ( *msghlr ) ( IMEMORYPOPUP* this, EDX, HWND, MESSAGE, WPARAM, LPARAM );
};

#endif

