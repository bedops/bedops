//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    unstarchHelpers.c
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
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>
#else
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#endif

#include <zlib.h>

#include "data/starch/starchSha1Digest.h"
#include "data/starch/starchBase64Coding.h"
#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchHelpers.h"
#include "data/starch/unstarchHelpers.h"
#include "data/starch/starchConstants.h"
#include "suite/BEDOPS.Constants.hpp"

#ifdef __cplusplus
namespace starch {
#endif

int 
UNSTARCH_extractDataWithGzip(FILE **inFp, FILE *outFp, const char *whichChr, const Metadata *md, const uint64_t mdOffset, const Boolean headerFlag) 
{
#ifdef DEBUG_VERBOSE
    fprintf(stderr, "\n--- UNSTARCH_extractDataWithGzip() ---\n");
#endif
#ifdef __cplusplus
    char* firstInputToken = nullptr;
    char* secondInputToken = nullptr;
#else
    char* firstInputToken = NULL;
    char* secondInputToken = NULL;
#endif
    const Metadata* iter;
    char* chromosome;
    uint64_t size;	
    uint64_t cumulativeSize = 0;
    SignedCoordType start, pLength, lastEnd;
    char const* all = "all";
    unsigned char zInBuf[STARCH_Z_CHUNK];
    unsigned char zOutBuf[STARCH_Z_CHUNK];
    unsigned char zLineBuf[STARCH_Z_CHUNK];
#ifdef __cplusplus
    unsigned char *zRemainderBuf = static_cast<unsigned char *>( malloc(1) ); 
#else
    unsigned char *zRemainderBuf = malloc(1); 
#endif
    z_stream zStream;
    unsigned int zHave, zOutBufIdx;
    size_t zBufIdx, zBufOffset;
    int zError;
    Boolean chrFound = kStarchFalse;

    if (!outFp)
        outFp = stdout;

#ifdef __cplusplus
    firstInputToken = static_cast<char *>( malloc(UNSTARCH_FIRST_TOKEN_MAX_LENGTH) );
#else
    firstInputToken = malloc(UNSTARCH_FIRST_TOKEN_MAX_LENGTH);
#endif
    if (!firstInputToken) {
        fprintf(stderr, "ERROR: (UNSTARCH_extractDataWithGzip) Could not allocate space for first input token\n");
        return UNSTARCH_FATAL_ERROR;
    }
    firstInputToken[0] = '\0';

#ifdef __cplusplus
    secondInputToken = static_cast<char *>( malloc(UNSTARCH_SECOND_TOKEN_MAX_LENGTH) );
#else
    secondInputToken = malloc(UNSTARCH_SECOND_TOKEN_MAX_LENGTH);
#endif
    if (!secondInputToken) {
        fprintf(stderr, "ERROR: (UNSTARCH_extractDataWithGzip) Could not allocate space for second input token\n");
        return UNSTARCH_FATAL_ERROR;
    }
    secondInputToken[0] = '\0';

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        chromosome = iter->chromosome; 
        size = iter->size;
        start = 0;
        pLength = 0;
        lastEnd = 0;

#ifdef __cplusplus
        if (STARCH_fseeko(*inFp, static_cast<off_t>( cumulativeSize + mdOffset ), SEEK_SET) != 0) {
#else
        if (STARCH_fseeko(*inFp, (off_t)(cumulativeSize + mdOffset), SEEK_SET) != 0) {
#endif
            fprintf(stderr, "ERROR: Could not seek data in archive at chromosome (%s) and offset (%" PRIu64 ")\n", chromosome, cumulativeSize + mdOffset);
            return UNSTARCH_FATAL_ERROR;
        }
        cumulativeSize += size;

        if ((strcmp(whichChr, all) == 0) || (strcmp(whichChr, chromosome) == 0)) {

            /* we have found at least one chromosome */
            chrFound = kStarchTrue;

            /* initialized and open gzip stream */
#ifdef __cplusplus
            zStream.zalloc = nullptr;
            zStream.zfree = nullptr;
            zStream.opaque = nullptr;
            zStream.next_in = nullptr;
#else
            zStream.zalloc = Z_NULL;
            zStream.zfree = Z_NULL;
            zStream.opaque = Z_NULL;
            zStream.next_in = Z_NULL;
#endif
            zStream.avail_in = 0;

            zError = inflateInit2(&zStream, (15+32)); /* cf. http://www.zlib.net/manual.html */
            if (zError != Z_OK) {
                fprintf(stderr, "ERROR: Could not initialize z-stream\n");
                return UNSTARCH_FATAL_ERROR;
            }

            *zRemainderBuf = '\0';
    
            /* while stream is open, read line from stream, and reverse transform */
            do {
#ifdef DEBUG_VERBOSE
                fprintf(stderr, "--> (pre-read-chunk)  currently at byte [%013ld]\n", ftell(*inFp));
#endif
#ifdef __cplusplus
                zStream.avail_in = static_cast<unsigned int>( fread(zInBuf, 1, STARCH_Z_CHUNK, *inFp) );
#else
                zStream.avail_in = (unsigned int) fread(zInBuf, 1, STARCH_Z_CHUNK, *inFp);
#endif
#ifdef DEBUG_VERBOSE
                fprintf(stderr, "--> (post-read-chunk) currently at byte [%013ld]\n", ftell(*inFp));
#endif
                if (zStream.avail_in == 0)
                    break;
                zStream.next_in = zInBuf;
                do {
                    zStream.avail_out = STARCH_Z_CHUNK;
                    zStream.next_out = zOutBuf;
#ifdef DEBUG_VERBOSE
                    fprintf(stderr, "--> (pre-loop)  zStream.avail_out [%013d]\n", zStream.avail_out);
#endif
                    zError = inflate(&zStream, Z_NO_FLUSH);
#ifdef DEBUG_VERBOSE
                    fprintf(stderr, "--> (post-loop) zStream.avail_out [%013d]\n", zStream.avail_out);
                    //cnt++;
#endif
                    switch (zError) {
                        case Z_NEED_DICT:  { fprintf(stderr, "ERROR: Z-stream needs dictionary\n");      return UNSTARCH_FATAL_ERROR; }
                        case Z_DATA_ERROR: { fprintf(stderr, "ERROR: Z-stream suffered data error\n");   return UNSTARCH_FATAL_ERROR; }
                        case Z_MEM_ERROR:  { fprintf(stderr, "ERROR: Z-stream suffered memory error\n"); return UNSTARCH_FATAL_ERROR; }
                    };
                    zHave = STARCH_Z_CHUNK - zStream.avail_out;
                    zOutBuf[zHave] = '\0';

                    /* copy remainder buffer onto line buffer, if not NULL */
                    if (zRemainderBuf) {
#ifdef __cplusplus
                        strncpy(reinterpret_cast<char *>( zLineBuf ), reinterpret_cast<const char *>( zRemainderBuf ), strlen(reinterpret_cast<const char *>( zRemainderBuf )));
                        zBufOffset = strlen(reinterpret_cast<const char *>( zRemainderBuf ));
#else
                        strncpy((char *) zLineBuf, (const char *) zRemainderBuf, strlen((const char *) zRemainderBuf));
                        zBufOffset = strlen((const char *) zRemainderBuf);
#endif
                    }
                    else 
                        zBufOffset = 0;

#ifdef DEBUG_VERBOSE
                    fprintf(stderr, "zHave [%d]\n", zHave);        
                    fprintf(stderr, "zBufIdx [%zu]\n", zBufOffset);
                    fprintf(stderr, "zOutBufIdx [%d]\n", zOutBufIdx);
#endif
                    
                    /* read through zOutBuf for newlines */                    
                    for (zBufIdx = zBufOffset, zOutBufIdx = 0; zOutBufIdx < zHave; zBufIdx++, zOutBufIdx++) {
                        zLineBuf[zBufIdx] = zOutBuf[zOutBufIdx];
                        if (zLineBuf[zBufIdx] == '\n') {
                            zLineBuf[zBufIdx] = '\0';
#ifdef __cplusplus
                            zBufIdx = static_cast<size_t>( -1 );
#else
                            zBufIdx = (size_t) -1;
#endif
                            (!headerFlag) ? \
                                UNSTARCH_reverseTransformHeaderlessInput(chromosome, zLineBuf, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp) : \
                                UNSTARCH_reverseTransformInput(chromosome, zLineBuf, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp);
                            firstInputToken[0] = '\0';
                            secondInputToken[0] = '\0';
                        }
                    }

                    /* copy some of line buffer onto the remainder buffer, if there are remnants from the z-stream */
#ifdef __cplusplus
                    if (strlen(reinterpret_cast<const char *>( zLineBuf )) > 0) {
#else
                    if (strlen((const char *) zLineBuf) > 0) {
#endif

#ifdef __cplusplus
                        if (strlen(reinterpret_cast<const char *>( zLineBuf )) > strlen(reinterpret_cast<const char *>( zRemainderBuf ))) {
#else
                        if (strlen((const char *) zLineBuf) > strlen((const char *) zRemainderBuf)) {
#endif
                            /* to minimize the chance of doing another (expensive) malloc, we double the length of zRemainderBuf */
                            free(zRemainderBuf);
#ifdef __cplusplus
                            zRemainderBuf = static_cast<unsigned char *>( malloc(strlen(reinterpret_cast<const char *>( zLineBuf )) * 2) );
#else
                            zRemainderBuf = malloc(strlen((const char *) zLineBuf) * 2);
#endif
                        }

                        /* it is necessary to copy only that part of zLineBuf up to zBufIdx characters  */
                        /* (zBufIdx characters were read into zLineBuf before no newline was found and  */
                        /* we end up in this conditional) as well as terminate the remainder buffer     */
                        /* zRemainderBuf, so that any cruft from a previous iteration is ignored in the */
                        /* next iteration of parsing the chromosome's z-stream                          */

#ifdef __cplusplus
                        strncpy(reinterpret_cast<char *>( zRemainderBuf ), reinterpret_cast<const char *>( zLineBuf ), zBufIdx);
#else
                        strncpy((char *) zRemainderBuf, (const char *) zLineBuf, zBufIdx);
#endif
                        zRemainderBuf[zBufIdx] = '\0';

                        /* we should only at most have to do this every STARCH_Z_CHUNK chars, and once  */
                        /* at the tail end of a chromosome's worth of a z-stream                        */
                    }    
                } while (zStream.avail_out == 0);

            } while (zError != Z_STREAM_END);

            /* close gzip stream */
            zError = inflateEnd(&zStream);
            if (zError != Z_OK) {
                fprintf(stderr, "ERROR: Could not close z-stream (%d)\n", zError);
                return UNSTARCH_FATAL_ERROR;
            }
            
            /* if we only want one specific chromosome, then we're done looping through chromosomes */
            if (strcmp(whichChr, chromosome) == 0)
                break;
        }
    }
    
    if (zRemainderBuf) {
        free(zRemainderBuf);
#ifdef __cplusplus
        zRemainderBuf = nullptr;
#else
        zRemainderBuf = NULL;
#endif
    }

    if (!chrFound) {
        fprintf(stderr, "ERROR: Could not find specified chromosome\n");
        return UNSTARCH_FATAL_ERROR;
    }

    if (firstInputToken) {
        free(firstInputToken);
#ifdef __cplusplus
        firstInputToken = nullptr;
#else
        firstInputToken = NULL;
#endif
    }
    
    if (secondInputToken) {
        free(secondInputToken);
#ifdef __cplusplus
        secondInputToken = nullptr;
#else
        secondInputToken = NULL;
#endif
    }

    return 0;
}

int 
UNSTARCH_extractDataWithBzip2(FILE **inFp, FILE *outFp, const char *whichChr, const Metadata *md, const uint64_t mdOffset, const Boolean headerFlag) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_extractDataWithBzip2() ---\n");
#endif
#ifdef __cplusplus
    char* firstInputToken = nullptr;
    char* secondInputToken = nullptr;
#else
    char* firstInputToken = NULL;
    char* secondInputToken = NULL;
#endif
    const Metadata* iter;
    char* chromosome;
    uint64_t size;	
    uint64_t cumulativeSize = 0;
    SignedCoordType start, pLength, lastEnd;
    char const* all = "all";
    BZFILE *bzFp;
    int bzError;
    unsigned char *bzOutput;
    size_t bzOutputLength = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
    /* unsigned char chrFound = UNSTARCH_FALSE; */

    if (!outFp)
        outFp = stdout;

#ifdef __cplusplus
    firstInputToken = static_cast<char *>( malloc(UNSTARCH_FIRST_TOKEN_MAX_LENGTH) );
#else
    firstInputToken = malloc(UNSTARCH_FIRST_TOKEN_MAX_LENGTH);
#endif
    if (!firstInputToken) {
        fprintf(stderr, "ERROR: (UNSTARCH_extractDataWithBzip2) Could not allocate space for first input token\n");
        return UNSTARCH_FATAL_ERROR;
    }
    firstInputToken[0] = '\0';

#ifdef __cplusplus
    secondInputToken = static_cast<char *>( malloc(UNSTARCH_SECOND_TOKEN_MAX_LENGTH) );
#else
    secondInputToken = malloc(UNSTARCH_SECOND_TOKEN_MAX_LENGTH);
#endif
    if (!secondInputToken) {
        fprintf(stderr, "ERROR: (UNSTARCH_extractDataWithBzip2) Could not allocate space for second input token\n");
        return UNSTARCH_FATAL_ERROR;
    }
    secondInputToken[0] = '\0';

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        chromosome = iter->chromosome; 
        size = iter->size;
        start = 0;
        pLength = 0;
        lastEnd = 0;

#ifdef __cplusplus
        if (STARCH_fseeko(*inFp, static_cast<off_t>( cumulativeSize + mdOffset ), SEEK_SET) != 0) {
#else
        if (STARCH_fseeko(*inFp, (off_t) (cumulativeSize + mdOffset), SEEK_SET) != 0) {
#endif
            fprintf(stderr, "ERROR: Could not seek data in archive\n");
            return UNSTARCH_FATAL_ERROR;
        }
        cumulativeSize += size;

        if ((strcmp(whichChr, all) == 0) || (strcmp(whichChr, chromosome) == 0)) {

            /* chrFound = UNSTARCH_TRUE; */
#ifdef __cplusplus
            bzFp = BZ2_bzReadOpen( &bzError, *inFp, 0, 0, nullptr, 0 ); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzcompress-init */
#else
            bzFp = BZ2_bzReadOpen( &bzError, *inFp, 0, 0, NULL, 0 ); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzcompress-init */
#endif
            if (bzError != BZ_OK) {
                BZ2_bzReadClose( &bzError, bzFp );
                fprintf(stderr, "ERROR: Bzip2 data stream could not be opened\n");
                return UNSTARCH_FATAL_ERROR;
            }

#ifdef __cplusplus
            bzOutput = static_cast<unsigned char *>( malloc(bzOutputLength) );
#else
            bzOutput = malloc(bzOutputLength);
#endif

            do {
                UNSTARCH_bzReadLine(bzFp, &bzOutput);                
                if (bzOutput) {
                    (!headerFlag) ? \
                        UNSTARCH_reverseTransformHeaderlessInput(chromosome, bzOutput, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp): \
                        UNSTARCH_reverseTransformInput(chromosome, bzOutput, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp);
                    firstInputToken[0] = '\0';
                    secondInputToken[0] = '\0';
                }
#ifdef __cplusplus
            } while (bzOutput != nullptr);
#else
            } while (bzOutput != NULL);
#endif
            free(bzOutput);

            BZ2_bzReadClose(&bzError, bzFp);		

            if (strcmp(whichChr, chromosome) == 0)
                break;
        }
    }

    /*
        If no chromosome is found, we do nothing...

        if (!chrFound) {
            fprintf(stderr, "ERROR: Could not find specified chromosome\n");
            return UNSTARCH_FATAL_ERROR;
        }
    */

    if (firstInputToken) {
        free(firstInputToken);
#ifdef __cplusplus
        firstInputToken = nullptr;
#else
        firstInputToken = NULL;
#endif
    }
    
    if (secondInputToken) {
        free(secondInputToken);
#ifdef __cplusplus
        secondInputToken = nullptr;
#else
        secondInputToken = NULL;
#endif
    }

    return 0;
}

void 
UNSTARCH_bzReadLine(BZFILE *input, unsigned char **output) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_bzReadLine() ---\n");
#endif
#ifdef __cplusplus
    unsigned char *outputCopy = nullptr;
#else
    unsigned char *outputCopy = NULL;
#endif
    size_t offset = 0;
    size_t len = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
    int bzError;
    Boolean runFlag = kStarchFalse;

