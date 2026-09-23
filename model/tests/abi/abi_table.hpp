#pragma once

// One measured property of a VSM header: a size, an offset, a constant, or
// the method found in a vtable slot. Two headers are binary compatible when
// they produce identical tables.
struct AbiFact
{
    const char *name;
    long long value;
};

struct AbiTable
{
    const AbiFact *facts;
    int count;
};

// Each side is compiled from abi_facts.inc against one header.
AbiTable abiFactsOpenvsm();
AbiTable abiFactsVendor();
AbiTable abiFactsOpenvsmWithWindows();
AbiTable abiFactsVendorWithWindows();
