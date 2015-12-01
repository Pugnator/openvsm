/**
 *
 * @file   utils.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 * @brief Set of PIN object methods
 *
 */

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	unsigned rand256()
 *
 * \brief	Random 256.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \return	An unsigned.
 **************************************************************************************************/

static uint32_t
rand256()
{
	static uint32_t const limit = RAND_MAX - RAND_MAX % 256;
	uint32_t result = rand();
	while (result >= limit) 
	{
		result = rand();
	}
	return result % 256;
}

/**********************************************************************************************//**
 * \fn	unsigned long long rand64bits()
 *
 * \brief	Random 64bits.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \return	A long.
 **************************************************************************************************/

uint64_t
rand64bits()
{
	uint64_t results = 0ULL;
	for (int count = 8; count > 0; --count) 
	{
		results = 256U * results + rand256();
	}
	return results;
}

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

uint64_t 
xorshift ( IDSIMMODEL* this )
{
	uint64_t x = this->xorseed[0];
	uint64_t const y = this->xorseed[1];
	this->xorseed[0] = y;
	x ^= x << 23; // a
	x ^= x >> 17; // b
	x ^= y ^ ( y >> 26 ); // c //-V537
	this->xorseed[1] = x;
	return x + y;
}