    if ((!input) || (!*output)) {
#ifdef DEBUG
        if (!input)
            fprintf(stderr, "\tinput is NULL\n");
        if (!*output)
            fprintf(stderr, "\toutput is NULL\n");
#endif
        return;
    }

    while (BZ2_bzRead(&bzError, input, *output + offset, 1) == 1) {
        runFlag = kStarchTrue;
        if (offset + 1 == len) {
            len += UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
            outputCopy = static_cast<unsigned char *>( realloc(*output, len) );
#else
            outputCopy = realloc(*output, len);
#endif
            if (! outputCopy) {
                fprintf(stderr, "ERROR: Could not reallocate space for compressed buffer.\n");
                exit(-1);
            }
            *output = outputCopy;
        }
        if (*(*output + offset) == '\n')
            break;
        offset++;
    }

    if (*(*output + offset) == '\n') {
#ifdef DEBUG
        fprintf(stderr, "\tterminating *output\n");
#endif
        *(*output + offset) = '\0';
    }
    else if ((bzError != BZ_STREAM_END) || (runFlag == kStarchFalse)) {
#ifdef DEBUG
        if (bzError != BZ_STREAM_END)
            fprintf(stderr, "\tbzError: %d\n", bzError);
        if (runFlag == kStarchFalse)
            fprintf(stderr, "\trunFlag is false\n");
#endif
        free(*output);
#ifdef __cplusplus
        *output = nullptr;
#else
        *output = NULL;
#endif
    }
}

