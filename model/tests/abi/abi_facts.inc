// Shared body of the VSM ABI comparison.
//
// Included once per side, after the VSM and VDM headers under test
// (<openvsm.hpp>/<openvdm.hpp>, or the vendor's <vsm.hpp>/<vdm.hpp>). The including file defines ABI_SIDE_FUNCTION, the name of the function
// that returns this side's table. The same source is compiled against both
// headers, so every difference in the tables is a difference in the headers.

#include <cstddef>

#include "abi_table.hpp"

#if !defined(_M_IX86)
#error "The VSM ABI check needs a 32-bit x86 build"
#endif

namespace
{
int g_tag;

#define TV(n)                                                                                                         \
    {                                                                                                                  \
        g_tag = n;                                                                                                     \
    }
#define TR(n)                                                                                                         \
    {                                                                                                                  \
        g_tag = n;                                                                                                     \
        return {};                                                                                                     \
    }

// Calls vtable slot `slot` of `object` with 24 zero arguments and restores the
// stack pointer afterwards, whatever the callee's convention or arity. The
// probe methods below never read their arguments; they only record a tag.
int callSlot(void *object, int slot)
{
    g_tag = -1;
    __asm {
        mov esi, esp
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        push 0
        mov ecx, object
        mov eax, [ecx]
        mov edx, slot
        call dword ptr [eax + edx * 4]
        mov esp, esi
    }
    return g_tag;
}

struct ProbeLicence : ILICENCESERVER
{
    BOOL authorize(DWORD, DWORD) override TR(1)
    BOOL authorizeex(DWORD, DWORD, DWORD) override TR(2)
    DWORD getid() override TR(3)
    CHAR *getname() override TR(4)
    CHAR *getcompany() override TR(5)
    CHAR *getexpirydate() override TR(6)
};

struct ProbeComponent : ICOMPONENT
{
    CHAR *getprop(CHAR *) override TR(1)
    CHAR *getproptext(VOID) override TR(2)
    VOID addprop(CHAR *, CHAR *, WORD) override TV(3)
    VOID delprop(CHAR *) override TV(4)
    VOID setproptext(CHAR *) override TV(5)
    ACTIVESTATE getstate(INT, ACTIVEDATA *) override TR(6)
    BOOL setstate(ACTIVESTATE) override TR(7)
    VOID setdrawscale(INT) override TV(8)
    HDC begincache(BOX &) override TR(9)
    HDC begincache(INT) override TR(10)
    VOID endcache() override TV(11)
    HGFXSTYLE creategfxstyle(CHAR *) override TR(12)
    VOID selectgfxstyle(HGFXSTYLE) override TV(13)
    VOID setpenwidth(INT) override TV(14)
    VOID setpencolour(COLOUR) override TV(15)
    VOID setbrushcolour(COLOUR) override TV(16)
    VOID drawline(INT, INT, INT, INT) override TV(17)
    VOID drawbox(INT, INT, INT, INT) override TV(18)
    VOID drawbox(BOX &) override TV(19)
    VOID drawcircle(INT, INT, INT) override TV(20)
    VOID drawbezier(POINT *, INT) override TV(21)
    VOID drawpolyline(POINT *, INT) override TV(22)
    VOID drawpolygon(POINT *, INT) override TV(23)
    VOID drawsymbol(INT) override TV(24)
    VOID drawsymbol(INT, INT, INT, INT, INT) override TV(25)
    VOID drawstate(ACTIVESTATE) override TV(26)
    BOOL getsymbolarea(INT, BOX *) override TR(27)
    BOOL getmarker(CHAR *, POINT *, INT *, INT *) override TR(28)
    HTEXTSTYLE createtextstyle(CHAR *) override TR(29)
    VOID selecttextstyle(HTEXTSTYLE) override TV(30)
    VOID settextfont(CHAR *) override TV(31)
    VOID settextsize(INT) override TV(32)
    VOID setbold(BOOL) override TV(33)
    VOID setitalic(BOOL) override TV(34)
    VOID setunderline(BOOL) override TV(35)
    VOID settextcolour(COLOUR) override TV(36)
    VOID drawtext(INT, INT, INT, INT, CHAR *, ...) override TV(37)
    IPOPUP *createpopup(CREATEPOPUPSTRUCT *) override TR(38)
    VOID deletepopup(POPUPID) override TV(39)
    VOID settimestep(DOUBLE) override TV(40)
    VOID error(CHAR *, ...) override TV(41)
    VOID repaint(BOOL) override TV(42)
};

struct ProbeActiveModel : IACTIVEMODEL
{
    VOID initialize(ICOMPONENT *) override TV(1)
    ISPICEMODEL *getspicemodel(CHAR *) override TR(2)
    IDSIMMODEL *getdsimmodel(CHAR *) override TR(3)
    VOID plot(ACTIVESTATE) override TV(4)
    VOID animate(INT, ACTIVEDATA *) override TV(5)
    BOOL actuate(WORD, INT, INT, DWORD) override TR(6)
};

struct ProbeInstance : IINSTANCE
{
    CHAR *id() override TR(1)
    CHAR *value() override TR(2)
    CHAR *getstrval(CHAR *, CHAR *) override TR(3)
    VOID getnumval(DOUBLE *, CHAR *, DOUBLE) override TV(4)
    BOOL getboolval(CHAR *, BOOL) override TR(5)
    DWORD gethexval(CHAR *, DWORD) override TR(6)
    LONG getinitval(CHAR *, LONG) override TR(7)
    RELTIME getdelay(CHAR *, RELTIME) override TR(8)
    IACTIVEMODEL *getactivemodel() override TR(9)
    IINSTANCE *getinterfacemodel() override TR(10)
    BOOL getmoddata(BYTE **, DWORD *) override TR(11)
    SPICENODE getspicenode(CHAR *, BOOL) override TR(12)
    IDSIMPIN *getdsimpin(CHAR *, BOOL) override TR(13)
    VOID log(CHAR *, ...) override TV(14)
    VOID warning(CHAR *, ...) override TV(15)
    VOID error(CHAR *, ...) override TV(16)
    VOID fatal(CHAR *, ...) override TV(17)
    BOOL message(CHAR *, ...) override TR(18)
    IPOPUP *createpopup(CREATEPOPUPSTRUCT *) override TR(19)
    VOID deletepopup(POPUPID) override TV(20)
    BOOL setvdmhlr(ICPU *) override TR(21)
    BOOL loadmemory(CHAR *, VOID *, UINT, UINT, UINT) override TR(22)
    IBUSPIN *getbuspin(CHAR *, UINT, UINT, BOOL) override TR(23)
    IBUSPIN *getbuspin(CHAR *, IDSIMPIN **, UINT) override TR(24)
};

struct ProbeCircuit : IDSIMCKT
{
    VOID sysvar(DOUBLE *, DSIMVARS) override TV(1)
    EVENT *setcallback(ABSTIME, IDSIMMODEL *, EVENTID) override TR(2)
    BOOL cancelcallback(EVENT *, IDSIMMODEL *) override TR(3)
    VOID setbreak(ABSTIME) override TV(4)
    VOID suspend(IINSTANCE *, CHAR *) override TV(5)
    EVENT *setcallbackex(ABSTIME, IDSIMMODEL *, CALLBACKHANDLERFN, EVENTID) override TR(6)
    DSIMNODE newnode(CHAR *, CHAR *) override TR(7)
    IDSIMPIN *newpin(IINSTANCE *, DSIMNODE, CHAR *, DWORD) override TR(8)
    EVENT *setclockcallback(ABSTIME, RELTIME, IDSIMMODEL *, CALLBACKHANDLERFN, EVENTID) override TR(9)
};

struct ProbePin : IDSIMPIN2
{
    BOOL invert() override TR(1)
    STATE istate() override TR(2)
    BOOL issteady() override TR(3)
    INT activity() override TR(4)
    BOOL isactive() override TR(5)
    BOOL isinactive() override TR(6)
    BOOL isposedge() override TR(7)
    BOOL isnegedge() override TR(8)
    BOOL isedge() override TR(9)
    EVENT *setstate(ABSTIME, RELTIME, RELTIME, RELTIME, STATE) override TR(10)
    EVENT *setstate(ABSTIME, RELTIME, STATE) override TR(11)
    VOID setstate(STATE) override TV(12)
    VOID sethandler(IDSIMMODEL *, PINHANDLERFN) override TV(13)
    DSIMNODE getnode() override TR(14)
    STATE getstate() override TR(15)
    VOID settiming(RELTIME, RELTIME, RELTIME) override TV(16)
    VOID setstates(STATE, STATE, STATE) override TV(17)
    EVENT *drivebool(ABSTIME, BOOL) override TR(18)
    EVENT *drivestate(ABSTIME, STATE) override TR(19)
    EVENT *drivetristate(ABSTIME) override TR(20)
};

struct ProbeBus : IBUSPIN
{
    VOID settiming(RELTIME, RELTIME, RELTIME) override TV(1)
    VOID setstates(STATE, STATE, STATE) override TV(2)
    VOID sethandler(IDSIMMODEL *, PINHANDLERFN) override TV(3)
    VOID drivebusvalue(ABSTIME, DWORD) override TV(4)
    VOID drivetristate(ABSTIME) override TV(5)
    VOID drivebitstate(ABSTIME, UINT, STATE) override TV(6)
    DWORD getbusvalue() override TR(7)
    DWORD getbusdrive() override TR(8)
    STATE getbitstate(UINT) override TR(9)
};

struct ProbeDigitalModel : IDSIMMODEL
{
    INT isdigital(CHAR *) override TR(1)
    VOID setup(IINSTANCE *, IDSIMCKT *) override TV(2)
    VOID runctrl(RUNMODES) override TV(3)
    VOID actuate(REALTIME, ACTIVESTATE) override TV(4)
    BOOL indicate(REALTIME, ACTIVEDATA *) override TR(5)
    VOID simulate(ABSTIME, DSIMMODES) override TV(6)
    VOID callback(ABSTIME, EVENTID) override TV(7)
};

struct ProbeMessageHandler : IMSGHLR
{
    LRESULT msghlr(HWND, MESSAGE, WPARAM, LPARAM) override TR(1)
};

struct ProbeUserPopup : IUSERPOPUP
{
    CHAR *getprop(CHAR *) override TR(1)
    VOID setprop(CHAR *, CHAR *) override TV(2)
    VOID setmsghlr(IMSGHLR *) override TV(3)
    LRESULT callwindowproc(MESSAGE, WPARAM, LPARAM) override TR(4)
};

struct ProbeDebugPopup : IDEBUGPOPUP
{
    VOID print(CHAR *, ...) override TV(1)
    VOID dump(const BYTE *, UINT, UINT) override TV(2)
};

struct ProbeStatusPopup : ISTATUSPOPUP
{
    VOID setarea(UINT, UINT, UINT, BOOL) override TV(1)
    VOID setcursorto(UINT, UINT) override TV(2)
    UINT getcharwidth(VOID) override TR(3)
    UINT getcharheight(VOID) override TR(4)
    VOID print(CHAR *, ...) override TV(5)
    VOID print(INT, INT, COLOUR, CHAR *, ...) override TV(6)
    VOID setctabstops(const INT[], INT) override TV(7)
    VOID setptabstops(const INT[], INT) override TV(8)
    VOID clear(COLOUR) override TV(9)
    BOOL setredraw(BOOL, BOOL) override TR(10)
    VOID repaint(VOID) override TV(11)
};

struct ProbeMemoryPopup : IMEMORYPOPUP
{
    VOID setmemory(ADDRESS, BYTE *, UINT) override TV(1)
    VOID repaint(VOID) override TV(2)
};

struct ProbeCpu : ICPU
{
    LRESULT vdmhlr(VDM_COMMAND *, BYTE *) override TR(1)
    VOID loaddata(INT, INT, ADDRESS, BYTE *, INT) override TV(2)
    VOID disassemble(ADDRESS, INT) override TV(3)
    BOOL getvardata(VARITEM *, VARDATA *) override TR(4)
};

#undef TV
#undef TR

const int maxFacts = 1024;
AbiFact g_facts[maxFacts];
int g_count;
char g_names[maxFacts][64];

void add(const char *name, long long value)
{
    if (g_count < maxFacts)
    {
        g_facts[g_count] = {name, value};
        ++g_count;
    }
}

// Records, for every vtable slot, which method the header put there.
void addSlots(const char *interfaceName, void *probe, int slots)
{
    for (int slot = 0; slot < slots && g_count < maxFacts; ++slot)
    {
        char *name = g_names[g_count];
        int length = 0;
        for (const char *c = interfaceName; *c && length < 50; ++c)
        {
            name[length++] = *c;
        }
        name[length++] = '[';
        name[length++] = static_cast<char>('0' + slot / 10);
        name[length++] = static_cast<char>('0' + slot % 10);
        name[length++] = ']';
        name[length] = '\0';
        add(name, callSlot(probe, slot));
    }
}

#define FACT(expr) add(#expr, static_cast<long long>(expr))
#define SIZE(type) add("sizeof(" #type ")", static_cast<long long>(sizeof(type)))
#define FIELD(type, field) add("offsetof(" #type ", " #field ")", static_cast<long long>(offsetof(type, field)))
#define SLOTS(probe, count)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        static probe instance;                                                                                         \
        addSlots(#probe, &instance, count);                                                                            \
    } while (0)

void collect()
{
    // Scalar types.
    SIZE(CHAR);
    SIZE(BYTE);
    SIZE(SHORT);
    SIZE(INT);
    SIZE(LONG);
    SIZE(WORD);
    SIZE(DWORD);
    SIZE(BOOL);
    SIZE(UINT);
    SIZE(LONGLONG);
    SIZE(COLOUR);
    SIZE(DOUBLE);
    SIZE(MESSAGE);
    SIZE(WPARAM);
    SIZE(LPARAM);
    SIZE(LRESULT);
    SIZE(HWND);
    SIZE(HDC);
    SIZE(POINT);
    SIZE(BOX);
    FACT(static_cast<WPARAM>(-1) > 0);
    FACT(static_cast<LPARAM>(-1) > 0);
    FACT(static_cast<COLOUR>(-1) > 0);
    SIZE(ACTIVESTATE);
    SIZE(SPICENODE);
    SIZE(DSIMNODE);
    SIZE(POPUPID);
    SIZE(REALTIME);
    SIZE(ABSTIME);
    SIZE(RELTIME);
    SIZE(EVENTID);
    FACT(static_cast<EVENTID>(-1) < 0);
    SIZE(ADDRESS);
    SIZE(PINHANDLERFN);
    SIZE(CALLBACKHANDLERFN);

    // Constants.
    FACT(VSM_API_VERSION);
    FACT(VSM_BUILD_DATE);
    FACT(SHOW_ALL);
    FACT(HIDE_KEYWORD);
    FACT(HIDE_VALUE);
    FACT(HIDE_ALL);
    FACT(HIDE_NOMODIFY);
    FACT(BLACK);
    FACT(BLUE);
    FACT(GREEN);
    FACT(CYAN);
    FACT(RED);
    FACT(MAGENTA);
    FACT(YELLOW);
    FACT(WHITE);
    FACT(GREY);
    FACT(BRIGHTBLUE);
    FACT(BRIGHTGREEN);
    FACT(BRIGHTCYAN);
    FACT(BRIGHTRED);
    FACT(BRIGHTMAGENTA);
    FACT(BRIGHTYELLOW);
    FACT(BRIGHTWHITE);
    FACT(MAKECOLOUR(0x12, 0x34, 0x56));
    FACT(INVISIBLE);
    FACT(NOCOLOUR);
    FACT(TXJ_LEFT);
    FACT(TXJ_RIGHT);
    FACT(TXJ_CENTRE);
    FACT(TXJ_BOTTOM);
    FACT(TXJ_TOP);
    FACT(TXJ_MIDDLE);
    FACT(PID_LOGWINDOW);
    FACT(PID_WATCHWINDOW);
    FACT(MIR_X);
    FACT(MIR_Y);
    FACT(DSIMTICK * 1e15);
    FACT(MAXBUSBITS);
    FACT(EID_BREAKPOINT);
    FACT(DPF_INPUT);
    FACT(DPF_OUTPUT);
    FACT(DPF_BIDIR);
    FACT(SP_UNDEFINED);
    FACT(SP_LOW);
    FACT(SP_FLOAT);
    FACT(SP_HIGH);
    FACT(SP_MASK);
    FACT(SS_FLOAT);
    FACT(SS_DRIVEN);
    FACT(SS_WEAK);
    FACT(SS_STRONG);
    FACT(SS_INJECT);
    FACT(SS_POWER);
    FACT(SS_MASK);
    FACT(SF_CONTENTION);
    FACT(SF_MESSAGE);
    FACT(SF_MASK);
    FACT(WATCHITEM_NAME_SIZE);
    FACT(WATCHITEM_ADDR_SIZE);
    FACT(FORMAT_BIN);
    FACT(FORMAT_HEX);
    FACT(FORMAT_S19);
    FACT(FORMAT_OMF51);
    FACT(FORMAT_UBROF);
    FACT(FORMAT_COD);
    FACT(FORMAT_BASIC);
    FACT(FORMAT_COFF);

    // Enumerations.
    FACT(PWT_USER);
    FACT(PWT_DEBUG);
    FACT(PWT_STATUS);
    FACT(PWT_MEMORY);
    FACT(PWT_SOURCE);
    FACT(PWT_WATCH);
    FACT(PWT_VAR);
    SIZE(POPUPTYPES);
    FACT(PWF_VISIBLE);
    FACT(PWF_SIZEABLE);
    FACT(PWF_LOCKPOSITION);
    FACT(PWF_HIDEONANIMATE);
    FACT(PWF_AUTOREFRESH);
    FACT(PWF_WANTKEYBOARD);
    FACT(PWF_ACTIVE);
    FACT(PWF_INTERNAL);
    FACT(ACF_LEFT);
    FACT(ACF_RIGHT);
    FACT(ACF_MIDDLE);
    FACT(ACF_INC);
    FACT(ACF_DEC);
    FACT(ACF_TOGGLE);
    FACT(RM_BATCH);
    FACT(RM_START);
    FACT(RM_STOP);
    FACT(RM_SUSPEND);
    FACT(RM_ANIMATE);
    FACT(RM_STEPTIME);
    FACT(RM_STEPOVER);
    FACT(RM_STEPINTO);
    FACT(RM_STEPOUT);
    FACT(RM_STEPTO);
    FACT(RM_META);
    FACT(RM_DUMP);
    SIZE(RUNMODES);
    FACT(DSIMBOOT);
    FACT(DSIMSETTLE);
    FACT(DSIMNORMAL);
    FACT(DSIMEND);
    SIZE(DSIMMODES);
    FACT(DSIMTIMENOW);
    FACT(DSIMTDSCALE);
    FACT(UNDEFINED);
    FACT(TSTATE);
    FACT(FSTATE);
    FACT(PLO);
    FACT(ILO);
    FACT(SLO);
    FACT(WLO);
    FACT(FLT);
    FACT(WHI);
    FACT(SHI);
    FACT(IHI);
    FACT(PHI);
    FACT(WUD);
    FACT(SUD);
    SIZE(STATE);
    FACT(ADT_VOID);
    FACT(ADT_REAL);
    FACT(ADT_BOOLEAN);
    FACT(ADT_INTEGER);
    FACT(ADT_STATE);
    FACT(ADT_PINVOLTAGE);
    FACT(ADT_PINSTATE);
    FACT(ADT_WIREINFO);
    FACT(ADT_SPICEDATA);
    FACT(ADT_DSIMDATA);
    FACT(ADT_USER);
    FACT(DT_VOID);
    FACT(DT_STRING);
    FACT(DT_TEXT);
    FACT(DT_BYTE);
    FACT(DT_WORD);
    FACT(DT_DWORD);
    FACT(DT_QWORD);
    FACT(DT_IEEE_FLOAT);
    FACT(DT_IEEE_DOUBLE);
    FACT(DT_HTEC_FLOAT);
    FACT(DT_MCHP_FLOAT);
    FACT(DT_BIGENDIAN);
    FACT(DF_VOID);
    FACT(DF_BINARY);
    FACT(DF_OCTAL);
    FACT(DF_HEXADECIMAL);
    FACT(DF_SIGNED);
    FACT(DF_UNSIGNED);
    FACT(DF_FLOAT);
    FACT(DF_TEXT);

    // Structures.
    SIZE(CREATEPOPUPSTRUCT);
    FIELD(CREATEPOPUPSTRUCT, id);
    FIELD(CREATEPOPUPSTRUCT, type);
    FIELD(CREATEPOPUPSTRUCT, caption);
    FIELD(CREATEPOPUPSTRUCT, width);
    FIELD(CREATEPOPUPSTRUCT, height);
    FIELD(CREATEPOPUPSTRUCT, flags);
    SIZE(SPICEDATA);
    FIELD(SPICEDATA, numtimepoints);
    FIELD(SPICEDATA, numpins);
    FIELD(SPICEDATA, timepoints);
    FIELD(SPICEDATA, nodedata);
    SIZE(DSIMDATA);
    FIELD(DSIMDATA, numtimepoints);
    FIELD(DSIMDATA, numpins);
    FIELD(DSIMDATA, timepoints);
    FIELD(DSIMDATA, nodedata);
    SIZE(ACTIVEDATA);
    FIELD(ACTIVEDATA, type);
    FIELD(ACTIVEDATA, intval);
    FIELD(ACTIVEDATA, realval);
    FIELD(ACTIVEDATA, stateval);
    FIELD(ACTIVEDATA, wireinfo);
    FIELD(ACTIVEDATA, spicedata);
    FIELD(ACTIVEDATA, dsimdata);
    FIELD(ACTIVEDATA, userinfo);
    SIZE(ACTIVEEVENT);
    FIELD(ACTIVEEVENT, instance);
    FIELD(ACTIVEEVENT, callback);
    FIELD(ACTIVEEVENT, element);
    FIELD(ACTIVEEVENT, data);
    SIZE(VARITEM);
    FIELD(VARITEM, name);
    FIELD(VARITEM, loader);
    FIELD(VARITEM, seg);
    FIELD(VARITEM, address);
    FIELD(VARITEM, type);
    FIELD(VARITEM, format);
    FIELD(VARITEM, size);
    FIELD(VARITEM, scope_begin);
    FIELD(VARITEM, scope_end);
    SIZE(VARDATA);
    FIELD(VARDATA, addr);
    FIELD(VARDATA, type);
    FIELD(VARDATA, memory);
    FIELD(VARDATA, memsize);
    FIELD(VARDATA, offset);
#ifndef _WINDOWS_
    FIELD(POINT, x);
    FIELD(POINT, y);
    FIELD(BOX, x1);
    FIELD(BOX, y1);
    FIELD(BOX, x2);
    FIELD(BOX, y2);
#endif

    // Debug monitor.
    FACT(VDM_API_VERSION);
    FACT(VDM51_CLASS);
    FACT(VDM11_CLASS);
    FACT(VDMPIC16_CLASS);
    FACT(VDMAVR_CLASS);
    FACT(VDMPIC18_CLASS);
    FACT(VDMARM7_CLASS);
    FACT(VDM_PORT);
    FACT(VDM_INVALID_HANDLE);
    FACT(VDM_INIT);
    FACT(VDM_TERM);
    FACT(VDM_PLAY);
    FACT(VDM_STEP);
    FACT(VDM_PAUSE);
    FACT(VDM_WRITEDATA);
    FACT(VDM_READDATA);
    FACT(VDM_READREGS);
    FACT(VDM_WRITEREGS);
    FACT(VDM_SETBP);
    FACT(VDM_CLRBP);
    FACT(VDM_SETPC);
    FACT(VDM_GETPC);
    FACT(VDM_RESET);
    FACT(VDM_GETTID);
    FACT(ERR_VDM_FAILED);
    FACT(ERR_VDM_OK);
    FACT(ERR_VDM_TIMEOUT);
    FACT(ERR_VDM_BADCOMMAND);
    FACT(ERR_VDM_BADADDRESS);
    FACT(ERR_VDM_BADDATALEN);
    FACT(ERR_VDM_SIMFAILED);
    FACT(ERR_VDM_NOTARGET);
    FACT(ERR_VDM_PENDING);
    FACT(ERR_VDM_PLAYDEAD);
    FACT(ERR_VDM_BADHANDLE);
    SIZE(VDM_MEMSPACE);
    SIZE(VDM_ADDRESS);
    SIZE(VDM_RESULT);
    SIZE(VDM_TARGETID);
    SIZE(UL64);
    SIZE(FL64);
    SIZE(VDM_COMMAND);
    FIELD(VDM_COMMAND, command);
    FIELD(VDM_COMMAND, memspace);
    FIELD(VDM_COMMAND, address);
    FIELD(VDM_COMMAND, datalength);
    SIZE(VDM_TARGETINFO);
    FIELD(VDM_TARGETINFO, id);
    FIELD(VDM_TARGETINFO, regclass);
    FIELD(VDM_TARGETINFO, regsize);
    FIELD(VDM_TARGETINFO, apiver);
    FIELD(VDM_TARGETINFO, dllver);
    FIELD(VDM_TARGETINFO, clock);
    FIELD(VDM_TARGETINFO, unused);
    FIELD(VDM_TARGETINFO, specific);

    // Interfaces: the method found in each vtable slot. A probe overrides every
    // method, so a header with a missing or extra method does not compile.
    SLOTS(ProbeLicence, 6);
    SLOTS(ProbeComponent, 42);
    SLOTS(ProbeActiveModel, 6);
    SLOTS(ProbeInstance, 24);
    SLOTS(ProbeCircuit, 9);
    SLOTS(ProbePin, 20);
    SLOTS(ProbeBus, 9);
    SLOTS(ProbeDigitalModel, 7);
    SLOTS(ProbeMessageHandler, 1);
    SLOTS(ProbeUserPopup, 4);
    SLOTS(ProbeDebugPopup, 2);
    SLOTS(ProbeStatusPopup, 11);
    SLOTS(ProbeMemoryPopup, 2);
    SLOTS(ProbeCpu, 4);
}

#undef FACT
#undef SIZE
#undef FIELD
#undef SLOTS
} // namespace

AbiTable ABI_SIDE_FUNCTION()
{
    if (g_count == 0)
    {
        collect();
    }
    return {g_facts, g_count};
}
