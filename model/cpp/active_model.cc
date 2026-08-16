#include "active_model.hpp"

#include <format>

#include "luabind/graphics_lua_api.hpp"
#include "runtime_logging.hpp"

namespace DeviceSimulator
{
void LuaActiveModel::initialize(ICOMPONENT *component)
{
    component_ = component;
    if (!component_)
    {
        logRuntimeDebug("Active model initialization did not receive a Proteus component surface");
        return;
    }

    logRuntimeDebug("Active model received the Proteus component surface");
    registerGraphicsLuaApi(getLuaContext(), component_);
}

ISPICEMODEL *LuaActiveModel::getspicemodel(CHAR *primitive)
{
    (void)primitive;
    return nullptr;
}

IDSIMMODEL *LuaActiveModel::getdsimmodel(CHAR *primitive)
{
    logRuntimeDebug(std::format("Active model attached its digital interface for primitive {}",
                                primitive && *primitive ? primitive : "<unknown>"));
    return this;
}

void LuaActiveModel::plot(ACTIVESTATE state)
{
    if (!firstPlotLogged_)
    {
        logRuntimeDebug(std::format("Active model received its first plot request with state {}", state));
        firstPlotLogged_ = true;
    }
    if (!simulationReadyForGraphics_)
    {
        return;
    }
    ensureGraphicsInitialized();
    dispatchLuaGraphicsPlot(getLuaContext(), state);
}

void LuaActiveModel::animate(INT element, ACTIVEDATA *newstate)
{
    if (!firstAnimationLogged_)
    {
        logRuntimeDebug(std::format("Active model received its first animation event for element {}", element));
        firstAnimationLogged_ = true;
    }
    if (!simulationReadyForGraphics_)
    {
        return;
    }
    ensureGraphicsInitialized();
    dispatchLuaGraphicsAnimate(getLuaContext(), element, newstate);
}

BOOL LuaActiveModel::actuate(WORD key, INT x, INT y, DWORD flags)
{
    if (!firstActuationLogged_)
    {
        logRuntimeDebug("Active model received its first actuation event");
        firstActuationLogged_ = true;
    }
    if (!simulationReadyForGraphics_)
    {
        return FALSE;
    }
    ensureGraphicsInitialized();
    return dispatchLuaGraphicsActuate(getLuaContext(), key, x, y, flags) ? TRUE : FALSE;
}

void LuaActiveModel::setup(IINSTANCE *instance, IDSIMCKT *dsim)
{
    logRuntimeDebug("Setting up the active model simulation interface");
    if (component_)
    {
        registerGraphicsLuaApi(getLuaContext(), component_);
    }
    VirtualDevice::setup(instance, dsim);
    simulationReadyForGraphics_ = true;
    ensureGraphicsInitialized();
}

void LuaActiveModel::ensureGraphicsInitialized()
{
    if (simulationReadyForGraphics_ && !graphicsInitialized_)
    {
        graphicsInitialized_ = dispatchLuaGraphicsInit(getLuaContext());
        if (graphicsInitialized_)
        {
            logRuntimeDebug("Lua graphics initialization completed");
        }
    }
}
} // namespace DeviceSimulator
