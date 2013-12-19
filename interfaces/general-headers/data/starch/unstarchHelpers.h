//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    unstarchHelpers.h
//=========

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef UNSTARCH_HELPERS_H
#define UNSTARCH_HELPERS_H

#include <bzlib.h>

#include "data/starch/starchMetadataHelpers.h"
#include "suite/BEDOPS.Constants.hpp"

#ifdef __cplusplus
  using namespace Bed;
#endif

#define UNSTARCH_COMPRESSED_BUFFER_MAX_LENGTH 8192
#define UNSTARCH_UNCOMPRESSED_BUFFER_MAX_LENGTH 2097152
#define UNSTARCH_BUFFER_MAX_LENGTH INT_TOKEN_REST_MAX_LENGTH + 1
#define UNSTARCH_EXTENSION_BZ2 "bz2"
#define UNSTARCH_EXTENSION_GZ "gz"
#define UNSTARCH_RADIX 10
#define UNSTARCH_FIRST_TOKEN_MAX_LENGTH INT_TOKEN_CHR_MAX_LENGTH + INT_TOKEN_ID_MAX_LENGTH + 1
#define UNSTARCH_SECOND_TOKEN_MAX_LENGTH INT_TOKEN_REST_MAX_LENGTH + 1

#define UNSTARCH_FATAL_ERROR -1
#define UNSTARCH_HELP_ERROR 10
#define UNSTARCH_VERSION_ERROR 11
#define UNSTARCH_ARCHIVE_VERSION_ERROR 12
#define UNSTARCH_ELEMENT_COUNT_CHR_ERROR 13
#define UNSTARCH_ELEMENT_COUNT_ALL_ERROR 14
#define UNSTARCH_LIST_CHROMOSOMES_ERROR 15
#define UNSTARCH_BASES_COUNT_CHR_ERROR 16
#define UNSTARCH_BASES_COUNT_ALL_ERROR 17
#define UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR 18
#define UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR 19
#define UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR 20
#define UNSTARCH_ARCHIVE_NOTE_ERROR 21
#define UNSTARCH_ARCHIVE_COMPRESSION_TYPE_ERROR 22
#define UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR 23

int                UNSTARCH_reverseTransformInput(const char *chr,
                                         const unsigned char *str,
                                                        char delim,
                                             SignedCoordType *start,
                                             SignedCoordType *pLength,
                                             SignedCoordType *lastEnd,
                                                        char elemTok1[],
                                                        char elemTok2[],
                                                        FILE *outFp);

int                UNSTARCH_sReverseTransformInput(const char *chr,
                                          const unsigned char *str,
                                                         char delim,
                                              SignedCoordType *start,
                                              SignedCoordType *pLength,
                                              SignedCoordType *lastEnd,
                                                         char *elemTok1,
                                                         char *elemTok2,
                                                         char **currentChr,
                                                       size_t *currentChrLen,
                                              SignedCoordType *currentStart,
                                              SignedCoordType *currentStop,
                                                         char **currentRemainder,
                                                       size_t *currentRemainderLen);

int                UNSTARCH_reverseTransformIgnoringHeaderedInput(const char *chr, 
                                                         const unsigned char *str, 
                                                                        char delim, 
                                                             SignedCoordType *start, 
                                                             SignedCoordType *pLength, 
                                                             SignedCoordType *lastEnd, 
                                                                        char elemTok1[], 
                                                                        char elemTok2[], 
                                                                        FILE *outFp);

int                UNSTARCH_sReverseTransformIgnoringHeaderedInput(const char *chr, 
                                                          const unsigned char *str, 
                                                                         char delim, 
                                                              SignedCoordType *start, 
                                                              SignedCoordType *pLength, 
                                                              SignedCoordType *lastEnd, 
                                                                         char *elemTok1,
                                                                         char *elemTok2,
                                                                         char **currentChr,
                                                                       size_t *currentChrLen,
                                                              SignedCoordType *currentStart,
                                                              SignedCoordType *currentLong,
                                                                         char **currentRemainder,
                                                                       size_t *currentRemainderLen);

