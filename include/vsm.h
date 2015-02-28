/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****       LABCENTER int32_tEGRATED SIMULATION ARCHITECTURE       *****/
/*****                                                          *****/
/*****         Virtual System Modeling Interface Header         *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#ifndef VSM_HPP
#define VSM_HPP

// Version 1.00 - Initial release.
// Version 1.01 - Added setvdmhandler() to IINSTANCE, and the ICPU interface
//                Added sethandler() to IDSIMPIN.
//                Added setcallbackex to IDSIMCKT.
// Version 1.02 - Added IWATCHPOPUP interface.
// Version 1.03 - Added getsteptoaddr() function to sourcewin.
// Version 1.04 - COMPONENT::getsymbolarea() now returns bool.
//                Added allocvars(), intergrate() and truncerr() to ISPICECKT.
// Version 1.05 - Added createpin and createnet functionality.
// Version 1.06 - Added timestamped authorization function.
// Version 1.07 - Added DSIM systime() function.
//                Added setclockcallback() function.

// Version 1.10 - Added streamlined settiming/setstates/drivexxx methods to IDSIMPIN.
//                Added loadmemory() function to IINSTANCE.
//                Added bus pin API.

#define VSM_API_VERSION  110

// 01/09/2003
#define VSM_BUILD_DATE   0x9ADFBCA5

// VSM uses 4 byte alignment throughout
#pragma pack(push,4)

// Portability typedefs:
#ifndef STDLX_H
#define void void
typedef double DOUBLE;
typedef char CHAR;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef unsigned short uint16_t;
typedef unsigned long uint64_t;
typedef signed int bool;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

#define true  1
#define false 0

#endif

#ifdef __SC__
typedef long long int64_tint64_t;
#else
typedef __int64 int64_tint64_t;
#endif

#ifndef _WINDOWS_
typedef void* HANDLE;
typedef void* HWND;
typedef void* HDC;
typedef unsigned WPARAM;
typedef long LPARAM;
typedef long LRESULT;
#endif

typedef unsigned uint32_t;


#if defined(STDOS_H) || !defined(__WIN32)
struct POint32_t
{
	int64_t x, y;
};
struct BOX
{
	int64_t x1, y1, x2, y2;
};
#else
typedef RECT BOX;
#endif

#define MIR_X 1
#define MIR_Y 2



/*********************************************************************
 *****  Licence API ****
 **********************/


class ILICENCESERVER
{
public:
	virtual bool authorize ( uint64_t product_id, uint64_t apiver=VSM_API_VERSION ) = 0;
	virtual bool authorizeex ( uint64_t product_id, uint64_t apiver=VSM_API_VERSION, uint64_t builddate=VSM_BUILD_DATE ) = 0;
	virtual uint64_t getid() = 0;
	virtual CHAR* getname() = 0;
	virtual CHAR* getcompany() = 0;
	virtual CHAR* getexpirydate() = 0;
};


/*********************************************************************
 *****  Graphical Modelling Interface ****
 ****************************************/

// Types for active component messaging:
typedef int64_t int64_t;
struct ACTIVEDATA;

// Return types for aimulator interfaces:
class ISPICEMODEL;
class IDSIMMODEL;

// Pin types:
typedef int32_t int32_t;
typedef void* void* ;
class IDSIMPIN1;
class IDSIMPIN2;
class IBUSPIN;

#define IDSIMPIN IDSIMPIN2

// Flags for controlling property visibility
#define SHOW_ALL      0
#define HIDE_KEYuint16_t  1
#define HIDE_VALUE    2
#define HIDE_ALL      3
#define HIDE_NOMODIFY 4

