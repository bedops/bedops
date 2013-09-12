//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchBase64Coding.h
//=========

#ifndef STARCH_BASE64CODING_H
#define STARCH_BASE64CODING_H

extern const char kStarchBase64EncodingTable[64];
extern const short kStarchBase64DecodingTable[256];

void      STARCH_encodeBase64(char **output, 
                      const size_t outputLength, 
               const unsigned char *inputBytes, 
                      const size_t inputLength);

void      STARCH_decodeBase64(char *input, 
                     unsigned char **output, 
                            size_t *outputLength);

#endif
