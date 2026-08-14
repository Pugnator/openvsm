#include "graphics_lua_api.hpp"

#include <cstdint>

namespace
{
class StackRestore
{
  public:
    explicit StackRestore(lua_State *lua) : lua_(lua), top_(lua_gettop(lua))
    {
    }

    ~StackRestore()
    {
        lua_settop(lua_, top_);
    }

  private:
    lua_State *lua_;
    int top_;
};

ICOMPONENT *component(lua_State *lua)
{
    return static_cast<ICOMPONENT *>(lua_touserdata(lua, lua_upvalueindex(1)));
}

int setDrawScale(lua_State *lua)
{
    component(lua)->setdrawscale(static_cast<INT>(luaL_checkinteger(lua, 1)));
    return 0;
}

int setPenWidth(lua_State *lua)
{
    component(lua)->setpenwidth(static_cast<INT>(luaL_checkinteger(lua, 1)));
    return 0;
}

int setPenColour(lua_State *lua)
{
    component(lua)->setpencolour(static_cast<COLOUR>(luaL_checkinteger(lua, 1)));
    return 0;
}

int setBrushColour(lua_State *lua)
{
    component(lua)->setbrushcolour(static_cast<COLOUR>(luaL_checkinteger(lua, 1)));
    return 0;
}

int setTextFont(lua_State *lua)
{
    const char *font = luaL_checkstring(lua, 1);
    component(lua)->settextfont(const_cast<char *>(font));
    return 0;
}

int setTextSize(lua_State *lua)
{
    component(lua)->settextsize(static_cast<INT>(luaL_checkinteger(lua, 1)));
    return 0;
}

int setBold(lua_State *lua)
{
    component(lua)->setbold(lua_toboolean(lua, 1) ? TRUE : FALSE);
    return 0;
}

int setItalic(lua_State *lua)
{
    component(lua)->setitalic(lua_toboolean(lua, 1) ? TRUE : FALSE);
    return 0;
}

int setUnderline(lua_State *lua)
{
    component(lua)->setunderline(lua_toboolean(lua, 1) ? TRUE : FALSE);
    return 0;
}

int setTextColour(lua_State *lua)
{
    component(lua)->settextcolour(static_cast<COLOUR>(luaL_checkinteger(lua, 1)));
    return 0;
}

int drawLine(lua_State *lua)
{
    component(lua)->drawline(static_cast<INT>(luaL_checkinteger(lua, 1)), static_cast<INT>(luaL_checkinteger(lua, 2)),
                             static_cast<INT>(luaL_checkinteger(lua, 3)), static_cast<INT>(luaL_checkinteger(lua, 4)));
    return 0;
}

int drawBox(lua_State *lua)
{
    component(lua)->drawbox(static_cast<INT>(luaL_checkinteger(lua, 1)), static_cast<INT>(luaL_checkinteger(lua, 2)),
                            static_cast<INT>(luaL_checkinteger(lua, 3)), static_cast<INT>(luaL_checkinteger(lua, 4)));
    return 0;
}

int drawCircle(lua_State *lua)
{
    component(lua)->drawcircle(static_cast<INT>(luaL_checkinteger(lua, 1)), static_cast<INT>(luaL_checkinteger(lua, 2)),
                               static_cast<INT>(luaL_checkinteger(lua, 3)));
    return 0;
}

int drawSymbol(lua_State *lua)
{
    if (lua_gettop(lua) == 1)
    {
        component(lua)->drawsymbol(static_cast<INT>(luaL_checkinteger(lua, 1)));
    }
    else
    {
        component(lua)->drawsymbol(
            static_cast<INT>(luaL_checkinteger(lua, 1)), static_cast<INT>(luaL_checkinteger(lua, 2)),
            static_cast<INT>(luaL_checkinteger(lua, 3)), static_cast<INT>(luaL_checkinteger(lua, 4)),
            static_cast<INT>(luaL_checkinteger(lua, 5)));
    }
    return 0;
}

int drawState(lua_State *lua)
{
    component(lua)->drawstate(static_cast<ACTIVESTATE>(luaL_checkinteger(lua, 1)));
    return 0;
}

int drawText(lua_State *lua)
{
    const INT x = static_cast<INT>(luaL_checkinteger(lua, 1));
    const INT y = static_cast<INT>(luaL_checkinteger(lua, 2));
    const INT rotation = static_cast<INT>(luaL_checkinteger(lua, 3));
    const INT justification = static_cast<INT>(luaL_checkinteger(lua, 4));
    const char *text = luaL_checkstring(lua, 5);
    component(lua)->drawtext(x, y, rotation, justification, const_cast<char *>("%s"), text);
    return 0;
}

int getSymbolArea(lua_State *lua)
{
    BOX area = {};
    if (!component(lua)->getsymbolarea(static_cast<INT>(luaL_checkinteger(lua, 1)), &area))
    {
        return 0;
    }

    lua_pushinteger(lua, area.x1);
    lua_pushinteger(lua, area.y1);
    lua_pushinteger(lua, area.x2);
    lua_pushinteger(lua, area.y2);
    return 4;
}

int getMarker(lua_State *lua)
{
    const char *name = luaL_checkstring(lua, 1);
    POINT position = {};
    INT rotation = 0;
    INT mirror = 0;
    if (!component(lua)->getmarker(const_cast<char *>(name), &position, &rotation, &mirror))
    {
        return 0;
    }

    lua_pushinteger(lua, position.x);
    lua_pushinteger(lua, position.y);
    lua_pushinteger(lua, rotation);
    lua_pushinteger(lua, mirror);
    return 4;
}

int repaint(lua_State *lua)
{
    const bool erase = lua_gettop(lua) == 0 || lua_toboolean(lua, 1);
    component(lua)->repaint(erase ? TRUE : FALSE);
    return 0;
}

int setTimestep(lua_State *lua)
{
    component(lua)->settimestep(luaL_checknumber(lua, 1));
    return 0;
}

void setFunction(lua_State *lua, ICOMPONENT *target, const char *name, lua_CFunction function)
{
    lua_pushlightuserdata(lua, target);
    lua_pushcclosure(lua, function, 1);
    lua_setfield(lua, -2, name);
}

void setInteger(lua_State *lua, const char *name, lua_Integer value)
{
    lua_pushinteger(lua, value);
    lua_setfield(lua, -2, name);
}

bool pushCallback(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    if (lua_isfunction(lua, -1))
    {
        return true;
    }
    lua_pop(lua, 1);
    return false;
}

void setTableInteger(lua_State *lua, const char *name, lua_Integer value)
{
    lua_pushinteger(lua, value);
    lua_setfield(lua, -2, name);
}

void pushActiveData(lua_State *lua, const ACTIVEDATA *data)
{
    if (!data)
    {
        lua_pushnil(lua);
        return;
    }

    switch (data->type)
    {
    case ADT_REAL:
    case ADT_PINVOLTAGE:
        lua_pushnumber(lua, data->realval);
        return;
    case ADT_BOOLEAN:
        lua_pushboolean(lua, data->intval != 0);
        return;
    case ADT_INTEGER:
        lua_pushinteger(lua, data->intval);
        return;
    case ADT_STATE:
    case ADT_PINSTATE:
        lua_pushinteger(lua, data->stateval);
        return;
    case ADT_WIREINFO:
        lua_createtable(lua, 0, 2);
        lua_pushnumber(lua, data->wireinfo[0]);
        lua_setfield(lua, -2, "voltage");
        lua_pushnumber(lua, data->wireinfo[1]);
        lua_setfield(lua, -2, "current");
        return;
    case ADT_SPICEDATA:
    {
        const auto &source = data->spicedata;
        const uint64_t sampleCount = static_cast<uint64_t>(source.numtimepoints) * source.numpins;
        if ((source.numtimepoints != 0 && !source.timepoints) || (sampleCount != 0 && !source.nodedata) ||
            source.numtimepoints > 1024 * 1024 || sampleCount > 1024 * 1024)
        {
            lua_pushnil(lua);
            return;
        }
        lua_createtable(lua, 0, 4);
        setTableInteger(lua, "num_timepoints", source.numtimepoints);
        setTableInteger(lua, "num_pins", source.numpins);
        lua_createtable(lua, static_cast<int>(source.numtimepoints), 0);
        for (DWORD i = 0; i < source.numtimepoints; ++i)
        {
            lua_pushnumber(lua, source.timepoints[i]);
            lua_rawseti(lua, -2, i + 1);
        }
        lua_setfield(lua, -2, "timepoints");
        lua_createtable(lua, static_cast<int>(sampleCount), 0);
        for (uint64_t i = 0; i < sampleCount; ++i)
        {
            lua_pushnumber(lua, source.nodedata[i]);
            lua_rawseti(lua, -2, static_cast<lua_Integer>(i + 1));
        }
        lua_setfield(lua, -2, "nodes");
        return;
    }
    case ADT_DSIMDATA:
    {
        const auto &source = data->dsimdata;
        const uint64_t sampleCount = static_cast<uint64_t>(source.numtimepoints) * source.numpins;
        if ((source.numtimepoints != 0 && !source.timepoints) || (sampleCount != 0 && !source.nodedata) ||
            source.numtimepoints > 1024 * 1024 || sampleCount > 1024 * 1024)
        {
            lua_pushnil(lua);
            return;
        }
        lua_createtable(lua, 0, 4);
        setTableInteger(lua, "num_timepoints", source.numtimepoints);
        setTableInteger(lua, "num_pins", source.numpins);
        lua_createtable(lua, static_cast<int>(source.numtimepoints), 0);
        for (DWORD i = 0; i < source.numtimepoints; ++i)
        {
            lua_pushinteger(lua, source.timepoints[i]);
            lua_rawseti(lua, -2, i + 1);
        }
        lua_setfield(lua, -2, "timepoints");
        lua_createtable(lua, static_cast<int>(sampleCount), 0);
        for (uint64_t i = 0; i < sampleCount; ++i)
        {
            lua_pushinteger(lua, source.nodedata[i]);
            lua_rawseti(lua, -2, static_cast<lua_Integer>(i + 1));
        }
        lua_setfield(lua, -2, "states");
        return;
    }
    case ADT_VOID:
    case ADT_USER:
    default:
        lua_pushnil(lua);
        return;
    }
}
} // namespace