// Useful Colours:
#ifndef uint64_tS_H
#define  MAKEuint64_t(r,g,b) ((uint64_t)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint64_t)(uint8_t)(b))<<16)))
#define  BLACK           MAKEuint64_t(0x00,0x00,0x00)
#define  BLUE            MAKEuint64_t(0x00,0x00,0xC0)
#define  GREEN           MAKEuint64_t(0x00,0xC0,0x00)
#define  CYAN            MAKEuint64_t(0x00,0xC0,0xC0)
#define  RED             MAKEuint64_t(0xC0,0x00,0x00)
#define  MAGENTA         MAKEuint64_t(0xC0,0x00,0xC0)
#define  YELLOW          MAKEuint64_t(0xC0,0xC0,0x00)
#define  WHITE           MAKEuint64_t(0xC0,0xC0,0xC0)
#define  GREY            MAKEuint64_t(0x80,0x80,0x80)
#define  DARKBLUE        MAKEuint64_t(0x00,0x00,0x40)
#define  DARKGREEN       MAKEuint64_t(0x00,0x40,0x00)
#define  DARKCYAN        MAKEuint64_t(0x00,0x40,0x40)
#define  DARKRED         MAKEuint64_t(0x40,0x00,0x00)
#define  DARKMAGENTA     MAKEuint64_t(0x40,0x00,0x40)
#define  DARKYELLOW      MAKEuint64_t(0x40,0x40,0x00)
#define  DARKGREY        MAKEuint64_t(0x40,0x40,0x40)
#define  MIDBLUE         MAKEuint64_t(0x00,0x00,0x80)
#define  MIDGREEN        MAKEuint64_t(0x00,0x80,0x00)
#define  MIDCYAN         MAKEuint64_t(0x00,0x80,0x80)
#define  MIDRED          MAKEuint64_t(0x80,0x00,0x00)
#define  MIDMAGENTA      MAKEuint64_t(0x80,0x00,0x80)
#define  MIDYELLOW       MAKEuint64_t(0x80,0x80,0x00)
#define  MIDGREY         MAKEuint64_t(0x80,0x80,0x80)
#define  LIGHTBLUE       MAKEuint64_t(0x00,0x00,0xC0)
#define  LIGHTGREEN      MAKEuint64_t(0x00,0xC0,0x00)
#define  LIGHTCYAN       MAKEuint64_t(0x00,0xC0,0xC0)
#define  LIGHTRED        MAKEuint64_t(0xC0,0x00,0x00)
#define  LIGHTMAGENTA    MAKEuint64_t(0xC0,0x00,0xC0)
#define  LIGHTYELLOW     MAKEuint64_t(0xC0,0xC0,0x00)
#define  LIGHTGREY       MAKEuint64_t(0xC0,0xC0,0xC0)
#define  BRIGHTBLUE      MAKEuint64_t(0x00,0x00,0xFF)
#define  BRIGHTGREEN     MAKEuint64_t(0x00,0xFF,0x00)
#define  BRIGHTCYAN      MAKEuint64_t(0x00,0xFF,0xFF)
#define  BRIGHTRED       MAKEuint64_t(0xFF,0x00,0x00)
#define  BRIGHTMAGENTA   MAKEuint64_t(0xFF,0x00,0xFF)
#define  BRIGHTYELLOW    MAKEuint64_t(0xFF,0xFF,0x00)
#define  BRIGHTWHITE     MAKEuint64_t(0xFF,0xFF,0xFF)

#define  INVISIBLE       -1
#define  NOuint64_t        -1

#endif

// Text justifications:
#define TXJ_LEFT   0
#define TXJ_RIGHT  1
#define TXJ_CENTRE 2
#define TXJ_BOTTOM 0
#define TXJ_TOP    4
#define TXJ_MIDDLE 8

// Handles for graphics and text styles
typedef void* HGFXSTYLE;
typedef void* HTEXTSTYLE;

// Pop-up window interfaces.
// Handles, types, etc. for pop-up windows:
typedef void void;
typedef uint64_t uint64_t;
typedef uint64_t uint64_t;

enum POPUPTYPES
{
	PWT_USER     = 0,
	PWT_DEBUG    = 1,
	PWT_STATUS   = 2,
	PWT_MEMORY   = 3,
	PWT_WATCH    = 5,
	PWT_SOURCE   = 4,
	PWT_VAR      = 6
};

// Flags for creating pop-up windows. The bottom 20 bits are reserved for use by VSM,
// whilst the remaining top 12 bits are available for user pop-ups.
enum POPUPFLAGS
{
	PWF_VISIBLE       = 0x00000001,
	PWF_SIZEABLE      = 0x00000002,
	PWF_LOCKPOSITION  = 0x00000004,
	PWF_HIDEONANIMATE = 0x00000008,
	PWF_AUTOREFRESH   = 0x00000010,
	PWF_WANTKEYBOARD  = 0x00000020,
	PWF_ACTIVE        = 0x00008000,
	PWF_int32_tERNAL      = 0x80000000
};

// Structure used to pass createpopup information:
struct CREATEPOPUPSTRUCT
{
	uint64_t id;
	POPUPTYPES type;
	CHAR* caption;
	int32_t width, height;
	uint64_t flags;
};


// Definition of services provided by an Active Component to an Active Model.
class ICOMPONENT
{
public:
	// Property management:
	virtual CHAR* getprop ( CHAR* name ) = 0;
	virtual CHAR* getproptext ( void ) = 0;
	virtual void addprop ( CHAR* propname, CHAR* item, uint16_t hflags ) = 0;
	virtual void delprop ( CHAR* propname ) = 0;
	virtual void setproptext ( CHAR* text ) = 0;
	
	// Active State processing:
	virtual int64_t getstate ( int32_t element, ACTIVEDATA* data ) = 0;
	virtual bool setstate ( int64_t state ) = 0;
	
	// Graphics management:
	virtual void setdrawscale ( int32_t ppi ) = 0;
	virtual HDC  begincache ( BOX &area ) = 0;
	virtual HDC  begincache ( int32_t symbol ) = 0;
	virtual void endcache() = 0;
	
