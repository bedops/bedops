/*
  FILE: BEDOPS.Constants.hpp
  AUTHOR: Shane Neph & Alex Reynolds
  CREATE DATE: Thu Sep 13 13:19:15 PDT 2012
*/

//
//    BEDOPS
//    Copyright (C) 2011-2017 Shane Neph, Scott Kuehn and Alex Reynolds
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
// Don't use these directly; they just synchronize C and C++'s uses below
//   - just want to utilize C++'s type system explicitly
//
// The minimum of running 'ulimit -s' on CentOS 7.2 and Mac OS X 10.12.4 shows
// a stack size limit of 8192 kB (8388608 bytes) and we adjust the TOKENS_MAX_LENGTH
// to this minimum value -- minus 4 MB, to leave some stack space for other parts of
// a running binary.
//
// This change is specifically so that sort-bed can sort very-long reads obtained from
// converting BAM to BED via convert2bed (e.g., Nanopore or PacBio sequencing reads), which
// easily blow up line sizes past what we've generally been used to with short-read sequencers.
// If read lengths get longer, we may need to explore some changes to sort-bed memory usage.
//

#include "utility/CompilerMath.hpp"

#ifndef REST_EXPONENT
#define REST_EXPONENT 15
#endif

#ifndef ID_EXPONENT
#define ID_EXPONENT 13
#endif

#ifndef CHROM_EXPONENT
#define CHROM_EXPONENT 7
#endif

#define BASE_VALUE 2
#define INT_TOKEN_CHR_MAX_LENGTH Ext::Pow<BASE_VALUE, CHROM_EXPONENT>::value-1
#define INT_TOKEN_ID_MAX_LENGTH Ext::Pow<BASE_VALUE, ID_EXPONENT>::value-1
#define INT_TOKEN_REST_MAX_LENGTH Ext::Pow<BASE_VALUE, REST_EXPONENT>::value-1
#define INT_MAX_DEC_INTEGERS 12
#define INT_MAX_COORD_VALUE 999999999999 /* MAX_DEC_INTEGERS decimal integers; we assume >= 64-bit systems */
#define INT_TOKENS_MAX_LENGTH (TOKEN_CHR_MAX_LENGTH + TOKEN_ID_MAX_LENGTH + TOKEN_REST_MAX_LENGTH + 2*MAX_DEC_INTEGERS)
#define INT_TOKENS_HEADER_MAX_LENGTH TOKENS_MAX_LENGTH
#define INT_TOKEN_CHR_FIELD_INDEX 0
#define INT_TOKEN_START_FIELD_INDEX 1
#define INT_TOKEN_STOP_FIELD_INDEX 2
#define INT_MEM_CHUNK_SZ 64 // how many BED elements allocated at a time

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
  typedef int LineLengthType;

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
