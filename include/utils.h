/**
 *
 * @file   utils.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 * @brief Set of PIN object methods
 *
 */

#pragma once
#include <vsmapi.h>

uint64_t rand64bits();
uint64_t xorshift ( IDSIMMODEL* this );
