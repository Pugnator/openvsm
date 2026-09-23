// OpenVSM description of the Proteus Virtual Debug Monitor (VDM) protocol.
//
// This file is part of OpenVSM (GPL-2.0-or-later). It declares the command
// codes, result codes and structures a CPU model exchanges with the Proteus
// debugger through ICPU::vdmhlr, independently of the proprietary SDK. The
// external client library API is not declared. Values and layouts are checked
// by model/tests/abi/vsm_abi_test.cc; see openvsm.hpp for the rules.

#pragma once
#ifndef OPENVSM_SDK_OPENVDM_HPP
#define OPENVSM_SDK_OPENVDM_HPP

#include "openvsm_types.hpp"

#define VDM_API_VERSION 101

// Target register-set classes.
#define VDM51_CLASS 1
#define VDM11_CLASS 2
#define VDMPIC16_CLASS 3
#define VDMAVR_CLASS 4
#define VDMPIC18_CLASS 5
#define VDMARM7_CLASS 10

typedef unsigned __int64 UL64;
typedef double FL64;

// Default TCP port of the debug monitor.
#define VDM_PORT 8000
#define VDM_INVALID_HANDLE -1

// Commands.
#define VDM_INIT 1
#define VDM_TERM 2
#define VDM_PLAY 3
#define VDM_STEP 4
#define VDM_PAUSE 5
#define VDM_WRITEDATA 6
#define VDM_READDATA 7
#define VDM_READREGS 8
#define VDM_WRITEREGS 9
#define VDM_SETBP 10
#define VDM_CLRBP 11
#define VDM_SETPC 12
#define VDM_GETPC 13
#define VDM_RESET 14
#define VDM_GETTID 15

// Results returned by a model's command handler.
#define ERR_VDM_FAILED -1
#define ERR_VDM_OK 0
#define ERR_VDM_TIMEOUT 1
#define ERR_VDM_BADCOMMAND 2
#define ERR_VDM_BADADDRESS 3
#define ERR_VDM_BADDATALEN 4
#define ERR_VDM_SIMFAILED 5
#define ERR_VDM_NOTARGET 6

// Results produced only by the client library.
#define ERR_VDM_PENDING 100
#define ERR_VDM_PLAYDEAD 101
#define ERR_VDM_BADHANDLE 102

#pragma pack(push, 4)

typedef BYTE VDM_MEMSPACE;
typedef DWORD VDM_ADDRESS;
typedef LONG VDM_RESULT;
typedef CHAR VDM_TARGETID[64];

// One debugger request. Payload bytes, if any, travel separately.
struct VDM_COMMAND
{
    BYTE command;
    BYTE memspace;
    DWORD address;
    DWORD datalength;
};

// Target description returned for VDM_INIT (128 bytes).
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

#pragma pack(pop)

#endif // OPENVSM_SDK_OPENVDM_HPP
