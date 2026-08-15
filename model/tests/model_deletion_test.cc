#include <type_traits>
#include "active_model.hpp"
#include "model.hpp"
#include <lua.hpp>

extern "C" void deletedsimmodel(IDSIMMODEL *model);

namespace
{
static_assert(std::has_virtual_destructor_v<DeviceSimulator::VirtualDevice>);

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

    constexpr int iterations = 64;
    for (int iteration = 0; iteration < iterations; ++iteration)
    {
        auto *device = new DeviceSimulator::VirtualDevice;
        installFinalizer(device->getLuaContext());

        IDSIMMODEL *baseModel = device;
        deletedsimmodel(baseModel);

        if (finalizerCalls != iteration + 1)
        {
            return 1;
        }
    }

    auto *activeModel = new DeviceSimulator::LuaActiveModel;
    installFinalizer(activeModel->getLuaContext());
    deletedsimmodel(activeModel->getdsimmodel(nullptr));

    return finalizerCalls == iterations + 1 ? 0 : 2;
}