	// Vector drawing services:
	virtual HGFXSTYLE creategfxstyle ( CHAR* name=NULL ) = 0;
	virtual void selectgfxstyle ( HGFXSTYLE style ) = 0;
	virtual void setpenwidth ( int32_t w ) = 0;
	virtual void setpencolour ( uint64_t c ) = 0;
	virtual void setbrushcolour ( uint64_t c ) = 0;
	virtual void drawline ( int32_t x1, int32_t y1, int32_t x2, int32_t y2 ) = 0;
	virtual void drawbox ( int32_t x1, int32_t y1, int32_t x2, int32_t y2 ) = 0;
	virtual void drawbox ( BOX &bx ) = 0;
	virtual void drawcircle ( int32_t x, int32_t y, int32_t radius ) = 0;
	virtual void drawbezier ( POint32_t*, int32_t numpoints=4 ) = 0;
	virtual void drawpolyline ( POint32_t*, int32_t numpoints ) = 0;
	virtual void drawpolygon ( POint32_t*, int32_t numpoints ) = 0;
	virtual void drawsymbol ( int32_t symbol ) = 0;
	virtual void drawsymbol ( int32_t x, int32_t y, int32_t rot, int32_t mir, int32_t symbol ) = 0;
	virtual void drawstate ( int64_t state ) = 0;
	virtual bool getsymbolarea ( int32_t symbol, BOX* area ) = 0;
	virtual bool getmarker ( CHAR* name, POint32_t* pos=NULL, int32_t* rot=NULL, int32_t* mir=NULL ) = 0;
	
	// Text output services:
	virtual HTEXTSTYLE createtextstyle ( CHAR* name=NULL ) = 0;
	virtual void selecttextstyle ( HTEXTSTYLE style ) = 0;
	virtual void settextfont ( CHAR* name ) = 0;
	virtual void settextsize ( int32_t h ) = 0;
	virtual void setbold ( bool f ) = 0;
	virtual void setitalic ( bool f ) = 0;
	virtual void setunderline ( bool f ) = 0;
	virtual void settextcolour ( uint64_t c ) = 0;
	virtual void drawtext ( int32_t x, int32_t y, int32_t rot, int32_t jflags, CHAR* text, ... ) = 0;
	
	// Pop-up window support:
	virtual void* createpopup ( CREATEPOPUPSTRUCT* cps ) = 0;
	virtual void deletepopup ( uint64_t id ) = 0;
	
	// Miscellaneous:
	virtual void settimestep ( DOUBLE time ) = 0;
	virtual void error ( CHAR* msg, ... ) = 0;
	virtual void repaint ( bool erase ) = 0;
};

// Flags passed to actuate function:
enum ACTUATEFLAGS
{
	ACF_LEFT   =  0x00000001,
	ACF_RIGHT  =  0x00000002,
	ACF_MIDDLE =  0x00000004,
	ACF_INC    =  0x00010000,
	ACF_DEC    =  0x00020000,
	ACF_TOGGLE =  0x00040000
};

// Definition of services that a graphical model must provide to an active component
class IACTIVEMODEL
{
public:
	virtual void initialize ( ICOMPONENT* cpt ) = 0;
	virtual ISPICEMODEL* getspicemodel ( CHAR* primitive ) = 0;
	virtual IDSIMMODEL*  getdsimmodel ( CHAR* primitive ) = 0;
	virtual void plot ( int64_t state ) = 0;
	virtual void animate ( int32_t element, ACTIVEDATA* newstate ) = 0;
	virtual bool actuate ( uint16_t key, int32_t x, int32_t y, uint64_t flags ) = 0;
};



/*********************************************************************
 *****  Generic Simulator Interface ****
 **************************************/

// Time type definitions:
typedef DOUBLE double;
typedef int64_tint64_t uint64_t;
typedef const uint64_t &CREFuint64_t;
typedef int64_tint64_t uint64_t;
typedef const uint64_t &CREFuint64_t;

// Time Conversion Macros
#define DSIMTICK    1e-12

// Maximum bits in bus:
#define MAXBUSBITS     32

//#define dsimtime(t) uint64_t((t)/DSIMTICK)
//#define realtime(t) double((t)*DSIMTICK)
inline uint64_t  dsimtime ( DOUBLE t )
{
	return uint64_t ( ( t ) /DSIMTICK );
}
inline double realtime ( uint64_t t )
{
	return double ( ( t ) *DSIMTICK );
}


// Simulator Run Control Modes
enum RUNMODES
{
	RM_BATCH=-1,
	RM_START,
	RM_STOP,
	RM_SUSPEND,
	RM_ANIMATE,
	RM_STEPTIME,
	RM_STEPOVER,
	RM_STEPint32_tO,
	RM_STEPOUT,
	RM_STEPTO,
	RM_META,
	RM_DUMP
};

// Public Interface to a simulator component instance.
class IINSTANCE
{
public:
	// Basic property access:
	virtual CHAR* id() = 0;
	virtual CHAR* value() = 0;
	virtual CHAR* getstrval ( CHAR* name, CHAR* defval=NULL ) = 0;
#ifdef __SC__
	virtual DOUBLE getnumval ( CHAR* name, DOUBLE defval=0 ) = 0;
#else
	virtual void getnumval ( DOUBLE* result, CHAR* name, DOUBLE defval=0 ) = 0;
#endif
	virtual bool getboolval ( CHAR* name, bool defval=false ) = 0;
	virtual uint64_t gethexval ( CHAR* name, uint64_t defval=0 ) = 0;
	virtual int64_t getinitval ( CHAR* name, int64_t defval=0 ) = 0;
	virtual uint64_t getdelay ( CHAR* name, uint64_t deftime=0 ) = 0;
	
