//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    unstarchHelpers.c
//=========

#ifdef __cplusplus
#include <cinttypes>
#include <cstdint>
#else
#include <inttypes.h>
#include <stdint.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <zlib.h>

#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchHelpers.h"
#include "data/starch/unstarchHelpers.h"
#include "data/starch/starchConstants.h"
#include "suite/BEDOPS.Constants.hpp"

int 
UNSTARCH_extractDataWithGzip(FILE **inFp, FILE *outFp, const char *whichChr, const Metadata *md, const uint64_t mdOffset, const Boolean headerFlag) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_extractDataWithGzip() ---\n");
#endif
    const Metadata *iter;
    char *chromosome;
    uint64_t size;	
    uint64_t cumulativeSize = UINT64_C(0);
    Bed::SignedCoordType start, pLength, lastEnd;
    char const *all = "all";
    char firstInputToken[UNSTARCH_FIRST_TOKEN_MAX_LENGTH]; 
    char secondInputToken[UNSTARCH_SECOND_TOKEN_MAX_LENGTH];
    unsigned char zInBuf[STARCH_Z_CHUNK];
    unsigned char zOutBuf[STARCH_Z_CHUNK];
    char zLineBuf[STARCH_Z_CHUNK];
    char *zRemainderBuf = (char*)malloc(1); 
    z_stream zStream;
    unsigned int zHave, zBufIdx, zBufOffset, zOutBufIdx;
    int zError;
    Boolean chrFound = kStarchFalse;

    if (!outFp)
        outFp = stdout;

    firstInputToken[0] = '\0';
    secondInputToken[0] = '\0';

    for (iter = md; iter != NULL; iter = iter->next) {
        chromosome = iter->chromosome; 
        size = iter->size;
        start = INT64_C(0);
        pLength = INT64_C(0);
        lastEnd = INT64_C(0);

        if (STARCH_fseeko(*inFp, (off_t)(cumulativeSize + mdOffset), SEEK_SET) != 0) {
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
                zStream.avail_in = fread(zInBuf, 1, STARCH_Z_CHUNK, *inFp);
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
                    }
                    zHave = STARCH_Z_CHUNK - zStream.avail_out;
                    zOutBuf[zHave] = '\0';

                    /* copy remainder buffer onto line buffer, if not NULL */
                    if (zRemainderBuf) {
                        strncpy(zLineBuf, zRemainderBuf, strlen(zRemainderBuf));
                        zBufOffset = strlen(zRemainderBuf);
                    }
                    else 
                        zBufOffset = 0;

                    /* read through zOutBuf for newlines */                    
                    for (zBufIdx = zBufOffset, zOutBufIdx = 0; zOutBufIdx < zHave; zBufIdx++, zOutBufIdx++) {
                        zLineBuf[zBufIdx] = zOutBuf[zOutBufIdx];
                        if (zLineBuf[zBufIdx] == '\n') {
                            zLineBuf[zBufIdx] = '\0';
                            zBufIdx = -1;
                            (!headerFlag) ? \
                                    UNSTARCH_reverseTransformHeaderlessInput(chromosome, zLineBuf, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp):\
                                    UNSTARCH_reverseTransformInput(chromosome, zLineBuf, '\t', &start, &pLength, &lastEnd, firstInputToken, secondInputToken, outFp);
                            firstInputToken[0] = '\0';
                            secondInputToken[0] = '\0';
                        }
                    }

                    /* copy some of line buffer onto the remainder buffer, if there are remnants from the z-stream */
                    if (strlen(zLineBuf) > 0) {

                        if (strlen(zLineBuf) > strlen(zRemainderBuf)) {
                            /* to minimize the chance of doing another (expensive) malloc, we double the length of zRemainderBuf */
                            free(zRemainderBuf);
                            zRemainderBuf = (char *) malloc(strlen(zLineBuf) * 2);
                        }

                        /* it is necessary to copy only that part of zLineBuf up to zBufIdx characters  */
                        /* (zBufIdx characters were read into zLineBuf before no newline was found and  */
                        /* we end up in this conditional) as well as terminate the remainder buffer     */
                        /* zRemainderBuf, so that any cruft from a previous iteration is ignored in the */
                        /* next iteration of parsing the chromosome's z-stream                          */

                        strncpy(zRemainderBuf, zLineBuf, zBufIdx);
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
    uint64_t cumulativeSize = UINT64_C(0);
    Bed::SignedCoordType start, pLength, lastEnd;
    char const *all = "all";
    char firstInputToken[UNSTARCH_FIRST_TOKEN_MAX_LENGTH]; 
    char secondInputToken[UNSTARCH_SECOND_TOKEN_MAX_LENGTH];
    BZFILE *bzFp;
    int bzError;
    char *bzOutput;
    size_t bzOutputLength = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
    /* unsigned char chrFound = UNSTARCH_FALSE; */

    if (!outFp)
        outFp = stdout;

    firstInputToken[0] = '\0';
    secondInputToken[0] = '\0';

    for (iter = md; iter != NULL; iter = iter->next) {

        chromosome = iter->chromosome; 
        size = iter->size;
        start = INT64_C(0);
        pLength = INT64_C(0);
        lastEnd = INT64_C(0);

        if (STARCH_fseeko(*inFp, (off_t) (cumulativeSize + mdOffset), SEEK_SET) != 0) {
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

            bzOutput = (char *) malloc(bzOutputLength);
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
UNSTARCH_bzReadLine(BZFILE *input, char **output) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_bzReadLine() ---\n");
#endif
    size_t offset = 0;
    size_t len = UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH;
    int bzError;
    char *outputCopy = NULL;
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
            outputCopy = (char *) realloc(*output, len);
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
UNSTARCH_reverseTransformInput(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformInput() ---\n");
#endif
    char *pTest = NULL;
    char *pTestChars;
    const char *pTestParam = "p";

    /* if *str begins with a reserved header name, then we
       shortcut and print the line without transformation */

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

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
                *start = *lastEnd + strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (Bed::SignedCoordType) *start, (Bed::SignedCoordType) *lastEnd, elemTok2);
            }
            else {
                *lastEnd = strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), (Bed::SignedCoordType) *lastEnd, elemTok2);
            }
        }
        else {
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
            if (pTest) {
                pTestChars = NULL;
                pTestChars = (char *) malloc(strlen(elemTok1));
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
                *pLength = (Bed::SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
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
UNSTARCH_sReverseTransformInput(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, Bed::SignedCoordType *currentStart, Bed::SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
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

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                    *currentRemainder = (char *) malloc(strlen(elemTok2) + 1);
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
                    currentRemainderCopy = (char *) realloc(*currentRemainder, strlen(elemTok2) * 2);
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
                *lastEnd = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                *currentStart = (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
                    *currentRemainder = (char *) malloc(strlen(elemTok2) + 1);
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
                    currentRemainderCopy = (char *) realloc(*currentRemainder, strlen(elemTok2) * 2);
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
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
            if (pTest) {
                pTestChars = NULL;
                pTestChars = (char *) malloc(strlen(elemTok1));
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
                *pLength = (Bed::SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
UNSTARCH_reverseTransformIgnoringHeaderedInput(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformIgnoringHeaderedInput() ---\n");
#endif
    char *pTest = NULL;
    char *pTestChars;
    const char *pTestParam = "p";

    /* if *str begins with a reserved header name, then we
       shortcut and do nothing */

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

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
            }
            else {
                *lastEnd = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
            }
        }
        else {
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
            if (pTest) {
                pTestChars = NULL;
                pTestChars = (char *) malloc(strlen(elemTok1));
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
                *pLength = (Bed::SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
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
UNSTARCH_sReverseTransformIgnoringHeaderedInput(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, Bed::SignedCoordType *currentStart, Bed::SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
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
    char pTestChars[Bed::MAX_DEC_INTEGERS] = {0};
    char *currentChrCopy = NULL;
    char *currentRemainderCopy = NULL;

    /* if *str begins with a reserved header name, then we shortcut */

    /* fprintf(stdout, "UNSTARCH_sReverseTransformIgnoringHeaderedInput - str: %s\n", str); */

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

    /* otherwise, we transform *str back into printable tokens */

    else if (UNSTARCH_createInverseTransformTokens(str, delim, elemTok1, elemTok2) == 0) { 

        if (elemTok2[0] != '\0') {
            if (*lastEnd > 0) {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
#ifdef DEBUG
                fprintf(stderr, "A: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
#endif
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                    *currentRemainder = (char *) malloc(strlen(elemTok2) + 1);
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
                    currentRemainderCopy = (char *) realloc(*currentRemainder, strlen(elemTok2) * 2);
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
                *lastEnd = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
#ifdef DEBUG
                /* fprintf(stderr, "B: %s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2); */
#endif
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if (strlen(chr) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                *currentStart = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
                    *currentRemainder = (char *) malloc(strlen(elemTok2) + 1);
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
                    currentRemainderCopy = (char *) realloc(*currentRemainder, strlen(elemTok2) * 2);
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
                *currentChr = (char *) malloc(strlen(chr) + 1);
                if (! *currentChr) {
                    fprintf(stderr, "ERROR: Ran out of memory while allocating chr token\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                *currentChrLen = strlen(chr) + 1;
            }
            else if ((strlen(chr) + 1) > *currentChrLen) {
                currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                *pLength = (Bed::SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
            }
            else {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
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
UNSTARCH_reverseTransformHeaderlessInput(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, FILE *outFp) 
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
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, *start, *lastEnd, elemTok2);
            }
            else {
                *lastEnd = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                fprintf(outFp, "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", chr, (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
            }
        }
        else {
            pTest = NULL;
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
            if (pTest) {
                pTestChars = NULL;
                pTestChars = (char *) malloc(strlen(elemTok1));
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
                *pLength = (Bed::SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
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
UNSTARCH_extractRawLine(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, Bed::SignedCoordType *currentStart, Bed::SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
{
    /*
        UNSTARCH_extractRawLine() takes in a buffer of post-transform BED data (which
        could come from a bzip2 or gzip stream that gets extracted from a starch file)
        and reverse transforms portions of a BED element to several variables passed 
        in by pointers.

        The following variables are "read-only" inputs:

            char     *chr                -> chromosome (obtained from metadata record)
            char     *str                -> buffer containing post-transform data to reverse transform
            char      delim              -> field delimiter (shoud usually be a tab character)

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
UNSTARCH_sReverseTransformHeaderlessInput(const char *chr, const char *str, char delim, Bed::SignedCoordType *start, Bed::SignedCoordType *pLength, Bed::SignedCoordType *lastEnd, char *elemTok1, char *elemTok2, char **currentChr, size_t *currentChrLen, Bed::SignedCoordType *currentStart, Bed::SignedCoordType *currentStop, char **currentRemainder, size_t *currentRemainderLen)
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
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                //sprintf(out, "%s\t%lld\t%lld\t%s\n", chr, *start, *lastEnd, elemTok2);
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                    *currentRemainder = (char *) malloc(strlen(elemTok2) + 1);
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
                    currentRemainderCopy = (char *) realloc(*currentRemainder, strlen(elemTok2) * 2);
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
                *lastEnd = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX) + *pLength;
                //sprintf(out, "%s\t%lld\t%lld\t%s\n", chr, (int64_t) strtoull(elemTok1, NULL, UNSTARCH_RADIX), *lastEnd, elemTok2);
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
                *currentStart = (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *currentStop = *lastEnd;
                if (! *currentRemainder) {
                    *currentRemainder = (char *) malloc(strlen(elemTok2) + 1);
                    *currentRemainderLen = strlen(elemTok2) + 1;
                }
                else if ((strlen(elemTok2) + 1) > *currentRemainderLen) {
                    currentRemainderCopy = (char *) realloc(*currentRemainder, strlen(elemTok2) * 2);
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
            pTest = UNSTARCH_strnstr((const char *)elemTok1, pTestParam, 1);
            if (pTest) {
                pTestChars = NULL;
                pTestChars = (char *) malloc(strlen(elemTok1));
                strncpy(pTestChars, elemTok1 + 1, strlen(elemTok1));
                if (!pTestChars)
                    return UNSTARCH_FATAL_ERROR;
                *pLength = (Bed::SignedCoordType) strtoull(pTestChars, NULL, UNSTARCH_RADIX);
                free(pTestChars); 
                pTestChars = NULL;
            }
            else {
                *start = *lastEnd + (Bed::SignedCoordType) strtoull(elemTok1, NULL, UNSTARCH_RADIX);
                *lastEnd = *start + *pLength;
                //sprintf(out, "%s\t%lld\t%lld\n", chr, *start, *lastEnd);
                if (! *currentChr) {
                    *currentChr = (char *) malloc(strlen(chr) + 1);
                    *currentChrLen = strlen(chr) + 1;
                }
                else if ((strlen(chr) + 1) > *currentChrLen) {
                    currentChrCopy = (char *) realloc(*currentChr, strlen(chr) * 2);
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
UNSTARCH_createInverseTransformTokens(const char *s, const char delim, char elemTok1[], char elemTok2[]) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_createInverseTransformTokens() ---\n");
#endif
    int charCnt, sCnt, elemCnt;
    char buffer[UNSTARCH_BUFFER_MAX_LENGTH];

    charCnt = 0;
    sCnt = 0;
    elemCnt = 0;
	
    do {
        buffer[charCnt++] = s[sCnt];
        if (buffer[(charCnt - 1)] == delim) {
            if (elemCnt == 0) { 
                buffer[(charCnt - 1)] = '\0';
                strncpy(elemTok1, (const char *)buffer, strlen(buffer) + 1); 
                elemCnt++; 
                charCnt = 0;
            }
        }
    } while (s[sCnt++] != 0);

    if (elemCnt == 0) {
        buffer[charCnt] = '\0';
        strncpy(elemTok1, (const char *)buffer, strlen(buffer) + 1);
    }
    if (elemCnt == 1) {
        buffer[charCnt] = '\0';
        strncpy(elemTok2, (const char *)buffer, strlen(buffer) + 1);
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

    if (*needle == '\0')    /* everything matches empty string */
        return (char *) haystack;

    pLen = haystackLen;
    for (p = (char *) haystack; p != NULL; p = (char *) memchr(p + 1, *needle, pLen-1)) {
        pLen = haystackLen - (p - haystack);
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

    result = (char *) malloc(len + 1);
    if (!result)
        return 0;

    result[len] = '\0';
    return (char *) memcpy (result, s, len);
}

Bed::LineCountType
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

    return (Bed::LineCountType) UINT64_C(0);
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
    Bed::LineCountType totalLineCount = UINT64_C(0);

    for (iter = md; iter != NULL; iter = iter->next)
        totalLineCount += iter->lineCount;

    fprintf(stdout, "%" PRIu64 "\n", totalLineCount);
}

Bed::BaseCountType
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

    return (Bed::BaseCountType) UINT64_C(0);
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
    Bed::BaseCountType totalBaseCount = UINT64_C(0);

    for (iter = md; iter != NULL; iter = iter->next)
        totalBaseCount += iter->totalNonUniqueBases;

    fprintf(stdout, "%" PRIu64 "\n", totalBaseCount);
}

Bed::BaseCountType
UNSTARCH_uniqueBaseCountForChromosome(const Metadata *md, const char *chr)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_uniqueBaseCountForChromosome() ---\n");
#endif
    const Metadata *iter;

    for (iter = md; iter != NULL; iter = iter->next) {
        if (strcmp(chr, iter->chromosome) == 0) {
            return iter->totalUniqueBases;
        }
    }

    return (Bed::BaseCountType) UINT64_C(0);
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
    fprintf(stderr, "\n--- UNSTARCH_printUniqueBaseCountForChromosome() ---\n");
#endif
    const Metadata *iter;
    Bed::BaseCountType totalBaseCount = UINT64_C(0);

    for (iter = md; iter != NULL; iter = iter->next)
        totalBaseCount += iter->totalUniqueBases;

    fprintf(stdout, "%" PRIu64 "\n", totalBaseCount);
}

int
UNSTARCH_reverseTransformCoordinates(const Bed::LineCountType lineIdx, Bed::SignedCoordType *lastPosition, Bed::SignedCoordType *lcDiff, Bed::SignedCoordType *currStart, Bed::SignedCoordType *currStop, char **currRemainder, char *lineBuf, int64_t *nLineBuf, int64_t *nLineBufPos)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_reverseTransformCoordinates() ---\n");
#endif
    Bed::SignedCoordType coordDiff;

    if (*currStop > *currStart)
        coordDiff = *currStop - *currStart;
    else {
        fprintf(stderr, "ERROR: BED data is corrupt at line %" PRIu64 " (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, *currStop, *currStart);
        return UNSTARCH_FATAL_ERROR;
    }

    /* offset */
    if (coordDiff != *lcDiff) {
        *lcDiff = coordDiff;
        *nLineBuf = sprintf(lineBuf + *nLineBufPos, "p%" PRId64 "\n", coordDiff);
        if (*nLineBuf < 0) {
            fprintf(stderr, "ERROR: Could not copy reverse-transformed extracted stream buffer to line buffer.\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *nLineBufPos += *nLineBuf;
    }

    /* line + remainder */
    if (*lastPosition != 0) {
        if (*currRemainder)
            *nLineBuf = sprintf(lineBuf + *nLineBufPos, "%" PRId64 "\t%s\n", (*currStart - *lastPosition), *currRemainder);
        else
            *nLineBuf = sprintf(lineBuf + *nLineBufPos, "%" PRId64 "\n", *currStart - *lastPosition);

        if (*nLineBuf < 0) {
            fprintf(stderr, "ERROR: Could not copy reverse-transformed extracted stream buffer to line buffer.\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *nLineBufPos += *nLineBuf;
    }
    else {
        if (*currRemainder)
            *nLineBuf = sprintf(lineBuf + *nLineBufPos, "%" PRId64 "\t%s\n", *currStart, *currRemainder);
        else
            *nLineBuf = sprintf(lineBuf + *nLineBufPos, "%" PRId64 "\n", *currStart);

        if (*nLineBuf < 0) {
            fprintf(stderr, "ERROR: Could not copy reverse-transformed extracted stream buffer to line buffer.\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *nLineBufPos += *nLineBuf;
    }
    *lastPosition = *currStop;

    return 0;
}
