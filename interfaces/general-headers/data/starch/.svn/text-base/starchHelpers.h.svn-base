//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchHelpers.h
//=========

#ifndef STARCH_HELPERS_H
#define STARCH_HELPERS_H

#ifdef __cplusplus
#include <cinttypes>
#include <cstdint>
#else
#include <inttypes.h>
#include <stdint.h>
#endif

#include <locale.h>

#include "data/starch/starchMetadataHelpers.h"

#define STARCH_BUFFER_MAX_LENGTH 1024*1024
#define STARCH_Z_CHUNK STARCH_BUFFER_MAX_LENGTH
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
                               const Boolean headerFlag);

int     STARCH2_transformHeaderedBEDInput(const FILE *inFp, 
                                            Metadata **md, 
                               const CompressionType compressionType, 
                                          const char *tag, 
                                          const char *note);

int     STARCH2_transformHeaderlessBEDInput(const FILE *inFp, 
                                              Metadata **md,
                                 const CompressionType compressionType,
                                            const char *tag,
                                            const char *note);

int     STARCH2_writeStarchHeaderToOutputFp(const unsigned char *header, 
                                                     const FILE *fp);

int     STARCH2_initializeStarchHeader(unsigned char **header);

void    STARCH2_printStarchHeader(const unsigned char *header);

#endif