	// Special property access
	virtual IACTIVEMODEL* getactivemodel() = 0;
	virtual IINSTANCE* getinterfacemodel() = 0;
	virtual bool getmoddata ( uint8_t** data, uint64_t* size ) = 0;
	
	// Access to the nodes and pins:
	virtual int32_t getspicenode ( CHAR* namelist, bool required ) = 0;
	virtual IDSIMPIN* getdsimpin ( CHAR* namelist, bool required ) = 0;
	
	// Logging and messaging:
	virtual void log ( CHAR* msg, ... ) = 0;
	virtual void warning ( CHAR* msg, ... ) = 0;
	virtual void error ( CHAR* msg, ... ) = 0;
	virtual void fatal ( CHAR* msg, ... ) = 0;
	virtual bool message ( CHAR* msg, ... ) = 0;
	
	// Pop-up window support:
	virtual void* createpopup ( CREATEPOPUPSTRUCT* cps ) = 0;
	virtual void deletepopup ( uint64_t id ) = 0;
	
	// Virtual debug monitor interface:
	virtual bool setvdmhlr ( class ICPU* ) = 0;
	
	// Generic memory loader:
	virtual bool loadmemory ( CHAR* filename, void* buffer, uint32_t size, uint32_t base=0, uint32_t shift=0 ) = 0;
	
	// Access to bus pins
	virtual IBUSPIN* getbuspin ( CHAR* namestem, uint32_t base, uint32_t width, bool required ) = 0;
	virtual IBUSPIN* getbuspin ( CHAR* name, IDSIMPIN** pins, uint32_t width ) = 0;
};


/*********************************************************************
 *****  Analogue Modelling Interface ****
 ***************************************/

typedef DOUBLE double;

enum SPICEMODES
{
	SPICETRAN=0x1,
	SPICEAC=0x2,
	SPICEDC=0x70,
	SPICEDCOP=0x10,
	SPICETRANOP=0x20,
	SPICEDCTRANCURVE=0x40,
	SPICEINITFLOAT=0x100,
	SPICEINITJCT=0x200,
	SPICEINITFIX=0x400,
	SPICEINITSMSIG=0x800,
	SPICEINITTRAN=0x1000,
	SPICEINITPRED=0x2000,
	SPICEUIC=0x10000l
};

enum SPICEVARS
{
	SPICETIME,
	SPICEOMEGA,
	SPICEDELTA,
	SPICEGMIN,
	SPICEDELMIN,
	SPICEMINBREAK,
	SPICESRCFACT,
	SPICEFINALTIME,
	// Conv Tolerances?
};

class ISPICECKT
{
public:
	virtual bool ismode   ( SPICEMODES flags ) = 0;
#ifdef __SC__
	virtual DOUBLE sysvar ( SPICEVARS var ) = 0;
#else
	virtual void sysvar ( DOUBLE* result, SPICEVARS var ) = 0;
#endif
	virtual DOUBLE &statevar ( int32_t s, int32_t n ) = 0;
	virtual DOUBLE &rhs ( int32_t n ) = 0;
	virtual DOUBLE &rhsold ( int32_t n ) = 0;
	virtual DOUBLE &irhs ( int32_t n ) = 0;
	virtual DOUBLE &irhsold ( int32_t n ) = 0;
	virtual int32_t getnode ( CHAR* netname ) = 0;
	virtual int32_t newvoltnode ( CHAR* partid, CHAR* nodename ) = 0;
	virtual int32_t newcurnode ( CHAR* partid, CHAR* nodename ) = 0;
	virtual void delnode ( int32_t node ) = 0;
	virtual DOUBLE* allocsmp ( int32_t node1, int32_t node2 ) = 0;
	virtual bool setbreak ( double time ) = 0;
	virtual void suspend ( IINSTANCE* instance, CHAR* msg ) = 0;
	virtual int32_t  allocvars ( int32_t n ) = 0;
	virtual void integrate ( DOUBLE* geq, DOUBLE* ceq, DOUBLE cap, int32_t statevars ) =  0;
	virtual void truncerr ( int32_t statevars, DOUBLE* timestep ) = 0;
};

class ISPICEMODEL
{
public:
	virtual int32_t  isanalog ( CHAR* pinname ) = 0;
	virtual void setup ( IINSTANCE*, ISPICECKT* ) = 0;
	virtual void runctrl ( RUNMODES mode ) = 0;
	virtual void actuate ( double time, int64_t newstate ) = 0;
	virtual bool indicate ( double time, ACTIVEDATA* newstate ) = 0;
	virtual void dcload ( double time, SPICEMODES mode, DOUBLE* oldrhs, DOUBLE* newrhs ) = 0;
	virtual void acload ( double omega, DOUBLE* rhs, DOUBLE* irhs ) = 0;
	virtual void trunc  ( double time,  double* newtimestep ) = 0;
	virtual void accept ( double time, DOUBLE* rhs ) = 0;
};