int                UNSTARCH_reverseTransformHeaderlessInput(const char *chr, 
                                                   const unsigned char *str, 
                                                                  char delim, 
                                                       SignedCoordType *start, 
                                                       SignedCoordType *pLength, 
                                                       SignedCoordType *lastEnd, 
                                                                  char elemTok1[], 
                                                                  char elemTok2[], 
                                                                  FILE *outFp);

int                UNSTARCH_extractRawLine(const char *chr,
                                  const unsigned char *str,
                                                 char delim,
                                      SignedCoordType *start,
                                      SignedCoordType *pLength,
                                      SignedCoordType *lastEnd,
                                                 char *elemTok1,
                                                 char *elemTok2,
                                                 char **currentChr,
                                               size_t *currentChrLen,
                                      SignedCoordType *currentStart,
                                      SignedCoordType *currentLong,
                                                 char **currentRemainder,
                                               size_t *currentRemainderLen);

int                UNSTARCH_sReverseTransformHeaderlessInput(const char *chr,
                                                    const unsigned char *str,
                                                                   char delim,
                                                        SignedCoordType *start,
                                                        SignedCoordType *pLength,
                                                        SignedCoordType *lastEnd,
                                                                   char *elemTok1,
                                                                   char *elemTok2,
                                                                   char **currentChr,
                                                                 size_t *currentChrLen,
                                                        SignedCoordType *currentStart,
                                                        SignedCoordType *currentLong,
                                                                   char **currentRemainder,
                                                                 size_t *currentRemainderLen);

int                UNSTARCH_createInverseTransformTokens(const unsigned char *s, 
                                                                        char delim, 
                                                                        char elemTok1[], 
                                                                        char elemTok2[]);

int                UNSTARCH_extractDataWithBzip2(FILE **inFp, 
                                                 FILE *outFp, 
                                           const char *whichChr, 
                                       const Metadata *md, 
                                       const uint64_t mdOffset,
                                        const Boolean headerFlag);

int                UNSTARCH_extractDataWithGzip(FILE **inFp, 
                                                FILE *outFp, 
                                          const char *whichChr, 
                                      const Metadata *md, 
                                      const uint64_t mdOffset,
                                       const Boolean headerFlag);

char *             UNSTARCH_strnstr(const char *haystack, 
                                    const char *needle, 
                                        size_t haystackLen);

char *             UNSTARCH_strndup(const char *s,
                                        size_t n);

void               UNSTARCH_bzReadLine(BZFILE *input, 
                                unsigned char **output);

LineCountType UNSTARCH_lineCountForChromosome(const Metadata *md, 
                                                  const char *chr);

void               UNSTARCH_printLineCountForChromosome(const Metadata *md,
                                                            const char *chr);

void               UNSTARCH_printLineCountForAllChromosomes(const Metadata *md);

BaseCountType UNSTARCH_nonUniqueBaseCountForChromosome(const Metadata *md, 
                                                           const char *chr);

void               UNSTARCH_printNonUniqueBaseCountForChromosome(const Metadata *md,
                                                                     const char *chr);

void               UNSTARCH_printNonUniqueBaseCountForAllChromosomes(const Metadata *md);

BaseCountType UNSTARCH_uniqueBaseCountForChromosome(const Metadata *md, 
                                                        const char *chr);

void               UNSTARCH_printUniqueBaseCountForChromosome(const Metadata *md,
                                                                  const char *chr);

void               UNSTARCH_printUniqueBaseCountForAllChromosomes(const Metadata *md);

int                UNSTARCH_reverseTransformCoordinates(const LineCountType lineIdx,
                                                            SignedCoordType *lastPosition,
                                                            SignedCoordType *lcDiff,
                                                            SignedCoordType *currStart, 
                                                            SignedCoordType *currStop,
                                                                       char **currRemainder, 
                                                              unsigned char *lineBuf, 
                                                                    int64_t *nLineBuf,
                                                                    int64_t *nLineBufPos);

#endif
