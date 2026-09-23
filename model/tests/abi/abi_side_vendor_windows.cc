// ABI facts of Labcenter's VSM headers, with <windows.h> included first.
// Headers from externals/sdk, supplied locally and never committed.

#include <windows.h>

#include <cstddef> // the vendor header expects NULL to be defined

#include <vsm.hpp>
#include <vdm.hpp>

#define ABI_SIDE_FUNCTION abiFactsVendorWithWindows
#include "abi_facts.inc"
