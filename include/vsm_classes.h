#ifndef VSM_CLASSES_H
#define VSM_CLASSES_H
#include <vsm_api.h>
#include <vdm_api.h>

#define EDX DWORD EDX

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
typedef struct ISTATUSPOPUP_vtable ISTATUSPOPUP_vtable;
typedef struct IMSGHLR_vtable IMSGHLR_vtable;


struct IDSIMPIN2_vtable
{

	VOID __attribute__((fastcall)) (*settiming)(IDSIMPIN2 *this, EDX, RELTIME tlh, RELTIME thl, RELTIME tgq );
	VOID __attribute__((fastcall)) (*setstates)(IDSIMPIN2 *this, EDX, STATE tstate, STATE fstate, STATE zstate);
	EVENT* __attribute__((fastcall)) (*drivebool)(IDSIMPIN2 *this, EDX, ABSTIME time, BOOL flag);
	EVENT* __attribute__((fastcall)) (*drivestate)(IDSIMPIN2 *this, EDX, ABSTIME time, STATE state);
	EVENT* __attribute__((fastcall)) (*drivetristate)(IDSIMPIN2 *this, EDX, ABSTIME time);

};
/************************************************************************/
struct IDSIMPIN1_vtable
{

	BOOL __attribute__((fastcall)) (*invert)(IDSIMPIN1 *this, EDX);
	STATE __attribute__((fastcall)) (*istate)(IDSIMPIN1 *this, EDX);
	BOOL __attribute__((fastcall)) (*issteady)(IDSIMPIN1 *this, EDX);
	INT __attribute__((fastcall)) (*activity)(IDSIMPIN1 *this, EDX);
	BOOL __attribute__((fastcall)) (*isactive)(IDSIMPIN1 *this, EDX);
	BOOL __attribute__((fastcall)) (*isinactive)(IDSIMPIN1 *this, EDX);
	BOOL __attribute__((fastcall)) (*isposedge)(IDSIMPIN1 *this, EDX);
	BOOL __attribute__((fastcall)) (*isnegedge)(IDSIMPIN1 *this, EDX);
	BOOL __attribute__((fastcall)) (*isedge)(IDSIMPIN1 *this, EDX);
	EVENT* __attribute__((fastcall)) (*setstate1)(IDSIMPIN1 *this, EDX, ABSTIME time, RELTIME tlh, RELTIME thl, RELTIME tgq, STATE state);
	EVENT* __attribute__((fastcall)) (*setstate2)(IDSIMPIN1 *this, EDX, ABSTIME time, RELTIME tgq, STATE state);
	VOID __attribute__((fastcall)) (*setstate3)(IDSIMPIN1 *this, EDX, STATE state);
	VOID __attribute__((fastcall)) (*sethandler)(IDSIMPIN1 *this, EDX, IDSIMMODEL *model, PINHANDLERFN phf);
	DSIMNODE __attribute__((fastcall)) (*getnode)(IDSIMPIN1 *this, EDX);
	STATE __attribute__((fastcall)) (*getstate)(IDSIMPIN1 *this, EDX);
};

struct EVENT_vtable
{


};
/************************************************************************/
/* License server API wrapper */
struct ILICENCESERVER_vtable
{

	BOOL __attribute__((fastcall)) (*authorize)( ILICENCESERVER *this, EDX, DWORD product_id, DWORD apiver );
	BOOL __attribute__((fastcall)) (*authorizeex)( ILICENCESERVER *this, EDX, DWORD product_id, DWORD apiver, DWORD builddate );
	DWORD __attribute__((fastcall)) (*getid)(  ILICENCESERVER *this, EDX );
	CHAR * __attribute__((fastcall)) (*getname)( ILICENCESERVER *this, EDX );
	CHAR * __attribute__((fastcall)) (*getcompany)( ILICENCESERVER *this, EDX );
	CHAR * __attribute__((fastcall)) (*getexpirydate)( ILICENCESERVER *this, EDX );
};

/*************************************************************************/

// CPU Interface - CPU models derive off this to implement debug support.
struct ICPU_vtable
{
	LRESULT __attribute__((fastcall)) (*vdmhlr)(ICPU *this, EDX, VDM_COMMAND *cmd, BYTE *data);
	VOID __attribute__((fastcall)) (*loaddata)(ICPU *this, EDX, INT format, INT seg, ADDRESS address, BYTE *data, INT numbytes);
	VOID __attribute__((fastcall)) (*disassemble)(ICPU *this, EDX, ADDRESS address, INT numbytes);
	BOOL __attribute__((fastcall)) (*getvardata)(ICPU *this, EDX, VARITEM *vip, VARDATA *vdp);
};

/*************************************************************************/
struct IDSIMMODEL_vtable
{

