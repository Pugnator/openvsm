// OpenVSM description of the Proteus VSM model interface.
//
// This file is part of OpenVSM (GPL-2.0-or-later). It is an independent
// declaration of the binary interface through which Proteus loads a VSM model
// DLL, written so that OpenVSM can be built and distributed without the
// proprietary SDK. Only the parts OpenVSM uses are declared. The interface
// facts come from the documented VSM API (the VSM SDK manual).
//
// Binary compatibility rules, verified by model/tests/abi/vsm_abi_test.cc
// against the vendor header when it is available locally:
// - virtual methods must stay in exactly this order, overloads included;
// - structures use 4-byte packing;
// - enumerator and constant values must not change.
// Reordering or "tidying" declarations here breaks every model at run time.

#pragma once
#ifndef OPENVSM_SDK_OPENVSM_HPP
#define OPENVSM_SDK_OPENVSM_HPP

#include "openvsm_types.hpp"

// API revision this description corresponds to, and the build stamp that is
// passed to ILICENCESERVER::authorizeex.
#define VSM_API_VERSION 110
#define VSM_BUILD_DATE 0x9ADFBCA5

#pragma pack(push, 4)

// ---------------------------------------------------------------------------
// Licensing

class ILICENCESERVER
{
  public:
    virtual BOOL authorize(DWORD product_id, DWORD apiver = VSM_API_VERSION) = 0;
    virtual BOOL authorizeex(DWORD product_id, DWORD apiver = VSM_API_VERSION, DWORD builddate = VSM_BUILD_DATE) = 0;
    virtual DWORD getid() = 0;
    virtual CHAR *getname() = 0;
    virtual CHAR *getcompany() = 0;
    virtual CHAR *getexpirydate() = 0;
};

// ---------------------------------------------------------------------------
// Common declarations

typedef LONG ACTIVESTATE;
struct ACTIVEDATA;

class ISPICEMODEL; // analogue models are not described here
class IDSIMMODEL;
class IDSIMPIN1;
class IDSIMPIN2;
class IBUSPIN;
class ICPU;

typedef INT SPICENODE;
typedef VOID *DSIMNODE;
typedef IDSIMPIN2 IDSIMPIN; // the current pin interface

// Property visibility flags for ICOMPONENT::addprop.
#define SHOW_ALL 0
#define HIDE_KEYWORD 1
#define HIDE_VALUE 2
#define HIDE_ALL 3
#define HIDE_NOMODIFY 4

// Colours are 0x00BBGGRR.
#define MAKECOLOUR(r, g, b) ((COLOUR)((DWORD)(BYTE)(r) | ((DWORD)(BYTE)(g) << 8) | ((DWORD)(BYTE)(b) << 16)))
#define BLACK MAKECOLOUR(0x00, 0x00, 0x00)
#define BLUE MAKECOLOUR(0x00, 0x00, 0xC0)
#define GREEN MAKECOLOUR(0x00, 0xC0, 0x00)
#define CYAN MAKECOLOUR(0x00, 0xC0, 0xC0)
#define RED MAKECOLOUR(0xC0, 0x00, 0x00)
#define MAGENTA MAKECOLOUR(0xC0, 0x00, 0xC0)
#define YELLOW MAKECOLOUR(0xC0, 0xC0, 0x00)
#define WHITE MAKECOLOUR(0xC0, 0xC0, 0xC0)
#define GREY MAKECOLOUR(0x80, 0x80, 0x80)
#define BRIGHTBLUE MAKECOLOUR(0x00, 0x00, 0xFF)
#define BRIGHTGREEN MAKECOLOUR(0x00, 0xFF, 0x00)
#define BRIGHTCYAN MAKECOLOUR(0x00, 0xFF, 0xFF)
#define BRIGHTRED MAKECOLOUR(0xFF, 0x00, 0x00)
#define BRIGHTMAGENTA MAKECOLOUR(0xFF, 0x00, 0xFF)
#define BRIGHTYELLOW MAKECOLOUR(0xFF, 0xFF, 0x00)
#define BRIGHTWHITE MAKECOLOUR(0xFF, 0xFF, 0xFF)
#define INVISIBLE -1
#define NOCOLOUR -1

