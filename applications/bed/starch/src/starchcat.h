//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchcat.h
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

#ifndef STARCHCAT_H
#define STARCHCAT_H

#ifdef __cplusplus
#include <cinttypes>
#else
#include <inttypes.h>
#endif

#include <getopt.h>
#include <bzlib.h>
#include <zlib.h>
#include <errno.h>

#include "data/starch/unstarchHelpers.h"
#include "data/starch/starchMetadataHelpers.h"
#include "data/starch/starchConstants.h"
#include "suite/BEDOPS.Constants.hpp"

#ifdef __cplusplus
namespace starch {
  using namespace Bed;
#endif

#define STARCHCAT_EXIT_FAILURE 0
#define STARCHCAT_EXIT_SUCCESS 1
#define STARCHCAT_FATAL_ERROR -1
#define STARCHCAT_HELP_ERROR -2
#define STARCHCAT_VERSION_ERROR -3
#define STARCHCAT_COPY_BUFFER_MAXSIZE 65536
#define STARCHCAT_RETRANSFORM_LINE_COUNT_MAX 100
#define STARCHCAT_RETRANSFORM_BUFFER_SIZE 1024*1024
#define STARCHCAT_FIELD_BUFFER_MAX_LENGTH 16

#if defined(__GNUC__) && !defined(__clang__)
#define HAS_GNU 1
#else
#define HAS_GNU 0
#endif

/*
    This is simply a struct containing a starch file's
    metadata, pathname and other data useful for our 
    purposes. We'll parse each metadata to find the 
    chromosomes within.
*/

typedef struct metadataRecord {
    Metadata         *metadata;
    char             *filename;
    FILE             *fp;
    CompressionType   type;
    Boolean           hFlag; /* header flag */
    uint64_t          mdOffset;
    ArchiveVersion   *av;
    char             *cTime;
} MetadataRecord;

/*
    This struct manages the parse data for the
    recompression stage. An array of these is
    created, one for each input stream.
*/

typedef struct transformState {
    LineCountType            t_lineIdx;
    SignedCoordType          t_start;
    SignedCoordType          t_pLength;
    SignedCoordType          t_lastEnd;
    char                     t_firstInputToken[UNSTARCH_FIRST_TOKEN_MAX_LENGTH + 1];
    char                     t_secondInputToken[UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1];
    char                     t_currentChromosome[TOKEN_CHR_MAX_LENGTH];
    size_t                   t_currentChromosomeLength;
    SignedCoordType          t_currentStart;
    SignedCoordType          t_currentStop;
    char                     t_currentRemainder[UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1];
    size_t                   t_currentRemainderLength;
    SignedCoordType          t_lastPosition;
    SignedCoordType          t_lcDiff;    
    size_t                   t_nExtractionBuffer;
    size_t                   t_nExtractionBufferPos;
    char                     r_chromosome[TOKEN_CHR_MAX_LENGTH];
    char                     r_remainder[UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1];
    SignedCoordType          r_start;
    SignedCoordType          r_stop;
    SignedCoordType          r_pStart;
    SignedCoordType          r_pStop;
    SignedCoordType          r_coordDiff;
    SignedCoordType          r_lcDiff;
    SignedCoordType          r_lastPosition;
    SignedCoordType          r_previousStop;
    uint64_t                 r_totalNonUniqueBases;
    uint64_t                 r_totalUniqueBases;
    Boolean                  r_duplicateElementExists;
    Boolean                  r_nestedElementExists;
    char *                   r_signature;
    LineLengthType           r_lineMaxStringLength;
    size_t                   r_nRetransBuf;
} TransformState;

/* 
    An array of MetadataRecords gets compressed down
    to a uniquified, sorted list of chromosomes, which 
    in turn is summarized as an array of chromosomes
    and records (an instance of ChromosomeSummaries*).
*/

typedef struct chromosomeSummary {
    char *chromosome;
    MetadataRecord **records;
    unsigned int numRecords;
} ChromosomeSummary;

typedef struct chromosomeSummaries {
    ChromosomeSummary *summary;
    unsigned int numChromosomes;
} ChromosomeSummaries;

static const char *name = "starchcat";
static const char *authors = "Alex Reynolds and Shane Neph";
static const char *usage = "\n" \
    "USAGE: starchcat [ --note=\"...\" ]\n" \
    "                 [ --bzip2 | --gzip ]\n" \
    "                 [ --omit-signature ]\n" \
    "                 [ --report-progress=N ] <starch-file-1> [<starch-file-2> ...]\n" \
    "\n" \
    "    * At least one lexicographically-sorted, headerless starch archive is\n" \
    "      required.\n\n" \
    "    * While two or more inputs make sense for a multiset union operation, you\n" \
    "      can starchcat one file in order to update its metadata, recompress it\n" \
    "      with a different backend method, or add a note annotation.\n" \
    "\n" \
    "    * Compressed data are sent to standard output. Use the '>' operator to\n" \
    "      redirect to a file.\n" \
    "\n" \
    "    Process Flags\n" \
    "    --------------------------------------------------------------------------\n" \
    "    --note=\"foo bar...\"   Append note to output archive metadata (optional).\n\n" \
    "    --bzip2 | --gzip      Specify backend compression type (optional, default\n" \
    "                          is bzip2).\n\n" \
    "    --omit-signature      Skip generating per-chromosome data integrity signature\n" \
    "                          (optional, default is to generate signature).\n\n" \
    "    --report-progress=N   Report compression progress every N elements per\n" \
    "                          chromosome to standard error stream (optional)\n\n" \
    "    --version             Show binary version.\n\n" \
    "    --help                Show this usage message.\n";

static struct starchcat_client_global_args_t {
    CompressionType compressionType;
    char *note;
    char **inputFiles;
    size_t numberInputFiles;
    Boolean generatePerChromosomeSignatureFlag;
    Boolean reportProgressFlag;
    LineCountType reportProgressN;
} starchcat_client_global_args;

#ifdef __cplusplus
static struct option starchcat_client_long_options[] = {    
    {"note",            required_argument, nullptr, 'n'},
    {"bzip2",           no_argument,       nullptr, 'b'},
    {"gzip",            no_argument,       nullptr, 'g'},
    {"omit-signature",  no_argument,       nullptr, 'o'},
    {"report-progress", required_argument, nullptr, 'r'},
    {"version",         no_argument,       nullptr, 'v'},
    {"help",            no_argument,       nullptr, 'h'},
    {nullptr,           no_argument,       nullptr,  0 }
};
#else
static struct option starchcat_client_long_options[] = {    
    {"note",            required_argument, NULL, 'n'},
    {"bzip2",           no_argument,       NULL, 'b'},
    {"gzip",            no_argument,       NULL, 'g'},
    {"omit-signature",  no_argument,       NULL, 'o'},
    {"report-progress", required_argument, NULL, 'r'},
    {"version",         no_argument,       NULL, 'v'},
    {"help",            no_argument,       NULL, 'h'},
    {NULL,              no_argument,       NULL,  0 }
};
#endif

static const char *starchcat_client_opt_string = "n:bgorvh?";

void     STARCHCAT_initializeGlobals();

int      STARCHCAT_parseCommandLineOptions(int argc, 
                                          char **argv);

int      STARCHCAT2_copyInputRecordToOutput (Metadata **outMd,
                                           const char *outTag,
                                const CompressionType outType,
                                           const char *inChr,
                                 const MetadataRecord *inRec,
                                               size_t *cumulativeOutputSize,
                                        const Boolean reportProgressFlag);

int      STARCHCAT_copyInputRecordToOutput (Metadata **outMd,
                                          const char *outTag,
                               const CompressionType outType,
                                          const char *inChr,
                                const MetadataRecord *inRec);

int      STARCHCAT2_rewriteInputRecordToOutput (Metadata **outMd, 
                                              const char *outTag, 
                                   const CompressionType outType, 
                                              const char *inChr, 
                                    const MetadataRecord *inRec,
                                                  size_t *cumulativeOutputSize,
                                           const Boolean generatePerChrSignatureFlag,
                                           const Boolean reportProgressFlag,
                                     const LineCountType reportProgressN);

int      STARCHCAT_rewriteInputRecordToOutput (Metadata **outMd,
                                             const char *outTag,
                                  const CompressionType outType,
                                             const char *inChr,
                                   const MetadataRecord *inRec);

int      STARCHCAT2_parseCoordinatesFromBedLine(const char *lineBuf, 
                                              const size_t inRecIdx, 
                                           SignedCoordType *starts, 
                                           SignedCoordType *stops);

int      STARCHCAT2_identifyLowestBedElement(const Boolean *eobFlags,
                                     const SignedCoordType *starts, 
                                     const SignedCoordType *stops, 
                                              const size_t numRecords, 
                                                    size_t *lowestIdx);

int      STARCHCAT2_identifyLowestBedElementV2p2(const Boolean *eobFlags,
                                         const SignedCoordType *starts, 
                                         const SignedCoordType *stops, 
                                                    const char **remainders,
                                                  const size_t numRecords, 
                                                        size_t *lowestIdx);

int      STARCHCAT2_pullNextBedElement (const size_t recIdx,
                                          const char **inLinesBuf,
                                 const LineCountType *nInLinesBuf,
                                                char **outLineBuf, 
                                            uint64_t **inBufNewlineOffsets);

int      STARCHCAT_mergeInputRecordsToOutput (Metadata **outMd,
                                            const char *outTag,
                                 const CompressionType outType,
                               const ChromosomeSummary *summary);

int      STARCHCAT_mergeChromosomeStreams (const ChromosomeSummaries *chrSums,
                                               const CompressionType outputType,
                                                          const char *note);

int      STARCHCAT2_mergeChromosomeStreams (const ChromosomeSummaries *chrSums,
                                                const CompressionType outputType,
                                                           const char *note,
                                                               size_t *cumulativeOutputSize,
                                                        const Boolean generatePerChrSignatureFlag,
                                                        const Boolean reportProgressFlag,
                                                  const LineCountType reportProgressN);

int      STARCHCAT_freeChromosomeNames (char ***chrs, 
                                unsigned int numChromosomes);

int      STARCHCAT_freeChromosomeSummaries (ChromosomeSummaries **chrSums);

int      STARCHCAT_allocChromosomeSummaries (ChromosomeSummaries **chrSums, 
                                              const unsigned int numChromosomes);

int      STARCHCAT_buildChromosomeSummaries (ChromosomeSummaries **chrSums, 
                                         const ChromosomeSummary *summary, 
                                              const unsigned int numChromosomes);

int      STARCHCAT_printChromosomeSummaries (const ChromosomeSummaries *chrSums);

int      STARCHCAT_freeChromosomeSummary (ChromosomeSummary **summary, 
                                         const unsigned int numChromosomes);

int      STARCHCAT_allocChromosomeSummary (ChromosomeSummary **summary, 
                                          const unsigned int numChromosomes);

int      STARCHCAT_buildChromosomeSummary (ChromosomeSummary **summary, 
                                        const MetadataRecord *mdRecords, 
                                          const unsigned int numRecords,
                                                  const char **chromosomes, 
                                          const unsigned int numChromosomes);

MetadataRecord * 
         STARCHCAT_copyMetadataRecord (const MetadataRecord *mdRec);

int      STARCHCAT_buildUniqueChromosomeList (char ***chromosomes,
                                      unsigned int *numChr,
                              const MetadataRecord *mdRecords,
                                const unsigned int numRecords);

int      STARCHCAT_compareCStrings (const void *a,
                                    const void *b);

int      STARCHCAT_allocMetadataRecords (MetadataRecord **mdRecords, 
                                     const unsigned int numRecords);

int      STARCHCAT_freeMetadataRecords (MetadataRecord **mdRecords,
                                    const unsigned int numRecords);

int      STARCHCAT_buildMetadataRecords (json_t ***metadataJSONs,
                                 MetadataRecord **mdRecords,
                             const unsigned int firstArgc, 
                                      const int argc,
                                     const char **argv);

Boolean  STARCHCAT_fileExists (const char *fn);

void     STARCHCAT_printUsage (int t);

void     STARCHCAT_printRevision ();

Boolean  STARCHCAT_isArchiveConcurrent (const ArchiveVersion *av);

Boolean  STARCHCAT_isArchiveConcurrentOrOlder (const ArchiveVersion *av);

Boolean  STARCHCAT_isArchiveOlder (const ArchiveVersion *av);

Boolean  STARCHCAT_isArchiveNewer (const ArchiveVersion *av);

Boolean  STARCHCAT_isArchiveNewerThan (const ArchiveVersion *av,
                                       const ArchiveVersion *comp);

Boolean  STARCHCAT_allocMetadataJSONObjects (json_t ***mdJSONs, 
                                 const unsigned int numRecs);

Boolean  STARCHCAT_freeMetadataJSONObjects (json_t ***mdJSONs, 
                                const unsigned int numRecs);

Boolean  STARCHCAT_checkMetadataJSONVersions (json_t ***mdJSONs,
                                  const unsigned int numRecs);

int      STARCHCAT2_mergeInputRecordsToOutput (const char *inChr, 
                                                 Metadata **outMd, 
                                               const char *outTag, 
                                    const CompressionType outType, 
                                  const ChromosomeSummary *summary, 
                                                   size_t *cumulativeOutputSize);

int      STARCHCAT2_setupBzip2OutputStream (BZFILE **bzStream, FILE *outStream);
int      STARCHCAT2_setupGzipOutputStream (z_stream *zStream);
int      STARCHCAT2_testSummaryForChromosomeExistence (const char *chrName, const ChromosomeSummary *chrSummary, const size_t recIndex);
int      STARCHCAT2_setupInitialFileOffsets (const char *chrName, const ChromosomeSummary *chrSummary, const size_t recIndex);
int      STARCHCAT2_setupBzip2InputStream (const size_t recIdx, const ChromosomeSummary *chrSummary, BZFILE **bzStream);
int      STARCHCAT2_setupGzipInputStream (z_stream *zStream);
int      STARCHCAT2_breakdownBzip2InputStream (BZFILE **bzStream);
int      STARCHCAT2_breakdownGzipInputStream (z_stream *zStream);
int      STARCHCAT2_breakdownBzip2OutputStream (BZFILE **bzStream, uint64_t *bzOutBytesConsumed, uint64_t *bzOutBytesWritten);
int      STARCHCAT2_breakdownGzipOutputStream (z_stream *zStream);
int      STARCHCAT2_fillExtractionBufferFromBzip2Stream (Boolean *eofFlag, char *recordChromosome, char *extractionBuffer, size_t *nExtractionBuffer, BZFILE **bzStream, size_t *nBzRead, char *bzRemainderBuf, size_t *nBzRemainderBuf, TransformState *t_state);
int      STARCHCAT2_fillExtractionBufferFromGzipStream (Boolean *eofFlag, FILE **inputFp, char *recordChromosome, char *extractionBuffer, size_t *nExtractionBuffer, z_stream *zStream, size_t *nZRead, char **zRemainderBuf, size_t *nZRemainderBuf, TransformState *t_state);
int      STARCHCAT2_extractBedLine (Boolean *eobFlag, char *extractionBuffer, int *extractionBufferOffset, char **extractedElement);
int      STARCHCAT2_parseCoordinatesFromBedLineV2 (Boolean *eobFlag, const char *extractedElement, SignedCoordType *start, SignedCoordType *stop);
int      STARCHCAT2_parseCoordinatesFromBedLineV2p2 (Boolean *eobFlag, const char *extractedElement, SignedCoordType *start, SignedCoordType *stop, char **remainder);
int      STARCHCAT2_addLowestBedElementToCompressionBuffer (char *compressionBuffer, const char *extractedElement, LineCountType *compressionLineCount);
int      STARCHCAT2_transformCompressionBuffer (const char *compressionBuffer, char *retransformedOutputBuffer, TransformState *retransState);
int      STARCHCAT2_squeezeRetransformedOutputBufferToBzip2Stream (BZFILE **bzStream, char *transformedBuffer);
int      STARCHCAT2_squeezeRetransformedOutputBufferToGzipStream (z_stream *zStream, const Boolean flushZStreamFlag, char *transformedBuffer, uint64_t *finalStreamSize, size_t *cumulativeOutputSize);
int      STARCHCAT2_resetCompressionBuffer (char *compressionBuffer, LineCountType *compressionLineCount);

int      STARCHCAT2_finalizeMetadata (Metadata **outMd, 
                                          char *finalChromosome, 
                                          char *finalOutTagFn, 
                                      uint64_t finalStreamSize, 
                                      uint64_t finalLineCount, 
                                      uint64_t finalTotalNonUniqueBases, 
                                      uint64_t finalTotalUniqueBases,
                                       Boolean finalDuplicateElementExists,
                                       Boolean finalNestedElementExists,
                                          char *finalSignature,
                                LineLengthType finalLineMaxStringLength);

#ifdef __cplusplus
} // namespace starch
#endif

#endif
