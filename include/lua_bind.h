#ifndef LUA_BIND_H
#define LUA_BIND_H
#include <vsm_api.h>

#define PIN_NAME "name"
#define PIN_NUM "number"
#define PIN_ON_TIME "on_time"
#define PIN_OFF_TIME "off_time"

void register_functions (lua_State *L);
void lua_run_function (const char *func_name);
void lua_load_script (const char *function);

extern lua_State *luactx;
#endif