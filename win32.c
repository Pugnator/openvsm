/**
 *
 * @file   win32.c
 * @Author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   27.06.2014
 * @brief  wrappers for non-posix functions absent in mingw, from Cygwin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <vsm_api.h>

int32_t vasprintf( char **sptr, char *fmt, va_list argv )
{
	int32_t wanted = vsnprintf( *sptr = NULL, 0, fmt, argv );
	if( (wanted > 0) && ((*sptr = malloc( 1 + wanted )) != NULL) )
		return vsprintf( *sptr, fmt, argv );

	return wanted;
}

int32_t asprintf( char **sptr, char *fmt, ... )
{
	int32_t retval;
	va_list argv;
	va_start( argv, fmt );
	retval = vasprintf( sptr, fmt, argv );
	va_end( argv );
	return retval;
}
