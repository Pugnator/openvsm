/**
 *
 * @file   pin.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#include <vsm_api.h>

int get_pin_self (lua_State* L)
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	if(0 == lua_istable(L, -1))
	{
		print_error(model, "No pin object found");
	}
	lua_pushstring(L, "pin");
    lua_gettable(L, -2);
	return lua_tointeger ( L, -1 );
}

int
pin_set_hi ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self(L);
	set_pin_bool ( model, model->device_pins[pin_num], LOGIC_HI );
	return 0;
}
