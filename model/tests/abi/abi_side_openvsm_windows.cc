// ABI facts of OpenVSM VSM headers, with <windows.h> included first.
// Headers from model/sdk.

#include <windows.h>

#include <cstddef> // the vendor header expects NULL to be defined

#include <openvsm.hpp>
#include <openvdm.hpp>

#define ABI_SIDE_FUNCTION abiFactsOpenvsmWithWindows
#include "abi_facts.inc"
