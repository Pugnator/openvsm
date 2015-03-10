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
#include <vsm_api.h>

int pin_set_hi ( lua_State* L );
int pin_set_lo ( lua_State* L );
int pin_set_fl ( lua_State* L );