int 
UNSTARCH_reverseTransformInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformInput() ---\n");
#endif
#ifdef __cplusplus
    char *pTest = nullptr;
#else
    char *pTest = NULL;
#endif
    char *pTestChars;
    const char *pTestParam = "p";

    /* if *str begins with a reserved header name, then we
       shortcut and print the line without transformation */

#ifdef __cplusplus
    if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        fprintf(outFp, "%s\n", str);
#else
    if (strncmp((const char *) str, kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        fprintf(outFp, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        fprintf(outFp, "%s\n", str);
#endif

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( *start ), static_cast<SignedCoordType>( *lastEnd ), elemTok2);
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) *start, (SignedCoordType) *lastEnd, elemTok2);
#endif
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ), static_cast<SignedCoordType>( *lastEnd ), elemTok2);
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), (SignedCoordType) *lastEnd, elemTok2);
#endif
            }
        }
        else {
#ifdef __cplusplus
            pTest = nullptr;
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
#ifdef __cplusplus
                pTestChars = nullptr;
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = NULL;
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, nullptr, UNSTARCH_RADIX) );
                free(pTestChars); 
                pTestChars = nullptr;
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\n", chr, *start, *lastEnd);
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Compressed data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

    return 0;
}

int 
UNSTARCH_sReverseTransformInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, SignedCoordType *currentStart, SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_sReverseTransformInput() ---\n");
#endif
#ifdef __cplusplus
    char *pTest = nullptr;
    char *currentRemainderCopy = nullptr;
#else
    char *pTest = NULL;
    char *currentRemainderCopy = NULL;
#endif
    char *pTestChars;
    const char *pTestParam = "p";

    /* if *str begins with a reserved header name, then we shortcut and print the line without transformation */

    /* 
        TO-DO: 
            -- out[] needs to be copied to currentChr or similar
    */
    
    char out[1024];

#ifdef __cplusplus
    if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        sprintf(out, "%s\n", str);
#else
    if (strncmp((const char *) str, kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        sprintf(out, "%s\n", str);
    else if (strncmp((const char *) str, kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        sprintf(out, "%s\n", str);
#endif

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Could not extend chr token beyond TOKEN_CHR_MAX_LENGTH characters\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
                *currentStart = *start;
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
#ifdef __cplusplus
                    *currentRemainder = static_cast<char *>( malloc(strlen(elemTok2) + 1) );
#else
                    *currentRemainder = malloc(strlen(elemTok2) + 1);
#endif
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
#ifdef __cplusplus
                    currentRemainderCopy = static_cast<char *>( realloc(*currentRemainder, strlen(elemTok2) * 2) );
#else
                    currentRemainderCopy = realloc(*currentRemainder, strlen(elemTok2) * 2);
#endif
                    if (!currentRemainderCopy) {
#ifdef __cplusplus
                        /* why can't c++ standardize on a format specifier for a simple std::size_t ? */
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token (%" PRIu64 " | %" PRIu64 ")\n",
                                static_cast<uint64_t>(strlen(elemTok2)), static_cast<uint64_t>(*currentRemainderLen));
#else
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token (%zu | %zu)\n", strlen(elemTok2), *currentRemainderLen);
#endif
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentRemainder = currentRemainderCopy;
                    *currentRemainderLen = strlen(elemTok2) * 2;
                }
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
           }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ) + *pLength;
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Cannot extend chr token past TOKEN_CHR_MAX_LENGTH bytes\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
#ifdef __cplusplus
                *currentStart = static_cast<int64_t>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *currentStart = (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
#ifdef __cplusplus
                    *currentRemainder = static_cast<char *>( malloc(strlen(elemTok2) + 1) );
#else
                    *currentRemainder = malloc(strlen(elemTok2) + 1);
#endif
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
#ifdef __cplusplus
                    currentRemainderCopy = static_cast<char *>( realloc(*currentRemainder, strlen(elemTok2) * 2) );
#else
                    currentRemainderCopy = realloc(*currentRemainder, strlen(elemTok2) * 2);
#endif
                    if (!currentRemainderCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentRemainder = currentRemainderCopy;
                    *currentRemainderLen = strlen(elemTok2) * 2;
                }
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;
                }
            }
        }
        else {
#ifdef __cplusplus
            pTest = nullptr;
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
#ifdef __cplusplus
                pTestChars = nullptr;
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = NULL;
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, nullptr, UNSTARCH_RADIX) );
                free(pTestChars); 
                pTestChars = nullptr;
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Cannot extend chr token beyond TOKEN_CHR_MAX_LENGTH in length\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
                *currentStart = *start;
                *currentStop = *lastEnd;
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Compressed data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

    return 0;
}

int 
UNSTARCH_reverseTransformIgnoringHeaderedInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformIgnoringHeaderedInput() ---\n");
#endif
#ifdef __cplusplus
    char *pTest = nullptr;
#else
    char *pTest = NULL;
#endif
    char *pTestChars;
    const char *pTestParam = "p";

    /* if *str begins with a reserved header name, then we
       shortcut and do nothing */

#ifdef __cplusplus
    if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        ;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        ;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        ;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        ;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        ;
