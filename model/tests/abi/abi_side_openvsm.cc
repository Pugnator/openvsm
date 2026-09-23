// ABI facts of OpenVSM VSM headers, without <windows.h>.
// Headers from model/sdk.

#include <cstddef> // the vendor header expects NULL to be defined

#include <openvsm.hpp>
#include <openvdm.hpp>

#define ABI_SIDE_FUNCTION abiFactsOpenvsm
#include "abi_facts.inc"
