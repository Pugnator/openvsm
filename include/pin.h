/**
 *
 * @file   pin.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#pragma once

void register_pin_obj ( lua_State* L, int num, char* name );
static int pin_set_hi(lua_State* L);
static int pin_set_lo(lua_State* L);
static int pin_set_fl(lua_State* L);
static int pin_toggle(lua_State* L);
static int pin_set(lua_State* L);
static int pin_get(lua_State* L);
static int pin_is_hi(lua_State* L);
static int pin_is_lo(lua_State* L);
static int pin_is_fl(lua_State* L);
static int pin_is_edge(lua_State* L);
static int pin_is_pedge(lua_State* L);
static int pin_is_nedge(lua_State* L);
static int pin_is_steady(lua_State* L);
static int pin_is_active(lua_State* L);
static int pin_is_inactive(lua_State* L);
static int pin_is_inverted(lua_State* L);
static int pin_set_state(lua_State* L);
static int pin_get_state(lua_State* L);