#else
    if (strncmp((const char *) str, kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        ;
    else if (strncmp((const char *) str, kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        ;
    else if (strncmp((const char *) str, kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        ;
    else if (strncmp((const char *) str, kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        ;
    else if (strncmp((const char *) str, kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        ;
#endif

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ), *lastEnd, elemTok2);
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
#endif
            }
        }
        else {
#ifdef __cplusplus
            pTest = nullptr;
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>(elemTok1), pTestParam, 1);
#else
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
#ifdef __cplusplus
                pTestChars = nullptr;
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = NULL;
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, nullptr, UNSTARCH_RADIX) );
                free(pTestChars); 
                pTestChars = nullptr;
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\n", chr, *start, *lastEnd);
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Compressed data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

    return 0;
}

int 
UNSTARCH_sReverseTransformIgnoringHeaderedInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, SignedCoordType *currentStart, SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
{
#ifdef DEBUG
    /*
    fprintf(stderr, "\n--- UNSTARCH_sReverseTransformIgnoringHeaderedInput() --- start\n");
    fprintf(stderr, "\tchr -> [%s]\n", chr);
    fprintf(stderr, "\tcurrentChr -> [%s]\n", *currentChr);
    fprintf(stderr, "\tcurrentChrLen -> [%zu]\n", *currentChrLen);
    fprintf(stderr, "\tstr -> [%s]\n", str);
    fprintf(stderr, "\tdelim -> [%c]\n", delim);
    fprintf(stderr, "\tstart -> [%" PRId64 "]\n", *start);
    fprintf(stderr, "\tpLength -> [%" PRId64 "]\n", *pLength);
    fprintf(stderr, "\tlastEnd -> [%" PRId64 "]\n", *lastEnd);
    fprintf(stderr, "\telemTok1 -> [%s]\n", elemTok1);
    fprintf(stderr, "\telemTok2 -> [%s]\n", elemTok2);
    fprintf(stderr, "\tcurrentStart -> [%" PRId64 "]\n", *currentStart);
    fprintf(stderr, "\tcurrentStop -> [%" PRId64 "]\n", *currentStop);
    fprintf(stderr, "\tcurrentRemainder -> [%s]\n", *currentRemainder);
    fprintf(stderr, "\tcurrentRemainderLen -> [%zu]\n", *currentRemainderLen);
    */
#endif
#ifdef __cplusplus
    char *currentRemainderCopy = nullptr;
#else
    char *currentRemainderCopy = NULL;
#endif
    char pTestChars[MAX_DEC_INTEGERS] = {0};

    /* if *str begins with a reserved header name, then we shortcut */

#ifdef __cplusplus
    if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        return -1;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        return -1;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        return -1;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        return -1;
    else if (strncmp(reinterpret_cast<const char *>(const_cast<unsigned char *>(str)), kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        return -1;
#else
    if (strncmp((const char *) str, kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0)
        return -1;
    else if (strncmp((const char *) str, kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0)
        return -1;
    else if (strncmp((const char *) str, kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0)
        return -1;
    else if (strncmp((const char *) str, kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0)
        return -1;
    else if (strncmp((const char *) str, kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0)
        return -1;
#endif

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 
        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                fprintf(stderr, "A: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
#endif
                if (! *currentChr) {
                    fprintf(stderr, "malloc-ing *currentChr in UNSTARCH_sReverseTransformIgnoringHeaderedInput()\n");
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Cannot extend chr token past TOKEN_CHR_MAX_LENGTH\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name is not set\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                *currentStart = *start;
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
#ifdef __cplusplus
                    *currentRemainder = static_cast<char *>( malloc(strlen(elemTok2) + 1) );
#else
                    *currentRemainder = malloc(strlen(elemTok2) + 1);
#endif
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
#ifdef __cplusplus
                    currentRemainderCopy = static_cast<char *>( realloc(*currentRemainder, strlen(elemTok2) * 2) );
#else
                    currentRemainderCopy = realloc(*currentRemainder, strlen(elemTok2) * 2);
#endif
                    if (!currentRemainderCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentRemainder = currentRemainderCopy;
                    *currentRemainderLen = strlen(elemTok2) * 2;
                }
                if (strlen(elemTok2) >= *currentRemainderLen) {
                    fprintf(stderr, "ERROR: Current remainder token not long enough!\n");
                    exit(-1);
                }
#ifdef DEBUG
                fprintf(stderr, "BEFORE currentRemainder -> [%s]\n", *currentRemainder);
                fprintf(stderr, "BEFORE elemTok2         -> [%s]\n", elemTok2);
#endif
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);  
#ifdef DEBUG
                fprintf(stderr, "AFTER  currentRemainder -> [%s]\n", *currentRemainder);
                fprintf(stderr, "AFTER  elemTok2         -> [%s]\n", elemTok2);
#endif
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ) + *pLength;
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#endif
#ifdef DEBUG
#ifdef __cplusplus
                fprintf(stderr, "B: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (int64_t) strtoull(elemTok1, nullptr, UNSTARCH_RADIX), *lastEnd, elemTok2);
#else
                fprintf(stderr, "B: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
#endif
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = (char *) malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if (strlen(chr) + 1 > *currentChrLen) {
                    fprintf(stderr, "ERROR: Cannot extend chr token past TOKEN_CHR_MAX_LENGTH\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (! *currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;
                }
#ifdef __cplusplus
                *currentStart = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *currentStart = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
#ifdef __cplusplus
                    *currentRemainder = static_cast<char *>( malloc(strlen(elemTok2) + 1) );
#else
                    *currentRemainder = malloc(strlen(elemTok2) + 1);
#endif
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
#ifdef __cplusplus
                    currentRemainderCopy = static_cast<char *>( realloc(*currentRemainder, strlen(elemTok2) * 2) );
#else
                    currentRemainderCopy = realloc(*currentRemainder, strlen(elemTok2) * 2);
#endif
                    if (!currentRemainderCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentRemainder = currentRemainderCopy;
                    *currentRemainderLen = strlen(elemTok2) * 2;
                }
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;
                }
            }
        }
        else {
            if (! *currentChr) {
#ifdef __cplusplus
                *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                if (! *currentChr) {
                    fprintf(stderr, "ERROR: Ran out of memory while allocating chr token\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                *currentChrLen = TOKEN_CHR_MAX_LENGTH;
            }
            else if ((strlen(chr) + 1) > *currentChrLen) {
                fprintf(stderr, "ERROR: Cannot extend chr token beyond TOKEN_CHR_MAX_LENGTH\n");
                return UNSTARCH_FATAL_ERROR;
            }
            strncpy(*currentChr, chr, strlen(chr) + 1);

            if (elemTok1[0] == 'p') {
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, nullptr, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                fprintf(stderr, "D: %s\t%" PRId64 "\t%" PRId64 "\n", chr, *start, *lastEnd);
#endif
                *currentStart = *start;
                *currentStop = *lastEnd;
            }
            if (*currentRemainder) {
                *currentRemainder[0] = '\0';
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Compressed data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

#ifdef DEBUG
    fprintf(stderr, "\n--- leaving UNSTARCH_sReverseTransformIgnoringHeaderedInput() ---\n");
#endif

    return 0;
}

int 
UNSTARCH_reverseTransformHeaderlessInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformHeaderlessInput() ---\n");
    fprintf(stderr, "\n--- str :      [%s] \n", str);
    fprintf(stderr, "\n--- delim :    [%c]\n", delim);
    fprintf(stderr, "\n--- elemTok1 : [%s]\n", elemTok1);
    fprintf(stderr, "\n--- elemTok2 : [%s]\n", elemTok2);
#endif
#ifdef __cplusplus
    char *pTest = nullptr;
#else
    char *pTest = NULL;
#endif
    char *pTestChars;
    const char *pTestParam = "p";

    if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) 
    { 
        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ) + *pLength;
		fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ), *lastEnd, elemTok2);
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
		fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
#endif
            }
        }
        else {
#ifdef __cplusplus
            pTest = nullptr;
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
#ifdef __cplusplus
                pTestChars = nullptr;
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = NULL;
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, nullptr, UNSTARCH_RADIX) );
                free(pTestChars); 
                pTestChars = nullptr;
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\n", chr, *start, *lastEnd);
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

    return 0;
}

int
UNSTARCH_extractRawLine(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, SignedCoordType *currentStart, SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
{
    /*
        UNSTARCH_extractRawLine() takes in a buffer of post-transform BED data (which
        could come from a bzip2 or gzip stream that gets extracted from a starch file)
        and reverse transforms portions of a BED element to several variables passed 
        in by pointers.

        The following variables are "read-only" inputs:

            char              *chr                -> chromosome (obtained from metadata record)
            unsigned char     *str                -> buffer containing post-transform data to reverse transform
            char               delim              -> field delimiter (shoud usually be a tab character)

        The *str value should be a single line of data extracted from a bzip2- or gzip-compressed
        chromosome stream. In other words, extract data from the stream into a buffer, scan through
        until a newline is found, and then run this function on the post-transform line.

        Other variables are populated as the extraction is perfomed. The following variables 
        are probably useful for reconstructing the original BED data:

            char     **currentChr        -> chromosome
            int64_t   *currentStart      -> start position
            int64_t   *currentStop       -> stop position
            char     **currentRemainder  -> rest of BED element (NULL, if not available)

        After running UNSTARCH_extractRawLine(), these variables can be used to rebuild
        a BED element.

        While the code below calls this function repeatedly, this is done to ensure that 
        we apply the reverse transformation steps correctly and move the calculation state and
        value cursor forvarts.
    */

#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_extractRawLine() ---\n");
    /* fprintf(stderr, "\tstr -> %s\nstart -> %" PRId64 "\n---\n", str, *start); */
#endif
    int res;

    do {
        /* res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, &(*start), &(*pLength), &(*lastEnd), elemTok1, elemTok2, currentChr, &(*currentChrLen), &(*currentStart), &(*currentStop), &(*currentRemainder), &(*currentRemainderLen)); */
        res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, start, pLength, lastEnd, elemTok1, elemTok2, currentChr, currentChrLen, currentStart, currentStop, currentRemainder, currentRemainderLen);
#ifdef DEBUG
        /*
        fprintf(stderr, "\tintermediate start   (A) -> %" PRId64 "\n", *start);
        fprintf(stderr, "\tintermediate pLength (A) -> %" PRId64 "\n", *pLength);
        fprintf(stderr, "\tintermediate lastEnd (A) -> %" PRId64 "\n", *lastEnd);
        */
#endif
        if (res != 0) {
            res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, &(*start), &(*pLength), &(*lastEnd), elemTok1, elemTok2, currentChr, &(*currentChrLen), &(*currentStart), &(*currentStop), &(*currentRemainder), &(*currentRemainderLen));
        }
#ifdef DEBUG
        /*
        fprintf(stderr, "\tintermediate start   (B) -> %" PRId64 "\n", *start);
        fprintf(stderr, "\tintermediate pLength (B) -> %" PRId64 "\n", *pLength);
        fprintf(stderr, "\tintermediate lastEnd (B) -> %" PRId64 "\n", *lastEnd);
        */
#endif
        /* res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, start, pLength, lastEnd, elemTok1, elemTok2, currentChr, currentChrLen, currentStart, currentStop, currentRemainder, currentRemainderLen); */
    } while (res != 0);

    if (str[0] == 'p') {
        res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, &(*start), &(*pLength), &(*lastEnd), elemTok1, elemTok2, currentChr, &(*currentChrLen), &(*currentStart), &(*currentStop), &(*currentRemainder), &(*currentRemainderLen));
#ifdef DEBUG
        /*
        fprintf(stderr, "\tintermediate start   (C) -> %" PRId64 "\n", *start);
        fprintf(stderr, "\tintermediate pLength (C) -> %" PRId64 "\n", *pLength);
        fprintf(stderr, "\tintermediate lastEnd (C) -> %" PRId64 "\n", *lastEnd);
        */
#endif
    }

        /* res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, start, pLength, lastEnd, elemTok1, elemTok2, currentChr, currentChrLen, currentStart, currentStop, currentRemainder, currentRemainderLen); */

    return 0;
}

int 
UNSTARCH_sReverseTransformHeaderlessInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, SignedCoordType *currentStart, SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_sReverseTransformHeaderlessInput() ---\n");
#endif
#ifdef __cplusplus
    char *pTest = nullptr;
    char *currentRemainderCopy = nullptr;
#else
    char *pTest = NULL;
    char *currentRemainderCopy = NULL;
#endif
    char *pTestChars;
    const char *pTestParam = "p";

    if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) 
    { 
        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                /*
                sprintf(out, "%s\t%lld\t%lld\t%s\n", chr, *start, *lastEnd, elemTok2);
                */
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Could not extend chr token past TOKEN_CHR_MAX_LENGTH characters\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
                *currentStart = *start;
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
#ifdef __cplusplus
                    *currentRemainder = static_cast<char *>( malloc(strlen(elemTok2) + 1) );
#else
                    *currentRemainder = malloc(strlen(elemTok2) + 1);
#endif
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
#ifdef __cplusplus
                    currentRemainderCopy = static_cast<char *>( realloc(*currentRemainder, strlen(elemTok2) * 2) );
#else
                    currentRemainderCopy = realloc(*currentRemainder, strlen(elemTok2) * 2);
#endif
                    if (!currentRemainderCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentRemainder = currentRemainderCopy;
                    *currentRemainderLen = strlen(elemTok2) * 2;
                }
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) ) + *pLength;
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#endif
#ifdef DEBUG
                /*
                sprintf(out, "%s\t%lld\t%lld\t%s\n", chr, (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
                */
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Cannot extend chr token past TOKEN_CHR_MAX_LENGTH characters in length\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
#ifdef __cplusplus
                *currentStart = static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *currentStart = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
#ifdef __cplusplus
                    *currentRemainder = static_cast<char *>( malloc(strlen(elemTok2) + 1) );
#else
                    *currentRemainder = malloc(strlen(elemTok2) + 1);
#endif
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
#ifdef __cplusplus
                    currentRemainderCopy = static_cast<char *>( realloc(*currentRemainder, strlen(elemTok2) * 2) );
#else
                    currentRemainderCopy = realloc(*currentRemainder, strlen(elemTok2) * 2);
#endif
                    if (!currentRemainderCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending remainder token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentRemainder = currentRemainderCopy;
                    *currentRemainderLen = strlen(elemTok2) * 2;
                }
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;
                }
            }
        }
        else {
#ifdef __cplusplus
            pTest = nullptr;
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
#ifdef __cplusplus
                pTestChars = nullptr;
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = NULL;
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, nullptr, UNSTARCH_RADIX) );
                free(pTestChars); 
                pTestChars = nullptr;
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, nullptr, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                /*
                sprintf(out, "%s\t%lld\t%lld\n", chr, *start, *lastEnd);
                */
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(TOKEN_CHR_MAX_LENGTH) );
#else
                    *currentChr = malloc(TOKEN_CHR_MAX_LENGTH);
