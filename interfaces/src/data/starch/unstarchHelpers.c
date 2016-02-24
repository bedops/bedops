//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    unstarchHelpers.c
//=========

//
//    BEDOPS
//    Copyright (C) 2011-2016 Shane Neph, Scott Kuehn and Alex Reynolds
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
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_extractDataWithGzip() ---\n");
#endif
    const Metadata *iter;
    char *chromosome;
    uint64_t size;	
    uint64_t cumulativeSize = 0;
    SignedCoordType start, pLength, lastEnd;
    char const *all = "all";
    char firstInputToken[UNSTARCH_FIRST_TOKEN_MAX_LENGTH]; 
    char secondInputToken[UNSTARCH_SECOND_TOKEN_MAX_LENGTH];
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

    firstInputToken[0] = '\0';
    secondInputToken[0] = '\0';

    for (iter = md; iter != NULL; iter = iter->next) {
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
            zStream.zalloc = Z_NULL;
            zStream.zfree = Z_NULL;
            zStream.opaque = Z_NULL;
            zStream.avail_in = 0;
            zStream.next_in = Z_NULL;

            zError = inflateInit2(&zStream, (15+32)); /* cf. http://www.zlib.net/manual.html */
            if (zError != Z_OK) {
                fprintf(stderr, "ERROR: Could not initialize z-stream\n");
                return UNSTARCH_FATAL_ERROR;
            }

            *zRemainderBuf = '\0';
    
            /* while stream is open, read line from stream, and reverse transform */
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
                                    UNSTARCH_reverseTransformHeaderlessInput(chromosome, zLineBuf, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp):\
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
    
    if (zRemainderBuf)
        free(zRemainderBuf);

    if (!chrFound) {
        fprintf(stderr, "ERROR: Could not find specified chromosome\n");
        return UNSTARCH_FATAL_ERROR;
    }

    return 0;
}

int 
UNSTARCH_extractDataWithBzip2(FILE **inFp, FILE *outFp, const char *whichChr, const Metadata *md, const uint64_t mdOffset, const Boolean headerFlag) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_extractDataWithBzip2() ---\n");
#endif
    const Metadata *iter;
    char *chromosome;
    uint64_t size;	
    uint64_t cumulativeSize = 0;
    SignedCoordType start, pLength, lastEnd;
    char const *all = "all";
    char firstInputToken[UNSTARCH_FIRST_TOKEN_MAX_LENGTH]; 
    char secondInputToken[UNSTARCH_SECOND_TOKEN_MAX_LENGTH];
    BZFILE *bzFp;
    int bzError;
    unsigned char *bzOutput;
    size_t bzOutputLength = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
    /* unsigned char chrFound = UNSTARCH_FALSE; */

    if (!outFp)
        outFp = stdout;

    firstInputToken[0] = '\0';
    secondInputToken[0] = '\0';

    for (iter = md; iter != NULL; iter = iter->next) {

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
            fprintf(stderr, "ERROR: Could not seek data in archve\n");
            return UNSTARCH_FATAL_ERROR;
        }
        cumulativeSize += size;

        if ((strcmp(whichChr, all) == 0) || (strcmp(whichChr, chromosome) == 0)) {

            /* chrFound = UNSTARCH_TRUE; */

            bzFp = BZ2_bzReadOpen( &bzError, *inFp, 0, 0, NULL, 0 ); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzcompress-init */
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
            } while (bzOutput != NULL);
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

    return 0;
}

void 
UNSTARCH_bzReadLine(BZFILE *input, unsigned char **output) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_bzReadLine() ---\n");
#endif
    size_t offset = 0;
    size_t len = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
    int bzError;
    unsigned char *outputCopy = NULL;
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
        *output = NULL;
    }
}

