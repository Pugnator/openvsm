#include <string>
#include <vector>
#include "model.hpp"
#include <lua.hpp>

extern "C" void deletedsimmodel(IDSIMMODEL *model);

namespace
{
int finalizerCalls = 0;

int countFinalizer(lua_State *luaContext)
{
    (void)luaContext;
    ++finalizerCalls;
    return 0;
}

void installFinalizer(lua_State *luaContext)
{
    lua_newuserdatauv(luaContext, 1, 0);
    lua_newtable(luaContext);
    lua_pushcfunction(luaContext, countFinalizer);
    lua_setfield(luaContext, -2, "__gc");
    lua_setmetatable(luaContext, -2);
    lua_setglobal(luaContext, "owned_resource");
}
} // namespace

int main()
{
    deletedsimmodel(nullptr);

    auto &manager = DeviceSimulator::VirtualContextManager::getInstance();
    constexpr int iterations = 64;
    for (int iteration = 0; iteration < iterations; ++iteration)
    {
        auto *device = new DeviceSimulator::VirtualDevice;
        const std::string id = "deletion-test-" + std::to_string(iteration);
        manager.registerDevice(id, *device);
        manager.setCurrentDevice(id);
        installFinalizer(device->getLuaContext());

        IDSIMMODEL *baseModel = device;
        deletedsimmodel(baseModel);

        if (finalizerCalls != iteration + 1)
        {
            return 1;
        }
        if (manager.getDevice(id) != nullptr)
        {
            return 2;
        }
        if (manager.getDevice() != nullptr)
        {
            return 3;
        }
    }

    return finalizerCalls == iterations ? 0 : 4;
}