#endif
                    *currentChrLen = TOKEN_CHR_MAX_LENGTH;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    fprintf(stderr, "ERROR: Cannot extend chr token past TOKEN_CHR_MAX_LENGTH characters\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                *currentStart = *start;
                *currentStop = *lastEnd;
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

    return 0;
}

int
UNSTARCH_createInverseTransformTokens(const unsigned char *s, const char delim, char elemTok1[], char elemTok2[])
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_createInverseTransformTokens() --- start\n");
#endif
    int charCnt, sCnt, elemCnt;
    unsigned char buffer[UNSTARCH_BUFFER_MAX_LENGTH];

    charCnt = 0;
    sCnt = 0;
    elemCnt = 0;

    elemTok1[0] = '\0';
    elemTok2[0] = '\0';

    do {
        buffer[charCnt++] = s[sCnt];
        if (buffer[(charCnt - 1)] == delim) {
            if (elemCnt == 0) {
                buffer[(charCnt - 1)] = '\0';
#ifdef DEBUG
                fprintf(stderr, "copying buffer [%d] [%s] to elemTok1 (A)\n", charCnt-1, buffer);
#endif
#ifdef __cplusplus
                strncpy(elemTok1, reinterpret_cast<const char *>( buffer ), strlen(reinterpret_cast<const char *>( buffer )) + 1);
#else
                strncpy(elemTok1, (const char *) buffer, strlen((const char *) buffer) + 1);
#endif
                elemCnt++;
                charCnt = 0;
            }
        }
    } while (s[sCnt++] != 0);

    if (elemCnt == 0) {
        buffer[charCnt-1] = '\0';
#ifdef DEBUG
        fprintf(stderr, "copying buffer [%d] [%s] to elemTok1 (B)\n", charCnt-1, buffer);
#endif
#ifdef __cplusplus
        strncpy(elemTok1, reinterpret_cast<const char *>( buffer ), strlen(reinterpret_cast<const char *>( buffer )) + 1);
#else
        strncpy(elemTok1, (const char *) buffer, strlen((const char *) buffer) + 1);
#endif
    }
    if (elemCnt == 1) {
        buffer[charCnt-1] = '\0';
#ifdef DEBUG
        fprintf(stderr, "copying buffer [%d] [%s] to elemTok2 (C)\n", charCnt-1, buffer);
#endif
#ifdef __cplusplus
        strncpy(elemTok2, reinterpret_cast<const char *>( buffer ), strlen(reinterpret_cast<const char *>( buffer )) + 1);
#else
        strncpy(elemTok2, (const char *) buffer, strlen((const char *) buffer) + 1);
#endif
    }

#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_createInverseTransformTokens() --- end\n");
#endif

    return 0;
}

char * 
UNSTARCH_strnstr(const char *haystack, const char *needle, size_t haystackLen) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_strnstr() ---\n");
#endif
    char *p;
    size_t pLen;
    size_t len = strlen(needle);

    if (*needle == '\0') {
	/* everything matches empty string */
#ifdef __cplusplus
        return const_cast<char *>( haystack );
#else
        return (char *) haystack;
#endif
    }

    pLen = haystackLen;
#ifdef __cplusplus
    for (p = const_cast<char *>( haystack ); p != nullptr; p = static_cast<char *>( memchr(p + 1, *needle, pLen-1) )) {
        pLen = haystackLen - static_cast<size_t>( p - const_cast<char *>( haystack ) );
        if (pLen < len) 
            return nullptr;
        if (strncmp(p, needle, len) == 0)
            return p;
    }

    return nullptr;
#else
    for (p = (char *) haystack; p != NULL; p = (char *) memchr(p + 1, *needle, pLen-1)) {
        pLen = haystackLen - (size_t) (p - haystack);
        if (pLen < len) 
            return NULL;
        if (strncmp(p, needle, len) == 0)
            return p;
    }

    return NULL;
#endif
}

char * 
UNSTARCH_strndup(const char *s, size_t n) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_strnstr() ---\n");
#endif
    char *result;
    size_t len = strlen(s);

    if (n < len)
        len = n;

#ifdef __cplusplus
    result = static_cast<char *>( malloc(len + 1) );

    if (!result)
        return nullptr;
#else
    result = malloc(len + 1);

    if (!result)
        return NULL;
#endif

    result[len] = '\0';

#ifdef __cplusplus
    return static_cast<char *>( memcpy (result, s, len) );
#else
    return (char *) memcpy (result, s, len);
#endif
}

LineCountType
UNSTARCH_lineCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_lineCountForChromosome() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(chr, iter->chromosome) == 0) {
            return iter->lineCount;
        }
    }

#ifdef __cplusplus
    return static_cast<LineCountType>( 0 );
#else
    return (LineCountType) 0;
#endif
}

void
UNSTARCH_printLineCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printLineCountForChromosome() ---\n");
#endif

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printLineCountForAllChromosomes(md);
    else
        fprintf(stdout, "%" PRIu64 "\n", UNSTARCH_lineCountForChromosome(md, chr));
}

void
UNSTARCH_printLineCountForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printLineCountForAllChromosomes() ---\n");
#endif
    const Metadata *iter;
    LineCountType totalLineCount = 0;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next)
#else
    for (iter = md; iter != NULL; iter = iter->next)
#endif
        totalLineCount += iter->lineCount;

    fprintf(stdout, "%" PRIu64 "\n", totalLineCount);
}

LineLengthType
UNSTARCH_lineMaxStringLengthForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_lineMaxStringLengthForChromosome() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(chr, iter->chromosome) == 0) {
            return iter->lineMaxStringLength;
        }
    }

#ifdef __cplusplus
    return static_cast<LineLengthType>( STARCH_DEFAULT_LINE_STRING_LENGTH );
#else
    return (LineLengthType) STARCH_DEFAULT_LINE_STRING_LENGTH;
#endif
}

void
UNSTARCH_printLineMaxStringLengthForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printLineMaxStringLengthForChromosome() ---\n");
#endif

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printLineMaxStringLengthForAllChromosomes(md);
    else
        fprintf(stdout, "%lu\n", UNSTARCH_lineMaxStringLengthForChromosome(md, chr));
}

void
UNSTARCH_printLineMaxStringLengthForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printLineMaxStringLengthForAllChromosomes() ---\n");
#endif
    const Metadata *iter;
    LineLengthType lineMaxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next)
#else
    for (iter = md; iter != NULL; iter = iter->next)
#endif
        lineMaxStringLength = (lineMaxStringLength >= iter->lineMaxStringLength) ? lineMaxStringLength : iter->lineMaxStringLength;

    fprintf(stdout, "%lu\n", lineMaxStringLength);
}

BaseCountType
UNSTARCH_nonUniqueBaseCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_nonUniqueBaseCountForChromosome() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(chr, iter->chromosome) == 0) {
            return iter->totalNonUniqueBases;
        }
    }

#ifdef __cplusplus
    return static_cast<BaseCountType>( 0 );
#else
    return (BaseCountType) 0;
#endif
}

void 
UNSTARCH_printNonUniqueBaseCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNonUniqueBaseCountForChromosome() ---\n");
#endif

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printNonUniqueBaseCountForAllChromosomes(md);
    else
        fprintf(stdout, "%" PRIu64 "\n", UNSTARCH_nonUniqueBaseCountForChromosome(md, chr));
}