// Text justification flags for ICOMPONENT::drawtext.
#define TXJ_LEFT 0
#define TXJ_RIGHT 1
#define TXJ_CENTRE 2
#define TXJ_BOTTOM 0
#define TXJ_TOP 4
#define TXJ_MIDDLE 8

// ---------------------------------------------------------------------------
// Popup windows

typedef VOID *HGFXSTYLE;
typedef VOID *HTEXTSTYLE;
typedef VOID IPOPUP; // cast to the interface that matches the popup type
typedef DWORD POPUPID;
typedef DWORD INSTANCEID;

enum POPUPTYPES
{
    PWT_USER = 0,
    PWT_DEBUG = 1,
    PWT_STATUS = 2,
    PWT_MEMORY = 3,
    PWT_SOURCE = 4,
    PWT_WATCH = 5,
    PWT_VAR = 6
};

// The low 20 bits are defined by Proteus; the top 12 are free for user popups.
enum POPUPFLAGS
{
    PWF_VISIBLE = 0x00000001,
    PWF_SIZEABLE = 0x00000002,
    PWF_LOCKPOSITION = 0x00000004,
    PWF_HIDEONANIMATE = 0x00000008,
    PWF_AUTOREFRESH = 0x00000010,
    PWF_WANTKEYBOARD = 0x00000020,
    PWF_ACTIVE = 0x00008000,
    PWF_INTERNAL = 0x80000000
};

struct CREATEPOPUPSTRUCT
{
    POPUPID id;
    POPUPTYPES type;
    CHAR *caption;
    INT width; // characters for text popups, pixels for PWT_USER
    INT height;
    DWORD flags; // POPUPFLAGS
};

// Proteus-wide popup identifiers.
#define PID_LOGWINDOW 0
#define PID_WATCHWINDOW 1

// ---------------------------------------------------------------------------
// Graphical modelling: services an active component offers its model.

class ICOMPONENT
{
  public:
    virtual CHAR *getprop(CHAR *name) = 0;
    virtual CHAR *getproptext(VOID) = 0;
    virtual VOID addprop(CHAR *propname, CHAR *item, WORD hflags) = 0;
    virtual VOID delprop(CHAR *propname) = 0;
    virtual VOID setproptext(CHAR *text) = 0;

    virtual ACTIVESTATE getstate(INT element, ACTIVEDATA *data) = 0;
    virtual BOOL setstate(ACTIVESTATE state) = 0;

    virtual VOID setdrawscale(INT ppi) = 0;
    virtual HDC begincache(BOX &area) = 0;
    virtual HDC begincache(INT symbol) = 0;
    virtual VOID endcache() = 0;

    virtual HGFXSTYLE creategfxstyle(CHAR *name = nullptr) = 0;
    virtual VOID selectgfxstyle(HGFXSTYLE style) = 0;
    virtual VOID setpenwidth(INT w) = 0;
    virtual VOID setpencolour(COLOUR c) = 0;
    virtual VOID setbrushcolour(COLOUR c) = 0;
    virtual VOID drawline(INT x1, INT y1, INT x2, INT y2) = 0;
    virtual VOID drawbox(INT x1, INT y1, INT x2, INT y2) = 0;
    virtual VOID drawbox(BOX &bx) = 0;
    virtual VOID drawcircle(INT x, INT y, INT radius) = 0;
    virtual VOID drawbezier(POINT *points, INT numpoints = 4) = 0;
    virtual VOID drawpolyline(POINT *points, INT numpoints) = 0;
    virtual VOID drawpolygon(POINT *points, INT numpoints) = 0;
    virtual VOID drawsymbol(INT symbol) = 0;
    virtual VOID drawsymbol(INT x, INT y, INT rot, INT mir, INT symbol) = 0;
    virtual VOID drawstate(ACTIVESTATE state) = 0;
    virtual BOOL getsymbolarea(INT symbol, BOX *area) = 0;
    virtual BOOL getmarker(CHAR *name, POINT *pos = nullptr, INT *rot = nullptr, INT *mir = nullptr) = 0;

