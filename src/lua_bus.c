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

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	int lua_set_bus ( lua_State* L )
 *
 * \brief	Lua set bus.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

int lua_set_bus ( lua_State* L )
{
	///FIXME: add custom table-checking function, as Lua's lua_istable is a macro and can be used
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int byte = lua_tointeger ( L, -1 );
	lua_pop ( this->luactx, 1 ); //Pop out the byte from the stack
	
	if ( 0 == lua_istable ( L, -1 ) )
	{
		print_error ( this, TEXT_NO_BUS_FOUND );
		return 0;
	}
	lua_len ( this->luactx, -1 );
	int32_t pin_number = lua_tointeger ( this->luactx, -1 );
	if ( 0 == pin_number )
	{
		WARNING ( this, TEXT_BUS_HAS_NO_PINS );
		return 0;
	}
#ifdef __DEBUG
	else
	{
		int maxval = pow ( 2, pin_number );
		if ( byte >= maxval )
		{
			WARNING ( this, TEXT_VALUE_WONT_FIT_BUS, byte, maxval - 1 );
		}
	}
#endif
	lua_pop ( this->luactx, 1 ); // Pop out pin_number from the stack
	int bit_counter = 0;
	for ( int i=1; i<=pin_number; i++ )
	{
		lua_rawgeti ( this->luactx,-1, i );
		lua_getfield ( this->luactx,-1, TEXT_PIN_FIELD );
		int pin = lua_tointeger ( this->luactx, -1 );
		set_pin_bool ( this, this->device_pins[pin], byte >> bit_counter & 1 );
		lua_pop ( this->luactx, 1 ); //Pop the value
		lua_pop ( this->luactx, 1 ); //Pop the pin object
		bit_counter++;
	}
	return 0;
}

/**********************************************************************************************//**
 * \fn	int lua_get_bus ( lua_State* L )
 *
 * \brief	Lua get bus.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

int lua_get_bus ( lua_State* L )
{
	///FIXME: add custom table-checking function, as Lua's lua_istable is a macro and can be used
	IDSIMMODEL* this = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	if ( 0 == lua_istable ( L, 1 ) )
	{
		print_error ( this, TEXT_NO_BUS_FOUND );
	}
	
	int bit_counter = 0;
	/**TODO: Add bus size check here. 32 or 64bit bus max */
	int data = 0;
	lua_len ( this->luactx, -1 );
	int32_t pin_number = lua_tointeger ( this->luactx, -1 );
	if ( 0 == pin_number )
	{
		WARNING ( this, TEXT_BUS_HAS_NO_PINS );
		return 0;
	}
	lua_pop ( this->luactx, 1 ); // Pop out pin_number from the stack
	bool has_floats = false;
	for ( int i=1; i<=pin_number; i++ )
	{
		lua_rawgeti ( this->luactx,-1, i );
		lua_getfield ( this->luactx,-1, TEXT_PIN_FIELD );
		int pin = lua_tointeger ( this->luactx, -1 );
		int bit = get_pin_bool ( this->device_pins[pin] );
		lua_pop ( this->luactx, 1 ); //Pop the value
		lua_pop ( this->luactx, 1 ); //Pop the pin object
		if ( -1 ==  bit )
		{
			//If pin is floating - set it to random value
			bit = xorshift ( this ) & 1;
			has_floats = true;
		}
		
		if ( bit )
		{
			data |= ( 1 << bit_counter );
		}
		else
		{
			data &= ~ ( 1 << bit_counter );
		}
		bit_counter++;
	}
#ifdef __DEBUG
	if ( has_floats )
	{
		WARNING ( this, "Bus has floating pins" );
	}
#endif
	lua_pushinteger ( L, data );
	return 1;
}