void
UNSTARCH_printNonUniqueBaseCountForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNonUniqueBaseCountForAllChromosomes() ---\n");
#endif
    const Metadata *iter;
    BaseCountType totalBaseCount = 0;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next)
#else
    for (iter = md; iter != NULL; iter = iter->next)
#endif
        totalBaseCount += iter->totalNonUniqueBases;

    fprintf(stdout, "%" PRIu64 "\n", totalBaseCount);
}

BaseCountType
UNSTARCH_uniqueBaseCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_uniqueBaseCountForChromosome() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(chr, iter->chromosome) == 0)
            return iter->totalUniqueBases;
    }

#ifdef __cplusplus
    return static_cast<BaseCountType>( 0 );
#else
    return (BaseCountType) 0;
#endif
}

void 
UNSTARCH_printUniqueBaseCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printUniqueBaseCountForChromosome() ---\n");
#endif

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printUniqueBaseCountForAllChromosomes(md);
    else
        fprintf(stdout, "%" PRIu64 "\n", UNSTARCH_uniqueBaseCountForChromosome(md, chr));
}

void
UNSTARCH_printUniqueBaseCountForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printUniqueBaseCountForAllChromosomes() ---\n");
#endif
    const Metadata *iter;
    BaseCountType totalBaseCount = 0;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next)
#else
    for (iter = md; iter != NULL; iter = iter->next)
#endif
        totalBaseCount += iter->totalUniqueBases;

    fprintf(stdout, "%" PRIu64 "\n", totalBaseCount);
}

Boolean
UNSTARCH_duplicateElementExistsForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_duplicateElementExistsForChromosome() ---\n");
#endif 
    const Metadata *iter;
    
#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(chr, iter->chromosome) == 0)
            return iter->duplicateElementExists;
    }

    return STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
}

void
UNSTARCH_printDuplicateElementExistsStringForChromosome(const Metadata *md, const char *chr) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printDuplicateElementExistsForChromosome() ---\n");
#endif

    Boolean res = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printDuplicateElementExistsStringsForAllChromosomes(md);
    else {
        res = UNSTARCH_duplicateElementExistsForChromosome(md, chr);
        fprintf(stdout, "%s\n", UNSTARCH_booleanToString(res));
    }
}

void
UNSTARCH_printDuplicateElementExistsStringsForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printDuplicateElementExistsForAllChromosomes() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (UNSTARCH_duplicateElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
            fprintf(stdout, "%s\n", UNSTARCH_booleanToString(kStarchTrue));
            return;
        }
    }

    fprintf(stdout, "%s\n", UNSTARCH_booleanToString(kStarchFalse));
}

void
UNSTARCH_printDuplicateElementExistsIntegerForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printDuplicateElementExistsIntegerForChromosome() ---\n");
#endif
    Boolean res = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printDuplicateElementExistsIntegersForAllChromosomes(md);
    else {
        res = UNSTARCH_duplicateElementExistsForChromosome(md, chr);
#ifdef __cplusplus
        fprintf(stdout, "%d\n", static_cast<int>( res ));
#else
        fprintf(stdout, "%d\n", (int) res);
#endif
    }
}

void
UNSTARCH_printDuplicateElementExistsIntegersForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printDuplicateElementExistsIntegersForAllChromosomes() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
        if (UNSTARCH_duplicateElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
            fprintf(stdout, "%d\n", static_cast<int>( kStarchTrue ));
            return;
        }
    }

    fprintf(stdout, "%d\n", static_cast<int>( kStarchFalse ));
#else
    for (iter = md; iter != NULL; iter = iter->next) {
        if (UNSTARCH_duplicateElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
            fprintf(stdout, "%d\n", (int) kStarchTrue);
            return;
        }
    }

    fprintf(stdout, "%d\n", (int) kStarchFalse);
#endif
}

Boolean
UNSTARCH_nestedElementExistsForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_nestedElementExistsForChromosome() ---\n");
#endif 
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else    
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(chr, iter->chromosome) == 0)
            return iter->nestedElementExists;
    }

    return STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
}

void
UNSTARCH_printNestedElementExistsStringForChromosome(const Metadata *md, const char *chr) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNestedElementExistsForChromosome() ---\n");
#endif
    Boolean res = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printNestedElementExistsStringsForAllChromosomes(md);
    else {
        res = UNSTARCH_nestedElementExistsForChromosome(md, chr);
        fprintf(stdout, "%s\n", UNSTARCH_booleanToString(res));
    }
}

void
UNSTARCH_printNestedElementExistsStringsForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNestedElementExistsForAllChromosomes() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (UNSTARCH_nestedElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
            fprintf(stdout, "%s\n", UNSTARCH_booleanToString(kStarchTrue));
            return;
        }
    }

    fprintf(stdout, "%s\n", UNSTARCH_booleanToString(kStarchFalse));
}

void
UNSTARCH_printNestedElementExistsIntegerForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNestedElementExistsIntegerForChromosome() ---\n");
#endif
    Boolean res = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;

    if (strcmp(chr, "all") == 0)
        UNSTARCH_printNestedElementExistsIntegersForAllChromosomes(md);
    else {
        res = UNSTARCH_nestedElementExistsForChromosome(md, chr);
#ifdef __cplusplus
        fprintf(stdout, "%d\n", static_cast<int>( res ));
#else
        fprintf(stdout, "%d\n", (int) res);
#endif
    }
}

void
UNSTARCH_printNestedElementExistsIntegersForAllChromosomes(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNestedElementExistsIntegersForAllChromosomes() ---\n");
#endif
    const Metadata *iter;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
        if (UNSTARCH_nestedElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
            fprintf(stdout, "%d\n", static_cast<int>( kStarchTrue ));
            return;
        }
    }

    fprintf(stdout, "%d\n", static_cast<int>( kStarchFalse ));
#else
    for (iter = md; iter != NULL; iter = iter->next) {
        if (UNSTARCH_nestedElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
            fprintf(stdout, "%d\n", (int) kStarchTrue);
            return;
        }
    }

    fprintf(stdout, "%d\n", (int) kStarchFalse);
#endif
}

void
UNSTARCH_printSignature(const Metadata *md, const char *chr, const unsigned char *mdSha1Buffer) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printSignature() ---\n");
#endif
#ifdef __cplusplus
    char *signature = nullptr;
#else
    char *signature = NULL;
#endif

    if (strcmp(chr, "all") == 0) {
        UNSTARCH_printAllSignatures(md, mdSha1Buffer);
    }
    else {
        signature = UNSTARCH_signatureForChromosome(md, chr);
        if (signature)
            fprintf(stdout, "%s\n", signature);
    }
}

char *
UNSTARCH_signatureForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_signatureForChromosome() ---\n");
#endif
    const Metadata *iter;
    
#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
        if ((strcmp(chr, iter->chromosome) == 0) && (iter->signature) && (strlen(iter->signature) > 0))
            return iter->signature;
    }
    return nullptr;
#else
    for (iter = md; iter != NULL; iter = iter->next) {
        if ((strcmp(chr, iter->chromosome) == 0) && (iter->signature) && (strlen(iter->signature) > 0))
            return iter->signature;
    }
    return NULL;
#endif
}

void
UNSTARCH_printAllSignatures(const Metadata *md, const unsigned char *mdSha1Buffer)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printAllSignatures() ---\n");
#endif
    UNSTARCH_printMetadataSignature(mdSha1Buffer);
    UNSTARCH_printAllChromosomeSignatures(md);
}

void
UNSTARCH_printMetadataSignature(const unsigned char *mdSha1Buffer)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printMetadataSignature() ---\n");
#endif
    size_t mdSha1BufferLength = STARCH2_MD_FOOTER_SHA1_LENGTH;

#ifdef __cplusplus
    char *jsonBase64String = nullptr;

    STARCH_encodeBase64(&jsonBase64String, 
            static_cast<const size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
            const_cast<const unsigned char *>( mdSha1Buffer ), 
            static_cast<const size_t>( mdSha1BufferLength ));

    if (!jsonBase64String) {
        fprintf(stderr, "ERROR: Could not allocate space for Base64-encoded metadata string representation\n");
        exit(-1);
    }
    fprintf(stdout, "metadata\t%s\n", jsonBase64String);
    free(jsonBase64String);
    jsonBase64String = nullptr;
#else
    char *jsonBase64String = NULL;

    STARCH_encodeBase64(&jsonBase64String, 
            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
            (const unsigned char *) mdSha1Buffer, 
            (const size_t) mdSha1BufferLength);

    if (!jsonBase64String) {
        fprintf(stderr, "ERROR: Could not allocate space for Base64-encoded metadata string representation\n");
        exit(-1);
    }
    fprintf(stdout, "metadata\t%s\n", jsonBase64String);
    free(jsonBase64String);
    jsonBase64String = NULL;
#endif
}

void
UNSTARCH_printAllChromosomeSignatures(const Metadata *md)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printAllChromosomeSignatures() ---\n");
#endif
    const Metadata *iter;
    
