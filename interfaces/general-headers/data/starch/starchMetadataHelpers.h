//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchMetadataHelpers.h
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

#ifndef STARCHMETADATAHELPERS_H
#define STARCHMETADATAHELPERS_H

#include <inttypes.h>

#include "data/starch/starchConstants.h"
#include "suite/BEDOPS.Constants.hpp"

#include "jansson.h"

/* old "stable" binary version:      1.2.0 */
/* current "stable" binary version:  2.3.0 */
/* current "dev" binary version:     2.4.0 */

/* current "stable" archive version: 2.0.0 */

#define STARCH_MAJOR_VERSION 2
#define STARCH_MINOR_VERSION 0
#define STARCH_REVISION_VERSION 0

#define STARCH_DEFAULT_COMPRESSION_TYPE kBzip2

#define STARCH_EXIT_FAILURE 0
#define STARCH_EXIT_SUCCESS 1

#define STARCH_LEGACY_METADATA_SIZE 8192
#define STARCH_LEGACY_EXTENSION_BZ2 ".bz2"
#define STARCH_LEGACY_EXTENSION_GZIP ".gz"
#define STARCH_FATAL_ERROR -1
#define STARCH_RADIX 10
#define STARCH_TEST_BYTE_COUNT 4
#define STARCH_TEST_BYTE_POSITION_RESET 3
#define STARCH_TAG_MAX_LENGTH 2048
#define STARCH_STREAM_METADATA_FILENAME_MAX_LENGTH 1024
#define STARCH_STREAM_METADATA_MAX_LENGTH 1048576
#define STARCH_DEFAULT_LINE_COUNT UINT64_C(0)
#define STARCH_DEFAULT_NON_UNIQUE_BASE_COUNT UINT64_C(0)
#define STARCH_DEFAULT_UNIQUE_BASE_COUNT UINT64_C(0)
#define STARCH_CREATION_TIMESTAMP_LENGTH 80
#define STARCH_ARCHIVE_VERSION_STRING_LENGTH 80

#define STARCH_METADATA_STREAM_HEADER_BED_TYPE_KEY "customUCSCHeaders"
#define STARCH_METADATA_STREAM_COMPRESSION_FORMAT_KEY "compressionFormat"
#define STARCH_METADATA_STREAM_LIST_KEY "streams"
#define STARCH_METADATA_STREAM_CHROMOSOME_KEY "chromosome"
#define STARCH_METADATA_STREAM_FILENAME_KEY "filename"
#define STARCH_METADATA_STREAM_SIZE_KEY "size"
#define STARCH_METADATA_STREAM_LINECOUNT_KEY "uncompressedLineCount"
#define STARCH_METADATA_STREAM_TOTALNONUNIQUEBASES_KEY "nonUniqueBaseCount"
#define STARCH_METADATA_STREAM_TOTALUNIQUEBASES_KEY "uniqueBaseCount"
#define STARCH_METADATA_STREAM_ARCHIVE_KEY "archive"
#define STARCH_METADATA_STREAM_ARCHIVE_TYPE_KEY "type"
#define STARCH_METADATA_STREAM_ARCHIVE_NOTE_KEY "note"
#define STARCH_METADATA_STREAM_ARCHIVE_CREATION_TIMESTAMP_KEY "creationTimestamp"
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY "version"
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_KEY "major"
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_KEY "minor"
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_KEY "revision"
#define STARCH_METADATA_STREAM_ARCHIVE_TYPE_VALUE "starch"
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_VALUE STARCH_MAJOR_VERSION
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_VALUE STARCH_MINOR_VERSION
#define STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_VALUE STARCH_REVISION_VERSION

#define STARCH2_MD_HEADER_BYTE_LENGTH 4
#define STARCH2_MD_FOOTER_LENGTH 128
#define STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH 20
#define STARCH2_MD_FOOTER_SHA1_LENGTH 20
#define STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH 29
#define STARCH2_MD_FOOTER_REMAINDER_LENGTH STARCH2_MD_FOOTER_LENGTH - STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH - STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH + 1
#define STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR 32

/*
    Starch rev. 1
    -------------------------------------------------
    We use the following values to test for delimiters between starch metadata
    and the leading magic bytes of the first bzip2 or gzip chromosome stream. Once
    we find magic bytes, we look backwards to see if we really have a boundary
    between MD and compressed streams.

    -- dynamicMdTerminatorBytes[] delimits the dynamically-sized metadata, and
       as this is how metadata will be sized from now on, this is the way to go

    -- legacyMdTerminatorBytes[] and otherLegacyMdTerminatorBytes[] cover 
       very early versions of starch archives and should not be needed going
       forward, except for backwards-compatibility
*/

static const int testElemSize = sizeof(char);
static const int testElemCount = STARCH_TEST_BYTE_COUNT;
static const unsigned char bzip2MagicBytes[] = { 0x42, 0x5a, 0x68, 0x39, 0x00 };
static const unsigned char gzipMagicBytes[] = { 0x1f, 0x8b, 0x08, 0x00 };
static const unsigned char dynamicMdTerminatorBytes[] = { 0x7d };
static const unsigned char legacyMdTerminatorBytes[] = { 0x00 };
static const unsigned char otherLegacyMdTerminatorBytes[] = { 0x0A };
static const unsigned char starchRevision1HeaderBytes[] = { 0x7b };
static const int mdTerminatorBytesLength = 1;

