/**
 *
 * @file   win32.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
  * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#include <vsmapi.h>

int32_t vasprintf ( char** sptr, char* fmt, va_list argv )
{
	int32_t wanted = vsnprintf ( *sptr = NULL, 0, fmt, argv );
	if ( ( wanted > 0 ) && ( ( *sptr = malloc ( 1 + wanted ) ) != NULL ) )
		return vsprintf ( *sptr, fmt, argv );
		
	return wanted;
}

int32_t asprintf ( char** sptr, char* fmt, ... )
{
	int32_t retval;
	va_list argv;
	va_start ( argv, fmt );
	retval = vasprintf ( sptr, fmt, argv );
	va_end ( argv );
	return retval;
}