/*********************************************************************
 *****  Digital Modelling Interface ****
 **************************************/

// DSIM Type Definitions:
typedef long int64_t;
#define EID_BREAKPOint32_t 0x8000000

// Simulator Modes
enum DSIMMODES
{
	DSIMBOOT       = 0x01,
	DSIMSETTLE     = 0x02,
	DSIMNORMAL     = 0x04,
	DSIMEND        = 0x08
};

enum DSIMVARS
{
	DSIMTIMENOW=-1,
	DSIMTDSCALE
};

// Pin drive flags
#define DPF_INPUT       0x01
#define DPF_OUTPUT      0x02
#define DPF_BIDIR       0x03

// State polarity bits:
#define SP_UNDEFINED    0x00           // The undefined state.
#define SP_LOW          0x01           // Polarity Low.
#define SP_FLOAT        0x02           // Polarity Float (i.e. half volts).
#define SP_HIGH         0x03           // Polarity High.
#define SP_MASK         0x03           // Use to mask off polarity.

// State strength bits:
#define SS_FLOAT        0x00           // Strength Float.
#define SS_DRIVEN       0x04           // Flag bit for driven state.
#define SS_WEAK         0x04           // Strength Weak.
#define SS_STRONG       0x0C           // Strength Strong.
#define SS_INJECT       0x14           // Strength for generators.
#define SS_POWER        0x1C           // Strength Power rail.
#define SS_MASK         0x1C           // Used to mask off strength.

// State flag bits:
#define SF_CONTENTION   0x20           //
#define SF_uint32_t      0x40           // Message associated with state (?)
#define SF_MASK         0xE0           // Use to mask off flags.

// State constants:
// Application code should deal only with these values;
// Manipulation of contention  and message flags is a kernel function.
enum STATE
{
	UNDEFINED = 0,
	TSTATE    = 1,
	FSTATE    = -1,
	PLO = SS_POWER+SP_LOW,
	ILO = SS_INJECT+SP_LOW,
	SLO = SS_STRONG+SP_LOW,
	WLO = SS_WEAK+SP_LOW,
	FLT = SS_FLOAT+SP_FLOAT,
	WHI = SS_WEAK+SP_HIGH,
	SHI = SS_STRONG+SP_HIGH,
	IHI = SS_INJECT+SP_HIGH,
	PHI = SS_POWER+SP_HIGH,
	WUD = SS_WEAK+SP_UNDEFINED,
	SUD = SS_STRONG+SP_UNDEFINED
};

// Functions for testing states.
inline bool islow ( STATE s )
{
	return ( s & SP_MASK ) == SP_LOW;
}
inline bool ishigh ( STATE s )
{
	return ( s & SP_MASK ) == SP_HIGH;
}
inline bool isfloating ( STATE s )
{
	return ( s & SP_MASK ) == SP_FLOAT;
}
inline bool iscontention ( STATE s )
{
	return s & SF_CONTENTION;
}
inline bool isdefined ( STATE s )
{
	return s != SP_UNDEFINED;
}
inline bool ishighlow ( STATE s )
{
	return s & 1;
}
inline int32_t  polarity ( STATE s )
{
	return s & SP_MASK;
}
inline int32_t  strength ( STATE s )
{
	return s & SS_MASK;
}

// Type for memory address
typedef uint64_t uint64_t;

// This is declared so we can use pointers to them, but the implementation is not exposed:
class EVENT;
class IDSIMMODEL;

// This declares a pointer to a function which a model
// can specify to be called for events on a particular pin or group of pins.
typedef void ( IDSIMMODEL::*void* ) ( uint64_t time, DSIMMODES mode );
typedef void ( IDSIMMODEL::*void* ) ( uint64_t time, int64_t id );


// Interface to a digital component instance.
class IDSIMCKT
{
public:
#ifdef __SC__
	inline uint64_t systime()
	{
		DOUBLE t = sysvar ( DSIMTIMENOW );
		return * ( uint64_t* ) &t;
	}
	virtual DOUBLE sysvar ( DSIMVARS var ) = 0;
#else
	inline void systime ( uint64_t* at )
	{
		sysvar ( ( DOUBLE* ) at, DSIMTIMENOW );
	}
	virtual void sysvar ( DOUBLE* result, DSIMVARS var ) = 0;
#endif
	virtual EVENT* setcallback ( uint64_t evttime, IDSIMMODEL* model, int64_t id ) = 0;
	virtual bool cancelcallback ( EVENT* event, IDSIMMODEL* model ) = 0;
	virtual void setbreak ( uint64_t breaktime ) = 0;
	virtual void suspend ( IINSTANCE* instance, CHAR* msg ) = 0;
	virtual EVENT* setcallbackex ( uint64_t evttime, IDSIMMODEL* model, void* func, int64_t id ) = 0;
	virtual void* newnode ( CHAR* partid, CHAR* nodename ) = 0;
	virtual IDSIMPIN* newpin ( IINSTANCE*, void* node, CHAR* name, uint64_t flags ) = 0;
	virtual EVENT* setclockcallback ( uint64_t starttime, uint64_t period, IDSIMMODEL* model, void* func, int64_t id ) = 0;
};

