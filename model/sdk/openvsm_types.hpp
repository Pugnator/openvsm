// OpenVSM description of the Proteus VSM model interface: base types.
//
// This file is part of OpenVSM (GPL-2.0-or-later); see openvsm.hpp.
//
// The VSM interface is expressed in Windows-style type names. Each typedef
// below names the same type as <windows.h> does on 32-bit Windows, so
// repeating it after <windows.h> is harmless. The handle, message and geometry
// types are only supplied when <windows.h> has not been included, as with the
// vendor header; include <windows.h> first in a file that needs both.

#pragma once
#ifndef OPENVSM_SDK_OPENVSM_TYPES_HPP
#define OPENVSM_SDK_OPENVSM_TYPES_HPP

#ifndef VOID
#define VOID void
#endif

typedef double DOUBLE;
typedef char CHAR;
typedef unsigned char BYTE;
typedef short SHORT;
typedef int INT;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned int UINT;
typedef __int64 LONGLONG;
typedef unsigned long COLOUR;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned MESSAGE;

#ifndef _WINDOWS_
typedef VOID *HANDLE;
typedef VOID *HWND;
typedef VOID *HDC;
typedef unsigned WPARAM;
typedef long LPARAM;
typedef long LRESULT;

struct POINT
{
    LONG x;
    LONG y;
};

// Proteus drawing rectangle: two corners in component units.
struct BOX
{
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
};
#else
typedef RECT BOX; // same layout; the members are left, top, right, bottom
#endif

#endif // OPENVSM_SDK_OPENVSM_TYPES_HPP
