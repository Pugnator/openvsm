/**
 *
 * @file   wincompat.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 *
 */

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	int32_t vasprintf ( char** sptr, char* fmt, va_list argv )
 *
 * \brief	Vasprintfs.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	sptr	If non-null, the sptr.
 * \param [in,out]	fmt 	If non-null, describes the format to use.
 * \param	argv			The argv.
 *
 * \return	An int32_t.
 **************************************************************************************************/

int32_t vasprintf ( char** sptr, char* fmt, va_list argv )
{
	int32_t wanted = vsnprintf ( *sptr = NULL, 0, fmt, argv );
	if ( ( wanted > 0 ) && ( ( *sptr = malloc ( 1 + wanted ) ) != NULL ) )
		return vsprintf ( *sptr, fmt, argv );
		
	return wanted;
}

/**********************************************************************************************//**
 * \fn	int32_t asprintf ( char** sptr, char* fmt, ... )
 *
 * \brief	Asprintfs.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	sptr	If non-null, the sptr.
 * \param [in,out]	fmt 	If non-null, describes the format to use.
 * \param	...				Variable arguments providing additional information.
 *
 * \return	An int32_t.
 **************************************************************************************************/

int32_t asprintf ( char** sptr, char* fmt, ... )
{
	int32_t retval;
	va_list argv;
	va_start ( argv, fmt );
	retval = vasprintf ( sptr, fmt, argv );
	va_end ( argv );
	return retval;
}
