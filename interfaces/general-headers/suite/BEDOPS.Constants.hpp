/*
  FILE: BEDOPS.Constants.hpp
  AUTHOR: Shane Neph & Alex Reynolds
  CREATE DATE: Thu Sep 13 13:19:15 PDT 2012
  PROJECT: BEDOPS suite
  ID: $Id:$
*/

// Macro Guard
#ifndef CONSTANTS_BEDOPS_H
#define CONSTANTS_BEDOPS_H


// Don't use these directly; they just synchronize C and C++'s uses below
//   - just want to utilize C++'s type system explicitly
#define INT_TOKEN_CHR_MAX_LENGTH 127
#define INT_TOKEN_ID_MAX_LENGTH 16383
#define INT_TOKEN_REST_MAX_LENGTH 131072
#define INT_MAX_DEC_INTEGERS 13
#define INT_MAX_COORD_VALUE 100000000000 /* MAX_DEC_INTEGERS decimal integers; we assume >= 64-bit systems */
#define INT_TOKENS_MAX_LENGTH (TOKEN_CHR_MAX_LENGTH + TOKEN_ID_MAX_LENGTH + TOKEN_REST_MAX_LENGTH + 2*MAX_DEC_INTEGERS)
#define INT_TOKENS_HEADER_MAX_LENGTH TOKENS_MAX_LENGTH
#define INT_TOKEN_CHR_FIELD_INDEX 0
#define INT_TOKEN_START_FIELD_INDEX 1
#define INT_TOKEN_STOP_FIELD_INDEX 2


#ifdef __cplusplus

#include <cinttypes>
namespace Bed {
// Use these typedef's in applications
typedef uint64_t CoordType;
typedef int64_t SignedCoordType;
typedef CoordType LineCountType;
typedef CoordType BaseCountType;

static_assert(sizeof(SignedCoordType) >= sizeof(INT_MAX_COORD_VALUE), "INT_MAX_COORD_VALUE is too big!");

static const unsigned long   TOKEN_CHR_MAX_LENGTH     = INT_TOKEN_CHR_MAX_LENGTH;
static const unsigned long   TOKEN_ID_MAX_LENGTH      = INT_TOKEN_ID_MAX_LENGTH;
static const unsigned long   TOKEN_REST_MAX_LENGTH    = INT_TOKEN_REST_MAX_LENGTH;
static const unsigned long   MAX_DEC_INTEGERS         = INT_MAX_DEC_INTEGERS;
static const SignedCoordType MAX_COORD_VALUE          = INT_MAX_COORD_VALUE;
static const unsigned long   TOKENS_MAX_LENGTH        = INT_TOKENS_MAX_LENGTH;
static const unsigned long   TOKENS_HEADER_MAX_LENGTH = INT_TOKENS_HEADER_MAX_LENGTH;
static const unsigned long   TOKEN_CHR_FIELD_INDEX    = INT_TOKEN_CHR_FIELD_INDEX;
static const unsigned long   TOKEN_START_FIELD_INDEX  = INT_TOKEN_START_FIELD_INDEX;
static const unsigned long   TOKEN_STOP_FIELD_INDEX   = INT_TOKEN_STOP_FIELD_INDEX;

#else

#include <inttypes.h>

// Use these typedef's in applications
typedef uint64_t CoordType;
typedef int64_t SignedCoordType;
typedef CoordType LineCountType;
typedef CoordType BaseCountType;

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

#endif

#ifdef __cplusplus
} // namespace Bed
#endif

#endif // CONSTANTS_BEDOPS_H
