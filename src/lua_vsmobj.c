/**
 *
 * @file   vsmobj.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 *
 */

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	void* lua_get_model_obj ( lua_State* L )
 *
 * \brief	Lua get model object.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	L	If non-null, the lua_State to process.
 *
 * \return	null if it fails, else a void*.
 **************************************************************************************************/

void* lua_get_model_obj ( lua_State* L )
{
	lua_pushliteral ( L, "__this" );
	lua_gettable ( L, LUA_REGISTRYINDEX );
	void* this = lua_touserdata ( L, -1 );
	lua_pop ( L, 1 );
	return this;
}
