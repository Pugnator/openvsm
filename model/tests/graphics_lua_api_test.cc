#include "luabind/graphics_lua_api.hpp"

#include <cstdarg>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "runtime_logging.hpp"

namespace
{
class FakeComponent final : public ICOMPONENT
{
  public:
    INT drawScale = 0;
    INT penWidth = 0;
    COLOUR penColour = 0;
    COLOUR brushColour = 0;
    INT line[4] = {};
    INT box[4] = {};
    INT circle[3] = {};
    std::string text;
    BOOL repaintedWithErase = FALSE;
    DOUBLE timestep = 0;

    CHAR *getprop(CHAR *) override
    {
        return nullptr;
    }
    CHAR *getproptext() override
    {
        return nullptr;
    }
    void addprop(CHAR *, CHAR *, WORD) override
    {
    }
    void delprop(CHAR *) override
    {
    }
    void setproptext(CHAR *) override
    {
    }
    ACTIVESTATE getstate(INT, ACTIVEDATA *) override
    {
        return 0;
    }
    BOOL setstate(ACTIVESTATE) override
    {
        return TRUE;
    }
    void setdrawscale(INT value) override
    {
        drawScale = value;
    }
    HDC begincache(BOX &) override
    {
        return nullptr;
    }
    HDC begincache(INT) override
    {
        return nullptr;
    }
    void endcache() override
    {
    }
    HGFXSTYLE creategfxstyle(CHAR *) override
    {
        return nullptr;
    }
    void selectgfxstyle(HGFXSTYLE) override
    {
    }
    void setpenwidth(INT value) override
    {
        penWidth = value;
    }
    void setpencolour(COLOUR value) override
    {
        penColour = value;
    }
    void setbrushcolour(COLOUR value) override
    {
        brushColour = value;
    }
    void drawline(INT x1, INT y1, INT x2, INT y2) override
    {
        line[0] = x1;
        line[1] = y1;
        line[2] = x2;
        line[3] = y2;
    }
    void drawbox(INT x1, INT y1, INT x2, INT y2) override
    {
        box[0] = x1;
        box[1] = y1;
        box[2] = x2;
        box[3] = y2;
    }
    void drawbox(BOX &area) override
    {
        drawbox(area.x1, area.y1, area.x2, area.y2);
    }
    void drawcircle(INT x, INT y, INT radius) override
    {
        circle[0] = x;
        circle[1] = y;
        circle[2] = radius;
    }
    void drawbezier(POINT *, INT) override
    {
    }
    void drawpolyline(POINT *, INT) override
    {
    }
    void drawpolygon(POINT *, INT) override
    {
    }
    void drawsymbol(INT) override
    {
    }
    void drawsymbol(INT, INT, INT, INT, INT) override
    {
    }
    void drawstate(ACTIVESTATE) override
    {
    }
    BOOL getsymbolarea(INT symbol, BOX *area) override
    {
        if (symbol != 9 || !area)
        {
            return FALSE;
        }
        area->x1 = 1;
        area->y1 = 2;
        area->x2 = 30;
        area->y2 = 40;
        return TRUE;
    }
    BOOL getmarker(CHAR *name, POINT *position, INT *rotation, INT *mirror) override
    {
        if (std::strcmp(name, "ORIGIN") != 0)
        {
            return FALSE;
        }
        position->x = 5;
        position->y = 6;
        *rotation = 90;
        *mirror = MIR_X;
        return TRUE;
    }
    HTEXTSTYLE createtextstyle(CHAR *) override
    {
        return nullptr;
    }
    void selecttextstyle(HTEXTSTYLE) override
    {
    }
    void settextfont(CHAR *) override
    {
    }
    void settextsize(INT) override
    {
    }
    void setbold(BOOL) override
    {
    }
    void setitalic(BOOL) override
    {
    }
    void setunderline(BOOL) override
    {
    }
    void settextcolour(COLOUR) override
    {
    }
    void drawtext(INT, INT, INT, INT, CHAR *format, ...) override
    {
        va_list arguments;
        va_start(arguments, format);
        text = std::strcmp(format, "%s") == 0 ? va_arg(arguments, const char *) : format;
        va_end(arguments);
    }
    IPOPUP *createpopup(CREATEPOPUPSTRUCT *) override
    {
        return nullptr;
    }
    void deletepopup(POPUPID) override
    {
    }
    void settimestep(DOUBLE value) override
    {
        timestep = value;
    }
    void error(CHAR *, ...) override
    {
    }
    void repaint(BOOL erase) override
    {
        repaintedWithErase = erase;
    }
};

bool expect(bool condition, const char *message)
{
    if (!condition)
    {
        std::cerr << message << '\n';
    }
    return condition;
}

bool run(lua_State *lua, const char *script)
{
    if (luaL_dostring(lua, script) == LUA_OK)
    {
        return true;
    }
    std::cerr << lua_tostring(lua, -1) << '\n';
    lua_pop(lua, 1);
    return false;
}

lua_Integer globalInteger(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    const lua_Integer result = lua_tointeger(lua, -1);
    lua_pop(lua, 1);
    return result;
}

std::size_t countOccurrences(const std::string &text, const std::string &needle)
{
    std::size_t count = 0;
    for (std::size_t position = 0; (position = text.find(needle, position)) != std::string::npos;
         position += needle.size())
    {
        ++count;
    }
    return count;
}
} // namespace