#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        fprintf(stdout, "%s\t%s\n", iter->chromosome, strlen(iter->signature) > 0 ? iter->signature : "NA");
    }
}

Boolean
UNSTARCH_verifySignature(FILE **inFp, const Metadata *md, const uint64_t mdOffset, const char *chr, CompressionType compType)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_verifySignature() ---\n");
#endif
#ifdef __cplusplus
    char *expectedSignature = nullptr;
    char *observedSignature = nullptr;
#else
    char *expectedSignature = NULL;
    char *observedSignature = NULL;
#endif
    Boolean signaturesVerifiedFlag = kStarchFalse;

    if (strcmp(chr, "all") == 0) {
        signaturesVerifiedFlag = UNSTARCH_verifyAllSignatures(inFp, md, mdOffset, compType);
    }
    else {
        expectedSignature = UNSTARCH_signatureForChromosome(md, chr);
        if (!expectedSignature) {
            fprintf(stderr, "ERROR: Could not locate signature in metadata for specified chromosome name for purposes of verification [%s]\n", chr);
            signaturesVerifiedFlag = kStarchFalse;
        }
        else {
            observedSignature = UNSTARCH_observedSignatureForChromosome(inFp, md, mdOffset, chr, compType);
            if (!observedSignature) {
                signaturesVerifiedFlag = kStarchFalse;
            }
        }
        if ((expectedSignature) && (observedSignature)) {
            if (strcmp(observedSignature, expectedSignature) != 0) {
                fprintf(stderr, "ERROR: Specified chromosome record may be corrupt -- observed and expected signatures do not match for chromosome [%s]\n", chr);
                signaturesVerifiedFlag = kStarchFalse;
            }
            else {
                fprintf(stderr, "Expected and observed data integrity signatures match for chromosome [%s]\n", chr);
                signaturesVerifiedFlag = kStarchTrue;
            }
        }
    }
    if (observedSignature) { 
        free(observedSignature);
#ifdef __cplusplus
        observedSignature = nullptr;
#else
        observedSignature = NULL;
#endif
    }
    return signaturesVerifiedFlag;
}

char *
UNSTARCH_observedSignatureForChromosome(FILE **inFp, const Metadata *md, const uint64_t mdOffset, const char *chr, CompressionType compType) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_observedSignatureForChromosome() ---\n");
#endif

    /*
        1) Open file pointer to specified offset
        2) Extract transformed data from compressed chromosome stream
        3) Run block SHA-1 function on byte stream until end-of-stream
        4) Return Base64 encoding of SHA-1 signature
    */

#ifdef __cplusplus
    const Metadata *iter = nullptr;
    char *currentChromosome = nullptr;
    char *base64EncodedSha1Digest = nullptr;
#else
    const Metadata *iter = NULL;
    char *currentChromosome = NULL;
    char *base64EncodedSha1Digest = NULL;
#endif
    uint64_t size = 0;  
    uint64_t cumulativeSize = 0;
    unsigned char sha1Digest[STARCH2_MD_FOOTER_SHA1_LENGTH] = {0};
    struct sha1_ctx perChromosomeHashCtx;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        currentChromosome = iter->chromosome; 
        size = iter->size;
#ifdef __cplusplus
        if (STARCH_fseeko(*inFp, static_cast<off_t>( cumulativeSize + mdOffset ), SEEK_SET) != 0) {
            fprintf(stderr, "ERROR: Could not seek data in archive\n");
            return nullptr;
        }            
#else
        if (STARCH_fseeko(*inFp, (off_t) (cumulativeSize + mdOffset), SEEK_SET) != 0) {
            fprintf(stderr, "ERROR: Could not seek data in archive\n");
            return NULL;
        }            
#endif
        cumulativeSize += size;
        if (strcmp(chr, currentChromosome) == 0) {

            // depending on archive compression type (bzip2 or gzip) we set up 
            // the machinery to extract a stream of transformed data out of the 
            // compressed bytes. we run our sha1_process_bytes() call on the
            // transformed data

            // initialize hash context
            sha1_init_ctx (&perChromosomeHashCtx);

            switch (compType) {
                case kBzip2: {
                    int bzError = 0;
                    size_t bzOutputLength = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                    BZFILE *bzFp = nullptr;
                    unsigned char *bzOutput = nullptr;
                    bzFp = BZ2_bzReadOpen( &bzError, *inFp, 0, 0, nullptr, 0 ); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzcompress-init */
                    if (bzError != BZ_OK) {
                        BZ2_bzReadClose( &bzError, bzFp );
                        fprintf(stderr, "ERROR: Bzip2 data stream could not be opened\n");
                        return nullptr;
                    }
                    bzOutput = static_cast<unsigned char *>( malloc(bzOutputLength) );
#else
                    BZFILE *bzFp = NULL;
                    unsigned char *bzOutput = NULL;
                    bzFp = BZ2_bzReadOpen( &bzError, *inFp, 0, 0, NULL, 0 ); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzcompress-init */
                    if (bzError != BZ_OK) {
                        BZ2_bzReadClose( &bzError, bzFp );
                        fprintf(stderr, "ERROR: Bzip2 data stream could not be opened\n");
                        return NULL;
                    }
                    bzOutput = malloc(bzOutputLength);
#endif
                    do {
                        UNSTARCH_bzReadLine(bzFp, &bzOutput); 
                        if (bzOutput) {
                            /*
                                The output of UNSTARCH_bzReadLine strips the newline character, because 
                                the transformation tokens do not need a newline when being turned back 
                                into raw BED. 

                                When the raw BED was originally turned into tranform tokens, the so-called
                                "transformation" buffer contained these newline characters. So we put them
                                back in the bzOutput buffer and add one byte to the string length. 

                                This modified buffer is what goes into sha1_process_bytes().
                            */
#ifdef __cplusplus
                            size_t len = strlen(reinterpret_cast<const char *>( bzOutput ));
#else
                            size_t len = strlen((const char *)bzOutput);
#endif
                            bzOutput[len] = '\n';
                            bzOutput[++len] = '\0';
                            sha1_process_bytes( bzOutput, len, &perChromosomeHashCtx );
                        }
#ifdef __cplusplus
                    } while (bzOutput != nullptr);
#else
                    } while (bzOutput != NULL);
#endif
                    /* cleanup */
                    if (bzOutput)
                        free(bzOutput);
                    BZ2_bzReadClose(&bzError, bzFp);
                    break;
                }
                case kGzip: {
                    z_stream zStream;
                    unsigned int zHave, zOutBufIdx;
                    size_t zBufIdx, zBufOffset;
                    int zError;
                    unsigned char zInBuf[STARCH_Z_CHUNK];
                    unsigned char zOutBuf[STARCH_Z_CHUNK];
                    unsigned char zLineBuf[STARCH_Z_CHUNK];
#ifdef __cplusplus
                    unsigned char *zRemainderBuf = nullptr;
                    zStream.zalloc = nullptr;
                    zStream.zfree = nullptr;
                    zStream.opaque = nullptr;
                    zStream.next_in = nullptr;
#else
		    unsigned char *zRemainderBuf = NULL;
                    zStream.zalloc = Z_NULL;
                    zStream.zfree = Z_NULL;
                    zStream.opaque = Z_NULL;
                    zStream.next_in = Z_NULL;
#endif
                    zStream.avail_in = 0;

                    zError = inflateInit2(&zStream, (15+32)); /* cf. http://www.zlib.net/manual.html */
                    if (zError != Z_OK) {
                        fprintf(stderr, "ERROR: Could not initialize z-stream\n");
#ifdef __cplusplus
                        return nullptr;
#else
                        return NULL;
#endif
                    }

#ifdef __cplusplus
                    zRemainderBuf = static_cast<unsigned char *>( malloc(1) );
#else
                    zRemainderBuf = (unsigned char *) malloc(1);
#endif
                    *zRemainderBuf = '\0';

                    do {
#ifdef __cplusplus
                        zStream.avail_in = static_cast<unsigned int>( fread(zInBuf, 1, STARCH_Z_CHUNK, *inFp) );
#else
                        zStream.avail_in = (unsigned int) fread(zInBuf, 1, STARCH_Z_CHUNK, *inFp);
#endif
                        if (zStream.avail_in == 0)
                            break;
                        zStream.next_in = zInBuf;
                        do {
                            zStream.avail_out = STARCH_Z_CHUNK;
                            zStream.next_out = zOutBuf;
                            zError = inflate(&zStream, Z_NO_FLUSH);
                            switch (zError) {
#ifdef __cplusplus
                                case Z_NEED_DICT:  { fprintf(stderr, "ERROR: Z-stream needs dictionary\n");      return nullptr; }
                                case Z_DATA_ERROR: { fprintf(stderr, "ERROR: Z-stream suffered data error\n");   return nullptr; }
                                case Z_MEM_ERROR:  { fprintf(stderr, "ERROR: Z-stream suffered memory error\n"); return nullptr; }
#else
                                case Z_NEED_DICT:  { fprintf(stderr, "ERROR: Z-stream needs dictionary\n");      return NULL; }
                                case Z_DATA_ERROR: { fprintf(stderr, "ERROR: Z-stream suffered data error\n");   return NULL; }
                                case Z_MEM_ERROR:  { fprintf(stderr, "ERROR: Z-stream suffered memory error\n"); return NULL; }
#endif
                            };
                            zHave = STARCH_Z_CHUNK - zStream.avail_out;
                            zOutBuf[zHave] = '\0';
                            /* copy remainder buffer onto line buffer, if not NULL */
#ifdef __cplusplus
                            if (zRemainderBuf) {
                                strncpy(reinterpret_cast<char *>( zLineBuf ), reinterpret_cast<const char *>( zRemainderBuf ), strlen(reinterpret_cast<const char *>( zRemainderBuf )));
                                zBufOffset = strlen(reinterpret_cast<const char *>( zRemainderBuf ));
                            }
#else
                            if (zRemainderBuf) {    
                                strncpy((char *) zLineBuf, (const char *) zRemainderBuf, strlen((const char *) zRemainderBuf));
                                zBufOffset = strlen((const char *) zRemainderBuf);
                            }
#endif                                
                            else {
                                zBufOffset = 0;
                            }
                            /* read through zOutBuf for newlines */                    
                            for (zBufIdx = zBufOffset, zOutBufIdx = 0; zOutBufIdx < zHave; zBufIdx++, zOutBufIdx++) {
                                zLineBuf[zBufIdx] = zOutBuf[zOutBufIdx];
                                if (zLineBuf[zBufIdx] == '\n') {
                                    zLineBuf[zBufIdx + 1] = '\0';
                                    sha1_process_bytes( zLineBuf, zBufIdx + 1, &perChromosomeHashCtx );
#ifdef __cplusplus
                                    zBufIdx = static_cast<size_t>( -1 );
#else
                                    zBufIdx = (size_t) -1;
#endif                                    
                                }
                            }
#ifdef __cplusplus
                            if (strlen(reinterpret_cast<const char *>( zLineBuf )) > 0) {
                                if (strlen(reinterpret_cast<const char *>( zLineBuf )) > strlen(reinterpret_cast<const char *>( zRemainderBuf ))) {
                                    free(zRemainderBuf);
                                    zRemainderBuf = reinterpret_cast<unsigned char *>( malloc(strlen(reinterpret_cast<const char *>( zLineBuf )) * 2) );
                                }
                                strncpy(reinterpret_cast<char *>( zRemainderBuf ), reinterpret_cast<const char *>( zLineBuf ), zBufIdx);
                                zRemainderBuf[zBufIdx] = '\0';
                            }
#else
                            if (strlen((const char *) zLineBuf) > 0) {
                                if (strlen((const char *) zLineBuf) > strlen((const char *) zRemainderBuf)) {
                                    free(zRemainderBuf);
                                    zRemainderBuf = (unsigned char *) malloc(strlen((const char *) zLineBuf) * 2);
                                }
                                strncpy((char *) zRemainderBuf, (const char *) zLineBuf, zBufIdx);
                                zRemainderBuf[zBufIdx] = '\0';
                            }
#endif
                        } while (zStream.avail_out == 0);
                    } while (zError != Z_STREAM_END);

                    /* cleanup */
                    if (zRemainderBuf) {
                        free(zRemainderBuf);
#ifdef __cplusplus
                        zRemainderBuf = nullptr;
#else
                        zRemainderBuf = NULL;
#endif
                    }
                    /* close gzip stream */
                    zError = inflateEnd(&zStream);
                    if (zError != Z_OK) {
                        fprintf(stderr, "ERROR: Could not close z-stream (%d)\n", zError);
#ifdef __cplusplus
                        return nullptr;
#else
                        return NULL;
#endif
                    }
                    break;
                }
                case kUndefined: {
                    fprintf(stderr, "ERROR: Archive compression type is undefined\n");
#ifdef __cplusplus
                    return nullptr;
#else
                    return NULL;
#endif
                }
            }
            
            sha1_finish_ctx (&perChromosomeHashCtx, sha1Digest);
