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

	VOID __FASTCALLP__ ( *settiming ) ( IDSIMPIN2* this, EDX, RELTIME tlh, RELTIME thl, RELTIME tgq );
	VOID __FASTCALLP__ ( *setstates ) ( IDSIMPIN2* this, EDX, STATE tstate, STATE fstate, STATE zstate );
	EVENT* __FASTCALLP__ ( *drivebool ) ( IDSIMPIN2* this, EDX, ABSTIME atime, bool flag );
	EVENT* __FASTCALLP__ ( *drivestate ) ( IDSIMPIN2* this, EDX, ABSTIME atime, STATE state );
	EVENT* __FASTCALLP__ ( *drivetristate ) ( IDSIMPIN2* this, EDX, ABSTIME atime );
	
};
/************************************************************************/
struct IDSIMPIN1_vtable
{

	bool __FASTCALLP__ ( *invert ) ( IDSIMPIN1* this, EDX );
	STATE __FASTCALLP__ ( *istate ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALLP__ ( *issteady ) ( IDSIMPIN1* this, EDX );
	int32_t __FASTCALLP__ ( *activity ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALLP__ ( *isactive ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALLP__ ( *isinactive ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALLP__ ( *isposedge ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALLP__ ( *isnegedge ) ( IDSIMPIN1* this, EDX );
	bool __FASTCALLP__ ( *isedge ) ( IDSIMPIN1* this, EDX );
	EVENT* __FASTCALLP__ ( *setstate1 ) ( IDSIMPIN1* this, EDX, ABSTIME atime, RELTIME tlh, RELTIME thl, RELTIME tgq, STATE state );
	EVENT* __FASTCALLP__ ( *setstate2 ) ( IDSIMPIN1* this, EDX, ABSTIME atime, RELTIME tgq, STATE state );
	VOID __FASTCALLP__ ( *setstate3 ) ( IDSIMPIN1* this, EDX, STATE state );
	VOID __FASTCALLP__ ( *sethandler ) ( IDSIMPIN1* this, EDX, IDSIMMODEL* model, void* phf );
	DSIMNODE __FASTCALLP__ ( *getnode ) ( IDSIMPIN1* this, EDX );
	STATE __FASTCALLP__ ( *getstate ) ( IDSIMPIN1* this, EDX );
};

/************************************************************************/
/* License server API wrapper */
struct ILICENCESERVER_vtable
{

	bool __FASTCALLP__ ( *authorize ) ( ILICENCESERVER* this, EDX, uint32_t product_id, uint32_t apiver );
	bool __FASTCALLP__ ( *authorizeex ) ( ILICENCESERVER* this, EDX, uint32_t product_id, uint32_t apiver, uint32_t builddate );
	uint32_t __FASTCALLP__ ( *getid ) (  ILICENCESERVER* this, EDX );
	CHAR* __FASTCALLP__ ( *getname ) ( ILICENCESERVER* this, EDX );
	CHAR* __FASTCALLP__ ( *getcompany ) ( ILICENCESERVER* this, EDX );
	CHAR* __FASTCALLP__ ( *getexpirydate ) ( ILICENCESERVER* this, EDX );
};

/*************************************************************************/

// CPU Interface - CPU models derive off this to implement debug support.
struct ICPU_vtable
{
	LRESULT __FASTCALLP__ ( *vdmhlr ) ( ICPU* this, EDX, VDM_COMMAND* cmd, BYTE* data );
	VOID __FASTCALLP__ ( *loaddata ) ( ICPU* this, EDX, int32_t format, int32_t seg, ADDRESS address, BYTE* data, int32_t numbytes );
	VOID __FASTCALLP__ ( *disassemble ) ( ICPU* this, EDX, ADDRESS address, int32_t numbytes );
	bool __FASTCALLP__ ( *getvardata ) ( ICPU* this, EDX, VARITEM* vip, VARDATA* vdp );
};

/*************************************************************************/
struct IDSIMMODEL_vtable
{

	int32_t __FASTCALLP__ ( *isdigital ) ( IDSIMMODEL* this, EDX, CHAR* pinname );
	VOID __FASTCALLP__ ( *setup ) ( IDSIMMODEL* this, EDX, IINSTANCE* inst, IDSIMCKT* dsim );
	VOID __FASTCALLP__ ( *runctrl ) ( IDSIMMODEL* this, EDX, RUNMODES mode );
	VOID __FASTCALLP__ ( *actuate ) ( IDSIMMODEL* this, EDX, REALTIME atime, ACTIVESTATE newstate );
	bool __FASTCALLP__ ( *indicate ) ( IDSIMMODEL* this, EDX, REALTIME atime, ACTIVEDATA* data );
	VOID __FASTCALLP__ ( *simulate ) ( IDSIMMODEL* this, EDX, ABSTIME atime, DSIMMODES mode );
	VOID __FASTCALLP__ ( *callback ) ( IDSIMMODEL* this, EDX, ABSTIME atime, EVENTID eventid );
};


/*************************************************************************/
// Interface to bus pin object - new in APIVER 1.10
struct IBUSPIN_vtable
{
	VOID __FASTCALLP__ ( *settiming ) ( IBUSPIN* this, EDX, RELTIME tlh, RELTIME thl, RELTIME tz );
	VOID __FASTCALLP__ ( *setstates ) ( IBUSPIN* this, EDX, STATE tstate, STATE fstate, STATE zstate );
	VOID __FASTCALLP__ ( *sethandler ) ( IBUSPIN* this, EDX, IDSIMMODEL* model, void* phf );
	VOID __FASTCALLP__ ( *drivebusvalue ) ( IBUSPIN* this, EDX, ABSTIME atime, uint32_t value );
	VOID __FASTCALLP__ ( *drivetristate ) ( IBUSPIN* this, EDX, ABSTIME atime );
	VOID __FASTCALLP__ ( *drivebitstate ) ( IBUSPIN* this, EDX, ABSTIME atime, uint32_t bit, STATE state );
	uint32_t __FASTCALLP__ ( *getbusvalue ) ( IBUSPIN* this, EDX );
	uint32_t __FASTCALLP__ ( *getbusdrive ) ( IBUSPIN* this, EDX );
	STATE __FASTCALLP__ ( *getbitstate ) ( IBUSPIN* this, EDX, uint32_t bit );
};
/*************************************************************************/
// Public Interface to a simulator component instance.

struct IINSTANCE_vtable
{
	// Basic property access:
	CHAR* __FASTCALLP__ ( *id ) ( IINSTANCE* this, EDX );
	CHAR* __FASTCALLP__ ( *value ) ( IINSTANCE* this, EDX );
	CHAR* __FASTCALLP__ ( *getstrval ) ( IINSTANCE* this, EDX, CHAR* name, CHAR* defval );
	VOID __FASTCALLP__ ( *getnumval ) ( IINSTANCE* this, EDX, double* result, CHAR* name, double defval );
	bool __FASTCALLP__ ( *getboolval ) ( IINSTANCE* this, EDX, CHAR* name, bool defval );
	uint32_t __FASTCALLP__ ( *gethexval ) ( IINSTANCE* this, EDX, CHAR* name, uint32_t defval );
	int64_t __FASTCALLP__ ( *getinitval ) ( IINSTANCE* this, EDX, CHAR* name, int64_t defval );
	RELTIME __FASTCALLP__ ( *getdelay ) ( IINSTANCE* this, EDX, CHAR* name, RELTIME defval );
	// Special property access
	IACTIVEMODEL* __FASTCALLP__ ( *getactivemodel ) (  IINSTANCE* this, EDX );
	IINSTANCE* __FASTCALLP__ ( *getinterfacemodel ) ( IINSTANCE* this, EDX );
	bool __FASTCALLP__ ( *getmoddata ) ( IINSTANCE* this, EDX, BYTE** data, uint32_t* size );
	// Access to the nodes and pins:
	SPICENODE __FASTCALLP__ ( *getspicenode ) ( IINSTANCE* this, EDX, CHAR* namelist, bool required );
	IDSIMPIN* __FASTCALLP__ ( *getdsimpin ) ( IINSTANCE* this, EDX, CHAR* namelist, bool required );
	// Logging and messaging:
	VOID __CDECLP__ ( *log ) ( IINSTANCE* this, CHAR* msg, ... );
	VOID __CDECLP__ ( *warning ) ( IINSTANCE* this, CHAR* msg, ... );
	VOID __CDECLP__ ( *error ) ( IINSTANCE* this, CHAR* msg, ... );
	VOID __CDECLP__ ( *fatal ) ( IINSTANCE* this, CHAR* msg, ... );
	bool __CDECLP__ ( *message ) ( IINSTANCE* this, CHAR* msg, ... );
	// Pop-up window support:
	IPOPUP* __FASTCALLP__ ( *createpopup ) ( IINSTANCE* this, EDX, CREATEPOPUPSTRUCT* cps );
	VOID __FASTCALLP__ ( *deletepopup ) ( IINSTANCE* this, EDX, POPUPID id );
	//  debug monitor interface:
	bool __FASTCALLP__ ( *setvdmhlr ) ( IINSTANCE* this, EDX, ICPU* );
	// Generic memory loader:
	bool __FASTCALLP__ ( *loadmemory ) ( IINSTANCE* this, EDX, CHAR* filename, VOID* buffer, uint32_t size, uint32_t base, uint32_t shift );
	// Access to bus pins
	IBUSPIN* __FASTCALLP__ ( *getbuspin1 ) ( IINSTANCE* this, EDX, CHAR* namestem, uint32_t base, uint32_t width, bool required );
	IBUSPIN __FASTCALLP__ ( *getbuspin2 ) ( IINSTANCE* this, EDX, CHAR* name, IDSIMPIN** pins, uint32_t width );
};
/*************************************************************************/
struct IDSIMCKT_vtable
{
	VOID __FASTCALLP__ ( *sysvar ) ( IDSIMCKT* this, EDX, double* result, DSIMVARS var );
	EVENT* __FASTCALLP__ ( *setcallback ) ( IDSIMCKT* this, EDX, ABSTIME evttime, IDSIMMODEL* model, EVENTID id );
	bool __FASTCALLP__ ( *cancelcallback ) ( IDSIMCKT* this, EDX, EVENT* event, IDSIMMODEL* model );
	VOID __FASTCALLP__ ( *setbreak ) ( IDSIMCKT* this, EDX, ABSTIME breaktime );
	VOID __FASTCALLP__ ( *suspend ) ( IDSIMCKT* this, EDX, IINSTANCE* instance, CHAR* msg );
	EVENT* __FASTCALLP__ ( *setcallbackex ) ( IDSIMCKT* this, EDX, ABSTIME evttime, IDSIMMODEL* model, void* func, EVENTID id );
	DSIMNODE __FASTCALLP__ ( *newnode ) ( IDSIMCKT* this, EDX, CHAR* partid, CHAR* nodename );
	IDSIMPIN* __FASTCALLP__ ( *newpin ) ( IDSIMCKT* this, EDX, IINSTANCE*, DSIMNODE node, CHAR* name, uint32_t flags );
	EVENT* __FASTCALLP__ ( *setclockcallback ) ( IDSIMCKT* this, EDX, ABSTIME starttime, RELTIME period, IDSIMMODEL* model, void* func, EVENTID id );
};

/*************************************************************************/
// User defined popup window:
struct IUSERPOPUP_vtable
{
	CHAR* __FASTCALLP__ ( *getprop ) ( IUSERPOPUP* this, EDX, CHAR* key );
	VOID __FASTCALLP__ ( *setprop ) ( IUSERPOPUP* this, EDX, CHAR* key, CHAR* value );
	VOID __FASTCALLP__ ( *setmsghlr ) ( IUSERPOPUP* this, EDX, IMSGHLR* handler );
	LRESULT __FASTCALLP__ ( *callwindowproc ) ( IUSERPOPUP* this, EDX, MESSAGE msg, WPARAM warg, LPARAM larg );
	
};

// Debug\logging window:
struct IDEBUGPOPUP_vtable
{
	VOID __CDECLP__ ( *print ) ( IDEBUGPOPUP* this, CHAR* msg, ... );
	VOID __FASTCALLP__ ( *dump ) ( IDEBUGPOPUP* this, EDX, const BYTE* ptr, uint32_t nbytes, uint32_t base );
};

// Memory dump window:
struct IMEMORYPOPUP_vtable
{
	VOID __FASTCALLP__ ( *setmemory ) ( IMEMORYPOPUP* this, EDX, ADDRESS baseaddr, BYTE* data, uint32_t nbytes );
	VOID __FASTCALLP__ ( *repaint ) ( IMEMORYPOPUP* this, EDX );
};

// Status display window:
struct ISTATUSPOPUP_vtable
{
	VOID __FASTCALLP__ ( *setarea ) ( ISTATUSPOPUP* this, EDX, uint32_t columns, uint32_t rows, uint32_t border, bool copy );
	VOID __FASTCALLP__ ( *setcursorto ) ( ISTATUSPOPUP* this, EDX, uint32_t pixelx, uint32_t pixely );
	uint32_t __FASTCALLP__ ( *getcharwidth ) ( ISTATUSPOPUP* this, EDX );
	uint32_t __FASTCALLP__ ( *getcharheight ) ( ISTATUSPOPUP* this, EDX );
	VOID __CDECLP__ ( *print1 ) ( ISTATUSPOPUP* this, CHAR* message, ... );
	VOID __CDECLP__ ( *print2 ) ( ISTATUSPOPUP* this, int32_t col, int32_t row, COLOUR textcolour, CHAR* msg, ... );
	VOID __FASTCALLP__ ( *setctabstops1 ) ( ISTATUSPOPUP* this, EDX, const int32_t ts[], int32_t n );
	VOID __FASTCALLP__ ( *setctabstops2 ) ( ISTATUSPOPUP* this, EDX, const int32_t ts[], int32_t n ); /*FIXME: Is it error?*/
	VOID __FASTCALLP__ ( *clear ) ( ISTATUSPOPUP* this, EDX, COLOUR bkcolour );
	bool __FASTCALLP__ ( *setredraw ) ( ISTATUSPOPUP* this, EDX, bool on_off, bool redraw_now );
	VOID __FASTCALLP__ ( *repaint ) ( ISTATUSPOPUP* this, EDX );
};

// Status display window:
struct ISOURCEPOPUP_vtable
{
	bool __FASTCALLP__ ( *addsdifile ) ( ISOURCEPOPUP* this, EDX, CHAR* file );
	bool __FASTCALLP__ ( *setpcaddress ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALLP__ ( *isbreakpoint ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALLP__ ( *iscurrentline ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALLP__ ( *findfirstbpt ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	bool __FASTCALLP__ ( *findnextbpt ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr );
	
	bool __FASTCALLP__ ( *addsrcfile ) ( ISOURCEPOPUP* this, EDX, CHAR* file, bool lowlevel );
	VOID __FASTCALLP__ ( *addcodeline ) ( ISOURCEPOPUP* this, EDX, int32_t srclinenum, ADDRESS address );
	VOID __FASTCALLP__ ( *addcodelabel ) ( ISOURCEPOPUP* this, EDX, CHAR* label, ADDRESS address );
	VOID __FASTCALLP__ ( *update ) ( ISOURCEPOPUP* this, EDX );
	
	bool __FASTCALLP__ ( *getsteptoaddr ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	VOID __FASTCALLP__ ( *setinsertpos ) ( ISOURCEPOPUP* this, EDX, int32_t fileid, int32_t linenum, bool newblock );
	VOID __FASTCALLP__ ( *insertline ) ( ISOURCEPOPUP* this, EDX, ADDRESS addr, CHAR* opcodes, CHAR* srctext );
	bool __FASTCALLP__ ( *findfirstsrcline ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	bool __FASTCALLP__ ( *findnextsrcline ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	CHAR* __FASTCALLP__ ( *findlabel ) ( ISOURCEPOPUP* this, EDX, ADDRESS* addr );
	
};

/*************************************************************************/

struct IMSGHLR_vtable
{
	LRESULT __FASTCALLP__ ( *msghlr ) ( IMEMORYPOPUP* this, EDX, HWND, MESSAGE, WPARAM, LPARAM );
};