/*
    Starch rev. 2+
    -------------------------------------------------
    We use the following bytes to identity the archive as a Starch v2+ file.
*/

static const unsigned char starchRevision2HeaderBytes[] = { 0xca, 0x5c, 0xad, 0xe5 }; /* ca5cade5 */

/*
    Structs and enumerations
    -------------------------------------------------
*/

typedef struct metadata {
    char *chromosome;
    char *filename;
    uint64_t size;
    Bed::LineCountType lineCount;
    Bed::BaseCountType totalNonUniqueBases;
    Bed::BaseCountType totalUniqueBases;
    struct metadata *next;
} Metadata;

typedef enum {
    kBzip2 = 0,
    kGzip,
    kUndefined
} CompressionType;

typedef struct archiveVersion {
    int major;
    int minor;
    int revision;
} ArchiveVersion;

typedef enum {
    kBedLineCoordinates = 0,
    kBedLineHeaderTrack,
    kBedLineHeaderBrowser,
    kBedLineHeaderSAM,
    kBedLineHeaderVCF,
    kBedLineGenericComment,
    kBedLineTypeUndefined
} BedLineType;

typedef 
unsigned int HeaderFlag;

/* 
   On Darwin, file I/O is 64-bit by default (OS X 10.5 at least) so we use standard 
   types and calls 
*/

#ifdef __APPLE__
#define off64_t off_t
#define fopen64 fopen
#endif

Metadata *       STARCH_createMetadata(char const *chr, 
                                       char const *fn, 
                                   uint64_t size,
                         Bed::LineCountType lineCount,
                         Bed::BaseCountType totalNonUniqueBases,
                         Bed::BaseCountType totalUniqueBases);

Metadata *       STARCH_addMetadata(Metadata *md, 
                                        char *chr, 
                                        char *fn, 
                                    uint64_t size,
                          Bed::LineCountType lineCount,
                          Bed::BaseCountType totalNonUniqueBases,
                          Bed::BaseCountType totalUniqueBases);

Metadata *       STARCH_copyMetadata(const Metadata *md);

int              STARCH_updateMetadataForChromosome(Metadata **md, 
                                                        char *chr, 
                                                        char *fn, 
                                                    uint64_t size,
                                          Bed::LineCountType lineCount,
                                          Bed::BaseCountType totalNonUniqueBases,
                                          Bed::BaseCountType totalUniqueBases);

int              STARCH_listMetadata(const Metadata *md,
                                         const char *chr);

void             STARCH_freeMetadata(Metadata **md);

int              STARCH_deleteCompressedFiles(const Metadata *md);

int              STARCH_writeJSONMetadata(const Metadata *md, 
                                                    char **buf, 
                                         CompressionType *type,
                                           const Boolean headerFlag,
                                              const char *note);

char *           STARCH_generateJSONMetadata(const Metadata *md, 
                                      const CompressionType type, 
                                       const ArchiveVersion *av,
                                                 const char *cTime,
                                                 const char *note,
                                              const Boolean headerFlag);

int              STARCH_readJSONMetadata(json_t **metadataJSON,
                                           FILE **fp, 
                                     const char *fn, 
                                       Metadata **rec, 
                                CompressionType *type, 
                                 ArchiveVersion **version, 
                                           char **cTime,
                                           char **note,
                                       uint64_t *mdOffset,
                                        Boolean *headerFlag,
                                  const Boolean suppressErrorMsgs,
                                  const Boolean preserveJSONRef);

int              STARCH_listJSONMetadata(FILE *out, 
                                         FILE *err, 
                               const Metadata *md, 
                        const CompressionType type, 
                         const ArchiveVersion *av,
                                   const char *cTime,
                                   const char *note,
                                const Boolean headerFlag,
                                const Boolean showNewlineFlag);

int              STARCH_listAllChromosomes(const Metadata *md);

int              STARCH_listChromosome(const Metadata *md, 
                                           const char *chr);

int              STARCH_mergeMetadataWithCompressedFiles(const Metadata *md, 
                                                                   char *mdHeader);

void             STARCH_buildProcessIDTag(char **tag);

int              STARCH_readLegacyMetadata(const char *buf,
                                             Metadata **rec,
                                      CompressionType *type,
                                       ArchiveVersion **version,
                                             uint64_t *mdOffset,
                                              Boolean *headerFlag,
                                        const Boolean suppressErrorMsgs);

char *           STARCH_strnstr(const char *haystack, 
                                const char *needle, 
                                    size_t haystackLen);

ArchiveVersion * STARCH_copyArchiveVersion(const ArchiveVersion *oav);

int              STARCH_chromosomeInMetadataRecords(const Metadata *md, 
                                                        const char *chr);

#endif