    virtual HTEXTSTYLE createtextstyle(CHAR *name = nullptr) = 0;
    virtual VOID selecttextstyle(HTEXTSTYLE style) = 0;
    virtual VOID settextfont(CHAR *name) = 0;
    virtual VOID settextsize(INT h) = 0;
    virtual VOID setbold(BOOL f) = 0;
    virtual VOID setitalic(BOOL f) = 0;
    virtual VOID setunderline(BOOL f) = 0;
    virtual VOID settextcolour(COLOUR c) = 0;
    virtual VOID drawtext(INT x, INT y, INT rot, INT jflags, CHAR *text, ...) = 0;

    virtual IPOPUP *createpopup(CREATEPOPUPSTRUCT *cps) = 0;
    virtual VOID deletepopup(POPUPID id) = 0;

    virtual VOID settimestep(DOUBLE time) = 0;
    virtual VOID error(CHAR *msg, ...) = 0;
    virtual VOID repaint(BOOL erase) = 0;
};

// Mouse and key flags passed to IACTIVEMODEL::actuate.
enum ACTUATEFLAGS
{
    ACF_LEFT = 0x00000001,
    ACF_RIGHT = 0x00000002,
    ACF_MIDDLE = 0x00000004,
    ACF_INC = 0x00010000,
    ACF_DEC = 0x00020000,
    ACF_TOGGLE = 0x00040000
};

// What a graphical model implements.
class IACTIVEMODEL
{
  public:
    virtual VOID initialize(ICOMPONENT *cpt) = 0;
    virtual ISPICEMODEL *getspicemodel(CHAR *primitive) = 0;
    virtual IDSIMMODEL *getdsimmodel(CHAR *primitive) = 0;
    virtual VOID plot(ACTIVESTATE state) = 0;
    virtual VOID animate(INT element, ACTIVEDATA *newstate) = 0;
    virtual BOOL actuate(WORD key, INT x, INT y, DWORD flags) = 0;
};

// Symbol mirroring flags.
#define MIR_X 1
#define MIR_Y 2

// ---------------------------------------------------------------------------
// Simulation time and run control

typedef DOUBLE REALTIME;  // seconds
typedef LONGLONG ABSTIME; // picoseconds
typedef LONGLONG RELTIME; // picoseconds
typedef const ABSTIME &CREFABSTIME;
typedef const RELTIME &CREFRELTIME;

#define DSIMTICK 1e-12
#define MAXBUSBITS 32

inline ABSTIME dsimtime(DOUBLE seconds)
{
    return ABSTIME(seconds / DSIMTICK);
}

inline REALTIME realtime(ABSTIME ticks)
{
    return REALTIME(ticks * DSIMTICK);
}

// RM_BATCH is never passed to runctrl; it is only a useful initial value.
enum RUNMODES
{
    RM_BATCH = -1,
    RM_START,
    RM_STOP,
    RM_SUSPEND,
    RM_ANIMATE,
    RM_STEPTIME,
    RM_STEPOVER,
    RM_STEPINTO,
    RM_STEPOUT,
    RM_STEPTO,
    RM_META,
    RM_DUMP
};

// The component instance a model is attached to.
class IINSTANCE
{
  public:
    virtual CHAR *id() = 0;
    virtual CHAR *value() = 0;
    virtual CHAR *getstrval(CHAR *name, CHAR *defval = nullptr) = 0;
    virtual VOID getnumval(DOUBLE *result, CHAR *name, DOUBLE defval = 0) = 0;
    virtual BOOL getboolval(CHAR *name, BOOL defval = FALSE) = 0;
    virtual DWORD gethexval(CHAR *name, DWORD defval = 0) = 0;
    virtual LONG getinitval(CHAR *name, LONG defval = 0) = 0;
    virtual RELTIME getdelay(CHAR *name, RELTIME deftime = 0) = 0;

