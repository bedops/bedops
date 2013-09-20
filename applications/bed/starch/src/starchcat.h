//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchcat.h
//=========

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

#include "data/starch/unstarchHelpers.h"
#include "data/starch/starchMetadataHelpers.h"
#include "data/starch/starchConstants.h"
#include "suite/BEDOPS.Constants.hpp"

#define STARCHCAT_EXIT_FAILURE 0
#define STARCHCAT_EXIT_SUCCESS 1
#define STARCHCAT_FATAL_ERROR -1
#define STARCHCAT_HELP_ERROR -2
#define STARCHCAT_VERSION_ERROR -3
#define STARCHCAT_COPY_BUFFER_MAXSIZE 65536
#define STARCHCAT_RETRANSFORM_LINE_COUNT_MAX 100
#define STARCHCAT_RETRANSFORM_BUFFER_SIZE 1024*1024
#define STARCHCAT_FIELD_BUFFER_MAX_LENGTH 16

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
    Bed::LineCountType       t_lineIdx;
    Bed::SignedCoordType     t_start;
    Bed::SignedCoordType     t_pLength;
    Bed::SignedCoordType     t_lastEnd;
    char                     t_firstInputToken[UNSTARCH_FIRST_TOKEN_MAX_LENGTH + 1];
    char                     t_secondInputToken[UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1];
    char                     t_currentChromosome[Bed::TOKEN_CHR_MAX_LENGTH + 1];
    size_t                   t_currentChromosomeLength;
    Bed::SignedCoordType     t_currentStart;
    Bed::SignedCoordType     t_currentStop;
    char                     t_currentRemainder[UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1];
    size_t                   t_currentRemainderLength;
    Bed::SignedCoordType     t_lastPosition;
    Bed::SignedCoordType     t_lcDiff;    
    size_t                   t_nExtractionBuffer;
    size_t                   t_nExtractionBufferPos;
    char                     r_chromosome[Bed::TOKEN_CHR_MAX_LENGTH + 1];
    char                     r_remainder[UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1];
    Bed::SignedCoordType     r_start;
    Bed::SignedCoordType     r_stop;
    Bed::SignedCoordType     r_coordDiff;
    Bed::SignedCoordType     r_lcDiff;
    Bed::SignedCoordType     r_lastPosition;
    Bed::SignedCoordType     r_previousStop;
    uint64_t                 r_totalNonUniqueBases;
    uint64_t                 r_totalUniqueBases;
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
    "USAGE: starchcat [ --note=\"...\" ] [ --bzip2 | --gzip ] <starch-file-1> [<starch-file-2> ...]\n" \
    "\n" \
    "    * At least one lexicographically-sorted, headerless starch archive is required.\n" \
    "      While two or more inputs make sense for a multiset union operation, you can starchcat \n" \
    "      one file in order to update its metadata, recompress it with a different backend method,\n" \
    "      or add a note annotation.\n" \
    "\n" \
    "    * Compressed data are sent to standard output. Use the '>' operator to redirect\n" \
    "      to a file.\n" \
    "\n" \
    "    Process Flags:\n\n" \
    "    --note=\"foo bar...\"   Append note to output archive metadata (optional)\n" \
    "    --bzip2 | --gzip      Specify backend compression type (optional, default is bzip2)\n" \
    "    --version             Show binary version\n" \
    "    --help                Show this usage message\n";

static struct starchcat_client_global_args_t {
    CompressionType compressionType;
    char *note;
    char **inputFiles;
    size_t numberInputFiles;
} starchcat_client_global_args;

static struct option starchcat_client_long_options[] = {    
    {"note",    required_argument, NULL, 'n'},
    {"bzip2",   no_argument,       NULL, 'b'},
    {"gzip",    no_argument,       NULL, 'g'},
    {"version", no_argument,       NULL, 'v'},
    {"help",    no_argument,       NULL, 'h'},
    {NULL,      no_argument,       NULL, 0}
};

static const char *starchcat_client_opt_string = "n:bgvh?";

void     STARCHCAT_initializeGlobals();

int      STARCHCAT_parseCommandLineOptions(int argc, 
                                          char **argv);

int      STARCHCAT2_copyInputRecordToOutput (Metadata **outMd,
                                           const char *outTag,
                                const CompressionType outType,
                                           const char *inChr,
                                 const MetadataRecord *inRec,
                                               size_t *cumulativeOutputSize);

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
                                                  size_t *cumulativeOutputSize);

int      STARCHCAT_rewriteInputRecordToOutput (Metadata **outMd,
                                             const char *outTag,
                                  const CompressionType outType,
                                             const char *inChr,
                                   const MetadataRecord *inRec);

int      STARCHCAT2_parseCoordinatesFromBedLine(const char *lineBuf, 
                                              const size_t inRecIdx, 
                                      Bed::SignedCoordType *starts, 
                                      Bed::SignedCoordType *stops);

int      STARCHCAT2_identifyLowestBedElement(const Boolean *eobFlags,
                                const Bed::SignedCoordType *starts, 
                                const Bed::SignedCoordType *stops, 
                                              const size_t numRecords, 
                                                    size_t *lowestIdx);

int      STARCHCAT2_pullNextBedElement (const size_t recIdx,
                                          const char **inLinesBuf,
                            const Bed::LineCountType *nInLinesBuf,
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
                                                               size_t *cumulativeOutputSize);

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
int      STARCHCAT2_parseCoordinatesFromBedLineV2 (Boolean *eobFlag, const char *extractedElement, Bed::SignedCoordType *start, Bed::SignedCoordType *stop);
int      STARCHCAT2_addLowestBedElementToCompressionBuffer (char *compressionBuffer, const char *extractedElement, Bed::LineCountType *compressionLineCount);
int      STARCHCAT2_transformCompressionBuffer (const char *compressionBuffer, char *retransformedOutputBuffer, TransformState *retransState);
int      STARCHCAT2_squeezeRetransformedOutputBufferToBzip2Stream (BZFILE **bzStream, char *transformedBuffer);
int      STARCHCAT2_squeezeRetransformedOutputBufferToGzipStream (z_stream *zStream, const Boolean flushZStreamFlag, char *transformedBuffer, uint64_t *finalStreamSize, size_t *cumulativeOutputSize);
int      STARCHCAT2_resetCompressionBuffer (char *compressionBuffer, Bed::LineCountType *compressionLineCount);
int      STARCHCAT2_finalizeMetadata (Metadata **outMd, char *finalChromosome, char *finalOutTagFn, uint64_t finalStreamSize, uint64_t finalLineCount, uint64_t finalTotalNonUniqueBases, uint64_t finalTotalUniqueBases);

#endif
