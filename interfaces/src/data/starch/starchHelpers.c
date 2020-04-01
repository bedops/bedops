//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchHelpers.c
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
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bzlib.h>
#include <zlib.h>

#include "data/starch/starchSha1Digest.h"
#include "data/starch/starchBase64Coding.h"
#include "data/starch/starchHelpers.h"
#include "data/starch/starchConstants.h"
#include "data/starch/starchFileHelpers.h"
#include "suite/BEDOPS.Constants.hpp"

#ifdef __cplusplus
namespace starch {
    using namespace Bed;
#endif
    
char *
STARCH_strdup(const char *str)
{
  /* Cygwin does not include support for strdup() so we include our own implementation here */
#ifdef __cplusplus
    char *dup = nullptr;
    if (str) {
        dup = static_cast<char *>( malloc(strlen(str) + 1) ); /* sizeof(char) = 1, of course */
#else
    char *dup = NULL;
    if (str) {
        dup = malloc(strlen(str) + 1); /* sizeof(char) = 1, of course */
#endif
        if (!dup) {
            fprintf(stderr, "ERROR: Out of memory\n");
            exit(EXIT_FAILURE);
        }
        strcpy(dup, str);
    }
    return dup;
}

int 
STARCH_compressFileWithGzip(const char *inFn, char **outFn, off_t *outFnSize)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_compressFileWithGzip() ---\n");
#endif
#ifdef __cplusplus
    FILE *inFnPtr = nullptr;
    FILE *outFnPtr = nullptr;
#else
    FILE *inFnPtr = NULL;
    FILE *outFnPtr = NULL;
#endif
    struct stat outSt;

    /* create output file handle */
#ifdef __cplusplus
    *outFn = static_cast<char *>( malloc((strlen(inFn) + 4) * sizeof(**outFn)) ); /* 4 <- ".gz\0" */
#else
    *outFn = malloc((strlen(inFn) + 4) * sizeof(**outFn)); /* 4 <- ".gz\0" */
#endif
    if (! *outFn) {
        fprintf(stderr, "ERROR: Out of memory\n");
        return STARCH_FATAL_ERROR;
    }
    sprintf(*outFn, "%s.gz", inFn);
    outFnPtr = STARCH_fopen(*outFn, "wb");
    if (!outFnPtr) {
        fprintf(stderr, "ERROR: Could not open a gzip output file handle to %s\n", *outFn);
        return STARCH_FATAL_ERROR;
    }

    /* open input for compression */
    inFnPtr = STARCH_fopen(inFn, "r");
    if (!inFnPtr) {
        fprintf(stderr, "ERROR: Could not open a gzip input file handle to %s\n", inFn);
        return STARCH_FATAL_ERROR;
    }

    /* compress file */
    /* cf. http://www.zlib.net/manual.html for level information */
    STARCH_gzip_deflate(inFnPtr, outFnPtr, STARCH_Z_COMPRESSION_LEVEL);

    /* close file pointers */
    fclose(inFnPtr);
    fclose(outFnPtr);

    /* get gzip file size */
#ifdef __cplusplus
    if (stat(reinterpret_cast<const char *>( *outFn ), &outSt) != 0) {
#else
    if (stat((const char *)*outFn, &outSt) != 0) {
#endif
        fprintf(stderr, "ERROR: Could not get gzip file attributes\n");
        return STARCH_FATAL_ERROR;
    }
    *outFnSize = outSt.st_size;

    return 0;
}

int 
STARCH_compressFileWithBzip2(const char *inFn, char **outFn, off_t *outFnSize)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_compressFileWithBzip2() ---\n");
#endif
#ifdef __cplusplus
    FILE *inFnPtr = nullptr;
    FILE *outFnPtr = nullptr;
    BZFILE *bzFp = nullptr;
#else
    FILE *inFnPtr = NULL;
    FILE *outFnPtr = NULL;
    BZFILE *bzFp = NULL;
#endif
    int nBzBuf = STARCH_BZ_BUFFER_MAX_LENGTH;
    char bzBuf[STARCH_BZ_BUFFER_MAX_LENGTH];
    int bzError;
    int c;
    unsigned int idx = 0U;
    struct stat outSt;

    /* create output file handle */
#ifdef __cplusplus
    *outFn = static_cast<char *>( malloc((strlen(inFn) + 5) * sizeof(**outFn)) ); /* 5 <- ".bz2\0" */
#else
    *outFn = malloc((strlen(inFn) + 5) * sizeof(**outFn)); /* 5 <- ".bz2\0" */
#endif
    if (! *outFn) {
        fprintf(stderr, "ERROR: Out of memory\n");
        return STARCH_FATAL_ERROR;
    }
    sprintf(*outFn, "%s.bz2", inFn);
    outFnPtr = STARCH_fopen(*outFn, "wb");
    if (!outFnPtr) {
        fprintf(stderr, "ERROR: Could not open a bzip2 output file handle to %s\n", *outFn);
        return STARCH_FATAL_ERROR;
    }

    /* open input for compression */
    inFnPtr = STARCH_fopen(inFn, "r");
    if (!inFnPtr) {
        fprintf(stderr, "ERROR: Could not open a bzip2 input file handle to %s\n", inFn);
        return STARCH_FATAL_ERROR;
    }
    bzFp = BZ2_bzWriteOpen( &bzError, outFnPtr, STARCH_BZ_COMPRESSION_LEVEL, 0, 0 );
    if (bzError != BZ_OK) {
#ifdef __cplusplus
        BZ2_bzWriteClose ( &bzError, bzFp, 0, nullptr, nullptr );
#else
        BZ2_bzWriteClose ( &bzError, bzFp, 0, NULL, NULL );
#endif
        fprintf(stderr, "ERROR: Could not open bzip2 file handle\n");
        return STARCH_FATAL_ERROR;
    }

    /* compress to bz stream */
    while ((c = fgetc(inFnPtr)) != EOF) { 
#ifdef __cplusplus
        bzBuf[idx++] = static_cast<char>( c );
#else
        bzBuf[idx++] = (char) c;
#endif
        if (idx == STARCH_BZ_BUFFER_MAX_LENGTH) {
            BZ2_bzWrite( &bzError, bzFp, bzBuf, nBzBuf );
            if (bzError == BZ_IO_ERROR) {
#ifdef __cplusplus
                BZ2_bzWriteClose ( &bzError, bzFp, 0, nullptr, nullptr );
#else
                BZ2_bzWriteClose ( &bzError, bzFp, 0, NULL, NULL );
#endif
                fprintf(stderr, "ERROR: Could not write to bzip2 file handle\n");
                return STARCH_FATAL_ERROR;
            }
            idx = 0;
        }
    }
    /* write out remainder of bzip2 buffer to output */
    bzBuf[idx] = '\0';
#ifdef __cplusplus
    BZ2_bzWrite(&bzError, bzFp, bzBuf, static_cast<int>( idx ));
#else
    BZ2_bzWrite(&bzError, bzFp, bzBuf, (int) idx);
#endif
    if (bzError == BZ_IO_ERROR) {   
#ifdef __cplusplus
        BZ2_bzWriteClose ( &bzError, bzFp, 0, nullptr, nullptr );
#else
        BZ2_bzWriteClose ( &bzError, bzFp, 0, NULL, NULL );
#endif
        fprintf(stderr, "ERROR: Could not write to bzip2 file handle\n");
        return STARCH_FATAL_ERROR;
    }

    /* close bzip2 stream */
#ifdef __cplusplus
    BZ2_bzWriteClose( &bzError, bzFp, 0, nullptr, nullptr );
#else
    BZ2_bzWriteClose( &bzError, bzFp, 0, NULL, NULL );
#endif
    if (bzError == BZ_IO_ERROR) {
        fprintf(stderr, "ERROR: Could not close bzip2 file handle\n");
        return STARCH_FATAL_ERROR;
    }

    /* close input */
    fclose(inFnPtr);

    /* close output */
    fclose(outFnPtr);

    /* get bzip2 file size */
#ifdef __cplusplus
    if (stat(reinterpret_cast<const char *>( *outFn ), &outSt) != 0) {
#else
    if (stat((const char *)*outFn, &outSt) != 0) {
#endif
        fprintf(stderr, "ERROR: Could not get bzip2 file attributes\n");
        return STARCH_FATAL_ERROR;
    }
    *outFnSize = outSt.st_size;

    return 0;
}

int 
STARCH_createTransformTokens(const char *s, const char delim, char **chr, int64_t *start, int64_t *stop, char **remainder, BedLineType *lineType) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_createTransformTokens() ---\n");
#endif
    unsigned int charCnt, sCnt;
    int elemCnt;
    char buffer[STARCH_BUFFER_MAX_LENGTH];
    unsigned int idIdx = 0U;
    unsigned int restIdx = 0U;

    charCnt = 0U;
    sCnt = 0U;
    elemCnt = 0U;

    do {
        buffer[charCnt++] = s[sCnt];
        if ((s[sCnt] == delim) || (s[sCnt] == '\0')) {
            if (elemCnt < 3) {
                buffer[(charCnt - 1)] = '\0';
                charCnt = 0;
            }
#ifdef DEBUG
            fprintf(stderr, "\t--- s [%s] buffer [%s], charCnt [%u], strlen(buffer) [%zu], sCnt [%u], strlen(s) [%zu]\n", s, buffer, charCnt, strlen(buffer), sCnt, strlen(s));
#endif
            switch (elemCnt) {
                case 0: {
                    /* we do field validation tests after we determine what kind of BED line this is */

                    /* determine what type of BED input line we're working with on the basis of the chr value */
#ifdef __cplusplus
                    if (strncmp(reinterpret_cast<const char *>( buffer ), kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0) {
#else
                    if (strncmp((const char *) buffer, kStarchBedHeaderTrack, strlen(kStarchBedHeaderTrack)) == 0) {
#endif
                        *lineType = kBedLineHeaderTrack;
                        elemCnt = 3;
                    }
#ifdef __cplusplus
                    else if (strncmp(reinterpret_cast<const char *>( buffer ), kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0) {
#else
                    else if (strncmp((const char *) buffer, kStarchBedHeaderBrowser, strlen(kStarchBedHeaderBrowser)) == 0) {
#endif
                        *lineType = kBedLineHeaderBrowser;
                        elemCnt = 3;
                    }
#ifdef __cplusplus
                    else if (strncmp(reinterpret_cast<const char *>( buffer ), kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0) {
#else
                    else if (strncmp((const char *) buffer, kStarchBedHeaderSAM, strlen(kStarchBedHeaderSAM)) == 0) {
#endif
                        *lineType = kBedLineHeaderSAM;
                        elemCnt = 3;
                    }
#ifdef __cplusplus
                    else if (strncmp(reinterpret_cast<const char *>( buffer ), kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0) {
#else
		    else if (strncmp((const char *) buffer, kStarchBedHeaderVCF, strlen(kStarchBedHeaderVCF)) == 0) {
#endif
                        *lineType = kBedLineHeaderVCF;
                        elemCnt = 3;
                    }
#ifdef __cplusplus
                    else if (strncmp(reinterpret_cast<const char *>( buffer ), kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0) {
#else
                    else if (strncmp((const char *) buffer, kStarchBedGenericComment, strlen(kStarchBedGenericComment)) == 0) {
#endif
                        *lineType = kBedLineGenericComment;
                        elemCnt = 3;
                    }
                    else {
                        *lineType = kBedLineCoordinates;
                    }

                    /* if line type is of kBedLineCoordinates type, then we test chromosome length */
                    if (*lineType == kBedLineCoordinates) {
#ifdef DEBUG
                        fprintf(stderr, "\t--- copying chromosome field ---\n");
#endif
                        if (strlen(buffer) > TOKEN_CHR_MAX_LENGTH) {
                            fprintf(stderr, "ERROR: Chromosome field length is too long (must be no longer than %lu characters)\n", TOKEN_CHR_MAX_LENGTH);
                            return STARCH_FATAL_ERROR;
                        }
                        /* copy element to chromosome variable, if memory is available */
#ifdef __cplusplus
                        *chr = static_cast<char *>( malloc((strlen(buffer) + 1) * sizeof(**chr)) );
#else
                        *chr = malloc((strlen(buffer) + 1) * sizeof(**chr));
#endif
                        if (! *chr) {
                            fprintf(stderr, "ERROR: Ran out of memory while creating transform tokens\n");
                            return STARCH_FATAL_ERROR;
                        }
#ifdef __cplusplus
                        strncpy(*chr, reinterpret_cast<const char *>( buffer ), strlen(buffer) + 1);
#else
                        strncpy(*chr, (const char *)buffer, strlen(buffer) + 1);
#endif
                    }
                    /* otherwise, we limit the length of a comment line to TOKENS_HEADER_MAX_LENGTH */
                    else {
#ifdef DEBUG
                        fprintf(stderr, "\t--- copying whole line ---\n");
#endif
                        if (strlen(s) > TOKENS_HEADER_MAX_LENGTH) {
                            fprintf(stderr, "ERROR: Comment line length is too long (must be no longer than %lu characters)\n", TOKEN_CHR_MAX_LENGTH);
                            return STARCH_FATAL_ERROR;
                        }
                        /* copy whole line to chromosome variable, if memory is available */
#ifdef __cplusplus
                        *chr = static_cast<char *>( malloc((strlen(s) + 1) * sizeof(**chr)) );
#else
                        *chr = malloc((strlen(s) + 1) * sizeof(**chr));
#endif
                        if (! *chr) {
                            fprintf(stderr, "ERROR: Ran out of memory while creating transform tokens\n");
                            return STARCH_FATAL_ERROR;
                        }
                        strncpy(*chr, s, strlen(s) + 1);
                    }
#ifdef DEBUG
                    fprintf(stderr, "\t--- resulting chr [%s]\n", *chr);
#endif
                    break;
                }
                case 1: {
                    /* test if element string is longer than allowed bounds */
                    if (strlen(buffer) > MAX_DEC_INTEGERS) {
                        fprintf(stderr, "ERROR: Start coordinate field length is too long ([%s] must be no greater than %ld characters)\n", buffer, MAX_DEC_INTEGERS);
                        return STARCH_FATAL_ERROR;
                    }
                    /* convert element string to start coordinate */
#ifdef __cplusplus
                    *start = static_cast<int64_t>( strtoull(reinterpret_cast<const char *>( buffer ), nullptr, STARCH_RADIX) );
#else
                    *start = (int64_t) strtoull((const char *)buffer, NULL, STARCH_RADIX);
#endif
                    /* test if start coordinate is larger than allowed bounds */
#ifdef __cplusplus
                    if (*start > static_cast<int64_t>( MAX_COORD_VALUE )) {
                        fprintf(stderr, "ERROR: Start coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *start, static_cast<int64_t>( MAX_COORD_VALUE ));
#else
                    if (*start > (int64_t) MAX_COORD_VALUE) {
                        fprintf(stderr, "ERROR: Start coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *start, (int64_t) MAX_COORD_VALUE);
#endif
                        return STARCH_FATAL_ERROR;
                    }
#ifdef DEBUG
                    fprintf(stderr, "\t--- resulting start [%" PRId64 "]\n", *start);
#endif
                    break;
                }
                case 2: {
                    /* test if element string is longer than allowed bounds */
                    if (strlen(buffer) > MAX_DEC_INTEGERS) {
                        fprintf(stderr, "ERROR: Stop coordinate field length is too long (must be no greater than %ld characters)\n", MAX_DEC_INTEGERS);
                        return STARCH_FATAL_ERROR;
                    }
                    /* convert element string to stop coordinate */
#ifdef __cplusplus
                    *stop = static_cast<int64_t>( strtoull(reinterpret_cast<const char *>( buffer ), nullptr, STARCH_RADIX) );
#else
                    *stop = (int64_t) strtoull((const char *)buffer, NULL, STARCH_RADIX);
#endif
                    /* test if stop coordinate is larger than allowed bounds */
#ifdef __cplusplus
                    if (*stop > static_cast<int64_t>( MAX_COORD_VALUE )) {
                        fprintf(stderr, "ERROR: Stop coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *stop, static_cast<int64_t>( MAX_COORD_VALUE ));
#else
                    if (*stop > (int64_t) MAX_COORD_VALUE) {
                        fprintf(stderr, "ERROR: Stop coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *stop, (int64_t) MAX_COORD_VALUE);
#endif
                        return STARCH_FATAL_ERROR;
                    }
#ifdef DEBUG
                    fprintf(stderr, "\t--- resulting stop [%" PRId64 "]\n", *stop);
#endif
                    break;
                }
                /* just keep filling the buffer until we reach s[sCnt]'s null -- we do tests later */
                case 3:
                    break;
            }
            elemCnt++;
        }
    } while (s[sCnt++] != '\0');

#ifdef DEBUG
    fprintf(stderr, "\t--- s [%s] buffer [%s], charCnt [%u], strlen(buffer) [%zu], sCnt [%u], strlen(s) [%zu], idIdx [%d]\n", s, buffer, charCnt, strlen(buffer), sCnt, strlen(s), idIdx);
    fprintf(stderr, "\t (post create-transform-tokens: chr -> %s\n\tstart -> %" PRId64 "\n\tstop -> %" PRId64 "\n\tremainder -> %s\n", *chr, *start, *stop, *remainder);
#endif

    if (elemCnt > 3) {
        if (charCnt > 0) {
            buffer[(charCnt - 1)] = '\0';
        }
        /* test id field length */
        while (((buffer[idIdx] != delim) && (buffer[idIdx] != '\0')) && (idIdx++ < TOKEN_ID_MAX_LENGTH)) { }
        if (idIdx >= TOKEN_ID_MAX_LENGTH) {
            fprintf(stderr, "ERROR: Id field is too long (must be less than %lu characters long)\n", TOKEN_ID_MAX_LENGTH);
            return STARCH_FATAL_ERROR;
        }
        /* test remnant of buffer, if there is more to look at */
        if (charCnt > idIdx) {
            while ((buffer[idIdx++] != '\0') && (restIdx++ < TOKEN_REST_MAX_LENGTH)) {}
            if (restIdx > TOKEN_REST_MAX_LENGTH) {
                fprintf(stderr, "ERROR: Remainder of BED input after id field is too long (must be less than %lu characters long)\n", TOKEN_REST_MAX_LENGTH);
                return STARCH_FATAL_ERROR;
            }
        }
#ifdef __cplusplus
        *remainder = static_cast<char *>( malloc((strlen(buffer) + 1) * sizeof(**remainder)) );
#else
        *remainder = malloc((strlen(buffer) + 1) * sizeof(**remainder));
#endif
        if (! *remainder) {
            fprintf(stderr, "ERROR: Ran out of memory handling token remainder\n");
            return STARCH_FATAL_ERROR;
        }
#ifdef __cplusplus
        strncpy(*remainder, reinterpret_cast<const char *>( buffer ), strlen(buffer) + 1);
#else
        strncpy(*remainder, (const char *)buffer, strlen(buffer) + 1);
#endif
#ifdef DEBUG
        fprintf(stderr, "\t--- resulting remainder [%s]\n", *remainder);
#endif
    }

    return 0;
}

int 
STARCH_createTransformTokensForHeaderlessInput(const char *s, const char delim, char **chr, int64_t *start, int64_t *stop, char **remainder) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_createTransformTokensForHeaderlessInput() ---\n");
#endif
#ifdef __cplusplus
    char *chrCopy = nullptr;
    char *remainderCopy = nullptr;
#else
    char *chrCopy = NULL;
    char *remainderCopy = NULL;
#endif
    unsigned int charCnt, sCnt, elemCnt;
    char buffer[STARCH_BUFFER_MAX_LENGTH];
    unsigned int idIdx = 0U;
    unsigned int restIdx = 0U;

    charCnt = 0U;
    sCnt = 0U;
    elemCnt = 0U;

    do {
        buffer[charCnt++] = s[sCnt];
        if ((s[sCnt] == delim) || (s[sCnt] == '\0')) {
            if (elemCnt < 3) {
                buffer[(charCnt - 1)] = '\0';
                charCnt = 0;
            }
            switch (elemCnt) {
                case 0: {
#ifdef DEBUG
                    fprintf(stderr, "\tcase 0\n");
#endif
                    /* test if element string is longer than allowed bounds */
                    if (strlen(buffer) > TOKEN_CHR_MAX_LENGTH) {
                        fprintf(stderr, "ERROR: Chromosome field length is too long (must be no longer than %lu characters)\n", TOKEN_CHR_MAX_LENGTH);
                        return STARCH_FATAL_ERROR;
                    }
                    /* copy element to chromosome variable, if memory is available */
                    if (! *chr)
#ifdef __cplusplus
                        *chr = static_cast<char *>( malloc((strlen(buffer) + 1) * sizeof(**chr)) );
#else
                        *chr = malloc((strlen(buffer) + 1) * sizeof(**chr));
#endif
                    else if (strlen(buffer) > strlen(*chr)) {
#ifdef __cplusplus
                        chrCopy = static_cast<char *>( realloc(*chr, strlen(buffer) * 2) );
#else
                        chrCopy = realloc(*chr, strlen(buffer) * 2);
#endif
                        if (!chrCopy) {
                            fprintf(stderr, "ERROR: Ran out of memory while extending chr token\n");
                            return STARCH_FATAL_ERROR;
                        }
                        *chr = chrCopy;
                    }
                    if (! *chr) {
                        fprintf(stderr, "ERROR: Ran out of memory while creating transform tokens\n");
                        return STARCH_FATAL_ERROR;
                    }
#ifdef __cplusplus
                    strncpy(*chr, reinterpret_cast<const char *>( buffer ), strlen(buffer) + 1);
#else
                    strncpy(*chr, (const char *)buffer, strlen(buffer) + 1);
#endif
                    break;
                }
                case 1: {
#ifdef DEBUG
                    fprintf(stderr, "\tcase 1\n");
#endif
                    /* test if element string is longer than allowed bounds */
                    if (strlen(buffer) > MAX_DEC_INTEGERS) {
                        fprintf(stderr, "ERROR: Start coordinate field length is too long ([%s] must be no greater than %ld characters)\n", buffer, MAX_DEC_INTEGERS);
                        return STARCH_FATAL_ERROR;
                    }
                    /* convert element string to start coordinate */
#ifdef __cplusplus
                    *start = static_cast<int64_t>( strtoll(reinterpret_cast<const char *>( buffer ), nullptr, STARCH_RADIX) );
#else
                    *start = (int64_t) strtoll((const char *)buffer, NULL, STARCH_RADIX);
#endif
                    /* test if start coordinate is larger than allowed bounds */
#ifdef __cplusplus
                    if (*start > static_cast<int64_t>( MAX_COORD_VALUE )) {
                        fprintf(stderr, "ERROR: Start coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *start, static_cast<int64_t>( MAX_COORD_VALUE ));
#else
                    if (*start > (int64_t) MAX_COORD_VALUE) {
                        fprintf(stderr, "ERROR: Start coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *start, (int64_t) MAX_COORD_VALUE);
#endif
                        return STARCH_FATAL_ERROR;
                    }
                    break;
                }
                case 2: {
#ifdef DEBUG
                    fprintf(stderr, "\tcase 2\n");
#endif
                    /* test if element string is longer than allowed bounds */
                    if (strlen(buffer) > MAX_DEC_INTEGERS) {
                        fprintf(stderr, "ERROR: Stop coordinate field length is too long (must be no greater than %ld characters)\n", MAX_DEC_INTEGERS);
                        return STARCH_FATAL_ERROR;
                    }
                    /* convert element string to stop coordinate */
#ifdef __cplusplus
                    *stop = static_cast<int64_t>( strtoll(reinterpret_cast<const char *>( buffer ), nullptr, STARCH_RADIX) );
#else
                    *stop = (int64_t) strtoll((const char *)buffer, NULL, STARCH_RADIX);
#endif
                    /* test if stop coordinate is larger than allowed bounds */
#ifdef __cplusplus
                    if (*stop > static_cast<int64_t>( MAX_COORD_VALUE )) {
                        fprintf(stderr, "ERROR: Stop coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *stop, static_cast<int64_t>( MAX_COORD_VALUE ));
                        return STARCH_FATAL_ERROR;
                    }
#else
                    if (*stop > (int64_t) MAX_COORD_VALUE) {
                        fprintf(stderr, "ERROR: Stop coordinate field value (%" PRId64 ") is too great (must be less than %" PRId64 ")\n", *stop, (int64_t) MAX_COORD_VALUE);
                        return STARCH_FATAL_ERROR;
                    }
#endif
                    break;
                }
                /* just keep filling the buffer until we reach s[sCnt]'s null -- we do tests later */
                case 3: {
#ifdef DEBUG
                    fprintf(stderr, "\tcase 3\n");
#endif
                    break;
                }
            }
            elemCnt++;
        }
    } while (s[sCnt++] != '\0');

    /* apply tests on id and score-strand-... ("rest") element strings */
    if (elemCnt > 3) {
        buffer[(charCnt - 1)] = '\0';
        /* test id field length */
        while (((buffer[idIdx] != delim) && (buffer[idIdx] != '\0')) && (idIdx++ < TOKEN_ID_MAX_LENGTH)) { }
        if (idIdx >= TOKEN_ID_MAX_LENGTH) {
            fprintf(stderr, "ERROR: Id field is too long (must be less than %lu characters long)\n", TOKEN_ID_MAX_LENGTH);
            return STARCH_FATAL_ERROR;
        }
        /* test remnant ("rest") of buffer, if there is more to look at */
        if (charCnt > idIdx) {
            while ((buffer[idIdx++] != '\0') && (restIdx++ < TOKEN_REST_MAX_LENGTH)) {}
            if (restIdx > TOKEN_REST_MAX_LENGTH) {
                fprintf(stderr, "ERROR: Remainder of BED input after id field is too long (must be less than %lu characters long)\n", TOKEN_REST_MAX_LENGTH);
                return STARCH_FATAL_ERROR;
            }
        }
        /* resize remainder, if needed */
        if (! *remainder)
#ifdef __cplusplus
            *remainder = static_cast<char *>( malloc((strlen(buffer) + 1) * sizeof(**remainder)) );
#else
            *remainder = malloc((strlen(buffer) + 1) * sizeof(**remainder));
#endif
        else if (strlen(buffer) > strlen(*remainder)) {
#ifdef DEBUG
            fprintf(stderr, "\tresizing remainder...\n");
#endif
#ifdef __cplusplus
            remainderCopy = static_cast<char *>( realloc(*remainder, strlen(buffer) * 2) );
#else
            remainderCopy = realloc(*remainder, strlen(buffer) * 2);
#endif
            if (!remainderCopy) {
                fprintf(stderr, "ERROR: Ran out of memory extending remainder token\n");
                return STARCH_FATAL_ERROR;
            }
            *remainder = remainderCopy;
        }
        if (! *remainder) {
            fprintf(stderr, "ERROR: Ran out of memory handling remainder token\n");
            return STARCH_FATAL_ERROR;
        }
#ifdef __cplusplus
        strncpy(*remainder, reinterpret_cast<const char *>( buffer ), strlen(buffer) + 1);
#else
        strncpy(*remainder, (const char *)buffer, strlen(buffer) + 1);
#endif
    }
    else if (elemCnt < 2) {
        fprintf(stderr, "ERROR: BED data is missing chromosome and/or coordinate data\n");
        return STARCH_FATAL_ERROR;
    }

#ifdef DEBUG
    fprintf(stderr, "\t (post create-transform-tokens: chr -> %s\n\tstart -> %" PRId64 "\n\tstop -> %" PRId64 "\n\tremainder -> %s\n", *chr, *start, *stop, *remainder);
#endif

    return 0;
}

int 
STARCH_transformInput(Metadata **md, const FILE *fp, const CompressionType type, const char *tag, const char *note) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_transformInput() ---\n");
#endif
#ifdef __cplusplus
    char *outFn = nullptr;
    FILE *outFnPtr = nullptr;
    FILE *streamPtr = const_cast<FILE *>( fp );
    char *outCompressedFn = nullptr;
    char *remainder = nullptr;
    char *prevChromosome = nullptr;
    char *chromosome = nullptr;
    Metadata *firstRecord = nullptr;
    char *legacyMdBuf = nullptr;
    char *dynamicMdBuf = nullptr;
#else
    char *outFn = NULL;
    FILE *outFnPtr = NULL;
    FILE *streamPtr = (FILE *) fp;
    char *outCompressedFn = NULL;
    char *remainder = NULL;
    char *prevChromosome = NULL;
    char *chromosome = NULL;
    Metadata *firstRecord = NULL;
    char *legacyMdBuf = NULL;
    char *dynamicMdBuf = NULL;
#endif
    int c;
    int cIdx = 0;
    int recIdx = 0;
    char buffer[STARCH_BUFFER_MAX_LENGTH];
    int64_t start = 0;
    int64_t stop = 0;
    int64_t previousStop = 0;
    int64_t lastPosition = 0;
    int64_t lcDiff = 0;
    int64_t coordDiff = 0;
    uint64_t outFnSize = 0;
    Boolean withinChr = kStarchFalse;
    unsigned long lineIdx = 0UL;
    off_t outCompressedFnSize = 0;
    BedLineType lineType = kBedLineTypeUndefined;
    char nonCoordLineBuf[STARCH_BUFFER_MAX_LENGTH] = {0};
    Boolean nonCoordLineBufNeedsPrinting = kStarchFalse;
    BaseCountType totalNonUniqueBases = 0;
    BaseCountType totalUniqueBases = 0;
    Boolean duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;

    if (!streamPtr)
        streamPtr = stdin;
    
    while ((c = fgetc(streamPtr)) != EOF) {
#ifdef __cplusplus
        buffer[cIdx] = static_cast<char>( c );
#else
        buffer[cIdx] = (char) c;
#endif
        if (c == '\n') {
            lineIdx++;
            buffer[cIdx] = '\0';
            if (STARCH_createTransformTokens(buffer, '\t', &chromosome, &start, &stop, &remainder, &lineType) == 0) {
                /* 
                    Either previous chromosome is NULL, or current chromosome does 
                    not equal previous chromosome, but the line must be of the 
                    type 'kBedLineCoordinates' (cf. 'starchMetadataHelpers.h')
                */
                if ( (lineType == kBedLineCoordinates) && ((!prevChromosome) || (strcmp(chromosome, prevChromosome) != 0)) ) {
                    /* close old output file pointer */
#ifdef __cplusplus
                    if (outFnPtr != nullptr) {
                        fclose(outFnPtr);
                        outFnPtr = nullptr;

                        if (type == kBzip2) {
                            /* bzip-compress the previous file */
                            if (STARCH_compressFileWithBzip2(reinterpret_cast<const char *>( outFn ), &outCompressedFn, static_cast<off_t *>( &outCompressedFnSize ) ) != 0) {
                                fprintf(stderr, "ERROR: Could not bzip2 compress per-chromosome output file %s\n", outFn);
                                return STARCH_FATAL_ERROR;
                            }
#else
                    if (outFnPtr != NULL) {
                        fclose(outFnPtr);
                        outFnPtr = NULL;

                        if (type == kBzip2) {
                            /* bzip-compress the previous file */
                            if (STARCH_compressFileWithBzip2((const char *)outFn, &outCompressedFn, (off_t *) &outCompressedFnSize ) != 0) {
                                fprintf(stderr, "ERROR: Could not bzip2 compress per-chromosome output file %s\n", outFn);
                                return STARCH_FATAL_ERROR;
                            }
#endif
                        }
                        else if (type == kGzip) {
                            /* gzip-compress file */
#ifdef __cplusplus
                            if (STARCH_compressFileWithGzip(reinterpret_cast<const char*>( outFn ), &outCompressedFn, static_cast<off_t *>( &outCompressedFnSize ) ) != 0) {
                                fprintf(stderr, "ERROR: Could not gzip compress per-chromosome output file %s\n", outFn);
                                return STARCH_FATAL_ERROR;
                            }
#else
                            if (STARCH_compressFileWithGzip((const char*) outFn, &outCompressedFn, (off_t *) &outCompressedFnSize ) != 0) {
                                fprintf(stderr, "ERROR: Could not gzip compress per-chromosome output file %s\n", outFn);
                                return STARCH_FATAL_ERROR;
                            }
#endif
                        }
                        else {
                            fprintf(stderr, "ERROR: Unknown compression regime\n");
                            return STARCH_FATAL_ERROR;
                        }
                        /* delete uncompressed file */
                        if (remove(outFn) != 0) {
                            fprintf(stderr, "ERROR: Could not delete per-chromosome output file %s\n", outFn);
                            return STARCH_FATAL_ERROR;
                        }

                        /* update metadata with compressed file attributes */
                        if (STARCH_updateMetadataForChromosome(md,
                                                               prevChromosome, 
                                                               outCompressedFn, 
#ifdef __cplusplus
                                                               static_cast<uint64_t>( outCompressedFnSize ), 
#else
                                                               (uint64_t) outCompressedFnSize, 
#endif
                                                               lineIdx, 
                                                               totalNonUniqueBases, 
                                                               totalUniqueBases,
                                                               duplicateElementExistsFlag,
                                                               nestedElementExistsFlag,
#ifdef __cplusplus
							       nullptr,
#else
                                                               NULL,
#endif
                                                               STARCH_DEFAULT_LINE_STRING_LENGTH) != STARCH_EXIT_SUCCESS) {
                            fprintf(stderr, "ERROR: Could not update metadata%s\n", outFn);
                            return STARCH_FATAL_ERROR;
                        }

                        /* cleanup */
#ifdef __cplusplus
                        free(outCompressedFn); 
			outCompressedFn = nullptr;
#else
                        free(outCompressedFn); 
			outCompressedFn = NULL;
#endif
                    }
			    
		    /* test if current chromosome is already a Metadata record */
#ifdef __cplusplus
		    if (STARCH_chromosomeInMetadataRecords(reinterpret_cast<const Metadata *>( *md ), chromosome) == STARCH_EXIT_SUCCESS) {
		        fprintf(stderr, "ERROR: Found same chromosome in earlier portion of file. Possible interleaving issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                        return STARCH_FATAL_ERROR;
		    }
#else
		    if (STARCH_chromosomeInMetadataRecords((const Metadata *)*md, chromosome) == STARCH_EXIT_SUCCESS) {
		        fprintf(stderr, "ERROR: Found same chromosome in earlier portion of file. Possible interleaving issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                        return STARCH_FATAL_ERROR;
		    }
#endif

                    /* open new output file pointer */
                    if (!outFnPtr) {
#ifdef __cplusplus
                        outFn = static_cast<char *>( malloc(strlen(chromosome) + strlen(tag) + 2) );
#else
                        outFn = malloc(strlen(chromosome) + strlen(tag) + 2);
#endif
                        sprintf(outFn, "%s.%s", chromosome, tag);
                        outFnPtr = STARCH_fopen(outFn, "a");
                        if (!outFnPtr) {
                            fprintf(stderr, "ERROR: Could not open an intermediate output file handle to %s\n", outFn);
                            return STARCH_FATAL_ERROR;
                        }
                    }
                    else {
                        fprintf(stderr, "ERROR: Could not open per-chromosome output file\n");
                        return STARCH_FATAL_ERROR;
                    }

                    /* add chromosome to metadata */
                    if (recIdx == 0) {
                        *md = STARCH_createMetadata(chromosome, 
                                                    outFn, 
                                                    outFnSize, 
                                                    lineIdx, 
                                                    totalNonUniqueBases, 
                                                    totalUniqueBases,
                                                    duplicateElementExistsFlag,
                                                    nestedElementExistsFlag,
#ifdef __cplusplus
                                                    nullptr,
#else
                                                    NULL,
#endif
                                                    STARCH_DEFAULT_LINE_STRING_LENGTH);
                        firstRecord = *md;
                    }
                    else {
                        *md = STARCH_addMetadata(*md, 
                                                 chromosome, 
                                                 outFn, 
                                                 outFnSize, 
                                                 lineIdx, 
                                                 totalNonUniqueBases, 
                                                 totalUniqueBases,
                                                 duplicateElementExistsFlag,
                                                 nestedElementExistsFlag,
#ifdef __cplusplus
						 nullptr,
#else
                                                 NULL,
#endif
                                                 STARCH_DEFAULT_LINE_STRING_LENGTH);
                    }

                    /* make previous chromosome the current chromosome */
#ifdef __cplusplus
                    if (prevChromosome != nullptr) {
                        free(prevChromosome);
                        prevChromosome = nullptr;
                    }
                    prevChromosome = static_cast<char *>( malloc(strlen(chromosome) + 1) );
                    strncpy(prevChromosome, reinterpret_cast<const char *>( chromosome ), strlen(chromosome) + 1);
#else
                    if (prevChromosome != NULL) {
                        free(prevChromosome);
                        prevChromosome = NULL;
                    }
                    prevChromosome = malloc(strlen(chromosome) + 1);
                    strncpy(prevChromosome, (const char *)chromosome, strlen(chromosome) + 1);
#endif

                    /* reset flag, lastPosition and lcDiff, increment record index */
                    withinChr = kStarchFalse;
                    lastPosition = 0;
                    previousStop = 0;
                    lcDiff = 0;
                    lineIdx = 0UL;
                    totalNonUniqueBases = 0UL;
                    totalUniqueBases = 0UL;
                    duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
                    nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
                    recIdx++;
                }
                else if (lineType == kBedLineCoordinates) {
                    withinChr = kStarchFalse;
                }

                /* transform data, depending on line type */                

                if (lineType != kBedLineCoordinates) {
                    /* 
                       It is possible for custom track header data to collect on two or
                       more consecutive lines. So we concatenate with any previously collected
                       header data, which will then be sent to the output file pointer 
                       at some point in the future as one big chunk...
    
                       Note that we do not expect that contiguous custom track header information 
                       will be larger than STARCH_BUFFER_MAX_LENGTH bytes. This might well prove
                       to be a dangerous assumption, but probably not, as 1 MB is a lot of custom 
                       track data in one contiguous block. This situation seems fairly unlikely.
                    */
#ifdef __cplusplus
                    strncat(nonCoordLineBuf, reinterpret_cast<const char *>( chromosome ), strlen(chromosome) + 1);
#else
                    strncat(nonCoordLineBuf, (const char *)chromosome, strlen(chromosome) + 1);
#endif
                    nonCoordLineBuf[strlen(nonCoordLineBuf)] = '\n';
                    nonCoordLineBufNeedsPrinting = kStarchTrue;
                }
                else {
                    if (nonCoordLineBufNeedsPrinting == kStarchTrue) {
                        /* 
                           if there's custom track data that needs printin', we do so now 
                           and reset the buffer and print flag
                        */
                        fprintf(outFnPtr, "%s", nonCoordLineBuf);
                        memset(nonCoordLineBuf, 0, strlen(nonCoordLineBuf));
                        nonCoordLineBufNeedsPrinting = kStarchFalse;
                    }
                    if (stop > start)
                        coordDiff = stop - start;
                    else {
                        fprintf(stderr, "ERROR: Bed data is corrupt at line %lu (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, stop, start);
                        return STARCH_FATAL_ERROR;
                    }
                    if (coordDiff != lcDiff) {
                        lcDiff = coordDiff;
                        fprintf( outFnPtr, "p%" PRId64 "\n", coordDiff );
                    }
                    if (lastPosition != 0) {
                        if (remainder)
                            fprintf( outFnPtr, "%" PRId64 "\t%s\n", (start - lastPosition), remainder );
                        else
                            fprintf( outFnPtr, "%" PRId64 "\n", (start - lastPosition) );
                    }
                    else {
                        if (remainder)
                            fprintf( outFnPtr, "%" PRId64 "\t%s\n", start, remainder );
                        else
                            fprintf( outFnPtr, "%" PRId64 "\n", start );
                    }
#ifdef __cplusplus
                    totalNonUniqueBases += static_cast<BaseCountType>( stop - start );
                    if (previousStop <= start)
                        totalUniqueBases += static_cast<BaseCountType>( stop - start );
                    else if (previousStop < stop)
                        totalUniqueBases += static_cast<BaseCountType>( stop - previousStop );
#else
                    totalNonUniqueBases += (BaseCountType) (stop - start);
                    if (previousStop <= start)
                        totalUniqueBases += (BaseCountType) (stop - start);
                    else if (previousStop < stop)
                        totalUniqueBases += (BaseCountType) (stop - previousStop);
#endif
                    lastPosition = stop;
                    previousStop = (stop > previousStop) ? stop : previousStop;
                }

                /* cleanup unused data */
#ifdef __cplusplus
                if (withinChr == kStarchTrue) {
                    free(chromosome); chromosome = nullptr;
                }
                if (remainder) {
                    free(remainder); remainder = nullptr;
                }
#else
                if (withinChr == kStarchTrue) {
                    free(chromosome); chromosome = NULL;
                }
                if (remainder) {
                    free(remainder); remainder = NULL;
                }
#endif
                cIdx = 0;                
            }
            else {
                fprintf(stderr, "ERROR: Bed data could not be transformed\n");
                return STARCH_FATAL_ERROR;
            }
        }
        else
            cIdx++;
    }
    
    /* compress the remaining file */
#ifdef __cplusplus
    if (outFnPtr != nullptr) {
        fclose(outFnPtr); outFnPtr = nullptr;
        if (type == kBzip2) {
            if (STARCH_compressFileWithBzip2(reinterpret_cast<const char *>( outFn ), &outCompressedFn, static_cast<off_t *>( &outCompressedFnSize ) ) != 0) {
#else
    if (outFnPtr != NULL) {
        fclose(outFnPtr); outFnPtr = NULL;
        if (type == kBzip2) {
            if (STARCH_compressFileWithBzip2((const char *)outFn, &outCompressedFn, (off_t *) &outCompressedFnSize ) != 0) {
#endif
                fprintf(stderr, "ERROR: Could not bzip2 compress per-chromosome output file %s\n", outFn);
                return STARCH_FATAL_ERROR;
            }
        }
        else if (type == kGzip) {
            /* gzip-compress file */
#ifdef __cplusplus
            if (STARCH_compressFileWithGzip(reinterpret_cast<const char*>( outFn ), &outCompressedFn, static_cast<off_t *>( &outCompressedFnSize ) ) != 0) {
#else
            if (STARCH_compressFileWithGzip((const char*)outFn, &outCompressedFn, (off_t *) &outCompressedFnSize ) != 0) {
#endif
                fprintf(stderr, "ERROR: Could not gzip compress per-chromosome output file %s\n", outFn);
                return STARCH_FATAL_ERROR;
            }
        }
        else {
            fprintf(stderr, "ERROR: Unknown compression regime\n");
            return STARCH_FATAL_ERROR;
        }
        /* delete uncompressed file */
        if (remove(outFn) != 0) {
            fprintf(stderr, "ERROR: Could not delete per-chromosome output file %s -- is the input's first column sorted lexicographically?\n", outFn);
            return STARCH_FATAL_ERROR;
        }
        /* update metadata with compressed file attributes */
        lineIdx++;
        STARCH_updateMetadataForChromosome(md, 
                                           prevChromosome,
                                           outCompressedFn,
#ifdef __cplusplus
                                           static_cast<uint64_t>( outCompressedFnSize ),
#else
                                           (uint64_t) outCompressedFnSize, 
#endif
                                           lineIdx, 
                                           totalNonUniqueBases, 
                                           totalUniqueBases,
                                           duplicateElementExistsFlag,
                                           nestedElementExistsFlag,
#ifdef __cplusplus
					   nullptr,
#else
                                           NULL,
#endif
                                           STARCH_DEFAULT_LINE_STRING_LENGTH);
#ifdef __cplusplus
        free(outCompressedFn); outCompressedFn = nullptr;
#else
        free(outCompressedFn); outCompressedFn = NULL;
#endif
    }

    /* reposition metadata pointer to first record */
    *md = firstRecord;

    /* write metadata header to buffer */
    /* and concatenate metadata header with compressed files */
    if ((STARCH_MAJOR_VERSION == 1) && (STARCH_MINOR_VERSION == 0) && (STARCH_REVISION_VERSION == 0)) {
#ifdef __cplusplus
        legacyMdBuf = static_cast<char *>( malloc(STARCH_LEGACY_METADATA_SIZE + 1) );
        if (legacyMdBuf != nullptr) {
            if (STARCH_writeJSONMetadata(reinterpret_cast<const Metadata *>( *md ), &legacyMdBuf, const_cast<CompressionType *>( &type ), kStarchFalse, reinterpret_cast<const char *>( note )) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
                return STARCH_FATAL_ERROR;
            }
            if (STARCH_mergeMetadataWithCompressedFiles(reinterpret_cast<const Metadata *>( *md ), legacyMdBuf) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
                return STARCH_FATAL_ERROR;
            }
            free(legacyMdBuf);
            legacyMdBuf = nullptr;
#else
        legacyMdBuf = malloc(STARCH_LEGACY_METADATA_SIZE + 1);
        if (legacyMdBuf != NULL) {
            if (STARCH_writeJSONMetadata((const Metadata *)*md, &legacyMdBuf, (CompressionType *) &type, kStarchFalse, (const char *) note) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
                return STARCH_FATAL_ERROR;
            }
            if (STARCH_mergeMetadataWithCompressedFiles((const Metadata *)*md, legacyMdBuf) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
                return STARCH_FATAL_ERROR;
            }
            free(legacyMdBuf);
            legacyMdBuf = NULL;
#endif
        }
        else 
            return STARCH_FATAL_ERROR;
    }
    else {
        /* this is the custom header version of the parser, so we set headerFlag to TRUE */
#ifdef __cplusplus
        if (STARCH_writeJSONMetadata(reinterpret_cast<const Metadata *>( *md ), &dynamicMdBuf, const_cast<CompressionType *>( &type ), kStarchTrue, reinterpret_cast<const char *>( note )) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
            return STARCH_FATAL_ERROR;
        }
        if (STARCH_mergeMetadataWithCompressedFiles(reinterpret_cast<const Metadata *>( *md ), dynamicMdBuf) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
            return STARCH_FATAL_ERROR;
        }
        if (dynamicMdBuf != nullptr) {
            free(dynamicMdBuf);
            dynamicMdBuf = nullptr;
        }
#else
        if (STARCH_writeJSONMetadata((const Metadata *)*md, &dynamicMdBuf, (CompressionType *) &type, kStarchTrue, (const char *) note) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
            return STARCH_FATAL_ERROR;
        }
        if (STARCH_mergeMetadataWithCompressedFiles((const Metadata *)*md, dynamicMdBuf) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
            return STARCH_FATAL_ERROR;
        }
        if (dynamicMdBuf != NULL) {
            free(dynamicMdBuf);
            dynamicMdBuf = NULL;
        }
#endif
        else
            return STARCH_FATAL_ERROR;
    }

    /* remove compressed files */
#ifdef __cplusplus
    if (STARCH_deleteCompressedFiles(reinterpret_cast<const Metadata *>( *md )) != STARCH_EXIT_SUCCESS) {
#else
    if (STARCH_deleteCompressedFiles((const Metadata *)*md) != STARCH_EXIT_SUCCESS) {
#endif
        fprintf(stderr, "ERROR: Could not delete compressed streams\n");
        return STARCH_FATAL_ERROR;
    }

    /* cleanup */
    free(prevChromosome);

    return 0;
}

int
STARCH_transformHeaderlessInput(Metadata **md, const FILE *fp, const CompressionType type, const char *tag, const Boolean finalizeFlag, const char *note) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_transformHeaderlessInput() ---\n");
#endif
#ifdef __cplusplus
    char *outFn = nullptr;
    FILE *outFnPtr = nullptr;
    FILE *streamPtr = const_cast<FILE *>( fp );
    char *outCompressedFn = nullptr;
    char *remainder = nullptr;
    char *prevChromosome = nullptr;
    char *chromosome = nullptr;
    Metadata *firstRecord = nullptr;
    char *legacyMdBuf = nullptr; 
    char *dynamicMdBuf = nullptr;
#else
    char *outFn = NULL;
    FILE *outFnPtr = NULL;
    FILE *streamPtr = (FILE *) fp;
    char *outCompressedFn = NULL;
    char *remainder = NULL;
    char *prevChromosome = NULL;
    char *chromosome = NULL;
    Metadata *firstRecord = NULL;
    char *legacyMdBuf = NULL; 
    char *dynamicMdBuf = NULL;
#endif
    int c;
    int cIdx = 0;
    int recIdx = 0;
    char buffer[STARCH_BUFFER_MAX_LENGTH];
    int64_t start = 0;
    int64_t stop = 0;
    int64_t previousStop = 0;
    int64_t lastPosition = 0;
    int64_t lcDiff = 0;
    int64_t coordDiff = 0;
    uint64_t outFnSize = 0;
    Boolean withinChr = kStarchFalse;
    unsigned long lineIdx = 0UL;
    off_t outCompressedFnSize = 0;
    BaseCountType totalNonUniqueBases = 0;
    BaseCountType totalUniqueBases = 0;
    Boolean duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;

    if (!streamPtr)
        streamPtr = stdin;

    while ((c = fgetc(streamPtr)) != EOF) {
#ifdef __cplusplus
        buffer[cIdx] = static_cast<char>( c );
#else
        buffer[cIdx] = (char) c;
#endif
        if (c == '\n') {
            lineIdx++;
            buffer[cIdx] = '\0';
            if (STARCH_createTransformTokensForHeaderlessInput(buffer, '\t', &chromosome, &start, &stop, &remainder) == 0) 
            {
                /* 
                   Either previous chromosome is NULL, or current chromosome does 
                   not equal previous chromosome
                */

                if ( (!prevChromosome) || (strcmp(chromosome, prevChromosome) != 0) ) 
                {
                    /* close old output file pointer */
#ifdef __cplusplus
                    if (outFnPtr != nullptr) {
                        fclose(outFnPtr); 
                        outFnPtr = nullptr;
                        if (type == kBzip2) {
                            /* bzip-compress the previous file */
                            if (STARCH_compressFileWithBzip2(reinterpret_cast<const char *>( outFn ), &outCompressedFn, static_cast<off_t *>( &outCompressedFnSize ) ) != 0) {
#else
                    if (outFnPtr != NULL) {
                        fclose(outFnPtr); 
                        outFnPtr = NULL;
                        if (type == kBzip2) {
                            /* bzip-compress the previous file */
                            if (STARCH_compressFileWithBzip2((const char *)outFn, &outCompressedFn, (off_t *) &outCompressedFnSize ) != 0) {
#endif
                                fprintf(stderr, "ERROR: Could not bzip2 compress per-chromosome output file %s\n", outFn);
                                return STARCH_FATAL_ERROR;
                            }
                        }
                        else if (type == kGzip) {
                            /* gzip-compress file */
#ifdef __cplusplus
                            if (STARCH_compressFileWithGzip(reinterpret_cast<const char*>( outFn ), &outCompressedFn, static_cast<off_t *>( &outCompressedFnSize ) ) != 0) {
#else
                            if (STARCH_compressFileWithGzip((const char*) outFn, &outCompressedFn, (off_t *) &outCompressedFnSize ) != 0) {
#endif
                                fprintf(stderr, "ERROR: Could not gzip compress per-chromosome output file %s\n", outFn);
                                return STARCH_FATAL_ERROR;
                            }
                        }
                        else {
                            fprintf(stderr, "ERROR: Unknown compression regime\n");
                            return STARCH_FATAL_ERROR;
                        }
                        /* delete uncompressed file */
                        if (remove(outFn) != 0) {
                            fprintf(stderr, "ERROR: Could not delete per-chromosome output file %s\n", outFn);
                            return STARCH_FATAL_ERROR;
                        }

                        /* update metadata with compressed file attributes */
                        if (STARCH_updateMetadataForChromosome(md, 
                                                               prevChromosome, 
                                                               outCompressedFn, 
#ifdef __cplusplus
                                                               static_cast<uint64_t>( outCompressedFnSize ), 
#else
                                                               (uint64_t) outCompressedFnSize, 
#endif
                                                               lineIdx, 
                                                               totalNonUniqueBases, 
                                                               totalUniqueBases,
                                                               duplicateElementExistsFlag,
                                                               nestedElementExistsFlag,
#ifdef __cplusplus
							       nullptr,
#else
                                                               NULL,
#endif
                                                               STARCH_DEFAULT_LINE_STRING_LENGTH) != STARCH_EXIT_SUCCESS) {
                            fprintf(stderr, "ERROR: Could not update metadata%s\n", outFn);
                            return STARCH_FATAL_ERROR;
                        }

                        /* cleanup */
#ifdef __cplusplus
                        free(outCompressedFn); outCompressedFn = nullptr;
#else
                        free(outCompressedFn); outCompressedFn = NULL;
#endif
                    }

		    /* test if current chromosome is already a Metadata record */
#ifdef __cplusplus
		    if (STARCH_chromosomeInMetadataRecords(reinterpret_cast<const Metadata *>( *md ), chromosome) == STARCH_EXIT_SUCCESS) {
#else
		    if (STARCH_chromosomeInMetadataRecords((const Metadata *)*md, chromosome) == STARCH_EXIT_SUCCESS) {
#endif
		        fprintf(stderr, "ERROR: Found same chromosome in earlier portion of file. Possible interleaving issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                        return STARCH_FATAL_ERROR;
		    }

                    /* open new output file pointer */
                    if (!outFnPtr) {
#ifdef __cplusplus
                        outFn = static_cast<char *>( malloc(strlen(chromosome) + strlen(tag) + 2) );
#else
                        outFn = malloc(strlen(chromosome) + strlen(tag) + 2);
#endif
                        sprintf(outFn, "%s.%s", chromosome, tag);
                        outFnPtr = STARCH_fopen(outFn, "a");
                        if (!outFnPtr) {
                            fprintf(stderr, "ERROR: Could not open an intermediate output file handle to %s\n", outFn);
                            return STARCH_FATAL_ERROR;
                        }
                    }
                    else {
                        fprintf(stderr, "ERROR: Could not open per-chromosome output file\n");
                        return STARCH_FATAL_ERROR;
                    }

                    /* add chromosome to metadata */
                    if (! *md) {
                        *md = STARCH_createMetadata(chromosome, 
                                                    outFn, 
                                                    outFnSize, 
                                                    lineIdx, 
                                                    totalNonUniqueBases, 
                                                    totalUniqueBases, 
                                                    duplicateElementExistsFlag,
                                                    nestedElementExistsFlag,
#ifdef __cplusplus
						    nullptr,
#else
                                                    NULL,
#endif
                                                    STARCH_DEFAULT_LINE_STRING_LENGTH);
                        firstRecord = *md;
                    }
                    else {
                        *md = STARCH_addMetadata(*md, 
                                                 chromosome, 
                                                 outFn, 
                                                 outFnSize, 
                                                 lineIdx, 
                                                 totalNonUniqueBases, 
                                                 totalUniqueBases,
                                                 duplicateElementExistsFlag,
                                                 nestedElementExistsFlag,
#ifdef __cplusplus
						 nullptr,
#else
                                                 NULL,
#endif
                                                 STARCH_DEFAULT_LINE_STRING_LENGTH);
                    }

                    /* make previous chromosome the current chromosome */
#ifdef __cplusplus
                    if (prevChromosome != nullptr) {
                        free(prevChromosome);
                        prevChromosome = nullptr;
                    }
                    prevChromosome = static_cast<char *>( malloc(strlen(chromosome) + 1) );
                    strncpy(prevChromosome, reinterpret_cast<const char *>( chromosome ), strlen(chromosome) + 1);
#else
                    if (prevChromosome != NULL) {
                        free(prevChromosome);
                        prevChromosome = NULL;
                    }
                    prevChromosome = malloc(strlen(chromosome) + 1);
                    strncpy(prevChromosome, (const char *)chromosome, strlen(chromosome) + 1);
#endif

                    /* reset flag, lastPosition and lcDiff, increment record index */
                    withinChr = kStarchFalse;
                    lastPosition = 0;
                    previousStop = 0;
                    lcDiff = 0;
                    lineIdx = 0UL;
                    totalNonUniqueBases = 0UL;
                    totalUniqueBases = 0UL;
                    duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
                    nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
                    recIdx++;
                }
                else
                    withinChr = kStarchTrue;

                /* transform data */
                if (stop > start)
                    coordDiff = stop - start;
                else {
                    fprintf(stderr, "ERROR: (A) BED data is corrupt at line %lu (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, stop, start);
                    return STARCH_FATAL_ERROR;
                }
                if (coordDiff != lcDiff) {
                    lcDiff = coordDiff;
#ifdef DEBUG
                    fprintf(stderr, "\tp%" PRId64 "\n", coordDiff);
#endif
                    fprintf(outFnPtr, "p%" PRId64 "\n", coordDiff );
                }
                if (lastPosition != 0) {
                    if (remainder) {
#ifdef DEBUG
                        fprintf(stderr, "\t%" PRId64 "\t%s\n", (start - lastPosition), remainder);
#endif
                        fprintf(outFnPtr, "%" PRId64 "\t%s\n", (start - lastPosition), remainder );
                    }
                    else {
#ifdef DEBUG
                        fprintf(stderr, "\t%" PRId64 "\n", (start - lastPosition));
#endif
                        fprintf(outFnPtr, "%" PRId64 "\n", (start - lastPosition) );
                    }
                }
                else {
                    if (remainder) {
#ifdef DEBUG
                        fprintf(stderr, "\t%" PRId64 "\t%s\n", start, remainder );
#endif
                        fprintf(outFnPtr, "%" PRId64 "\t%s\n", start, remainder );
                    }
                    else {
#ifdef DEBUG
                        fprintf(stderr, "\t%" PRId64 "\n", start );
#endif
                        fprintf(outFnPtr, "%" PRId64 "\n", start );
                    }
                }
#ifdef __cplusplus
                totalNonUniqueBases += static_cast<BaseCountType>( stop - start );
                if (previousStop <= start)
                    totalUniqueBases += static_cast<BaseCountType>( stop - start );
                else if (previousStop < stop)
                    totalUniqueBases += static_cast<BaseCountType>( stop - previousStop );
#else
                totalNonUniqueBases += (BaseCountType) (stop - start);
                if (previousStop <= start)
                    totalUniqueBases += (BaseCountType) (stop - start);
                else if (previousStop < stop)
                    totalUniqueBases += (BaseCountType) (stop - previousStop);
#endif
                lastPosition = stop;
                previousStop = (stop > previousStop) ? stop : previousStop;

                /* cleanup unused data */
                if (withinChr == kStarchTrue) {
                    free(chromosome);
#ifdef __cplusplus
                    chromosome = nullptr;
#else
                    chromosome = NULL;
#endif
                }
                if (remainder) {
                    free(remainder);
#ifdef __cplusplus
                    remainder = nullptr;
#else
                    remainder = NULL;
#endif
                }
                cIdx = 0;                
            }
            else {
                fprintf(stderr, "ERROR: BED data could not be transformed\n");
                return STARCH_FATAL_ERROR;
            }
        }
        else
            cIdx++;
    }
    
    /* compress the remaining file */
#ifdef __cplusplus
    if (outFnPtr != nullptr) {
        fclose(outFnPtr); 
        outFnPtr = nullptr;
        
        if (type == kBzip2) {
            if (STARCH_compressFileWithBzip2(reinterpret_cast<const char *>( outFn ), 
                                             &outCompressedFn, 
                                             static_cast<off_t *>( &outCompressedFnSize )) != 0) {
                fprintf(stderr, "ERROR: Could not bzip2 compress per-chromosome output file %s\n", outFn);
                return STARCH_FATAL_ERROR;
            }
#else
    if (outFnPtr != NULL) {
        fclose(outFnPtr); 
        outFnPtr = NULL;
        
        if (type == kBzip2) {
            if (STARCH_compressFileWithBzip2((const char *)outFn, 
                                             &outCompressedFn, 
                                             (off_t *) &outCompressedFnSize) != 0) {
                fprintf(stderr, "ERROR: Could not bzip2 compress per-chromosome output file %s\n", outFn);
                return STARCH_FATAL_ERROR;
            }
#endif
        }
        else if (type == kGzip) {
            /* gzip-compress file */
#ifdef __cplusplus
            if (STARCH_compressFileWithGzip(reinterpret_cast<const char*>( outFn ), 
                                            &outCompressedFn, 
                                            static_cast<off_t *>( &outCompressedFnSize )) != 0) {
                fprintf(stderr, "ERROR: Could not gzip compress per-chromosome output file %s\n", outFn);
                return STARCH_FATAL_ERROR;
            }
#else
            if (STARCH_compressFileWithGzip((const char*)outFn, 
                                            &outCompressedFn, 
                                            (off_t *) &outCompressedFnSize) != 0) {
                fprintf(stderr, "ERROR: Could not gzip compress per-chromosome output file %s\n", outFn);
                return STARCH_FATAL_ERROR;
            }
#endif
        }
        else {
            fprintf(stderr, "ERROR: Unknown compression regime\n");
            return STARCH_FATAL_ERROR;
        }

        /* delete uncompressed, transformed file */
        if (remove(outFn) != 0) {
            fprintf(stderr, "ERROR: Could not delete per-chromosome output file %s -- is the input's first column sorted lexicographically?\n", outFn);
            return STARCH_FATAL_ERROR;
        }

        /* update metadata with compressed file attributes */
        lineIdx++;
        STARCH_updateMetadataForChromosome(md, 
                                           prevChromosome, 
                                           outCompressedFn, 
#ifdef __cplusplus
                                           static_cast<uint64_t>( outCompressedFnSize ),
#else
                                           (uint64_t) outCompressedFnSize, 
#endif
                                           lineIdx, 
                                           totalNonUniqueBases, 
                                           totalUniqueBases,
                                           duplicateElementExistsFlag,
                                           nestedElementExistsFlag,
#ifdef __cplusplus
					   nullptr,
#else
                                           NULL,
#endif
                                           STARCH_DEFAULT_LINE_STRING_LENGTH);

#ifdef __cplusplus
        free(outCompressedFn); outCompressedFn = nullptr;
        free(outFn); outFn = nullptr;
#else
        free(outCompressedFn); outCompressedFn = NULL;
        free(outFn); outFn = NULL;
#endif
    }

    /* 
        We return early if we don't need to bundle up the starch archive 
        at this stage. We do this for the starchcat utility, for example,
        because we're probably in the middle of transforming multiple
        streams...
    */

    if (finalizeFlag == kStarchFalse)
        return 0;

    /*
        Otherwise, we wrap things up. In the future, this will go into its 
        own function for clarity...
    */

    /* reposition metadata pointer to first record */
    *md = firstRecord;

    /* write metadata header to buffer */
    /* concatenate metadata header with compressed files */
    if ((STARCH_MAJOR_VERSION == 1) && (STARCH_MINOR_VERSION == 0) && (STARCH_REVISION_VERSION == 0)) {
#ifdef __cplusplus
        legacyMdBuf = static_cast<char *>( malloc(STARCH_LEGACY_METADATA_SIZE + 1) );
        if (legacyMdBuf != nullptr) {
            /* headerless input was not supported in this version, so it is set to FALSE */
            if (STARCH_writeJSONMetadata(reinterpret_cast<const Metadata *>( *md ), 
					 &legacyMdBuf, 
					 const_cast<CompressionType *>( &type ), 
					 kStarchFalse, 
					 reinterpret_cast<const char *>( note )) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
                return STARCH_FATAL_ERROR;
            }
            if (STARCH_mergeMetadataWithCompressedFiles(reinterpret_cast<const Metadata *>( *md ), legacyMdBuf) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
                return STARCH_FATAL_ERROR;
            }
            free(legacyMdBuf);
            legacyMdBuf = nullptr;
	}
#else
        legacyMdBuf = malloc(STARCH_LEGACY_METADATA_SIZE + 1);
        if (legacyMdBuf != NULL) {
            /* headerless input was not supported in this version, so it is set to FALSE */
            if (STARCH_writeJSONMetadata((const Metadata *) *md, 
					 &legacyMdBuf, 
					 (CompressionType *) &type, 
					 kStarchFalse, 
					 (const char *) note) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
                return STARCH_FATAL_ERROR;
            }
            if (STARCH_mergeMetadataWithCompressedFiles((const Metadata *) *md, legacyMdBuf) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
                return STARCH_FATAL_ERROR;
            }
            free(legacyMdBuf);
            legacyMdBuf = NULL;
        }
#endif
        else 
            return STARCH_FATAL_ERROR;
    }
    else {
        /* headerless input means headerFlag is FALSE */
#ifdef __cplusplus
        if (STARCH_writeJSONMetadata(reinterpret_cast<const Metadata *>( *md ), 
				     &dynamicMdBuf, 
				     const_cast<CompressionType *>( &type ), 
				     kStarchFalse, 
				     reinterpret_cast<const char *>( note )) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
            return STARCH_FATAL_ERROR;
        }
        if (STARCH_mergeMetadataWithCompressedFiles(reinterpret_cast<const Metadata *>( *md ), dynamicMdBuf) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
            return STARCH_FATAL_ERROR;
        }

        if (dynamicMdBuf != nullptr) {
            free(dynamicMdBuf);
            dynamicMdBuf = nullptr;
        }
#else
        if (STARCH_writeJSONMetadata((const Metadata *) *md, 
				     &dynamicMdBuf, 
				     (CompressionType *) &type, 
				     kStarchFalse, 
				     (const char *) note) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not write metadata to buffer\n");
            return STARCH_FATAL_ERROR;
        }
        if (STARCH_mergeMetadataWithCompressedFiles((const Metadata *) *md, dynamicMdBuf) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not merge metadata with compressed streams\n");
            return STARCH_FATAL_ERROR;
        }

        if (dynamicMdBuf != NULL) {
            free(dynamicMdBuf);
            dynamicMdBuf = NULL;
        }
#endif
        else
            return STARCH_FATAL_ERROR;
    }

    /* remove compressed files */
#ifdef __cplusplus
    if (STARCH_deleteCompressedFiles(reinterpret_cast<const Metadata *>( *md )) != STARCH_EXIT_SUCCESS) {
#else
    if (STARCH_deleteCompressedFiles((const Metadata *)*md) != STARCH_EXIT_SUCCESS) {
#endif
        fprintf(stderr, "ERROR: Could not delete compressed streams\n");
        return STARCH_FATAL_ERROR;
    }

    /* cleanup */
    free(prevChromosome);

    return 0;
}

Boolean 
STARCH_fileExists(const char *fn) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_fileExists() ---\n");
#endif
    struct stat buf;
    int i = stat (fn, &buf);
    
    /* 
        Regarding 64-bit support
        cf. http://www.gnu.org/s/libc/manual/html_node/Reading-Attributes.html

        When the sources are compiled with _FILE_OFFSET_BITS == 64 this function is 
        available under the name stat and so transparently replaces the interface for 
        small files on 32-bit machines.
     */

    if (i == 0)
        return kStarchTrue;

    return kStarchFalse;
}

char * 
STARCH_strndup(const char *s, size_t n) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_strndup() ---\n");
#endif
    char *result;
    size_t len = strlen(s);

    if (n < len)
        len = n;

#ifdef __cplusplus
    result = static_cast<char *>( malloc(len + 1) );
    if (!result) {
        return nullptr;
    }
#else
    result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
#endif

    result[len] = '\0';
#ifdef __cplusplus
    return static_cast<char *>( memcpy (result, s, len) );
#else
    return (char *) memcpy (result, s, len);
#endif
}

int 
STARCH2_transformInput(unsigned char **header, Metadata **md, const FILE *inFp, const CompressionType compressionType, const char *tag, const char *note, const Boolean generatePerChrSignatureFlag, const Boolean headerFlag, const Boolean reportProgressFlag, const LineCountType reportProgressN)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH2_transformInput() ---\n");
#endif
    /*
        Overview of Starch rev. 2
        ------------------------------------------------

        We reserve a 4-byte header at the front of the file. The header contains 
        the following data:

        * magic number - the magic number '[ca][5c][ad][e5]' identifies this 
                         as a Starch rev. 2-formatted file 

                         (4 bytes, constant)

        At the end of the file, we write 128 bytes:

        * offset       - a zero-padded 16-digit value marks the byte
                         into the file at which the archive's metadata 
                         starts (including the 4-byte header)

                         (16 bytes, calculated)

        * hash         - a SHA-1 hash of the metadata string, to validate
                         archive integrity

                         (20 bytes, calculated)

        * reserved     - we keep 92 bytes of space free, in case we need it
                         for future purposes

                         (92 bytes, zeros)

        Before these 128 bytes, the compressed, per-chromosome streams start, and 
        we then wrap up by writing the metadata at the end of the file, followed by 
        the footer.
    */

    if (STARCH2_initializeStarchHeader(header) != STARCH_EXIT_SUCCESS) {
        fprintf(stderr, "ERROR: Could not initialize archive header.\n");
        return STARCH_EXIT_FAILURE;
    }

    if (STARCH2_writeStarchHeaderToOutputFp(*header, stdout) != STARCH_EXIT_SUCCESS) {
        fprintf(stderr, "ERROR: Could not write archive header to output file pointer.\n");
        return STARCH_EXIT_FAILURE;
    }

    if (headerFlag == kStarchFalse) {
        if (STARCH2_transformHeaderlessBEDInput(inFp, md, compressionType, tag, note, generatePerChrSignatureFlag, reportProgressFlag, reportProgressN) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not write transformed/compressed data to output file pointer.\n");
            return STARCH_EXIT_FAILURE;
        }
    }
    else {
        if (STARCH2_transformHeaderedBEDInput(inFp, md, compressionType, tag, note, generatePerChrSignatureFlag, reportProgressFlag, reportProgressN) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not write transformed/compressed data to output file pointer.\n");
            return STARCH_EXIT_FAILURE;
        }
    }

    /* 
        1. Read through inFp
        2. Transform a chromosome's worth of data ("record")
        3. Write record to outFp
        4. Add record description to metadata (md)
        5. Repeat 1-4 until EOF of BED input
        6. Calculate JSON string from metadata (md)
        7. Write JSON to outFp
        8. Take SHA-1 hash of JSON string
        9. Write 'offset' and 'hash' values to archive header section of outFp
       10. Close outFp
    */

#ifdef DEBUG
    fprintf(stderr, "\ttag: %s\n\tnote: %s\n", tag, note);
    STARCH2_printStarchHeader(*header);
#endif

    return STARCH_EXIT_SUCCESS;
}

int
STARCH2_transformHeaderedBEDInput(const FILE *inFp, Metadata **md, const CompressionType compressionType, const char *tag, const char *note, const Boolean generatePerChrSignatureFlag, const Boolean reportProgressFlag, const LineCountType reportProgressN)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH2_transformHeaderedBEDInput() ---\n");
#endif
#ifdef __cplusplus
    char *pRemainder = nullptr;
    char *prevChromosome = nullptr;
    char *chromosome = nullptr;
    char *remainder = nullptr;
    char *compressedFn = nullptr;
    Metadata *firstRecord = nullptr;
    char *json = nullptr;
    char *base64EncodedSha1Digest = nullptr;
    BZFILE *bzFp = nullptr;
#else
    char *pRemainder = NULL;
    char *prevChromosome = NULL;
    char *chromosome = NULL;
    char *remainder = NULL;
    char *compressedFn = NULL;
    Metadata *firstRecord = NULL;
    char *json = NULL;
    char *base64EncodedSha1Digest = NULL;
    BZFILE *bzFp = NULL;
#endif
    int c;
    unsigned int cIdx = 0;
    char untransformedBuffer[STARCH_BUFFER_MAX_LENGTH];
    char intermediateBuffer[STARCH_BUFFER_MAX_LENGTH];
    char transformedBuffer[STARCH_BUFFER_MAX_LENGTH];
    unsigned long lineIdx = 0UL;
    int64_t start = 0;
    int64_t stop = 0;
    int64_t pStart = -1;
    int64_t pStop = -1;
    int64_t previousStop = 0;
    int64_t lastPosition = 0;
    int64_t lcDiff = 0;
    int64_t coordDiff = 0;
    Boolean withinChr = kStarchFalse;
    unsigned long totalNonUniqueBases = 0UL;
    unsigned long totalUniqueBases = 0UL;
    Boolean duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
    size_t intermediateBufferLength = 0U;
    size_t currentTransformedBufferLength = 0U;
    size_t recIdx = 0U;
    size_t currentRecSize = 0U;
    size_t cumulativeRecSize = 0U;
    CompressionType type = compressionType;
    unsigned char sha1Digest[STARCH2_MD_FOOTER_SHA1_LENGTH] = {0};
    int zError = -1;
    char zBuffer[STARCH_Z_BUFFER_MAX_LENGTH] = {0};
    z_stream zStream;
    size_t zHave;
    int bzError = BZ_OK;
    unsigned int bzBytesConsumedLo32 = 0U;
    unsigned int bzBytesConsumedHi32 = 0U;
    size_t bzBytesWritten = 0;
    unsigned int bzBytesWrittenLo32 = 0U;
    unsigned int bzBytesWrittenHi32 = 0U;
    FILE *outFp = stdout;
    char footerCumulativeRecordSizeBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + 1] = {0};
    char footerRemainderBuffer[STARCH2_MD_FOOTER_REMAINDER_LENGTH] = {0};
    char footerBuffer[STARCH2_MD_FOOTER_LENGTH] = {0};
    BedLineType lineType = kBedLineTypeUndefined;
    char nonCoordLineBuf[STARCH_BUFFER_MAX_LENGTH] = {0};
    Boolean nonCoordLineBufNeedsPrinting = kStarchFalse;
    char const *nullChr = "null";
    char const *nullCompressedFn = "null";
    char const *nullSig = "null";
    struct sha1_ctx perChromosomeHashCtx;
    LineLengthType maxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
    Boolean previousAndCurrentChromosomesAreIdentical = kStarchTrue;

    /* increment total file size by header bytes */
#ifdef DEBUG
    fprintf(stderr, "\tincrementing file size by sizeof(header)\n");
#endif
    cumulativeRecSize += STARCH2_MD_HEADER_BYTE_LENGTH;

#ifdef __cplusplus
    compressedFn = static_cast<char *>( malloc(STARCH_STREAM_METADATA_FILENAME_MAX_LENGTH) );
#else
    compressedFn = malloc(STARCH_STREAM_METADATA_FILENAME_MAX_LENGTH);
#endif
    if (!compressedFn) {
        fprintf(stderr, "ERROR: Could not allocate space to compressed filename stub\n");
        return STARCH_EXIT_FAILURE;
    }

    /* set up compression streams */
    if (compressionType == kBzip2) {
#ifdef DEBUG
        fprintf(stderr, "\tsetting up bzip2 stream...\n");
#endif
        bzFp = BZ2_bzWriteOpen(&bzError, outFp, STARCH_BZ_COMPRESSION_LEVEL, STARCH_BZ_VERBOSITY, STARCH_BZ_WORKFACTOR);
        if (!bzFp) {
            fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
            return STARCH_EXIT_FAILURE;
        }
        else if (bzError != BZ_OK) {
            switch (bzError) {
                case BZ_CONFIG_ERROR: {
                    fprintf(stderr, "ERROR: Bzip2 library has been miscompiled\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_PARAM_ERROR: {
                    fprintf(stderr, "ERROR: Stream is null, or block size, verbosity and work factor parameters are invalid\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_IO_ERROR: {
                    fprintf(stderr, "ERROR: The value of ferror(outFp) is nonzero -- check outFp\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_MEM_ERROR: {
                    fprintf(stderr, "ERROR: Not enough memory is available\n");
                    return STARCH_EXIT_FAILURE;
                }
                default: {
                    fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteOpen() (err: %d)\n", bzError);
                    return STARCH_EXIT_FAILURE;
                }
            }
        }
    }
    else if (compressionType == kGzip) {
#ifdef DEBUG
        fprintf(stderr, "\tsetting up gzip stream...\n");
#endif
#ifdef __cplusplus
        zStream.zalloc = nullptr;
        zStream.zfree  = nullptr;
        zStream.opaque = nullptr;
#else
        zStream.zalloc = Z_NULL;
        zStream.zfree  = Z_NULL;
        zStream.opaque = Z_NULL;
#endif
        /* cf. http://www.zlib.net/manual.html for level information */
        /* zError = deflateInit2(&zStream, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY); */
        zError = deflateInit(&zStream, STARCH_Z_COMPRESSION_LEVEL);
        switch(zError) {
            case Z_MEM_ERROR: {
                fprintf(stderr, "ERROR: Not enough memory is available\n");
                return STARCH_EXIT_FAILURE;
            }
            case Z_STREAM_ERROR: {
                fprintf(stderr, "ERROR: Gzip initialization parameter is invalid (e.g., invalid method)\n");
                return STARCH_EXIT_FAILURE;
            }
            case Z_VERSION_ERROR: {
                fprintf(stderr, "ERROR: the zlib library version is incompatible with the version assumed by the caller (ZLIB_VERSION)\n");
                return STARCH_EXIT_FAILURE;
            }
            case Z_OK:
            default:
                break;
        }
    }

    if (generatePerChrSignatureFlag) {
        /* set up per-chromosome hash context */
        sha1_init_ctx(&perChromosomeHashCtx);
    }

    /* fill up a "transformation" buffer with data and then compress it */
#ifdef __cplusplus
    while ((c = fgetc(const_cast<FILE *>( inFp ))) != EOF) {
        untransformedBuffer[cIdx] = static_cast<char>( c );
#else
    while ((c = fgetc((FILE *)inFp)) != EOF) {
        untransformedBuffer[cIdx] = (char) c;
#endif
        if (c == '\n') {
            lineIdx++;
            untransformedBuffer[cIdx] = '\0';
            maxStringLength = (maxStringLength >= cIdx) ? maxStringLength : cIdx;

            if (STARCH_createTransformTokens(untransformedBuffer, '\t', &chromosome, &start, &stop, &remainder, &lineType) == 0) {
                if (pRemainder) {
                    previousAndCurrentChromosomesAreIdentical = ((prevChromosome) && (strcmp(chromosome, prevChromosome) == 0)) ? kStarchTrue : kStarchFalse;
                    /* if previous start and stop coordinates are the same, compare the remainder here */
                    if ((start == pStart) && (stop == pStop) && (strcmp(remainder, pRemainder) < 0) && (previousAndCurrentChromosomesAreIdentical)) {
                        fprintf(stderr, "ERROR: (A) Elements with same start and stop coordinates have remainders in wrong sort order.\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\nDebug:\nchromosome [%s] start [%" PRId64 "] stop [%" PRId64 "]\nline [%ld]\nremainder A [%s]\nremainder B [%s]\nstrcmp(A,B) [%d]\n", chromosome, start, stop, lineIdx, remainder, pRemainder, strcmp(remainder, pRemainder));
                        return STARCH_FATAL_ERROR;
                    }
                    free(pRemainder);
#ifdef __cplusplus
                    pRemainder = nullptr;
#else
                    pRemainder = NULL;
#endif
                }

                if ((reportProgressFlag == kStarchTrue) && (lineIdx % reportProgressN == 0)) {
                    fprintf(stderr, "PROGRESS: Transforming element [%ld] of chromosome [%s] -> [%s]\n", lineIdx, chromosome, untransformedBuffer);
                }

                if ( (lineType == kBedLineCoordinates) && ((!prevChromosome) || (strcmp(chromosome, prevChromosome) != 0)) ) 
                {
                    if (prevChromosome) 
                    {
#ifdef __cplusplus
                        if (STARCH_chromosomeInMetadataRecords(reinterpret_cast<const Metadata *>( firstRecord ), chromosome) == STARCH_EXIT_SUCCESS)
#else
                        if (STARCH_chromosomeInMetadataRecords((const Metadata *)firstRecord, chromosome) == STARCH_EXIT_SUCCESS)
#endif
                        {
                            fprintf(stderr, "ERROR: Found same chromosome in earlier portion of file. Possible interleaving issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                            return STARCH_FATAL_ERROR;
                        }
#ifdef __cplusplus
                        if (STARCH_chromosomePositionedBeforeExistingMetadataRecord(reinterpret_cast<const Metadata *>( firstRecord ), chromosome) == STARCH_EXIT_SUCCESS)
#else
                        if (STARCH_chromosomePositionedBeforeExistingMetadataRecord((const Metadata *)firstRecord, chromosome) == STARCH_EXIT_SUCCESS)
#endif
                        {
                            fprintf(stderr, "ERROR: Chromosome name not ordered lexicographically. Possible sorting issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                            return STARCH_FATAL_ERROR;
                        }
                        sprintf(compressedFn, "%s.%s", prevChromosome, tag);
#ifdef DEBUG                        
                        fprintf(stderr, "\t(final-between-chromosome) transformedBuffer:\n%s\n\t\tintermediateBuffer:\n%s\n", transformedBuffer, intermediateBuffer);
#endif
                        if (generatePerChrSignatureFlag) {
                            /* hash the transformed buffer */
                            sha1_process_bytes(transformedBuffer, currentTransformedBufferLength, &perChromosomeHashCtx);
                            sha1_finish_ctx(&perChromosomeHashCtx, sha1Digest);
#ifdef __cplusplus
                            STARCH_encodeBase64(&base64EncodedSha1Digest, 
                                                static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                                                reinterpret_cast<const unsigned char *>( sha1Digest ), 
                                                static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
                            STARCH_encodeBase64(&base64EncodedSha1Digest, 
                                                (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                                                (const unsigned char *) sha1Digest, 
                                                (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
#ifdef DEBUG
                            fprintf(stderr, "\nPROGRESS: SHA-1 digest for chr [%s] is [%s]\n", prevChromosome, base64EncodedSha1Digest);
#endif
                        
                            sha1_init_ctx(&perChromosomeHashCtx);
                        }

                        if (compressionType == kBzip2) {
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) finalizing current chromosome: %s\n", prevChromosome);
#endif
                            /* write transformed buffer to output stream */
#ifdef __cplusplus
                            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, static_cast<int>( currentTransformedBufferLength ));
#else
                            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, (int) currentTransformedBufferLength);
#endif
                            if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_PARAM_ERROR: {
                                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_SEQUENCE_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }

                            /* close bzip2 stream and collect/reset stats */
                            BZ2_bzWriteClose64(&bzError, bzFp, STARCH_BZ_ABANDON, &bzBytesConsumedLo32, &bzBytesConsumedHi32, &bzBytesWrittenLo32, &bzBytesWrittenHi32);
                            if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_SEQUENCE_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }
#ifdef __cplusplus
                            bzBytesWritten = static_cast<size_t>( bzBytesWrittenHi32 ) << 32 | bzBytesWrittenLo32;
#else
                            bzBytesWritten = (size_t) bzBytesWrittenHi32 << 32 | bzBytesWrittenLo32;
#endif
                            cumulativeRecSize += bzBytesWritten;
                            currentRecSize += bzBytesWritten;
                            bzBytesWritten = 0;
                            bzBytesWrittenLo32 = 0U;
                            bzBytesWrittenHi32 = 0U;
#ifdef __cplusplus
                            bzFp = nullptr;
#else
                            bzFp = NULL;
#endif

                            if (STARCH_updateMetadataForChromosome(md, 
                                                                   prevChromosome, 
                                                                   compressedFn, 
                                                                   currentRecSize, 
                                                                   lineIdx, 
                                                                   totalNonUniqueBases, 
                                                                   totalUniqueBases, 
                                                                   duplicateElementExistsFlag, 
                                                                   nestedElementExistsFlag,
                                                                   base64EncodedSha1Digest,
                                                                   maxStringLength) != STARCH_EXIT_SUCCESS) {
                                fprintf(stderr, "ERROR: Could not update metadata %s\n", compressedFn);
                                return STARCH_FATAL_ERROR;
                            }

                            /* start again, anew */
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) resetting bzip2 stream...\n");
#endif
                            bzFp = BZ2_bzWriteOpen(&bzError, outFp, STARCH_BZ_COMPRESSION_LEVEL, STARCH_BZ_VERBOSITY, STARCH_BZ_WORKFACTOR);
                            if (!bzFp) {
                                fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
                                return STARCH_EXIT_FAILURE;
                            }
                            else if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_CONFIG_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 library has been miscompiled\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_PARAM_ERROR: {
                                        fprintf(stderr, "ERROR: Stream is null, or block size, verbosity and work factor parameters are invalid\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: The value of ferror(outFp) is nonzero -- check outFp\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_MEM_ERROR: {
                                        fprintf(stderr, "ERROR: Not enough memory is available\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteOpen() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }
                        }

                        else if (compressionType == kGzip) 
                        {
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) current chromosome: %s\n", prevChromosome);
                            fprintf(stderr, "\t(final-between-chromosome) transformedBuffer:\n%s\n", transformedBuffer);
#endif
#ifdef __cplusplus
                            zStream.next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
                            zStream.avail_in = static_cast<unsigned int>( currentTransformedBufferLength );
#else
                            zStream.next_in = (unsigned char *) transformedBuffer;
                            zStream.avail_in = (unsigned int) currentTransformedBufferLength;
#endif
                            do {
                                zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                                zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                                zStream.next_out = (unsigned char *) zBuffer;
#endif
                                zError = deflate (&zStream, Z_FINISH);
                                switch (zError) {
                                    case Z_MEM_ERROR: {
                                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                        return STARCH_FATAL_ERROR;
                                    }                                    
                                    case Z_BUF_ERROR:
                                    default:
                                        break;
                                }
                                zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                                cumulativeRecSize += zHave;
                                currentRecSize += zHave;
#ifdef DEBUG
                                fprintf(stderr, "\t(final-between-chromosome) writing: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                                fwrite(zBuffer, 1, zHave, stdout);
                                fflush(stdout);
                            } while (zStream.avail_out == 0);
                            assert(zStream.avail_in == 0);

#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) attempting to close z-stream...\n");
#endif
                            zError = deflateEnd(&zStream);
                            switch (zError) {
                                case Z_STREAM_ERROR: {
                                    fprintf(stderr, "ERROR: z-stream state is inconsistent\n");
                                    break;
                                }
                                case Z_DATA_ERROR: {
                                    fprintf(stderr, "ERROR: stream was freed prematurely\n");
                                    break;
                                }
                                case Z_OK:
                                default:
                                    break;
                            }
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) closed z-stream...\n");
                            fprintf(stderr, "\t(final-between-chromosome) updating metadata...\n");
#endif
                            if (STARCH_updateMetadataForChromosome(md, 
                                                                   prevChromosome, 
                                                                   compressedFn, 
                                                                   currentRecSize, 
                                                                   lineIdx, 
                                                                   totalNonUniqueBases, 
                                                                   totalUniqueBases, 
                                                                   duplicateElementExistsFlag, 
                                                                   nestedElementExistsFlag,
                                                                   base64EncodedSha1Digest,
                                                                   maxStringLength) != STARCH_EXIT_SUCCESS) {
                                fprintf(stderr, "ERROR: Could not update metadata %s\n", compressedFn);
                                return STARCH_FATAL_ERROR;
                            }

                            /* begin anew with a fresh compression z-stream */
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) creating fresh z-stream\n");
#endif
#ifdef __cplusplus
                            zStream.zalloc = nullptr;
                            zStream.zfree  = nullptr;
                            zStream.opaque = nullptr;
#else
                            zStream.zalloc = Z_NULL;
                            zStream.zfree  = Z_NULL;
                            zStream.opaque = Z_NULL;
#endif
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) initializing z-stream\n");
#endif
                            /* zError = deflateInit2(&zStream, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY); */
                            zError = deflateInit(&zStream, STARCH_Z_COMPRESSION_LEVEL);
                            switch (zError) {
                                case Z_MEM_ERROR: {
                                    fprintf(stderr, "ERROR: Not enough memory is available\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case Z_STREAM_ERROR: {
                                    fprintf(stderr, "ERROR: Gzip initialization parameter is invalid (e.g., invalid method)\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case Z_VERSION_ERROR: {
                                    fprintf(stderr, "ERROR: the zlib library version is incompatible with the version assumed by the caller (ZLIB_VERSION)\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case Z_OK:
                                default:
                                    break;
                            }
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) initialized z-stream\n");
#endif
                        }

                        /* clean up per-chromosome hash digest */
                        if (base64EncodedSha1Digest) {
                            free(base64EncodedSha1Digest);
#ifdef __cplusplus
                            base64EncodedSha1Digest = nullptr;
#else
                            base64EncodedSha1Digest = NULL;
#endif
                        }
                    }

                    /* create placeholder records at current chromosome */
                    sprintf(compressedFn, "%s.%s", chromosome, tag);
#ifdef DEBUG
                    fprintf(stderr, "\t(final-between-chromosome) creating placeholder md record at chromosome: %s (compressedFn: %s)\n", chromosome, compressedFn);
#endif
                    if (recIdx == 0) {
#ifdef __cplusplus
                        *md = nullptr;
#else
                        *md = NULL;
#endif
                        *md = STARCH_createMetadata(chromosome, 
                                                    compressedFn, 
                                                    0, 
                                                    0UL, 
                                                    0UL, 
                                                    0UL, 
                                                    STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE, 
                                                    STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE,
                                                    base64EncodedSha1Digest,
                                                    maxStringLength);
                        if (!*md) { 
                            fprintf(stderr, "ERROR: Not enough memory is available\n");
                            return STARCH_EXIT_FAILURE;
                        }
                        firstRecord = *md;
                    }
                    else {
                        *md = STARCH_addMetadata(*md, 
                                                 chromosome, 
                                                 compressedFn, 
                                                 0, 
                                                 0UL, 
                                                 0UL, 
                                                 0UL, 
                                                 STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE, 
                                                 STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE,
                                                 base64EncodedSha1Digest,
                                                 maxStringLength);
                    }

                    /* make previous chromosome the current chromosome */
#ifdef __cplusplus
                    if (prevChromosome != nullptr) {
                        free(prevChromosome);
                        prevChromosome = nullptr;
                    }
                    prevChromosome = static_cast<char *>( malloc(strlen(chromosome) + 1) );
                    if (!prevChromosome) {
                        fprintf(stderr, "ERROR: Could not allocate space for previous chromosome marker.");
                        return STARCH_FATAL_ERROR;
                    }
                    strncpy(prevChromosome, reinterpret_cast<const char *>( chromosome ), strlen(chromosome) + 1);
#else
                    if (prevChromosome != NULL) {
                        free(prevChromosome);
                        prevChromosome = NULL;
                    }
                    prevChromosome = malloc(strlen(chromosome) + 1);
                    if (!prevChromosome) {
                        fprintf(stderr, "ERROR: Could not allocate space for previous chromosome marker.");
                        return STARCH_FATAL_ERROR;
                    }
                    strncpy(prevChromosome, (const char *) chromosome, strlen(chromosome) + 1);
#endif

                    /* reset flag, lastPosition and lcDiff, increment record index */
#ifdef DEBUG
                    fprintf(stderr, "\t(final-between-chromosome A) resetting per-chromosome stream transformation parameters...\n");
#endif
                    withinChr = kStarchFalse;
                    lastPosition = 0;
                    pStart = -1;
                    pStop = -1;
                    if (pRemainder) { 
                        free(pRemainder);
#ifdef __cplusplus
                        pRemainder = nullptr; 
#else
                        pRemainder = NULL; 
#endif
                    }
                    previousStop = 0;
                    lcDiff = 0;
                    lineIdx = 0UL;
                    totalNonUniqueBases = 0UL;
                    totalUniqueBases = 0UL;
                    duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
                    nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
                    recIdx++;
                    currentRecSize = 0UL;
                    transformedBuffer[currentTransformedBufferLength] = '\0';
                    currentTransformedBufferLength = 0U;
                    maxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
                }
                else if (lineType == kBedLineCoordinates)
                    withinChr = kStarchTrue;

                if (lineType != kBedLineCoordinates) {
#ifdef __cplusplus
                    strncat(nonCoordLineBuf, reinterpret_cast<const char *>( chromosome ), strlen(chromosome) + 1);
#else
                    strncat(nonCoordLineBuf, (const char *)chromosome, strlen(chromosome) + 1);
#endif
                    nonCoordLineBuf[strlen(nonCoordLineBuf)] = '\n';
                    nonCoordLineBufNeedsPrinting = kStarchTrue;
                }
                else {
                    if (nonCoordLineBufNeedsPrinting == kStarchTrue) {
                        sprintf(intermediateBuffer + strlen(intermediateBuffer), "%s", nonCoordLineBuf);
                        memset(nonCoordLineBuf, 0, strlen(nonCoordLineBuf));
                        nonCoordLineBufNeedsPrinting = kStarchFalse;
                    }

                    /* test for out-of-order element */
                    if (pStart > start) {
                        fprintf(stderr, "ERROR: BED data is not properly sorted by start coordinates at line %lu [ pStart: %" PRId64 " | start: %" PRId64 " ]\n", lineIdx, pStart, start);
                        exit (EXIT_FAILURE);
                    }
                    else if ((pStart == start) && (pStop > stop)) {
                        fprintf(stderr, "ERROR: BED data is not properly sorted by end coordinates (when start coordinates are equal) at line %lu\n", lineIdx);
                        exit (EXIT_FAILURE);
                    }

                    if (stop > start)
                        coordDiff = stop - start;
                    else {
                        fprintf(stderr, "ERROR: (B) BED data is corrupt at line %lu (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, stop, start);
                        return STARCH_FATAL_ERROR;
                    }
                    if (coordDiff != lcDiff) {
                        lcDiff = coordDiff;
                        sprintf(intermediateBuffer + strlen(intermediateBuffer), "p%" PRId64 "\n", coordDiff);
                    }
                    if (lastPosition != 0) {
                        if (remainder)
                            sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", (start - lastPosition), remainder);
                        else
                            sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", (start - lastPosition));
                    }
                    else {
                        if (remainder)
                            sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", start, remainder);
                        else 
                            sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", start);
                    }
                    intermediateBufferLength = strlen(intermediateBuffer);

                    if ((currentTransformedBufferLength + intermediateBufferLength) < STARCH_BUFFER_MAX_LENGTH) {
                        /* append intermediateBuffer to transformedBuffer */
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) appending intermediateBuffer to transformedBuffer (old currentTransformedBufferLength: %lu)\n%s\n", currentTransformedBufferLength, intermediateBuffer);
#endif
                        memcpy(transformedBuffer + currentTransformedBufferLength, intermediateBuffer, intermediateBufferLength);
                        currentTransformedBufferLength += intermediateBufferLength;
                        transformedBuffer[currentTransformedBufferLength] = '\0';
                        memset(intermediateBuffer, 0, intermediateBufferLength + 1);
                    }
                    else {
                        /* compress transformedBuffer[] and send to stdout */
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) to be compressed -- transformedBuffer:\n%s\n", transformedBuffer);
#endif                    
                        if (compressionType == kBzip2) {
#ifdef DEBUG
                            fprintf(stderr, "\t(intermediate) current chromosome: %s\n", prevChromosome);
#endif
#ifdef __cplusplus
                            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, static_cast<int>( currentTransformedBufferLength ));
#else
                            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, (int) currentTransformedBufferLength);
#endif
                            if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_PARAM_ERROR: {
                                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_SEQUENCE_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }                        
                        }
                        else if (compressionType == kGzip) {
#ifdef DEBUG
                            fprintf(stderr, "\t(intermediate) current chromosome: %s\n", prevChromosome);
#endif
#ifdef __cplusplus
                            zStream.next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
                            zStream.avail_in = static_cast<unsigned int>( currentTransformedBufferLength );
#else
                            zStream.next_in = (unsigned char *) transformedBuffer;
                            zStream.avail_in = (unsigned int) currentTransformedBufferLength;
#endif
                            do {
                                zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                                zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                                zStream.next_out = (unsigned char *) zBuffer;
#endif
                                zError = deflate (&zStream, Z_NO_FLUSH);
                                switch (zError) {
                                    case Z_MEM_ERROR: {
                                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                        return STARCH_FATAL_ERROR;
                                    }
                                    case Z_BUF_ERROR:
                                    default:
                                        break;
                                }
                                zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                                cumulativeRecSize += zHave;
                                currentRecSize += zHave;
#ifdef DEBUG
                                fprintf(stderr, "\t(intermediate) written: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                                fwrite(zBuffer, 1, zHave, stdout);
                                fflush(stdout);
                            } while (zStream.avail_out == 0);

#ifdef __cplusplus
                            zStream.next_in = reinterpret_cast<unsigned char *>( intermediateBuffer );
                            zStream.avail_in = static_cast<unsigned int>( strlen(intermediateBuffer) );
#else
                            zStream.next_in = (unsigned char *) intermediateBuffer;
                            zStream.avail_in = (unsigned int) strlen(intermediateBuffer);
#endif
                            do {
                                zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                                zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                                zStream.next_out = (unsigned char *) zBuffer;
#endif
                                zError = deflate (&zStream, Z_NO_FLUSH);
                                switch (zError) {
                                    case Z_MEM_ERROR: {
                                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                        return STARCH_FATAL_ERROR;
                                    }
                                    case Z_BUF_ERROR:
                                    default:
                                        break;
                                }
                                zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                                cumulativeRecSize += zHave;
                                currentRecSize += zHave;
#ifdef DEBUG
                                fprintf(stderr, "\t(intermediate) written: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                                fwrite(zBuffer, 1, zHave, stdout);
                                fflush(stdout);
                            } while (zStream.avail_out == 0);
                        }

                        memcpy(transformedBuffer, intermediateBuffer, strlen(intermediateBuffer) + 1);
                        currentTransformedBufferLength = strlen(intermediateBuffer);
                        memset(intermediateBuffer, 0, strlen(intermediateBuffer) + 1);
                        intermediateBufferLength = 0;
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) end-of-loop: transformedBuffer:\n%s\n\t\tintermediateBuffer:\n%s\n", transformedBuffer, intermediateBuffer);
#endif
                    }

                    lastPosition = stop;
#ifdef __cplusplus
                    totalNonUniqueBases += static_cast<BaseCountType>( stop - start );
                    if (previousStop <= start)
                        totalUniqueBases += static_cast<BaseCountType>( stop - start );
                    else if (previousStop < stop)
                        totalUniqueBases += static_cast<BaseCountType>( stop - previousStop );
#else
                    totalNonUniqueBases += (BaseCountType) (stop - start);
                    if (previousStop <= start)
                        totalUniqueBases += (BaseCountType) (stop - start);
                    else if (previousStop < stop)
                        totalUniqueBases += (BaseCountType) (stop - previousStop);
#endif
                    previousStop = (stop > previousStop) ? stop : previousStop;

#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) start: %" PRId64 "\tpStart: %" PRId64 "\tstop: %" PRId64 "\tpStop: %" PRId64 "\n", start, pStart, stop, pStop);
#ifdef __cplusplus
                    fprintf(stderr, "\t(intermediate) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", static_cast<int>( duplicateElementExistsFlag ), static_cast<int>( nestedElementExistsFlag ));
#else
                    fprintf(stderr, "\t(intermediate) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", (int) duplicateElementExistsFlag, (int) nestedElementExistsFlag);
#endif
#endif

                    /* test for duplicate element */
                    if ((pStart == start) && (pStop == stop))
                        duplicateElementExistsFlag = kStarchTrue;
                    
                    /* test for nested element */
                    if ((pStart < start) && (pStop > stop))
                        nestedElementExistsFlag = kStarchTrue;
                
                    /* set pElement values */
                    pStart = start;
                    pStop = stop;
                    if (pRemainder) { 
                        free(pRemainder);
#ifdef __cplusplus
                        pRemainder = nullptr;
#else
                        pRemainder = NULL;
#endif
                    }
                    if (remainder)
                        pRemainder = STARCH_strndup(remainder, strlen(remainder));
                }

                if (withinChr == kStarchTrue) {
                    free(chromosome);
#ifdef __cplusplus
                    chromosome = nullptr;
#else
                    chromosome = NULL;
#endif
                }
                if (remainder) {
                    free(remainder);
#ifdef __cplusplus
                    remainder = nullptr;
#else
                    remainder = NULL;
#endif
                }
                cIdx = 0;
            }
            else {
                fprintf(stderr, "ERROR: BED data could not be transformed.\n");
                return STARCH_FATAL_ERROR;
            }
        }
        else
            cIdx++;
    }

    /* if we don't have a trailing newline in BED input, then we have reached EOF before we can process a line, so we try that now */

    if (cIdx > 0) {
        untransformedBuffer[cIdx] = '\0';
#ifdef __cplusplus
        maxStringLength = (maxStringLength >= static_cast<LineLengthType>(strlen(untransformedBuffer))) ? maxStringLength : static_cast<LineLengthType>(strlen(untransformedBuffer));
#else
        maxStringLength = (maxStringLength >= (LineLengthType) strlen(untransformedBuffer)) ? maxStringLength : (LineLengthType) strlen(untransformedBuffer);   
#endif
        if (STARCH_createTransformTokensForHeaderlessInput(untransformedBuffer, '\t', &chromosome, &start, &stop, &remainder) == 0)  {
#ifdef DEBUG
            fprintf(stderr, "\t(just-before-last-pass) untransformedBuffer:\n%s\n", untransformedBuffer);
#endif

            /* test for out-of-order element */
            if (pStart > start) {
                fprintf(stderr, "ERROR: BED data is not properly sorted by start coordinates at line %lu [ pStart: %" PRId64 " | start: %" PRId64 " ]\n", lineIdx, pStart, start);
                exit (EXIT_FAILURE);
            }
            else if ((pStart == start) && (pStop > stop)) {
                fprintf(stderr, "ERROR: BED data is not properly sorted by end coordinates (when start coordinates are equal) at line %lu\n", lineIdx);
                exit (EXIT_FAILURE);
            }

            /* transform */
            if (stop > start)
                coordDiff = stop - start;
            else {
                fprintf(stderr, "ERROR: (C) BED data is corrupt at line %lu (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, stop, start);
                return STARCH_FATAL_ERROR;
            }
            if (coordDiff != lcDiff) {
                lcDiff = coordDiff;
                sprintf(intermediateBuffer + strlen(intermediateBuffer), "p%" PRId64 "\n", coordDiff);
            }
            if (lastPosition != 0) {
                if (remainder)
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", (start - lastPosition), remainder);
                else
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", (start - lastPosition));
            }
            else {
                if (remainder)
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", start, remainder);
                else
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", start);
            }
            intermediateBufferLength = strlen(intermediateBuffer);
            
            /* append intermediateBuffer to transformedBuffer */

            memcpy(transformedBuffer + currentTransformedBufferLength, intermediateBuffer, intermediateBufferLength);
            currentTransformedBufferLength += intermediateBufferLength;
            transformedBuffer[currentTransformedBufferLength] = '\0';
            memset(intermediateBuffer, 0, intermediateBufferLength + 1);
            
            lastPosition = stop;
#ifdef __cplusplus
            totalNonUniqueBases += static_cast<BaseCountType>( stop - start );
            if (previousStop <= start)
                totalUniqueBases += static_cast<BaseCountType>( stop - start );
            else if (previousStop < stop) 
                totalUniqueBases += static_cast<BaseCountType>( stop - previousStop );
#else
            totalNonUniqueBases += (BaseCountType) (stop - start);
            if (previousStop <= start)
                totalUniqueBases += (BaseCountType) (stop - start);
            else if (previousStop < stop) 
                totalUniqueBases += (BaseCountType) (stop - previousStop);
#endif
            previousStop = (stop > previousStop) ? stop : previousStop;

#ifdef DEBUG
            fprintf(stderr, "\t(just-before-last-pass) start: %" PRId64 "\tpStart: %" PRId64 "\tstop: %" PRId64 "\tpStop: %" PRId64 "\n", start, pStart, stop, pStop);
#ifdef __cplusplus
            fprintf(stderr, "\t(just-before-last-pass) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", static_cast<int>( duplicateElementExistsFlag ), static_cast<int>( nestedElementExistsFlag ));
#else
            fprintf(stderr, "\t(just-before-last-pass) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", (int) duplicateElementExistsFlag, (int) nestedElementExistsFlag);
#endif
#endif
            /* test for duplicate element */
            if ((pStart == start) && (pStop == stop))
                duplicateElementExistsFlag = kStarchTrue;

            /* test for nested element */
            if ((pStart < start) && (pStop > stop))
                nestedElementExistsFlag = kStarchTrue;

            if (pRemainder) {
                previousAndCurrentChromosomesAreIdentical = ((prevChromosome) && (strcmp(chromosome, prevChromosome) == 0)) ? kStarchTrue : kStarchFalse;
                /* if previous start and stop coordinates are the same, compare the remainder here */
                if ((start == pStart) && (stop == pStop) && (strcmp(remainder, pRemainder) < 0) && (previousAndCurrentChromosomesAreIdentical)) {
                    fprintf(stderr, "ERROR: (B) Elements with same start and stop coordinates have remainders in wrong sort order.\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\nDebug:\nchromosome [%s] start [%" PRId64 "] stop [%" PRId64 "]\nline [%ld]\nremainder A [%s]\nremainder B [%s]\nstrcmp(A,B) [%d]\n", chromosome, start, stop, lineIdx, remainder, pRemainder, strcmp(remainder, pRemainder));
                    return STARCH_FATAL_ERROR;
                }
                free(pRemainder);
#ifdef __cplusplus
                pRemainder = nullptr;
#else
                pRemainder = NULL;
#endif
            }
        }
        else {
            fprintf(stderr, "ERROR: (D) BED data is corrupt at line %lu\n", lineIdx);
            return STARCH_FATAL_ERROR;
        }
    }
    
    lineIdx++;
    sprintf(compressedFn, "%s.%s", prevChromosome, tag);

#ifdef DEBUG
    fprintf(stderr, "\t(last-pass) transformedBuffer:\n%s\n\t\tintermediateBuffer:\n%s\n", transformedBuffer, intermediateBuffer);
#endif

    if (generatePerChrSignatureFlag) {
        /* hash the transformed buffer */
        sha1_process_bytes(transformedBuffer, currentTransformedBufferLength, &perChromosomeHashCtx);
        sha1_finish_ctx(&perChromosomeHashCtx, sha1Digest);
#ifdef __cplusplus
        STARCH_encodeBase64(&base64EncodedSha1Digest, 
                            static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                            reinterpret_cast<const unsigned char *>( sha1Digest ), 
                            static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
        STARCH_encodeBase64(&base64EncodedSha1Digest, 
                            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                            (const unsigned char *) sha1Digest, 
                            (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
#ifdef DEBUG
        fprintf(stderr, "\nPROGRESS: SHA-1 digest for chr [%s] is [%s]\n", prevChromosome, base64EncodedSha1Digest);
#endif
    }

    /* last-pass, bzip2 */
    if (compressionType == kBzip2) {
#ifdef DEBUG
        fprintf(stderr, "\t(last-pass) current chromosome: %s\n", prevChromosome);
#endif
        if (currentTransformedBufferLength > 0) 
        {
#ifdef __cplusplus
            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, static_cast<int>( currentTransformedBufferLength ));
#else
            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, (int) currentTransformedBufferLength);
#endif
            if (bzError != BZ_OK) {
                switch (bzError) {
                    case BZ_PARAM_ERROR: {
                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case BZ_SEQUENCE_ERROR: {
                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case BZ_IO_ERROR: {
                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    default: {
                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                        return STARCH_EXIT_FAILURE;
                    }
                }
            }
        }
#ifdef DEBUG
        fprintf(stderr, "\t(last-pass) attempting to close bzip2-stream...\n");
#endif
        BZ2_bzWriteClose64(&bzError, bzFp, STARCH_BZ_ABANDON, &bzBytesConsumedLo32, &bzBytesConsumedHi32, &bzBytesWrittenLo32, &bzBytesWrittenHi32);
        if (bzError != BZ_OK) {
            switch (bzError) {
                case BZ_PARAM_ERROR: {
                    fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_SEQUENCE_ERROR: {
                    fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_IO_ERROR: {
                    fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                    return STARCH_EXIT_FAILURE;
                }
                default: {
                    fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                    return STARCH_EXIT_FAILURE;
                }
            }
        }
#ifdef __cplusplus
        bzBytesWritten = static_cast<size_t>( bzBytesWrittenHi32 ) << 32 | bzBytesWrittenLo32;
#else
        bzBytesWritten = (size_t) bzBytesWrittenHi32 << 32 | bzBytesWrittenLo32;
#endif
        cumulativeRecSize += bzBytesWritten;
        currentRecSize += bzBytesWritten;

#ifdef DEBUG
        fprintf(stderr, "\t(last-pass) closed bzip2-stream...\n");
#endif
    }

    /* last-pass, gzip */
    else if (compressionType == kGzip) {
#ifdef DEBUG
        /*fprintf(stderr, "\t(last-pass) to be compressed - transformedBuffer:\n%s\n", transformedBuffer);*/
#endif        
        if (currentTransformedBufferLength > 0) 
        {
#ifdef DEBUG
            fprintf(stderr, "\t(last-pass) current chromosome: %s\n", prevChromosome);
#endif
#ifdef __cplusplus
            zStream.next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
            zStream.avail_in = static_cast<unsigned int>( currentTransformedBufferLength );
#else
            zStream.next_in = (unsigned char *) transformedBuffer;
            zStream.avail_in = (unsigned int) currentTransformedBufferLength;
#endif
            do {
                zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                zStream.next_out = (unsigned char *) zBuffer;
#endif
                zError = deflate(&zStream, Z_FINISH);
                switch (zError) {
                    case Z_MEM_ERROR: {
                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                        return STARCH_FATAL_ERROR;
                    }
                    case Z_BUF_ERROR:
                    default:
                        break;
                }
                zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                cumulativeRecSize += zHave;
                currentRecSize += zHave;
#ifdef DEBUG
                fprintf(stderr, "\t(last-pass) written: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                fwrite(zBuffer, 1, zHave, stdout);
                fflush(stdout);
            } while (zStream.avail_out == 0);
#ifdef DEBUG
            fprintf(stderr, "\t(last-pass) attempting to close z-stream...\n");
#endif
            deflateEnd(&zStream);
#ifdef DEBUG
            fprintf(stderr, "\t(last-pass) closed z-stream...\n");
#endif
        }
    }

#ifdef DEBUG
    fprintf(stderr, "\t(last-pass) updating last md record...\n");
#endif
    if (STARCH_updateMetadataForChromosome(md, 
                                           prevChromosome, 
                                           compressedFn, 
                                           currentRecSize, 
                                           lineIdx, 
                                           totalNonUniqueBases, 
                                           totalUniqueBases, 
                                           duplicateElementExistsFlag, 
                                           nestedElementExistsFlag,
                                           base64EncodedSha1Digest,
                                           maxStringLength) != STARCH_EXIT_SUCCESS) {
        /* 
           If the stream or input file contains no BED records, then the Metadata pointer md will
           be NULL, as will the char pointer prevChromosome. So we put in a stub metadata record.
        */
        lineIdx = 0;
#ifdef __cplusplus
        *md = nullptr;
#else
        *md = NULL;
#endif
        *md = STARCH_createMetadata(nullChr, 
                                    nullCompressedFn, 
                                    currentRecSize, 
                                    lineIdx, 
                                    0UL, 
                                    0UL, 
                                    STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE, 
                                    STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE, 
                                    nullSig,
                                    0UL);
        if (!*md) {
            fprintf(stderr, "ERROR: Not enough memory is available\n");
            return STARCH_EXIT_FAILURE;
        }
        firstRecord = *md;
    }

    /* clean up per-chromosome hash digest */
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest);
#ifdef __cplusplus
        base64EncodedSha1Digest = nullptr;
#else
        base64EncodedSha1Digest = NULL;
#endif
    }

    /* reset metadata pointer */
    *md = firstRecord;

    /* write metadata */
#ifdef DEBUG
    fprintf(stderr, "\twriting md to output stream (as JSON)...\n");
#endif
    /* this is the custom header version of the parser, so we set headerFlag to TRUE */
    STARCH_writeJSONMetadata(*md, &json, &type, kStarchTrue, note);
    fwrite(json, 1, strlen(json), stdout);
    fflush(stdout);

    /* write metadata signature */
#ifdef DEBUG
    fprintf(stderr, "\twriting md signature...\n");
#endif
#ifdef __cplusplus
    STARCH_SHA1_All(reinterpret_cast<const unsigned char *>( json ), strlen(json), sha1Digest);
#else
    STARCH_SHA1_All((const unsigned char *)json, strlen(json), sha1Digest);
#endif

    /* encode signature in base64 encoding */
#ifdef DEBUG
    fprintf(stderr, "\tencoding md signature...\n");
#endif
#ifdef __cplusplus
    STARCH_encodeBase64(&base64EncodedSha1Digest, 
                        static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                        reinterpret_cast<const unsigned char *>( sha1Digest ), 
                        static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ));
#else
    STARCH_encodeBase64(&base64EncodedSha1Digest, 
                        (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                        (const unsigned char *) sha1Digest, 
                        (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif

    /* build footer */
#ifdef DEBUG
#ifdef __cplusplus
    fprintf(stderr, "\tWARNING:\nmdLength: %llu\nmd   - [%s]\nsha1 - [%s]\n", static_cast<unsigned long long>( strlen(json) ), json, sha1Digest);
#else
    fprintf(stderr, "\tWARNING:\nmdLength: %llu\nmd   - [%s]\nsha1 - [%s]\n", (unsigned long long) strlen(json), json, sha1Digest);
#endif
    fprintf(stderr, "\twriting offset and signature to output stream...\n");
#endif
#ifdef __cplusplus
    sprintf(footerCumulativeRecordSizeBuffer, "%020llu", static_cast<unsigned long long>( cumulativeRecSize )); /* we cast this size_t to an unsigned long long in order to allow warning-free compilation with an ISO C++ compiler like g++ */
#else
    sprintf(footerCumulativeRecordSizeBuffer, "%020llu", (unsigned long long) cumulativeRecSize); /* we cast this size_t to an unsigned long long in order to allow warning-free compilation with an ISO C++ compiler like g++ */
#endif
#ifdef DEBUG
    fprintf(stderr, "\tfooterCumulativeRecordSizeBuffer: %s\n", footerCumulativeRecordSizeBuffer);
#endif
    memcpy(footerBuffer, footerCumulativeRecordSizeBuffer, strlen(footerCumulativeRecordSizeBuffer));
    memcpy(footerBuffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH, base64EncodedSha1Digest, STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1); /* strip trailing null */
#ifdef __cplusplus
    memset(footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, static_cast<size_t>( STARCH2_MD_FOOTER_REMAINDER_LENGTH ));
#else
    memset(footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, (size_t) STARCH2_MD_FOOTER_REMAINDER_LENGTH);
#endif
#ifdef DEBUG
    fprintf(stderr, "\tfooterRemainderBuffer: [%s]\n", footerRemainderBuffer);
#endif
    memcpy(footerBuffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1, footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_LENGTH); /* don't forget to offset pointer index by -1 for base64-sha1's null */
    footerBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 1] = '\0';
    footerBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 2] = '\n';
    fprintf(stdout, "%s", footerBuffer);
    fflush(stdout);

#ifdef __cplusplus
    if (json) {
        free(json);
        json = nullptr;
    }
    if (compressedFn) {
        free(compressedFn);
        compressedFn = nullptr;
    }
    if (prevChromosome) {
        free(prevChromosome);
        prevChromosome = nullptr;
    }
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest);
        base64EncodedSha1Digest = nullptr;
    }
#else
    if (json) {
        free(json);
        json = NULL;
    }
    if (compressedFn) {
        free(compressedFn);
        compressedFn = NULL;
    }
    if (prevChromosome) {
        free(prevChromosome);
        prevChromosome = NULL;
    }
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest);
        base64EncodedSha1Digest = NULL;
    }
#endif

    return STARCH_EXIT_SUCCESS;
}

int
STARCH2_transformHeaderlessBEDInput(const FILE *inFp, Metadata **md, const CompressionType compressionType, const char *tag, const char *note, const Boolean generatePerChrSignatureFlag, const Boolean reportProgressFlag, const LineCountType reportProgressN)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH2_transformHeaderlessBEDInput() ---\n");
#endif
#ifdef __cplusplus
    char *pRemainder = nullptr;
    char *prevChromosome = nullptr;
    char *chromosome = nullptr;
    char *remainder = nullptr;
    char *compressedFn = nullptr;
    Metadata *firstRecord = nullptr;
    char *json = nullptr;
    char *jsonCopy = nullptr;
    char *base64EncodedSha1Digest = nullptr;
    BZFILE *bzFp = nullptr;
#else
    char *pRemainder = NULL;
    char *prevChromosome = NULL;
    char *chromosome = NULL;
    char *remainder = NULL;
    char *compressedFn = NULL;
    Metadata *firstRecord = NULL;
    char *json = NULL;
    char *jsonCopy = NULL;
    char *base64EncodedSha1Digest = NULL;
    BZFILE *bzFp = NULL;
#endif
    int c;
    unsigned int cIdx = 0;
    char untransformedBuffer[STARCH_BUFFER_MAX_LENGTH + 1] = {0};
    char intermediateBuffer[STARCH_BUFFER_MAX_LENGTH + 1] = {0};
    char transformedBuffer[STARCH_BUFFER_MAX_LENGTH + 1] = {0};
    unsigned long lineIdx = 0UL;
    int64_t start = 0;
    int64_t stop = 0;
    int64_t pStart = -1;
    int64_t pStop = -1;
    int64_t previousStop = 0;
    int64_t lastPosition = 0;
    int64_t lcDiff = 0;
    int64_t coordDiff = 0;
    Boolean withinChr = kStarchFalse;
    unsigned long totalNonUniqueBases = 0UL;
    unsigned long totalUniqueBases = 0UL;
    Boolean duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
    size_t intermediateBufferLength = 0U;
    size_t currentTransformedBufferLength = 0U;
    size_t recIdx = 0U;
    size_t currentRecSize = 0U;
    size_t cumulativeRecSize = 0U;
    CompressionType type = compressionType;
    unsigned char sha1Digest[STARCH2_MD_FOOTER_SHA1_LENGTH] = {0};
    int zError = -1;
    char zBuffer[STARCH_Z_BUFFER_MAX_LENGTH] = {0};
    z_stream zStream;
    size_t zHave;
    int bzError = BZ_OK;
    unsigned int bzBytesConsumedLo32 = 0U;
    unsigned int bzBytesConsumedHi32 = 0U;
    size_t bzBytesWritten = 0;
    unsigned int bzBytesWrittenLo32 = 0U;
    unsigned int bzBytesWrittenHi32 = 0U;
    FILE *outFp = stdout;
    char footerCumulativeRecordSizeBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + 1] = {0};
    char footerRemainderBuffer[STARCH2_MD_FOOTER_REMAINDER_LENGTH] = {0};
    char footerBuffer[STARCH2_MD_FOOTER_LENGTH] = {0};
    char const *nullChr = "null";
    char const *nullCompressedFn = "null";
    char const *nullSig = "null";
    struct sha1_ctx perChromosomeHashCtx;
    LineLengthType maxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
    Boolean previousAndCurrentChromosomesAreIdentical = kStarchTrue;

    /* increment total file size by header bytes */
#ifdef DEBUG
    fprintf(stderr, "\tincrementing file size by sizeof(header)\n");
#endif
    cumulativeRecSize += STARCH2_MD_HEADER_BYTE_LENGTH;

#ifdef __cplusplus
    compressedFn = static_cast<char *>( malloc(STARCH_STREAM_METADATA_FILENAME_MAX_LENGTH) );
#else
    compressedFn = malloc(STARCH_STREAM_METADATA_FILENAME_MAX_LENGTH);
#endif
    if (!compressedFn) {
        fprintf(stderr, "ERROR: Could not allocate space to compressed filename stub\n");
        return STARCH_EXIT_FAILURE;
    }

    /* set up compression streams */
    if (compressionType == kBzip2) {
#ifdef DEBUG
        fprintf(stderr, "\tsetting up bzip2 stream...\n");
#endif
        bzFp = BZ2_bzWriteOpen(&bzError, outFp, STARCH_BZ_COMPRESSION_LEVEL, STARCH_BZ_VERBOSITY, STARCH_BZ_WORKFACTOR);
        if (!bzFp) {
            fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
            return STARCH_EXIT_FAILURE;
        }
        else if (bzError != BZ_OK) {
            switch (bzError) {
                case BZ_CONFIG_ERROR: {
                    fprintf(stderr, "ERROR: Bzip2 library has been miscompiled\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_PARAM_ERROR: {
                    fprintf(stderr, "ERROR: Stream is null, or block size, verbosity and work factor parameters are invalid\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_IO_ERROR: {
                    fprintf(stderr, "ERROR: The value of ferror(outFp) is nonzero -- check outFp\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_MEM_ERROR: {
                    fprintf(stderr, "ERROR: Not enough memory is available\n");
                    return STARCH_EXIT_FAILURE;
                }
                default: {
                    fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteOpen() (err: %d)\n", bzError);
                    return STARCH_EXIT_FAILURE;
                }
            }
        }
    }
    else if (compressionType == kGzip) {
#ifdef DEBUG
        fprintf(stderr, "\tsetting up gzip stream...\n");
#endif
#ifdef __cplusplus
        zStream.zalloc = nullptr;
        zStream.zfree  = nullptr;
        zStream.opaque = nullptr;
#else
        zStream.zalloc = Z_NULL;
        zStream.zfree  = Z_NULL;
        zStream.opaque = Z_NULL;
#endif
        /* cf. http://www.zlib.net/manual.html for level information */
        /* zError = deflateInit2(&zStream, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY); */
        zError = deflateInit(&zStream, STARCH_Z_COMPRESSION_LEVEL);
        switch(zError) {
            case Z_MEM_ERROR: {
                fprintf(stderr, "ERROR: Not enough memory is available\n");
                return STARCH_EXIT_FAILURE;
            }
            case Z_STREAM_ERROR: {
                fprintf(stderr, "ERROR: Gzip initialization parameter is invalid (e.g., invalid method)\n");
                return STARCH_EXIT_FAILURE;
            }
            case Z_VERSION_ERROR: {
                fprintf(stderr, "ERROR: the zlib library version is incompatible with the version assumed by the caller (ZLIB_VERSION)\n");
                return STARCH_EXIT_FAILURE;
            }
            case Z_OK:
            default:
                break;
        }
    }

    if (generatePerChrSignatureFlag) {
        /* set up per-chromosome hash context */
        sha1_init_ctx(&perChromosomeHashCtx);
    }

    /* fill up a "transformation" buffer with data and then compress it */
#ifdef __cplusplus
    while ((c = fgetc(const_cast<FILE *>( inFp ))) != EOF) {
        untransformedBuffer[cIdx] = static_cast<char>( c );
#else
    while ((c = fgetc((FILE *)inFp)) != EOF) {
        untransformedBuffer[cIdx] = (char) c;
#endif
        if (c == '\n') {
            lineIdx++;
            untransformedBuffer[cIdx] = '\0';
            maxStringLength = (maxStringLength >= cIdx) ? maxStringLength : cIdx;

            if (STARCH_createTransformTokensForHeaderlessInput(untransformedBuffer, '\t', &chromosome, &start, &stop, &remainder) == 0) {
#ifdef DEBUG                        
                fprintf(stderr, "\t(tokens) chromosome: [%s] start [%" PRId64 "] stop [%" PRId64 "] remainder [%s]\n", chromosome, start, stop, remainder);
#endif
                if (pRemainder) {
                    previousAndCurrentChromosomesAreIdentical = ((prevChromosome) && (strcmp(chromosome, prevChromosome) == 0)) ? kStarchTrue : kStarchFalse;
                    /* if previous start and stop coordinates are the same, compare the remainder here */
                    if ((start == pStart) && (stop == pStop) && (strcmp(remainder, pRemainder) < 0) && (previousAndCurrentChromosomesAreIdentical)) {
                        fprintf(stderr, "ERROR: (C) Elements with same start and stop coordinates have remainders in wrong sort order.\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\nDebug:\nchromosome [%s] start [%" PRId64 "] stop [%" PRId64 "]\nline [%ld]\nremainder A [%s]\nremainder B [%s]\nstrcmp(A,B) [%d]\n", chromosome, start, stop, lineIdx, remainder, pRemainder, strcmp(remainder, pRemainder));
                        return STARCH_FATAL_ERROR;
                    }
                    free(pRemainder);
#ifdef __cplusplus
                    pRemainder = nullptr;
#else
                    pRemainder = NULL;
#endif
                }

                if ((reportProgressFlag == kStarchTrue) && (lineIdx % reportProgressN == 0)) {
                    fprintf(stderr, "PROGRESS: Transforming element [%ld] of chromosome [%s] -> [%s]\n", lineIdx, chromosome, untransformedBuffer);
                }

                if ( (!prevChromosome) || (strcmp(chromosome, prevChromosome) != 0) ) 
                {
                    if (prevChromosome) 
                    {
#ifdef __cplusplus
                        if (STARCH_chromosomeInMetadataRecords(reinterpret_cast<const Metadata *>( firstRecord ), chromosome) == STARCH_EXIT_SUCCESS)
#else
                        if (STARCH_chromosomeInMetadataRecords((const Metadata *)firstRecord, chromosome) == STARCH_EXIT_SUCCESS)
#endif
                        {
                            fprintf(stderr, "ERROR: Found same chromosome in earlier portion of file. Possible interleaving issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                            return STARCH_FATAL_ERROR;
                        }
#ifdef __cplusplus
                        if (STARCH_chromosomePositionedBeforeExistingMetadataRecord(reinterpret_cast<const Metadata *>( firstRecord ), chromosome) == STARCH_EXIT_SUCCESS)
#else
                        if (STARCH_chromosomePositionedBeforeExistingMetadataRecord((const Metadata *)firstRecord, chromosome) == STARCH_EXIT_SUCCESS)
#endif
                        {
                            fprintf(stderr, "ERROR: Chromosome name not ordered lexicographically. Possible sorting issue?\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\n");
                            return STARCH_FATAL_ERROR;
                        }
                        sprintf(compressedFn, "%s.%s", prevChromosome, tag);
#ifdef DEBUG                        
                        fprintf(stderr, "\t(final-between-chromosome) transformedBuffer before hash:\n[%s]\n", transformedBuffer);
#endif
                        if (generatePerChrSignatureFlag) {
                            /* hash the transformed buffer */
                            sha1_process_bytes(transformedBuffer, currentTransformedBufferLength, &perChromosomeHashCtx);
                            sha1_finish_ctx(&perChromosomeHashCtx, sha1Digest);
#ifdef __cplusplus
                            STARCH_encodeBase64(&base64EncodedSha1Digest, 
                                                static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                                                reinterpret_cast<const unsigned char *>( sha1Digest ), 
                                                static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
                            STARCH_encodeBase64(&base64EncodedSha1Digest, 
                                                (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                                                (const unsigned char *) sha1Digest, 
                                                (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
#ifdef DEBUG
                            fprintf(stderr, "\nPROGRESS: SHA-1 digest for chr [%s] is [%s]\n", prevChromosome, base64EncodedSha1Digest);
#endif
                            sha1_init_ctx(&perChromosomeHashCtx);
                        }

                        if (compressionType == kBzip2) {
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) finalizing current chromosome: %s\n", prevChromosome);
#endif
                            /* write transformed buffer to output stream */
#ifdef __cplusplus
                            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, static_cast<int>( currentTransformedBufferLength ));
#else
                            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, (int) currentTransformedBufferLength);
#endif
                            if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_PARAM_ERROR: {
                                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_SEQUENCE_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }

                            /* close bzip2 stream and collect/reset stats */
                            BZ2_bzWriteClose64(&bzError, bzFp, STARCH_BZ_ABANDON, &bzBytesConsumedLo32, &bzBytesConsumedHi32, &bzBytesWrittenLo32, &bzBytesWrittenHi32);
                            if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_SEQUENCE_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }
#ifdef __cplusplus
                            bzBytesWritten = static_cast<size_t>( bzBytesWrittenHi32 ) << 32 | bzBytesWrittenLo32;
#else
                            bzBytesWritten = (size_t) bzBytesWrittenHi32 << 32 | bzBytesWrittenLo32;
#endif
                            cumulativeRecSize += bzBytesWritten;
                            currentRecSize += bzBytesWritten;
                            bzBytesWritten = 0;
                            bzBytesWrittenLo32 = 0U;
                            bzBytesWrittenHi32 = 0U;
#ifdef __cplusplus
			    bzFp = nullptr;
#else
                            bzFp = NULL;
#endif

                            if (STARCH_updateMetadataForChromosome(md, 
                                                                   prevChromosome, 
                                                                   compressedFn, 
                                                                   currentRecSize, 
                                                                   lineIdx, 
                                                                   totalNonUniqueBases, 
                                                                   totalUniqueBases, 
                                                                   duplicateElementExistsFlag, 
                                                                   nestedElementExistsFlag,
                                                                   base64EncodedSha1Digest,
                                                                   maxStringLength) != STARCH_EXIT_SUCCESS) {
                                fprintf(stderr, "ERROR: Could not update metadata %s\n", compressedFn);
                                return STARCH_FATAL_ERROR;
                            }

                            /* start again, anew, with a fresh bzip2 BZFILE pointer */
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) resetting bzip2 stream...\n");
#endif
                            bzFp = BZ2_bzWriteOpen(&bzError, outFp, STARCH_BZ_COMPRESSION_LEVEL, STARCH_BZ_VERBOSITY, STARCH_BZ_WORKFACTOR);
                            if (!bzFp) {
                                fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
                                return STARCH_EXIT_FAILURE;
                            }
                            else if (bzError != BZ_OK) {
                                switch (bzError) {
                                    case BZ_CONFIG_ERROR: {
                                        fprintf(stderr, "ERROR: Bzip2 library has been miscompiled\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_PARAM_ERROR: {
                                        fprintf(stderr, "ERROR: Stream is null, or block size, verbosity and work factor parameters are invalid\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_IO_ERROR: {
                                        fprintf(stderr, "ERROR: The value of ferror(outFp) is nonzero -- check outFp\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    case BZ_MEM_ERROR: {
                                        fprintf(stderr, "ERROR: Not enough memory is available\n");
                                        return STARCH_EXIT_FAILURE;
                                    }
                                    default: {
                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteOpen() (err: %d)\n", bzError);
                                        return STARCH_EXIT_FAILURE;
                                    }
                                }
                            }
                        }

                        else if (compressionType == kGzip) 
                        {
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) current chromosome: %s\n", prevChromosome);
                            fprintf(stderr, "\t(final-between-chromosome) transformedBuffer:\n%s\n", transformedBuffer);
#endif
#ifdef __cplusplus
                            zStream.next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
                            zStream.avail_in = static_cast<unsigned int>( currentTransformedBufferLength );
#else
                            zStream.next_in = (unsigned char *) transformedBuffer;
                            zStream.avail_in = (unsigned int) currentTransformedBufferLength;
#endif
                            do {
                                zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                                zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                                zStream.next_out = (unsigned char *) zBuffer;
#endif
                                zError = deflate (&zStream, Z_FINISH);
                                switch (zError) {
                                    case Z_MEM_ERROR: {
                                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                        return STARCH_FATAL_ERROR;
                                    }                                    
                                    case Z_BUF_ERROR:
                                    default:
                                        break;
                                }
                                zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                                cumulativeRecSize += zHave;
                                currentRecSize += zHave;
#ifdef DEBUG
                                fprintf(stderr, "\t(final-between-chromosome) writing: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                                fwrite(zBuffer, 1, zHave, stdout);
                                fflush(stdout);
                            } while (zStream.avail_out == 0);
                            assert(zStream.avail_in == 0);

#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) attempting to close z-stream...\n");
#endif
                            zError = deflateEnd(&zStream);
                            switch (zError) {
                                case Z_STREAM_ERROR: {
                                    fprintf(stderr, "ERROR: z-stream state is inconsistent\n");
                                    break;
                                }
                                case Z_DATA_ERROR: {
                                    fprintf(stderr, "ERROR: stream was freed prematurely\n");
                                    break;
                                }
                                case Z_OK:
                                default:
                                    break;
                            }
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) closed z-stream...\n");
                            fprintf(stderr, "\t(final-between-chromosome) updating metadata...\n");
#endif
                            if (STARCH_updateMetadataForChromosome(md, 
                                                                   prevChromosome, 
                                                                   compressedFn, 
                                                                   currentRecSize, 
                                                                   lineIdx, 
                                                                   totalNonUniqueBases, 
                                                                   totalUniqueBases, 
                                                                   duplicateElementExistsFlag, 
                                                                   nestedElementExistsFlag,
                                                                   base64EncodedSha1Digest,
                                                                   maxStringLength) != STARCH_EXIT_SUCCESS) {
                                fprintf(stderr, "ERROR: Could not update metadata %s\n", compressedFn);
                                return STARCH_FATAL_ERROR;
                            }

                            /* begin anew with a fresh compression z-stream */
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) creating fresh z-stream\n");
#endif
#ifdef __cplusplus
                            zStream.zalloc = nullptr;
                            zStream.zfree  = nullptr;
                            zStream.opaque = nullptr;
#else
                            zStream.zalloc = Z_NULL;
                            zStream.zfree  = Z_NULL;
                            zStream.opaque = Z_NULL;
#endif
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) initializing z-stream\n");
#endif
                            /* zError = deflateInit2(&zStream, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY); */
                            zError = deflateInit(&zStream, STARCH_Z_COMPRESSION_LEVEL);
                            switch (zError) {
                                case Z_MEM_ERROR: {
                                    fprintf(stderr, "ERROR: Not enough memory is available\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case Z_STREAM_ERROR: {
                                    fprintf(stderr, "ERROR: Gzip initialization parameter is invalid (e.g., invalid method)\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case Z_VERSION_ERROR: {
                                    fprintf(stderr, "ERROR: the zlib library version is incompatible with the version assumed by the caller (ZLIB_VERSION)\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case Z_OK:
                                default:
                                    break;
                            }
#ifdef DEBUG
                            fprintf(stderr, "\t(final-between-chromosome) initialized z-stream\n");
#endif
                        }

                        /* clean up per-chromosome hash digest */
                        if (base64EncodedSha1Digest) {
                            free(base64EncodedSha1Digest);
#ifdef __cplusplus
                            base64EncodedSha1Digest = nullptr;
#else
                            base64EncodedSha1Digest = NULL;
#endif
                        }
                    }

                    /* create placeholder records at current chromosome */
                    sprintf(compressedFn, "%s.%s", chromosome, tag);
#ifdef DEBUG
                    fprintf(stderr, "\t(final-between-chromosome) creating placeholder md record at chromosome: %s (compressedFn: %s) (signature: %s)\n", chromosome, compressedFn, base64EncodedSha1Digest);
#endif
                    if (recIdx == 0) {
#ifdef __cplusplus
                        *md = nullptr;
#else
                        *md = NULL;
#endif
                        *md = STARCH_createMetadata(chromosome, 
                                                    compressedFn, 
                                                    0, 
                                                    0UL, 
                                                    0UL, 
                                                    0UL, 
                                                    STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE, 
                                                    STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE,
#ifdef __cplusplus
                                                    nullptr,
#else
                                                    NULL,
#endif
                                                    STARCH_DEFAULT_LINE_STRING_LENGTH);
                        if (!*md) { 
                            fprintf(stderr, "ERROR: Not enough memory is available\n");
                            return STARCH_EXIT_FAILURE;
                        }
                        firstRecord = *md;
                    }
                    else {
                        *md = STARCH_addMetadata(*md, 
                                                 chromosome, 
                                                 compressedFn, 
                                                 0, 
                                                 0UL, 
                                                 0UL, 
                                                 0UL, 
                                                 STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE,
                                                 STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE,
#ifdef __cplusplus
                                                 nullptr,
#else
                                                 NULL,
#endif
                                                 STARCH_DEFAULT_LINE_STRING_LENGTH);
                    }

                    /* make previous chromosome the current chromosome */
#ifdef __cplusplus
                    if (prevChromosome != nullptr) {
                        free(prevChromosome);
                        prevChromosome = nullptr;
                    }
                    prevChromosome = static_cast<char *>( malloc(strlen(chromosome) + 1) );
#else
                    if (prevChromosome != NULL) {
                        free(prevChromosome);
                        prevChromosome = NULL;
                    }
                    prevChromosome = malloc(strlen(chromosome) + 1);
#endif
                    if (!prevChromosome) {
                        fprintf(stderr, "ERROR: Could not allocate space for previous chromosome marker.");
                        return STARCH_FATAL_ERROR;
                    }
#ifdef __cplusplus
                    strncpy(prevChromosome, reinterpret_cast<const char *>( chromosome ), strlen(chromosome) + 1);
#else
                    strncpy(prevChromosome, (const char *) chromosome, strlen(chromosome) + 1);
#endif

                    /* reset flag, lastPosition and lcDiff, increment record index */
#ifdef DEBUG
                    fprintf(stderr, "\t(final-between-chromosome B) resetting per-chromosome stream transformation parameters...\n");
#endif
                    withinChr = kStarchFalse;
                    lastPosition = 0;
                    pStart = -1;
                    pStop = -1;
                    if (pRemainder) { 
                        free(pRemainder);
#ifdef __cplusplus
                        pRemainder = nullptr; 
#else
                        pRemainder = NULL; 
#endif
                    }
                    previousStop = 0;
                    lcDiff = 0;
                    lineIdx = 0UL;
                    totalNonUniqueBases = 0UL;
                    totalUniqueBases = 0UL;
                    duplicateElementExistsFlag = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
                    nestedElementExistsFlag = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
                    recIdx++;
                    currentRecSize = 0UL;
                    transformedBuffer[currentTransformedBufferLength] = '\0';
                    currentTransformedBufferLength = 0U;
                    maxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
                }
                else 
                    withinChr = kStarchTrue;

                /* transform */
                if (stop > start)
                    coordDiff = stop - start;
                else {
                    fprintf(stderr, "ERROR: (E) BED data is corrupt at line %lu (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, stop, start);
                    return STARCH_FATAL_ERROR;
                }
                if (coordDiff != lcDiff) {
                    lcDiff = coordDiff;
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) A -- \np%" PRId64 "\n", coordDiff);
#endif
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "p%" PRId64 "\n", coordDiff);
                }
                if (lastPosition != 0) {
                    if (remainder) {                        
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) B --\n%" PRId64 "\t%s\n", (start - lastPosition), remainder);
#endif
                        sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", (start - lastPosition), remainder);
                    }
                    else {
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) C --\n%" PRId64 "\n", (start - lastPosition));
#endif
                        sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", (start - lastPosition));
                    }
                }
                else {
                    if (remainder) {
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) D --\n%" PRId64 "\t%s\n", start, remainder);
#endif
                        sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", start, remainder);
                    }
                    else {
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) E --\n%" PRId64 "\n", start);
#endif
                        sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", start);
                    }
                }
                
                intermediateBufferLength = strlen(intermediateBuffer);
#ifdef DEBUG
                fprintf(stderr, "\t(intermediate) state of intermediateBuffer before test:\n%s\n", intermediateBuffer);
#endif

                if ((currentTransformedBufferLength + intermediateBufferLength) < STARCH_BUFFER_MAX_LENGTH) {
                    /* append intermediateBuffer to transformedBuffer */
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) appending intermediateBuffer to transformedBuffer\n");
#endif
                    memcpy(transformedBuffer + currentTransformedBufferLength, intermediateBuffer, intermediateBufferLength);
                    currentTransformedBufferLength += intermediateBufferLength;
                    transformedBuffer[currentTransformedBufferLength] = '\0';
                    memset(intermediateBuffer, 0, intermediateBufferLength + 1);
                }
                else {
                    /* compress transformedBuffer[] and send to stdout */
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) to be compressed -- transformedBuffer:\n%s\n", transformedBuffer);
#endif
                    if (compressionType == kBzip2) {
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) current chromosome: %s\n", prevChromosome);
#endif
#ifdef __cplusplus
                        BZ2_bzWrite(&bzError, bzFp, transformedBuffer, static_cast<int>( currentTransformedBufferLength ));
#else
                        BZ2_bzWrite(&bzError, bzFp, transformedBuffer, (int) currentTransformedBufferLength);
#endif
                        if (bzError != BZ_OK) {
                            switch (bzError) {
                                case BZ_PARAM_ERROR: {
                                    fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case BZ_SEQUENCE_ERROR: {
                                    fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                case BZ_IO_ERROR: {
                                    fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                    return STARCH_EXIT_FAILURE;
                                }
                                default: {
                                    fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                                    return STARCH_EXIT_FAILURE;
                                }
                            }
                        }                        
                    }
                    else if (compressionType == kGzip) {
#ifdef DEBUG
                        fprintf(stderr, "\t(intermediate) current chromosome: %s\n", prevChromosome);
#endif
#ifdef __cplusplus
                        zStream.next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
                        zStream.avail_in = static_cast<unsigned int>( currentTransformedBufferLength );
#else
                        zStream.next_in = (unsigned char *) transformedBuffer;
                        zStream.avail_in = (unsigned int) currentTransformedBufferLength;
#endif
                        do {
                            zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                            zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                            zStream.next_out = (unsigned char *) zBuffer;
#endif
                            zError = deflate (&zStream, Z_NO_FLUSH);
                            switch (zError) {
                                case Z_MEM_ERROR: {
                                    fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                    return STARCH_FATAL_ERROR;
                                }
                                case Z_BUF_ERROR:
                                default:
                                    break;
                            }
                            zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                            cumulativeRecSize += zHave;
                            currentRecSize += zHave;
#ifdef DEBUG
                            fprintf(stderr, "\t(intermediate) written: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                            fwrite(zBuffer, 1, zHave, stdout);
                            fflush(stdout);
                        } while (zStream.avail_out == 0);
                    }
#ifdef DEBUG                        
                    fprintf(stderr, "\t(intermediate) transformedBuffer before hash:\n[%s]\n", transformedBuffer);
#endif
                    if (generatePerChrSignatureFlag) {
                        /* hash the transformed buffer */
                        sha1_process_bytes(transformedBuffer, currentTransformedBufferLength, &perChromosomeHashCtx);
                    }

                    /* copy transformed buffer to intermediate buffer */
                    memcpy(transformedBuffer, intermediateBuffer, strlen(intermediateBuffer) + 1);
                    currentTransformedBufferLength = strlen(intermediateBuffer);
                    memset(intermediateBuffer, 0, strlen(intermediateBuffer) + 1);
                    intermediateBufferLength = 0;
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) end-of-loop: transformedBuffer:\n%s\n\t\tintermediateBuffer:\n%s\n", transformedBuffer, intermediateBuffer);
#endif
                }

                /* test for out-of-order element */
                if (pStart > start) {
                    fprintf(stderr, "ERROR: BED data is not properly sorted by start coordinates at line %lu [ pStart: %" PRId64 " | start: %" PRId64 " ]\n", lineIdx, pStart, start);
                    exit (EXIT_FAILURE);
                }
                else if ((pStart == start) && (pStop > stop)) {
                    fprintf(stderr, "ERROR: BED data is not properly sorted by end coordinates (when start coordinates are equal) at line %lu\n", lineIdx);
                    exit (EXIT_FAILURE);
                }

                lastPosition = stop;
#ifdef __cplusplus
                totalNonUniqueBases += static_cast<BaseCountType>( stop - start );
#else
                totalNonUniqueBases += (BaseCountType) (stop - start);
#endif
                if (previousStop <= start) {
#ifdef __cplusplus
                    totalUniqueBases += static_cast<BaseCountType>( stop - start );
#else
                    totalUniqueBases += (BaseCountType) (stop - start);
#endif
                }
                else if (previousStop < stop) {
#ifdef __cplusplus
                    totalUniqueBases += static_cast<BaseCountType>( stop - previousStop );
#else
                    totalUniqueBases += (BaseCountType) (stop - previousStop);
#endif
                }
                previousStop = (stop > previousStop) ? stop : previousStop;

#ifdef DEBUG
                fprintf(stderr, "\t(intermediate) start: %" PRId64 "\tpStart: %" PRId64 "\tstop: %" PRId64 "\tpStop: %" PRId64 "\n", start, pStart, stop, pStop);
#ifdef __cplusplus
                fprintf(stderr, "\t(intermediate) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", static_cast<int>( duplicateElementExistsFlag ), static_cast<int>( nestedElementExistsFlag ));
#else
                fprintf(stderr, "\t(intermediate) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", (int) duplicateElementExistsFlag, (int) nestedElementExistsFlag);
#endif
#endif
            
                /* test for duplicate element */
                if ((pStart == start) && (pStop == stop))
                    duplicateElementExistsFlag = kStarchTrue;

                /* test for nested element */
                if ((pStart < start) && (pStop > stop))
                    nestedElementExistsFlag = kStarchTrue;
                
                /* set pElement values */
                pStart = start;
                pStop = stop;
#ifdef __cplusplus
                if (pRemainder) { 
                    free(pRemainder);
                    pRemainder = nullptr;
                }
                if (remainder)
                    pRemainder = STARCH_strndup(remainder, strlen(remainder));

                if (withinChr == kStarchTrue)  {
                    free(chromosome);
                    chromosome = nullptr;
                }
                if (remainder) {
                    free(remainder);
                    remainder = nullptr;
                }
#else
                if (pRemainder) { 
                    free(pRemainder);
                    pRemainder = NULL;
                }
                if (remainder)
                    pRemainder = STARCH_strndup(remainder, strlen(remainder));

                if (withinChr == kStarchTrue)  {
                    free(chromosome);
                    chromosome = NULL;
                }
                if (remainder) {
                    free(remainder);
                    remainder = NULL;
                }
#endif
                cIdx = 0;
            }
            else {
                fprintf(stderr, "ERROR: BED data could not be transformed.\n");
                return STARCH_FATAL_ERROR;
            }
        }
        else
            cIdx++;
    }

    /* if we don't have a trailing newline in BED input, then we have reached EOF before we can process a line, so we try that now */

    if (cIdx > 0) {
        untransformedBuffer[cIdx] = '\0';
        maxStringLength = (maxStringLength >= cIdx) ? maxStringLength : cIdx;
        if (STARCH_createTransformTokensForHeaderlessInput(untransformedBuffer, '\t', &chromosome, &start, &stop, &remainder) == 0)  {
#ifdef DEBUG
            fprintf(stderr, "\t(just-before-last-pass) untransformedBuffer:\n%s\n", untransformedBuffer);
#endif
            /* test for out-of-order element */
            if (pStart > start) {
                fprintf(stderr, "ERROR: BED data is not properly sorted by start coordinates at line %lu [ pStart: %" PRId64 " | start: %" PRId64 " ]\n", lineIdx, pStart, start);
                exit (EXIT_FAILURE);
            }
            else if ((pStart == start) && (pStop > stop)) {
                fprintf(stderr, "ERROR: BED data is not properly sorted by end coordinates (when start coordinates are equal) at line %lu\n", lineIdx);
                exit (EXIT_FAILURE);
            }

            /* transform */
            if (stop > start)
                coordDiff = stop - start;
            else {
                fprintf(stderr, "ERROR: (F) BED data is corrupt at line %lu (stop: %" PRId64 ", start: %" PRId64 ")\n", lineIdx, stop, start);
                return STARCH_FATAL_ERROR;
            }
            if (coordDiff != lcDiff) {
                lcDiff = coordDiff;
#ifdef DEBUG
                fprintf(stderr, "\t(intermediate) A -- \np%" PRId64 "\n", coordDiff);
#endif
                sprintf(intermediateBuffer + strlen(intermediateBuffer), "p%" PRId64 "\n", coordDiff);
            }
            if (lastPosition != 0) {
                if (remainder) {                        
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) B --\n%" PRId64 "\t%s\n", (start - lastPosition), remainder);
#endif
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", (start - lastPosition), remainder);
                }
                else {
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) C --\n%" PRId64 "\n", (start - lastPosition));
#endif
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", (start - lastPosition));
                }
            }
            else {
                if (remainder) {
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) D --\n%" PRId64 "\t%s\n", start, remainder);
#endif
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\t%s\n", start, remainder);
                }
                else {
#ifdef DEBUG
                    fprintf(stderr, "\t(intermediate) E --\n%" PRId64 "\n", start);
#endif
                    sprintf(intermediateBuffer + strlen(intermediateBuffer), "%" PRId64 "\n", start);
                }
            }
            intermediateBufferLength = strlen(intermediateBuffer);
#ifdef DEBUG
            fprintf(stderr, "\t(intermediate) state of intermediateBuffer before test:\n%s\n", intermediateBuffer);
#endif
        
            /* append intermediateBuffer to transformedBuffer */
#ifdef DEBUG
            fprintf(stderr, "\t(intermediate) appending intermediateBuffer to transformedBuffer\n");
#endif
            memcpy(transformedBuffer + currentTransformedBufferLength, intermediateBuffer, intermediateBufferLength);
            currentTransformedBufferLength += intermediateBufferLength;
            transformedBuffer[currentTransformedBufferLength] = '\0';
            memset(intermediateBuffer, 0, intermediateBufferLength + 1);
            
            lastPosition = stop;
#ifdef __cplusplus
            totalNonUniqueBases += static_cast<BaseCountType>( stop - start );
#else
            totalNonUniqueBases += (BaseCountType) (stop - start);
#endif
            if (previousStop <= start) {
#ifdef __cplusplus
                totalUniqueBases += static_cast<BaseCountType>( stop - start );
#else
                totalUniqueBases += (BaseCountType) (stop - start);
#endif
            }
            else if (previousStop < stop) {
#ifdef __cplusplus
                totalUniqueBases += static_cast<BaseCountType>( stop - previousStop );
#else
                totalUniqueBases += (BaseCountType) (stop - previousStop);
#endif
            }
            previousStop = (stop > previousStop) ? stop : previousStop;

#ifdef DEBUG
            fprintf(stderr, "\t(intermediate) start: %" PRId64 "\tpStart: %" PRId64 "\tstop: %" PRId64 "\tpStop: %" PRId64 "\n", start, pStart, stop, pStop);
#ifdef __cplusplus
            fprintf(stderr, "\t(intermediate) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", static_cast<int>( duplicateElementExistsFlag ), static_cast<int>( nestedElementExistsFlag ));
#else
            fprintf(stderr, "\t(intermediate) duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", (int) duplicateElementExistsFlag, (int) nestedElementExistsFlag);
#endif
#endif
        
            /* test for duplicate element */
            if ((pStart == start) && (pStop == stop))
                duplicateElementExistsFlag = kStarchTrue;

            /* test for nested element */
            if ((pStart < start) && (pStop > stop))
                nestedElementExistsFlag = kStarchTrue;

            if (pRemainder) {
                previousAndCurrentChromosomesAreIdentical = ((prevChromosome) && (strcmp(chromosome, prevChromosome) == 0)) ? kStarchTrue : kStarchFalse;
                /* if previous start and stop coordinates are the same, compare the remainder here */
                if ((start == pStart) && (stop == pStop) && (strcmp(remainder, pRemainder) < 0) && (previousAndCurrentChromosomesAreIdentical)) {
                    fprintf(stderr, "ERROR: (D) Elements with same start and stop coordinates have remainders in wrong sort order.\nBe sure to first sort input with sort-bed or remove --do-not-sort option from conversion script.\nDebug:\nchromosome [%s] start [%" PRId64 "] stop [%" PRId64 "]\nline [%ld]\nremainder A [%s]\nremainder B [%s]\nstrcmp(A,B) [%d]\n", chromosome, start, stop, lineIdx, remainder, pRemainder, strcmp(remainder, pRemainder));
                    return STARCH_FATAL_ERROR;
                }
                free(pRemainder);
#ifdef __cplusplus
                pRemainder = nullptr;
#else
                pRemainder = NULL;
#endif
            }
        }
        else {
            fprintf(stderr, "ERROR: (G) BED data is corrupt at line %lu\n", lineIdx);
            return STARCH_FATAL_ERROR;
        }
    }
    
    lineIdx++;
    sprintf(compressedFn, "%s.%s", prevChromosome, tag);

#ifdef DEBUG
    fprintf(stderr, "\t(last-pass) transformedBuffer:\n%s\n\t\tintermediateBuffer:\n%s\n", transformedBuffer, intermediateBuffer);
#endif

#ifdef DEBUG                        
    fprintf(stderr, "\t(last-pass) transformedBuffer before hash:\n[%s]\n", transformedBuffer);
#endif

    if (generatePerChrSignatureFlag) {
        /* hash the transformed buffer */
        sha1_process_bytes(transformedBuffer, currentTransformedBufferLength, &perChromosomeHashCtx);
        sha1_finish_ctx(&perChromosomeHashCtx, sha1Digest);
#ifdef __cplusplus
        STARCH_encodeBase64(&base64EncodedSha1Digest, 
                            static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                            reinterpret_cast<const unsigned char *>( sha1Digest ), 
                            static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
        STARCH_encodeBase64(&base64EncodedSha1Digest, 
                            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                            (const unsigned char *) sha1Digest, 
                            (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
#ifdef DEBUG
        fprintf(stderr, "\nPROGRESS: SHA-1 digest for chr [%s] is [%s]\n", prevChromosome, base64EncodedSha1Digest);
#endif
    }

    /* last-pass, bzip2 */
    if (compressionType == kBzip2) {
#ifdef DEBUG
        fprintf(stderr, "\t(last-pass) current chromosome: %s\n", prevChromosome);
#endif
        if (currentTransformedBufferLength > 0) 
        {
#ifdef __cplusplus
            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, static_cast<int>( currentTransformedBufferLength ));
#else
            BZ2_bzWrite(&bzError, bzFp, transformedBuffer, (int) currentTransformedBufferLength);
#endif
            if (bzError != BZ_OK) {
                switch (bzError) {
                    case BZ_PARAM_ERROR: {
                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case BZ_SEQUENCE_ERROR: {
                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case BZ_IO_ERROR: {
                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    default: {
                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                        return STARCH_EXIT_FAILURE;
                    }
                }
            }
        }

#ifdef DEBUG
        fprintf(stderr, "\t(last-pass) attempting to close bzip2 stream...\n");
#endif
        BZ2_bzWriteClose64(&bzError, bzFp, STARCH_BZ_ABANDON, &bzBytesConsumedLo32, &bzBytesConsumedHi32, &bzBytesWrittenLo32, &bzBytesWrittenHi32);
        if (bzError != BZ_OK) {
            switch (bzError) {
                case BZ_PARAM_ERROR: {
                    fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_SEQUENCE_ERROR: {
                    fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                    return STARCH_EXIT_FAILURE;
                }
                case BZ_IO_ERROR: {
                    fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                    return STARCH_EXIT_FAILURE;
                }
                default: {
                    fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                    return STARCH_EXIT_FAILURE;
                }
            }
        }
#ifdef __cplusplus
        bzBytesWritten = static_cast<size_t>( bzBytesWrittenHi32 ) << 32 | bzBytesWrittenLo32;
#else
        bzBytesWritten = (size_t) bzBytesWrittenHi32 << 32 | bzBytesWrittenLo32;
#endif
        cumulativeRecSize += bzBytesWritten;
        currentRecSize += bzBytesWritten;
#ifdef DEBUG
        fprintf(stderr, "\t(last-pass) closed bzip2 stream...\n");
#endif
    }

    /* last-pass, gzip */
    else if (compressionType == kGzip) {
#ifdef DEBUG
        /*fprintf(stderr, "\t(last-pass) to be compressed - transformedBuffer:\n%s\n", transformedBuffer);*/
#endif        
        if (currentTransformedBufferLength > 0) 
        {
#ifdef DEBUG
            fprintf(stderr, "\t(last-pass) current chromosome: %s\n", prevChromosome);
#endif
#ifdef __cplusplus
            zStream.next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
            zStream.avail_in = static_cast<unsigned int>( currentTransformedBufferLength );
#else
            zStream.next_in = (unsigned char *) transformedBuffer;
            zStream.avail_in = (unsigned int) currentTransformedBufferLength;
#endif
            do {
                zStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                zStream.next_out = reinterpret_cast<unsigned char *>( zBuffer );
#else
                zStream.next_out = (unsigned char *) zBuffer;
#endif
                zError = deflate(&zStream, Z_FINISH);
                switch (zError) {
                    case Z_MEM_ERROR: {
                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                        return STARCH_FATAL_ERROR;
                    }
                    case Z_BUF_ERROR:
                    default:
                        break;
                }
                zHave = STARCH_Z_BUFFER_MAX_LENGTH - zStream.avail_out;
                cumulativeRecSize += zHave;
                currentRecSize += zHave;
#ifdef DEBUG
                fprintf(stderr, "\t(last-pass) written: %zu bytes\tcurrent record size: %zu\n", cumulativeRecSize, currentRecSize);
#endif
                fwrite(zBuffer, 1, zHave, stdout);
                fflush(stdout);
            } while (zStream.avail_out == 0);
#ifdef DEBUG
            fprintf(stderr, "\t(last-pass) attempting to close z-stream...\n");
#endif
            deflateEnd(&zStream);
#ifdef DEBUG
            fprintf(stderr, "\t(last-pass) closed z-stream...\n");
#endif
        }
    }

#ifdef DEBUG
    fprintf(stderr, "\t(last-pass) updating last md record...\n");
#ifdef __cplusplus
    fprintf(stderr, "\t(last-pass) flag state: duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", static_cast<int>( duplicateElementExistsFlag ), static_cast<int>( nestedElementExistsFlag ));
#else
    fprintf(stderr, "\t(last-pass) flag state: duplicateElementExistsFlag: %d\tnestedElementExistsFlag: %d\n", (int) duplicateElementExistsFlag, (int) nestedElementExistsFlag);
#endif
#endif
    if (STARCH_updateMetadataForChromosome(md, 
                                           prevChromosome, 
                                           compressedFn, 
                                           currentRecSize, 
                                           lineIdx, 
                                           totalNonUniqueBases, 
                                           totalUniqueBases,
                                           duplicateElementExistsFlag,
                                           nestedElementExistsFlag,
                                           base64EncodedSha1Digest,
                                           maxStringLength) != STARCH_EXIT_SUCCESS) {
        /* 
           If the stream or input file contains no BED records, then the Metadata pointer md will
           be NULL, as will the char pointer prevChromosome. So we put in a stub metadata record.
        */
        lineIdx = 0;
#ifdef __cplusplus
        *md = nullptr;
#else
        *md = NULL;
#endif
        *md = STARCH_createMetadata(nullChr, 
                                    nullCompressedFn, 
                                    currentRecSize, 
                                    lineIdx, 
                                    0UL, 
                                    0UL,
                                    STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE,
                                    STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE,
                                    nullSig,
                                    0UL);
        if (!*md) { 
            fprintf(stderr, "ERROR: Not enough memory is available\n");
            return STARCH_EXIT_FAILURE;
        }
        firstRecord = *md;
    }

    /* clean up per-chromosome hash digest */
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest);
#ifdef __cplusplus
        base64EncodedSha1Digest = nullptr;
#else
        base64EncodedSha1Digest = NULL;
#endif
    }

    /* reset metadata pointer */
    *md = firstRecord;

    /* write metadata */
#ifdef DEBUG
    fprintf(stderr, "\twriting md to output stream (as JSON)...\n");
#endif
    STARCH_writeJSONMetadata(*md, &json, &type, 0, note);
    fwrite(json, 1, strlen(json), stdout);
    fflush(stdout);

    /* write metadata signature */
#ifdef DEBUG
    fprintf(stderr, "\twriting md signature...\n");
#endif
    jsonCopy = STARCH_strdup(json);
#ifdef __cplusplus
    STARCH_SHA1_All(reinterpret_cast<const unsigned char *>( jsonCopy ), strlen(jsonCopy), sha1Digest);
#else
    STARCH_SHA1_All((const unsigned char *)jsonCopy, strlen(jsonCopy), sha1Digest);
#endif
    free(jsonCopy);

/* encode signature in base64 encoding */
#ifdef DEBUG
    fprintf(stderr, "\tencoding md signature...\n");
#endif

#ifdef __cplusplus
    STARCH_encodeBase64(&base64EncodedSha1Digest, 
                        static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                        reinterpret_cast<const unsigned char *>( sha1Digest ), 
                        static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
    STARCH_encodeBase64(&base64EncodedSha1Digest, 
                        (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                        (const unsigned char *) sha1Digest, 
                        (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH );
#endif

/* build footer */
#ifdef DEBUG
#ifdef __cplusplus
    fprintf(stderr, "\tWARNING:\nmdLength: %llu\nmd   - [%s]\nsha1 - [%s]\n", static_cast<unsigned long long>( strlen(json) ), json, sha1Digest);
#else
    fprintf(stderr, "\tWARNING:\nmdLength: %llu\nmd   - [%s]\nsha1 - [%s]\n", (unsigned long long) strlen(json), json, sha1Digest);
#endif
    fprintf(stderr, "\twriting offset and signature to output stream...\n");
#endif
#ifdef __cplusplus
    sprintf(footerCumulativeRecordSizeBuffer, "%020llu", static_cast<unsigned long long>( cumulativeRecSize )); /* size_t cast to unsigned long long to avoid compilation warnings from ISO C++ compiler */
#else
    sprintf(footerCumulativeRecordSizeBuffer, "%020llu", (unsigned long long) cumulativeRecSize); /* size_t cast to unsigned long long to avoid compilation warnings from ISO C++ compiler */
#endif
#ifdef DEBUG
    fprintf(stderr, "\tfooterCumulativeRecordSizeBuffer: %s\n", footerCumulativeRecordSizeBuffer);
#endif
    memcpy(footerBuffer, footerCumulativeRecordSizeBuffer, strlen(footerCumulativeRecordSizeBuffer));
    memcpy(footerBuffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH, base64EncodedSha1Digest, STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1); /* strip trailing null */
#ifdef __cplusplus
    memset(footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, static_cast<size_t>( STARCH2_MD_FOOTER_REMAINDER_LENGTH ));
#else
    memset(footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, (size_t) STARCH2_MD_FOOTER_REMAINDER_LENGTH);
#endif
#ifdef DEBUG
    fprintf(stderr, "\tfooterRemainderBuffer: [%s]\n", footerRemainderBuffer);
#endif
    memcpy(footerBuffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1, footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_LENGTH); /* don't forget to offset pointer index by -1 for base64-sha1's null */
    footerBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 1] = '\0';
    footerBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 2] = '\n';
    fprintf(stdout, "%s", footerBuffer);
    fflush(stdout);

#ifdef __cplusplus
    if (json) {
        free(json);
        json = nullptr;
    }
    if (compressedFn) {
        free(compressedFn); 
        compressedFn = nullptr;
    }
    if (prevChromosome) {
        free(prevChromosome); 
        prevChromosome = nullptr;
    }
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest); 
        base64EncodedSha1Digest = nullptr;
    }
#else
    if (json) {
        free(json);
        json = NULL;
    }
    if (compressedFn) {
        free(compressedFn); 
        compressedFn = NULL;
    }
    if (prevChromosome) {
        free(prevChromosome); 
        prevChromosome = NULL;
    }
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest); 
        base64EncodedSha1Digest = NULL;
    }
#endif

    return STARCH_EXIT_SUCCESS;
}

int
STARCH2_writeStarchHeaderToOutputFp(const unsigned char *header, const FILE *outFp)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH2_writeStarchHeaderToOutputFp() ---\n");
    fprintf(stderr, "\theader: %s\n", header);
#endif

    if (!outFp) {
        fprintf(stderr, "ERROR: No output file pointer available to write starch header.\n");
        return STARCH_EXIT_FAILURE;
    }

#ifdef __cplusplus
    if (fwrite(header, STARCH2_MD_HEADER_BYTE_LENGTH, 1, const_cast<FILE *>( outFp )) != 1) {
#else
    if (fwrite(header, STARCH2_MD_HEADER_BYTE_LENGTH, 1, (FILE *)outFp) != 1) {
#endif
        fprintf(stderr, "ERROR: Could not write all of starch header items to output file pointer.\n");
        return STARCH_EXIT_FAILURE;
    }

    return STARCH_EXIT_SUCCESS;
}

int
STARCH2_initializeStarchHeader(unsigned char **header) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH2_initializeStarchHeader() ---\n");
#endif
    int idx;

#ifdef __cplusplus
    *header = static_cast<unsigned char *>( malloc (STARCH2_MD_HEADER_BYTE_LENGTH) );
#else
    *header = malloc (STARCH2_MD_HEADER_BYTE_LENGTH);
#endif
    if (!*header) {
        fprintf(stderr, "ERROR: Could not allocate space for header.\n");
        return STARCH_EXIT_FAILURE;
    }
    memset(*header, 0, STARCH2_MD_HEADER_BYTE_LENGTH);

    for (idx = 0; idx < STARCH2_MD_HEADER_BYTE_LENGTH; idx++) {
        if (idx < 4)
            (*header)[idx] = starchRevision2HeaderBytes[idx];
    }

    return STARCH_EXIT_SUCCESS;
}

void
STARCH2_printStarchHeader(const unsigned char *header)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH2_printStarchHeader() ---\n");
#endif
    int idx;

    fprintf(stderr, "ERROR: Archive header:\n\t");
    for (idx = 0; idx < STARCH2_MD_HEADER_BYTE_LENGTH; idx++) {
        fprintf(stderr, "%02x", header[idx]);
        if ( ((idx + 1) % 4 == 0) && (idx != STARCH2_MD_HEADER_BYTE_LENGTH - 1) )
            fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
}

#ifdef __cplusplus
} // namespace starch
#endif