    virtual IACTIVEMODEL *getactivemodel() = 0;
    virtual IINSTANCE *getinterfacemodel() = 0;
    virtual BOOL getmoddata(BYTE **data, DWORD *size) = 0;

    // Pin lookup. namelist may hold comma-separated alternatives.
    virtual SPICENODE getspicenode(CHAR *namelist, BOOL required) = 0;
    virtual IDSIMPIN *getdsimpin(CHAR *namelist, BOOL required) = 0;

    // Simulation log and status bar; printf-style.
    virtual VOID log(CHAR *msg, ...) = 0;
    virtual VOID warning(CHAR *msg, ...) = 0;
    virtual VOID error(CHAR *msg, ...) = 0;
    virtual VOID fatal(CHAR *msg, ...) = 0;
    virtual BOOL message(CHAR *msg, ...) = 0;

    virtual IPOPUP *createpopup(CREATEPOPUPSTRUCT *cps) = 0;
    virtual VOID deletepopup(POPUPID id) = 0;

    virtual BOOL setvdmhlr(ICPU *cpu) = 0;

    virtual BOOL loadmemory(CHAR *filename, VOID *buffer, UINT size, UINT base = 0, UINT shift = 0) = 0;

    virtual IBUSPIN *getbuspin(CHAR *namestem, UINT base, UINT width, BOOL required) = 0;
    virtual IBUSPIN *getbuspin(CHAR *name, IDSIMPIN **pins, UINT width) = 0;
};

// ---------------------------------------------------------------------------
// Digital modelling

typedef long EVENTID; // values with the top bit set are reserved for Proteus
#define EID_BREAKPOINT 0x8000000

enum DSIMMODES
{
    DSIMBOOT = 0x01,
    DSIMSETTLE = 0x02,
    DSIMNORMAL = 0x04,
    DSIMEND = 0x08
};

enum DSIMVARS
{
    DSIMTIMENOW = -1,
    DSIMTDSCALE
};

// Direction flags for IDSIMCKT::newpin.
#define DPF_INPUT 0x01
#define DPF_OUTPUT 0x02
#define DPF_BIDIR 0x03

// A logic state combines a polarity (bits 0-1), a strength (bits 2-4) and
// kernel flags (bits 5-7).
#define SP_UNDEFINED 0x00
#define SP_LOW 0x01
#define SP_FLOAT 0x02
#define SP_HIGH 0x03
#define SP_MASK 0x03

#define SS_FLOAT 0x00
#define SS_DRIVEN 0x04
#define SS_WEAK 0x04
#define SS_STRONG 0x0C
#define SS_INJECT 0x14
#define SS_POWER 0x1C
#define SS_MASK 0x1C

#define SF_CONTENTION 0x20
#define SF_MESSAGE 0x40
#define SF_MASK 0xE0

enum STATE
{
    UNDEFINED = 0,
    TSTATE = 1,
    FSTATE = -1,
    PLO = SS_POWER + SP_LOW,
    ILO = SS_INJECT + SP_LOW,
    SLO = SS_STRONG + SP_LOW,
    WLO = SS_WEAK + SP_LOW,
    FLT = SS_FLOAT + SP_FLOAT,
    WHI = SS_WEAK + SP_HIGH,
    SHI = SS_STRONG + SP_HIGH,
    IHI = SS_INJECT + SP_HIGH,
    PHI = SS_POWER + SP_HIGH,
    WUD = SS_WEAK + SP_UNDEFINED,
    SUD = SS_STRONG + SP_UNDEFINED
};

inline BOOL islow(STATE s)
{
    return (s & SP_MASK) == SP_LOW;
}

