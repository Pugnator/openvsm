/**
 *
 * @file   lua_pin.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 * @brief Set of PIN object methods
 *
 */

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	void register_pin_obj ( lua_State* L, int num, char* name )
 *
 * \brief	Registers the pin object.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L   	If non-null, the lua_State to process.
 * \param	num				Number of.
 * \param [in,out]	name	If non-null, the name.
 **************************************************************************************************/

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

void register_pin_obj ( lua_State* L, int num, char* name )
{
	lua_newtable ( L );
	lua_pushstring ( L, TEXT_PIN_FIELD );
	lua_pushinteger ( L, num );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_HI_FIELD );
	lua_pushcfunction ( L, pin_set_hi );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_LO_FIELD );
	lua_pushcfunction ( L, pin_set_lo );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_FL_FIELD );
	lua_pushcfunction ( L, pin_set_fl );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_TOGGLE_FIELD );
	lua_pushcfunction ( L, pin_toggle );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_SET_FIELD );
	lua_pushcfunction ( L, pin_set );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_GET_FIELD );
	lua_pushcfunction ( L, pin_get );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_HI_FIELD );
	lua_pushcfunction ( L, pin_is_hi );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_LO_FIELD );
	lua_pushcfunction ( L, pin_is_lo );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_FL_FIELD );
	lua_pushcfunction ( L, pin_is_fl );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_EDGE_FIELD );
	lua_pushcfunction ( L, pin_is_edge );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_PEDGE_FIELD );
	lua_pushcfunction ( L, pin_is_pedge );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_NEDGE_FIELD );
	lua_pushcfunction ( L, pin_is_nedge );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_STEADY_FIELD );
	lua_pushcfunction ( L, pin_is_steady );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_ACTIVE_FIELD );
	lua_pushcfunction ( L, pin_is_active );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_INACTIVE_FIELD );
	lua_pushcfunction ( L, pin_is_inactive );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_IS_INVERTED_FIELD );
	lua_pushcfunction ( L, pin_is_inverted );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_SET_STATE_FIELD );
	lua_pushcfunction ( L, pin_set_state );
	lua_rawset ( L, -3 );
	lua_pushstring ( L, TEXT_GET_STATE_FIELD );
	lua_pushcfunction ( L, pin_get_state );
	lua_rawset ( L, -3 );
	
	lua_setglobal ( L, name );
	lua_pop ( L, 1 );
}

/**********************************************************************************************//**
 * \fn	int get_pin_self ( lua_State* L )
 *
 * \brief	Gets pin self.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	The pin self.
 **************************************************************************************************/

static int get_pin_self ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	if ( 0 == lua_istable ( L, - lua_gettop ( L ) ) ) //take the first element
	{
		print_error ( model, TEXT_NO_PIN_OBJECT_FOUND );
	}
	lua_pushstring ( L, TEXT_PIN_FIELD ); ///this fields contains pin index
	lua_gettable ( L, - lua_gettop ( L )  ); //take the first element
	return lua_tointeger ( L, -1 );
}

/**********************************************************************************************//**
 * \fn	int pin_set_hi ( lua_State* L )
 *
 * \brief	Pin set higher.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_set_hi ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	set_pin_state ( model, model->device_pins[pin_num], LOGIC_HI );
	return 0;
}

/**********************************************************************************************//**
 * \fn	int pin_set_lo ( lua_State* L )
 *
 * \brief	Pin set lower.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_set_lo ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	set_pin_state ( model, model->device_pins[pin_num], LOGIC_LO );
	return 0;
}

/**********************************************************************************************//**
 * \fn	int pin_set_fl ( lua_State* L )
 *
 * \brief	Pin set fl.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_set_fl ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	set_pin_state ( model, model->device_pins[pin_num], LOGIC_FLT );
	return 0;
}

/**********************************************************************************************//**
 * \fn	int pin_get ( lua_State* L )
 *
 * \brief	Pin get.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_get ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	int result = get_pin_bool ( model->device_pins[pin_num] );
	lua_pushinteger ( L, result );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_set ( lua_State* L )
 *
 * \brief	Pin set.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_set ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	int value = lua_tointeger ( L, -2 );
	set_pin_bool ( model, model->device_pins[pin_num], value );
	return 0;
}

/**********************************************************************************************//**
 * \fn	int pin_is_edge ( lua_State* L )
 *
 * \brief	Pin is edge.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_edge ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_edge ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_pedge ( lua_State* L )
 *
 * \brief	Pin is pedge.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_pedge ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_posedge ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_nedge ( lua_State* L )
 *
 * \brief	Pin is nedge.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_nedge ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_negedge ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_inverted ( lua_State* L )
 *
 * \brief	Pin is inverted.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_inverted ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_inverted ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_steady ( lua_State* L )
 *
 * \brief	Pin is steady.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_steady ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_steady ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_active ( lua_State* L )
 *
 * \brief	Pin is active.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_active ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_active ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_inactive ( lua_State* L )
 *
 * \brief	Pin is inactive.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_inactive ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_inactive ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_set_state ( lua_State* L )
 *
 * \brief	Pin set state.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_set_state ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	int pin_state = lua_tointeger ( L, -1 );
	set_pin_state ( model, model->device_pins[pin_num], pin_state );
	return 0;
}

/**********************************************************************************************//**
 * \fn	int pin_get_state ( lua_State* L )
 *
 * \brief	Pin get state.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_get_state ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, get_pin_state ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_hi ( lua_State* L )
 *
 * \brief	Pin is higher.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_hi ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_high ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_lo ( lua_State* L )
 *
 * \brief	Pin is lower.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_lo ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_low ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_is_fl ( lua_State* L )
 *
 * \brief	Pin is fl.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_is_fl ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	lua_pushinteger ( L, is_pin_floating ( model->device_pins[pin_num].pin ) );
	return 1;
}

/**********************************************************************************************//**
 * \fn	int pin_toggle ( lua_State* L )
 *
 * \brief	Pin toggle.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	An int.
 **************************************************************************************************/

static int pin_toggle ( lua_State* L )
{
	IDSIMMODEL* model = ( IDSIMMODEL* ) lua_get_model_obj ( L );
	int pin_num = get_pin_self ( L );
	int val = get_pin_bool ( model->device_pins[pin_num] );
	if ( val > 0 )
	{
		set_pin_bool ( model, model->device_pins[pin_num], 0 );
	}
	else
	{
		set_pin_bool ( model, model->device_pins[pin_num], 1 );
	}
	return 0;
}