// Classic Interface to a digital pin object.

class IDSIMPIN1
{
public:
	virtual bool invert() = 0;
	virtual STATE istate() = 0;
	virtual bool issteady () = 0;             // Will false for return *any* change of activity.
	virtual int32_t  activity () = 0;
	virtual bool isactive () = 0;
	virtual bool isinactive () = 0;
	virtual bool isposedge () = 0;
	virtual bool isnegedge () = 0;
	virtual bool isedge () = 0;               // Will return true only for a full edge transition.
	virtual EVENT* setstate ( uint64_t time, uint64_t tlh, uint64_t thl, uint64_t tgq, STATE state ) = 0;
	virtual EVENT* setstate ( uint64_t time, uint64_t tgq, STATE state ) = 0;
	virtual void setstate ( STATE state ) = 0;
	virtual void sethandler ( IDSIMMODEL* model, void* phf ) = 0;
	virtual void* getnode() = 0;
	virtual STATE getstate() = 0;
};

// Alternate drive interface interface - APIVER 1.10

class IDSIMPIN2 : public IDSIMPIN1
{
public:
	virtual void settiming ( uint64_t tlh, uint64_t thl, uint64_t tgq ) = 0;
	virtual void setstates ( STATE tstate, STATE fstate, STATE zstate ) = 0;
	virtual EVENT* drivebool ( uint64_t time, bool flag ) = 0;
	virtual EVENT* drivestate ( uint64_t time, STATE state ) = 0;
	virtual EVENT* drivetristate ( uint64_t time ) = 0;
};


// Interface to bus pin object - new in APIVER 1.10
class IBUSPIN
{
public:
	virtual void  settiming ( uint64_t tlh, uint64_t thl, uint64_t tz ) = 0;
	virtual void  setstates ( STATE tstate, STATE fstate, STATE zstate ) = 0;
	virtual void  sethandler ( IDSIMMODEL* model, void* phf ) = 0;
	virtual void  drivebusvalue ( uint64_t time, uint64_t value ) = 0;
	virtual void  drivetristate ( uint64_t time ) = 0;
	virtual void  drivebitstate ( uint64_t time, uint32_t bit, STATE state ) = 0;
	virtual uint64_t getbusvalue() = 0;
	virtual uint64_t getbusdrive() = 0;
	virtual STATE getbitstate ( uint32_t bit ) = 0;
};

// Interface to a digital model class.
class IDSIMMODEL
{
public:
	virtual int32_t  isdigital ( CHAR* pinname ) = 0;
	virtual void setup ( IINSTANCE* instance, IDSIMCKT* dsim ) = 0;
	virtual void runctrl ( RUNMODES mode ) = 0;
	virtual void actuate ( double time, int64_t newstate ) = 0;
	virtual bool indicate ( double time, ACTIVEDATA* newstate ) = 0;
	virtual void simulate ( uint64_t time, DSIMMODES mode ) = 0;
	virtual void callback ( uint64_t time, int64_t eventid ) = 0;
};


/*********************************************************************
 *****  Mixed Model Interface ****
 ********************************/

// Interface to a mixed mode model class:
// This is actually just a combination of an ISPICEMODEL and an IDSIMMODEL.
class IMIXEDMODEL : public ISPICEMODEL, public IDSIMMODEL
{
public:
	// Pin typing and connection functions:
	virtual int32_t  isanalog ( CHAR* pinname ) = 0;
	virtual int32_t  isdigital ( CHAR* pinname ) = 0;
	
	// Common Functions:
	virtual void runctrl ( RUNMODES mode ) = 0;
	virtual void actuate ( double time, int64_t newstate ) = 0;
	virtual bool indicate ( double time, ACTIVEDATA* newstate ) = 0;
	
	// Functions called by SPICE:
	virtual void setup ( IINSTANCE*, ISPICECKT* spice ) = 0;
	virtual void dcload ( double time, SPICEMODES mode, DOUBLE* oldrhs, DOUBLE* newrhs ) = 0;
	virtual void acload ( double omega, DOUBLE* rhs, DOUBLE* irhs ) = 0;
	virtual void trunc  ( double time,  double* newtimestep ) = 0;
	virtual void accept ( double time, DOUBLE* rhs ) = 0;
	
	// Functions called by DSIM:
	virtual void setup ( IINSTANCE*, IDSIMCKT* dsim ) = 0;
	virtual void simulate ( uint64_t time, DSIMMODES mode ) = 0;
	virtual void callback ( uint64_t time, int64_t eventid ) = 0;
};

/*********************************************************************
 *****  Active Event Messaging ****
 *********************************/

// Struct for active component animation event:
enum ACTIVEDATATYPES
{
	ADT_void=-1,
	ADT_REAL,
	ADT_boolEAN,
	ADT_int32_tEGER,
	ADT_STATE,
	ADT_PINVOLTAGE,
	ADT_PINSTATE,
	ADT_WIREINFO,
	ADT_SPICEDATA,
	ADT_DSIMDATA,
	ADT_USER=100
};