#ifdef __cplusplus
            STARCH_encodeBase64(&base64EncodedSha1Digest, 
                                static_cast<const size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                                const_cast<const unsigned char *>( sha1Digest ), 
                                static_cast<const size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ));
#else
            STARCH_encodeBase64(&base64EncodedSha1Digest, 
                                (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                                (const unsigned char *) sha1Digest, 
                                (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
            return base64EncodedSha1Digest;
        }
    }
    fprintf(stderr, "ERROR: Leaving UNSTARCH_observedSignatureForChromosome() without having processed chromosome [%s]\n", chr);
#ifdef __cplusplus
    return nullptr;
#else
    return NULL;
#endif
}

Boolean
UNSTARCH_verifyAllSignatures(FILE **inFp, const Metadata *md, const uint64_t mdOffset, CompressionType compType)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_verifyAllSignatures() ---\n");
#endif

#ifdef __cplusplus
    const Metadata *iter = nullptr;
#else
    const Metadata *iter = NULL;
#endif
    Boolean perChromosomeSignatureVerifiedFlag = kStarchFalse;
    Boolean allSignaturesVerifiedFlag = kStarchTrue;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        perChromosomeSignatureVerifiedFlag = UNSTARCH_verifySignature(inFp, md, mdOffset, iter->chromosome, compType);
        if (!perChromosomeSignatureVerifiedFlag) {
            allSignaturesVerifiedFlag = kStarchFalse;
        }
    }

    return allSignaturesVerifiedFlag;
}

const char *
UNSTARCH_booleanToString(const Boolean val) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_booleanToString() ---\n");
#endif
    const char *t = "true";
    const char *f = "false";

    return ((val == kStarchTrue) ? t : f);
}

int
UNSTARCH_reverseTransformCoordinates(const LineCountType lineIdx, SignedCoordType *lastPosition, SignedCoordType *lcDiff, SignedCoordType *currStart, SignedCoordType *currStop, char **currRemainder, unsigned char *lineBuf, int64_t *nLineBuf, int64_t *nLineBufPos)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformCoordinates() ---\n");
#endif
    SignedCoordType coordDiff;

    if (*currStop > *currStart)
        coordDiff = *currStop - *currStart;
    else {
        fprintf(stderr, "ERROR: BED data is corrupt at line %" PRIu64 " (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, *currStop, *currStart);
        return UNSTARCH_FATAL_ERROR;
    }

    /* offset */
    if (coordDiff != *lcDiff) {
        *lcDiff = coordDiff;
#ifdef __cplusplus
        *nLineBuf = sprintf(reinterpret_cast<char *>(lineBuf) + *nLineBufPos, "p%" PRId64 "\n", coordDiff);
#else
        *nLineBuf = sprintf((char *)lineBuf + *nLineBufPos, "p%" PRId64 "\n", coordDiff);
#endif
        if (*nLineBuf < 0) {
            fprintf(stderr, "ERROR: Could not copy reverse-transformed extracted stream buffer to line buffer.\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *nLineBufPos += *nLineBuf;
    }

    /* line + remainder */
    if (*lastPosition != 0) {
        if (*currRemainder) {
#ifdef __cplusplus
            *nLineBuf = sprintf(reinterpret_cast<char *>(lineBuf) + *nLineBufPos, "%" PRId64 "\t%s\n", (*currStart - *lastPosition), *currRemainder);
#else
            *nLineBuf = sprintf((char *)lineBuf + *nLineBufPos, "%" PRId64 "\t%s\n", (*currStart - *lastPosition), *currRemainder);
#endif
	}
        else {
#ifdef __cplusplus
            *nLineBuf = sprintf(reinterpret_cast<char *>(lineBuf) + *nLineBufPos, "%" PRId64 "\n", *currStart - *lastPosition);
#else
            *nLineBuf = sprintf((char *)lineBuf + *nLineBufPos, "%" PRId64 "\n", *currStart - *lastPosition);
#endif
	}

        if (*nLineBuf < 0) {
            fprintf(stderr, "ERROR: Could not copy reverse-transformed extracted stream buffer to line buffer.\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *nLineBufPos += *nLineBuf;
    }
    else {
        if (*currRemainder) {
#ifdef __cplusplus
            *nLineBuf = sprintf(reinterpret_cast<char *>(lineBuf) + *nLineBufPos, "%" PRId64 "\t%s\n", *currStart, *currRemainder);
#else
            *nLineBuf = sprintf((char *)lineBuf + *nLineBufPos, "%" PRId64 "\t%s\n", *currStart, *currRemainder);
#endif
	}
        else {
#ifdef __cplusplus
            *nLineBuf = sprintf(reinterpret_cast<char *>(lineBuf) + *nLineBufPos, "%" PRId64 "\n", *currStart);
#else
            *nLineBuf = sprintf((char *)lineBuf + *nLineBufPos, "%" PRId64 "\n", *currStart);
#endif
        }

        if (*nLineBuf < 0) {
            fprintf(stderr, "ERROR: Could not copy reverse-transformed extracted stream buffer to line buffer.\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *nLineBufPos += *nLineBuf;
    }
    *lastPosition = *currStop;

    return 0;
}

#ifdef __cplusplus
} // namespace starch
#endif
