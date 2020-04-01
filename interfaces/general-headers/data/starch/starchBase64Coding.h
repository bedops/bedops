//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchBase64Coding.h
//=========

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

#ifndef STARCH_BASE64CODING_H
#define STARCH_BASE64CODING_H

#include "data/starch/starchHelpers.h"

#ifdef __cplusplus
namespace starch {
#endif

extern const char kStarchBase64EncodingTable[64];
extern const short kStarchBase64DecodingTable[256];

void      STARCH_encodeBase64(char **output, 
                      const size_t outputLength, 
               const unsigned char *inputBytes, 
                      const size_t inputLength);

void      STARCH_decodeBase64(char *input, 
                     unsigned char **output, 
                            size_t *outputLength);
#ifdef __cplusplus
} // namespace starch
#endif

#endif