int main()
{
    const std::filesystem::path logPath{"log.txt"};
    std::error_code fileError;
    std::filesystem::remove(logPath, fileError);
    DeviceSimulator::initializeRuntimeLogging();

    lua_State *lua = luaL_newstate();
    if (!expect(lua != nullptr, "Lua state creation failed"))
    {
        return 1;
    }
    luaL_openlibs(lua);
    const int initialTop = lua_gettop(lua);

    FakeComponent component;
    DeviceSimulator::registerGraphicsLuaApi(lua, &component);
    bool ok = expect(lua_gettop(lua) == initialTop, "graphics registration changed the Lua stack") && run(lua, R"(
                  graphics.set_draw_scale(96)
                  graphics.set_pen_width(3)
                  graphics.set_pen_colour(0x112233)
                  graphics.set_brush_colour(0x445566)
                  graphics.draw_line(1, 2, 3, 4)
                  graphics.draw_box(5, 6, 7, 8)
                  graphics.draw_circle(9, 10, 11)
                  graphics.draw_text(0, 0, 0, graphics.TXJ_LEFT, "100% safe")
                  marker_x, marker_y, marker_rotation, marker_mirror = graphics.get_marker("ORIGIN")
                  area_x1, area_y1, area_x2, area_y2 = graphics.get_symbol_area(9)
                  graphics.repaint()
                  graphics.set_timestep(0.000001)

                  function device_graphics_init()
                      init_called = (init_called or 0) + 1
                  end

                  function device_graphics_plot(state)
                      plot_state = state
                  end

                  function device_graphics_animate(element, data_type, value)
                      animate_element = element
                      animate_type = data_type
                      animate_value = value
                  end

                  function device_graphics_actuate(key, x, y, flags)
                      actuate_key = key
                      actuate_flags = flags
                      return x == 12 and y == 13
                  end
              )");

    ok = expect(component.drawScale == 96, "draw scale was not forwarded") &&
         expect(component.penWidth == 3, "pen width was not forwarded") &&
         expect(component.penColour == 0x112233, "pen colour was not forwarded") &&
         expect(component.brushColour == 0x445566, "brush colour was not forwarded") &&
         expect(component.line[0] == 1 && component.line[3] == 4, "line coordinates were not forwarded") &&
         expect(component.box[0] == 5 && component.box[3] == 8, "box coordinates were not forwarded") &&
         expect(component.circle[0] == 9 && component.circle[2] == 11, "circle was not forwarded") &&
         expect(component.text == "100% safe", "text was treated as a format string") &&
         expect(component.repaintedWithErase == TRUE, "default repaint did not erase") &&
         expect(component.timestep == 0.000001, "timestep was not forwarded") &&
         expect(globalInteger(lua, "marker_rotation") == 90, "marker result was not returned") &&
         expect(globalInteger(lua, "area_x2") == 30, "symbol area was not returned") && ok;

    ACTIVEDATA data = {};
    data.type = ADT_INTEGER;
    data.intval = 42;
    ok = expect(DeviceSimulator::dispatchLuaGraphicsInit(lua), "graphics init callback was not found") && ok;
    DeviceSimulator::dispatchLuaGraphicsPlot(lua, 77);
    DeviceSimulator::dispatchLuaGraphicsAnimate(lua, 3, &data);
    ok = expect(DeviceSimulator::dispatchLuaGraphicsActuate(lua, 65, 12, 13, ACF_LEFT | ACF_TOGGLE),
                "actuate callback result was not returned") &&
         expect(globalInteger(lua, "init_called") == 1, "graphics init was not dispatched") &&
         expect(globalInteger(lua, "plot_state") == 77, "plot state was not dispatched") &&
         expect(globalInteger(lua, "animate_element") == 3, "animation element was not dispatched") &&
         expect(globalInteger(lua, "animate_type") == ADT_INTEGER, "animation type was not dispatched") &&
         expect(globalInteger(lua, "animate_value") == 42, "animation value was not dispatched") &&
         expect(globalInteger(lua, "actuate_key") == 65, "actuate key was not dispatched") &&
         expect(lua_gettop(lua) == initialTop, "graphics callbacks changed the Lua stack") && ok;

    ABSTIME timepoints[] = {100, 200};
    STATE states[] = {SLO, SHI, FLT, TSTATE};
    data.type = ADT_DSIMDATA;
    data.dsimdata = {2, 2, timepoints, states};
    lua_pushinteger(lua, TSTATE);
    lua_setglobal(lua, "expected_tstate");
    DeviceSimulator::dispatchLuaGraphicsAnimate(lua, 4, &data);
    ok = expect(globalInteger(lua, "animate_element") == 4, "sample-block element was not dispatched") &&
         expect(globalInteger(lua, "animate_type") == ADT_DSIMDATA, "sample-block type was not dispatched") &&
         run(lua, "assert(animate_value.num_timepoints == 2); assert(animate_value.timepoints[2] == 200); "
                  "assert(animate_value.states[4] == expected_tstate)") &&
         expect(lua_gettop(lua) == initialTop, "sample-block callback changed the Lua stack") && ok;

    ok = run(lua, "function device_graphics_actuate() error('boom') end") && ok;
    ok = expect(!DeviceSimulator::dispatchLuaGraphicsActuate(lua, 0, 0, 0, 0),
                "throwing actuate callback was accepted") &&
         expect(!DeviceSimulator::dispatchLuaGraphicsActuate(lua, 0, 0, 0, 0),
                "repeated throwing actuate callback was accepted") &&
         expect(lua_gettop(lua) == initialTop, "throwing callback changed the Lua stack") && ok;

    std::ifstream diagnosticFile{logPath, std::ios::binary};
    const std::string diagnosticContents{std::istreambuf_iterator<char>{diagnosticFile},
                                         std::istreambuf_iterator<char>{}};
    ok = expect(diagnosticContents.find("device_graphics_actuate failed") != std::string::npos,
                "throwing callback name was not logged") &&
         expect(diagnosticContents.find("boom") != std::string::npos, "throwing callback error was not logged") &&
         expect(countOccurrences(diagnosticContents, "device_graphics_actuate failed") == 1,
                "throwing callback diagnostic was logged more than once") &&
         ok;

    lua_close(lua);
    return ok ? 0 : 1;
}
