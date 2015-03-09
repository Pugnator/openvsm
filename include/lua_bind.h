/**
 *
 * @file   lua_bind.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#ifndef LUA_BIND_H
#define LUA_BIND_H
#include <vsm_api.h>

typedef struct lua_bind_func
{
	int ( *lua_c_api ) ( lua_State* );
	const char* lua_func_name;
	int ( *args[16] ) ( lua_State*, int index );
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

static int lua_state_to_string ( lua_State* L );
static int lua_set_pin_state ( lua_State* L );
static int lua_set_pin_bool ( lua_State* L );
static int lua_get_pin_bool ( lua_State* L );
static int lua_get_pin_state ( lua_State* L );
static int lua_is_pin_active ( lua_State* L );
static int lua_is_pin_edge ( lua_State* L );
static int lua_is_pin_posedge ( lua_State* L );
static int lua_is_pin_negedge ( lua_State* L );
static int lua_is_pin_low ( lua_State* L );
static int lua_is_pin_high ( lua_State* L );
static int lua_is_pin_floating ( lua_State* L );
static int lua_is_pin_steady ( lua_State* L );
static int lua_is_pin_inverted ( lua_State* L );
static int lua_toggle_pin_state ( lua_State* L );
static int lua_print_info ( lua_State* L );
static int lua_print_message ( lua_State* L );
static int lua_print_warning ( lua_State* L );
static int lua_print_error ( lua_State* L );
static int lua_set_callback ( lua_State* L );
static int lua_create_debug_popup ( lua_State* L );
static int lua_print_to_debug_popup ( lua_State* L );
static int lua_dump_to_debug_popup ( lua_State* L );
static int lua_create_memory_popup ( lua_State* L );
static int lua_create_source_popup ( lua_State* L );
static int lua_create_status_popup ( lua_State* L );
static int lua_create_var_popup ( lua_State* L );
static int lua_delete_popup ( lua_State* L );
static int lua_set_memory_popup ( lua_State* L );
static int lua_repaint_memory_popup ( lua_State* L );
static int lua_get_string_param ( lua_State* L );
static int lua_get_bool_param ( lua_State* L );
static int lua_get_num_param ( lua_State* L );
static int lua_get_hex_param ( lua_State* L );
static int lua_get_init_param ( lua_State* L );
static int lua_add_source_file ( lua_State* L );
static void* lua_get_model_obj ( lua_State* L );

/* Stdlib */
static int lua_set_bus ( lua_State* L );
static int lua_get_bus ( lua_State* L );

/* Bit twiddling */
static int lua_get_bit ( lua_State* L );
static int lua_set_bit ( lua_State* L );
static int lua_toggle_bit ( lua_State* L );
static int lua_clear_bit ( lua_State* L );

static int lua_get_systime ( lua_State* L );

#endif