inline BOOL ishigh(STATE s)
{
    return (s & SP_MASK) == SP_HIGH;
}

inline BOOL isfloating(STATE s)
{
    return (s & SP_MASK) == SP_FLOAT;
}

inline BOOL iscontention(STATE s)
{
    return s & SF_CONTENTION;
}

inline BOOL isdefined(STATE s)
{
    return s != SP_UNDEFINED;
}

inline BOOL ishighlow(STATE s)
{
    return s & 1;
}

inline INT polarity(STATE s)
{
    return s & SP_MASK;
}

inline INT strength(STATE s)
{
    return s & SS_MASK;
}

typedef DWORD ADDRESS;

class EVENT; // opaque to models

// Handlers for IDSIMPIN1::sethandler, IBUSPIN::sethandler and
// IDSIMCKT::setcallbackex. They are passed by value, so their size is part of
// the binary interface: 4 bytes with MSVC's default pointer-to-member
// representation. Do not build with /vmg or /vmv.
typedef VOID (IDSIMMODEL::*PINHANDLERFN)(ABSTIME time, DSIMMODES mode);
typedef VOID (IDSIMMODEL::*CALLBACKHANDLERFN)(ABSTIME time, EVENTID id);

// Services of the digital simulator.
class IDSIMCKT
{
  public:
    inline VOID systime(ABSTIME *at)
    {
        sysvar(reinterpret_cast<DOUBLE *>(at), DSIMTIMENOW);
    }
    virtual VOID sysvar(DOUBLE *result, DSIMVARS var) = 0;
    virtual EVENT *setcallback(ABSTIME evttime, IDSIMMODEL *model, EVENTID id) = 0;
    virtual BOOL cancelcallback(EVENT *event, IDSIMMODEL *model) = 0;
    virtual VOID setbreak(ABSTIME breaktime) = 0; // forces an analogue timestep; not a breakpoint
    virtual VOID suspend(IINSTANCE *instance, CHAR *msg) = 0;
    virtual EVENT *setcallbackex(ABSTIME evttime, IDSIMMODEL *model, CALLBACKHANDLERFN func, EVENTID id) = 0;
    virtual DSIMNODE newnode(CHAR *partid, CHAR *nodename) = 0;
    virtual IDSIMPIN *newpin(IINSTANCE *instance, DSIMNODE node, CHAR *name, DWORD flags) = 0;
    virtual EVENT *setclockcallback(ABSTIME starttime, RELTIME period, IDSIMMODEL *model, CALLBACKHANDLERFN func,
                                    EVENTID id) = 0;
};

// Original pin interface.
class IDSIMPIN1
{
  public:
    virtual BOOL invert() = 0;
    virtual STATE istate() = 0;
    virtual BOOL issteady() = 0;
    virtual INT activity() = 0;
    virtual BOOL isactive() = 0;
    virtual BOOL isinactive() = 0;
    virtual BOOL isposedge() = 0;
    virtual BOOL isnegedge() = 0;
    virtual BOOL isedge() = 0;
    virtual EVENT *setstate(ABSTIME time, RELTIME tlh, RELTIME thl, RELTIME tgq, STATE state) = 0;
    virtual EVENT *setstate(ABSTIME time, RELTIME tgq, STATE state) = 0;
    virtual VOID setstate(STATE state) = 0;
    virtual VOID sethandler(IDSIMMODEL *model, PINHANDLERFN phf) = 0;
    virtual DSIMNODE getnode() = 0;
    virtual STATE getstate() = 0;
};

// Pin interface with the streamlined drive methods (API 1.10).
class IDSIMPIN2 : public IDSIMPIN1
{
  public:
    virtual VOID settiming(RELTIME tlh, RELTIME thl, RELTIME tgq) = 0;
    virtual VOID setstates(STATE tstate, STATE fstate, STATE zstate) = 0;
    virtual EVENT *drivebool(ABSTIME time, BOOL flag) = 0;
    virtual EVENT *drivestate(ABSTIME time, STATE state) = 0;
    virtual EVENT *drivetristate(ABSTIME time) = 0;
};