namespace DeviceSimulator
{
void registerGraphicsLuaApi(lua_State *lua, ICOMPONENT *target)
{
    if (!lua || !target)
    {
        return;
    }

    StackRestore restore(lua);
    lua_createtable(lua, 0, 64);

    setFunction(lua, target, "set_draw_scale", setDrawScale);
    setFunction(lua, target, "set_pen_width", setPenWidth);
    setFunction(lua, target, "set_pen_colour", setPenColour);
    setFunction(lua, target, "set_brush_colour", setBrushColour);
    setFunction(lua, target, "set_text_font", setTextFont);
    setFunction(lua, target, "set_text_size", setTextSize);
    setFunction(lua, target, "set_bold", setBold);
    setFunction(lua, target, "set_italic", setItalic);
    setFunction(lua, target, "set_underline", setUnderline);
    setFunction(lua, target, "set_text_colour", setTextColour);
    setFunction(lua, target, "draw_line", drawLine);
    setFunction(lua, target, "draw_box", drawBox);
    setFunction(lua, target, "draw_circle", drawCircle);
    setFunction(lua, target, "draw_symbol", drawSymbol);
    setFunction(lua, target, "draw_state", drawState);
    setFunction(lua, target, "draw_text", drawText);
    setFunction(lua, target, "get_symbol_area", getSymbolArea);
    setFunction(lua, target, "get_marker", getMarker);
    setFunction(lua, target, "repaint", repaint);
    setFunction(lua, target, "set_timestep", setTimestep);

    setInteger(lua, "BLACK", BLACK);
    setInteger(lua, "WHITE", WHITE);
    setInteger(lua, "GREY", GREY);
    setInteger(lua, "RED", RED);
    setInteger(lua, "GREEN", GREEN);
    setInteger(lua, "BLUE", BLUE);
    setInteger(lua, "CYAN", CYAN);
    setInteger(lua, "MAGENTA", MAGENTA);
    setInteger(lua, "YELLOW", YELLOW);
    setInteger(lua, "BRIGHTWHITE", BRIGHTWHITE);
    setInteger(lua, "BRIGHTRED", BRIGHTRED);
    setInteger(lua, "BRIGHTGREEN", BRIGHTGREEN);
    setInteger(lua, "BRIGHTBLUE", BRIGHTBLUE);
    setInteger(lua, "TXJ_LEFT", TXJ_LEFT);
    setInteger(lua, "TXJ_RIGHT", TXJ_RIGHT);
    setInteger(lua, "TXJ_CENTRE", TXJ_CENTRE);
    setInteger(lua, "TXJ_TOP", TXJ_TOP);
    setInteger(lua, "TXJ_MIDDLE", TXJ_MIDDLE);
    setInteger(lua, "TXJ_BOTTOM", TXJ_BOTTOM);
    setInteger(lua, "ACF_LEFT", ACF_LEFT);
    setInteger(lua, "ACF_RIGHT", ACF_RIGHT);
    setInteger(lua, "ACF_MIDDLE", ACF_MIDDLE);
    setInteger(lua, "ACF_INC", ACF_INC);
    setInteger(lua, "ACF_DEC", ACF_DEC);
    setInteger(lua, "ACF_TOGGLE", ACF_TOGGLE);
    setInteger(lua, "ADT_VOID", ADT_VOID);
    setInteger(lua, "ADT_REAL", ADT_REAL);
    setInteger(lua, "ADT_BOOLEAN", ADT_BOOLEAN);
    setInteger(lua, "ADT_INTEGER", ADT_INTEGER);
    setInteger(lua, "ADT_STATE", ADT_STATE);
    setInteger(lua, "ADT_PINVOLTAGE", ADT_PINVOLTAGE);
    setInteger(lua, "ADT_PINSTATE", ADT_PINSTATE);
    setInteger(lua, "ADT_WIREINFO", ADT_WIREINFO);
    setInteger(lua, "ADT_SPICEDATA", ADT_SPICEDATA);
    setInteger(lua, "ADT_DSIMDATA", ADT_DSIMDATA);

    lua_setglobal(lua, "graphics");
}

bool dispatchLuaGraphicsInit(lua_State *lua)
{
    if (!lua)
    {
        return false;
    }

    StackRestore restore(lua);
    if (!pushCallback(lua, "device_graphics_init"))
    {
        return false;
    }
    lua_pcall(lua, 0, 0, 0);
    return true;
}

void dispatchLuaGraphicsPlot(lua_State *lua, ACTIVESTATE state)
{
    if (!lua)
    {
        return;
    }

    StackRestore restore(lua);
    if (!pushCallback(lua, "device_graphics_plot"))
    {
        return;
    }
    lua_pushinteger(lua, state);
    lua_pcall(lua, 1, 0, 0);
}

void dispatchLuaGraphicsAnimate(lua_State *lua, INT element, const ACTIVEDATA *data)
{
    if (!lua)
    {
        return;
    }

    StackRestore restore(lua);
    if (!pushCallback(lua, "device_graphics_animate"))
    {
        return;
    }
    lua_pushinteger(lua, element);
    lua_pushinteger(lua, data ? data->type : ADT_VOID);
    pushActiveData(lua, data);
    lua_pcall(lua, 3, 0, 0);
}

bool dispatchLuaGraphicsActuate(lua_State *lua, WORD key, INT x, INT y, DWORD flags)
{
    if (!lua)
    {
        return false;
    }

    StackRestore restore(lua);
    if (!pushCallback(lua, "device_graphics_actuate"))
    {
        return false;
    }
    lua_pushinteger(lua, key);
    lua_pushinteger(lua, x);
    lua_pushinteger(lua, y);
    lua_pushinteger(lua, flags);
    if (lua_pcall(lua, 4, 1, 0) != LUA_OK)
    {
        return false;
    }
    return lua_toboolean(lua, -1) != 0;
}
} // namespace DeviceSimulator
