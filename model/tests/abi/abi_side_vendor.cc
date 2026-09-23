// ABI facts of Labcenter's VSM headers, without <windows.h>.
// Headers from externals/sdk, supplied locally and never committed.

#include <cstddef> // the vendor header expects NULL to be defined

#include <vsm.hpp>
#include <vdm.hpp>

#define ABI_SIDE_FUNCTION abiFactsVendor
#include "abi_facts.inc"
