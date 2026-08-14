#pragma once

#include <string>

#include <lua.hpp>
#include <vsm.hpp>

namespace DeviceSimulator
{
struct BusDefinition
{
    std::string name;
    UINT base = 0;
    UINT width = 0;
    RELTIME on_time = 0;
    RELTIME off_time = 0;
    RELTIME tristate_time = 0;
    STATE on_state = TSTATE;
    STATE off_state = FSTATE;
    STATE tristate_state = FLT;
    BOOL required = TRUE;
};

bool readBusDefinition(lua_State *lua, int tableIndex, BusDefinition &definition, std::string &error);
void registerBusObject(lua_State *lua, const BusDefinition &definition, IBUSPIN *bus, IDSIMCKT *dsim);
} // namespace DeviceSimulator
