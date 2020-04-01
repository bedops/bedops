//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchBase64Coding.c
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

#ifdef __cplusplus
#include <cstdlib>
#include <cstdio>
#include <cstring>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include "data/starch/starchBase64Coding.h"

#ifdef __cplusplus
namespace starch {
#endif

const char kStarchBase64EncodingTable[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

const short kStarchBase64DecodingTable[256] = {
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -1, -1, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
    -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
    -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};

void 
STARCH_encodeBase64(char **output, const size_t outputLength, const unsigned char *inputBytes, const size_t inputLength) 
{
#ifdef __cplusplus
    unsigned char *inputPointer = const_cast<unsigned char *>( inputBytes );
#else
    unsigned char *inputPointer = (unsigned char *) inputBytes;
#endif
#ifdef __cplusplus
    char *objPointer = nullptr;
    char *strResult = nullptr;
#else
    char *objPointer = NULL;
    char *strResult = NULL;
#endif
    size_t bytesLength = inputLength;
    /* size_t resultLength = (((bytesLength + 2) / 3) * 4) + 1; */

    if (inputLength == 0) 
        return;

#ifdef DEBUG
    fwrite(inputPointer, sizeof(unsigned char), inputLength, stderr);
#endif

    // Setup the String-based Result placeholder and pointer within that placeholder
#ifdef __cplusplus
    strResult = static_cast<char *>( calloc(outputLength, sizeof(char)) );
#else
    strResult = calloc(outputLength, sizeof(char));
#endif
    if (!strResult) {
        fprintf(stderr, "ERROR: Could not allocate space for base64-encoded string\n");
        return;
    }
    objPointer = strResult;

    // Iterate through everything
    while (bytesLength > 2) { // keep going until we have less than 24 bits
        *objPointer++ = kStarchBase64EncodingTable[inputPointer[0] >> 2];
        *objPointer++ = kStarchBase64EncodingTable[((inputPointer[0] & 0x03) << 4) + (inputPointer[1] >> 4)];
        *objPointer++ = kStarchBase64EncodingTable[((inputPointer[1] & 0x0f) << 2) + (inputPointer[2] >> 6)];
        *objPointer++ = kStarchBase64EncodingTable[inputPointer[2] & 0x3f];

        // we just handled 3 octets (24 bits) of data
        inputPointer += 3;
        bytesLength -= 3; 
    }

    // now deal with the tail end of things
    if (bytesLength != 0) {
        *objPointer++ = kStarchBase64EncodingTable[inputPointer[0] >> 2];
        if (bytesLength > 1) {
            *objPointer++ = kStarchBase64EncodingTable[((inputPointer[0] & 0x03) << 4) + (inputPointer[1] >> 4)];
            *objPointer++ = kStarchBase64EncodingTable[(inputPointer[1] & 0x0f) << 2];
            *objPointer++ = '=';
        } else {
            *objPointer++ = kStarchBase64EncodingTable[(inputPointer[0] & 0x03) << 4];
            *objPointer++ = '=';
            *objPointer++ = '=';
        }
    }

    // Terminate the string-based result
    *objPointer = '\0';

#ifdef DEBUG
    fprintf(stderr, "\n");
    fprintf(stderr, "%s\n", strResult);
#endif

    *output = STARCH_strdup(strResult);

    return;
}

void 
STARCH_decodeBase64(char *input, unsigned char **output, size_t *outputLength) 
{
    const char *objPointer = input;
    size_t intLength = strlen(objPointer);
    int intCurrent;
    int i = 0, j = 0, k;
#ifdef __cplusplus
    unsigned char *objResult = static_cast<unsigned char *>( calloc(intLength, sizeof(unsigned char)) );
#else
    unsigned char *objResult = calloc(intLength, sizeof(unsigned char));
#endif

    // Run through the whole string, converting as we go
    while ( ((intCurrent = *objPointer++) != '\0') && (intLength-- > 0) ) {
        if (intCurrent == '=') {
            if (*objPointer != '=' && ((i % 4) == 1)) {// || (intLength > 0)) {
                // the padding character is invalid at this point -- so this entire string is invalid
                free(objResult);
                return;
            }
            continue;
        }

        intCurrent = kStarchBase64DecodingTable[intCurrent];
        if (intCurrent == -1) {
            // we're at a whitespace -- simply skip over
            continue;
        } else if (intCurrent == -2) {
            // we're at an invalid character
            free(objResult);
            return;
        }

#ifdef __cplusplus
        switch (i % 4) {
            case 0: {
                objResult[j] = static_cast<unsigned char>( intCurrent << 2 );
                break;
            }
            case 1: {
                objResult[j++] |= intCurrent >> 4;
                objResult[j] = static_cast<unsigned char>( (intCurrent & 0x0f) << 4 );
                break;
            }
            case 2: {
                objResult[j++] |= intCurrent >>2;
                objResult[j] = static_cast<unsigned char>( (intCurrent & 0x03) << 6 );
                break;
            }
            case 3: {
                objResult[j++] |= intCurrent;
                break;
            }
        }
#else
        switch (i % 4) {
            case 0: {
                objResult[j] = (unsigned char) (intCurrent << 2);
                break;
            }
            case 1: {
                objResult[j++] |= intCurrent >> 4;
                objResult[j] = (unsigned char) ((intCurrent & 0x0f) << 4);
                break;
            }
            case 2: {
                objResult[j++] |= intCurrent >>2;
                objResult[j] = (unsigned char) ((intCurrent & 0x03) << 6);
                break;
            }
            case 3: {
                objResult[j++] |= intCurrent;
                break;
            }
        }
#endif
        i++;
    }

    // mop things up if we ended on a boundary
    k = j;
    if (intCurrent == '=') {
        switch (i % 4) {
            case 1: {
                // Invalid state
                free(objResult);
                return;
            }
            case 2: {
                k++;
                objResult[k] = 0;
                break;
            }
            case 3: {
                objResult[k] = 0;
                break;
            }
        }
    }

    // Cleanup and setup the return bytes
#ifdef __cplusplus
    *outputLength = static_cast<size_t>( j );
#else
    *outputLength = (size_t) j;
#endif
    memcpy(*output, objResult, *outputLength);
    free(objResult);
}

#ifdef __cplusplus
} // namespace starch
#endif
