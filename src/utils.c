/**
 *
 * @file   utils.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 * @brief Set of PIN object methods
 *
 */

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	uint64_t xorshift ( IDSIMMODEL* this )
 *
 * \brief	Xorshifts the given this.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	this	If non-null, this.
 *
 * \return	An uint64_t.
 **************************************************************************************************/

uint64_t xorshift ( IDSIMMODEL* this )
{
	uint64_t x = this->xorseed[0];
	uint64_t const y = this->xorseed[1];
	this->xorseed[0] = y;
	x ^= x << 23; // a
	x ^= x >> 17; // b
	x ^= y ^ ( y >> 26 ); // c
	this->xorseed[1] = x;
	return x + y;
}
