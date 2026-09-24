#include <type_traits>
#include "active_model.hpp"
#include "model.hpp"
#include <lua.hpp>

extern "C" IDSIMMODEL *createdsimmodel(char *device, ILICENCESERVER *licenceServer);
extern "C" IACTIVEMODEL *createactivemodel(char *device, ILICENCESERVER *licenceServer);
extern "C" void deletedsimmodel(IDSIMMODEL *model);

namespace
{
static_assert(std::has_virtual_destructor_v<DeviceSimulator::VirtualDevice>);

// Grants every authorization request, so the factories create models.
class LicenceServer final : public ILICENCESERVER
{
  public:
    BOOL authorize(DWORD, DWORD) override
    {
        return TRUE;
    }
    BOOL authorizeex(DWORD, DWORD, DWORD) override
    {
        return TRUE;
    }
    DWORD getid() override
    {
        return 0;
    }
    CHAR *getname() override
    {
        return emptyText;
    }
    CHAR *getcompany() override
    {
        return emptyText;
    }
    CHAR *getexpirydate() override
    {
        return emptyText;
    }

  private:
    CHAR emptyText[1] = {'\0'};
};

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

// Models are created through the exported factories, as Proteus does, so the
// DLL both allocates and frees them. The test executable and openvsm.dll each
// link their own static CRT, so allocating here and freeing there would mix
// heaps, which debug builds stop on.
int main()
{
    deletedsimmodel(nullptr);

    LicenceServer licenceServer;
    constexpr int iterations = 64;
    for (int iteration = 0; iteration < iterations; ++iteration)
    {
        IDSIMMODEL *model = createdsimmodel(nullptr, &licenceServer);
        auto *device = dynamic_cast<DeviceSimulator::VirtualDevice *>(model);
        if (device == nullptr)
        {
            return 1;
        }
        installFinalizer(device->getLuaContext());
        deletedsimmodel(model);

        if (finalizerCalls != iteration + 1)
        {
            return 2;
        }
    }

    IACTIVEMODEL *activeModel = createactivemodel(nullptr, &licenceServer);
    if (activeModel == nullptr)
    {
        return 3;
    }
    IDSIMMODEL *digitalPart = activeModel->getdsimmodel(nullptr);
    auto *activeDevice = dynamic_cast<DeviceSimulator::VirtualDevice *>(digitalPart);
    if (activeDevice == nullptr)
    {
        return 4;
    }
    installFinalizer(activeDevice->getLuaContext());
    deletedsimmodel(digitalPart);

    return finalizerCalls == iterations + 1 ? 0 : 5;
}
