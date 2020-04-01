//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchHelpers.h
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

#ifndef STARCH_HELPERS_H
#define STARCH_HELPERS_H

#ifdef __cplusplus
#include <cinttypes>
#include <cstdint>
#include <clocale>
#else
#include <inttypes.h>
#include <stdint.h>
#include <locale.h>
#endif

#include "data/starch/starchMetadataHelpers.h"

#ifdef __cplusplus
namespace starch {
#endif

#define STARCH_BUFFER_MAX_LENGTH 1024*1024
#define STARCH_Z_CHUNK STARCH_BUFFER_MAX_LENGTH
#define STARCH_Z_CHUNK_MULTIPLIER 8
#define STARCH_Z_BUFFER_MAX_LENGTH STARCH_BUFFER_MAX_LENGTH
#define STARCH_Z_IN_BUFFER_MAX_LENGTH 1024
#define STARCH_Z_COMPRESSION_LEVEL 1
#define STARCH_Z_WINDOW_BITS 31
#define STARCH_Z_MEMORY_LEVEL 9
#define STARCH_BZ_BUFFER_MAX_LENGTH STARCH_BUFFER_MAX_LENGTH
#define STARCH_BZ_COMPRESSION_LEVEL 9
#define STARCH_BZ_VERBOSITY 0
#define STARCH_BZ_SMALL 0
#define STARCH_BZ_WORKFACTOR 0
#define STARCH_BZ_ABANDON 0
#define STARCH_RADIX 10

#define STARCH_NONFATAL_ERROR -2
#define STARCH_FATAL_ERROR -1
#define STARCH_HELP_ERROR 2
#define STARCH_VERSION_ERROR 3

char *  STARCH_strdup(const char *str);

int     STARCH_compressFileWithGzip(const char *inFn, 
                                          char **outFn, 
                                         off_t *outFnSize);

int     STARCH_compressFileWithBzip2(const char *inFn, 
                                           char **outFn, 
                                          off_t *outFnSize);

int     STARCH_createTransformTokens(const char *s, 
                                     const char delim, 
                                      char **chr, 
                                   int64_t *start, 
                                   int64_t *stop, 
                                      char **remainder, 
                               BedLineType *lineType);

int     STARCH_createTransformTokensForHeaderlessInput(const char *s,
                                                       const char delim,
                                                             char **chr,
                                                          int64_t *start,
                                                          int64_t *stop,
                                                             char **remainder);

int     STARCH_transformInput(Metadata **md,
                            const FILE *fp, 
                 const CompressionType type, 
                            const char *tag,
                            const char *note);

int     STARCH_transformHeaderlessInput(Metadata **md,
                                      const FILE *fp, 
                           const CompressionType type, 
                                      const char *tag, 
                                   const Boolean finalizeFlag,
                                      const char *note);

Boolean STARCH_fileExists(const char *fn);

char *  STARCH_strndup(const char *s, 
                           size_t n);

void    STARCH_printUsage(int t);

void    STARCH_printRevision();

int     STARCH2_transformInput(unsigned char **header, 
                                    Metadata **md,
                                  const FILE *inFp,
                       const CompressionType compressionType, 
                                  const char *tag,
                                  const char *note,
                               const Boolean generatePerChrSignatureFlag,
                               const Boolean headerFlag,
                               const Boolean reportProgressFlag,
                         const LineCountType reportProgressN);

int     STARCH2_transformHeaderedBEDInput(const FILE *inFp, 
                                            Metadata **md, 
                               const CompressionType compressionType, 
                                          const char *tag, 
                                          const char *note,
                                       const Boolean generatePerChrSignatureFlag,
                                       const Boolean reportProgressFlag,
                                 const LineCountType reportProgressN);

int     STARCH2_transformHeaderlessBEDInput(const FILE *inFp, 
                                              Metadata **md,
                                 const CompressionType compressionType,
                                            const char *tag,
                                            const char *note,
                                         const Boolean generatePerChrSignatureFlag,
                                         const Boolean reportProgressFlag,
                                   const LineCountType reportProgressN);

int     STARCH2_writeStarchHeaderToOutputFp(const unsigned char *header, 
                                                     const FILE *fp);

int     STARCH2_initializeStarchHeader(unsigned char **header);

void    STARCH2_printStarchHeader(const unsigned char *header);

#ifdef __cplusplus
} // namespace starch
#endif

#endif
