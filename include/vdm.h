/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****       LABCENTER INTEGRATED SIMULATION ARCHITECTURE       *****/
/*****                                                          *****/
/*****       Virtual Debug Monitor & CPU Interface Header       *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#ifndef VDM_HPP
#define VDM_HPP

// Version 1.00   Initial Release
// Version 1.01   Extended target info structure

#define VDM_API_VERSION 101

// VDM Target Classes

#define VDM51_CLASS      1
#define VDM11_CLASS      2
#define VDMPIC16_CLASS   3
#define VDMAVR_CLASS     4
#define VDMPIC18_CLASS   5

#define VDMARM7_CLASS     10

// Defintions for VC++ compilation
#ifndef RC_INVOKED
#ifndef STDLX_H
#define VOID void
typedef char CHAR;
typedef unsigned char BYTE;
typedef signed short SHORT;
typedef signed int INT;
typedef signed long LONG;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef signed int BOOL;
typedef unsigned int UINT;
#endif

#ifndef _WINDOWS_
typedef VOID* HWND;
#endif

#ifdef __SC__
typedef unsigned long long UL64;
#else
typedef unsigned __int64 UL64;
#endif

typedef double FL64;

#endif

/*********************************************************************
 **** Definitions for VDM Protocol ****
 *************************************/

// Default TCP/IP socket:
#define VDM_PORT 8000
#define VDM_INVALID_HANDLE -1

// VDM protocol command codes
#define VDM_INIT       1
#define VDM_TERM       2
#define VDM_PLAY       3
#define VDM_STEP       4
#define VDM_PAUSE      5
#define VDM_READDATA   7
#define VDM_WRITEDATA  6
#define VDM_READREGS   8
#define VDM_WRITEREGS  9
#define VDM_SETBP      10
#define VDM_CLRBP      11
#define VDM_SETPC      12
#define VDM_GETPC      13
#define VDM_RESET      14
#define VDM_GETTID     15

// VDM protocol status/error codes
#define ERR_VDM_FAILED    -1
#define ERR_VDM_OK         0
#define ERR_VDM_TIMEOUT    1
#define ERR_VDM_BADCOMMAND 2
#define ERR_VDM_BADADDRESS 3
#define ERR_VDM_BADDATALEN 4
#define ERR_VDM_SIMFAILED  5
#define ERR_VDM_NOTARGET   6

// These message occur in the API DLL only:
#define ERR_VDM_PENDING    100
#define ERR_VDM_PLAYDEAD   101
#define ERR_VDM_BADHANDLE  102

#ifndef RC_INVOKED

// VSM uses 4 byte alignment throughout
#pragma pack(push,4)

// VDM data stypes;
typedef BYTE VDM_MEMSPACE;
typedef DWORD VDM_ADDRESS;
typedef LONG VDM_RESULT;

// VDM target name
typedef CHAR VDM_TARGETID[64];

// VDM Protocol command strcuture:
struct VDM_COMMAND
{
	BYTE command;
	BYTE memspace;
	DWORD address;
	DWORD datalength;
};


#endif

/*********************************************************************
 **** Definitions for VDMAPI DLL ****
 ***********************************/

#ifndef RC_INVOKED

// Registry key for config settings:
#define VDM_REGKEY  "Software\\Labcenter Electronics\\VDM"

// Filename for session log:
#define VDM_LOGFILE "VDMLOG.TXT"

// Define the function prototypes according to whether we are
// building the VDMAPI DLL or using it:
#ifdef VDMDLL
#define VDMAPI __declspec(dllexport)
#else
#define VDMAPI __declspec(dllimport)
#endif


// API configuration structure:
struct VDM_CONFIG
{
	CHAR address[64];
	DWORD port;
	DWORD timeout;
	DWORD loglevel;
};

// Target information structure (128 bytes)
// Now includes 64 byte structure for target specific (derivitive) parameters
struct VDM_TARGETINFO
{
	CHAR id[32];
	DWORD regclass;
	DWORD regsize;
	DWORD apiver;
	DWORD dllver;
	FL64 clock;
	DWORD unused[2];
	BYTE specific[64];
};

typedef LONG VDM_HANDLE;
typedef VOID __cdecl VDM_CALLBACK ( VDM_HANDLE, VDM_RESULT );


#ifdef __cplusplus
extern "C" {
#endif


VDM_HANDLE VDMAPI vdm_open ( VDM_CALLBACK* func = NULL );
VOID VDMAPI vdm_close ( VDM_HANDLE h );

VDM_RESULT VDMAPI vdm_init ( VDM_HANDLE h, VDM_TARGETINFO* info, UINT size=sizeof ( VDM_TARGETINFO ) );
VDM_RESULT VDMAPI vdm_term ( VDM_HANDLE h );

VDM_RESULT VDMAPI vdm_play ( VDM_HANDLE h );
VDM_RESULT VDMAPI vdm_step ( VDM_HANDLE h );
VDM_RESULT VDMAPI vdm_pause ( VDM_HANDLE h );
VDM_RESULT VDMAPI vdm_reset ( VDM_HANDLE h );

VDM_RESULT VDMAPI vdm_readdata ( VDM_HANDLE h, VDM_MEMSPACE space, VDM_ADDRESS addr, VOID* buf, UINT numbytes );
VDM_RESULT VDMAPI vdm_writedata ( VDM_HANDLE h, VDM_MEMSPACE space, VDM_ADDRESS addr, VOID* buf, UINT numbytes );
VDM_RESULT VDMAPI vdm_readregs ( VDM_HANDLE h, VOID* regs, UINT size );
VDM_RESULT VDMAPI vdm_writeregs ( VDM_HANDLE h, VOID* regs, UINT size );

VDM_RESULT VDMAPI vdm_setbp ( VDM_HANDLE h, VDM_ADDRESS addr );
VDM_RESULT VDMAPI vdm_clrbp ( VDM_HANDLE h, VDM_ADDRESS addr );
VDM_RESULT VDMAPI vdm_getpc ( VDM_HANDLE h, VDM_ADDRESS* npc );
VDM_RESULT VDMAPI vdm_setpc ( VDM_HANDLE h, VDM_ADDRESS npc );

BOOL VDMAPI vdm_isalive ( VDM_HANDLE h );

BOOL VDMAPI vdm_editconfig ( HWND phwnd );
VOID VDMAPI vdm_getconfig ( VDM_CONFIG*, UINT size=sizeof ( VDM_CONFIG ) );
VOID VDMAPI vdm_setconfig ( VDM_CONFIG*, UINT size=sizeof ( VDM_CONFIG ) );

BOOL VDMAPI vdm_geterrmsg ( VDM_RESULT result, CHAR* buf, INT size );

#ifdef __cplusplus
}
#endif


#pragma pack(pop)

#endif

#endif

