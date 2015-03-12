/**
 *
 * @file   bus.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 * @brief Set of PIN object methods
 *
 */

#include <vsm_api.h>

int lua_set_bus ( lua_State* L )
{
	///FIXME: add custom table-checking function, as Lua's lua_istable is a macro and can be used
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int byte = lua_tointeger ( L, -1 );
	if ( 0 == lua_istable ( L, -2 ) )
	{
		print_error ( this, "No bus specified" );
	}
	/* Iterate throug the bus table */
	lua_pushnil ( L );
	/* Pins should be added to bus as big-endian one (MSB pin first) */
	int bit_counter = 0;
	while ( 0 != lua_next ( L, 1 ) )
	{		
		print_info(this, "%s", lua_typename(L, 1));
		lua_pop(L, -1);
		bit_counter++;
	}
	if ( 0 == bit_counter )
	{
		print_warning ( this, "Bus specified contains no pins" );
	}
	return 0;
}

int lua_get_bus ( lua_State* L )
{
	///FIXME: add custom table-checking function, as Lua's lua_istable is a macro and can be used
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	if ( 0 == lua_istable ( L, 1 ) )
	{
		print_error ( this, "No bus specified" );
	}
	/* Iterate throug the bus table */
	lua_pushnil ( L );
	/* Pins should be added to bus as big-endian one (MSB pin first) */
	int bit_counter = 0;
	/* Add bus size check here. 32 or 64bit bus max */
	int data = 0;
	while ( 0 != lua_next ( L, 1 ) )
	{
		int pin = get_pin_self(L);
		int bit = get_pin_bool ( this->device_pins[pin] );
		int state = get_pin_state ( this->device_pins[pin].pin );
		if ( -1 ==  bit )
		{
			return 0;
		}
		
		if ( bit )
		{
			data |= ( 1 << bit_counter );
		}
		else
		{
			data &= ~ ( 1 << bit_counter );
		}
		lua_pop ( L, 1 );
		bit_counter++;
	}
	lua_pushinteger ( L, data );
	if ( 0 == bit_counter )
	{
		print_warning ( this, "Bus specified contains no pins" );
	}
	return 1;
}