	INT __attribute__((fastcall)) (*isdigital)( IDSIMMODEL *this, EDX, CHAR *pinname );
	VOID __attribute__((fastcall)) (*setup)( IDSIMMODEL *this, EDX, IINSTANCE *inst, IDSIMCKT *dsim );
	VOID __attribute__((fastcall)) (*runctrl)( IDSIMMODEL *this, EDX, RUNMODES mode);
	VOID __attribute__((fastcall)) (*actuate)( IDSIMMODEL *this, EDX, REALTIME time, ACTIVESTATE newstate);
	BOOL __attribute__((fastcall)) (*indicate)( IDSIMMODEL *this, EDX, REALTIME time, ACTIVEDATA *data);
	VOID __attribute__((fastcall)) (*simulate)( IDSIMMODEL *this, EDX, ABSTIME time, DSIMMODES mode);
	VOID __attribute__((fastcall)) (*callback)( IDSIMMODEL *this, EDX, ABSTIME time, EVENTID eventid);
};


/*************************************************************************/
// Interface to bus pin object - new in APIVER 1.10
struct IBUSPIN_vtable
{
	VOID __attribute__((fastcall)) (*settiming)( IBUSPIN *this, EDX, RELTIME tlh, RELTIME thl, RELTIME tz );
	VOID __attribute__((fastcall)) (*setstates)( IBUSPIN *this, EDX, STATE tstate, STATE fstate, STATE zstate);
	VOID __attribute__((fastcall)) (*sethandler)( IBUSPIN *this, EDX, IDSIMMODEL *model, PINHANDLERFN phf);
	VOID __attribute__((fastcall)) (*drivebusvalue)( IBUSPIN *this, EDX, ABSTIME time, DWORD value);
	VOID __attribute__((fastcall)) (*drivetristate)( IBUSPIN *this, EDX, ABSTIME time);
	VOID __attribute__((fastcall)) (*drivebitstate)( IBUSPIN *this, EDX, ABSTIME time, UINT bit, STATE state);
	DWORD __attribute__((fastcall)) (*getbusvalue)( IBUSPIN *this, EDX);
	DWORD __attribute__((fastcall)) (*getbusdrive)( IBUSPIN *this, EDX);
	STATE __attribute__((fastcall)) (*getbitstate)( IBUSPIN *this, EDX, UINT bit);
};
/*************************************************************************/
// Public Interface to a simulator component instance.

struct IINSTANCE_vtable
{
	// Basic property access:
	CHAR* __attribute__((fastcall)) (*id)(IINSTANCE *this, EDX);
	CHAR* __attribute__((fastcall)) (*value)(IINSTANCE *this, EDX);
	CHAR* __attribute__((fastcall)) (*getstrval)(IINSTANCE *this, EDX, CHAR *name, CHAR *defval);
	VOID __attribute__((fastcall)) (*getnumval)(IINSTANCE *this, EDX, DOUBLE *result, CHAR *name, DOUBLE defval);
	BOOL __attribute__((fastcall)) (*getboolval)(IINSTANCE *this, EDX, CHAR *name, BOOL defval);
	DWORD __attribute__((fastcall)) (*gethexval)(IINSTANCE *this, EDX, CHAR *name, DWORD defval);
	LONG __attribute__((fastcall)) (*getinitval)(IINSTANCE *this, EDX, CHAR *name, LONG defval);
	RELTIME __attribute__((fastcall)) (*getdelay)(IINSTANCE *this, EDX, CHAR *name, RELTIME defval);
	// Special property access
	IACTIVEMODEL * __attribute__((fastcall)) (*getactivemodel)(  IINSTANCE *this, EDX );
	IINSTANCE* __attribute__((fastcall)) (*getinterfacemodel)(IINSTANCE *this, EDX);
	BOOL __attribute__((fastcall)) (*getmoddata)(IINSTANCE *this, EDX, BYTE **data, DWORD *size);
	// Access to the nodes and pins:
	SPICENODE __attribute__((fastcall)) (*getspicenode)(IINSTANCE *this, EDX, CHAR *namelist, BOOL required);
	IDSIMPIN* __attribute__((fastcall)) (*getdsimpin)(IINSTANCE *this, EDX, CHAR *namelist, BOOL required);
	// Logging and messaging:
	VOID __cdecl (*log)(IINSTANCE *this, CHAR *msg, ...);
	VOID __cdecl (*warning)(IINSTANCE *this, CHAR *msg, ...);
	VOID __cdecl (*error)(IINSTANCE *this, CHAR *msg, ...);
	VOID __cdecl (*fatal)(IINSTANCE *this, CHAR *msg, ...);
	BOOL __cdecl (*message)(IINSTANCE *this, CHAR *msg, ...);
	// Pop-up window support:
	IPOPUP* __attribute__((fastcall)) (*createpopup)(IINSTANCE *this, EDX, CREATEPOPUPSTRUCT *cps);
	VOID __attribute__((fastcall)) (*deletepopup)(IINSTANCE *this, EDX, POPUPID id);
	//  debug monitor interface:
	BOOL __attribute__((fastcall)) (*setvdmhlr)(IINSTANCE *this, EDX, ICPU *);
	// Generic memory loader:
	BOOL __attribute__((fastcall)) (*loadmemory)(IINSTANCE *this, EDX, CHAR *filename, VOID *buffer, UINT size, UINT base, UINT shift);
	// Access to bus pins
	IBUSPIN* __attribute__((fastcall)) (*getbuspin1)(IINSTANCE *this, EDX, CHAR *namestem, UINT base, UINT width, BOOL required);
	IBUSPIN __attribute__((fastcall)) (*getbuspin2)(IINSTANCE *this, EDX, CHAR *name, IDSIMPIN **pins, UINT width);
};
/*************************************************************************/
struct IDSIMCKT_vtable
{
	VOID __attribute__((fastcall)) (*sysvar)( IDSIMCKT *this, EDX, DOUBLE *result, DSIMVARS var );
	EVENT* __attribute__((fastcall)) (*setcallback)( IDSIMCKT *this, EDX, ABSTIME evttime, IDSIMMODEL *model, EVENTID id);
	BOOL __attribute__((fastcall)) (*cancelcallback)( IDSIMCKT *this, EDX, EVENT *event, IDSIMMODEL *model);
	VOID __attribute__((fastcall)) (*setbreak)( IDSIMCKT *this, EDX, ABSTIME breaktime);
	VOID __attribute__((fastcall)) (*suspend)( IDSIMCKT *this, EDX, IINSTANCE *instance, CHAR *msg);
	EVENT* __attribute__((fastcall)) (*setcallbackex)( IDSIMCKT *this, EDX, ABSTIME evttime, IDSIMMODEL *model, CALLBACKHANDLERFN func, EVENTID id);
	DSIMNODE __attribute__((fastcall)) (*newnode)( IDSIMCKT *this, EDX, CHAR *partid, CHAR *nodename);
	IDSIMPIN* __attribute__((fastcall)) (*newpin)( IDSIMCKT *this, EDX, IINSTANCE *, DSIMNODE node, CHAR *name, DWORD flags);
	EVENT* __attribute__((fastcall)) (*setclockcallback)( IDSIMCKT *this, EDX, ABSTIME starttime, RELTIME period, IDSIMMODEL *model, CALLBACKHANDLERFN func, EVENTID id);
};

