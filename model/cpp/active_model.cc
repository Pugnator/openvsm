#include "active_model.hpp"

#include "luabind/graphics_lua_api.hpp"

namespace DeviceSimulator
{
void LuaActiveModel::initialize(ICOMPONENT *component)
{
    component_ = component;
    if (component_)
    {
        registerGraphicsLuaApi(getLuaContext(), component_);
        ensureGraphicsInitialized();
    }
}

ISPICEMODEL *LuaActiveModel::getspicemodel(CHAR *primitive)
{
    (void)primitive;
    return nullptr;
}

IDSIMMODEL *LuaActiveModel::getdsimmodel(CHAR *primitive)
{
    (void)primitive;
    return this;
}

void LuaActiveModel::plot(ACTIVESTATE state)
{
    ensureGraphicsInitialized();
    dispatchLuaGraphicsPlot(getLuaContext(), state);
}

void LuaActiveModel::animate(INT element, ACTIVEDATA *newstate)
{
    ensureGraphicsInitialized();
    dispatchLuaGraphicsAnimate(getLuaContext(), element, newstate);
}

BOOL LuaActiveModel::actuate(WORD key, INT x, INT y, DWORD flags)
{
    ensureGraphicsInitialized();
    return dispatchLuaGraphicsActuate(getLuaContext(), key, x, y, flags) ? TRUE : FALSE;
}

void LuaActiveModel::setup(IINSTANCE *instance, IDSIMCKT *dsim)
{
    if (component_)
    {
        registerGraphicsLuaApi(getLuaContext(), component_);
    }
    VirtualDevice::setup(instance, dsim);
    ensureGraphicsInitialized();
}

void LuaActiveModel::ensureGraphicsInitialized()
{
    if (!graphicsInitialized_)
    {
        graphicsInitialized_ = dispatchLuaGraphicsInit(getLuaContext());
    }
}
} // namespace DeviceSimulator