int 
UNSTARCH_reverseTransformInput(const char *chr, const unsigned char *str, char delim, SignedCoordType *start, SignedCoordType *pLength, SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformInput() ---\n");
#endif
    char *pTest = NULL;
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
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ), static_cast<SignedCoordType>( *lastEnd ), elemTok2);
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), (SignedCoordType) *lastEnd, elemTok2);
#endif
            }
        }
        else {
            pTest = NULL;
#ifdef __cplusplus
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
                pTestChars = NULL;
#ifdef __cplusplus
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, NULL, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
    char *pTest = NULL;
    char *pTestChars;
    const char *pTestParam = "p";
    char *currentChrCopy = NULL;
    char *currentRemainderCopy = NULL;

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
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
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
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ) + *pLength;
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
#ifdef __cplusplus
                *currentStart = static_cast<int64_t>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
            pTest = NULL;
#ifdef __cplusplus
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
                pTestChars = NULL;
#ifdef __cplusplus
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, NULL, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
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
    char *pTest = NULL;
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
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ), *lastEnd, elemTok2);
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
#endif
            }
        }
        else {
            pTest = NULL;
#ifdef __cplusplus
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>(elemTok1), pTestParam, 1);
#else
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
                pTestChars = NULL;
#ifdef __cplusplus
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, NULL, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
    
    fprintf(stderr, "\n--- UNSTARCH_sReverseTransformIgnoringHeaderedInput() ---\n");
    /*
    fprintf(stderr, "\tchr -> %s\n", chr);
    fprintf(stderr, "\tstr -> %s\n", str);
    fprintf(stderr, "\tdelim -> %c\n", delim);
    fprintf(stderr, "\tstart -> %" PRId64 "\n", *start);
    fprintf(stderr, "\tpLength -> %" PRId64 "\n", *pLength);
    fprintf(stderr, "\tlastEnd -> %" PRId64 "\n", *lastEnd);
    fprintf(stderr, "\telemTok1 -> %s\n", elemTok1);
    fprintf(stderr, "\telemTok2 -> %s\n", elemTok2);
    fprintf(stderr, "\tcurrentChr -> %s\n", *currentChr);
    fprintf(stderr, "\tcurrentChrLen -> %zu\n", *currentChrLen);
    fprintf(stderr, "\tcurrentStart -> %" PRId64 "\n", *currentStart);
    fprintf(stderr, "\tcurrentStop -> %" PRId64 "\n", *currentStop);
    fprintf(stderr, "\tcurrentRemainder -> %s\n", *currentRemainder);
    fprintf(stderr, "\tcurrentRemainderLen -> %zu\n", *currentRemainderLen);
    */
#endif
    char pTestChars[MAX_DEC_INTEGERS] = {0};
    char *currentChrCopy = NULL;
    char *currentRemainderCopy = NULL;

    /* if *str begins with a reserved header name, then we shortcut */

    /* fprintf(stdout, "UNSTARCH_sReverseTransformIgnoringHeaderedInput - str: %s\n", str); */

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
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                fprintf(stderr, "A: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
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
                /*
                fprintf(stderr, "BEFORE currentRemainder -> [%s]\n", *currentRemainder);
                fprintf(stderr, "BEFORE elemTok2         -> [%s]\n", elemTok2);
                */
#endif
                strncpy(*currentRemainder, elemTok2, strlen(elemTok2) + 1);  
#ifdef DEBUG
                /*
                fprintf(stderr, "AFTER  currentRemainder -> [%s]\n", *currentRemainder);
                fprintf(stderr, "AFTER  elemTok2         -> [%s]\n", elemTok2);
                */
#endif
                if (!*currentRemainder) {
                    fprintf(stderr, "ERROR: Current remainder token could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ) + *pLength;
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#endif
#ifdef DEBUG
                /* fprintf(stderr, "B: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2); */
#endif
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = (char *) malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if (strlen(chr) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
                }
                /* strncpy(*currentChr, chr, strlen(chr) + 1); */
                if (! *currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
#ifdef __cplusplus
                *currentStart = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
                *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                *currentChr = malloc(strlen(chr) + 1);
#endif
                if (! *currentChr) {
                    fprintf(stderr, "ERROR: Ran out of memory while allocating chr token\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                *currentChrLen = strlen(chr) + 1;
            }
            else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                if (!currentChrCopy) {
                    fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                *currentChr = currentChrCopy;
                *currentChrLen = strlen(chr) * 2;
            }
            strncpy(*currentChr, chr, strlen(chr) + 1);
            if (! *currentChr) {
                fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                return UNSTARCH_FATAL_ERROR;                
            }

            if (elemTok1[0] == 'p') {
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, NULL, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                /* fprintf(stderr, "D: %s\t%" PRId64 "\t%" PRId64 "\n", chr, *start, *lastEnd); */
#endif
                *currentStart = *start;
                *currentStop = *lastEnd;
            }
        }
    }
    else {
        fprintf(stderr, "ERROR: Compressed data stream could not be transformed\n");
        return UNSTARCH_FATAL_ERROR;
    }

#ifdef DEBUG
    /* fprintf(stderr, "\n--- leaving UNSTARCH_sReverseTransformIgnoringHeaderedInput() ---\n"); */
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
    char *pTest = NULL;
    char *pTestChars;
    const char *pTestParam = "p";

    if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) 
    { 
        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
            }
            else {
#ifdef __cplusplus
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ) + *pLength;
		fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ), *lastEnd, elemTok2);
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
		fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
