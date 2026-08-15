#include "model.hpp"

#include <set>

namespace
{
constexpr char randomSeedSequenceKey[] = "openvsm.random_seed.sequence";
}

int main()
{
    std::set<lua_Integer> observedSequences;
    constexpr int deviceCount = 128;

    for (int index = 0; index < deviceCount; ++index)
    {
        DeviceSimulator::VirtualDevice device;
        auto *lua = device.getLuaContext();
        lua_getfield(lua, LUA_REGISTRYINDEX, randomSeedSequenceKey);
        if (!lua_isinteger(lua, -1))
        {
            return 1;
        }

        const auto sequence = lua_tointeger(lua, -1);
        lua_pop(lua, 1);
        if (!observedSequences.insert(sequence).second)
        {
            return 2;
        }
    }

    return observedSequences.size() == deviceCount ? 0 : 3;
}
