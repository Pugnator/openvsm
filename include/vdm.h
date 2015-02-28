/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****       LABCENTER int32_tEGRATED SIMULATION ARCHITECTURE       *****/
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
#define void void
typedef char CHAR;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef unsigned short uint16_t;
typedef unsigned long uint64_t;
typedef signed int bool;
typedef unsigned int uint32_t;
#endif

#ifndef _WINDOWS_
typedef void* HWND;
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
#define ERR_VDM_BADuint64_t 3
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
typedef uint8_t VDM_MEMSPACE;
typedef uint64_t VDM_uint64_t;
typedef int64_t VDM_RESULT;

// VDM target name
typedef CHAR VDM_TARGETID[64];

// VDM Protocol command strcuture:
struct VDM_COMMAND
{
	uint8_t command;
	uint8_t memspace;
	uint64_t address;
	uint64_t datalength;
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
	uint64_t port;
	uint64_t timeout;
	uint64_t loglevel;
};

// Target information structure (128 bytes)
// Now includes 64 byte structure for target specific (derivitive) parameters
struct VDM_TARGETINFO
{
	CHAR id[32];
	uint64_t regclass;
	uint64_t regsize;
	uint64_t apiver;
	uint64_t dllver;
	FL64 clock;
	uint64_t unused[2];
	uint8_t specific[64];
};

typedef int64_t VDM_HANDLE;
typedef void __cdecl VDM_CALLBACK ( VDM_HANDLE, VDM_RESULT );


#ifdef __cplusplus
extern "C" {
#endif


VDM_HANDLE VDMAPI vdm_open ( VDM_CALLBACK* func = NULL );
void VDMAPI vdm_close ( VDM_HANDLE h );

VDM_RESULT VDMAPI vdm_init ( VDM_HANDLE h, VDM_TARGETINFO* info, uint32_t size=sizeof ( VDM_TARGETINFO ) );
VDM_RESULT VDMAPI vdm_term ( VDM_HANDLE h );

VDM_RESULT VDMAPI vdm_play ( VDM_HANDLE h );
VDM_RESULT VDMAPI vdm_step ( VDM_HANDLE h );
VDM_RESULT VDMAPI vdm_pause ( VDM_HANDLE h );
VDM_RESULT VDMAPI vdm_reset ( VDM_HANDLE h );

VDM_RESULT VDMAPI vdm_readdata ( VDM_HANDLE h, VDM_MEMSPACE space, VDM_uint64_t addr, void* buf, uint32_t numbytes );
VDM_RESULT VDMAPI vdm_writedata ( VDM_HANDLE h, VDM_MEMSPACE space, VDM_uint64_t addr, void* buf, uint32_t numbytes );
VDM_RESULT VDMAPI vdm_readregs ( VDM_HANDLE h, void* regs, uint32_t size );
VDM_RESULT VDMAPI vdm_writeregs ( VDM_HANDLE h, void* regs, uint32_t size );

VDM_RESULT VDMAPI vdm_setbp ( VDM_HANDLE h, VDM_uint64_t addr );
VDM_RESULT VDMAPI vdm_clrbp ( VDM_HANDLE h, VDM_uint64_t addr );
VDM_RESULT VDMAPI vdm_getpc ( VDM_HANDLE h, VDM_uint64_t* npc );
VDM_RESULT VDMAPI vdm_setpc ( VDM_HANDLE h, VDM_uint64_t npc );

bool VDMAPI vdm_isalive ( VDM_HANDLE h );

bool VDMAPI vdm_editconfig ( HWND phwnd );
void VDMAPI vdm_getconfig ( VDM_CONFIG*, uint32_t size=sizeof ( VDM_CONFIG ) );
void VDMAPI vdm_setconfig ( VDM_CONFIG*, uint32_t size=sizeof ( VDM_CONFIG ) );

bool VDMAPI vdm_geterrmsg ( VDM_RESULT result, CHAR* buf, int32_t size );

#ifdef __cplusplus
}
#endif


#pragma pack(pop)

#endif

#endif