#endif
            }
        }
        else {
            pTest = NULL;
#ifdef __cplusplus
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
                pTestChars = NULL;
#ifdef __cplusplus
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, NULL, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
    /* fprintf(stderr, "\n--- UNSTARCH_extractRawLine() ---\n"); */
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
        if (res != 0)
            res = UNSTARCH_sReverseTransformIgnoringHeaderedInput(chr, str, delim, &(*start), &(*pLength), &(*lastEnd), elemTok1, elemTok2, currentChr, &(*currentChrLen), &(*currentStart), &(*currentStop), &(*currentRemainder), &(*currentRemainderLen));

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
    char *pTest = NULL;
    char *pTestChars;
    const char *pTestParam = "p";
    char *currentChrCopy = NULL;
    char *currentRemainderCopy = NULL;

    if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) 
    { 
        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                //sprintf(out, "%s\t%lld\t%lld\t%s\n", chr, *start, *lastEnd, elemTok2);
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
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
                *lastEnd = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) ) + *pLength;
#else
                *lastEnd = (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#endif
                //sprintf(out, "%s\t%lld\t%lld\t%s\n", chr, (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
                }
                strncpy(*currentChr, chr, strlen(chr) + 1);
                if (!*currentChr) {
                    fprintf(stderr, "ERROR: Current chromosome name could not be copied\n");
                    return UNSTARCH_FATAL_ERROR;                
                }
#ifdef __cplusplus
                *currentStart = static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
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
            pTest = NULL;
#ifdef __cplusplus
            pTest = UNSTARCH_strnstr(reinterpret_cast<const char *>( elemTok1 ), pTestParam, 1);