// A group of pins driven and read as one value (API 1.10).
class IBUSPIN
{
  public:
    virtual VOID settiming(RELTIME tlh, RELTIME thl, RELTIME tz) = 0;
    virtual VOID setstates(STATE tstate, STATE fstate, STATE zstate) = 0;
    virtual VOID sethandler(IDSIMMODEL *model, PINHANDLERFN phf) = 0;
    virtual VOID drivebusvalue(ABSTIME time, DWORD value) = 0;
    virtual VOID drivetristate(ABSTIME time) = 0;
    virtual VOID drivebitstate(ABSTIME time, UINT bit, STATE state) = 0;
    virtual DWORD getbusvalue() = 0;
    virtual DWORD getbusdrive() = 0;
    virtual STATE getbitstate(UINT bit) = 0;
};

// What a digital model implements.
class IDSIMMODEL
{
  public:
    virtual INT isdigital(CHAR *pinname) = 0;
    virtual VOID setup(IINSTANCE *instance, IDSIMCKT *dsim) = 0;
    virtual VOID runctrl(RUNMODES mode) = 0;
    virtual VOID actuate(REALTIME time, ACTIVESTATE newstate) = 0;
    virtual BOOL indicate(REALTIME time, ACTIVEDATA *newstate) = 0;
    virtual VOID simulate(ABSTIME time, DSIMMODES mode) = 0;
    virtual VOID callback(ABSTIME time, EVENTID eventid) = 0;
};

// ---------------------------------------------------------------------------
// Animation data passed between simulator and graphical model

enum ACTIVEDATATYPES
{
    ADT_VOID = -1,
    ADT_REAL,
    ADT_BOOLEAN,
    ADT_INTEGER,
    ADT_STATE,
    ADT_PINVOLTAGE,
    ADT_PINSTATE,
    ADT_WIREINFO,
    ADT_SPICEDATA,
    ADT_DSIMDATA,
    ADT_USER = 100
};

struct SPICEDATA
{
    DWORD numtimepoints;
    DWORD numpins;
    REALTIME *timepoints;
    DOUBLE *nodedata;
};

struct DSIMDATA
{
    DWORD numtimepoints;
    DWORD numpins;
    ABSTIME *timepoints;
    STATE *nodedata;
};

struct ACTIVEDATA
{
    ACTIVEDATATYPES type;
    union
    {
        INT intval;
        DOUBLE realval;
        STATE stateval;
        DOUBLE wireinfo[2]; // voltage, current
        SPICEDATA spicedata;
        DSIMDATA dsimdata;
        VOID *userinfo;
    };
};

struct ACTIVEEVENT
{
    DWORD instance;
    DWORD callback;
    DWORD element;
    ACTIVEDATA data;
};

// ---------------------------------------------------------------------------
// Popup window interfaces

enum DATATYPES
{
    DT_VOID = -1,
    DT_STRING,
    DT_TEXT,
    DT_BYTE,
    DT_WORD,
    DT_DWORD,
    DT_QWORD,
    DT_IEEE_FLOAT,
    DT_IEEE_DOUBLE,
    DT_HTEC_FLOAT,
    DT_MCHP_FLOAT,
    DT_BIGENDIAN = 0x8000
};

enum DISPFORMATS
{
    DF_VOID = -1,
    DF_BINARY,
    DF_OCTAL,
    DF_HEXADECIMAL,
    DF_SIGNED,
    DF_UNSIGNED,
    DF_FLOAT,
    DF_TEXT
};

// Receives the window messages of a PWT_USER popup.
class IMSGHLR
{
  public:
    virtual LRESULT msghlr(HWND hwnd, MESSAGE msg, WPARAM warg, LPARAM larg) = 0;
};