/*************************************************************************/
// User defined popup window:
struct IUSERPOPUP_vtable
{
	CHAR* __attribute__((fastcall)) (*getprop)( IUSERPOPUP *this, EDX, CHAR *key);
	VOID __attribute__((fastcall)) (*setprop)( IUSERPOPUP *this, EDX, CHAR *key, CHAR *value);
	VOID __attribute__((fastcall)) (*setmsghlr)( IUSERPOPUP *this, EDX, IMSGHLR *handler);
	LRESULT __attribute__((fastcall)) (*callwindowproc)( IUSERPOPUP *this, EDX, MESSAGE msg, WPARAM warg, LPARAM larg);

};

// Debug\logging window:
struct IDEBUGPOPUP_vtable
{
	VOID __cdecl (*print)( IDEBUGPOPUP *this, CHAR *msg, ...);
	VOID __attribute__((fastcall)) (*dump)( IDEBUGPOPUP *this, EDX, const BYTE *ptr, UINT nbytes, UINT base);
};

// Memory dump window:
struct IMEMORYPOPUP_vtable
{
	VOID __attribute__((fastcall)) (*setmemory)( IMEMORYPOPUP *this, EDX, ADDRESS baseaddr, BYTE *data, UINT nbytes);
	VOID __attribute__((fastcall)) (*repaint)( IMEMORYPOPUP *this, EDX);
};

// Status display window:
struct ISTATUSPOPUP_vtable
{
	VOID __attribute__((fastcall)) (*setarea)( ISTATUSPOPUP *this, EDX, UINT columns, UINT rows, UINT border, BOOL copy);
	VOID __attribute__((fastcall)) (*setcursorto)( ISTATUSPOPUP *this, EDX, UINT pixelx, UINT pixely);
	UINT __attribute__((fastcall)) (*getcharwidth)( ISTATUSPOPUP *this, EDX);
	UINT __attribute__((fastcall)) (*getcharheight)( ISTATUSPOPUP *this, EDX);
	VOID __cdecl (*print1)( ISTATUSPOPUP *this, CHAR *message, ...);
	VOID __cdecl (*print2)( ISTATUSPOPUP *this, INT col, INT row, COLOUR textcolour, CHAR *msg, ...);
	VOID __attribute__((fastcall)) (*setctabstops1)( ISTATUSPOPUP *this, EDX, const INT ts[], INT n);
	VOID __attribute__((fastcall)) (*setctabstops2)( ISTATUSPOPUP *this, EDX, const INT ts[], INT n); /*FIXME: Is it error?*/
	VOID __attribute__((fastcall)) (*clear)( ISTATUSPOPUP *this, EDX, COLOUR bkcolour);
	BOOL __attribute__((fastcall)) (*setredraw)( ISTATUSPOPUP *this, EDX, BOOL on_off, BOOL redraw_now);
	VOID __attribute__((fastcall)) (*repaint)( ISTATUSPOPUP *this, EDX);
};

/*************************************************************************/

struct IMSGHLR_vtable
{
	LRESULT __attribute__((fastcall)) (*msghlr)( IMEMORYPOPUP *this, EDX, HWND, MESSAGE, WPARAM, LPARAM);
};

#endif
