// Checks that OpenVSM's own VSM headers (model/sdk) are binary compatible with
// Labcenter's SDK headers. It is built only when the vendor headers are present
// in externals/sdk, which is never the case in CI; run it locally after any
// change to model/sdk. With --dump it also prints every fact it measured.

#include <cstdio>
#include <cstring>

#include "abi_table.hpp"

namespace
{
int compare(const char *label, AbiTable ours, AbiTable vendor)
{
    int failures = 0;
    if (ours.count != vendor.count)
    {
        std::printf("%s: %d facts from model/sdk, %d from the vendor header\n", label, ours.count, vendor.count);
        ++failures;
    }

    const int count = ours.count < vendor.count ? ours.count : vendor.count;
    for (int i = 0; i < count; ++i)
    {
        const AbiFact &a = ours.facts[i];
        const AbiFact &b = vendor.facts[i];
        if (std::strcmp(a.name, b.name) != 0)
        {
            std::printf("%s: fact %d is %s here but %s in the vendor table\n", label, i, a.name, b.name);
            ++failures;
            continue;
        }
        if (a.value != b.value)
        {
            std::printf("%s: %s is %lld in model/sdk but %lld in the vendor header\n", label, a.name, a.value, b.value);
            ++failures;
        }
    }

    std::printf("%s: %d facts compared, %d differences\n", label, count, failures);
    return failures;
}

void dump(const char *label, AbiTable table)
{
    for (int i = 0; i < table.count; ++i)
    {
        std::printf("%s %s = %lld\n", label, table.facts[i].name, table.facts[i].value);
    }
}
} // namespace

int main(int argc, char **argv)
{
    if (argc > 1 && std::strcmp(argv[1], "--dump") == 0)
    {
        dump("plain", abiFactsOpenvsm());
        dump("windows", abiFactsOpenvsmWithWindows());
    }

    int failures = compare("without <windows.h>", abiFactsOpenvsm(), abiFactsVendor());
    failures += compare("with <windows.h>", abiFactsOpenvsmWithWindows(), abiFactsVendorWithWindows());
    return failures == 0 ? 0 : 1;
}
