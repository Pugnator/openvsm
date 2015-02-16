#ifndef LUA_BIND_H
#define LUA_BIND_H
#include <vsm_api.h>

typedef struct lua_bind_func
{
	int32_t ( *lua_c_api ) ( lua_State* );
	const char* lua_func_name;
} lua_bind_func;

typedef struct lua_bind_var
{
	const char* var_name;
	int64_t var_value;
} lua_bind_var;

#define PIN_NAME "name"
#define PIN_NUM "number"
#define PIN_OFF_TIME "off_time"
#define PIN_ON_TIME "on_time"

bool load_device_script ( IDSIMMODEL* model, const char* function );
void register_functions ( IDSIMMODEL* model, lua_State* L );

#endif
