/**
 *
 * @file   pin.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 * @brief Set of PIN object methods
 *
 */

#include <vsm_api.h>

/**
 * @brief [extract pin number from object]
 * @details [This must be called from pin's methods in order to get pin index]
 *
 * @param L [Lua state]
 * @return [pin number index in device_pins array]
 */
int 
get_pin_self ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	if ( 0 == lua_istable ( L, - lua_gettop(L) ) ) //take the first element
	{
		print_error ( model, TEXT_NO_PIN_OBJECT_FOUND );
	}
	lua_pushstring ( L, TEXT_PIN_FIELD ); ///this fields contains pin index
	lua_gettable ( L, - lua_gettop(L)  ); //take the first element	
	return lua_tointeger ( L, -1 );
}

int
pin_set_hi ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	set_pin_state ( model, model->device_pins[pin_num], LOGIC_HI );
	return 0;
}

int
pin_set_lo ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	set_pin_state ( model, model->device_pins[pin_num], LOGIC_LO );
	return 0;
}

int
pin_set_fl ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	set_pin_state ( model, model->device_pins[pin_num], LOGIC_FLT );
	return 0;
}

int
pin_get ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	int result = get_pin_bool ( model->device_pins[pin_num] );
	print_info(model, "%s state %d", model->device_pins[pin_num].name, result);
	lua_pushinteger(L, result);
	return 1;
}

int
pin_set ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );	
	int value = lua_tointeger ( L, -1);
	set_pin_bool ( model, model->device_pins[pin_num], value );
	return 0;
}
