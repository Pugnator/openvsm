/**
 *
 * @file   vsmobj.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#include <vsmapi.h>

/**
 * @brief extracts model object from Lua state
 *
 * @param L Lua state
 * @return model object
 */
void* lua_get_model_obj ( lua_State* L )
{
	lua_pushliteral ( L, "__this" );
	lua_gettable ( L, LUA_REGISTRYINDEX );
	void* this = lua_touserdata ( L, -1 );
	lua_pop ( L, 1 );
	return this;
}
