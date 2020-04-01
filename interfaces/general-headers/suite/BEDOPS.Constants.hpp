/*
  FILE: BEDOPS.Constants.hpp
  AUTHOR: Shane Neph & Alex Reynolds
  CREATE DATE: Thu Sep 13 13:19:15 PDT 2012
*/

//
//    BEDOPS
//    Copyright (C) 2011-2020 Shane Neph, Scott Kuehn and Alex Reynolds
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef CONSTANTS_BEDOPS_H
#define CONSTANTS_BEDOPS_H

//
// Typically don't use these directly in an application
//   (further down are comments on stuff to use)
//

#ifdef __cplusplus

#include "utility/CompilerMath.hpp"

#define BASE_VALUE 2
#define INT_TOKEN_CHR_MAX_LENGTH Ext::Pow<BASE_VALUE, CHROM_EXPONENT>::value-1
#define INT_TOKEN_ID_MAX_LENGTH Ext::Pow<BASE_VALUE, ID_EXPONENT>::value-1
#define INT_TOKEN_REST_MAX_LENGTH Ext::Pow<BASE_VALUE, REST_EXPONENT>::value-1

#else

#define pwrtwo(x) (1UL << (x))
#define INT_TOKEN_CHR_MAX_LENGTH pwrtwo(CHROM_EXPONENT)-1
#define INT_TOKEN_ID_MAX_LENGTH pwrtwo(ID_EXPONENT)-1
#define INT_TOKEN_REST_MAX_LENGTH pwrtwo(REST_EXPONENT)-1

#endif

#undef NORMAL_REST
#undef NORMAL_ID
#undef NORMAL_CHR
#define SPECIALFLOAT_BUILD 1

#ifndef REST_EXPONENT
#define REST_EXPONENT 15
#define NORMAL_REST 1
#endif

#ifndef ID_EXPONENT
#define ID_EXPONENT 13
#define NORMAL_ID 1
#endif

#ifndef CHROM_EXPONENT
#define CHROM_EXPONENT 7
#define NORMAL_CHR 1
#endif

#ifndef MEASURE_TYPE /* could be long double for 128 bits, for example */
#define MEASURE_TYPE double
#undef SPECIALFLOAT_BUILD
#endif

#if !defined NORMAL_REST || !defined NORMAL_ID || !defined NORMAL_CHR
#define MEGASIZE_BUILD 1
#endif

#if defined MEGASIZE_BUILD
#if defined SPECIALFLOAT_BUILD
#define BUILD_OPTS "(megarow, quadruple precision float)"
#endif
#define BUILD_OPTS "(megarow)"
#elif defined SPECIALFLOAT_BUILD
#define BUILD_OPTS "(typical, quadruple precision float)"
#else
#define BUILD_OPTS "(typical)"
#endif

#define INT_MAX_DEC_INTEGERS 12L
#define INT_MAX_COORD_VALUE 999999999999 /* INT_MAX_DEC_INTEGERS decimal integers; we assume >= 64-bit systems */
#define INT_TOKENS_MAX_LENGTH (INT_TOKEN_CHR_MAX_LENGTH + INT_TOKEN_ID_MAX_LENGTH + INT_TOKEN_REST_MAX_LENGTH + 2*INT_MAX_DEC_INTEGERS)
#define INT_TOKENS_HEADER_MAX_LENGTH INT_TOKENS_MAX_LENGTH
#define INT_TOKEN_CHR_FIELD_INDEX 0
#define INT_TOKEN_START_FIELD_INDEX 1
#define INT_TOKEN_STOP_FIELD_INDEX 2
#define INT_MEM_CHUNK_SZ 64 // how many BED elements allocated at a time


//
// The constants and typedefs to use in applications are defined below
//

#ifdef __cplusplus

#include <cinttypes>
#include <cstddef>

namespace Bed {
  // Use these typedef's in applications
  typedef uint64_t CoordType;
  typedef int64_t SignedCoordType;
  typedef CoordType LineCountType;
  typedef CoordType BaseCountType;
  typedef unsigned long LineLengthType;
  typedef MEASURE_TYPE MeasurementType;

  static_assert(sizeof(SignedCoordType) >= sizeof(INT_MAX_COORD_VALUE), "INT_MAX_COORD_VALUE is too big!"); // expected-warning {{static_assert declarations are incompatible with C++98}}

  constexpr LineLengthType  TOKEN_CHR_MAX_LENGTH     = INT_TOKEN_CHR_MAX_LENGTH;
  constexpr LineLengthType  TOKEN_ID_MAX_LENGTH      = INT_TOKEN_ID_MAX_LENGTH;
  constexpr LineLengthType  TOKEN_REST_MAX_LENGTH    = INT_TOKEN_REST_MAX_LENGTH;
  constexpr unsigned long   MAX_DEC_INTEGERS         = INT_MAX_DEC_INTEGERS;
  constexpr SignedCoordType MAX_COORD_VALUE          = INT_MAX_COORD_VALUE;
  constexpr LineLengthType  TOKENS_MAX_LENGTH        = INT_TOKENS_MAX_LENGTH;
  constexpr LineLengthType  TOKENS_HEADER_MAX_LENGTH = INT_TOKENS_HEADER_MAX_LENGTH;
  constexpr unsigned long   TOKEN_CHR_FIELD_INDEX    = INT_TOKEN_CHR_FIELD_INDEX;
  constexpr unsigned long   TOKEN_START_FIELD_INDEX  = INT_TOKEN_START_FIELD_INDEX;
  constexpr unsigned long   TOKEN_STOP_FIELD_INDEX   = INT_TOKEN_STOP_FIELD_INDEX;
  constexpr std::size_t     CHUNKSZ                  = INT_MEM_CHUNK_SZ;
} // namespace Bed
    
#else
    
#include <inttypes.h>
    
  // Use these typedef's in applications
  typedef uint64_t CoordType;
  typedef int64_t SignedCoordType;
  typedef CoordType LineCountType;
  typedef CoordType BaseCountType;
  typedef unsigned long LineLengthType;
  typedef MEASURE_TYPE MeasurementType;

#define TOKEN_CHR_MAX_LENGTH INT_TOKEN_CHR_MAX_LENGTH
#define TOKEN_ID_MAX_LENGTH INT_TOKEN_ID_MAX_LENGTH
#define TOKEN_REST_MAX_LENGTH INT_TOKEN_REST_MAX_LENGTH
#define MAX_DEC_INTEGERS INT_MAX_DEC_INTEGERS
#define MAX_COORD_VALUE INT_MAX_COORD_VALUE
#define TOKENS_MAX_LENGTH INT_TOKENS_MAX_LENGTH
#define TOKENS_HEADER_MAX_LENGTH INT_TOKENS_HEADER_MAX_LENGTH
#define TOKEN_CHR_FIELD_INDEX INT_TOKEN_CHR_FIELD_INDEX
#define TOKEN_START_FIELD_INDEX INT_TOKEN_START_FIELD_INDEX
#define TOKEN_STOP_FIELD_INDEX INT_TOKEN_STOP_FIELD_INDEX
#define CHUNKSZ INT_MEM_CHUNK_SZ

#endif

#endif // CONSTANTS_BEDOPS_H
