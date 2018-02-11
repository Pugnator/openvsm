/**
 *
 * @file   defines.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */


#ifndef VSMAPI_DEFINES
#define VSMAPI_DEFINES

#include <vsmapi.h>

typedef uint32_t COLOUR;
typedef uint32_t MESSAGE;

#define IDSIMPIN IDSIMPIN1
//#define IDSIMPIN IDSIMPIN2

typedef uint32_t ADDRESS;
#define WATCHITEM_NAME_SIZE 128
#define WATCHITEM_ADDR_SIZE 32


// Flags for controlling property visibility
#define SHOW_ALL      0
#define HIDE_KEYWORD  1
#define HIDE_VALUE    2
#define HIDE_ALL      3
#define HIDE_NOMODIFY 4

// Useful Colours:
#define  MAKECOLOUR(r,g,b) ((COLOUR)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint32_t)(uint8_t)(b))<<16)))
#define  BLACK           MAKECOLOUR(0x00,0x00,0x00)
#define  BLUE            MAKECOLOUR(0x00,0x00,0xC0)
#define  GREEN           MAKECOLOUR(0x00,0xC0,0x00)
#define  CYAN            MAKECOLOUR(0x00,0xC0,0xC0)
#define  RED             MAKECOLOUR(0xC0,0x00,0x00)
#define  MAGENTA         MAKECOLOUR(0xC0,0x00,0xC0)
#define  YELLOW          MAKECOLOUR(0xC0,0xC0,0x00)
#define  WHITE           MAKECOLOUR(0xC0,0xC0,0xC0)
#define  GREY            MAKECOLOUR(0x80,0x80,0x80)
#define  DARKBLUE        MAKECOLOUR(0x00,0x00,0x40)
#define  DARKGREEN       MAKECOLOUR(0x00,0x40,0x00)
#define  DARKCYAN        MAKECOLOUR(0x00,0x40,0x40)
#define  DARKRED         MAKECOLOUR(0x40,0x00,0x00)
#define  DARKMAGENTA     MAKECOLOUR(0x40,0x00,0x40)
#define  DARKYELLOW      MAKECOLOUR(0x40,0x40,0x00)
#define  DARKGREY        MAKECOLOUR(0x40,0x40,0x40)
#define  MIDBLUE         MAKECOLOUR(0x00,0x00,0x80)
#define  MIDGREEN        MAKECOLOUR(0x00,0x80,0x00)
#define  MIDCYAN         MAKECOLOUR(0x00,0x80,0x80)
#define  MIDRED          MAKECOLOUR(0x80,0x00,0x00)
#define  MIDMAGENTA      MAKECOLOUR(0x80,0x00,0x80)
#define  MIDYELLOW       MAKECOLOUR(0x80,0x80,0x00)
#define  MIDGREY         MAKECOLOUR(0x80,0x80,0x80)
#define  LIGHTBLUE       MAKECOLOUR(0x00,0x00,0xC0)
#define  LIGHTGREEN      MAKECOLOUR(0x00,0xC0,0x00)
#define  LIGHTCYAN       MAKECOLOUR(0x00,0xC0,0xC0)
#define  LIGHTRED        MAKECOLOUR(0xC0,0x00,0x00)
#define  LIGHTMAGENTA    MAKECOLOUR(0xC0,0x00,0xC0)
#define  LIGHTYELLOW     MAKECOLOUR(0xC0,0xC0,0x00)
#define  LIGHTGREY       MAKECOLOUR(0xC0,0xC0,0xC0)
#define  BRIGHTBLUE      MAKECOLOUR(0x00,0x00,0xFF)
#define  BRIGHTGREEN     MAKECOLOUR(0x00,0xFF,0x00)
#define  BRIGHTCYAN      MAKECOLOUR(0x00,0xFF,0xFF)
#define  BRIGHTRED       MAKECOLOUR(0xFF,0x00,0x00)
#define  BRIGHTMAGENTA   MAKECOLOUR(0xFF,0x00,0xFF)
#define  BRIGHTYELLOW    MAKECOLOUR(0xFF,0xFF,0x00)
#define  BRIGHTWHITE     MAKECOLOUR(0xFF,0xFF,0xFF)

#define  INVISIBLE       -1
#define  NOCOLOUR        -1



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
typedef void IPOPUP;
typedef uint32_t POPUPID;
typedef uint32_t INSTANCEID;

typedef double REALTIME;
typedef LONGLONG ABSTIME;
//typedef const ABSTIME &CREFABSTIME;
typedef LONGLONG RELTIME;
//typedef const RELTIME &CREFRELTIME;

// Time Conversion Macros
#define DSIMTICK    1e-12

// Maximum bits in bus:
#define MAXBUSBITS     32

typedef double SPICEFREQ;

typedef int64_t ACTIVESTATE;

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
#define SF_MESSAGE      0x40           // Message associated with state (?)
#define SF_MASK         0xE0           // Use to mask off flags.

typedef long EVENTID;
#define EID_BREAKPOINT 0x8000000

// Pin types:
typedef int32_t SPICENODE;
typedef void* DSIMNODE;

#endif