#else
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
#endif
            if (pTest) {
                pTestChars = NULL;
#ifdef __cplusplus
                pTestChars = static_cast<char *>( malloc(strlen(elemTok1)) );
#else
                pTestChars = malloc(strlen(elemTok1));
#endif
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
#ifdef __cplusplus
                *pLength = static_cast<SignedCoordType>( strtoull(pTestChars, NULL, UNSTARCH_RADIX) );
#else
                *pLength = (SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
#endif
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
#ifdef __cplusplus
                *start = *lastEnd + static_cast<SignedCoordType>( strtoull(elemTok1, NULL, UNSTARCH_RADIX) );
#else
                *start = *lastEnd + (SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
#endif
                *lastEnd = *start + *pLength;
                //sprintf(out, "%s\t%lld\t%lld\n", chr, *start, *lastEnd);
                if (! *currentChr) {
#ifdef __cplusplus
                    *currentChr = static_cast<char *>( malloc(strlen(chr) + 1) );
#else
                    *currentChr = malloc(strlen(chr) + 1);
#endif
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
#ifdef __cplusplus
                    currentChrCopy = static_cast<char *>( realloc(*currentChr, strlen(chr) * 2) );
#else
                    currentChrCopy = realloc(*currentChr, strlen(chr) * 2);
#endif
                    if (!currentChrCopy) {
                        fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                        return UNSTARCH_FATAL_ERROR;
                    }
                    *currentChr = currentChrCopy;
                    *currentChrLen = strlen(chr) * 2;
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
    fprintf(stderr, "\n--- UNSTARCH_createInverseTransformTokens() ---\n");
#endif
    int charCnt, sCnt, elemCnt;
    unsigned char buffer[UNSTARCH_BUFFER_MAX_LENGTH];

    charCnt = 0;
    sCnt = 0;
    elemCnt = 0;
	
    do {
        buffer[charCnt++] = s[sCnt];
        if (buffer[(charCnt - 1)] == delim) {
            if (elemCnt == 0) { 
                buffer[(charCnt - 1)] = '\0';
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
        buffer[charCnt] = '\0';
#ifdef __cplusplus
        strncpy(elemTok1, reinterpret_cast<const char *>( buffer ), strlen(reinterpret_cast<const char *>( buffer )) + 1);
#else
        strncpy(elemTok1, (const char *) buffer, strlen((const char *) buffer) + 1);
#endif
    }
    if (elemCnt == 1) {
        buffer[charCnt] = '\0';
#ifdef __cplusplus
        strncpy(elemTok2, reinterpret_cast<const char *>( buffer ), strlen(reinterpret_cast<const char *>( buffer )) + 1);
#else
        strncpy(elemTok2, (const char *) buffer, strlen((const char *) buffer) + 1);
#endif
    }

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
    for (p = const_cast<char *>( haystack ); p != NULL; p = static_cast<char *>( memchr(p + 1, *needle, pLen-1) )) {
#else
    for (p = (char *) haystack; p != NULL; p = (char *) memchr(p + 1, *needle, pLen-1)) {
#endif
#ifdef __cplusplus
        pLen = haystackLen - static_cast<size_t>( p - const_cast<char *>( haystack ) );
#else
        pLen = haystackLen - (size_t) (p - haystack);
#endif
        if (pLen < len) 
            return NULL;
        if (strncmp(p, needle, len) == 0)
            return p;
    }

    return NULL;
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
#else
    result = malloc(len + 1);
#endif

    if (!result)
        return 0;

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

    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next)
        totalLineCount += iter->lineCount;

    fprintf(stdout, "%" PRIu64 "\n", totalLineCount);
}

BaseCountType
UNSTARCH_nonUniqueBaseCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_nonUniqueBaseCountForChromosome() ---\n");
#endif
    const Metadata *iter;

    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next)
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

    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next)
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
    
    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next) {
        if (UNSTARCH_duplicateElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
#ifdef __cplusplus
            fprintf(stdout, "%d\n", static_cast<int>( kStarchTrue ));
#else
            fprintf(stdout, "%d\n", (int) kStarchTrue);
#endif
            return;
        }
    }

#ifdef __cplusplus
    fprintf(stdout, "%d\n", static_cast<int>( kStarchFalse ));
#else
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
    
    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next) {
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

    for (iter = md; iter != NULL; iter = iter->next) {
        if (UNSTARCH_nestedElementExistsForChromosome(md, iter->chromosome) == kStarchTrue) {
#ifdef __cplusplus
            fprintf(stdout, "%d\n", static_cast<int>( kStarchTrue ));
#else
            fprintf(stdout, "%d\n", (int) kStarchTrue);
#endif
            return;
        }
    }

#ifdef __cplusplus
    fprintf(stdout, "%d\n", static_cast<int>( kStarchFalse ));
#else
    fprintf(stdout, "%d\n", (int) kStarchFalse);
#endif
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
