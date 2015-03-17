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
#include <vsmapi.h>

void register_pin_obj ( lua_State* L, int num, char* name );
int get_pin_self ( lua_State* L );
int pin_set_hi ( lua_State* L );
int pin_set_lo ( lua_State* L );
int pin_set_fl ( lua_State* L );
int pin_set ( lua_State* L );
int pin_get ( lua_State* L );
