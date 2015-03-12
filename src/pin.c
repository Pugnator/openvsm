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

int
pin_is_edge ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_edge ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_pedge ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_posedge ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_nedge ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_negedge ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_inverted ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_inverted ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_steady ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_steady ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_active ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_active ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_inactive ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_inactive ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_set_state ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );		
	int pin_state = lua_tointeger ( L, -1 );
	set_pin_state ( model, model->device_pins[pin_num], pin_state );
	return 0;
}

int
pin_get_state ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, get_pin_state ( model->device_pins[pin_num].pin ) );	
	return 1;
}

int
pin_is_hi ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_high ( model->device_pins[pin_num].pin ) );
	return 1;
}

int
pin_is_lo ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_low ( model->device_pins[pin_num].pin ) );	
	return 1;
}

int
pin_is_fl ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );	
	int pin_num = get_pin_self ( L );	
	lua_pushinteger ( L, is_pin_floating ( model->device_pins[pin_num].pin ) );
	return 1;
}