class IUSERPOPUP
{
  public:
    virtual CHAR *getprop(CHAR *key) = 0;
    virtual VOID setprop(CHAR *key, CHAR *value) = 0;
    virtual VOID setmsghlr(IMSGHLR *handler) = 0;
    virtual LRESULT callwindowproc(MESSAGE msg, WPARAM warg, LPARAM larg) = 0;
};

class IDEBUGPOPUP
{
  public:
    virtual VOID print(CHAR *msg, ...) = 0;
    virtual VOID dump(const BYTE *ptr, UINT nbytes, UINT base = 0) = 0;
};

class ISTATUSPOPUP
{
  public:
    virtual VOID setarea(UINT columns, UINT rows, UINT border, BOOL copy) = 0;
    virtual VOID setcursorto(UINT pixelx, UINT pixely) = 0;
    virtual UINT getcharwidth(VOID) = 0;
    virtual UINT getcharheight(VOID) = 0;
    virtual VOID print(CHAR *message, ...) = 0;
    virtual VOID print(INT col, INT row, COLOUR textcolour, CHAR *msg, ...) = 0;
    virtual VOID setctabstops(const INT ts[], INT n) = 0;
    virtual VOID setptabstops(const INT ts[], INT n) = 0;
    virtual VOID clear(COLOUR bkcolour = NOCOLOUR) = 0;
    virtual BOOL setredraw(BOOL on_off, BOOL redraw_now) = 0;
    virtual VOID repaint(VOID) = 0;
};

// Shows a block of memory the model owns; Proteus keeps the pointer.
class IMEMORYPOPUP
{
  public:
    virtual VOID setmemory(ADDRESS baseaddr, BYTE *data, UINT nbytes) = 0;
    virtual VOID repaint(VOID) = 0;
};

// ---------------------------------------------------------------------------
// Debugger support for CPU models

struct VDM_COMMAND; // declared in openvdm.hpp

#define WATCHITEM_NAME_SIZE 128
#define WATCHITEM_ADDR_SIZE 32

struct VARITEM
{
    CHAR name[WATCHITEM_NAME_SIZE];
    DWORD loader;
    DWORD seg;
    ADDRESS address;
    DATATYPES type;
    DISPFORMATS format;
    DWORD size;
    ADDRESS scope_begin;
    ADDRESS scope_end;
};

struct VARDATA
{
    CHAR addr[WATCHITEM_ADDR_SIZE];
    DATATYPES type;
    BYTE *memory;
    DWORD memsize;
    DWORD offset; // from the start of the memory block
};

// Implemented by CPU models that support the debugger.
class ICPU
{
  public:
    virtual LRESULT vdmhlr(VDM_COMMAND *cmd, BYTE *data) = 0;
    virtual VOID loaddata(INT format, INT seg, ADDRESS address, BYTE *data, INT numbytes) = 0;
    virtual VOID disassemble(ADDRESS address, INT numbytes) = 0;
    virtual BOOL getvardata(VARITEM *vip, VARDATA *vdp) = 0;
};

// Loader formats passed to ICPU::loaddata.
#define FORMAT_BIN 0
#define FORMAT_HEX 1
#define FORMAT_S19 2
#define FORMAT_OMF51 3
#define FORMAT_UBROF 4
#define FORMAT_COD 5
#define FORMAT_BASIC 6
#define FORMAT_COFF 7

// ---------------------------------------------------------------------------
// Model DLL entry points (exported with C linkage by the model DLL)

extern "C"
{
    typedef IACTIVEMODEL *CREATEACTIVEMODELFN(CHAR *device, ILICENCESERVER *ils);
    typedef VOID DELETEACTIVEMODELFN(IACTIVEMODEL *model);
    typedef IDSIMMODEL *CREATEDSIMMODELFN(CHAR *device, ILICENCESERVER *ils);
    typedef VOID DELETEDSIMMODELFN(IDSIMMODEL *model);
}

#pragma pack(pop)

#endif // OPENVSM_SDK_OPENVSM_HPP
