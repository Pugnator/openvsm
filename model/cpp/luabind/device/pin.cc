#include <log/log.hpp>
#include "model.hpp"
#include "lua.hpp"

namespace DeviceSimulator
{
#define TEXT_PIN_FIELD "pin"
#define TEXT_HI_FIELD "hi"
#define TEXT_LO_FIELD "lo"
#define TEXT_FL_FIELD "fl"
#define TEXT_SET_FIELD "set"
#define TEXT_GET_FIELD "get"
#define TEXT_TOGGLE_FIELD "toggle"

#define TEXT_IS_HI_FIELD "is_hi"
#define TEXT_IS_LO_FIELD "is_lo"
#define TEXT_IS_FL_FIELD "is_fl"
#define TEXT_IS_EDGE_FIELD "is_edge"
#define TEXT_IS_PEDGE_FIELD "is_pedge"
#define TEXT_IS_NEDGE_FIELD "is_nedge"
#define TEXT_IS_STEADY_FIELD "is_steady"
#define TEXT_IS_ACTIVE_FIELD "is_active"
#define TEXT_IS_INACTIVE_FIELD "is_inactive"
#define TEXT_IS_INVERTED_FIELD "is_inverted"
#define TEXT_SET_STATE_FIELD "set_state"
#define TEXT_GET_STATE_FIELD "get_state"

  /*

        virtual BOOL invert() = 0;
        virtual STATE istate() = 0;
        virtual BOOL issteady () = 0;         // Will false for return *any* change of activity.
        virtual INT  activity () = 0;
        virtual BOOL isactive () = 0;
        virtual BOOL isinactive () = 0;
        virtual BOOL isposedge () = 0;
        virtual BOOL isnegedge () = 0;
        virtual BOOL isedge () = 0;           // Will return true only for a full edge transition.
        virtual EVENT *setstate (ABSTIME time, RELTIME tlh, RELTIME thl, RELTIME tgq, STATE state) = 0;
        virtual EVENT *setstate (ABSTIME time, RELTIME tgq, STATE state) = 0;
        virtual VOID setstate (STATE state) = 0;
        virtual VOID sethandler (IDSIMMODEL *model, PINHANDLERFN phf) = 0;
        virtual DSIMNODE getnode() = 0;
        virtual STATE getstate() = 0;
   */

  static int l_pin_set(lua_State *L)
  {
    auto device = VirtualContextManager::getInstance().getDevice();

    if (!lua_istable(L, -lua_gettop(L)))
    {
      LOG_DEBUG("No pin found\n");
    }
    lua_pushstring(L, TEXT_PIN_FIELD);
    lua_gettable(L, -lua_gettop(L));
    auto pinNumber = lua_tointeger(L, -1);
    lua_pop(L, 1);
    CHAR pinName[] = "Q";
    auto pinInstance = device->getPin(pinName);
    return 0;
  }

  static const luaL_Reg VsmPinMethodsLib[] = {
      {"set", l_pin_set},
      {NULL, NULL}};

  void VirtualDevice::registerPin(const char *name, int num)
  {
    luaL_newlib(luactx_, VsmPinMethodsLib);    
    lua_pushstring(luactx_, "pin");
    lua_pushinteger(luactx_, num);
    lua_setglobal(luactx_, name);
    lua_pop(luactx_, 1);

    /*
    lua_pushstring(luactx_, TEXT_HI_FIELD);
    lua_pushcfunction(luactx_, pin_set_hi);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_LO_FIELD);
    lua_pushcfunction(luactx_, pin_set_lo);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_FL_FIELD);
    lua_pushcfunction(luactx_, pin_set_fluactx_);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_TOGGLE_FIELD);
    lua_pushcfunction(luactx_, pin_toggle);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_SET_FIELD);
    lua_pushcfunction(luactx_, pin_set);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_GET_FIELD);
    lua_pushcfunction(luactx_, pin_get);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_HI_FIELD);
    lua_pushcfunction(luactx_, pin_is_hi);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_LO_FIELD);
    lua_pushcfunction(luactx_, pin_is_lo);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_FL_FIELD);
    lua_pushcfunction(luactx_, pin_is_fluactx_);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_EDGE_FIELD);
    lua_pushcfunction(luactx_, pin_is_edge);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_PEDGE_FIELD);
    lua_pushcfunction(luactx_, pin_is_pedge);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_NEDGE_FIELD);
    lua_pushcfunction(luactx_, pin_is_nedge);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_STEADY_FIELD);
    lua_pushcfunction(luactx_, pin_is_steady);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_ACTIVE_FIELD);
    lua_pushcfunction(luactx_, pin_is_active);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_INACTIVE_FIELD);
    lua_pushcfunction(luactx_, pin_is_inactive);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_IS_INVERTED_FIELD);
    lua_pushcfunction(luactx_, pin_is_inverted);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_SET_STATE_FIELD);
    lua_pushcfunction(luactx_, pin_set_state);
    lua_rawset(luactx_, -3);
    lua_pushstring(luactx_, TEXT_GET_STATE_FIELD);
    lua_pushcfunction(luactx_, pin_get_state);
    lua_rawset(luactx_, -3);
    */

    lua_setglobal(luactx_, name);
    lua_pop(luactx_, 1);
  }
}