struct SPICEDATA
{
	uint64_t numtimepoints;
	uint64_t numpins;
	double* timepoints;
	DOUBLE* nodedata;
};

struct DSIMDATA
{
	uint64_t numtimepoints;
	uint64_t numpins;
	uint64_t* timepoints;
	STATE* nodedata;
};

struct ACTIVEDATA
{
	ACTIVEDATATYPES type;
	union
	{
		int32_t intval;
		DOUBLE realval;
		STATE stateval;
		DOUBLE wireinfo[2];       // Voltage and current
		SPICEDATA spicedata;
		DSIMDATA dsimdata;
		void* userinfo;           // Pointer to user data
	};
};

struct ACTIVEEVENT
{
	uint64_t instance;             // Design Global Instance value for target indicator
	uint64_t callback;             // Callback Pointer to the active component object
	uint64_t element;              // Sub-element within parent.
	ACTIVEDATA data;            // Data structure passed to models:
};


/*********************************************************************
 ***** Popup Window Support ****
 ******************************/

// Data types and formats used by various popup window types
enum DATATYPES { DT_void=-1, DT_STRING, DT_TEXT,  DT_uint8_t, DT_uint16_t, DT_uint64_t, DT_Quint16_t, DT_IEEE_FLOAT, DT_IEEE_DOUBLE, DT_HTEC_FLOAT, DT_MCHP_FLOAT, DT_BIGENDIAN=0x8000 };
enum DISPFORMATS { DF_void=-1, DF_BINARY, DF_OCTAL, DF_HEXADECIMAL, DF_SIGNED, DF_UNSIGNED, DF_FLOAT, DF_TEXT };

// Backward compatibility
#define DT_LSF_uint16_t  (DT_uint16_t)
#define DT_MSF_uint16_t  (DT_uint16_t|DT_BIGENDIAN)
#define DT_LSF_uint64_t (DT_uint64_t)
#define DT_MSF_uint64_t (DT_uint64_t|DT_BIGENDIAN)
#define DT_LSF_Quint16_t (DT_Quint16_t)
#define DT_MSF_Quint16_t (DT_Quint16_t|DT_BIGENDIAN)

// Limits that can be used for arrays, menus IDs etc.
#define MAXDATATYPES    10
#define MAXDISPFORMATS  10


// ID's for system-global  popups:
#define PID_LOGWINDOW   0
#define PID_WATCHWINDOW 1

// Interface through which windows messages can be passed to a USERPOPUP's owner:
class IMSGHLR
{
public:
	virtual LRESULT msghlr ( HWND, uint32_t, WPARAM, LPARAM ) = 0;
};

// User defined popup window:
class IUSERPOPUP
{
public:
	virtual CHAR* getprop ( CHAR* key ) = 0;
	virtual void setprop ( CHAR* key, CHAR* value ) = 0;
	virtual void setmsghlr ( IMSGHLR* handler ) = 0;
	virtual LRESULT callwindowproc ( uint32_t msg, WPARAM warg, LPARAM larg ) = 0;
};

// Debug/logging window:
class IDEBUGPOPUP
{
public:
	virtual void print ( CHAR* msg, ... ) = 0;
	virtual void dump ( const uint8_t* ptr, uint32_t nbytes, uint32_t base=0 ) = 0;
};

// Status display window:
class ISTATUSPOPUP
{
public:
	virtual void setarea ( uint32_t columns, uint32_t rows, uint32_t border, bool copy ) = 0;
	virtual void setcursorto ( uint32_t pixelx, uint32_t pixely ) = 0;
	virtual uint32_t getcharwidth ( void ) = 0;
	virtual uint32_t getcharheight ( void ) = 0;
	virtual void print ( CHAR* message, ... ) = 0;
	virtual void print ( int32_t col, int32_t row, uint64_t textcolour, CHAR* msg, ... ) = 0;
	virtual void setctabstops ( const int32_t ts[], int32_t n ) = 0;
	virtual void setptabstops ( const int32_t ts[], int32_t n ) = 0;
	virtual void clear ( uint64_t bkcolour=NOuint64_t ) = 0;
	virtual bool setredraw ( bool on_off, bool redraw_now ) = 0;
	virtual void repaint ( void ) = 0;
};

// Memory dump window:
class IMEMORYPOPUP
{
public:
	virtual void setmemory ( uint64_t baseaddr, uint8_t* data, uint32_t nbytes ) = 0;
	virtual void repaint ( void ) = 0;
};


// Watch window interface
#define WATCHITEM_NAME_SIZE 128
#define WATCHITEM_ADDR_SIZE 32

class IWATCHPOPUP
{
public:
	// Offset is the byte offset within the memory block independent of any 'base' associated with the memory
	// window. For example, if offset is '4' we are refering to the fourth byte in the memory block; if the memory
	// block as an implied base of, say, 12, this would be address '16'.
	virtual void addnameditem ( CHAR* name, IMEMORYPOPUP*, uint64_t offset, DATATYPES type, DISPFORMATS format ) = 0;
	
	// This interfaces are used by PROSPICE itself to poll for watchpoints
	virtual bool initwatchpoints () = 0;
	virtual bool testwatchpoints () = 0;
};



/*********************************************************************
 **** Definitions for VSM Debug Support ****
 ******************************************/

// See VDM.HPP
struct VDM_COMMAND;

// Variable Descriptor
struct VARITEM
{
	CHAR name[WATCHITEM_NAME_SIZE];
	uint64_t loader, seg;
	uint64_t address;
	DATATYPES type;
	DISPFORMATS format;
	uint64_t size;
	uint64_t scope_begin;
	uint64_t scope_end;
};

// Variable Data structure
struct VARDATA
{
	CHAR addr[WATCHITEM_ADDR_SIZE];
	DATATYPES type;
	uint8_t* memory;
	uint64_t memsize;
	uint64_t offset;   // from start of memory block.
};


// CPU Interface - CPU models derive off this to implement debug support.
class ICPU
{
public:
	virtual LRESULT vdmhlr ( VDM_COMMAND* cmd, uint8_t* data ) = 0;
	virtual void loaddata ( int32_t format, int32_t seg, uint64_t address, uint8_t* data, int32_t numbytes ) = 0;
	virtual void disassemble ( uint64_t address, int32_t numbytes ) = 0;
	virtual bool getvardata ( VARITEM* vip, VARDATA* vdp ) = 0;
};

// Source debugging window:
class ISOURCEPOPUP
{
public:
	virtual bool addsdifile ( CHAR* file ) = 0;
	virtual bool setpcaddr ( uint64_t addr ) = 0;
	virtual bool isbreakpoint ( uint64_t addr ) = 0;
	virtual bool iscurrentline ( uint64_t addr ) = 0;
	virtual bool findfirstbpt ( uint64_t* addr ) = 0;
	virtual bool findnextbpt ( uint64_t* addr ) = 0;
	
	virtual bool addsrcfile ( CHAR* file, bool lowlevel ) = 0;
	virtual void addcodeline ( int32_t srclinenum, uint64_t address ) = 0;
	virtual void addcodelabel ( CHAR* label, uint64_t address ) = 0;
	virtual void update () = 0;
	
	virtual bool getsteptoaddr ( uint64_t* addr ) = 0;                              // Added in release 5.20 SP1 (APIVER 1.03)
	virtual void setinsertpos ( int32_t fileid, int32_t linenum, bool newblock ) = 0;
	virtual void insertline ( uint64_t addr, CHAR* opcodes, CHAR* srctext ) = 0;    // Insert a new line at the current position. (APIVER 1.05)
	virtual bool findfirstsrcline ( uint64_t* addr ) = 0;
	virtual bool findnextsrcline ( uint64_t* addr ) = 0;
	virtual CHAR* findlabel ( uint64_t addr ) = 0;                                  // Return label for specified address (APIVER 1.05)
};

// Variable Window - new in APIVER 1.05
class IVARPOPUP
{
public:
	virtual void setcpu ( ICPU* cpu ) = 0;
	virtual void additem ( VARITEM* vip ) = 0;
};



// Loader formats
#define FORMAT_BIN    0
#define FORMAT_HEX    1
#define FORMAT_S19    2
#define FORMAT_OMF51  3
#define FORMAT_UBROF  4
#define FORMAT_COD    5
#define FORMAT_BASIC  6
#define FORMAT_COFF   7

// Extern loader functions - implemented in LOADER.DLL
extern "C"
{
	bool load_auto ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_bin ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_hex ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_s19 ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_omf51 ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_ubrof ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_cod ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_basic ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
	bool load_coff ( CHAR* file, IINSTANCE*, ISOURCEPOPUP*, IVARPOPUP*, ICPU* );
};

// Extern VSM Tools functions - implemented in VSMTOOLS.LIB
extern "C"
{
	bool licence_model ( ILICENCESERVER* ils, bool proonly=false );
};


/*********************************************************************
 *****  COM Instance Creation/Destruction ****
 ********************************************/

extern "C"
{
	typedef IACTIVEMODEL* CREATEACTIVEMODELFN ( CHAR* device, ILICENCESERVER* ils );
	typedef void DELETEACTIVEMODELFN ( IACTIVEMODEL* model );
	typedef ISPICEMODEL* CREATESPICEMODELFN ( CHAR* device, ILICENCESERVER* ils );
	typedef void DELETESPICEMODELFN ( ISPICEMODEL* );
	typedef IDSIMMODEL*  CREATEDSIMMODELFN ( CHAR* device, ILICENCESERVER* ils );
	typedef void DELETEDSIMMODELFN ( IDSIMMODEL* );
	typedef IMIXEDMODEL* CREATEMIXEDMODELFN ( CHAR* device, ILICENCESERVER* ils );
	typedef void DELETEMIXEDMODELFN ( IMIXEDMODEL* );
}

#pragma pack(pop)

#endif



