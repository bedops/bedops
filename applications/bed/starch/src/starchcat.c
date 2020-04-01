//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchcat.c
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
#include <cstdint>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>
#include <cassert>
#include <climits>
#include <limits>
#else
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <assert.h>
#include <limits.h>
#endif

#include <errno.h>
#include <sys/stat.h>

#include "starchcat.h"

#include "data/starch/starchBase64Coding.h"
#include "data/starch/starchSha1Digest.h"
#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchHelpers.h"
#include "data/starch/starchConstants.h"
#include "suite/BEDOPS.Version.hpp"

#ifdef __cplusplus
namespace {
  using namespace Bed;
  using namespace starch;
} // unnamed namespace
#endif

int
main (int argc, char **argv)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- starchcat main() - enter ---\n");
#endif
#ifdef __cplusplus
    char *note = nullptr;
    ChromosomeSummaries *summaries = nullptr;
    ChromosomeSummary *summary = nullptr;
    MetadataRecord *mdRecords = nullptr;
    char **chromosomes = nullptr;
    json_t **metadataJSONs = nullptr;
    unsigned char *header = nullptr;
#else
    char *note = NULL;
    ChromosomeSummaries *summaries = NULL;
    ChromosomeSummary *summary = NULL;
    MetadataRecord *mdRecords = NULL;
    char **chromosomes = NULL;
    json_t **metadataJSONs = NULL;
    unsigned char *header = NULL;
#endif
    CompressionType outputType = STARCH_DEFAULT_COMPRESSION_TYPE;
    int parseResult = 0;
    unsigned int firstArgc = UINT_MAX;
    unsigned int numRecords = 0U;
    unsigned int numChromosomes = 0U;
    size_t cumulativeRecSize = 0U;
    Boolean bedGeneratePerChrSignatureFlag = kStarchFalse;
    Boolean bedReportProgressFlag = kStarchFalse;
    LineCountType bedReportProgressN = 0;

    setlocale(LC_ALL, "POSIX");

#ifdef DEBUG
    fprintf (stderr, "\tparsing command-line options\n");
#endif

    parseResult = STARCHCAT_parseCommandLineOptions(argc, argv);
    note = starchcat_client_global_args.note;
    outputType = starchcat_client_global_args.compressionType;
    bedGeneratePerChrSignatureFlag = starchcat_client_global_args.generatePerChromosomeSignatureFlag;
    bedReportProgressFlag = starchcat_client_global_args.reportProgressFlag;
    bedReportProgressN = starchcat_client_global_args.reportProgressN;
#ifdef __cplusplus
    firstArgc = static_cast<unsigned int>( argc ) - static_cast<unsigned int>( starchcat_client_global_args.numberInputFiles );
#else
    firstArgc = (unsigned int) argc - (unsigned int) starchcat_client_global_args.numberInputFiles;
#endif

    switch (parseResult) {
        case STARCHCAT_EXIT_SUCCESS: {

#ifdef __cplusplus
            if (firstArgc < static_cast<unsigned int>( argc ))
                numRecords = static_cast<unsigned int>( argc ) - firstArgc;
#else
            if (firstArgc < (unsigned int) argc)
                numRecords = (unsigned int) argc - firstArgc;
#endif
            else {
                fprintf(stderr, "ERROR: No files specified\n");
                STARCHCAT_printUsage(STARCHCAT_FATAL_ERROR);
                return EXIT_FAILURE;
            }
#ifdef DEBUG
            fprintf(stderr, "\t--- PARSING METADATA ---\n\n");
#endif
#ifdef __cplusplus
            assert( STARCHCAT_allocMetadataRecords      ( &mdRecords, static_cast<const unsigned int>( numRecords ) ) );
            assert( STARCHCAT_allocMetadataJSONObjects  ( &metadataJSONs, static_cast<const unsigned int>( numRecords ) ) );
            assert( STARCHCAT_buildMetadataRecords      ( &metadataJSONs, &mdRecords, static_cast<const unsigned int>( firstArgc ), static_cast<const int>( argc ), const_cast<const char **>( argv ) ) );
            assert( STARCHCAT_checkMetadataJSONVersions ( &metadataJSONs, static_cast<const unsigned int>( numRecords ) ) );
            assert( STARCHCAT_buildUniqueChromosomeList ( &chromosomes, &numChromosomes, reinterpret_cast<const MetadataRecord *>( mdRecords ), static_cast<const unsigned int>( numRecords ) ) );
            assert( STARCHCAT_allocChromosomeSummary    ( &summary, static_cast<const unsigned int>( numChromosomes ) ) );
            assert( STARCHCAT_buildChromosomeSummary    ( &summary, reinterpret_cast<const MetadataRecord *>( mdRecords ), static_cast<const unsigned int>( numRecords ), const_cast<const char **>( chromosomes ), static_cast<const unsigned int>( numChromosomes ) ) );
            assert( STARCHCAT_allocChromosomeSummaries  ( &summaries, static_cast<const unsigned int>( numChromosomes ) ) );
            assert( STARCHCAT_buildChromosomeSummaries  ( &summaries, reinterpret_cast<const ChromosomeSummary *>( summary ), static_cast<const unsigned int>( numChromosomes ) ) );
            assert( STARCHCAT_printChromosomeSummaries  ( reinterpret_cast<const ChromosomeSummaries *>( summaries ) ) );
#else
            assert( STARCHCAT_allocMetadataRecords      ( &mdRecords, (const unsigned int) numRecords ) );
            assert( STARCHCAT_allocMetadataJSONObjects  ( &metadataJSONs, (const unsigned int) numRecords ) );
            assert( STARCHCAT_buildMetadataRecords      ( &metadataJSONs, &mdRecords, (const unsigned int) firstArgc, (const int) argc, (const char **) argv ) );
            assert( STARCHCAT_checkMetadataJSONVersions ( &metadataJSONs, (const unsigned int) numRecords ) );
            assert( STARCHCAT_buildUniqueChromosomeList ( &chromosomes, &numChromosomes, (const MetadataRecord *) mdRecords, (const unsigned int) numRecords ) );
            assert( STARCHCAT_allocChromosomeSummary    ( &summary, (const unsigned int) numChromosomes ) );
            assert( STARCHCAT_buildChromosomeSummary    ( &summary, (const MetadataRecord *) mdRecords, (const unsigned int) numRecords, (const char **) chromosomes, (const unsigned int) numChromosomes ) );
            assert( STARCHCAT_allocChromosomeSummaries  ( &summaries, (const unsigned int) numChromosomes ) );
            assert( STARCHCAT_buildChromosomeSummaries  ( &summaries, (const ChromosomeSummary *) summary, (const unsigned int) numChromosomes ) );
            assert( STARCHCAT_printChromosomeSummaries  ( (const ChromosomeSummaries *) summaries ) );
#endif
#ifdef DEBUG
            fprintf(stderr, "\t--- MERGE ---\n\n");
#endif
            if (STARCH_MAJOR_VERSION == 1) {
#ifdef __cplusplus
                assert( STARCHCAT_mergeChromosomeStreams  ( reinterpret_cast<const ChromosomeSummaries *>( summaries ), 
                                                            static_cast<const CompressionType>( outputType ), 
                                                            reinterpret_cast<const char *>( note ) ) );
#else
                assert( STARCHCAT_mergeChromosomeStreams  ( (const ChromosomeSummaries *) summaries, 
                                                            (const CompressionType) outputType, 
                                                            (const char *) note ) );
#endif
            }
            else if (STARCH_MAJOR_VERSION == 2) {
                assert( STARCH2_initializeStarchHeader(&header) );
                assert( STARCH2_writeStarchHeaderToOutputFp(header, stdout) );
                cumulativeRecSize += STARCH2_MD_HEADER_BYTE_LENGTH;
#ifdef __cplusplus
                assert( STARCHCAT2_mergeChromosomeStreams ( reinterpret_cast<const ChromosomeSummaries *>( summaries ), 
                                                            static_cast<const CompressionType>( outputType ), 
                                                            reinterpret_cast<const char *>( note ),
                                                            &cumulativeRecSize,
                                                            bedGeneratePerChrSignatureFlag,
                                                            bedReportProgressFlag,
                                                            bedReportProgressN ) );
#else
                assert( STARCHCAT2_mergeChromosomeStreams ( (const ChromosomeSummaries *) summaries, 
                                                            (const CompressionType) outputType, 
                                                            (const char *) note,
                                                            &cumulativeRecSize,
                                                            bedGeneratePerChrSignatureFlag,
                                                            bedReportProgressFlag,
                                                            bedReportProgressN ) );
#endif
            }
            break;
        }
        case STARCHCAT_HELP_ERROR: {
            STARCHCAT_printUsage(parseResult);
            return EXIT_SUCCESS;
        }
        case STARCHCAT_VERSION_ERROR: {
            STARCHCAT_printRevision();
            return EXIT_SUCCESS;
        }
        case STARCHCAT_FATAL_ERROR: {
            STARCHCAT_printUsage(parseResult);
            return EXIT_FAILURE;
        }
        default: {
            fprintf(stderr, "ERROR: Unknown parsing error.\n");
            return EXIT_FAILURE;
        }
    }

#ifdef DEBUG
    fprintf(stderr, "\t--- CLEANUP ---\n\n");
#endif

    if (summaries)
        assert( STARCHCAT_freeChromosomeSummaries( &summaries ) );

    if (mdRecords)
#ifdef __cplusplus
        assert( STARCHCAT_freeMetadataRecords( &mdRecords, static_cast<const unsigned int>( numRecords ) ) );
#else
        assert( STARCHCAT_freeMetadataRecords( &mdRecords, (const unsigned int) numRecords ) );
#endif

    if (chromosomes)
        assert( STARCHCAT_freeChromosomeNames( &chromosomes, numChromosomes ) );

    if (metadataJSONs)
#ifdef __cplusplus
        assert (STARCHCAT_freeMetadataJSONObjects( &metadataJSONs, static_cast<const unsigned int>( numRecords ) ) );
#else
        assert (STARCHCAT_freeMetadataJSONObjects( &metadataJSONs, (const unsigned int) numRecords ) );
#endif

#ifdef DEBUG
    fprintf (stderr, "\n--- starchcat main() - exit ---\n");
#endif

    return EXIT_SUCCESS;
}

#ifdef __cplusplus
namespace starch {
#endif

void
STARCHCAT_initializeGlobals()
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_initializeGlobals() ---\n");
#endif

#ifdef __cplusplus
    starchcat_client_global_args.note = nullptr;
    starchcat_client_global_args.inputFiles = nullptr;
#else
    starchcat_client_global_args.note = NULL;
    starchcat_client_global_args.inputFiles = NULL;
#endif
    starchcat_client_global_args.compressionType = STARCH_DEFAULT_COMPRESSION_TYPE;
    starchcat_client_global_args.numberInputFiles = 0;
    starchcat_client_global_args.generatePerChromosomeSignatureFlag = kStarchTrue;
}

int
STARCHCAT_parseCommandLineOptions(int argc, char **argv)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_parseCommandLineOptions() ---\n");
#endif
    int starchcat_client_long_index;
    int starchcat_client_opt = getopt_long(argc, argv, starchcat_client_opt_string, starchcat_client_long_options, &starchcat_client_long_index);

    opterr = 0; /* disable error reporting by GNU getopt -- we handle this */
    STARCHCAT_initializeGlobals();

    while (starchcat_client_opt != -1) {
        switch (starchcat_client_opt) {
            case 'v':
                return STARCHCAT_VERSION_ERROR;
            case 'n':
                starchcat_client_global_args.note = optarg;
                break;
            case 'b':
                starchcat_client_global_args.compressionType = kBzip2;
                break;
            case 'g':
                starchcat_client_global_args.compressionType = kGzip;
                break;
            case 'o':
                starchcat_client_global_args.generatePerChromosomeSignatureFlag = kStarchFalse;
                break;
            case 'r':
                starchcat_client_global_args.reportProgressFlag = kStarchTrue;
                errno = 0;
#ifdef __cplusplus
                starchcat_client_global_args.reportProgressN = strtoumax(optarg, nullptr, 10);
#else
                starchcat_client_global_args.reportProgressN = strtoumax(optarg, NULL, 10);
#endif
                if (errno == ERANGE) {
                    fprintf (stderr, "ERROR: Numerical value is outside of range.\n");
                    return STARCHCAT_FATAL_ERROR;
                }
                break;
            case 'h':
                return STARCHCAT_HELP_ERROR;
            case '?':
                return STARCHCAT_FATAL_ERROR;
            default:
                break;
        }
        starchcat_client_opt = getopt_long(argc, argv, starchcat_client_opt_string, starchcat_client_long_options, &starchcat_client_long_index);
    }

    starchcat_client_global_args.inputFiles = argv + optind;

#ifdef __cplusplus
    starchcat_client_global_args.numberInputFiles = static_cast<size_t>( argc - optind );
#else
    starchcat_client_global_args.numberInputFiles = (size_t) (argc - optind);
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_copyInputRecordToOutput(Metadata **outMd, const char *outTag, const CompressionType outType, const char *inChr, const MetadataRecord *inRec, size_t *cumulativeOutputSize, const Boolean reportProgressFlag)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT2_copyInputRecordToOutput() ---\n");
#endif
    /*
        This function copies the bytes of a single record (chromosome) 
        of data to standard output. The output metadata specifics are copied
        over without alterations to the stream.
    */
#ifdef __cplusplus
    char *outFn = nullptr;
    char *outSignature = nullptr;
#else
    char *outFn = NULL;
    char *outSignature = NULL;
#endif
    FILE *outFnPtr = stdout;
    uint64_t startOffset = 0;
    uint64_t endOffset = 0;
    uint64_t outFileSize = 0;
    uint64_t outFileSizeCounter = 0;
    LineCountType outFileLineCount = 0;
    LineLengthType outFileLineMaxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
    BaseCountType outFileNonUniqueBases = 0;
    BaseCountType outFileUniqueBases = 0;
    Boolean outDuplicateElementExists = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean outNestedElementExists = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
    Metadata *iter, *inMd = inRec->metadata;
    const ArchiveVersion *av = inRec->av;
    char buffer[STARCHCAT_COPY_BUFFER_MAXSIZE];
    size_t nBytesRead = 0;

    if (!inMd) {
        fprintf(stderr, "ERROR: Could not locate input metadata.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

    switch (outType) {
        case kBzip2: {
#ifdef __cplusplus
            outFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 6) ); /* X.Y.bz2\0 */
#else
            outFn = malloc(strlen(inChr) + strlen(outTag) + 6); /* X.Y.bz2\0 */
#endif
            if (!outFn) {
                fprintf(stderr, "ERROR: Could not allocate space for output filename in input copy routine.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            sprintf(outFn, "%s.%s.bz2", inChr, outTag);
            break;
        }
        case kGzip: {
#ifdef __cplusplus
            outFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 5) ); /* X.Y.gz\0 */
#else
            outFn = malloc(strlen(inChr) + strlen(outTag) + 5); /* X.Y.gz\0 */
#endif
            if (!outFn) {
                fprintf(stderr, "ERROR: Could not allocate space for output filename in input copy routine.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            sprintf(outFn, "%s.%s.gz", inChr, outTag);
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Undefined compression type.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

#ifdef DEBUG
    fprintf(stderr, "\tinRec->chromosome -> %s \t av->major -> %d \t av->minor -> %d\n", inRec->metadata->chromosome, inRec->av->major, inRec->av->minor);
#endif

    /* determine the offsets of the stream-of-interest */
    if (av->major == 2)
        startOffset += STARCH2_MD_HEADER_BYTE_LENGTH;
    else
        startOffset += inRec->mdOffset;

#ifdef DEBUG
    fprintf(stderr, "\tdetermined startOffset -> %" PRId64 "\n", startOffset);
#endif

#ifdef __cplusplus
    for (iter = inMd; iter != nullptr; iter = iter->next) {
#else
    for (iter = inMd; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(iter->chromosome, inChr) == 0) {
            if (((av->major == 1) && (av->minor >= 4)) || (av->major == 2)) {
                outFileLineCount = iter->lineCount;
                outFileLineMaxStringLength = iter->lineMaxStringLength;
                outFileNonUniqueBases = iter->totalNonUniqueBases;
                outFileUniqueBases = iter->totalUniqueBases;
                outDuplicateElementExists = iter->duplicateElementExists;
                outNestedElementExists = iter->nestedElementExists;
                if (iter->signature) {
                    outSignature = STARCH_strndup(iter->signature, strlen(iter->signature) + 1);
                }
            }
            else if ((av->major == 1) && (av->minor >= 3))
                outFileLineCount = iter->lineCount;
            else {
                /* 
                    Something went wrong here. We needed to 
                    extract the stream, get the line count 
                    and put this into the metadata. Note that
                    we should never see this error, if the version
                    tests' logic is written correctly.
                */
                fprintf(stderr, "ERROR: Using older archive with newer starchcat requires line count migration.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            endOffset = startOffset + iter->size;
            break;
        }
        else {
            startOffset += iter->size;
        }
    }
#ifdef __cplusplus
    outFileSize = static_cast<uint64_t>( endOffset - startOffset );
#else
    outFileSize = (uint64_t) (endOffset - startOffset);
#endif

    *cumulativeOutputSize += outFileSize;

#ifdef __cplusplus
    fseeko(inRec->fp, static_cast<off_t>( startOffset ), SEEK_SET);
#else
    fseeko(inRec->fp, (off_t) startOffset, SEEK_SET);
#endif

#ifdef DEBUG
#ifdef __cplusplus
    fprintf(stderr, "\tstartOffset -> %" PRId64" \t outFileSize -> %" PRIu64 " \t *cumulativeOutputSize -> %" PRIu64 "\n", static_cast<uint64_t>( startOffset ), static_cast<uint64_t>( outFileSize ), static_cast<uint64_t>( *cumulativeOutputSize ));
#else
    fprintf(stderr, "\tstartOffset -> %" PRId64" \t outFileSize -> %" PRIu64 " \t *cumulativeOutputSize -> %" PRIu64 "\n", (uint64_t) startOffset, (uint64_t) outFileSize, (uint64_t) *cumulativeOutputSize);
#endif
#endif

    /* 
        Copy STARCHCAT_COPY_BUFFER_MAXSIZE-chunked bytes from input 
        stream to output file -- or less, if the output file size is
        smaller than STARCHCAT_COPY_BUFFER_MAXSIZE.
    */ 

    outFileSizeCounter = outFileSize;
    do {
        if (outFileSizeCounter > STARCHCAT_COPY_BUFFER_MAXSIZE) {
#ifdef __cplusplus
           nBytesRead = fread(buffer, sizeof(char), static_cast<size_t>( STARCHCAT_COPY_BUFFER_MAXSIZE ), inRec->fp);
#else
           nBytesRead = fread(buffer, sizeof(char), (size_t) STARCHCAT_COPY_BUFFER_MAXSIZE, inRec->fp);
#endif
           if (nBytesRead != STARCHCAT_COPY_BUFFER_MAXSIZE * sizeof(char)) {
              fprintf(stderr, "ERROR: Was not able to copy sufficient bytes into buffer (STARCHCAT_COPY_BUFFER_MAXSIZE).\n");
              return STARCHCAT_EXIT_FAILURE;
           }
#ifdef __cplusplus
           fwrite(buffer, sizeof(char), static_cast<size_t>( STARCHCAT_COPY_BUFFER_MAXSIZE ), outFnPtr);
#else
           fwrite(buffer, sizeof(char), (size_t) STARCHCAT_COPY_BUFFER_MAXSIZE, outFnPtr);
#endif
           outFileSizeCounter -= STARCHCAT_COPY_BUFFER_MAXSIZE;
       }
       else {
#ifdef __cplusplus
           nBytesRead = fread(buffer, sizeof(char), static_cast<size_t>( outFileSizeCounter ), inRec->fp);
#else
           nBytesRead = fread(buffer, sizeof(char), (size_t) outFileSizeCounter, inRec->fp);
#endif
           if (nBytesRead != outFileSizeCounter * sizeof(char)) {
              fprintf(stderr, "ERROR: Was not able to copy sufficient bytes into buffer (outFileSizeCounter).\n");
              return STARCHCAT_EXIT_FAILURE;
           }
#ifdef __cplusplus
           fwrite(buffer, sizeof(char), static_cast<size_t>( outFileSizeCounter ), outFnPtr);
#else
           fwrite(buffer, sizeof(char), (size_t) outFileSizeCounter, outFnPtr);
#endif
           outFileSizeCounter = 0ULL;
       }
    } while (outFileSizeCounter > 0);

    if (reportProgressFlag) {
        fprintf(stderr, "PROGRESS: Copied chromosome [%s] to output stream\n", inChr);
    }

    /* update output metadata */
    if (! *outMd) {
#ifdef DEBUG
        fprintf(stderr, "\t\tmaking new output metadata structure... (%" PRIu64 ")\n", outFileSize);
#endif
#ifdef __cplusplus
        *outMd = STARCH_createMetadata( const_cast<char *>( inChr ), 
#else
        *outMd = STARCH_createMetadata( (char *) inChr, 
#endif
                                        outFn, 
                                        outFileSize, 
                                        outFileLineCount, 
                                        outFileNonUniqueBases, 
                                        outFileUniqueBases,
                                        outDuplicateElementExists,
                                        outNestedElementExists,
                                        outSignature,
                                        outFileLineMaxStringLength );
    }
    else {
#ifdef DEBUG
        fprintf(stderr, "\t\tappending to existing output metadata structure...\n");
#endif
        *outMd = STARCH_addMetadata( *outMd, 
#ifdef __cplusplus
                                     const_cast<char *>( inChr ), 
#else
                                     (char *) inChr, 
#endif
                                     outFn, 
                                     outFileSize, 
                                     outFileLineCount, 
                                     outFileNonUniqueBases, 
                                     outFileUniqueBases,
                                     outDuplicateElementExists,
                                     outNestedElementExists,
                                     outSignature,
                                     outFileLineMaxStringLength );
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_copyInputRecordToOutput(Metadata **outMd, const char *outTag, const CompressionType outType, const char *inChr, const MetadataRecord *inRec)
{
    /*
        This function copies the bytes of a single record (chromosome) 
        of data to a temporary file. The temporary file remains compressed
        with the same compression type and is added to the output metadata
        without alterations to the stream.
    */
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_copyInputRecordToOutput() ---\n");
#endif
#ifdef __cplusplus
    FILE *outFnPtr = nullptr;
    char *outFn = nullptr;
    char *outSignature = nullptr;
#else
    FILE *outFnPtr = NULL;
    char *outFn = NULL;
    char *outSignature = NULL;
#endif
    uint64_t startOffset = inRec->mdOffset;
    uint64_t endOffset = 0;
    uint64_t outFileSize = 0;
    uint64_t outFileSizeCounter = 0;
    LineCountType outFileLineCount = 0;
    LineLengthType outFileLineMaxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
    BaseCountType outFileNonUniqueBases = 0;
    BaseCountType outFileUniqueBases = 0;
    Boolean outDuplicateElementExists = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean outNestedElementExists = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
    Metadata *iter, *inMd = inRec->metadata;
    const ArchiveVersion *av = inRec->av;
    char buffer[STARCHCAT_COPY_BUFFER_MAXSIZE];
    size_t nBytesRead = 0;

    if (!inMd) {
        fprintf(stderr, "ERROR: Could not locate input metadata.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

    /* build temporary output filename */
    switch (outType) {
        case kBzip2: {
#ifdef __cplusplus
            outFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 6) ); /* X.Y.bz2\0 */
#else
            outFn = malloc(strlen(inChr) + strlen(outTag) + 6); /* X.Y.bz2\0 */
#endif
            if (!outFn) {
                fprintf(stderr, "ERROR: Could not allocate space for output filename in input copy routine.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            sprintf(outFn, "%s.%s.bz2", inChr, outTag);
            break;
        }
        case kGzip: {
#ifdef __cplusplus
            outFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 5) ); /* X.Y.gz\0 */
#else
            outFn = malloc(strlen(inChr) + strlen(outTag) + 5); /* X.Y.gz\0 */
#endif
            if (!outFn) {
                fprintf(stderr, "ERROR: Could not allocate space for output filename in input copy routine.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            sprintf(outFn, "%s.%s.gz", inChr, outTag);
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Undefined compression type.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }
    outFnPtr = STARCH_fopen(outFn, "wb");
    if (!outFnPtr) {
        fprintf(stderr, "ERROR: Could not open an output file handle to %s\n", outFn);
        return STARCHCAT_EXIT_FAILURE;
    }

    /* determine the offsets of the stream-of-interest */
#ifdef __cplusplus
    for (iter = inMd; iter != nullptr; iter = iter->next) {
#else
    for (iter = inMd; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(iter->chromosome, inChr) != 0)
            startOffset += iter->size;
        else {
            if ((av->major >= 1) && (av->minor >= 4)) {
                outFileLineCount = iter->lineCount;
                outFileNonUniqueBases = iter->totalNonUniqueBases;
                outFileUniqueBases = iter->totalUniqueBases;
                outDuplicateElementExists = iter->duplicateElementExists;
                outNestedElementExists = iter->nestedElementExists;
            }
            else if ((av->major >= 1) && (av->minor >= 3))
                outFileLineCount = iter->lineCount;
            else {
                /* 
                   something went wrong here. we needed to 
                   extract the stream, get the line count 
                   and put this into the metadata.
                */
                fprintf(stderr, "ERROR: Using older archive with newer starchcat requires line count migration.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
#ifdef __cplusplus
            endOffset = static_cast<uint64_t>( startOffset + iter->size );
#else
            endOffset = (uint64_t) (startOffset + iter->size);
#endif
            break;
        }
    }
#ifdef __cplusplus
    outFileSize = static_cast<uint64_t>( endOffset - startOffset );
#else
    outFileSize = (uint64_t) (endOffset - startOffset);
#endif

#ifdef DEBUG
    fprintf(stderr, "\t\tfile offsets: %" PRId64 " - %" PRId64 "\n", startOffset, endOffset);
#endif

    /* 
        copy STARCHCAT_COPY_BUFFER_MAXSIZE-chunked bytes from input 
        stream to output file, or less, if the output file size is
        smaller than STARCHCAT_COPY_BUFFER_MAXSIZE
    */ 

    outFileSizeCounter = outFileSize;
#ifdef __cplusplus
    fseeko(inRec->fp, static_cast<off_t>( startOffset ), SEEK_SET);
#else
    fseeko(inRec->fp, (off_t) startOffset, SEEK_SET);
#endif
    do {
        if (outFileSizeCounter > STARCHCAT_COPY_BUFFER_MAXSIZE) {
#ifdef __cplusplus
            nBytesRead = fread(buffer, sizeof(char), static_cast<size_t>( STARCHCAT_COPY_BUFFER_MAXSIZE ), inRec->fp);
#else
            nBytesRead = fread(buffer, sizeof(char), (size_t) STARCHCAT_COPY_BUFFER_MAXSIZE, inRec->fp);
#endif
            if (nBytesRead != STARCHCAT_COPY_BUFFER_MAXSIZE * sizeof(char)) {
                fprintf(stderr, "ERROR: Was not able to copy sufficient bytes into buffer (STARCHCAT_COPY_BUFFER_MAXSIZE).\n");
                return STARCHCAT_EXIT_FAILURE;
            }
#ifdef __cplusplus
            fwrite(buffer, sizeof(char), static_cast<size_t>( STARCHCAT_COPY_BUFFER_MAXSIZE ), outFnPtr);
#else
            fwrite(buffer, sizeof(char), (size_t) STARCHCAT_COPY_BUFFER_MAXSIZE, outFnPtr);
#endif
            outFileSizeCounter -= STARCHCAT_COPY_BUFFER_MAXSIZE;
        }
        else {
#ifdef __cplusplus
            nBytesRead = fread(buffer, sizeof(char), static_cast<size_t>( outFileSizeCounter ), inRec->fp);
#else
            nBytesRead = fread(buffer, sizeof(char), (size_t) outFileSizeCounter, inRec->fp);
#endif
            if (nBytesRead != outFileSizeCounter * sizeof(char)) {
                fprintf(stderr, "ERROR: Was not able to copy sufficient bytes into buffer (outFileSizeCounter).\n");
                return STARCHCAT_EXIT_FAILURE;
            }
#ifdef __cplusplus
            fwrite(buffer, sizeof(char), static_cast<size_t>( outFileSizeCounter ), outFnPtr);
#else
            fwrite(buffer, sizeof(char), (size_t) outFileSizeCounter, outFnPtr);
#endif
            outFileSizeCounter = 0ULL;
        }
    } while (outFileSizeCounter > 0);

    /* update output metadata */
    if (! *outMd) {
#ifdef DEBUG
        fprintf(stderr, "\t\tmaking new output metadata structure...\n");
#endif
#ifdef __cplusplus
        *outMd = STARCH_createMetadata( const_cast<char *>( inChr ), 
#else
        *outMd = STARCH_createMetadata( (char *) inChr, 
#endif
                                        outFn, 
                                        outFileSize, 
                                        outFileLineCount, 
                                        outFileNonUniqueBases, 
                                        outFileUniqueBases,
                                        outDuplicateElementExists,
                                        outNestedElementExists,
                                        outSignature,
                                        outFileLineMaxStringLength);
    }
    else {
#ifdef DEBUG
        fprintf(stderr, "\t\tappending to existing output metadata structure...\n");
#endif
        *outMd = STARCH_addMetadata( *outMd, 
#ifdef __cplusplus
                                     const_cast<char *>( inChr ), 
#else
                                     (char *) inChr, 
#endif
                                     outFn, 
                                     outFileSize, 
                                     outFileLineCount, 
                                     outFileNonUniqueBases, 
                                     outFileUniqueBases,
                                     outDuplicateElementExists,
                                     outNestedElementExists,
                                     outSignature,
                                     outFileLineMaxStringLength);
    }
    /* fprintf(stderr, "\t\tchr: %s, outFn: %s, size: %llu\n", (*outMd)->chromosome, (*outMd)->filename, (*outMd)->size); */

    /* cleanup */
    fclose(outFnPtr);

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_rewriteInputRecordToOutput(Metadata **outMd, const char *outTag, const CompressionType outType, const char *inChr, const MetadataRecord *inRec, size_t *cumulativeOutputSize, const Boolean generatePerChrSignatureFlag, const Boolean reportProgressFlag, const LineCountType reportProgressN)
{
    /*
        This function extracts a single record (chromosome) of data
        to a temporary buffer. The temporary buffer is reverse-transformed
        to BED, retransformed, and then compressed with the given outbound 
        stream compression type. The records metadata are updated at the end 
        with the statistics for the new record.
    */
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT2_rewriteInputRecordToOutput() ---\n");
#endif
#ifdef __cplusplus
    char *outTagFn = nullptr;
    unsigned char *retransformLineBuf = nullptr;
    unsigned char *retransformBuf = nullptr;
#else
    char *outTagFn = NULL;
    unsigned char *retransformLineBuf = NULL;
    unsigned char *retransformBuf = NULL;
#endif
    FILE *inFp = inRec->fp;
    FILE *outFp = stdout;
    CompressionType inType = inRec->type;
    SignedCoordType startOffset = 0;
    Metadata *iter, *inMd = inRec->metadata;
    ArchiveVersion *av = inRec->av;

    /* intermediate buffer variables */
    int64_t nRetransformLineBuf = 0;
    int64_t nRetransformLineBufPos = 0;
    int64_t nRetransformBuf = 0;

    /* allocate memory for intermediate buffer variables */
#ifdef __cplusplus
    retransformLineBuf = static_cast<unsigned char *>( calloc(TOKENS_MAX_LENGTH, sizeof(unsigned char)) );
#else
    retransformLineBuf = (unsigned char *) calloc(TOKENS_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!retransformLineBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for intermediate buffer variable [retransformLineBuf].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    retransformBuf = static_cast<unsigned char *>( calloc(STARCH_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    retransformBuf = (unsigned char *) calloc(STARCH_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!retransformBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for intermediate buffer variable [retransformBuf].\n");
        exit(ENOMEM);
    }

    /* bzip2 variables */
#ifdef __cplusplus
    BZFILE *bzInFp = nullptr;
    BZFILE *bzOutFp = nullptr;
    unsigned char *bzReadBuf = nullptr;
    unsigned char *bzRemainderBuf = nullptr;
    unsigned char *bzLineBuf = nullptr;
#else
    BZFILE *bzInFp = NULL;
    BZFILE *bzOutFp = NULL;
    unsigned char *bzReadBuf = NULL;
    unsigned char *bzRemainderBuf = NULL;
    unsigned char *bzLineBuf = NULL;
#endif
    int bzInError = BZ_OK;
    int bzOutError = BZ_OK;
    size_t nBzReadBuf = STARCH_BZ_BUFFER_MAX_LENGTH;
    size_t nBzRemainderBuf = 0;
    size_t nBzRead = 0;
    size_t bzBufIndex = 0;
    unsigned int bzOutBytesConsumedLo32 = 0U;
    unsigned int bzOutBytesConsumedHi32 = 0U;
    size_t bzOutBytesWritten = 0;
    unsigned int bzOutBytesWrittenLo32 = 0U;
    unsigned int bzOutBytesWrittenHi32 = 0U;

    /* allocate memory for bzip2 variables */
#ifdef __cplusplus
    bzReadBuf = static_cast<unsigned char *>( calloc(STARCH_BZ_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    bzReadBuf = (unsigned char *) calloc(STARCH_BZ_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!bzReadBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for bzip2 variable [bzReadBuf].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    bzRemainderBuf = static_cast<unsigned char *>( calloc(STARCH_BZ_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    bzRemainderBuf = (unsigned char *) calloc(STARCH_BZ_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!bzRemainderBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for bzip2 variable [bzRemainderBuf].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    bzLineBuf = static_cast<unsigned char *>( calloc(STARCH_BZ_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    bzLineBuf = (unsigned char *) calloc(STARCH_BZ_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!bzLineBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for bzip2 variable [bzLineBuf].\n");
        exit(ENOMEM);
    }

    /* gzip variables */
#ifdef __cplusplus
    unsigned char *zOutBuffer = nullptr;
    unsigned char *zReadBuf = nullptr;
    unsigned char *zOutBuf = nullptr;
    unsigned char *zRemainderBuf = nullptr;
    unsigned char *zLineBuf = nullptr;
#else
    unsigned char *zOutBuffer = NULL;
    unsigned char *zReadBuf = NULL;
    unsigned char *zOutBuf = NULL;
    unsigned char *zRemainderBuf = NULL;
    unsigned char *zLineBuf = NULL;
#endif
    z_stream zInStream;
    z_stream zOutStream;
    int zInError = -1;
    int zOutError = -1;
    size_t zInHave = 0;
    size_t zOutHave = 0;
    size_t zBufIndex = 0;
    size_t nZRemainderBuf = 0;

    /* allocate memory for gzip variables */
#ifdef __cplusplus
    zOutBuffer = static_cast<unsigned char *>( calloc(STARCH_Z_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    zOutBuffer = (unsigned char *) calloc(STARCH_Z_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!zOutBuffer) {
        fprintf(stderr, "ERROR: Insufficient memory for gzip variable [zOutBuffer].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    zReadBuf = static_cast<unsigned char *>( calloc(STARCH_Z_CHUNK/1024, sizeof(unsigned char)) );
#else
    zReadBuf = (unsigned char *) calloc(STARCH_Z_CHUNK/1024, sizeof(unsigned char));
#endif
    if (!zReadBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for gzip variable [zReadBuf].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    zOutBuf = static_cast<unsigned char *>( calloc(STARCH_Z_CHUNK, sizeof(unsigned char)) );
#else
    zOutBuf = (unsigned char *) calloc(STARCH_Z_CHUNK, sizeof(unsigned char));
#endif
    if (!zOutBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for gzip variable [zOutBuf].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    zRemainderBuf = static_cast<unsigned char *>( calloc(STARCH_Z_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    zRemainderBuf = (unsigned char *) calloc(STARCH_Z_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!zRemainderBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for gzip variable [zRemainderBuf].\n");
        exit(ENOMEM);
    }

#ifdef __cplusplus
    zLineBuf = static_cast<unsigned char *>( calloc(STARCH_Z_BUFFER_MAX_LENGTH, sizeof(unsigned char)) );
#else
    zLineBuf = (unsigned char *) calloc(STARCH_Z_BUFFER_MAX_LENGTH, sizeof(unsigned char));
#endif
    if (!zLineBuf) {
        fprintf(stderr, "ERROR: Insufficient memory for gzip variable [zLineBuf].\n");
        exit(ENOMEM);
    }

    /* transformation variables */
#ifdef __cplusplus
    char *t_firstInputToken = nullptr;
    char *t_secondInputToken = nullptr;
    char *t_currChr = nullptr;
    char *t_currRemainder = nullptr;
#else
    char *t_firstInputToken = NULL;
    char *t_secondInputToken = NULL;
    char *t_currChr = NULL;
    char *t_currRemainder = NULL;
#endif
    size_t lastNewlineOffset = 0U;
    unsigned char bufChar = '\0';
    size_t bufCharIndex = 0;
    SignedCoordType t_start = 0;
    SignedCoordType t_pLength = 0;
    SignedCoordType t_lastEnd = 0;
    size_t t_currChrLen = 0U;
    SignedCoordType t_currStart = 0;
    SignedCoordType t_currStop = 0;
    size_t t_currRemainderLen = 0U;
    LineCountType t_lineIdx = 0;
    SignedCoordType t_pStart = 0;
    SignedCoordType t_pStop = 0;
    SignedCoordType t_previousStop = 0;
    SignedCoordType t_lastPosition = 0;
    SignedCoordType t_lcDiff = 0;
    BaseCountType t_totalNonUniqueBases = 0;
    BaseCountType t_totalUniqueBases = 0;
    Boolean t_duplicateElementExists = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean t_nestedElementExists = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
    size_t t_fileSize = 0U;
    LineLengthType t_lineMaxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;

    /* allocate memory for transformation variables */
#ifdef __cplusplus
    t_firstInputToken = static_cast<char *>( calloc(UNSTARCH_FIRST_TOKEN_MAX_LENGTH, sizeof(char)) );
#else
    t_firstInputToken = (char *) calloc(UNSTARCH_FIRST_TOKEN_MAX_LENGTH, sizeof(char));
#endif
    if (!t_firstInputToken) {
        fprintf(stderr, "ERROR: Insufficient memory for transformation variable [t_firstInputToken].\n");
        exit(ENOMEM);
    }
    
#ifdef __cplusplus
    t_secondInputToken = static_cast<char *>( calloc(UNSTARCH_SECOND_TOKEN_MAX_LENGTH, sizeof(char)) );
#else
    t_secondInputToken = (char *) calloc(UNSTARCH_SECOND_TOKEN_MAX_LENGTH, sizeof(char));
#endif
    if (!t_secondInputToken) {
        fprintf(stderr, "ERROR: Insufficient memory for transformation variable [t_secondInputToken].\n");
        exit(ENOMEM);
    }

    /* hash variables */
    struct sha1_ctx t_perChromosomeHashCtx;
    unsigned char t_sha1Digest[STARCH2_MD_FOOTER_SHA1_LENGTH] = {0};
#ifdef __cplusplus
    char *t_base64EncodedSha1Digest = nullptr;
#else
    char *t_base64EncodedSha1Digest = NULL;
#endif

    static const char tab = '\t';

#ifdef DEBUG
    /*
    fprintf(stderr, "\tsetting up tag... (%s)\n", outTag);
    */
#endif
    switch (outType) {
        case kBzip2: {
#ifdef __cplusplus
            outTagFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".bz2")) );
#else
            outTagFn = malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".bz2"));
#endif
            sprintf(outTagFn, "%s.%s.bz2", inChr, outTag);
            break;
        }
        case kGzip: {
#ifdef __cplusplus
            outTagFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".gz")) );
#else
            outTagFn = malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".gz"));
#endif
            sprintf(outTagFn, "%s.%s.gz", inChr, outTag);
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Undefined outbound compression type.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }
#ifdef DEBUG
    fprintf(stderr, "\toutTagFn: %s\n", outTagFn);
#endif

    /*
        Use fseek() to ensure we are at the correct file offset before reading bytes
    */
    if (av->major == 2)
        startOffset += STARCH2_MD_HEADER_BYTE_LENGTH;
    else if (av->major == 1)
        startOffset += inRec->mdOffset;

#ifdef __cplusplus
    for (iter = inMd; iter != nullptr; iter = iter->next) {
#else
    for (iter = inMd; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(iter->chromosome, inChr) == 0)
            break;
        else
            startOffset += iter->size;
    }

#ifdef DEBUG
    fprintf(stderr, "\tseeking to file offset: %" PRIu64 "\n", startOffset);
#endif

#ifdef __cplusplus
    fseeko(inFp, static_cast<off_t>( startOffset ), SEEK_SET);
#else
    fseeko(inFp, (off_t) startOffset, SEEK_SET);
#endif

    if (generatePerChrSignatureFlag) {
        /* set up per-chromosome hash context */
        sha1_init_ctx(&t_perChromosomeHashCtx);
    }

    /*
        Set up I/O streams 
    */

#ifdef DEBUG    
    fprintf(stderr, "\tsetting up I/O streams...\n");
#endif
    switch (outType) {
        case kBzip2: {
#ifdef DEBUG
            fprintf (stderr, "\tsetting up bzip2 write stream...\n");
#endif
            bzOutFp = BZ2_bzWriteOpen(&bzOutError, outFp, STARCH_BZ_COMPRESSION_LEVEL, STARCH_BZ_VERBOSITY, STARCH_BZ_WORKFACTOR);
            if (!bzOutFp) {
                fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            else if (bzOutError != BZ_OK) {
                switch (bzOutError) {
                    case BZ_CONFIG_ERROR: {
                        fprintf(stderr, "ERROR: Bzip2 library has been miscompiled\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_PARAM_ERROR: {
                        fprintf(stderr, "ERROR: Stream is null, or block size, verbosity and work factor parameters are invalid\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_IO_ERROR: {
                        fprintf(stderr, "ERROR: The value of ferror(outFp) is nonzero -- check outFp\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_MEM_ERROR: {
                        fprintf(stderr, "ERROR: Not enough memory is available\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    default: {
                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteOpen() (err: %d)\n", bzOutError);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                }
            }
            break;
        }
        case kGzip: {
#ifdef DEBUG
            fprintf (stderr, "\tsetting up gzip write stream...\n");
#endif
#ifdef __cplusplus
            zOutStream.zalloc = nullptr;
            zOutStream.zfree  = nullptr;
            zOutStream.opaque = nullptr;
#else
            zOutStream.zalloc = Z_NULL;
            zOutStream.zfree  = Z_NULL;
            zOutStream.opaque = Z_NULL;
#endif
            /* cf. http://www.zlib.net/manual.html for level information */
            /* zOutError = deflateInit2(&zOutStream, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY); */
            zOutError = deflateInit(&zOutStream, STARCH_Z_COMPRESSION_LEVEL);
            if (zOutError != Z_OK) {
                switch (zOutError) {
                    case Z_MEM_ERROR: {
                        fprintf(stderr, "ERROR: Not enough memory is available\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case Z_STREAM_ERROR: {
                        fprintf(stderr, "ERROR: Gzip initialization parameter is invalid (e.g., invalid method)\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case Z_VERSION_ERROR: {
                        fprintf(stderr, "ERROR: the zlib library version is incompatible with the version assumed by the caller (ZLIB_VERSION)\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    default: {
                        fprintf(stderr, "ERROR: Unknown error with deflateInit() (err: %d)\n", zOutError);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                }
            }
            break;
        }
        case kUndefined: {
            fprintf (stderr, "ERROR: Unknown output compression type specified!\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

    /* 
        Reprocess based on input compression type 
    */
    switch (inType) {
        /*
            Bzip2 stream reprocessing
        */
        case kBzip2: {
#ifdef DEBUG
            fprintf (stderr, "\tsetting up bzip2 read stream...\n");
#endif
#ifdef __cplusplus
            bzInFp = BZ2_bzReadOpen(&bzInError, inFp, STARCH_BZ_VERBOSITY, STARCH_BZ_SMALL, nullptr, 0); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzreadopen */
#else
            bzInFp = BZ2_bzReadOpen(&bzInError, inFp, STARCH_BZ_VERBOSITY, STARCH_BZ_SMALL, NULL, 0); /* http://www.bzip.org/1.0.5/bzip2-manual-1.0.5.html#bzreadopen */
#endif
            if (bzInError != BZ_OK) {
                switch (bzInError) {
                    case BZ_CONFIG_ERROR: {
                        fprintf(stderr, "ERROR: Bzip2 library was miscompiled. Contact your system administrator.\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_PARAM_ERROR: {
                        fprintf(stderr, "ERROR: Input file stream is NULL, small value is invalid, or unused parameters are invalid.\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_IO_ERROR: {
                        fprintf(stderr, "ERROR: Error reading the underlying compressed file.\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_MEM_ERROR: {
                        fprintf(stderr, "ERROR: Insufficient memory is available for bzip2 setup.\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    default:
                        break;
                }
            }

            while (bzInError == BZ_OK) {
#ifdef DEBUG
                fprintf (stderr, "\t\treading from stream... (%s)\n", inChr);
#endif
#ifdef __cplusplus
                nBzRead = static_cast<size_t>( BZ2_bzRead(&bzInError, bzInFp, bzReadBuf, static_cast<int>( nBzReadBuf )) );
#else
                nBzRead = (size_t) BZ2_bzRead(&bzInError, bzInFp, bzReadBuf, (int) nBzReadBuf);
#endif
#ifdef DEBUG
                fprintf (stderr, "\t\tbzInError: %d \t nBzRemainderBuf: %zu \t nBzRead: %zu\n", bzInError, nBzRemainderBuf, nBzRead);
#endif
                if ((bzInError == BZ_OK) || (bzInError == BZ_STREAM_END)) {
                    /* 
                        We read nBzRemainderBuf bytes from bzRemainderBuf
                        and nBzRead bytes from bzReadBuf. We then tokenize by 
                        newline and reverse transform the line token. 
                        
                        Once we have a buffer of BED data, we then 
                        transform that in order to recalculate statistics.
                        Transormed data are recompressed and sent to stdout.
                    */
                    bufCharIndex = 0;
                    if (nBzRemainderBuf > 0) {
                        memset(bzLineBuf, 0, nBzRemainderBuf + 1);
                        bzBufIndex = 0;
                        while (bzBufIndex < nBzRemainderBuf) {
                            bufChar = bzRemainderBuf[bzBufIndex];
                            bzLineBuf[bufCharIndex++] = bufChar;
#ifdef DEBUG
                            fprintf(stderr, "bufChar [%c]\n", bufChar); 
#endif
                            bzBufIndex++;
                        }
                        /* memset(bzRemainderBuf, 0, nBzRemainderBuf); */
                        bzRemainderBuf[nBzRemainderBuf] = '\0';
                        nBzRemainderBuf = 0;
                    }
                    bzBufIndex = 0;
                    while (bzBufIndex < nBzRead) {
                        bufChar = bzReadBuf[bzBufIndex];
                        bzLineBuf[bufCharIndex++] = bufChar;
#ifdef DEBUG
                        fprintf(stderr, "bufChar [%c]", bufChar);
#endif
                        /* 
                            We have extracted a line of post-transformed data, so we process it 
                            with UNSTARCH_extractRawLine(), which converts it to BED data. 
                        */
                        if (bufChar == '\n') {
                            bzLineBuf[bufCharIndex - 1] = '\0';
#ifdef __cplusplus
                            t_lineMaxStringLength = (t_lineMaxStringLength >= static_cast<LineLengthType>(bufCharIndex - 1)) ? t_lineMaxStringLength : static_cast<LineLengthType>(bufCharIndex - 1);
#else
                            t_lineMaxStringLength = (t_lineMaxStringLength >= (LineLengthType)(bufCharIndex - 1)) ? t_lineMaxStringLength : (LineLengthType)(bufCharIndex - 1);
#endif

#ifdef DEBUG
                            fprintf(stderr, "\tbzLineBuf: %s\n", bzLineBuf);
                            fprintf(stderr, "\tBEFORE UNSTARCH_extractLine() - t_currChr: %s \t t_currStart: %" PRIu64 " \t t_currStop: %" PRIu64 " \t t_currRemainder: %s\n", t_currChr, t_currStart, t_currStop, t_currRemainder);
#endif                            
                            UNSTARCH_extractRawLine( inChr, 
                                                     bzLineBuf, 
                                                     tab, 
                                                     &t_start, 
                                                     &t_pLength, 
                                                     &t_lastEnd, 
                                                     t_firstInputToken, 
                                                     t_secondInputToken, 
                                                     &t_currChr, 
                                                     &t_currChrLen, 
                                                     &t_currStart, 
                                                     &t_currStop, 
                                                     &t_currRemainder, 
                                                     &t_currRemainderLen );

#ifdef DEBUG
                            fprintf(stderr, "\tAFTER UNSTARCH_extractLine()  - t_currChr: %s \t t_currStart: %" PRIu64 " \t t_currStop: %" PRIu64 " \t t_currRemainder: %s\n", t_currChr, t_currStart, t_currStop, t_currRemainder);
                            fprintf(stderr, "\tfirst token: %s \t second token: %s\n", t_firstInputToken, t_secondInputToken);
#endif
                            /* 
                                If we aren't on a post-transform line starting with 'p', then we have a complete BED 
                                element.

                                So we build a BED element from the results, adding it to the BED buffer (if space 
                                is available). This buffer stores BED elements that will eventually be retransformed,
                                compressed and written to stdout.
            
                                Else, if the BED buffer will overflow, we instead compress the current BED buffer 
                                with our output compression type (which may be the same as or different from the 
                                input compression type) and put the BED element into a remainder-BED buffer that 
                                is primed on the next loop.
                            */

                            if (bzLineBuf[0] != 'p') {
#ifdef DEBUG
                                fprintf(stderr, "\tRaw BED data -> %s\t%" PRIu64 "\t%" PRIu64 "\t%s\n", t_currChr, t_currStart, t_currStop, t_currRemainder);
#endif
                                /* increment line counter */
                                t_lineIdx++;

                                /* reverse transform */
#ifdef __cplusplus
                                UNSTARCH_reverseTransformCoordinates( static_cast<const LineCountType>( t_lineIdx ), 
#else
                                UNSTARCH_reverseTransformCoordinates( (const LineCountType) t_lineIdx, 
#endif
                                                                      &t_lastPosition,
                                                                      &t_lcDiff,
                                                                      &t_currStart,
                                                                      &t_currStop,
                                                                      &t_currRemainder,
                                                                      retransformLineBuf,
                                                                      &nRetransformLineBuf,
                                                                      &nRetransformLineBufPos );

                                if ((reportProgressFlag == kStarchTrue) && (t_lineIdx % reportProgressN == 0)) {
                                    fprintf(stderr, "PROGRESS: Retransforming element [%" PRIu64 "] of chromosome [%s] -> [%s\t%" PRId64 "\t%" PRId64 "\t%s]\n", t_lineIdx, t_currChr, t_currChr, t_currStart, t_currStop, t_currRemainder);
                                }

                                /* adjust per-stream statistics */
                                t_lastPosition = t_currStop;
#ifdef __cplusplus
                                t_totalNonUniqueBases += static_cast<BaseCountType>( t_currStop - t_currStart );
                                if (t_previousStop <= t_currStart)
                                    t_totalUniqueBases += static_cast<BaseCountType>( t_currStop - t_currStart );
                                else if (t_previousStop < t_currStop)
                                    t_totalUniqueBases += static_cast<BaseCountType>( t_currStop - t_previousStop );
#else
                                t_totalNonUniqueBases += (BaseCountType) (t_currStop - t_currStart);
                                if (t_previousStop <= t_currStart)
                                    t_totalUniqueBases += (BaseCountType) (t_currStop - t_currStart);
                                else if (t_previousStop < t_currStop)
                                    t_totalUniqueBases += (BaseCountType) (t_currStop - t_previousStop);
#endif
                                t_previousStop = (t_currStop > t_previousStop) ? t_currStop : t_previousStop;

                                /* test for duplicate element */
                                if ((t_pStart == t_currStart) && (t_pStop == t_currStop))
                                    t_duplicateElementExists = kStarchTrue;

                                /* test for nested element */
                                if ((t_pStart < t_currStart) && (t_pStop > t_currStop))
                                    t_nestedElementExists = kStarchTrue;

                                /* set pElement values */
                                t_pStart = t_currStart;
                                t_pStop = t_currStop;

                                /* collate transformed coordinates with larger buffer */
#ifdef __cplusplus
                                if (nRetransformBuf + nRetransformLineBufPos < static_cast<int64_t>( sizeof(retransformBuf) )) {
                                    memcpy(retransformBuf + nRetransformBuf, retransformLineBuf, static_cast<size_t>( nRetransformLineBufPos ));
#else
                                if (nRetransformBuf + nRetransformLineBufPos < (int64_t) sizeof(retransformBuf)) {
                                    memcpy(retransformBuf + nRetransformBuf, retransformLineBuf, (size_t) nRetransformLineBufPos);
#endif
                                    nRetransformBuf += nRetransformLineBufPos;
                                    nRetransformLineBufPos = 0;
                                    nRetransformLineBuf = 0;
                                }
                                else {
                                    /* 
                                        Compress whatever is already in retransformBuf given the specified
                                        outbound compression type, add retransformLineBuf to the start of 
                                        retransformBuf, and set nRetransformBuf.
                                    */

                                    if (generatePerChrSignatureFlag) {
                                        /* hash the transformed buffer */
#ifdef __cplusplus
                                        sha1_process_bytes(retransformBuf, static_cast<size_t>( nRetransformBuf ), &t_perChromosomeHashCtx);
#else
                                        sha1_process_bytes(retransformBuf, (size_t) nRetransformBuf, &t_perChromosomeHashCtx);
#endif
                                    }

                                    switch (outType) {
                                        /* compress with bzip2 */
                                        case kBzip2: {
#ifdef DEBUG
                                            fprintf(stderr, "\twriting compressed retransformLineBuf data to bzOutFp:\n----\n%s\n---\n%" PRId64 " | % " PRId64 "\n---\n", retransformLineBuf, nRetransformLineBuf, nRetransformLineBufPos);
#endif
#ifdef __cplusplus
                                            BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, static_cast<int>( nRetransformBuf ));
#else
                                            BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, (int) nRetransformBuf);
#endif
                                            if (bzOutError != BZ_OK) {
                                                switch (bzOutError) {
                                                    case BZ_PARAM_ERROR: {
                                                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                    case BZ_SEQUENCE_ERROR: {
                                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                    case BZ_IO_ERROR: {
                                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                    default: {
                                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzOutError);
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                        /* compress with gzip */
                                        case kGzip: {
#ifdef __cplusplus
                                            zOutStream.next_in = reinterpret_cast<unsigned char *>( retransformBuf );
                                            zOutStream.avail_in = static_cast<unsigned int>( nRetransformBuf );
#else
                                            zOutStream.next_in = (unsigned char *) retransformBuf;
                                            zOutStream.avail_in = (unsigned int) nRetransformBuf;
#endif
                                            do {
                                                zOutStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                                                zOutStream.next_out = reinterpret_cast<unsigned char *>( zOutBuffer );
#else
                                                zOutStream.next_out = (unsigned char *) zOutBuffer;
#endif
                                                /* zOutError = deflate (&zOutStream, feof(inFp) ? Z_FINISH : Z_NO_FLUSH); */
                                                zOutError = deflate (&zOutStream, Z_NO_FLUSH);
                                                switch (zOutError) {
                                                    case Z_MEM_ERROR: {
                                                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                                        return STARCHCAT_FATAL_ERROR;
                                                    }
                                                    case Z_BUF_ERROR:
                                                    default:
                                                        break;
                                                }
#ifdef __cplusplus
                                                zOutHave = static_cast<size_t>( STARCH_Z_BUFFER_MAX_LENGTH - zOutStream.avail_out );
#else
                                                zOutHave = (size_t) (STARCH_Z_BUFFER_MAX_LENGTH - zOutStream.avail_out);
#endif
                                                t_fileSize += zOutHave;
                                                *cumulativeOutputSize += zOutHave;
                                                fwrite(zOutBuffer, 1, zOutHave, outFp);
                                                fflush(outFp);
                                            } while (zOutStream.avail_out == 0);
                                            break;
                                        }
                                        case kUndefined: {
                                            fprintf(stderr, "ERROR: Outbound compression type is unknown. Are the parameters corrupt?\n");
                                            return STARCHCAT_EXIT_FAILURE;
                                        }
                                    }
                                    /* memcpy(retransformBuf, retransformLineBuf + nRetransformLineBufPos, nRetransformLineBuf); */
#ifdef __cplusplus
                                    memcpy(retransformBuf, retransformLineBuf, static_cast<size_t>( nRetransformLineBufPos ));
#else
                                    memcpy(retransformBuf, retransformLineBuf, (size_t) nRetransformLineBufPos);
#endif
                                    nRetransformBuf = nRetransformLineBufPos;
                                    retransformBuf[nRetransformBuf] = '\0';
                                    nRetransformLineBuf = 0;
                                    nRetransformLineBufPos = 0;
#ifdef DEBUG
                                    fprintf(stderr, "POST RETRANS COMP: [%s]\n", retransformBuf);
#endif
                                }
                            }

                            /* this lets us know where we last found a newline, for handling the remainder */
                            lastNewlineOffset = bzBufIndex;
#ifdef DEBUG
                            fprintf(stderr, "\tLAST NEWLINE OFFSET -> %lu\n", lastNewlineOffset);
#endif

                            /* reset parameters */
                            t_firstInputToken[0] = '\0';
                            t_secondInputToken[0] = '\0';
                            bufCharIndex = 0;
                        }

                        /* 
                            If we're at the end of the extracted buffer and there is no
                            newline, then we need to put that remainder at the front of the
                            remainder buffer, which can then be copied back to the line
                            buffer before any further parsing of the extraction buffer is
                            done.
                        */
                        else if ((bzBufIndex + 1) == nBzRead) {
                            /*
                                Note that there may be data left over in bzReadBuf
                                that remains after a final newline. This needs to be
                                processed on the next loop through the BZ2_bzRead()
                                call.
                            */
                            memset(bzRemainderBuf, 0, nBzRemainderBuf);
#ifdef __cplusplus
                            nBzRemainderBuf = static_cast<size_t>( nBzRead - (lastNewlineOffset + 1) );
#else
                            nBzRemainderBuf = (size_t) nBzRead - (lastNewlineOffset + 1);
#endif
                            memcpy(bzRemainderBuf, bzReadBuf + lastNewlineOffset + 1, nBzRemainderBuf);
#ifdef DEBUG
                            bzRemainderBuf[nBzRemainderBuf] = '\0';
                            fprintf(stderr, "\t\tPOST-LOOP bzReadBuf: [%s]\n", bzReadBuf + lastNewlineOffset);
                            fprintf(stderr, "\t\tPOST-LOOP bzRemainderBuf: [%s]\n", bzRemainderBuf);
#endif                            
                        }
                        bzBufIndex++;
                    }
                }
            }

            if (generatePerChrSignatureFlag) {
                /* hash the transformed buffer */
#ifdef __cplusplus
                sha1_process_bytes(retransformBuf, static_cast<size_t>( nRetransformBuf ), &t_perChromosomeHashCtx);
#else
                sha1_process_bytes(retransformBuf, (size_t) nRetransformBuf, &t_perChromosomeHashCtx);
#endif
            }

            /* compress whatever is left in the retransform buffer */
            switch (outType) {
                case kBzip2: {
#ifdef DEBUG
                    retransformBuf[nRetransformBuf] = '\0';
                    fprintf(stderr, "\t(whatever's left) writing compressed retransformBuf data to bzOutFp:\n----\n%s\n---\n%" PRId64 "\n---\n", retransformBuf, nRetransformBuf);
#endif
#ifdef __cplusplus
                    BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, static_cast<int>( nRetransformBuf ));
#else
                    BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, (int) nRetransformBuf);
#endif
                    if (bzOutError != BZ_OK) {
                        switch (bzOutError) {
                            case BZ_PARAM_ERROR: {
                                fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            case BZ_SEQUENCE_ERROR: {
                                fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            case BZ_IO_ERROR: {
                                fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            default: {
                                fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzOutError);
                                return STARCHCAT_EXIT_FAILURE;
                            }
                        }
                    }
                    break;
                }
                case kGzip: {
#ifdef DEBUG
                    retransformBuf[nRetransformBuf] = '\0';
                    fprintf(stderr, "\t(whatever's left) writing compressed retransformBuf data to zOutFp:\n----\n%s\n---\n%" PRId64 "\n---\n", retransformBuf, nRetransformBuf);
#endif
#ifdef __cplusplus
                    zOutStream.next_in = reinterpret_cast<unsigned char *>( retransformBuf );
                    zOutStream.avail_in = static_cast<unsigned int>( nRetransformBuf );
#else
                    zOutStream.next_in = (unsigned char *) retransformBuf;
                    zOutStream.avail_in = (unsigned int) nRetransformBuf;
#endif

                    do {
#ifdef DEBUG
                        fprintf(stderr, "START zOutStream.avail_out -> %d\n", zOutStream.avail_out);
#endif
                        zOutStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                        zOutStream.next_out = reinterpret_cast<unsigned char *>( zOutBuffer );
#else
                        zOutStream.next_out = (unsigned char *) zOutBuffer;
#endif

                        zOutError = deflate (&zOutStream, Z_FINISH);

#ifdef DEBUG
                        fprintf(stderr, "zOutError -> %d\n", zOutError);
#endif

                        switch (zOutError) {
                            case Z_MEM_ERROR: {
                                fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                return STARCHCAT_FATAL_ERROR;
                            }
                            case Z_BUF_ERROR:
                            default:
                                break;
                        }
                        zOutHave = STARCH_Z_BUFFER_MAX_LENGTH - zOutStream.avail_out;
#ifdef DEBUG
                        fprintf(stderr, "zOutHave -> %lu\n", zOutHave);
#endif
                        t_fileSize += zOutHave;
                        *cumulativeOutputSize += zOutHave;
                        fwrite(zOutBuffer, 1, zOutHave, outFp);
                        fflush(outFp);
                    } while (zOutStream.avail_out == 0);

                    break;
                }
                case kUndefined: {
                    fprintf(stderr, "ERROR: Outbound compression type is unknown. Are the parameters corrupt?\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
            }

            /* clean up bzip2 read stream */
#ifdef DEBUG
            fprintf(stderr, " -> cleaning up bzip2 read stream...\n");
#endif
            if (bzInError != BZ_STREAM_END) {
                fprintf(stderr, "ERROR: Bzip2 error after read: %d\n", bzInError);
                return STARCHCAT_EXIT_FAILURE;
            }
            BZ2_bzReadClose(&bzInError, bzInFp);
            if (bzInError != BZ_OK) {
                fprintf(stderr, "ERROR: Bzip2 error after closing read stream: %d\n", bzInError);
                return STARCHCAT_EXIT_FAILURE;
            }
            break;
        }

        /*
            -------------------------------------------------------------------------------------------------------
            Gzip-stream reprocessing
            -------------------------------------------------------------------------------------------------------
        */
        case kGzip: {
#ifdef DEBUG
            fprintf (stderr, "\tsetting up gzip read stream...\n");
#endif
#ifdef __cplusplus
            zInStream.zalloc = nullptr;
            zInStream.zfree = nullptr;
            zInStream.opaque = nullptr;
            zInStream.next_in = nullptr;
#else
            zInStream.zalloc = Z_NULL;
            zInStream.zfree = Z_NULL;
            zInStream.opaque = Z_NULL;
            zInStream.next_in = Z_NULL;
#endif
            zInStream.avail_in = 0;
            /* cf. http://www.zlib.net/manual.html for level information */
            zInError = inflateInit2(&zInStream, (15+32));
            if (zInError != Z_OK) {
                fprintf(stderr, "ERROR: Could not initialize z-stream\n");
                return STARCHCAT_EXIT_FAILURE;
            }

            do {
#ifdef DEBUG
                fprintf (stderr, "\treading from stream... (%s)\n", inChr);
#endif
#ifdef __cplusplus
                zInStream.avail_in = static_cast<unsigned int>( fread(zReadBuf, 1, STARCH_Z_CHUNK/1024, inFp) );
#else
                zInStream.avail_in = (unsigned int) fread(zReadBuf, 1, STARCH_Z_CHUNK/1024, inFp);
#endif
#ifdef DEBUG
                fprintf (stderr, "\tzInStream.avail_in -> %d\n", zInStream.avail_in);
#endif
                if (zInStream.avail_in == 0)
                    break;
                zInStream.next_in = zReadBuf;
                do {
                    zInStream.avail_out = STARCH_Z_CHUNK;
                    zInStream.next_out = zOutBuf;
                    zOutError = inflate(&zInStream, Z_NO_FLUSH);
                    if (zOutError != Z_OK) {
                        switch (zOutError) {
                            case Z_STREAM_ERROR: {
                                fprintf(stderr, "ERROR: Z-stream clobbered.\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            case Z_NEED_DICT: {
                                zOutError = Z_DATA_ERROR;
                                inflateEnd(&zInStream);
                                break;
                            }
                            case Z_DATA_ERROR: {
                                inflateEnd(&zInStream);
                                break;
                            }
                            case Z_MEM_ERROR: {
                                inflateEnd(&zInStream);
                                break;
                            }
                        }
                    }
#ifdef __cplusplus
                    zInHave = static_cast<size_t>( STARCH_Z_CHUNK - zInStream.avail_out );
#else
                    zInHave = (size_t) (STARCH_Z_CHUNK - zInStream.avail_out);
#endif

#ifdef DEBUG
                    fprintf (stderr, "\tzOutBuf -> [");
                    fwrite (zOutBuf, 1, zInHave, stderr);
                    fprintf (stderr, "]\n");
#endif

                    /*
                        Read through zInHave bytes of zOutBuf, splitting
                        on a newline character. We reverse transform the line
                        buffer as is done for bzip2 streams.
                    */
                    bufCharIndex = 0;

                    if (nZRemainderBuf > 0) {
#ifdef DEBUG
                        fprintf(stderr, "\tADDING FROM REMAINDER [%zd] [%s]\n", nZRemainderBuf, zRemainderBuf);
#endif
#ifdef __cplusplus
                        memset(reinterpret_cast<char *>( zLineBuf ), 0, nZRemainderBuf + 1);
#else
                        memset((char *) zLineBuf, 0, nZRemainderBuf + 1);
#endif
                        zBufIndex = 0;
                        while (zBufIndex < nZRemainderBuf) {
                            bufChar = zRemainderBuf[zBufIndex];
                            zLineBuf[bufCharIndex++] = bufChar;
                            zBufIndex++;
                        }
                        memset(zRemainderBuf, 0, nZRemainderBuf + 1);
                        /* zRemainderBuf[nZRemainderBuf] = '\0'; */
                        nZRemainderBuf = 0;
                    }

                    zBufIndex = 0;
                    while (zBufIndex < zInHave) {

                        bufChar = zOutBuf[zBufIndex];
                        zLineBuf[bufCharIndex++] = bufChar;

                        if (bufChar == '\n') {
                            zLineBuf[bufCharIndex - 1] = '\0';
#ifdef __cplusplus
                            t_lineMaxStringLength = (t_lineMaxStringLength >= static_cast<LineLengthType>(bufCharIndex - 1)) ? t_lineMaxStringLength : static_cast<LineLengthType>(bufCharIndex - 1);
#else
                            t_lineMaxStringLength = (t_lineMaxStringLength >= (LineLengthType)(bufCharIndex - 1)) ? t_lineMaxStringLength : (LineLengthType)(bufCharIndex - 1);
#endif
                            /* extract a line of transformed data */
                            UNSTARCH_extractRawLine( inChr, 
                                                     zLineBuf, 
                                                     tab, 
                                                     &t_start, 
                                                     &t_pLength, 
                                                     &t_lastEnd, 
                                                     t_firstInputToken, 
                                                     t_secondInputToken, 
                                                     &t_currChr, 
                                                     &t_currChrLen, 
                                                     &t_currStart, 
                                                     &t_currStop, 
                                                     &t_currRemainder, 
                                                     &t_currRemainderLen );
                            if (zLineBuf[0] != 'p') {
                                /* increment line counter */
                                t_lineIdx++;

                                /* reverse transform */
#ifdef __cplusplus
                                UNSTARCH_reverseTransformCoordinates( static_cast<const LineCountType>( t_lineIdx ), 
#else
                                UNSTARCH_reverseTransformCoordinates( (const LineCountType) t_lineIdx, 
#endif
                                                                      &t_lastPosition,
                                                                      &t_lcDiff,
                                                                      &t_currStart,
                                                                      &t_currStop,
                                                                      &t_currRemainder,
                                                                      retransformLineBuf,
                                                                      &nRetransformLineBuf,
                                                                      &nRetransformLineBufPos );

                                if ((reportProgressFlag == kStarchTrue) && (t_lineIdx % reportProgressN == 0)) {
                                    fprintf(stderr, "PROGRESS: Retransforming element [%" PRIu64 "] of chromosome [%s] -> [%s\t%" PRId64 "\t%" PRId64 "\t%s]\n", t_lineIdx, t_currChr, t_currChr, t_currStart, t_currStop, t_currRemainder);
                                }

                                /* adjust statistics */
                                t_lastPosition = t_currStop;
#ifdef __cplusplus
                                t_totalNonUniqueBases += static_cast<BaseCountType>( t_currStop - t_currStart );
                                if (t_previousStop <= t_currStart)
                                    t_totalUniqueBases += static_cast<BaseCountType>( t_currStop - t_currStart );
                                else if (t_previousStop < t_currStop)
                                    t_totalUniqueBases += static_cast<BaseCountType>( t_currStop - t_previousStop );
#else
                                t_totalNonUniqueBases += (BaseCountType) (t_currStop - t_currStart);
                                if (t_previousStop <= t_currStart)
                                    t_totalUniqueBases += (BaseCountType) (t_currStop - t_currStart);
                                else if (t_previousStop < t_currStop)
                                    t_totalUniqueBases += (BaseCountType) (t_currStop - t_previousStop);
#endif
                                t_previousStop = (t_currStop > t_previousStop) ? t_currStop : t_previousStop;

                                /* test for duplicate element */
                                if ((t_pStart == t_currStart) && (t_pStop == t_currStop))
                                    t_duplicateElementExists = kStarchTrue;

                                /* test for nested element */
                                if ((t_pStart < t_currStart) && (t_pStop > t_currStop))
                                    t_nestedElementExists = kStarchTrue;

                                /* set pElement values */
                                t_pStart = t_currStart;
                                t_pStop = t_currStop;

                                /* shuffle data into retransformation buffer if we're not yet ready to compress */
#ifdef __cplusplus
                                if (nRetransformBuf + nRetransformLineBufPos < static_cast<int64_t>( sizeof(retransformBuf) )) {
                                    memcpy(retransformBuf + nRetransformBuf, retransformLineBuf, static_cast<size_t>( nRetransformLineBufPos ));
#else
                                if (nRetransformBuf + nRetransformLineBufPos < (int64_t) sizeof(retransformBuf)) {
                                    memcpy(retransformBuf + nRetransformBuf, retransformLineBuf, (size_t) nRetransformLineBufPos);
#endif
                                    nRetransformBuf += nRetransformLineBufPos;
                                    retransformLineBuf[nRetransformLineBufPos] = '\0';
                                    nRetransformLineBufPos = 0;
                                    nRetransformLineBuf = 0;
                                }
                                else {

                                    if (generatePerChrSignatureFlag) {
                                        /* hash the transformed buffer */
#ifdef __cplusplus
                                        sha1_process_bytes(retransformBuf, static_cast<size_t>( nRetransformBuf ), &t_perChromosomeHashCtx);
#else
                                        sha1_process_bytes(retransformBuf, (size_t) nRetransformBuf, &t_perChromosomeHashCtx);
#endif
                                    }

                                    /* 
                                        Compress whatever is already in retransformBuf given the specified
                                        outbound compression type, add retransformLineBuf to the start of 
                                        retransformBuf, and set nRetransformBuf.
                                    */
                                    switch (outType) {
                                        /* compress with bzip2 */
                                        case kBzip2: {
#ifdef __cplusplus
                                            BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, static_cast<int>( nRetransformBuf ));
#else
                                            BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, (int) nRetransformBuf);
#endif
                                            if (bzOutError != BZ_OK) {
                                                switch (bzOutError) {
                                                    case BZ_PARAM_ERROR: {
                                                        fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                    case BZ_SEQUENCE_ERROR: {
                                                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                    case BZ_IO_ERROR: {
                                                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                    default: {
                                                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzOutError);
                                                        return STARCHCAT_EXIT_FAILURE;
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                        /* compress with gzip */
                                        case kGzip: {
#ifdef __cplusplus
                                            zOutStream.next_in = reinterpret_cast<unsigned char *>( retransformBuf );
                                            zOutStream.avail_in = static_cast<unsigned int>( nRetransformBuf );
#else
                                            zOutStream.next_in = (unsigned char *) retransformBuf;
                                            zOutStream.avail_in = (unsigned int) nRetransformBuf;
#endif
                                            do {
                                                zOutStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                                                zOutStream.next_out = reinterpret_cast<unsigned char *>( zOutBuffer );
#else
                                                zOutStream.next_out = (unsigned char *) zOutBuffer;
#endif
                                                /* zOutError = deflate (&zOutStream, feof(inFp) ? Z_FINISH : Z_NO_FLUSH); */
                                                zOutError = deflate (&zOutStream, Z_NO_FLUSH);
                                                switch (zOutError) {
                                                    case Z_MEM_ERROR: {
                                                        fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                                        return STARCHCAT_FATAL_ERROR;
                                                    }
                                                    case Z_BUF_ERROR:
                                                    default:
                                                        break;
                                                }
                                                zOutHave = STARCH_Z_BUFFER_MAX_LENGTH - zOutStream.avail_out;
                                                t_fileSize += zOutHave;
                                                *cumulativeOutputSize += zOutHave;
                                                fwrite(zOutBuffer, 1, zOutHave, outFp);
                                                fflush(outFp);
                                            } while (zOutStream.avail_out == 0);
                                            break;
                                        }
                                        case kUndefined: {
                                            fprintf(stderr, "ERROR: Outbound compression type is unknown. Are the parameters corrupt?\n");
                                            return STARCHCAT_EXIT_FAILURE;
                                        }
                                    }
#ifdef __cplusplus
                                    memcpy(retransformBuf, retransformLineBuf, static_cast<size_t>( nRetransformLineBufPos ));
#else
                                    memcpy(retransformBuf, retransformLineBuf, (size_t) nRetransformLineBufPos);
#endif
                                    nRetransformBuf = nRetransformLineBufPos;
                                    retransformBuf[nRetransformBuf] = '\0';
                                    nRetransformLineBuf = 0;
                                    nRetransformLineBufPos = 0;
                                }
                            }

                            lastNewlineOffset = zBufIndex;

                            t_firstInputToken[0] = '\0';
                            t_secondInputToken[0] = '\0';
                            bufCharIndex = 0;

#ifdef DEBUG
                            if ((zBufIndex + 1) == zInHave) {
                                fprintf(stderr, "\tNEWLINE FOUND AT END OF Z-STREAM BUFFER! zLineBuf: [%s]\n", zLineBuf);
                            }
#endif
                        }

                        else if ((zBufIndex + 1) == zInHave) {
                            /* add to remainder if we're at the end of zInHave */
                            memset(zRemainderBuf, 0, nZRemainderBuf);
#ifdef __cplusplus
                            nZRemainderBuf = static_cast<size_t>( zInHave ) - lastNewlineOffset - 1;
#else
                            nZRemainderBuf = (size_t) zInHave - lastNewlineOffset - 1;
#endif
                            /* ugly hack until I rewrite this from the ground up -- my sincere apologies, seriously */
                            if (*(zOutBuf + lastNewlineOffset) == '\n')
                                memcpy(zRemainderBuf, zOutBuf + lastNewlineOffset + 1, nZRemainderBuf);
                            else
                                memcpy(zRemainderBuf, zOutBuf + lastNewlineOffset, nZRemainderBuf);
                            zRemainderBuf[nZRemainderBuf] = '\0';
                        }
                        zBufIndex++;
                    }
                } while (zInStream.avail_out == 0);
            } while (zInError != Z_STREAM_END);

            if (generatePerChrSignatureFlag) {
                /* hash the transformed buffer */
#ifdef __cplusplus
                sha1_process_bytes(retransformBuf, static_cast<size_t>( nRetransformBuf ), &t_perChromosomeHashCtx);
#else
                sha1_process_bytes(retransformBuf, (size_t) nRetransformBuf, &t_perChromosomeHashCtx);
#endif
            }
  
            /* process any remainder */
            switch (outType) {
                case kBzip2: {
#ifdef __cplusplus
                    BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, static_cast<int>( nRetransformBuf ));
#else
                    BZ2_bzWrite(&bzOutError, bzOutFp, retransformBuf, (int) nRetransformBuf);
#endif
                    if (bzOutError != BZ_OK) {
                        switch (bzOutError) {
                            case BZ_PARAM_ERROR: {
                                fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or currentTransformedBufferLength is negative\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            case BZ_SEQUENCE_ERROR: {
                                fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            case BZ_IO_ERROR: {
                                fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                                return STARCHCAT_EXIT_FAILURE;
                            }
                            default: {
                                fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzOutError);
                                return STARCHCAT_EXIT_FAILURE;
                            }
                        }
                    }
                    break;
                }
                case kGzip: {
#ifdef __cplusplus
                    zOutStream.next_in = reinterpret_cast<unsigned char *>( retransformBuf );
                    zOutStream.avail_in = static_cast<unsigned int>( nRetransformBuf );
#else
                    zOutStream.next_in = (unsigned char *) retransformBuf;
                    zOutStream.avail_in = (unsigned int) nRetransformBuf;
#endif
                    do {
                        zOutStream.avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
#ifdef __cplusplus
                        zOutStream.next_out = reinterpret_cast<unsigned char *>( zOutBuffer );
#else
                        zOutStream.next_out = (unsigned char *) zOutBuffer;
#endif
                        zOutError = deflate (&zOutStream, Z_FINISH);
                        switch (zOutError) {
                            case Z_MEM_ERROR: {
                                fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                                return STARCHCAT_FATAL_ERROR;
                            }
                            case Z_BUF_ERROR:
                            default:
                                break;
                        }
                        zOutHave = STARCH_Z_BUFFER_MAX_LENGTH - zOutStream.avail_out;
                        t_fileSize += zOutHave;
                        *cumulativeOutputSize += zOutHave;
                        fwrite(zOutBuffer, 1, zOutHave, outFp);
                        fflush(outFp);
                    } while (zOutStream.avail_out == 0);
                    break;
                }
                case kUndefined: {
                    break;
                }
            }

            /* cleanup */
            inflateEnd(&zInStream);

            break;
        }

        /*
            Unknown compression type (error)
        */
        case kUndefined: {
            fprintf(stderr, "ERROR: Unknown compression type in stream (is the archive or metadata corrupt?)\n");
            break;
        }
    }

    if (generatePerChrSignatureFlag) {
        sha1_finish_ctx(&t_perChromosomeHashCtx, t_sha1Digest);
#ifdef DEBUG
        fprintf(stderr, "STARCH_encodeBase64()\n");
#endif
#ifdef __cplusplus
        STARCH_encodeBase64(&t_base64EncodedSha1Digest, 
                            static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                            reinterpret_cast<const unsigned char *>( t_sha1Digest ), 
                            static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
        STARCH_encodeBase64(&t_base64EncodedSha1Digest, 
                            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                            (const unsigned char *) t_sha1Digest, 
                            (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
#ifdef DEBUG
       fprintf(stderr, "\nPROGRESS: SHA-1 digest for chr [%s] is [%s]\n", inChr, t_base64EncodedSha1Digest);
#endif
    }

    /* clean up outbound compression stream */
    switch (outType) {
        case kBzip2: {
            BZ2_bzWriteClose64(&bzOutError, bzOutFp, STARCH_BZ_ABANDON, &bzOutBytesConsumedLo32, &bzOutBytesConsumedHi32, &bzOutBytesWrittenLo32, &bzOutBytesWrittenHi32);
            if (bzOutError != BZ_OK) {
                fprintf(stderr, "ERROR: Bzip2 error after closing write stream: %d\n", bzOutError);
                switch (bzOutError) {
                    case BZ_SEQUENCE_ERROR: {
                        fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case BZ_IO_ERROR: {
                        fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    default: {
                        fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteClose64() (err: %d)\n", bzOutError);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                }
            }
#ifdef __cplusplus
            bzOutBytesWritten = static_cast<size_t>( bzOutBytesWrittenHi32 ) << 32 | bzOutBytesWrittenLo32;
#else
            bzOutBytesWritten = (size_t) bzOutBytesWrittenHi32 << 32 | bzOutBytesWrittenLo32;
#endif
            t_fileSize += bzOutBytesWritten;
            *cumulativeOutputSize += bzOutBytesWritten;
            bzOutBytesWritten = 0;
            bzOutBytesWrittenLo32 = 0U;
            bzOutBytesWrittenHi32 = 0U;
            break;
        }
        case kGzip: {
            zOutError = deflateEnd(&zOutStream);
            if (zOutError != Z_OK) {
                switch (zOutError) {
                    case Z_STREAM_ERROR: {
                        fprintf(stderr, "ERROR: Z-stream state is inconsistent\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    case Z_DATA_ERROR: {
                        fprintf(stderr, "ERROR: Stream was freed prematurely\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    default: {
                        fprintf(stderr, "ERROR: Unknown error with deflateEnd() (err: %d)\n", zOutError);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                }
            }
#ifdef __cplusplus
            zOutStream.zalloc = nullptr;
            zOutStream.zfree  = nullptr;
            zOutStream.opaque = nullptr;
#else
            zOutStream.zalloc = Z_NULL;
            zOutStream.zfree  = Z_NULL;
            zOutStream.opaque = Z_NULL;
#endif
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Undefined compression stream type specified. You shouldn't see this unless there is a catastrophic failure in recompression.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

    /* update the metadata */
    if (!*outMd) {
#ifdef __cplusplus
        *outMd = STARCH_createMetadata( const_cast<char *>( inChr ), 
                                        outTagFn, 
                                        t_fileSize, 
                                        t_lineIdx, 
                                        t_totalNonUniqueBases, 
                                        t_totalUniqueBases,
                                        t_duplicateElementExists,
                                        t_nestedElementExists,
                                        t_base64EncodedSha1Digest,
                                        t_lineMaxStringLength );

#else
        *outMd = STARCH_createMetadata( (char *) inChr, 
                                        outTagFn, 
                                        t_fileSize, 
                                        t_lineIdx, 
                                        t_totalNonUniqueBases, 
                                        t_totalUniqueBases,
                                        t_duplicateElementExists,
                                        t_nestedElementExists,
                                        t_base64EncodedSha1Digest,
                                        t_lineMaxStringLength );
#endif
    }
    else {
#ifdef DEBUG
        fprintf(stderr, "STARCH_addMetadata()\n");
#endif
        *outMd = STARCH_addMetadata( *outMd, 
#ifdef __cplusplus
                                     const_cast<char *>( inChr ),
                                     outTagFn, 
                                     t_fileSize, 
                                     t_lineIdx,
                                     t_totalNonUniqueBases, 
                                     t_totalUniqueBases,
                                     t_duplicateElementExists,
                                     t_nestedElementExists,
                                     t_base64EncodedSha1Digest,
                                     t_lineMaxStringLength );

#else
                                     (char *) inChr, 
                                     outTagFn, 
                                     t_fileSize, 
                                     t_lineIdx,
                                     t_totalNonUniqueBases, 
                                     t_totalUniqueBases,
                                     t_duplicateElementExists,
                                     t_nestedElementExists,
                                     t_base64EncodedSha1Digest,
                                     t_lineMaxStringLength );
#endif
    }
                                        
    /* cleanup */
    if (outTagFn) {
#ifdef DEBUG
        fprintf(stderr, "outTagFn freed...\n");
#endif
        free(outTagFn);
#ifdef __cplusplus
        outTagFn = nullptr;
#else
        outTagFn = NULL;
#endif
    }
    if (t_base64EncodedSha1Digest) {
#ifdef DEBUG
        fprintf(stderr, "t_base64EncodedSha1Digest freed...\n");
#endif
        free(t_base64EncodedSha1Digest);
#ifdef __cplusplus
        t_base64EncodedSha1Digest = nullptr;
#else
        t_base64EncodedSha1Digest = NULL;
#endif
    }
    if (retransformLineBuf) {
#ifdef DEBUG
        fprintf(stderr, "retransformLineBuf freed...\n");
#endif
        free(retransformLineBuf);
#ifdef __cplusplus
        retransformLineBuf = nullptr;
#else
        retransformLineBuf = NULL;
#endif
    }
    if (retransformBuf) {
#ifdef DEBUG
        fprintf(stderr, "retransformBuf freed...\n");
#endif
        free(retransformBuf);
#ifdef __cplusplus
        retransformBuf = nullptr;
#else
        retransformBuf = NULL;
#endif
    }
    if (bzReadBuf) {
#ifdef DEBUG
        fprintf(stderr, "bzReadBuf freed...\n");
#endif
        free(bzReadBuf);
#ifdef __cplusplus
        bzReadBuf = nullptr;
#else
        bzReadBuf = NULL;
#endif
    }
    if (bzRemainderBuf) {
#ifdef DEBUG
        fprintf(stderr, "bzRemainderBuf freed...\n");
#endif
        free(bzRemainderBuf);
#ifdef __cplusplus
        bzRemainderBuf = nullptr;
#else
        bzRemainderBuf = NULL;
#endif
    }
    if (bzLineBuf) {
#ifdef DEBUG
        fprintf(stderr, "bzLineBuf freed...\n");
#endif
        free(bzLineBuf);
#ifdef __cplusplus
        bzLineBuf = nullptr;
#else
        bzLineBuf = NULL;
#endif
    }
    if (zOutBuffer) {
#ifdef DEBUG
        fprintf(stderr, "zOutBuffer freed...\n");
#endif
        free(zOutBuffer);
#ifdef __cplusplus
        zOutBuffer = nullptr;
#else
        zOutBuffer = NULL;
#endif
    }
    if (zReadBuf) {
#ifdef DEBUG
        fprintf(stderr, "zReadBuf freed...\n");
#endif
        free(zReadBuf);
#ifdef __cplusplus
        zReadBuf = nullptr;
#else
        zReadBuf = NULL;
#endif
    }
    if (zOutBuf) {
#ifdef DEBUG
        fprintf(stderr, "zOutBuf freed...\n");
#endif
        free(zOutBuf);
#ifdef __cplusplus
        zOutBuf = nullptr;
#else
        zOutBuf = NULL;
#endif
    }
    if (zRemainderBuf) {
#ifdef DEBUG
        fprintf(stderr, "zRemainderBuf freed...\n");
#endif
        free(zRemainderBuf);
#ifdef __cplusplus
        zRemainderBuf = nullptr;
#else
        zRemainderBuf = NULL;
#endif
    }
    if (zLineBuf) {
#ifdef DEBUG
        fprintf(stderr, "zLineBuf freed...\n");
#endif
        free(zLineBuf);
#ifdef __cplusplus
        zLineBuf = nullptr;
#else
        zLineBuf = NULL;
#endif
    }
    if (t_firstInputToken) {
#ifdef DEBUG
        fprintf(stderr, "t_firstInputToken freed...\n");
#endif
        free(t_firstInputToken);
#ifdef __cplusplus
        t_firstInputToken = nullptr;
#else
        t_firstInputToken = NULL;
#endif
    }
    if (t_secondInputToken) {
#ifdef DEBUG
        fprintf(stderr, "t_secondInputToken freed...\n");
#endif
        free(t_secondInputToken);
#ifdef __cplusplus
        t_secondInputToken = nullptr;
#else
        t_secondInputToken = NULL;
#endif
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_rewriteInputRecordToOutput(Metadata **outMd, const char *outTag, const CompressionType outType, const char *inChr, const MetadataRecord *inRec)
{
    /*
        This function extracts a single record (chromosome) of
        data to a temporary file. The temporary file is compressed
        with the new output type and added to the output metadata.
    */

#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_rewriteInputRecordToOutput() ---\n");
#endif
#ifdef __cplusplus
    char *uncomprOutFn = nullptr;
    char *comprOutFn = nullptr;
    FILE *uncomprOutFnPtr = nullptr;
    FILE *transformFnPtr = nullptr;
    char *note = nullptr;
#else
    char *uncomprOutFn = NULL;
    char *comprOutFn = NULL;
    FILE *uncomprOutFnPtr = NULL;
    FILE *transformFnPtr = NULL;
    char *note = NULL;
#endif
    FILE *inFp = inRec->fp;
    Metadata *inMd = inRec->metadata;
    CompressionType inType = inRec->type;

    /* extract input record to uncompressed file */
#ifdef __cplusplus
    uncomprOutFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 16) );
#else
    uncomprOutFn = malloc(strlen(inChr) + strlen(outTag) + 16);
#endif
    if (!uncomprOutFn) {
        fprintf(stderr, "ERROR: Could not allocate space for uncompressed output filename in input rewrite routine.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    sprintf(uncomprOutFn, "%s.%s.uncompressed", inChr, outTag);
#ifdef DEBUG
    fprintf(stderr, "\t\t\t%s\n", uncomprOutFn);
#endif
    uncomprOutFnPtr = STARCH_fopen(uncomprOutFn, "wb");
    if (!uncomprOutFnPtr) {
        fprintf(stderr, "ERROR: Could not open an uncompressed output file handle to %s\n", uncomprOutFn);
        return STARCHCAT_EXIT_FAILURE;
    }

    switch (inType) {
        case kBzip2: {
#ifdef __cplusplus
            if ( UNSTARCH_extractDataWithBzip2( &inFp, 
                        uncomprOutFnPtr, 
                        reinterpret_cast<const char *>( inChr ), 
                        reinterpret_cast<const Metadata *>( inMd ), 
                        static_cast<const uint64_t>( inRec->mdOffset ), 
                        static_cast<const Boolean>( inRec->hFlag )) != 0 ) {
#else
            if ( UNSTARCH_extractDataWithBzip2( &inFp, 
                        uncomprOutFnPtr, 
                        (const char *) inChr, 
                        (const Metadata *) inMd, 
                        (const uint64_t) inRec->mdOffset, 
                        (const Boolean) inRec->hFlag) != 0 ) {
#endif
                fprintf(stderr, "ERROR: Could not extract to uncompressed output file (bzip2).\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            break;
        }
        case kGzip: {
#ifdef __cplusplus
            if ( UNSTARCH_extractDataWithGzip( &inFp, 
                           uncomprOutFnPtr, 
                           reinterpret_cast<const char *>( inChr ), 
                           reinterpret_cast<const Metadata *>( inMd ), 
                           static_cast<const uint64_t>( inRec->mdOffset ), 
                           static_cast<const Boolean>( inRec->hFlag )) != 0 ) {
#else
            if ( UNSTARCH_extractDataWithGzip( &inFp, 
                           uncomprOutFnPtr, 
                           (const char *) inChr, 
                           (const Metadata *) inMd, 
                           (const uint64_t) inRec->mdOffset, 
                           (const Boolean) inRec->hFlag) != 0 ) {
#endif
                fprintf(stderr, "ERROR: Could not extract to uncompressed output file (gzip).\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Cannot extract to uncompressed output file with unsupported compression type.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

    /* cleanup */
    fclose(uncomprOutFnPtr);
#ifdef __cplusplus
    uncomprOutFnPtr = nullptr;
#else
    uncomprOutFnPtr = NULL;
#endif

    /* transform and recompress extracted, headerless data with new output type */
    transformFnPtr = STARCH_fopen(uncomprOutFn, "rb");
    if (!transformFnPtr) {
        fprintf(stderr, "ERROR: Could not open a transformation output file handle to %s\n", uncomprOutFn);
        return STARCHCAT_EXIT_FAILURE;
    }
#ifdef __cplusplus
    if ( STARCH_transformHeaderlessInput( &(*outMd), 
                      reinterpret_cast<const FILE *>( transformFnPtr ), 
                      static_cast<const CompressionType>( outType ), 
                      reinterpret_cast<const char *>( outTag ), 
                      static_cast<const Boolean>( kStarchFinalizeTransformFalse ), 
                      reinterpret_cast<const char *>( note ) ) != 0 ) {
        fprintf(stderr, "ERROR: Could not transform output file\n");
        return STARCHCAT_EXIT_FAILURE;
    }
#else
    if ( STARCH_transformHeaderlessInput( &(*outMd), 
                      (const FILE *) transformFnPtr, 
                      (const CompressionType) outType, 
                      (const char *) outTag, 
                      (const Boolean) kStarchFinalizeTransformFalse, 
                      (const char *) note ) != 0 ) {
        fprintf(stderr, "ERROR: Could not transform output file\n");
        return STARCHCAT_EXIT_FAILURE;
    }
#endif

    /* cleanup */
    fclose(transformFnPtr);
#ifdef __cplusplus
    transformFnPtr = nullptr;
#else
    transformFnPtr = NULL;
#endif

    /* delete uncompressed/untransformed file */
    if ( remove(uncomprOutFn) != 0 ) {
        fprintf(stderr, "ERROR: Cannot remove uncompressed output file %s\n", uncomprOutFn);
        return STARCHCAT_EXIT_FAILURE;
    }

    /*
        Because we used the STARCH helper routines to transform the BED 
        data, those routines updated the metadata for us. So we don't need 
        to do any extra work at this step.
    */

    /* cleanup */
    if (uncomprOutFn)
        free(uncomprOutFn);
    if (comprOutFn) 
        free(comprOutFn);

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_parseCoordinatesFromBedLine(const char *lineBuf, const size_t inRecIdx, SignedCoordType *starts, SignedCoordType *stops)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT2_parseCoordinatesFromBedLine() ---\n");
    fprintf (stderr, "\tlineBuf -> %s\n", lineBuf);
    fprintf (stderr, "\tinRecIdx -> %zu\n", inRecIdx);
    fprintf (stderr, "\tstarts[inRecIdx] -> %" PRId64 "\n", starts[inRecIdx]);
    fprintf (stderr, "\tstops[inRecIdx] -> %" PRId64 "\n", stops[inRecIdx]);
#endif
    static const char tab = '\t';
    static const char newline = '\n';
    char lineBufChar;
    size_t lineBufIdx;
    unsigned int fieldIdx = 0U;
    char fieldBuf[MAX_DEC_INTEGERS + 1];
    unsigned int fieldBufIdx = 0U;

    for (lineBufIdx = 0; lineBufIdx < strlen(lineBuf); lineBufIdx++) {
        lineBufChar = lineBuf[lineBufIdx];
        if ((lineBufChar != tab) && (lineBufChar != newline))
            fieldBuf[fieldBufIdx++] = lineBufChar;
        else {
            fieldBuf[fieldBufIdx] = '\0';
#ifdef __cplusplus
            switch (fieldIdx) {
                case TOKEN_START_FIELD_INDEX: {
#ifdef __cplusplus
                    starts[inRecIdx] = static_cast<SignedCoordType>( strtoll(reinterpret_cast<const char *>( fieldBuf ), nullptr, STARCH_RADIX) );
#else
                    starts[inRecIdx] = static_cast<SignedCoordType>( strtoll(reinterpret_cast<const char *>( fieldBuf ), NULL, STARCH_RADIX) );
#endif
                    break;
                }
                case TOKEN_STOP_FIELD_INDEX: {
#ifdef __cplusplus
                    stops[inRecIdx] = static_cast<SignedCoordType>( strtoll(reinterpret_cast<const char *>( fieldBuf ), nullptr, STARCH_RADIX) );
#else
                    stops[inRecIdx] = static_cast<SignedCoordType>( strtoll(reinterpret_cast<const char *>( fieldBuf ), NULL, STARCH_RADIX) );
#endif
                    break;
                }
                default:
                    break;
            }
#else
            switch (fieldIdx) {
                case TOKEN_START_FIELD_INDEX: {
#ifdef __cplusplus
                    starts[inRecIdx] = (SignedCoordType) strtoll((const char *)fieldBuf, nullptr, STARCH_RADIX);
#else
                    starts[inRecIdx] = (SignedCoordType) strtoll((const char *)fieldBuf, NULL, STARCH_RADIX);
#endif
                    break;
                }
                case TOKEN_STOP_FIELD_INDEX: {
#ifdef __cplusplus
                    stops[inRecIdx] = (SignedCoordType) strtoll((const char *)fieldBuf, nullptr, STARCH_RADIX);
#else
                    stops[inRecIdx] = (SignedCoordType) strtoll((const char *)fieldBuf, NULL, STARCH_RADIX);
#endif
                    break;
                }
                default:
                    break;
            }
#endif
            fieldBufIdx = 0U;
            fieldIdx++;
        }
        if (fieldIdx == 4)
            break;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_identifyLowestBedElement(const Boolean *eobFlags, const SignedCoordType *starts, const SignedCoordType *stops, const size_t numRecords, size_t *lowestIdx)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT2_identifyLowestBedElement() ---\n");
#endif    
    size_t recIdx;

#ifdef __cplusplus
    SignedCoordType currentStart = std::numeric_limits<int64_t>::min();
    SignedCoordType currentStop = std::numeric_limits<int64_t>::min();
    SignedCoordType lowestStart = std::numeric_limits<int64_t>::max();
    SignedCoordType lowestStop = std::numeric_limits<int64_t>::max();
#else
    SignedCoordType currentStart = INT64_MIN;
    SignedCoordType currentStop = INT64_MIN;
    SignedCoordType lowestStart = INT64_MAX;
    SignedCoordType lowestStop = INT64_MAX;
#endif
    Boolean checkStopFlag = kStarchFalse;

#ifdef __cplusplus
    *lowestIdx = static_cast<size_t>( -1 );
#else
    *lowestIdx = (size_t) -1;
#endif

    for (recIdx = 0U; recIdx < numRecords; recIdx++) {
        if (eobFlags[recIdx] == kStarchFalse) {
            currentStart = starts[recIdx];
            currentStop = stops[recIdx];
#ifdef DEBUG
            fprintf(stderr, "\trecIdx -> %zu \n", recIdx);
            fprintf(stderr, "\tstarts[recIdx] -> %" PRId64 " \n", currentStart);
            fprintf(stderr, "\tstops[recIdx] -> %" PRId64 " \n", currentStop);
            fprintf(stderr, "\tlowestStart -> %" PRId64 "\n", lowestStart);
#endif
            if (currentStart < lowestStart) {
                lowestStart = currentStart;
                *lowestIdx = recIdx;
            }
            else if (currentStart == lowestStart) {
                checkStopFlag = kStarchTrue;
            }
        }
    }

    /* if the start coordinates match, we decide upon comparison of the stop coordinates */
    if (checkStopFlag == kStarchTrue) {
        for (recIdx = 0U; recIdx < numRecords; recIdx++) {
            currentStart = starts[recIdx];
            currentStop = stops[recIdx];
            if (eobFlags[recIdx] == kStarchFalse) {
                if ((currentStart == lowestStart) && (currentStop < lowestStop)) { 
                    lowestStop = currentStop;
                    *lowestIdx = recIdx;
                }
            }
        }
    }

#ifdef DEBUG
    fprintf(stderr, "\tLE STATE *lowestIdx -> %zu\n", *lowestIdx);
#endif

#ifdef __cplusplus
    if (*lowestIdx == static_cast<size_t>( -1 ))
        return STARCHCAT_EXIT_FAILURE;
#else
    if (*lowestIdx == (size_t) -1)
        return STARCHCAT_EXIT_FAILURE;
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_identifyLowestBedElementV2p2(const Boolean *eobFlags, const SignedCoordType *starts, const SignedCoordType *stops, const char **remainders, const size_t numRecords, size_t *lowestIdx)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT2_identifyLowestBedElementV2p2() ---\n");
#endif    
    size_t recIdx;

#ifdef __cplusplus
    SignedCoordType currentStart = std::numeric_limits<int64_t>::min();
    SignedCoordType currentStop = std::numeric_limits<int64_t>::min();
    SignedCoordType lowestStart = std::numeric_limits<int64_t>::max();
    SignedCoordType lowestStop = std::numeric_limits<int64_t>::max();
#else
    SignedCoordType currentStart = INT64_MIN;
    SignedCoordType currentStop = INT64_MIN;
    SignedCoordType lowestStart = INT64_MAX;
    SignedCoordType lowestStop = INT64_MAX;
#endif
    Boolean checkStopFlag = kStarchFalse;

#ifdef __cplusplus
    *lowestIdx = static_cast<size_t>( -1 );
#else
    *lowestIdx = (size_t) -1;
#endif

    for (recIdx = 0U; recIdx < numRecords; recIdx++) {
        if (eobFlags[recIdx] == kStarchFalse) {
            currentStart = starts[recIdx];
            currentStop = stops[recIdx];
#ifdef DEBUG
            fprintf(stderr, "\trecIdx -> %zu \n", recIdx);
            fprintf(stderr, "\tstarts[recIdx] -> %" PRId64 " \n", currentStart);
            fprintf(stderr, "\tstops[recIdx] -> %" PRId64 " \n", currentStop);
            fprintf(stderr, "\tlowestStart -> %" PRId64 "\n", lowestStart);
#endif
            if (currentStart < lowestStart) {
                lowestStart = currentStart;
                *lowestIdx = recIdx;
            }
            else if (currentStart == lowestStart) {
                checkStopFlag = kStarchTrue;
            }
        }
    }

    /* 
        if the start coordinates match, we decide upon comparison of the stop coordinates 
        by keeping an array of matches on lowest-stop coordinates -- when there are no more 
        records to check, then compare remainders between matching lowest-stop records 
    */

    if (checkStopFlag == kStarchTrue) {

#ifdef __cplusplus
        size_t *lowestStopMatches = nullptr;
#else
        size_t *lowestStopMatches = NULL;
#endif
        size_t lowestStopMatchesCapacity = 0;
#ifdef __cplusplus
        lowestStopMatches = static_cast<size_t *> (malloc(numRecords * sizeof(*lowestStopMatches)));
#else
        lowestStopMatches = malloc(numRecords * sizeof(*lowestStopMatches));
#endif
        if (!lowestStopMatches) {
            fprintf(stderr, "ERROR: Could not allocate space for lowest-stop matches array!\n");
            exit(ENOMEM);
        }

        for (recIdx = 0U; recIdx < numRecords; recIdx++) {
            currentStart = starts[recIdx];
            currentStop = stops[recIdx];
            if (eobFlags[recIdx] == kStarchFalse) {
                if ((currentStart == lowestStart) && (currentStop < lowestStop)) { 
                    lowestStop = currentStop;
                    *lowestIdx = recIdx;
                    /* empty out the lowestStopMatches array and add this element */
                    lowestStopMatchesCapacity = 0;
                    lowestStopMatches[lowestStopMatchesCapacity++] = recIdx;
                }
                else if ((currentStart == lowestStart) && (currentStop == lowestStop)) { 
                    lowestStopMatches[lowestStopMatchesCapacity++] = recIdx;
                }
            }
        }

        /*
            process remainders when there are two or more lowestStopMatches entries
            (the lowestStopMatches array points to record indices, so we dereference
            from there to the original index)
        */

        size_t lowestStopMatchIdx;
        if (lowestStopMatchesCapacity > 1) {
            const char *lowestRemainder = remainders[lowestStopMatches[0]];
            for (lowestStopMatchIdx = 1U; lowestStopMatchIdx < lowestStopMatchesCapacity; lowestStopMatchIdx++) {
                if (strcmp(lowestRemainder, remainders[lowestStopMatches[lowestStopMatchIdx]]) > 0) {
                    *lowestIdx = lowestStopMatches[lowestStopMatchIdx];
                }
            }
        }

        free(lowestStopMatches);
#ifdef __cplusplus
        lowestStopMatches = nullptr;
#else
        lowestStopMatches = NULL;
#endif
    }

#ifdef DEBUG
    fprintf(stderr, "\tLE STATE *lowestIdx -> %zu\n", *lowestIdx);
#endif

#ifdef __cplusplus
    if (*lowestIdx == static_cast<size_t>( -1 ))
        return STARCHCAT_EXIT_FAILURE;
#else
    if (*lowestIdx == (size_t) -1)
        return STARCHCAT_EXIT_FAILURE;
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_pullNextBedElement(const size_t recIdx, const char **inLinesBuf, const LineCountType *nInLinesBuf, char **outLineBuf, uint64_t **inBufNewlineOffsets)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_pullNextBedElement() ---\n");
    fprintf(stderr, "\trecIdx -> %zu\n", recIdx);
#endif
    const char *inLinesBufPtr;
    uint64_t inLinesBufIdx;
    char charBuf;
    size_t charBufIdx;
    static const char newline = '\n';
    char bedLineBuf[TOKENS_MAX_LENGTH + 1] = {0}; /* zeroing the stack array ensures we don't copy extra junk between iterations */

    if (!inLinesBuf) {
        fprintf(stderr, "ERROR: no inLinesBuf in STARCHCAT2_pullNextBedElement()\n");
        exit(-1);
    }
    inLinesBufPtr = inLinesBuf[recIdx];

    if (!inBufNewlineOffsets) {
        fprintf(stderr, "ERROR: no inBufNewlineOffsets in STARCHCAT2_pullNextBedElement()\n");
        exit(-1);
    }
    inLinesBufIdx = (*inBufNewlineOffsets)[recIdx];

    if (inLinesBufIdx >= nInLinesBuf[recIdx])
        return STARCHCAT_EXIT_FAILURE;

    charBuf = inLinesBufPtr[inLinesBufIdx];
    charBufIdx = 0U;

    while (charBuf != newline) {
        bedLineBuf[charBufIdx++] = inLinesBufPtr[inLinesBufIdx];
        charBuf = inLinesBufPtr[inLinesBufIdx++];
    }
    bedLineBuf[inLinesBufIdx] = '\0';

#ifdef DEBUG
    fprintf(stderr, "\tCOPIED %zu characters from inLinesBufPtr: [%s] to bedLineBuf: [%s]\n", charBufIdx, inLinesBufPtr, bedLineBuf);
#endif

    (*inBufNewlineOffsets)[recIdx] = inLinesBufIdx;
    memcpy(outLineBuf[recIdx], bedLineBuf, charBufIdx + 1);

#ifdef DEBUG
    fprintf(stderr, "\toutput from STARCHCAT2_pullNextBedElement() is:\n[%s]\n---\n", bedLineBuf);
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_mergeInputRecordsToOutput(const char *inChr, Metadata **outMd, const char *outTag, const CompressionType outType, const ChromosomeSummary *summary, size_t *cumulativeOutputSize)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT2_mergeInputRecordsToOutput() ---\n");
#endif

    /*
        1. Fill extractionBuffers for starch files 1..N
        2. Extract one BED line from each extractionBuffer
        3. Test which is lowest and put that into compressionBuffer
        4. Re-extract BED line from extractionBuffer with lowest element
            a. If the extractionBuffer is empty, attempt to refill it from starch file X
            b. If starch file X has no more data, mark the starch file as off-line
        5. Repeat steps 3 and 4 until the compressionBuffer is full or all starch files 1..N are off-line
            a. If the compressionBuffer is full: transform, compress, empty. Go to steps 3 and 4.
            b. If all starch files are off-line, do the same, but quit afterwards -- we're done.
    */

#ifdef __cplusplus
    char **extractionBuffers = nullptr;
    size_t *nExtractionBuffers = nullptr;
    int *extractionBufferOffsets = nullptr;
    char **extractionRemainderBufs = nullptr;
    size_t *nExtractionRemainderBufs = nullptr;
    char *compressionBuffer = nullptr;
    LineCountType *extractedLineCounts = nullptr;
    char **extractedElements = nullptr;
    SignedCoordType *starts = nullptr;
    SignedCoordType *stops = nullptr;
    char **remainders = nullptr;
    Boolean *eobFlags = nullptr;
    Boolean *eofFlags = nullptr;
    FILE *inFp = nullptr;
    BZFILE *bzOutFp = nullptr;
    FILE **zInFps = nullptr;
    BZFILE **bzInFps = nullptr;
    z_stream *zInStreams = nullptr;
    char *finalSignature = nullptr;
    char *finalOutTagFn = nullptr;
    MetadataRecord *inRecord = nullptr;
    size_t *nBzReads = nullptr;
    size_t *nZReads = nullptr;
    TransformState **transformStates = nullptr;
    char *retransformedOutputBuffer = nullptr;
    TransformState *outputRetransformState = nullptr;
#else
    char **extractionBuffers = NULL;
    size_t *nExtractionBuffers = NULL;
    int *extractionBufferOffsets = NULL;
    char **extractionRemainderBufs = NULL;
    size_t *nExtractionRemainderBufs = NULL;
    char *compressionBuffer = NULL;
    LineCountType *extractedLineCounts = NULL;
    char **extractedElements = NULL;
    SignedCoordType *starts = NULL;
    SignedCoordType *stops = NULL;
    char **remainders = NULL;
    Boolean *eobFlags = NULL;
    Boolean *eofFlags = NULL;
    FILE *inFp = NULL;
    BZFILE *bzOutFp = NULL;
    FILE **zInFps = NULL;
    BZFILE **bzInFps = NULL;
    z_stream *zInStreams = NULL;
    char *finalSignature = NULL;
    char *finalOutTagFn = NULL;
    MetadataRecord *inRecord = NULL;
    size_t *nBzReads = NULL;
    size_t *nZReads = NULL;
    TransformState **transformStates = NULL;
    char *retransformedOutputBuffer = NULL;
    TransformState *outputRetransformState = NULL;
#endif
    size_t inRecIdx = 0U;
    LineCountType compressionLineCount = 0;
    size_t remainderCapacity = TOKENS_MAX_LENGTH + 1;
    size_t lowestStartElementIdx = 0U;
    Boolean allEOF = kStarchFalse;
    FILE *outFp = stdout;
    z_stream zOutStream;
    CompressionType inType = kUndefined;
    uint64_t bzOutBytesConsumed = 0;
    uint64_t bzOutBytesWritten = 0;
    uint64_t finalStreamSize = 0;
    LineCountType finalLineCount = 0;
    LineLengthType finalLineMaxStringLength = STARCH_DEFAULT_LINE_STRING_LENGTH;
    BaseCountType finalTotalNonUniqueBases = 0;
    BaseCountType finalTotalUniqueBases = 0;
    Boolean finalDuplicateElementExists = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
    Boolean finalNestedElementExists = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
    Boolean flushZStreamFlag = kStarchFalse;
    size_t nCompressionBuffer = STARCHCAT_RETRANSFORM_LINE_COUNT_MAX * TOKENS_MAX_LENGTH + 1;
    int lowestElementRes = STARCHCAT_EXIT_SUCCESS;

    /* hash variables */
    struct sha1_ctx r_perChromosomeHashCtx;
    unsigned char r_sha1Digest[STARCH2_MD_FOOTER_SHA1_LENGTH];
#ifdef __cplusplus
    char *r_base64EncodedSha1Digest = nullptr;
#else
    char *r_base64EncodedSha1Digest = NULL;
#endif

    /* setup */
#ifdef __cplusplus
    extractionBuffers              = static_cast<char **>(                  malloc(sizeof(char *)               * summary->numRecords) );
    nExtractionBuffers             = static_cast<size_t *>(                 malloc(sizeof(size_t)               * summary->numRecords) );
    extractionBufferOffsets        = static_cast<int *>(                    malloc(sizeof(int)                  * summary->numRecords) );
    compressionBuffer              = static_cast<char *>(                   malloc(sizeof(char)                 * nCompressionBuffer) );
    extractedLineCounts            = static_cast<LineCountType *>(          malloc(sizeof(LineCountType)        * summary->numRecords) );
    extractedElements              = static_cast<char **>(                  malloc(sizeof(char *)               * summary->numRecords) );
    eobFlags                       = static_cast<Boolean *>(                malloc(sizeof(Boolean)              * summary->numRecords) );
    eofFlags                       = static_cast<Boolean *>(                malloc(sizeof(Boolean)              * summary->numRecords) );
    starts                         = static_cast<SignedCoordType *>(        malloc(sizeof(SignedCoordType)      * summary->numRecords) );
    stops                          = static_cast<SignedCoordType *>(        malloc(sizeof(SignedCoordType)      * summary->numRecords) );
    remainders                     = static_cast<char **>(                  malloc(sizeof(char *)               * summary->numRecords) );
    transformStates                = static_cast<TransformState **>(        malloc(sizeof(TransformState *)     * summary->numRecords) );
    extractionRemainderBufs        = static_cast<char **>(                  malloc(sizeof(char *)               * summary->numRecords) );
    nExtractionRemainderBufs       = static_cast<size_t *>(                 malloc(sizeof(size_t)               * summary->numRecords) );
    bzInFps                        = static_cast<BZFILE **>(                malloc(sizeof(BZFILE *)             * summary->numRecords) );
    nBzReads                       = static_cast<size_t *>(                 malloc(sizeof(size_t)               * summary->numRecords) );
    zInFps                         = static_cast<FILE **>(                  malloc(sizeof(FILE *)               * summary->numRecords) );
    zInStreams                     = static_cast<z_stream *>(               malloc(sizeof(z_stream)             * summary->numRecords) );
    nZReads                        = static_cast<size_t *>(                 malloc(sizeof(size_t)               * summary->numRecords) );
    retransformedOutputBuffer      = static_cast<char *>(                   malloc(sizeof(char)                 * STARCHCAT_RETRANSFORM_BUFFER_SIZE + 1) );
    outputRetransformState         = static_cast<TransformState *>(         malloc(sizeof(TransformState)) );
#else
    extractionBuffers              = malloc(sizeof(char *)               * summary->numRecords);
    nExtractionBuffers             = malloc(sizeof(size_t)               * summary->numRecords);
    extractionBufferOffsets        = malloc(sizeof(int)                  * summary->numRecords);
    compressionBuffer              = malloc(sizeof(char)                 * nCompressionBuffer);
    extractedLineCounts            = malloc(sizeof(LineCountType)        * summary->numRecords);
    extractedElements              = malloc(sizeof(char *)               * summary->numRecords);
    eobFlags                       = malloc(sizeof(Boolean)              * summary->numRecords);
    eofFlags                       = malloc(sizeof(Boolean)              * summary->numRecords);
    starts                         = malloc(sizeof(SignedCoordType)      * summary->numRecords);
    stops                          = malloc(sizeof(SignedCoordType)      * summary->numRecords);
    remainders                     = malloc(sizeof(char *)               * summary->numRecords);
    transformStates                = malloc(sizeof(TransformState *)     * summary->numRecords);
    extractionRemainderBufs        = malloc(sizeof(char *)               * summary->numRecords);
    nExtractionRemainderBufs       = malloc(sizeof(size_t)               * summary->numRecords);
    bzInFps                        = malloc(sizeof(BZFILE *)             * summary->numRecords);
    nBzReads                       = malloc(sizeof(size_t)               * summary->numRecords);
    zInFps                         = malloc(sizeof(FILE *)               * summary->numRecords);
    zInStreams                     = malloc(sizeof(z_stream)             * summary->numRecords);
    nZReads                        = malloc(sizeof(size_t)               * summary->numRecords);
    retransformedOutputBuffer      = malloc(sizeof(char)                 * STARCHCAT_RETRANSFORM_BUFFER_SIZE + 1);
    outputRetransformState         = malloc(sizeof(TransformState));
#endif

    memset(outputRetransformState->r_chromosome, 0, TOKEN_CHR_MAX_LENGTH);

    /* set up per-chromosome hash context */
    sha1_init_ctx (&r_perChromosomeHashCtx);

    /* test if we allocated memory */
    if (!compressionBuffer) {
        fprintf(stderr, "ERROR: Could not allocate space for compression buffer!\n");
        return STARCHCAT_EXIT_FAILURE;
    }

    /* initialize output stream (stdout) */
    switch (outType) {
        case kBzip2: {
            if (STARCHCAT2_setupBzip2OutputStream(&bzOutFp, outFp) != STARCHCAT_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not set up bzip2 output stream!\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            break;
        }
        case kGzip: {
            if (STARCHCAT2_setupGzipOutputStream(&zOutStream) != STARCHCAT_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Could not set up gzip output stream!\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Unknown compression type specified!\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

    /* 1 -- initialize and fill input (extraction) buffers on first pass */
    for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++)
    {
        eofFlags[inRecIdx]                                      = kStarchFalse;
        eobFlags[inRecIdx]                                      = kStarchFalse;

        if (STARCHCAT2_testSummaryForChromosomeExistence(inChr, summary, inRecIdx) == STARCHCAT_EXIT_FAILURE) {
            eobFlags[inRecIdx] = kStarchTrue;
            eofFlags[inRecIdx] = kStarchTrue;
            continue;
        }

        if (STARCHCAT2_setupInitialFileOffsets(inChr, summary, inRecIdx) != STARCHCAT_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not set up file offsets at chromosome [%s]!\n", inChr);
            return STARCHCAT_EXIT_FAILURE;
        }

        inRecord                                                = *(summary->records) + inRecIdx;
        inType                                                  = inRecord->type; /* get record type */
        inFp                                                    = inRecord->fp;

        if (inType == kGzip)
            zInFps[inRecIdx] = inFp;

        nExtractionBuffers[inRecIdx]                            = STARCHCAT_RETRANSFORM_LINE_COUNT_MAX * TOKENS_MAX_LENGTH;
        extractedLineCounts[inRecIdx]                           = 0;
#ifdef __cplusplus
        extractionBuffers[inRecIdx]                             = static_cast<char *>( malloc(nExtractionBuffers[inRecIdx] + 1) ); /* max lines per record, essentially */
#else
        extractionBuffers[inRecIdx]                             = malloc(nExtractionBuffers[inRecIdx] + 1); /* max lines per record, essentially */
#endif
        if (!extractionBuffers[inRecIdx]) {
            fprintf(stderr, "ERROR: Could not allocate space for extraction buffer at index [%zu]!\n", inRecIdx);
            return STARCHCAT_EXIT_FAILURE;
        }
#ifdef __cplusplus
        transformStates[inRecIdx]                               = static_cast<TransformState *>( malloc(sizeof(TransformState)) );
#else
        transformStates[inRecIdx]                               = malloc(sizeof(TransformState));
#endif
        if (!transformStates[inRecIdx]) {
            fprintf(stderr, "ERROR: Could not allocate space for transform state instance at index [%zu]!\n", inRecIdx);
            return STARCHCAT_EXIT_FAILURE;
        }
        transformStates[inRecIdx]->t_lineIdx                    = 0;
        transformStates[inRecIdx]->t_start                      = 0;
        transformStates[inRecIdx]->t_pLength                    = 0;
        transformStates[inRecIdx]->t_lastEnd                    = 0;
        transformStates[inRecIdx]->t_lcDiff                     = 0;
        transformStates[inRecIdx]->t_lastPosition               = 0;
        strncpy(transformStates[inRecIdx]->t_currentChromosome, inChr, strlen(inChr) + 1);
        transformStates[inRecIdx]->t_currentChromosomeLength    = TOKEN_CHR_MAX_LENGTH;

#ifdef DEBUG
        fprintf(stderr, "transformStates[inRecIdx]->t_currentChromosome [%s]\n", transformStates[inRecIdx]->t_currentChromosome);
        fprintf(stderr, "transformStates[inRecIdx]->t_currentChromosomeLength [%lu]\n", transformStates[inRecIdx]->t_currentChromosomeLength);
#endif

        memset(transformStates[inRecIdx]->t_firstInputToken, 0, UNSTARCH_FIRST_TOKEN_MAX_LENGTH);
        memset(transformStates[inRecIdx]->t_secondInputToken, 0, UNSTARCH_SECOND_TOKEN_MAX_LENGTH);
        memset(transformStates[inRecIdx]->t_currentRemainder, 0, UNSTARCH_SECOND_TOKEN_MAX_LENGTH);
        transformStates[inRecIdx]->t_currentRemainderLength     = UNSTARCH_SECOND_TOKEN_MAX_LENGTH + 1; 
        transformStates[inRecIdx]->t_nExtractionBuffer          = 0;
        transformStates[inRecIdx]->t_nExtractionBufferPos       = 0;
#ifdef __cplusplus
        extractionRemainderBufs[inRecIdx]                       = static_cast<char *>( malloc(TOKENS_MAX_LENGTH + 1) );
#else
        extractionRemainderBufs[inRecIdx]                       = malloc(TOKENS_MAX_LENGTH + 1);
#endif
        if (!extractionRemainderBufs[inRecIdx]) {
            fprintf(stderr, "ERROR: Could not allocate space for extraction buffer remainder at index [%zu]!\n", inRecIdx);
            return STARCHCAT_EXIT_FAILURE;
        }
        nExtractionRemainderBufs[inRecIdx]                      = 0;

        switch (inType) {
            case kBzip2: {
                nBzReads[inRecIdx] = 0;
#ifdef __cplusplus
                if (STARCHCAT2_setupBzip2InputStream(static_cast<const size_t>( inRecIdx ), 
                             summary, 
                             &bzInFps[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
#else
                if (STARCHCAT2_setupBzip2InputStream((const size_t) inRecIdx, 
                             summary, 
                             &bzInFps[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
#endif
                    fprintf(stderr, "ERROR: Could not set up bzip2 input stream at index [%zu]!\n", inRecIdx);
                    return STARCHCAT_EXIT_FAILURE;
                }
                if (STARCHCAT2_fillExtractionBufferFromBzip2Stream(&eofFlags[inRecIdx], 
#ifdef __cplusplus
                                                                   const_cast<char *>( inChr ), 
#else
                                                                   (char *) inChr, 
#endif
                                                                   extractionBuffers[inRecIdx], 
                                                                   &nExtractionBuffers[inRecIdx], 
                                                                   &bzInFps[inRecIdx], 
                                                                   &nBzReads[inRecIdx], 
                                                                   extractionRemainderBufs[inRecIdx], 
                                                                   &nExtractionRemainderBufs[inRecIdx], 
                                                                   transformStates[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
                    fprintf(stderr, "ERROR: Could not extract data from bzip2 input stream at index [%zu]!\n", inRecIdx);
                    return STARCHCAT_EXIT_FAILURE;
                }
                break;
            }
            case kGzip: {
                nZReads[inRecIdx] = 0;
                if (STARCHCAT2_setupGzipInputStream(&zInStreams[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
                    fprintf(stderr, "ERROR: Could not set up gzip input stream at index [%zu]!\n", inRecIdx);
                    return STARCHCAT_EXIT_FAILURE;
                }
                if (STARCHCAT2_fillExtractionBufferFromGzipStream(&eofFlags[inRecIdx], 
                                                                  &zInFps[inRecIdx], 
#ifdef __cplusplus
                                                                  const_cast<char *>( inChr ), 
#else
                                                                  (char *) inChr, 
#endif
                                                                  extractionBuffers[inRecIdx], 
                                                                  &nExtractionBuffers[inRecIdx], 
                                                                  &zInStreams[inRecIdx], 
                                                                  &nZReads[inRecIdx], 
                                                                  &extractionRemainderBufs[inRecIdx], 
                                                                  &nExtractionRemainderBufs[inRecIdx], 
                                                                  transformStates[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
                    fprintf(stderr, "ERROR: Could not extract data from gzip input stream at index [%zu]!\n", inRecIdx);
                    return STARCHCAT_EXIT_FAILURE;
                }
                break;
            }
            case kUndefined: {
                fprintf(stderr, "ERROR: Unknown compression type specified in input stream at index [%zu]!\n", inRecIdx);
                return STARCHCAT_EXIT_FAILURE;
            }
        }
        extractionBufferOffsets[inRecIdx] = 0; /* point these guys to the first element */
        extractedLineCounts[inRecIdx] = transformStates[inRecIdx]->t_lineIdx;
#ifdef __cplusplus
        extractedElements[inRecIdx] = static_cast<char *>( malloc(TOKENS_MAX_LENGTH + 1) );
#else
        extractedElements[inRecIdx] = malloc(TOKENS_MAX_LENGTH + 1);
#endif
        if (!extractedElements[inRecIdx]) {
            fprintf(stderr, "ERROR: Could not allocate memory for extracted element buffer!\n");
            return STARCHCAT_EXIT_FAILURE;
        }

        /* eobFlags[inRecIdx] = kStarchFalse; */
        STARCHCAT2_extractBedLine(&eobFlags[inRecIdx], 
                                  extractionBuffers[inRecIdx], 
                                  &extractionBufferOffsets[inRecIdx], 
                                  &extractedElements[inRecIdx]);
        extractedLineCounts[inRecIdx]--;

        /* memset(outputRetransformState->r_chromosome, 0, TOKEN_CHR_MAX_LENGTH); */

        memset(outputRetransformState->r_remainder, 0, UNSTARCH_SECOND_TOKEN_MAX_LENGTH);
        outputRetransformState->r_start                     = 0;
        outputRetransformState->r_stop                      = 0;
        outputRetransformState->r_pStart                    = 0;
        outputRetransformState->r_pStop                     = 0;
        outputRetransformState->r_coordDiff                 = 0;
        outputRetransformState->r_lcDiff                    = 0;
        outputRetransformState->r_lastPosition              = 0;
        outputRetransformState->r_totalNonUniqueBases       = 0;
        outputRetransformState->r_totalUniqueBases          = 0;
#ifdef __cplusplus
        outputRetransformState->r_signature                 = nullptr;
#else
        outputRetransformState->r_signature                 = NULL;
#endif
        outputRetransformState->r_lineMaxStringLength       = STARCH_DEFAULT_LINE_STRING_LENGTH;
        outputRetransformState->r_duplicateElementExists    = STARCH_DEFAULT_DUPLICATE_ELEMENT_FLAG_VALUE;
        outputRetransformState->r_nestedElementExists       = STARCH_DEFAULT_NESTED_ELEMENT_FLAG_VALUE;
        outputRetransformState->r_previousStop              = 0;
#ifdef __cplusplus
        outputRetransformState->r_nRetransBuf               = static_cast<size_t>( 0 );
#else
        outputRetransformState->r_nRetransBuf               = (size_t) 0;
#endif

        compressionBuffer[0] = '\0';

        starts[inRecIdx] = 0;
        stops[inRecIdx] = 0;

#ifdef __cplusplus
        remainders[inRecIdx] = static_cast<char *>( malloc(remainderCapacity) );
#else
        remainders[inRecIdx] = malloc(remainderCapacity);
#endif
        if (!remainders[inRecIdx]) {
            fprintf(stderr, "ERROR: Could not allocate memory for remainder buffer!\n");
            return STARCHCAT_EXIT_FAILURE;
        }
        memset(remainders[inRecIdx], 0, remainderCapacity);
    }

    /* merge */
    do {
        /* first, check if all extraction buffers are marked EOB or EOF */
        allEOF = kStarchTrue;
        for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
            if ((eobFlags[inRecIdx] == kStarchFalse) || (eofFlags[inRecIdx] == kStarchFalse)) {
                allEOF = kStarchFalse;
                continue;
            }
        }

        /* 2, 3 -- parse coordinates for each record's current bed element (this gets us the start and stop coords) */

        for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
#ifdef DEBUG
            fprintf(stderr, "inRecIdx -> %zu\teobFlags[inRecIdx] -> %d\teofFlags[inRecIdx] -> %d\n", inRecIdx, eobFlags[inRecIdx], eofFlags[inRecIdx]);
#endif
            //if ((eobFlags[inRecIdx] == kStarchFalse) && (eofFlags[inRecIdx] == kStarchFalse))
            if (eobFlags[inRecIdx] == kStarchFalse) {
#ifdef __cplusplus
                STARCHCAT2_parseCoordinatesFromBedLineV2p2( &eobFlags[inRecIdx], 
                                                            reinterpret_cast<const char *>( extractedElements[inRecIdx] ), 
                                                            &starts[inRecIdx], 
                                                            &stops[inRecIdx],
                                                            &remainders[inRecIdx]);
#else
                STARCHCAT2_parseCoordinatesFromBedLineV2p2( &eobFlags[inRecIdx], 
                                                            (const char *) extractedElements[inRecIdx], 
                                                            &starts[inRecIdx], 
                                                            &stops[inRecIdx],
                                                            &remainders[inRecIdx]);
#endif
            }
        }

        /* identify the lowest element, put it into the compression buffer for later processing, and refill from the input stream, if needed */

#ifdef __cplusplus
        lowestElementRes = STARCHCAT2_identifyLowestBedElementV2p2( reinterpret_cast<const Boolean *>( eobFlags ), 
                                                                    reinterpret_cast<const SignedCoordType *>( starts ), 
                                                                    reinterpret_cast<const SignedCoordType *>( stops ),
                                                                    const_cast<const char **>( remainders ), 
                                                                    static_cast<const size_t>( summary->numRecords ), 
                                                                    &lowestStartElementIdx);
#else
        lowestElementRes = STARCHCAT2_identifyLowestBedElementV2p2( (const Boolean *) eobFlags, 
                                                                    (const SignedCoordType *) starts, 
                                                                    (const SignedCoordType *) stops, 
                                                                    (const char **) remainders, 
                                                                    (const size_t) summary->numRecords, 
                                                                    &lowestStartElementIdx);
#endif
        if ((eobFlags) && (starts) && (stops) && (lowestElementRes == STARCHCAT_EXIT_SUCCESS)) {
#ifdef __cplusplus
            LineLengthType leLineMaxStringLength = static_cast<LineLengthType>(strlen(static_cast<const char *>( extractedElements[lowestStartElementIdx] )) - 1);
            outputRetransformState->r_lineMaxStringLength = (outputRetransformState->r_lineMaxStringLength >= leLineMaxStringLength) ? outputRetransformState->r_lineMaxStringLength : leLineMaxStringLength;
            STARCHCAT2_addLowestBedElementToCompressionBuffer( compressionBuffer, 
                                                               static_cast<const char *>( extractedElements[lowestStartElementIdx] ), 
                                                               &compressionLineCount);
#else
            LineLengthType leLineMaxStringLength = (LineLengthType) strlen(extractedElements[lowestStartElementIdx]) - 1;
            outputRetransformState->r_lineMaxStringLength = (outputRetransformState->r_lineMaxStringLength >= leLineMaxStringLength) ? outputRetransformState->r_lineMaxStringLength : leLineMaxStringLength;
            STARCHCAT2_addLowestBedElementToCompressionBuffer( compressionBuffer, 
                                                               (const char *) extractedElements[lowestStartElementIdx], 
                                                               &compressionLineCount);
#endif

            /* 4 -- extract lowest element to extracted elements buffer */
            STARCHCAT2_extractBedLine( &eobFlags[lowestStartElementIdx], 
                                       extractionBuffers[lowestStartElementIdx], 
                                       &extractionBufferOffsets[lowestStartElementIdx], 
                                       &extractedElements[lowestStartElementIdx]);

            extractedLineCounts[lowestStartElementIdx]--;

            /* 
                if we extract a bed line and end up at the end of the extraction 
                buffer, we try to read in a fresh extraction buffer 
            */
            if ((eobFlags[lowestStartElementIdx] == kStarchTrue) && (eofFlags[lowestStartElementIdx] == kStarchFalse)) 
            {
                eobFlags[lowestStartElementIdx] = kStarchFalse;
                inRecord    = *(summary->records) + lowestStartElementIdx;
                inType      = inRecord->type;
                inFp        = inRecord->fp;

                nExtractionBuffers[lowestStartElementIdx] = STARCHCAT_RETRANSFORM_LINE_COUNT_MAX * TOKENS_MAX_LENGTH;
                memset(extractionBuffers[lowestStartElementIdx], 0, nExtractionBuffers[lowestStartElementIdx]);
                memset(transformStates[lowestStartElementIdx]->t_firstInputToken, 0, UNSTARCH_FIRST_TOKEN_MAX_LENGTH);
                memset(transformStates[lowestStartElementIdx]->t_secondInputToken, 0, UNSTARCH_SECOND_TOKEN_MAX_LENGTH);
                memset(transformStates[lowestStartElementIdx]->t_currentRemainder, 0, UNSTARCH_SECOND_TOKEN_MAX_LENGTH);

                switch (inType) {
                    case kBzip2: {
                        if (STARCHCAT2_fillExtractionBufferFromBzip2Stream(&eofFlags[lowestStartElementIdx], 
#ifdef __cplusplus
                                                                           const_cast<char *>( inChr ), 
#else
                                                                           (char *) inChr, 
#endif
                                                                           extractionBuffers[lowestStartElementIdx], 
                                                                           &nExtractionBuffers[lowestStartElementIdx], 
                                                                           &bzInFps[lowestStartElementIdx], 
                                                                           &nBzReads[lowestStartElementIdx], 
                                                                           extractionRemainderBufs[lowestStartElementIdx], 
                                                                           &nExtractionRemainderBufs[lowestStartElementIdx], 
                                                                           transformStates[lowestStartElementIdx]) != STARCHCAT_EXIT_SUCCESS) {
                            fprintf(stderr, "ERROR: Could not extract data from bzip2 input stream at index [%zu]!\n", lowestStartElementIdx);
                            return STARCHCAT_EXIT_FAILURE;
                        }
                        break;
                    }
                    case kGzip: {
                        if (STARCHCAT2_fillExtractionBufferFromGzipStream(&eofFlags[lowestStartElementIdx], 
                                                                          &zInFps[lowestStartElementIdx], 
#ifdef __cplusplus
                                                                          const_cast<char *>( inChr ), 
#else
                                                                          (char *) inChr, 
#endif
                                                                          extractionBuffers[lowestStartElementIdx], 
                                                                          &nExtractionBuffers[lowestStartElementIdx], 
                                                                          &zInStreams[lowestStartElementIdx], 
                                                                          &nZReads[lowestStartElementIdx], 
                                                                          &extractionRemainderBufs[lowestStartElementIdx], 
                                                                          &nExtractionRemainderBufs[lowestStartElementIdx], 
                                                                          transformStates[lowestStartElementIdx]) != STARCHCAT_EXIT_SUCCESS) {
                            fprintf(stderr, "ERROR: Could not extract data from gzip input stream at index [%zu]!\n", lowestStartElementIdx);
                            return STARCHCAT_EXIT_FAILURE;
                        }
                        break;
                    }
                    case kUndefined: {
                        fprintf(stderr, "ERROR: Unknown compression type specified in input stream at index [%zu]!\n", lowestStartElementIdx);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                }

                extractedLineCounts[lowestStartElementIdx] = transformStates[lowestStartElementIdx]->t_lineIdx;

                /* 
                    reset the extraction buffer offset and try to pull something from 
                    the extraction buffer into the extracted elements buffer 
                */
                extractionBufferOffsets[lowestStartElementIdx] = 0;
                STARCHCAT2_extractBedLine(&eobFlags[lowestStartElementIdx], 
                                          extractionBuffers[lowestStartElementIdx], 
                                          &extractionBufferOffsets[lowestStartElementIdx], 
                                          &extractedElements[lowestStartElementIdx]);
                extractedLineCounts[lowestStartElementIdx]--;
            }
        }
        else {
            allEOF = kStarchTrue;
        }

        /* 5 -- compress transformation buffer if it is full or if allEOF is true */
        if ((compressionLineCount == STARCHCAT_RETRANSFORM_LINE_COUNT_MAX) || (allEOF == kStarchTrue))
        {
            memcpy(outputRetransformState->r_chromosome, inChr, strlen(inChr) + 1);
            memset(retransformedOutputBuffer, 0, STARCHCAT_RETRANSFORM_BUFFER_SIZE);
            STARCHCAT2_transformCompressionBuffer(compressionBuffer, retransformedOutputBuffer, outputRetransformState);
            finalLineCount += compressionLineCount;
#ifdef DEBUG
            fprintf(stderr, "STEP 5 - COMP BUFFER [%s] -->\nCOMPRESS RETRANSFORMATION BUFFER (FULL)\n[%s]\n", compressionBuffer, retransformedOutputBuffer);
#endif

            /* hash the transformed buffer */
            sha1_process_bytes (retransformedOutputBuffer, strlen(retransformedOutputBuffer), &r_perChromosomeHashCtx);

            STARCHCAT2_resetCompressionBuffer(compressionBuffer, &compressionLineCount);
            switch (outType) {
                case kBzip2: {
                    STARCHCAT2_squeezeRetransformedOutputBufferToBzip2Stream(&bzOutFp, 
                                                                             retransformedOutputBuffer);
                    break;
                }
                case kGzip: {
                    flushZStreamFlag = allEOF;
                    STARCHCAT2_squeezeRetransformedOutputBufferToGzipStream(&zOutStream, 
#ifdef __cplusplus
                                                                            static_cast<const Boolean>( flushZStreamFlag ),
#else
                                                                            (const Boolean) flushZStreamFlag, 
#endif
                                                                            retransformedOutputBuffer, 
                                                                            &finalStreamSize, 
                                                                            cumulativeOutputSize);
                    break;
                }
                case kUndefined: {
                    fprintf(stderr, "ERROR: Unknown compression type specified in output stream!\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
            }
        }

    } while (allEOF == kStarchFalse);

    /* finalize the hash of the transformed buffer */
    sha1_finish_ctx (&r_perChromosomeHashCtx, r_sha1Digest);
#ifdef __cplusplus
    STARCH_encodeBase64(&r_base64EncodedSha1Digest, 
                        static_cast<size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
                        reinterpret_cast<const unsigned char *>( r_sha1Digest ), 
                        static_cast<size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ) );
#else
    STARCH_encodeBase64(&r_base64EncodedSha1Digest, 
                        (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
                        (const unsigned char *) r_sha1Digest, 
                        (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif
    outputRetransformState->r_signature = r_base64EncodedSha1Digest;
#ifdef DEBUG
    fprintf(stderr, "\nPROGRESS: SHA-1 digest for chr [%s] is [%s]\n", inChr, r_base64EncodedSha1Digest);
#endif

    /* breakdown output stream */
    switch (outType) {
        case kBzip2: {
            STARCHCAT2_breakdownBzip2OutputStream(&bzOutFp, &bzOutBytesConsumed, &bzOutBytesWritten);
            finalStreamSize = bzOutBytesWritten;
            *cumulativeOutputSize += bzOutBytesWritten;
            break;
        }
        case kGzip: {
            STARCHCAT2_breakdownGzipOutputStream(&zOutStream);
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Unknown output compression type specified!\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

    /* finalize metadata */
    switch (outType) {
        case kBzip2: {
#ifdef __cplusplus
            finalOutTagFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".bz2")) );
#else
            finalOutTagFn = malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".bz2"));
#endif
            sprintf(finalOutTagFn, "%s.%s.bz2", inChr, outTag);
            break;
        }
        case kGzip: {
#ifdef __cplusplus
            finalOutTagFn = static_cast<char *>( malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".gz")) );
#else
            finalOutTagFn = malloc(strlen(inChr) + strlen(outTag) + 3 + strlen(".gz"));
#endif
            sprintf(finalOutTagFn, "%s.%s.gz", inChr, outTag);
            break;
        }
        case kUndefined: {
            fprintf(stderr, "ERROR: Undefined outbound compression type!\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }
    finalTotalNonUniqueBases = outputRetransformState->r_totalNonUniqueBases;
    finalTotalUniqueBases = outputRetransformState->r_totalUniqueBases;
    finalDuplicateElementExists = outputRetransformState->r_duplicateElementExists;
    finalNestedElementExists = outputRetransformState->r_nestedElementExists;
    finalSignature = outputRetransformState->r_signature;
    finalLineMaxStringLength = outputRetransformState->r_lineMaxStringLength;

    STARCHCAT2_finalizeMetadata( outMd, 
#ifdef __cplusplus
                                 const_cast<char *>( inChr ), 
#else
                                 (char *) inChr, 
#endif
                                 finalOutTagFn, 
                                 finalStreamSize, 
                                 finalLineCount, 
                                 finalTotalNonUniqueBases, 
                                 finalTotalUniqueBases,
                                 finalDuplicateElementExists,
                                 finalNestedElementExists,
                                 finalSignature,
                                 finalLineMaxStringLength );

    /* breakdown */
    for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
        if (STARCHCAT2_testSummaryForChromosomeExistence(inChr, summary, inRecIdx) == STARCHCAT_EXIT_SUCCESS) { 
            if (extractionBuffers[inRecIdx]) {
                free(extractionBuffers[inRecIdx]);
#ifdef __cplusplus
                extractionBuffers[inRecIdx] = nullptr;
#else
                extractionBuffers[inRecIdx] = NULL;
#endif
            }
            if (extractedElements[inRecIdx]) {
                free(extractedElements[inRecIdx]);
#ifdef __cplusplus
                extractedElements[inRecIdx] = nullptr;
#else
                extractedElements[inRecIdx] = NULL;
#endif
            }
            if (extractionRemainderBufs[inRecIdx]) {
                free(extractionRemainderBufs[inRecIdx]);
#ifdef __cplusplus
                extractionRemainderBufs[inRecIdx] = nullptr;
#else
                extractionRemainderBufs[inRecIdx] = NULL;
#endif
            }
            if (remainders[inRecIdx]) {
                free(remainders[inRecIdx]);
#ifdef __cplusplus
                remainders[inRecIdx] = nullptr;
#else
                remainders[inRecIdx] = NULL;
#endif
            }
            inRecord = *(summary->records) + inRecIdx;
            inType = inRecord->type; /* get record type of input stream */
            switch (inType) {
                case kBzip2: {
                    if (STARCHCAT2_breakdownBzip2InputStream(&bzInFps[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
                        fprintf(stderr, "ERROR: Could not break down bzip2 input stream at index [%zu]!\n", inRecIdx);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    break;
                }
                case kGzip: {
                    if (STARCHCAT2_breakdownGzipInputStream(&zInStreams[inRecIdx]) != STARCHCAT_EXIT_SUCCESS) {
                        fprintf(stderr, "ERROR: Could not break down gzip input stream at index [%zu]!\n", inRecIdx);
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    break;
                }
                case kUndefined: {
                    fprintf(stderr, "ERROR: Unknown compression type specified in input stream at index [%zu]!\n", inRecIdx);
                    return STARCHCAT_EXIT_FAILURE;
                }
            }
        } 
    }
    if (extractionBuffers) {
        free(extractionBuffers);
#ifdef __cplusplus
        extractionBuffers = nullptr;
#else
        extractionBuffers = NULL;
#endif
    }
    if (nExtractionBuffers) {
        free(nExtractionBuffers);
#ifdef __cplusplus
        nExtractionBuffers = nullptr;
#else
        nExtractionBuffers = NULL;
#endif
    }
    if (extractionBufferOffsets) {
        free(extractionBufferOffsets);
#ifdef __cplusplus
        extractionBufferOffsets = nullptr;
#else
        extractionBufferOffsets = NULL;
#endif
    }
    if (compressionBuffer) {
        free(compressionBuffer);
#ifdef __cplusplus
        compressionBuffer = nullptr;
#else
        compressionBuffer = NULL;
#endif
    }
    if (zInFps) {
        free(zInFps);
#ifdef __cplusplus
        zInFps = nullptr;
#else
        zInFps = NULL;
#endif
    }
    if (extractedLineCounts) {
        free(extractedLineCounts);
#ifdef __cplusplus
        extractedLineCounts = nullptr;
#else
        extractedLineCounts = NULL;
#endif
    }
    if (extractedElements) {
        free(extractedElements);
#ifdef __cplusplus
        extractedElements = nullptr;
#else
        extractedElements = NULL;
#endif
    }
    if (eobFlags) {
        free(eobFlags);
#ifdef __cplusplus
        eobFlags = nullptr;
#else
        eobFlags = NULL;
#endif
    }
    if (eofFlags) {
        free(eofFlags);
#ifdef __cplusplus
        eofFlags = nullptr;
#else
        eofFlags = NULL;
#endif
    }
    if (starts) {
        free(starts);
#ifdef __cplusplus
        starts = nullptr;
#else
        starts = NULL;
#endif
    }
    if (stops) {
        free(stops);
#ifdef __cplusplus
        stops = nullptr;
#else
        stops = NULL;
#endif
    }
    if (remainders) {
        free(remainders);
#ifdef __cplusplus
        remainders = nullptr;
#else
        remainders = NULL;
#endif
    }
    if (transformStates) {
        free(transformStates);
#ifdef __cplusplus
        transformStates = nullptr;
#else
        transformStates = NULL;
#endif
    }
    if (extractionRemainderBufs) {
        free(extractionRemainderBufs);
#ifdef __cplusplus
        extractionRemainderBufs = nullptr;
#else
        extractionRemainderBufs = NULL;
#endif
    }
    if (nExtractionRemainderBufs) {
        free(nExtractionRemainderBufs);
#ifdef __cplusplus
        nExtractionRemainderBufs = nullptr;
#else
        nExtractionRemainderBufs = NULL;
#endif
    }
    if (bzInFps) {
        free(bzInFps);
#ifdef __cplusplus
        bzInFps = nullptr;
#else
        bzInFps = NULL;
#endif
    }
    if (nBzReads) {
        free(nBzReads);
#ifdef __cplusplus
        nBzReads = nullptr;
#else
        nBzReads = NULL;
#endif
    }
    if (zInStreams) {
        free(zInStreams);
#ifdef __cplusplus
        zInStreams = nullptr;
#else
        zInStreams = NULL;
#endif
    }
    if (nZReads) {
        free(nZReads);
#ifdef __cplusplus
        nZReads = nullptr;
#else
        nZReads = NULL;
#endif
    }
    if (retransformedOutputBuffer) {
        free(retransformedOutputBuffer);
#ifdef __cplusplus
        retransformedOutputBuffer = nullptr;
#else
        retransformedOutputBuffer = NULL;
#endif
    }
    if (outputRetransformState) {
        free(outputRetransformState);
#ifdef __cplusplus
        outputRetransformState = nullptr;
#else
        outputRetransformState = NULL;
#endif
    }
    if (finalOutTagFn) {
        free(finalOutTagFn);
#ifdef __cplusplus
        finalOutTagFn = nullptr;
#else
        finalOutTagFn = NULL;
#endif
    }
    if (r_base64EncodedSha1Digest) {
        free(r_base64EncodedSha1Digest);
#ifdef __cplusplus
        r_base64EncodedSha1Digest = nullptr;
#else
        r_base64EncodedSha1Digest = NULL;
#endif
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_mergeInputRecordsToOutput(Metadata **outMd, const char *outTag, const CompressionType outType, const ChromosomeSummary *summary)
{
    /*
        This function will grab a buffer-ful of each file's chromosomal data, read 
        through each buffer until the 'lowest' item is found, and print that item 
        to a temporary file buffer. Once all the buffers are read through, we 
        compress the temp buffer and update the output metadata with the newly- 
        minted, newly-compressed chromosome.
    */

#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_mergeInputRecordsToOutput() ---\n");
#endif
#ifdef __cplusplus
    MetadataRecord *inRec = nullptr;
    char *tempOutFn = nullptr;
    char **tempOutFns = nullptr;
    FILE *tempOutFp = nullptr;
    FILE **tempOutFps = nullptr;
    char **lineBuffers = nullptr;
    SignedCoordType *starts = nullptr;
    SignedCoordType *stops = nullptr;
    int *lowestStartIdxs = nullptr;
    char *uncomprOutFn = nullptr;
    FILE *uncomprOutFnPtr = nullptr;
    FILE *transformFnPtr = nullptr;
    char *note = nullptr;
#else
    MetadataRecord *inRec = NULL;
    char *tempOutFn = NULL;
    char **tempOutFns = NULL;
    FILE *tempOutFp = NULL;
    FILE **tempOutFps = NULL;
    char **lineBuffers = NULL;
    SignedCoordType *starts = NULL;
    SignedCoordType *stops = NULL;
    int *lowestStartIdxs = NULL;
    char *uncomprOutFn = NULL;
    FILE *uncomprOutFnPtr = NULL;
    FILE *transformFnPtr = NULL;
    char *note = NULL;
#endif
    unsigned int inRecIdx;
    unsigned int allEOFFlag = 0U;
    unsigned int bufferIdx;
    int c;
    unsigned int lsiIdx, lsiTestIdx;
    SignedCoordType lowestStart;
    SignedCoordType lowestStop;
    unsigned int lowestStopIdx = 0U;
    unsigned long fieldIdx;
    char fieldBuffer[STARCHCAT_FIELD_BUFFER_MAX_LENGTH];
    unsigned int fieldBufferIdx;

    /* 
        Set up temporary output streams, which will contain uncompressed,
        per-record and per-chromosome data. We will ultimately walk through
        these streams a line at a time...
    */
#ifdef __cplusplus
    tempOutFns = static_cast<char **>( malloc(sizeof(char *) * summary->numRecords) );
    tempOutFps = static_cast<FILE **>( malloc(sizeof(FILE *) * summary->numRecords) );
#else
    tempOutFns = malloc(sizeof(char *) * summary->numRecords);
    tempOutFps = malloc(sizeof(FILE *) * summary->numRecords);
#endif
    if ((! tempOutFns) || (! tempOutFps)) {
        fprintf(stderr, "ERROR: Could not allocate space for temporary output buffers. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
#ifdef __cplusplus
        tempOutFn = static_cast<char *>( malloc(strlen(summary->chromosome) + strlen(outTag) + 20) );
#else
        tempOutFn = malloc(strlen(summary->chromosome) + strlen(outTag) + 20);
#endif
        if (! tempOutFn) {
            fprintf(stderr, "ERROR: Could not allocate space for temporary output filename. Could not merge.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
        sprintf(tempOutFn, "%s.%s.uncompressed.%04u", summary->chromosome, outTag, inRecIdx);
        tempOutFns[inRecIdx] = STARCH_strdup(tempOutFn);
        /* 
            Set up the out-bound file streams to allow reading and writing
            modes, which will allow extraction, rewinding and subsequent 
            parsing/reading.

            cf. http://pubs.opengroup.org/onlinepubs/007904975/functions/fopen.html 
        */
        tempOutFps[inRecIdx] = STARCH_fopen(tempOutFn, "a+b");

        /* cleanup */
        free(tempOutFn);
    }

    /* 
        Extract records to per-record pointers.
    */
    for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
        inRec = *(summary->records) + inRecIdx; /* records[inRecIdx] */
        tempOutFp = tempOutFps[inRecIdx];
#ifdef DEBUG
        fprintf(stderr, "\t\textracting %s from record %u (of %u) to temporary output file %s\n", summary->chromosome, (inRecIdx + 1), summary->numRecords, tempOutFns[inRecIdx]);
#endif
        switch (inRec->type) {
            case kBzip2: {
                UNSTARCH_extractDataWithBzip2( &(inRec->fp), 
                                               tempOutFp, 
                                               summary->chromosome, 
                                               inRec->metadata, 
                                               inRec->mdOffset, 
                                               inRec->hFlag );
                break;
            }
            case kGzip: {
                UNSTARCH_extractDataWithGzip( &(inRec->fp), 
                                              tempOutFp, 
                                              summary->chromosome, 
                                              inRec->metadata, 
                                              inRec->mdOffset, 
                                              inRec->hFlag );
                break;
            }
            case kUndefined: {
                fprintf(stderr, "ERROR: Input file uses undefined compression method. Could not merge.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
        };
    }

    /* 
        Set up merged output stream. Sorted results will be 
        directed here and then ultimately compressed.
    */
#ifdef __cplusplus
    uncomprOutFn = static_cast<char *>( malloc(strlen(summary->chromosome) + strlen(outTag) + 16) );
#else
    uncomprOutFn = malloc(strlen(summary->chromosome) + strlen(outTag) + 16);
#endif
    if (!uncomprOutFn) {
        fprintf(stderr, "ERROR: Could not allocate space for uncompressed output filename for final merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    sprintf(uncomprOutFn, "%s.%s.uncompressed", summary->chromosome, outTag);
    uncomprOutFnPtr = STARCH_fopen(uncomprOutFn, "wb");
    if (!uncomprOutFnPtr) {
        fprintf(stderr, "ERROR: Could not open an uncompressed output file handle to %s\n", uncomprOutFn);
        return STARCHCAT_EXIT_FAILURE;
    }

    /* 
        Read through records line by line, finding the 'lowest'
        start position (or any ties). If no ties are found, that 
        element if printed to the merged output stream. If ties are 
        found, then we search for the element from among these ties 
        which has the lowest stop position, which is then printed 
        to the merged output stream.
    */

#ifdef DEBUG
    fprintf(stderr, "\t\tparsing temporary per-record output files...\n");
#endif

#ifdef __cplusplus
    starts = static_cast<SignedCoordType *>( malloc(sizeof(SignedCoordType) * summary->numRecords) );
    stops = static_cast<SignedCoordType *>( malloc(sizeof(SignedCoordType) * summary->numRecords) );
    lowestStartIdxs = static_cast<int *>( malloc(sizeof(int) * summary->numRecords) );
    lineBuffers = static_cast<char **>( malloc(sizeof(char *) * summary->numRecords) );
#else
    starts = malloc(sizeof(SignedCoordType) * summary->numRecords);
    stops = malloc(sizeof(SignedCoordType) * summary->numRecords);
    lowestStartIdxs = malloc(sizeof(int) * summary->numRecords);
    lineBuffers = malloc(sizeof(char *) * summary->numRecords);
#endif

    if ((!starts) || (!stops) || (!lowestStartIdxs) || (!lineBuffers)) {
        fprintf(stderr, "ERROR: Could not allocate space for line buffer data. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
        /* 
            We re-use the file pointers we set up earlier. No sense
            in closing and reopening them.
        */
        rewind( tempOutFps[inRecIdx] );
        /* 
            We "prime" the line buffers in order to get an initial 
            comparison going in the first pass through the parse/sort
            loop.
        */
#ifdef __cplusplus
        lineBuffers[inRecIdx] = static_cast<char *>( malloc (STARCH_BUFFER_MAX_LENGTH) );
#else
        lineBuffers[inRecIdx] = malloc (STARCH_BUFFER_MAX_LENGTH);
#endif
        bufferIdx = 0U;
        while ( (c = fgetc(tempOutFps[inRecIdx])) ) {
            if (c == EOF) {
                fprintf(stderr, "ERROR: Could not retrieve data from archive (is it empty or corrupt?). Could not merge.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
#ifdef __cplusplus
            lineBuffers[inRecIdx][bufferIdx++] = static_cast<char>( c );
#else
            lineBuffers[inRecIdx][bufferIdx++] = (char) c;
#endif
            if (c == '\n') {
                lineBuffers[inRecIdx][(bufferIdx-1)] = '\0';
                break;
            }
        }
    }

    /*
        Do the final parsing and sorting of per-record data until all 
        the line buffers are empty (i.e., EOF is reached). 
    */
    do {
        /* 
            Reset bounds of lowest start and stop values. We are mostly assured 
            that BED data inputs will have smaller start and stop coordinates, 
            unless the inputs are corrupt or otherwise very, very unusual.
        */
#ifdef __cplusplus
        lowestStart = std::numeric_limits<int64_t>::max();
        lowestStop = std::numeric_limits<int64_t>::max();
#else
        lowestStart = INT64_MAX;
        lowestStop = INT64_MAX;
#endif
        /* 
            We parse the start and stop fields out of a buffered line 
            to retrieve their coordinates as sortable numerical values.
        */
        for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
            fieldIdx = 0U;
#ifdef __cplusplus
            if (lineBuffers[inRecIdx] == nullptr) 
                continue;
#else
            if (lineBuffers[inRecIdx] == NULL) 
                continue;
#endif
            for (bufferIdx = 0U, fieldBufferIdx = 0U; bufferIdx < strlen(lineBuffers[inRecIdx]); bufferIdx++, fieldBufferIdx++) {
                fieldBuffer[fieldBufferIdx] = lineBuffers[inRecIdx][bufferIdx];
                if (fieldBuffer[fieldBufferIdx] == '\t') {
                    fieldBuffer[fieldBufferIdx] = '\0';
#ifdef __cplusplus
                    fieldBufferIdx = static_cast<unsigned int>( -1 );
                    fieldIdx++;
                    if (fieldIdx == 2)
                        starts[inRecIdx] = static_cast<SignedCoordType>( strtoll(fieldBuffer, nullptr, 10) );
                    else if (fieldIdx == 3)
                        stops[inRecIdx] = static_cast<SignedCoordType>( strtoll(fieldBuffer, nullptr, 10) );
                    else if (fieldIdx > 3)
                        break;
#else
                    fieldBufferIdx = (unsigned int) -1;
                    fieldIdx++;
                    if (fieldIdx == 2)
                        starts[inRecIdx] = (SignedCoordType) strtoll(fieldBuffer, NULL, 10);
                    else if (fieldIdx == 3)
                        stops[inRecIdx] = (SignedCoordType) strtoll(fieldBuffer, NULL, 10);
                    else if (fieldIdx > 3)
                        break;
#endif            
                }
            }
        }
        /* 
            Find the lowest start index or indices (when ties occur) for
            line buffers which exist. If NULL, we skip over testing it.
        */
        lsiIdx = 0U;
        for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
#ifdef __cplusplus
            if (lineBuffers[inRecIdx] == nullptr) 
                continue;
#else
            if (lineBuffers[inRecIdx] == NULL) 
                continue;
#endif
            if (starts[inRecIdx] < lowestStart) {
                lowestStart = starts[inRecIdx];
                lsiIdx = 0U;
#ifdef __cplusplus
                lowestStartIdxs[lsiIdx++] = static_cast<int>( inRecIdx );
#else
                lowestStartIdxs[lsiIdx++] = (int) inRecIdx;
#endif
                if (lsiIdx < summary->numRecords)
                    lowestStartIdxs[lsiIdx] = -1;
            }
            else if (starts[inRecIdx] == lowestStart) {
#ifdef __cplusplus
                lowestStartIdxs[lsiIdx++] = static_cast<int>( inRecIdx );
#else
                lowestStartIdxs[lsiIdx++] = (int) inRecIdx;
#endif
                if (lsiIdx < summary->numRecords)
                    lowestStartIdxs[lsiIdx] = -1;
            }
        }
        /* 
            There is either one or multiple lowest-start indices. In the 
            case of one lowest-start index, we simply print that element to
            the (still-uncompressed) merged output.
        
            In the second case, where there are ties for the lowest start, 
            we must examine lowestStartIdxs[], looking for the index of 
            the lowest stop value. Once we find that, we have satisfied the
            BEDOPS lexicographical sort criteria and can print out that
            element to the merged output.

            In either case, once we print the contents of the line buffer, 
            we try to read characters into the line buffer from the 
            temporary file pointer until we hit a newline. We then repeat
            the parsing process. If there are no more characters (EOF), 
            we then mark the line buffer as NULL.

            Once all line buffers are NULL, we know there is nothing more
            to sort and so move on to compression and updating the metadata,
            along with finishing up any remaining cleanup duties.
        */
        if (lsiIdx == 1) {
#ifdef __cplusplus
            lsiIdx = static_cast<unsigned int>( lowestStartIdxs[(lsiIdx - 1)] );
#else
            lsiIdx = (unsigned int) lowestStartIdxs[(lsiIdx - 1)];
#endif
            fprintf(uncomprOutFnPtr, "%s\n", lineBuffers[lsiIdx]);
            /* 
                Retrieve another line or set lineBuffers element to NULL 
            */
            bufferIdx = 0U;
            while ( (c = fgetc(tempOutFps[lsiIdx])) ) {
                if (c == EOF) {
                    free(lineBuffers[lsiIdx]);
#ifdef __cplusplus
                    lineBuffers[lsiIdx] = nullptr;
#else
                    lineBuffers[lsiIdx] = NULL;
#endif
                    break;
                }
#ifdef __cplusplus
                lineBuffers[lsiIdx][bufferIdx++] = static_cast<char>( c );
#else
                lineBuffers[lsiIdx][bufferIdx++] = (char) c;
#endif
                if (c == '\n') {
                    lineBuffers[lsiIdx][(bufferIdx-1)] = '\0';
                    break;
                }
            }
        }
        else {
            for (lsiTestIdx = 0; lsiTestIdx < lsiIdx; lsiTestIdx++) {
                if (stops[lowestStartIdxs[lsiTestIdx]] < lowestStop) {
                    lowestStop = stops[lowestStartIdxs[lsiTestIdx]];
#ifdef __cplusplus
                    lowestStopIdx = static_cast<unsigned int>( lowestStartIdxs[lsiTestIdx] );
#else
                    lowestStopIdx = (unsigned int) lowestStartIdxs[lsiTestIdx];
#endif
                }
            }
            fprintf(uncomprOutFnPtr, "%s\n", lineBuffers[lowestStopIdx]);
            /* 
                Retrieve another line or set lineBuffers element to NULL 
            */
            bufferIdx = 0U;
            while ( (c = fgetc(tempOutFps[lowestStopIdx])) ) {
                if (c == EOF) {
                    free(lineBuffers[lowestStopIdx]);
#ifdef __cplusplus
                    lineBuffers[lowestStopIdx] = nullptr;
#else
                    lineBuffers[lowestStopIdx] = NULL;
#endif
                    break;
                }
#ifdef __cplusplus
                lineBuffers[lowestStopIdx][bufferIdx++] = static_cast<char>( c );
#else
                lineBuffers[lowestStopIdx][bufferIdx++] = (char) c;
#endif
                if (c == '\n') {
                    lineBuffers[lowestStopIdx][(bufferIdx-1)] = '\0';
                    break;
                }
            }
        }
        /* 
            Unless all buffers are empty, we keep sorting... 
        */
        allEOFFlag = 1;
        for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
#ifdef __cplusplus
            if (lineBuffers[inRecIdx] != nullptr) {
#else
            if (lineBuffers[inRecIdx] != NULL) {
#endif
                allEOFFlag = 0;
                break;
            }
        }
    } while (allEOFFlag != 1);

    /* cleanup */

#ifdef DEBUG
    fprintf(stderr, "\t\tdeleting temporary per-record output files...\n");
#endif

    for (inRecIdx = 0U; inRecIdx < summary->numRecords; inRecIdx++) {
        fclose(tempOutFps[inRecIdx]);
        if (remove(tempOutFns[inRecIdx]) != 0) {
            fprintf(stderr, "ERROR: Could not delete temporary output file. Could not merge.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
        free(tempOutFns[inRecIdx]);
    }
    if (tempOutFns)
        free(tempOutFns);
    if (tempOutFps)
        free(tempOutFps);
    if (uncomprOutFnPtr)
        fclose(uncomprOutFnPtr);
    if (starts)
        free(starts);
    if (stops)
        free(stops);
    if (lowestStartIdxs) 
        free(lowestStartIdxs);
    if (lineBuffers)
        free(lineBuffers);

    /* 
        We now transform and recompress extracted, headerless data with
        whatever specified output compression type, and we then update the
        output metadata.
    */
#ifdef DEBUG
    fprintf(stderr, "\t\ttransforming temporary merged output file...\n");
#endif

    transformFnPtr = STARCH_fopen(uncomprOutFn, "rb");
    if (!transformFnPtr) {
        fprintf(stderr, "ERROR: Could not open a transformation output file handle to %s . Could not merge.\n", uncomprOutFn);
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    (! *outMd) ? fprintf(stderr, "\t\tmaking new output metadata structure...\n") : fprintf(stderr, "\t\tappending to existing output metadata structure...\n");
#endif

#ifdef __cplusplus
    if ( STARCH_transformHeaderlessInput( &(*outMd), 
                                          reinterpret_cast<const FILE *>( transformFnPtr ), 
                                          static_cast<const CompressionType>( outType ), 
                                          reinterpret_cast<const char *>( outTag ), 
                                          static_cast<const Boolean>( kStarchFinalizeTransformFalse ), 
                                          reinterpret_cast<const char *>( note ) ) != 0 ) {
        fprintf(stderr, "ERROR: Could not transform output file. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
#else
    if ( STARCH_transformHeaderlessInput( &(*outMd), 
                                          (const FILE *) transformFnPtr, 
                                          (const CompressionType) outType, 
                                          (const char *) outTag, 
                                          (const Boolean) kStarchFinalizeTransformFalse, 
                                          (const char *) note ) != 0 ) {
        fprintf(stderr, "ERROR: Could not transform output file. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
#endif

#ifdef DEBUG
    fprintf(stderr, "\t\tchr: %s, outFn: %s, size: %" PRIu64 "\n", (*outMd)->chromosome, (*outMd)->filename, (*outMd)->size);
#endif

    fclose(transformFnPtr);
#ifdef __cplusplus
    transformFnPtr = nullptr;
#else
    transformFnPtr = NULL;
#endif

    /* 
        Delete uncompressed (untransformed) file as we no longer need it.
    */

#ifdef DEBUG
    fprintf(stderr, "\t\tdeleting temporary merged output file...\n");
#endif

    if ( remove(uncomprOutFn) != 0 ) {
        fprintf(stderr, "ERROR: Cannot remove uncompressed output file %s. Could not merge.\n", uncomprOutFn);
        return STARCHCAT_EXIT_FAILURE;
    }

    /* cleanup */
    if (uncomprOutFn)
        free(uncomprOutFn);

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_mergeChromosomeStreams(const ChromosomeSummaries *chrSums, const CompressionType outputType, const char *note) 
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_mergeChromosomeStreams() ---\n");
#endif
#ifdef __cplusplus
    ChromosomeSummary *summary = nullptr;
    MetadataRecord *inputRecord = nullptr;
    Metadata *headOutputMd = nullptr;
    Metadata *outputMd = nullptr;
    char *outputTag = nullptr;
    char *inputChr = nullptr;
    char *dynamicMdBuffer = nullptr;
#else
    ChromosomeSummary *summary = NULL;
    MetadataRecord *inputRecord = NULL;
    Metadata *headOutputMd = NULL;
    Metadata *outputMd = NULL;
    char *outputTag = NULL;
    char *inputChr = NULL;
    char *dynamicMdBuffer = NULL;
#endif
    unsigned int chrIdx = 0U;
    CompressionType inputType;
    Boolean firstOutputMd = kStarchTrue;
    Boolean hFlag = kStarchFalse; /* starchcat does not currently support headers */

    if (!chrSums) {
        fprintf(stderr, "ERROR: Chromosome summary is empty. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

    /*
        When we initially built the chromosome list, we applied a 
        lexicographical sort, so that we can process and export 
        streams in that order.

        We now loop through each chromosome of the summary 
        records, in order to determine how many files need 
        to be extracted to build the merged output stream.

        If there is only one source file associated with a given 
        chromosome, then we can simply copy the byte range over 
        (after calculating the relevant file offsets) but **only** 
        if the source compression type matches the output type **and**
        if the starch archive version is concurrent or newer than 
        the version of starchcat.
 
        Otherwise, we must extract to the output stream and recompress 
        it into the new output type.

        If there are two or more files associated with a chromosome,
        then we need to extract a BED line from each file pointer,
        compare BED coordinates from all source streams, and print the 
        next closest coordinate to the output stream.
    */

    STARCH_buildProcessIDTag( &outputTag );

    for (chrIdx = 0U; chrIdx < chrSums->numChromosomes; chrIdx++)
    {
        summary = chrSums->summary + chrIdx;

        if (summary->numRecords < 1) {
            /* If we get here, something went wrong with a data structure. */
            fprintf(stderr, "ERROR: Summaries pointer corrupt? Could not locate records in summaries.\n");
            return STARCHCAT_EXIT_FAILURE;
        }

        /*
            If there is only one record (and archive versions are concurrent), 
            compare output and input types and either copy bytes over directly, 
            or retransform data with new output type and/or newer version.
        */

        else if (summary->numRecords == 1) {
            inputRecord = *(summary->records);
            inputChr = summary->chromosome;
            inputType = inputRecord->type;
            if ((inputType == outputType) && (STARCHCAT_isArchiveConcurrent(inputRecord->av) == kStarchTrue)) {
#ifdef DEBUG
                fprintf(stderr, "\t[%s] copying bytes straight over...\n", summary->chromosome);
#endif
#ifdef __cplusplus
                assert( STARCHCAT_copyInputRecordToOutput( &outputMd, 
                                                           reinterpret_cast<const char *>( outputTag ), 
                                                           static_cast<const CompressionType>( outputType ), 
                                                           reinterpret_cast<const char *>( inputChr ), 
                                                           reinterpret_cast<const MetadataRecord *>( inputRecord )) );
#else
                assert( STARCHCAT_copyInputRecordToOutput( &outputMd, 
                                                           (const char *) outputTag, 
                                                           (const CompressionType) outputType, 
                                                           (const char *) inputChr, 
                                                           (const MetadataRecord *) inputRecord) );
#endif
            }
            else {
#ifdef DEBUG
                fprintf(stderr, "\t[%s] extracting bytes to recompressed stream...\n", summary->chromosome);
#endif
#ifdef __cplusplus
                assert( STARCHCAT_rewriteInputRecordToOutput( &outputMd, 
                                                              reinterpret_cast<const char *>( outputTag ), 
                                                              static_cast<const CompressionType>( outputType ), 
                                                              reinterpret_cast<const char *>( inputChr ), 
                                                              reinterpret_cast<const MetadataRecord *>( inputRecord )) );
#else
                assert( STARCHCAT_rewriteInputRecordToOutput( &outputMd, 
                                                              (const char *) outputTag, 
                                                              (const CompressionType) outputType, 
                                                              (const char *) inputChr, 
                                                              (const MetadataRecord *) inputRecord) );
#endif
            }
        }

        /* 
            In this case, we have a mix of records for a given chromosome. We walk through 
            input records line by line, returning the next lexicographically ordered BED
            element from the set of records. The final result is transformed and compressed, 
            and a new record added to the output metadata.
        */

        else {
#ifdef DEBUG
            fprintf(stderr, "\t[%s] unpacking/repacking mixed data...\n", summary->chromosome);
#endif
#ifdef __cplusplus
            assert( STARCHCAT_mergeInputRecordsToOutput(&outputMd, 
                            reinterpret_cast<const char *>( outputTag ), 
                                                        static_cast<const CompressionType>( outputType ), 
                                                        reinterpret_cast<const ChromosomeSummary *>( summary )) );
#else
            assert( STARCHCAT_mergeInputRecordsToOutput(&outputMd, 
                            (const char *) outputTag, 
                                                        (const CompressionType) outputType, 
                                                        (const ChromosomeSummary *) summary) );
#endif
        }

        if (!outputMd) {
            fprintf(stderr, "ERROR: Output metadata structure is empty after adding data. Something went wrong in mid-stream.\n");
            return STARCHCAT_EXIT_FAILURE;
        }

        /* 
            Grab a pointer to the head element of the output metadata. When we
            export the metadata to the final archive, we need to walk through the
            metadata from the first record.
        */

        if (firstOutputMd == kStarchTrue) {
            headOutputMd = outputMd; 
            firstOutputMd = kStarchFalse; 
        }
    }

    /* stitch up compressed files into one archive, along with metadata header */
#ifdef __cplusplus
    assert( STARCH_writeJSONMetadata( reinterpret_cast<const Metadata *>( headOutputMd ), 
                                      &dynamicMdBuffer, 
                                      const_cast<CompressionType *>( &outputType ), 
                                      static_cast<const Boolean>( hFlag ), 
                                      reinterpret_cast<const char *>( note )) );
    assert( STARCH_mergeMetadataWithCompressedFiles( reinterpret_cast<const Metadata *>( headOutputMd ), 
                             dynamicMdBuffer) );
    assert( STARCH_deleteCompressedFiles( reinterpret_cast<const Metadata *>( headOutputMd )) );
#else
    assert( STARCH_writeJSONMetadata( (const Metadata *) headOutputMd, 
                                      &dynamicMdBuffer, 
                                      (CompressionType *) &outputType, 
                                      (const Boolean) hFlag, 
                                      (const char *) note) );
    assert( STARCH_mergeMetadataWithCompressedFiles( (const Metadata *) headOutputMd, 
                             dynamicMdBuffer) );
    assert( STARCH_deleteCompressedFiles( (const Metadata *) headOutputMd) );
#endif

    /* cleanup */
    if (dynamicMdBuffer)
        free(dynamicMdBuffer);

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_freeChromosomeNames(char ***chrs, unsigned int numChromosomes)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_freeChromosomeNames() ---\n");
#endif
    unsigned int chrIdx = 0U;

    if (! *chrs)
        return STARCHCAT_EXIT_FAILURE;

    for (chrIdx = 0U; chrIdx < numChromosomes; chrIdx++) {
#ifdef DEBUG
        fprintf(stderr, "\tfreeing chromosome name [%s]\n", *(*chrs + chrIdx));
#endif
        free( *(*chrs + chrIdx) );
    }

    free(*chrs);
#ifdef __cplusplus
    *chrs = nullptr;
#else
    *chrs = NULL;
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_freeChromosomeSummaries (ChromosomeSummaries **chrSums)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT_freeChromosomeSummaries() ---\n");
#endif

    if (! *chrSums)
        return STARCHCAT_EXIT_SUCCESS;

#ifdef __cplusplus
    STARCHCAT_freeChromosomeSummary( &((*chrSums)->summary), static_cast<const unsigned int>( (*chrSums)->numChromosomes ));
#else
    STARCHCAT_freeChromosomeSummary( &((*chrSums)->summary), (const unsigned int) (*chrSums)->numChromosomes );
#endif
    free(*chrSums);
#ifdef __cplusplus
    *chrSums = nullptr;
#else
    *chrSums = NULL;
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_allocChromosomeSummaries(ChromosomeSummaries **chrSums, const unsigned int numChromosomes)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT_allocChromosomeSummaries() ---\n");
#endif

    if (numChromosomes == 0)
        return STARCHCAT_EXIT_SUCCESS;

#ifdef __cplusplus
    *chrSums = static_cast<ChromosomeSummaries *>( malloc( sizeof(ChromosomeSummaries) * numChromosomes ) );
    if (*chrSums == nullptr) {
        fprintf(stderr, "ERROR: Could not allocate space for chromosome summaries.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    (*chrSums)->summary = nullptr;
#else
    *chrSums = malloc( sizeof(ChromosomeSummaries) * numChromosomes );
    if (*chrSums == NULL) {
        fprintf(stderr, "ERROR: Could not allocate space for chromosome summaries.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    (*chrSums)->summary = NULL;
#endif

    (*chrSums)->numChromosomes = 0U;

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_buildChromosomeSummaries(ChromosomeSummaries **chrSums, const ChromosomeSummary *summary, const unsigned int numChromosomes)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT_buildChromosomeSummaries() ---\n");
#endif

    if ((! *chrSums) || (!summary) || (numChromosomes == 0)) {
        fprintf(stderr, "ERROR: Could not build chromosome summaries.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    (*chrSums)->summary = const_cast<ChromosomeSummary *>( summary );
#else
    (*chrSums)->summary = (ChromosomeSummary *) summary;
#endif
    (*chrSums)->numChromosomes = numChromosomes;

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_printChromosomeSummaries(const ChromosomeSummaries *chrSums)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_printChromosomeSummaries() ---\n");
#endif
#ifdef __cplusplus
    ChromosomeSummary *summary = nullptr;
    MetadataRecord **recs = nullptr;
    MetadataRecord *mdRecord = nullptr;
    Metadata *iter, *md = nullptr;
#else
    ChromosomeSummary *summary = NULL;
    MetadataRecord **recs = NULL;
    MetadataRecord *mdRecord = NULL;
    Metadata *iter, *md = NULL;
#endif
    unsigned int chrIdx = 0U;
    unsigned int recIdx = 0U;

    if (!chrSums)
        return STARCHCAT_EXIT_FAILURE;

    for (chrIdx = 0U; chrIdx < chrSums->numChromosomes; chrIdx++) {
        summary = chrSums->summary + chrIdx;
        recs = summary->records;
#ifdef DEBUG
        fprintf(stderr, "\t[chrIdx: %02u]\n\t\tchromosome: %s\n\t\tnumRecords: %u\n", chrIdx, summary->chromosome, summary->numRecords);
#endif
        for (recIdx = 0U; recIdx < summary->numRecords; recIdx++) {
            mdRecord = recs[recIdx];
#ifdef __cplusplus
            md = static_cast<Metadata *>( mdRecord->metadata );
            /* loop through Metadata * to look for matching chromosome */
            for (iter = md; iter != nullptr; iter = iter->next) {
#else
            md = (Metadata *) mdRecord->metadata;
            /* loop through Metadata * to look for matching chromosome */
            for (iter = md; iter != NULL; iter = iter->next) {
#endif
#ifdef DEBUG
                fprintf(stderr, "COMPARING [%s] chromosome iter [%s] vs summary [%s]\n", mdRecord->filename, iter->chromosome, summary->chromosome);
#endif
                if (strcmp(iter->chromosome, summary->chromosome) == 0) {
#ifdef DEBUG
#ifdef __cplusplus
                    fprintf(stderr, "\t\t\t[recIdx: %02u]\n" \
                            "\t\t\t\tfilename: %s\n" \
                            "\t\t\t\theaderFlag: %u\n" \
                            "\t\t\t\tmdOffset: %" PRIu64 "\n" \
                            "\t\t\t\ttype: %u\n" \
                            "\t\t\t\tintChr: %s\n" \
                            "\t\t\t\tintFn: %s\n" \
                            "\t\t\t\tintSize: %" PRIu64 "\n" \
                            "\t\t\t\tintLineCount: %" PRId64 "\n", \
                            recIdx, 
                mdRecord->filename, 
                mdRecord->hFlag, 
                static_cast<uint64_t>( mdRecord->mdOffset ), 
                mdRecord->type, 
                iter->chromosome, 
                iter->filename, 
                static_cast<uint64_t>( iter->size ), 
                static_cast<uint64_t>( iter->lineCount ));
#else
                    fprintf(stderr, "\t\t\t[recIdx: %02u]\n" \
                            "\t\t\t\tfilename: %s\n" \
                            "\t\t\t\theaderFlag: %u\n" \
                            "\t\t\t\tmdOffset: %" PRIu64 "\n" \
                            "\t\t\t\ttype: %u\n" \
                            "\t\t\t\tintChr: %s\n" \
                            "\t\t\t\tintFn: %s\n" \
                            "\t\t\t\tintSize: %" PRIu64 "\n" \
                            "\t\t\t\tintLineCount: %" PRId64 "\n", \
                            recIdx, 
                mdRecord->filename, 
                mdRecord->hFlag, 
                (uint64_t) mdRecord->mdOffset, 
                mdRecord->type, 
                iter->chromosome, 
                iter->filename, 
                (uint64_t) iter->size, 
                (uint64_t) iter->lineCount);
#endif
#endif
                    break;
                }
            }
        }
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_freeChromosomeSummary(ChromosomeSummary **summary, const unsigned int numChromosomes)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_freeChromosomeSummary() ---\n");
#endif
#ifdef __cplusplus
    ChromosomeSummary *summaryInstance = nullptr;

    if (*summary == nullptr)
        return STARCHCAT_EXIT_SUCCESS;
#else
    ChromosomeSummary *summaryInstance = NULL;

    if (*summary == NULL)
        return STARCHCAT_EXIT_SUCCESS;
#endif
    unsigned int chrIdx = 0U;

    for (chrIdx = 0U; chrIdx < numChromosomes; chrIdx++) {
        summaryInstance = *summary + chrIdx;
#ifdef DEBUG
        fprintf(stderr, "\tfreeing chromosome [%s] %u (of %u)...\n", summaryInstance->chromosome, (chrIdx + 1), numChromosomes);
#endif
        if (summaryInstance->chromosome) {
            free( summaryInstance->chromosome );
#ifdef __cplusplus
            summaryInstance->chromosome = nullptr;
#else
            summaryInstance->chromosome = NULL;
#endif
        }
        if (summaryInstance->records)
            if ( STARCHCAT_freeMetadataRecords(summaryInstance->records, summaryInstance->numRecords) != STARCHCAT_EXIT_SUCCESS )
                return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_allocChromosomeSummary(ChromosomeSummary **summary, const unsigned int numChromosomes)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_allocChromosomeSummary() ---\n");
#endif
#ifdef __cplusplus
    ChromosomeSummary *instance = nullptr;
#else
    ChromosomeSummary *instance = NULL;
#endif
    unsigned int chrIdx = 0U;

#ifdef __cplusplus
    if (*summary != nullptr) {
#else
    if (*summary != NULL) {
#endif
        fprintf(stderr, "ERROR: Chromosome summary is not empty.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    fprintf(stderr, "\tallocating space for %u chromosomes...\n", numChromosomes);
#endif

#ifdef __cplusplus
    *summary = static_cast<ChromosomeSummary *>( malloc( sizeof(ChromosomeSummary) * numChromosomes ) );
    if (*summary == nullptr) {
        fprintf(stderr, "ERROR: Could not allocate space for chromosome summary.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    for (chrIdx = 0U; chrIdx < numChromosomes; chrIdx++) {
        instance = *summary + chrIdx;
        instance->chromosome = nullptr;
        instance->records = nullptr;
        instance->numRecords = 0U;
    }
#else
    *summary = malloc( sizeof(ChromosomeSummary) * numChromosomes );
    if (*summary == NULL) {
        fprintf(stderr, "ERROR: Could not allocate space for chromosome summary.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    for (chrIdx = 0U; chrIdx < numChromosomes; chrIdx++) {
        instance = *summary + chrIdx;
        instance->chromosome = NULL;
        instance->records = NULL;
        instance->numRecords = 0U;
    }
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_buildChromosomeSummary(ChromosomeSummary **summary, const MetadataRecord *mdRecords, const unsigned int numRecords, const char **chromosomes, const unsigned int numChromosomes) 
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_buildChromosomeSummary() ---\n");
#endif
#ifdef __cplusplus
    Metadata *iter, *md = nullptr;
#else
    Metadata *iter, *md = NULL;
#endif
    unsigned int numHits;
    unsigned int chrIdx;
    unsigned int recIdx;
    unsigned int hitIdx;
    ChromosomeSummary *instance;

    for (chrIdx = 0U; chrIdx < numChromosomes; chrIdx++) {
        numHits = 0U;
        instance = *summary + chrIdx;
#ifdef DEBUG
        fprintf(stderr, "\tworking on chr %u (of %u) [%s]...\n", (chrIdx + 1), numChromosomes, *(chromosomes + chrIdx));
#endif
        instance->chromosome = STARCH_strdup( *(chromosomes + chrIdx) );
        for (recIdx = 0U; recIdx < numRecords; recIdx++) {
            md = (mdRecords + recIdx)->metadata;
#ifdef __cplusplus
            for (iter = md; iter != nullptr; iter = iter->next) {
#else
            for (iter = md; iter != NULL; iter = iter->next) {
#endif
#ifdef DEBUG
                fprintf(stderr, "\tcomparing [%s] against [record %u | %s]...\n", instance->chromosome, recIdx, iter->chromosome);
#endif
                if (strcmp((*summary + chrIdx)->chromosome, iter->chromosome) == 0) {
                    numHits++;
                    break;
                }
            }
        }

#ifdef DEBUG
        fprintf(stderr, "\t---\n\tfor [%s], we find %u hits\n", (*summary + chrIdx)->chromosome, numHits);
        fprintf(stderr, "\tallocating %u records for instance %u...\n", numHits, chrIdx);
#endif

#ifdef __cplusplus
        instance->records = static_cast<MetadataRecord **>( malloc(sizeof(MetadataRecord *) * numHits) );
#else
        instance->records = malloc(sizeof(MetadataRecord *) * numHits);
#endif
        instance->numRecords = numHits;

#ifdef __cplusplus
        if (instance->records != nullptr) {
#else
        if (instance->records != NULL) {
#endif
            for (recIdx = 0U, hitIdx = 0U; recIdx < numRecords; recIdx++) {
                md = (mdRecords + recIdx)->metadata;
#ifdef __cplusplus
                for (iter = md; iter != nullptr; iter = iter->next)
#else
                for (iter = md; iter != NULL; iter = iter->next)
#endif
                    if (strcmp(instance->chromosome, iter->chromosome) == 0) {
#ifdef DEBUG
                        fprintf(stderr, "\treferencing metadata record idx %u -> hit idx %u\n", recIdx, hitIdx);
#endif
#ifdef __cplusplus
                        *(instance->records + hitIdx) = const_cast<MetadataRecord *>( mdRecords + recIdx );
#else
                        *(instance->records + hitIdx) = (MetadataRecord *) (mdRecords + recIdx);
#endif
#ifdef DEBUG
                        fprintf(stderr, "\tfilename in ref is: %s\n", (*(instance->records + hitIdx))->filename);
#endif
                        hitIdx++;
                    }
            }
        }
        else {
            fprintf(stderr, "ERROR: Could not allocate space for chromosome summary records.\n");
            return STARCHCAT_EXIT_FAILURE;
        }
    }

    return STARCHCAT_EXIT_SUCCESS;
}

MetadataRecord *
STARCHCAT_copyMetadataRecord(const MetadataRecord *mdRec) 
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_copyMetadataRecord() ---\n");
#endif
#ifdef __cplusplus
    MetadataRecord *copy = nullptr;
#else
    MetadataRecord *copy = NULL;
#endif

#ifdef __cplusplus
    copy = static_cast<MetadataRecord *>( malloc(sizeof(MetadataRecord)) );
#else
    copy = malloc(sizeof(MetadataRecord));
#endif

    copy->metadata = STARCH_copyMetadata(mdRec->metadata);
#ifdef DEBUG
    fprintf(stderr, "\tcopying filename from mdRec to copy: %s\n", mdRec->filename);
#endif
    copy->filename = STARCH_strdup(mdRec->filename);
    copy->fp = mdRec->fp;
    copy->type = mdRec->type;
    copy->hFlag = mdRec->hFlag;

    return copy;
}

int
STARCHCAT_buildUniqueChromosomeList(char ***chromosomes, unsigned int *numChr, const MetadataRecord *mdRecords, const unsigned int numRecords)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_buildUniqueChromosomeList() ---\n");
#endif
#ifdef __cplusplus
    Metadata *iter, *md = nullptr;
    char **chr = nullptr;
    char **chrCopy = nullptr;
    char **sortedChr = nullptr;
    char **sortedUniqChr = nullptr;
#else
    Metadata *iter, *md = NULL;
    char **chr = NULL;
    char **chrCopy = NULL;
    char **sortedChr = NULL;
    char **sortedUniqChr = NULL;
#endif
    unsigned int idx = 0U;
    size_t uninitIdx = 0U;
    size_t totalChr = 1;
    size_t currChr = 1;
    unsigned int allSortedChr = 0;

#ifdef DEBUG
    fprintf (stderr, "\tbuild temporary chromosome list\n");
#endif
    for (idx = 0U; idx < numRecords; idx++) {
        md = (mdRecords + idx)->metadata;
#ifdef DEBUG
        fprintf(stderr, "\t%s\n", (mdRecords + idx)->filename);
#endif
#ifdef __cplusplus
        for (iter = md; iter != nullptr; iter = iter->next, currChr++) {
#else
        for (iter = md; iter != NULL; iter = iter->next, currChr++) {
#endif
            if (currChr == totalChr) {
                totalChr *= 2;
#ifdef __cplusplus
                chrCopy = static_cast<char **>( realloc ( chr, totalChr * sizeof(*chrCopy) + 1 ) );
#else
                chrCopy = realloc ( chr, totalChr * sizeof(*chrCopy) + 1 );
#endif
                if (! chrCopy) {
                    fprintf(stderr, "ERROR: Could not allocate space for chromosome list.\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
                chr = chrCopy;
                for (uninitIdx = totalChr - 1; uninitIdx >= currChr; uninitIdx--) {
#ifdef __cplusplus
                    *(chr + uninitIdx) = nullptr;
#else
                    *(chr + uninitIdx) = NULL;
#endif
		}
            }
#ifdef DEBUG
            fprintf(stderr, "\t\t(%zu | %zu) %-25s\n", totalChr, currChr, iter->chromosome);
#endif
            *(chr + currChr - 1) = STARCH_strdup(iter->chromosome);
        }
    }

    /* copy references to items over to resized array */
    currChr--;
#ifdef __cplusplus
    sortedChr = static_cast<char **>( malloc (currChr * sizeof(char *)) );
#else
    sortedChr = malloc (currChr * sizeof(char *));
#endif
    if (! sortedChr) {
        fprintf(stderr, "ERROR: Could not allocate space for sorted chromosome list.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    for (idx = 0U; idx < currChr; idx++)
        *(sortedChr + idx) = *(chr + idx);

    /* lexicographically-sort chr array */
    qsort(sortedChr, currChr, sizeof(char *), STARCHCAT_compareCStrings);

    /* find count of sorted-chr uniques */
    for (idx = 0U; idx < currChr - 1; idx++) 
        if ( strcmp(*(sortedChr + idx), *(sortedChr + idx + 1)) != 0 ) 
            allSortedChr++;
    allSortedChr++; /* take into account last element, which is unique */

    /* duplicate unique elements to sorted, unique string array */
#ifdef __cplusplus
    sortedUniqChr = static_cast<char **>( malloc (sizeof(char *) * allSortedChr) );
#else
    sortedUniqChr = malloc (sizeof(char *) * allSortedChr);
#endif
    if (! sortedUniqChr) {
        fprintf(stderr, "ERROR: Could not allocate space for sorted, unique chromosome list.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    for (idx = 0U, uninitIdx = 0U; idx < currChr - 1; idx++) {
        if ( strcmp(*(sortedChr + idx), *(sortedChr + idx + 1)) != 0 )
            *(sortedUniqChr + uninitIdx++) = STARCH_strdup(*(sortedChr + idx));
    }
    *(sortedUniqChr + uninitIdx) = STARCH_strdup(*(sortedChr + idx)); /* copy last remaining element in sorted array */

    *chromosomes = sortedUniqChr;
    *numChr = allSortedChr;

    /* delete temporary chromosome list */
#ifdef __cplusplus
    if (chr != nullptr) {
#else
    if (chr != NULL) {
#endif
        for (idx = 0U; idx < currChr; idx++) {
            if (*(chr + idx)) {
#ifdef DEBUG
                fprintf(stderr, "\tfreeing temporary chromosome... %s\n", *(chr + idx));
#endif
                free( *(chr + idx) );
#ifdef __cplusplus
                *(chr + idx) = nullptr;
#else
                *(chr + idx) = NULL;
#endif
            }
        }
        free(chr);
#ifdef __cplusplus
        chr = nullptr;
#else
        chr = NULL;
#endif
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_compareCStrings(const void *a, const void *b)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_compareCStrings() ---\n");
#endif
#ifdef __cplusplus
    char *ca = reinterpret_cast<char *>(const_cast<void *>(a));
    char *cb = reinterpret_cast<char *>(const_cast<void *>(b));

    return strcmp(*(reinterpret_cast<const char **>(ca)), *(reinterpret_cast<const char **>(cb)));
#else
    return strcmp(*(const char **)a, *(const char **)b);
#endif
}

int
STARCHCAT_allocMetadataRecords(MetadataRecord **mdRecords, const unsigned int numRecords)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_allocMetadataRecords() ---\n");
#endif
#ifdef __cplusplus
    MetadataRecord *instance = nullptr;
#else
    MetadataRecord *instance = NULL;
#endif
    unsigned int recIdx = 0U;

#ifdef __cplusplus
    if (*mdRecords != nullptr) {
#else
    if (*mdRecords != NULL) {
#endif
        fprintf(stderr, "ERROR: Metadata records are not empty.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    fprintf(stderr, "\tallocating space for %u records...\n", numRecords);
#endif

#ifdef __cplusplus
    *mdRecords = static_cast<MetadataRecord *>( malloc( sizeof(MetadataRecord) * numRecords ) );
#else
    *mdRecords = malloc( sizeof(MetadataRecord) * numRecords );
#endif

#ifdef __cplusplus
    if (*mdRecords == nullptr) {
#else
    if (*mdRecords == NULL) {
#endif
        fprintf(stderr, "ERROR: Could not allocate space for metadata records.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    
    for (recIdx = 0U; recIdx < numRecords; recIdx++) {
        instance = *mdRecords + recIdx;
#ifdef __cplusplus
        instance->metadata = nullptr;
        instance->filename = nullptr;
        instance->fp = nullptr;
        instance->av = nullptr;
#else
        instance->metadata = NULL;
        instance->filename = NULL;
        instance->fp = NULL;
        instance->av = NULL;
#endif
        instance->type = STARCH_DEFAULT_COMPRESSION_TYPE;
        instance->hFlag = kStarchFalse;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_freeMetadataRecords(MetadataRecord **mdRecords, const unsigned int numRecords)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_freeMetadataRecords() ---\n");
#endif
#ifdef __cplusplus
    MetadataRecord *instance = nullptr;
#else
    MetadataRecord *instance = NULL;
#endif
    unsigned int recIdx = 0U;

    if (! *mdRecords) {
        return STARCHCAT_EXIT_SUCCESS;
    }

    for (recIdx = 0U; recIdx < numRecords; recIdx++) {
#ifdef DEBUG
        fprintf(stderr, "\tattempting to free record %u (of %u)...\n", (recIdx + 1), numRecords);
#endif
        if (mdRecords[recIdx]) {
            instance = *mdRecords + recIdx;
            if (instance->filename) {
                free(instance->filename);
#ifdef __cplusplus
                instance->filename = nullptr;
#else
                instance->filename = NULL;
#endif
            }
            if (instance->metadata) {
                STARCH_freeMetadata( &(instance->metadata) );
#ifdef __cplusplus
                instance->metadata = nullptr;
#else
                instance->metadata = NULL;
#endif
            }
            if (instance->fp) {
                fclose(instance->fp);
#ifdef __cplusplus
                instance->fp = nullptr;
#else
                instance->fp = NULL;
#endif
            }
            if (instance->av) {
                free(instance->av);
#ifdef __cplusplus
                instance->av = nullptr;
#else
                instance->av = NULL;
#endif
            }
            if (instance->cTime) {
                free(instance->cTime);
#ifdef __cplusplus
                instance->cTime = nullptr;
#else
                instance->cTime = NULL;
#endif
            }
        }
    }

#ifdef DEBUG
    fprintf(stderr, "\t...returning...\n");
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT_buildMetadataRecords(json_t ***metadataJSONs, MetadataRecord **mdRecords, const unsigned int firstArgc, const int argc, const char **argv)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_buildMetadataRecords() ---\n");
#endif
#ifdef __cplusplus
    FILE *inFilePtr = nullptr;
    char *inFile = nullptr;
    Metadata *md = nullptr;
    ArchiveVersion *version = nullptr;
    char *creationTimestamp = nullptr;
    char *cTime = nullptr;
    char *note = nullptr;
    MetadataRecord *mdRecord = nullptr;
    json_t *mdJSON = nullptr;
    struct tm *creationTimeInformation = nullptr;
#else
    FILE *inFilePtr = NULL;
    char *inFile = NULL;
    Metadata *md = NULL;
    ArchiveVersion *version = NULL;
    char *creationTimestamp = NULL;
    char *cTime = NULL;
    char *note = NULL;
    MetadataRecord *mdRecord = NULL;
    json_t *mdJSON = NULL;
    struct tm *creationTimeInformation = NULL;
#endif
    unsigned int idx = 0U;
    unsigned int realIdx = 0U;
    CompressionType inType = STARCH_DEFAULT_COMPRESSION_TYPE;
    uint64_t mdOffset = 0;
    Boolean hFlag = kStarchFalse;
    const Boolean suppressErrorMsgs = kStarchFalse; /* i.e., we want to see error messages */
    const Boolean preserveJSONRef = kStarchTrue;
    time_t creationTime;
    size_t creationTimestampLength = STARCH_CREATION_TIMESTAMP_LENGTH;
    unsigned int numGzipRecs = 0U;

#ifdef __cplusplus
    for (idx = firstArgc, realIdx = 0U; idx < static_cast<unsigned int>( argc ); idx++, realIdx++) 
#else
    for (idx = firstArgc, realIdx = 0U; idx < (unsigned int) argc; idx++, realIdx++) 
#endif
    {
        mdRecord = (*mdRecords + realIdx);

        /* reset */
#ifdef __cplusplus
        inFilePtr = nullptr;
        inFile = const_cast<char *>( argv[idx] );
        md = nullptr;
        version = nullptr;
#else
        inFilePtr = NULL;
        inFile = (char *) argv[idx];
        md = NULL;
        version = NULL;
#endif
        inType = STARCH_DEFAULT_COMPRESSION_TYPE;
        hFlag = kStarchFalse;

        /* debug */
#ifdef DEBUG
        fprintf(stderr, "\tidx/realIdx: %d/%d\n", idx, realIdx);
        fprintf(stderr, "\topening file: %s\n", inFile);
#endif

        /* parse the archive file's metadata */
        if ( STARCH_readJSONMetadata( &mdJSON, 
                      &inFilePtr, 
#ifdef __cplusplus
                      reinterpret_cast<const char *>( inFile ),
#else
                      (const char *) inFile, 
#endif
                      &md, 
                      &inType, 
                      &version, 
                      &cTime, 
                      &note, 
                      &mdOffset, 
                      &hFlag, 
                      suppressErrorMsgs, 
                      preserveJSONRef) != STARCH_EXIT_SUCCESS ) {
            fprintf(stderr, "ERROR: Could not read metadata from archive: %s (is this a starch file?)\n", inFile);
            return STARCHCAT_EXIT_FAILURE;
        }

        /* set metadata JSON object reference */
        if (!mdJSON) {
            fprintf(stderr, "ERROR: JSON object is empty -- something went wrong with extraction\n");
            return STARCHCAT_EXIT_FAILURE;
        }
        *(*metadataJSONs + realIdx) = mdJSON;

        /* debug */

#ifdef DEBUG
        switch (inType) {
            case kBzip2: {
                fprintf(stderr, "\ttype: bzip2\n");
                break;
            }
            case kGzip: {
                fprintf(stderr, "\ttype: gzip\n");
                break;
            }
            case kUndefined: {
                fprintf(stderr, "ERROR: Undefined compression type in archive header.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
        }
#endif

        if (hFlag == kStarchTrue) {
            fprintf(stderr, "ERROR: This application does not currently support starch archives with header information.\n");
            return STARCHCAT_EXIT_FAILURE;
        }

        /* assignment */
        mdRecord->filename = STARCH_strdup(inFile);
#ifdef __cplusplus
        mdRecord->metadata = STARCH_copyMetadata(reinterpret_cast<const Metadata *>( md ));
#else
        mdRecord->metadata = STARCH_copyMetadata((const Metadata *) md);
#endif
        mdRecord->fp = inFilePtr;
        mdRecord->type = inType;
        mdRecord->hFlag = hFlag;
        mdRecord->mdOffset = mdOffset;
#ifdef __cplusplus
        mdRecord->av = STARCH_copyArchiveVersion(reinterpret_cast<const ArchiveVersion *>( version ));
#else
        mdRecord->av = STARCH_copyArchiveVersion((const ArchiveVersion *) version);
#endif

        if (!cTime) {
            time(&creationTime);
            creationTimeInformation = localtime(&creationTime);

#ifdef __cplusplus
            creationTimestamp = static_cast<char *>( malloc(creationTimestampLength) );
#else
            creationTimestamp = (char *) malloc(creationTimestampLength);
#endif

            if (!creationTimestamp) {
                fprintf(stderr, "[unstarch] - Error: Could not instantiate temporary creation timestamp string\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            strftime(creationTimestamp, creationTimestampLength, "%Y-%m-%dT%H:%M:%S%z", creationTimeInformation);

#ifdef __cplusplus
            cTime = static_cast<char *>( malloc(strlen(creationTimestamp) + 1) );
#else
            cTime = malloc(strlen(creationTimestamp) + 1);
#endif

            if (!cTime) {
                fprintf(stderr, "[unstarch] - Error: Could not instantiate creation timestamp string\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            strncpy(cTime, creationTimestamp, strlen(creationTimestamp) + 1);
            free(creationTimestamp);
#ifdef __cplusplus
            creationTimestamp = nullptr;
#else
            creationTimestamp = NULL;
#endif
        }
        mdRecord->cTime = cTime;

        if (inType == kGzip) {
            if (mdRecord->av->major == 1)
                numGzipRecs++;
            /*
            if (numGzipRecs >= 2) {
                fprintf(stderr, "ERROR: At this time, starchcat cannot stitch two or more gzip-backed v1.x archives.\n\tPlease use starchcat to upgrade each gzip-backed v1.x archive individually,\n\tand then restitch the resulting updated archives. This feature will be\n\tadded in a later update.\n");            
                return STARCHCAT_EXIT_FAILURE;
            }
            */
        }

#ifdef DEBUG
        fprintf(stderr, "\t---\n");
#endif

        /* check archive version concurrency */
#ifdef __cplusplus
        if (STARCHCAT_isArchiveNewer(reinterpret_cast<const ArchiveVersion *>( mdRecord->av )) == kStarchTrue) {
#else
        if (STARCHCAT_isArchiveNewer((const ArchiveVersion *) mdRecord->av) == kStarchTrue) {
#endif
            fprintf(stderr, "ERROR: Archive [%s] is newer than this binary [archive: %d.%d.%d; binary: %d.%d.%d]. Please update your toolkit to add support for this archive.\n",
                            mdRecord->filename,
                            mdRecord->av->major,
                            mdRecord->av->minor,
                            mdRecord->av->revision,
                            STARCH_MAJOR_VERSION,
                            STARCH_MINOR_VERSION,
                            STARCH_REVISION_VERSION);
            return STARCHCAT_EXIT_FAILURE;
        }

        /* cleanup */
#ifdef __cplusplus
        if (md) {
            STARCH_freeMetadata(&md);
            md = nullptr;
        }
        if (version) {
            free(version);
            version = nullptr;
        }
#else
        if (md) {
            STARCH_freeMetadata(&md);
            md = NULL;
        }
        if (version) {
            free(version);
            version = NULL;
        }
#endif
    }

    return STARCHCAT_EXIT_SUCCESS;
}

Boolean
STARCHCAT_fileExists(const char *fn)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_fileExists() ---\n");
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

void
STARCHCAT_printUsage(int errorType)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_printUsage() ---\n");
#endif
#ifdef __cplusplus
    char *avStr = nullptr;
#else
    char *avStr = NULL;
#endif

#ifdef __cplusplus
    avStr = static_cast<char *>( malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH) );
#else
    avStr = malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH);
#endif

    if (avStr) {
        int result = sprintf (avStr, "%d.%d.%d", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        if (result != -1) {
            switch (errorType) {
                case STARCHCAT_VERSION_ERROR:
                    fprintf(stdout, "%s\n  binary version: %s (creates archive version: %s)\n", name, BEDOPS::version(), avStr);
                    break;
                case STARCHCAT_FATAL_ERROR:
                case STARCHCAT_HELP_ERROR:
                default:
                    fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n", name, BEDOPS::citation(), BEDOPS::version(), authors, usage);
                    break;
            }
        }
        free(avStr);
    }
}

void
STARCHCAT_printRevision()
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_printRevision() ---\n");
#endif
#ifdef __cplusplus
    char *avStr = nullptr;
#else
    char *avStr = NULL;
#endif

#ifdef __cplusplus
    avStr = static_cast<char *>( malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH) );
#else
    avStr = malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH);
#endif

#ifdef __cplusplus
    if (avStr != nullptr) {
#else
    if (avStr != NULL) {
#endif
        int result = sprintf (avStr, "%d.%d.%d", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        if (result != -1) {
            fprintf(stderr, "%s\n  binary version: %s (creates archive version: %s)\n", name, BEDOPS::version(), avStr);
            free(avStr);
        }
    }
}

Boolean
STARCHCAT_isArchiveConcurrent(const ArchiveVersion *av)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_isArchiveConcurrent() ---\n");
#endif

    if ((av->major == STARCH_MAJOR_VERSION) && 
        (av->minor == STARCH_MINOR_VERSION) && 
        (av->revision == STARCH_REVISION_VERSION))
        return kStarchTrue;

    return kStarchFalse;
}

Boolean
STARCHCAT_isArchiveNewerThan(const ArchiveVersion *av, const ArchiveVersion *comp)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_isArchiveNewerThan() ---\n");
#endif

    if (   (av->major > comp->major) 
         ||
         ( (av->major == comp->major) &&
           (av->minor > comp->minor) ) 
         ||
           ((av->major == comp->major) &&
           (av->minor == comp->minor) &&
           (av->revision > comp->revision) ) 
       )
        return kStarchTrue;

    return kStarchFalse;
}

Boolean
STARCHCAT_isArchiveConcurrentOrOlder(const ArchiveVersion *av)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_isArchiveConcurrentOrOlder() ---\n");
#endif

    if ( ( (av->major < STARCH_MAJOR_VERSION) ) ||
         ( (av->major == STARCH_MAJOR_VERSION)  && 
           (av->minor <= STARCH_MINOR_VERSION)  && 
           (av->revision <= STARCH_REVISION_VERSION) )
       )
        return kStarchTrue;

    return kStarchFalse;
}

Boolean
STARCHCAT_isArchiveOlder(const ArchiveVersion *av)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_isArchiveOlder() ---\n");
#endif

    if ( ( (av->major < STARCH_MAJOR_VERSION) ) ||
         ( (av->major == STARCH_MAJOR_VERSION)  && 
           (av->minor < STARCH_MINOR_VERSION)  && 
           (av->revision <= STARCH_REVISION_VERSION) )
       )
        return kStarchTrue;

    return kStarchFalse;
}

Boolean
STARCHCAT_isArchiveNewer(const ArchiveVersion *av)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCHCAT_isArchiveNewer() ---\n");
    fprintf (stderr, "\tcurrent archive version: %d.%d.%d\n", av->major, av->minor, av->revision);
#endif

    if (((av->major == STARCH_MAJOR_VERSION) && (av->minor > STARCH_MINOR_VERSION) && (av->revision >= STARCH_REVISION_VERSION)) ||
         (av->major > STARCH_MAJOR_VERSION))
        return kStarchTrue;

    return kStarchFalse;
}

Boolean
STARCHCAT_allocMetadataJSONObjects(json_t ***mdJSONs, const unsigned int numRecs)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT_allocMetadataJSONObjects() ---\n");
    fprintf(stderr, "\tallocating space for %u records...\n", numRecs);
#endif

#ifdef __cplusplus
    *mdJSONs = static_cast<json_t **>( malloc(sizeof(json_t *) * numRecs) );
#else
    *mdJSONs = malloc(sizeof(json_t *) * numRecs);
#endif

    if (!*mdJSONs)
        return kStarchFalse;

#ifdef DEBUG
    fprintf(stderr, "\tallocated %u records...\n", numRecs);
#endif

    return kStarchTrue;
}

Boolean
STARCHCAT_freeMetadataJSONObjects(json_t ***mdJSONs, const unsigned int numRecs)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT_freeMetadataJSONObjects() ---\n");
#endif   
    unsigned int idx;

    for (idx = 0U; idx < numRecs; idx++)
        json_decref(*(*mdJSONs + idx));

    return kStarchTrue;
}

Boolean
STARCHCAT_checkMetadataJSONVersions(json_t ***mdJSONs, const unsigned int numRecs)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT_checkMetadataJSONVersions() ---\n");
#endif   
#ifdef __cplusplus
    json_t *streamArchive = nullptr;
    json_t *streamArchiveVersion = nullptr;
    const char *jsonObjKey = nullptr;
    json_t *jsonObjValue = nullptr;
    const char *jsonObjAvKey = nullptr;
    json_t *jsonObjAvValue = nullptr;
    ArchiveVersion *av = nullptr;
#else
    json_t *streamArchive = NULL;
    json_t *streamArchiveVersion = NULL;
    const char *jsonObjKey = NULL;
    json_t *jsonObjValue = NULL;
    const char *jsonObjAvKey = NULL;
    json_t *jsonObjAvValue = NULL;
    ArchiveVersion *av = NULL;
#endif
    unsigned int idx;
    json_t *mdJSON;

#ifdef __cplusplus
    av = static_cast<ArchiveVersion *>( malloc (sizeof(ArchiveVersion)) );
#else
    av = malloc (sizeof(ArchiveVersion));
#endif

    if (!av) {
        fprintf(stderr, "ERROR: Could not allocate space for archive version object\n");
        return kStarchFalse;
    }

    for (idx = 0U; idx < numRecs; idx++) {
        mdJSON = *(*mdJSONs + idx);
        if (!mdJSON) {
            fprintf(stderr, "ERROR: Could not find metadata JSON object reference\n");
            return kStarchFalse;
        }

        streamArchive = json_object_get(mdJSON, STARCH_METADATA_STREAM_ARCHIVE_KEY);
        if (streamArchive) {
#ifdef __cplusplus
            json_object_foreach(streamArchive, jsonObjKey, jsonObjValue)
            {
                if (strcmp(jsonObjKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY) == 0) {
                    streamArchiveVersion = json_object_get(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY);
                    json_object_foreach(streamArchiveVersion, jsonObjAvKey, jsonObjAvValue) 
                    {
                        if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_KEY) == 0)
                            av->major = static_cast<int>( json_integer_value(jsonObjAvValue) );
                        else if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_KEY) == 0)
                            av->minor = static_cast<int>( json_integer_value(jsonObjAvValue) );
                        else if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_KEY) == 0)
                            av->revision = static_cast<int>( json_integer_value(jsonObjAvValue) );
                    }
                    streamArchiveVersion = nullptr;
                }
            }
            streamArchive = nullptr;
#else
            json_object_foreach(streamArchive, jsonObjKey, jsonObjValue)
            {
                if (strcmp(jsonObjKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY) == 0) {
                    streamArchiveVersion = json_object_get(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY);
                    json_object_foreach(streamArchiveVersion, jsonObjAvKey, jsonObjAvValue) 
                    {
                        if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_KEY) == 0)
                            av->major = (int) json_integer_value(jsonObjAvValue);
                        else if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_KEY) == 0)
                            av->minor = (int) json_integer_value(jsonObjAvValue);
                        else if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_KEY) == 0)
                            av->revision = (int) json_integer_value(jsonObjAvValue);
                    }
                    streamArchiveVersion = NULL;
                }
            }
            streamArchive = NULL;
#endif
        }
        else {
            fprintf(stderr, "ERROR: Could not retrieve stream archive object from metadata JSON string\n");
            return kStarchFalse;
        }

#ifdef __cplusplus
        if (STARCHCAT_isArchiveNewer(reinterpret_cast<const ArchiveVersion *>( av )) == kStarchTrue) {
#else
        if (STARCHCAT_isArchiveNewer((const ArchiveVersion *) av) == kStarchTrue) {
#endif
            fprintf(stderr, "ERROR: Cannot process newer archive (v%d.%d.%d) with older version of tools (v%d.%d.%d). Please update your toolkit.\n", av->major, av->minor, av->revision, STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
            return kStarchFalse;
        }
        json_decref(mdJSON);
#ifdef __cplusplus
        mdJSON = nullptr;
#else
        mdJSON = NULL;
#endif
    }

    return kStarchTrue;
}

int
STARCHCAT2_mergeChromosomeStreams(const ChromosomeSummaries *chrSums, const CompressionType outputType, const char *note, size_t *cumulativeOutputSize, const Boolean generatePerChrSignatureFlag, const Boolean reportProgressFlag, const LineCountType reportProgressN)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_mergeChromosomeStreams() ---\n");
#endif
#ifdef __cplusplus
    char *outputTag = nullptr;
    ChromosomeSummary *summary = nullptr;
    MetadataRecord *inputRecord = nullptr;
    char *inputChr = nullptr;
    Metadata *outputMd = nullptr;
    Metadata *headOutputMd = nullptr;
    char *dynamicMdBuffer = nullptr;
    char *dynamicMdBufferCopy = nullptr;
    char *base64EncodedSha1Digest = nullptr;
    ArchiveVersion *av120 = nullptr;
#else
    char *outputTag = NULL;
    ChromosomeSummary *summary = NULL;
    MetadataRecord *inputRecord = NULL;
    char *inputChr = NULL;
    Metadata *outputMd = NULL;
    Metadata *headOutputMd = NULL;
    char *dynamicMdBuffer = NULL;
    char *dynamicMdBufferCopy = NULL;
    char *base64EncodedSha1Digest = NULL;
    ArchiveVersion *av120 = NULL;
#endif
    unsigned int chrIdx = 0U;
    CompressionType inputType;
    Boolean firstOutputMdFlag = kStarchTrue;
    Boolean hFlag = kStarchFalse; /* starchcat does not currently support headers */
    unsigned char sha1Digest[STARCH2_MD_FOOTER_SHA1_LENGTH] = {0};
    char footerCumulativeRecordSizeBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + 1] = {0};
    char footerRemainderBuffer[STARCH2_MD_FOOTER_REMAINDER_LENGTH + 1] = {0};
    char footerBuffer[STARCH2_MD_FOOTER_LENGTH + 1] = {0};
    int footerCumulativeRecordSizeBufferCharsCopied = -1;

    if (!chrSums) {
        fprintf(stderr, "ERROR: Chromosome summary is empty. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    av120 = static_cast<ArchiveVersion *>( malloc (sizeof(ArchiveVersion)) );
#else
    av120 = malloc (sizeof(ArchiveVersion));
#endif

    if (!av120) {
        fprintf(stderr, "ERROR: Could not allocate space to archive version test struct.\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    av120->major = 1;
    av120->minor = 2;
    av120->revision = 0;

    /*
        When we initially built the chromosome list, we applied a 
        lexicographical sort, so that we can process and export 
        streams in that order.

        We now loop through each chromosome of the summary 
        records, in order to determine how many files need 
        to be extracted to build the merged output stream.

        If there is only one source file associated with a given 
        chromosome, then we can simply copy the byte range over 
        (after calculating the relevant file offsets) but **only** 
        if the source compression type matches the output type **and**
        if the starch archive version is concurrent or newer than 
        the version of starchcat.
 
        Otherwise, we must extract to the output stream and recompress 
        it into the new output type.

        If there are two or more files associated with a chromosome,
        then we need to extract a BED line from each file pointer,
        compare BED coordinates from all source streams, and print the 
        next closest coordinate to the output stream.
    */

    STARCH_buildProcessIDTag( &outputTag );

    for (chrIdx = 0U; chrIdx < chrSums->numChromosomes; chrIdx++) 
    {
        summary = chrSums->summary + chrIdx;

        if (summary->numRecords < 1) {
            /* If we get here, something went wrong with a data structure. */
            fprintf(stderr, "ERROR: Summaries pointer corrupt? Could not locate records in summaries.\n");
            return STARCHCAT_EXIT_FAILURE;
        }

        /*
            If there is only one record (and archive versions are concurrent), 
            compare output and input types and either copy bytes over directly, 
            or retransform data with new output type and/or newer version.

            Note that we do not transform data if: 

                1) data are newer than v1.2 (inclusive)
                2) data are older than the current build's archive version support
                3) we are staying with original bzip2 or gzip stream compression type
                
            Otherwise, we need to rewrite to obtain features lacking in pre-v1.2 
            archives, or extract/recompress to change stream compression type.
        */

        else if (summary->numRecords == 1) {
            inputRecord = *(summary->records);
            inputChr = summary->chromosome;
            inputType = inputRecord->type;
            if (!inputRecord->av) {
                fprintf(stderr, "ERROR: Input record has no archive version data\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            if (!av120) {
                fprintf(stderr, "ERROR: The av120 variable has no archive version data\n");
                return STARCHCAT_EXIT_FAILURE;
            }

#ifdef DEBUG
            fprintf(stderr, "chromosome                                [%s]\n",   inputChr);
            fprintf(stderr, "kStarchTrue                               [%d]\n",   (int) kStarchTrue);
            fprintf(stderr, "STARCHCAT_isArchiveOlder                  [%d]\n",   (int) STARCHCAT_isArchiveOlder((const ArchiveVersion *) inputRecord->av));
            fprintf(stderr, "STARCHCAT_isArchiveConcurrent             [%d]\n",   (int) STARCHCAT_isArchiveConcurrent((const ArchiveVersion *) inputRecord->av));
            fprintf(stderr, "STARCHCAT_isArchiveConcurrentOrOlder      [%d]\n",   (int) STARCHCAT_isArchiveConcurrentOrOlder((const ArchiveVersion *) inputRecord->av));
            fprintf(stderr, "STARCHCAT_isArchiveNewerThan              [%d]\n",   (int) STARCHCAT_isArchiveNewerThan((const ArchiveVersion *) inputRecord->av, av120));
            fprintf(stderr, "inputType                                 [%d]\n",   (int) inputType);
            fprintf(stderr, "outputType                                [%d]\n\n", (int) outputType);
#endif

#ifdef __cplusplus
            if ( (inputType == outputType) && 
                 (STARCHCAT_isArchiveConcurrent(reinterpret_cast<const ArchiveVersion *>( inputRecord->av )) == kStarchTrue) ) {
#else
            if ( (inputType == outputType) && 
                 (STARCHCAT_isArchiveConcurrent((const ArchiveVersion *) inputRecord->av) == kStarchTrue) ) {
#endif

#ifdef __cplusplus
                assert( STARCHCAT2_copyInputRecordToOutput( &outputMd, 
                                                            reinterpret_cast<const char *>( outputTag ), 
                                                            static_cast<const CompressionType>( outputType ), 
                                                            reinterpret_cast<const char *>( inputChr ), 
                                                            reinterpret_cast<const MetadataRecord *>( inputRecord ), 
                                                            cumulativeOutputSize,
                                                            reportProgressFlag) );
#else
                assert( STARCHCAT2_copyInputRecordToOutput( &outputMd, 
                                                            (const char *) outputTag, 
                                                            (const CompressionType) outputType, 
                                                            (const char *) inputChr, 
                                                            (const MetadataRecord *) inputRecord, 
                                                            cumulativeOutputSize,
                                                            reportProgressFlag) );
#endif
            }
#ifdef __cplusplus
            else if ( (inputType == outputType) && 
                 (STARCHCAT_isArchiveOlder(reinterpret_cast<const ArchiveVersion *>( inputRecord->av )) ) && 
                 (STARCHCAT_isArchiveNewerThan(reinterpret_cast<const ArchiveVersion *>( inputRecord->av ), av120)) ) {
#else
            else if ( (inputType == outputType) && 
                 (STARCHCAT_isArchiveOlder((const ArchiveVersion *) inputRecord->av)) && 
                 (STARCHCAT_isArchiveNewerThan((const ArchiveVersion *) inputRecord->av, av120)) ) {
#endif
#ifdef __cplusplus
                assert( STARCHCAT2_rewriteInputRecordToOutput( &outputMd, 
                                                               reinterpret_cast<const char *>( outputTag ), 
                                                               static_cast<const CompressionType>( outputType ), 
                                                               reinterpret_cast<const char *>( inputChr ), 
                                                               reinterpret_cast<const MetadataRecord *>( inputRecord ), 
                                                               cumulativeOutputSize,
                                                               generatePerChrSignatureFlag,
                                                               reportProgressFlag,
                                                               reportProgressN) );
#else
                assert( STARCHCAT2_rewriteInputRecordToOutput( &outputMd, 
                                                               (const char *) outputTag, 
                                                               (const CompressionType) outputType, 
                                                               (const char *) inputChr, 
                                                               (const MetadataRecord *) inputRecord, 
                                                               cumulativeOutputSize,
                                                               generatePerChrSignatureFlag,
                                                               reportProgressFlag,
                                                               reportProgressN) );
#endif
            }
            else {
#ifdef __cplusplus
                assert( STARCHCAT2_rewriteInputRecordToOutput( &outputMd, 
                                                               reinterpret_cast<const char *>( outputTag ), 
                                                               static_cast<const CompressionType>( outputType ), 
                                                               reinterpret_cast<const char *>( inputChr ), 
                                                               reinterpret_cast<const MetadataRecord *>( inputRecord ), 
                                                               cumulativeOutputSize,
                                                               generatePerChrSignatureFlag,
                                                               reportProgressFlag,
                                                               reportProgressN) );
#else
                assert( STARCHCAT2_rewriteInputRecordToOutput( &outputMd, 
                                                               (const char *) outputTag, 
                                                               (const CompressionType) outputType, 
                                                               (const char *) inputChr, 
                                                               (const MetadataRecord *) inputRecord, 
                                                               cumulativeOutputSize,
                                                               generatePerChrSignatureFlag,
                                                               reportProgressFlag,
                                                               reportProgressN) );
#endif
            }
            if (!outputMd) {
                fprintf(stderr, "ERROR: Output metadata structure is empty after adding data. Something went wrong in mid-stream.\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            /* *cumulativeOutputSize += outputMd->size; */
        }

        /* 
            In this case, we have a mix of records for a given chromosome. We walk through 
            input records line by line, returning the next lexicographically ordered BED
            element from the set of records. The final result is transformed and compressed, 
            and a new record added to the output metadata.
        */

        else {
#ifdef __cplusplus
            assert( STARCHCAT2_mergeInputRecordsToOutput( reinterpret_cast<const char *>( summary->chromosome ), 
                                                          &outputMd, 
                                                          reinterpret_cast<const char *>( outputTag ), 
                                                          static_cast<const CompressionType>( outputType ), 
                                                          reinterpret_cast<const ChromosomeSummary *>( summary ),
                                                          cumulativeOutputSize) );
#else
            assert( STARCHCAT2_mergeInputRecordsToOutput( (const char *) summary->chromosome, 
                                                          &outputMd, 
                                                          (const char *) outputTag, 
                                                          (const CompressionType) outputType, 
                                                          (const ChromosomeSummary *) summary,
                                                          cumulativeOutputSize) );
#endif
        }

        if (!outputMd) {
            fprintf(stderr, "ERROR: Output metadata structure is empty after adding data. Something went wrong in mid-stream.\n");
            return STARCHCAT_EXIT_FAILURE;
        }

        /* 
            Grab a pointer to the head element of the output metadata. When we
            export the metadata to the final archive, we need to walk through the
            metadata from the first record.
        */

        if (firstOutputMdFlag == kStarchTrue) {
            headOutputMd = outputMd; 
            firstOutputMdFlag = kStarchFalse; 
        }
    }

    /* stitch up compressed files into one archive, along with metadata header */
#ifdef __cplusplus
    assert(STARCH_writeJSONMetadata( reinterpret_cast<const Metadata *>( headOutputMd ), 
                                     &dynamicMdBuffer, 
                                     const_cast<CompressionType *>( &outputType ), 
                                     static_cast<const Boolean>( hFlag ), 
                                     static_cast<const char *>( note )) );
#else
    assert(STARCH_writeJSONMetadata( (const Metadata *) headOutputMd, 
                                     &dynamicMdBuffer, 
                                     (CompressionType *) &outputType, 
                                     (const Boolean) hFlag, 
                                     (const char *) note) );
#endif
    fwrite(dynamicMdBuffer, 1, strlen(dynamicMdBuffer), stdout);
    fflush(stdout);

    /* write metadata signature */
    dynamicMdBufferCopy = STARCH_strdup(dynamicMdBuffer);
#ifdef __cplusplus
    STARCH_SHA1_All(reinterpret_cast<const unsigned char *>( dynamicMdBuffer ), 
            strlen(dynamicMdBuffer), 
            sha1Digest);
#else
    STARCH_SHA1_All((const unsigned char *) dynamicMdBuffer, 
            strlen(dynamicMdBuffer), 
            sha1Digest);
#endif
    free(dynamicMdBufferCopy);

    /* encode signature in base64 encoding */
#ifdef __cplusplus
    STARCH_encodeBase64(&base64EncodedSha1Digest, 
            static_cast<const size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ), 
            reinterpret_cast<const unsigned char *>( sha1Digest ), 
            static_cast<const size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ));
#else
    STARCH_encodeBase64(&base64EncodedSha1Digest, 
            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, 
            (const unsigned char *) sha1Digest, 
            (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
#endif

    /* build footer */
#ifdef __cplusplus
    /* we cast this size_t to an unsigned long long in order to allow warning-free compilation with an ISO C++ compiler like g++ */
    footerCumulativeRecordSizeBufferCharsCopied = sprintf(footerCumulativeRecordSizeBuffer, "%020llu", static_cast<unsigned long long>( *cumulativeOutputSize )); 
#else
    footerCumulativeRecordSizeBufferCharsCopied = sprintf(footerCumulativeRecordSizeBuffer, "%020llu", (unsigned long long) *cumulativeOutputSize); 
#endif

    /* ensure that we built a correct footer with the right record size */
    assert(footerCumulativeRecordSizeBufferCharsCopied == STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH);

    memcpy(footerBuffer, footerCumulativeRecordSizeBuffer, strlen(footerCumulativeRecordSizeBuffer));
    memcpy(footerBuffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH, base64EncodedSha1Digest, STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1); /* strip trailing null */
#ifdef __cplusplus
    memset(footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, static_cast<size_t>( STARCH2_MD_FOOTER_REMAINDER_LENGTH ));
#else
    memset(footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, (size_t) STARCH2_MD_FOOTER_REMAINDER_LENGTH);
#endif

    memcpy(footerBuffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1, footerRemainderBuffer, STARCH2_MD_FOOTER_REMAINDER_LENGTH); /* don't forget to offset pointer index by -1 for base64-sha1's null */
    footerBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 1] = '\0';
    footerBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 2] = '\n';
    fprintf(stdout, "%s", footerBuffer);
    fflush(stdout);

    /* cleanup */
    if (dynamicMdBuffer) {
        free(dynamicMdBuffer);
#ifdef __cplusplus
        dynamicMdBuffer = nullptr;
#else
        dynamicMdBuffer = NULL;
#endif
    }
    if (base64EncodedSha1Digest) {
        free(base64EncodedSha1Digest);
#ifdef __cplusplus
        base64EncodedSha1Digest = nullptr;
#else
        base64EncodedSha1Digest = NULL;
#endif
    }
    if (av120) {
        free(av120);
#ifdef __cplusplus
        av120 = nullptr;
#else
        av120 = NULL;
#endif
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_setupBzip2OutputStream(BZFILE **bzStream, FILE *outStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_setupBzip2OutputStream() ---\n");
#endif 
    int bzError;

    *bzStream = BZ2_bzWriteOpen(&bzError, outStream, STARCH_BZ_COMPRESSION_LEVEL, STARCH_BZ_VERBOSITY, STARCH_BZ_WORKFACTOR);

    if (!*bzStream) {
        fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    else if (bzError != BZ_OK) {
        switch (bzError) {
            case BZ_CONFIG_ERROR: {
                fprintf(stderr, "ERROR: Bzip2 library has been miscompiled\n");
                break;
            }
            case BZ_PARAM_ERROR: {
                fprintf(stderr, "ERROR: Stream is null, or block size, verbosity and work factor parameters are invalid\n");
                break;
            }
            case BZ_IO_ERROR: {
                fprintf(stderr, "ERROR: The value of ferror(outStream) is nonzero -- check outStream\n");
                break;
            }
            case BZ_MEM_ERROR: {
                fprintf(stderr, "ERROR: Not enough memory is available\n");
                break;
            }
            default: {
                fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteOpen() (err: %d)\n", bzError);
                break;
            }
        }
        return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_setupGzipOutputStream(z_stream *zStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_setupGzipOutputStream() ---\n");
#endif 
    int zError;
    z_stream *zStreamPtr = zStream;

#ifdef __cplusplus
    zStreamPtr->zalloc = nullptr;
    zStreamPtr->zfree = nullptr;
    zStreamPtr->opaque = nullptr;
#else
    zStreamPtr->zalloc = Z_NULL;
    zStreamPtr->zfree = Z_NULL;
    zStreamPtr->opaque = Z_NULL;
#endif

    /* cf. http://www.zlib.net/manual.html for level information */
#ifdef __cplusplus
    zError = deflateInit2cpp(zStreamPtr, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY);
#else
    zError = deflateInit2(zStreamPtr, STARCH_Z_COMPRESSION_LEVEL, Z_DEFLATED, STARCH_Z_WINDOW_BITS, STARCH_Z_MEMORY_LEVEL, Z_DEFAULT_STRATEGY);
#endif
    /* zError = deflateInit(zStreamPtr, STARCH_Z_COMPRESSION_LEVEL); */

    if (zError != Z_OK) {
        switch (zError) {
            case Z_MEM_ERROR: {
                fprintf(stderr, "ERROR: Not enough memory is available\n");
                break;
            }
            case Z_STREAM_ERROR: {
                fprintf(stderr, "ERROR: Gzip initialization parameter is invalid (e.g., invalid method)\n");
                break;
            }
            case Z_VERSION_ERROR: {
                fprintf(stderr, "ERROR: the zlib library version is incompatible with the version assumed by the caller (ZLIB_VERSION)\n");
                break;
            }
            default: {
                fprintf(stderr, "ERROR: Unknown error with deflateInit() (err: %d)\n", zError);
                break;
            }
        }
        return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_testSummaryForChromosomeExistence(const char *chrName, const ChromosomeSummary *chrSummary, const size_t recIndex)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_testSummaryForChromosomeExistence() ---\n");
#endif 

#ifdef __cplusplus
    MetadataRecord *rec = nullptr;
    MetadataRecord **recs = nullptr;
    Metadata *md = nullptr;
    Metadata *iter = nullptr;
#else
    MetadataRecord *rec = NULL;
    MetadataRecord **recs = NULL;
    Metadata *md = NULL;
    Metadata *iter = NULL;
#endif

    recs = chrSummary->records;
    rec = recs[recIndex];

    md = rec->metadata;
#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(iter->chromosome, chrName) == 0)
            return STARCHCAT_EXIT_SUCCESS;
    }

    return STARCHCAT_EXIT_FAILURE;
}

int
STARCHCAT2_setupInitialFileOffsets(const char *chrName, const ChromosomeSummary *chrSummary, const size_t recIndex)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_setupInitialFileOffsets() ---\n");
#endif 

#ifdef __cplusplus
    MetadataRecord *rec = nullptr;
    MetadataRecord **recs = nullptr;
    Metadata *md = nullptr;
    Metadata *iter = nullptr;
#else
    MetadataRecord *rec = NULL;
    MetadataRecord **recs = NULL;
    Metadata *md = NULL;
    Metadata *iter = NULL;
#endif
    uint64_t offset = 0;

    recs = chrSummary->records;
    rec = recs[recIndex];

    if (rec->av->major == 2)
        offset = STARCH2_MD_HEADER_BYTE_LENGTH;
    else if (rec->av->major == 1)
        offset = rec->mdOffset;
    md = rec->metadata;

#ifdef __cplusplus
    for (iter = md; iter != nullptr; iter = iter->next) {
#else
    for (iter = md; iter != NULL; iter = iter->next) {
#endif
        if (strcmp(iter->chromosome, chrName) == 0)
            break;
        else
            offset += iter->size;
    }

#ifdef __cplusplus
    fseeko(rec->fp, static_cast<off_t>( offset ), SEEK_SET);
#else
    fseeko(rec->fp, (off_t) offset, SEEK_SET);
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_setupBzip2InputStream(const size_t recIdx, const ChromosomeSummary *chrSummary, BZFILE **bzStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_setupBzip2InputStream() ---\n");
#endif 

#ifdef __cplusplus
    MetadataRecord **inRecs = nullptr;
    MetadataRecord *inRec = nullptr;
    FILE *inFp = nullptr;
#else
    MetadataRecord **inRecs = NULL;
    MetadataRecord *inRec = NULL;
    FILE *inFp = NULL;
#endif
    int bzError = BZ_OK;

    inRecs = chrSummary->records;
    inRec = inRecs[recIdx];
    inFp = inRec->fp;

#ifdef __cplusplus
    *bzStream = BZ2_bzReadOpen(&bzError, 
			       inFp, 
			       STARCH_BZ_VERBOSITY, 
			       STARCH_BZ_SMALL, 
			       nullptr, 
			       0);
#else
    *bzStream = BZ2_bzReadOpen(&bzError, 
			       inFp, 
			       STARCH_BZ_VERBOSITY, 
			       STARCH_BZ_SMALL, 
			       NULL, 
			       0);
#endif

    if (!*bzStream) {
        fprintf(stderr, "ERROR: Could not instantiate BZFILE pointer\n");
        return STARCHCAT_EXIT_FAILURE;
    }
    else if (bzError != BZ_OK) {
        switch (bzError) {
            case BZ_CONFIG_ERROR: {
                fprintf(stderr, "ERROR: Bzip2 library was miscompiled. Contact your system administrator.\n");
                break;
            }
            case BZ_PARAM_ERROR: {
                fprintf(stderr, "ERROR: Input file stream is NULL, small value is invalid, or unused parameters are invalid.\n");
                break;
            }
            case BZ_IO_ERROR: {
                fprintf(stderr, "ERROR: Error reading the underlying compressed file.\n");
                break;
            }
            case BZ_MEM_ERROR: {
                fprintf(stderr, "ERROR: Insufficient memory is available for bzip2 setup.\n");
                break;
            }
            default: {
                fprintf(stderr, "ERROR: Unknown bzip2 error (%d)\n", bzError);
                break;
            }
        }
        return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_setupGzipInputStream(z_stream *zStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_setupGzipInputStream() ---\n");
#endif

    int zError = Z_OK;

#ifdef __cplusplus
    zStream->zalloc = nullptr;
    zStream->zfree = nullptr;
    zStream->opaque = nullptr;
    zStream->next_in = nullptr;
#else
    zStream->zalloc = Z_NULL;
    zStream->zfree = Z_NULL;
    zStream->opaque = Z_NULL;
    zStream->next_in = Z_NULL;
#endif
    zStream->avail_in = 0;

    /* cf. http://www.zlib.net/manual.html for level information */
    zError = inflateInit2(zStream, (15+32));
    if (zError != Z_OK) {
        fprintf(stderr, "ERROR: Could not initialize z-stream (%d)\n", zError);
        return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_breakdownBzip2InputStream(BZFILE **bzStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_breakdownBzip2InputStream() ---\n");
#endif

    int bzError = BZ_OK;

    BZ2_bzReadClose(&bzError, *bzStream);

    if (bzError != BZ_OK) {
        fprintf(stderr, "ERROR: Bzip2 error after closing read stream: %d\n", bzError);
        return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_breakdownGzipInputStream(z_stream *zStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_breakdownGzipInputStream() ---\n");
#endif

    int zError = Z_OK;

    zError = inflateEnd(zStream);

    if (zError != Z_OK) {
        fprintf(stderr, "ERROR: Gzip error after closing read stream: %d\n", zError);
        return STARCHCAT_EXIT_FAILURE;
    }

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_breakdownBzip2OutputStream(BZFILE **bzStream, uint64_t *bzOutBytesConsumed, uint64_t *bzOutBytesWritten)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_breakdownBzip2OutputStream() ---\n");
#endif    

    int bzError = BZ_OK;
    unsigned int nbytes_in_lo32 = 0U;
    unsigned int nbytes_in_hi32 = 0U;
    unsigned int nbytes_out_lo32 = 0U;
    unsigned int nbytes_out_hi32 = 0U;

    BZ2_bzWriteClose64(&bzError, *bzStream, STARCH_BZ_ABANDON, &nbytes_in_lo32, &nbytes_in_hi32, &nbytes_out_lo32, &nbytes_out_hi32);

    if (bzError != BZ_OK) {
        switch (bzError) {
            case BZ_SEQUENCE_ERROR: {
                fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                break;
            }
            case BZ_IO_ERROR: {
                fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                break;
            }
            default: {
                fprintf(stderr, "ERROR: Unknown error with BZ2_bzWriteClose64() (err: %d)\n", bzError);
                break;
            }
        }
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    *bzOutBytesConsumed = nbytes_in_lo32 + (static_cast<uint64_t>( nbytes_in_hi32 ) << 32 );
    *bzOutBytesWritten = nbytes_out_lo32 + (static_cast<uint64_t>( nbytes_out_hi32 ) << 32 );
#else
    *bzOutBytesConsumed = nbytes_in_lo32 + ((uint64_t) nbytes_in_hi32 << 32);
    *bzOutBytesWritten = nbytes_out_lo32 + ((uint64_t) nbytes_out_hi32 << 32);
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_breakdownGzipOutputStream(z_stream *zStream)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_breakdownGzipOutputStream() ---\n");
#endif    

    z_stream *zStreamPtr = zStream;
    int zError = Z_OK;

    zError = deflateEnd(zStreamPtr);

    if (zError != Z_OK) {
        switch (zError) {
            case Z_STREAM_ERROR: {
                fprintf(stderr, "ERROR: Z-stream state is inconsistent\n");
                break;
            }
            case Z_DATA_ERROR: {
                fprintf(stderr, "ERROR: Stream was freed prematurely\n");
                break;
            }
            default: {
                fprintf(stderr, "ERROR: Unknown error with deflateEnd() (err: %d)\n", zError);
                break;
            }
        }
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    zStreamPtr->zalloc = nullptr;
    zStreamPtr->zfree  = nullptr;
    zStreamPtr->opaque = nullptr;
#else
    zStreamPtr->zalloc = Z_NULL;
    zStreamPtr->zfree  = Z_NULL;
    zStreamPtr->opaque = Z_NULL;
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_fillExtractionBufferFromBzip2Stream(Boolean *eofFlag, char *recordChromosome, char *extractionBuffer, size_t *nExtractionBuffer, BZFILE **bzStream, size_t *nBzRead, char *bzRemainderBuf, size_t *nBzRemainderBuf, TransformState *t_state)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_fillExtractionBufferFromBzip2Stream() (%s) --- start\n", recordChromosome);
#endif

    if (*eofFlag == kStarchTrue)
        return STARCHCAT_EXIT_SUCCESS;

    char *finalBuffer                           = extractionBuffer;
#ifdef __cplusplus
    unsigned char *bzReadBuf                    = nullptr;
#else
    unsigned char *bzReadBuf                    = NULL;
#endif
    size_t nBzReadBuf                           = 1024*1024/8;
    size_t bzBufIndex                           = 0;
    int bzError                                 = BZ_OK;
#ifdef __cplusplus
    unsigned char *bzLineBuf                    = nullptr;
#else
    unsigned char *bzLineBuf                    = NULL;
#endif
    size_t bzCharIndex                          = 0;
    static const char tab                       = '\t';

    LineCountType *t_lineIdxPtr                 = &t_state->t_lineIdx;
    SignedCoordType *t_startPtr                 = &t_state->t_start;
    SignedCoordType *t_pLengthPtr               = &t_state->t_pLength;
    SignedCoordType *t_lastEndPtr               = &t_state->t_lastEnd;
    char *t_firstInputToken                     = t_state->t_firstInputToken;
    char *t_secondInputToken                    = t_state->t_secondInputToken;
    char *t_currentChromosome                   = t_state->t_currentChromosome;
    size_t *t_currentChromosomeLengthPtr        = &t_state->t_currentChromosomeLength;
    SignedCoordType *t_currentStartPtr          = &t_state->t_currentStart;
    SignedCoordType *t_currentStopPtr           = &t_state->t_currentStop;
    char *t_currentRemainder                    = t_state->t_currentRemainder;
    size_t *t_currentRemainderLengthPtr         = &t_state->t_currentRemainderLength;
    SignedCoordType *t_lastPositionPtr          = &t_state->t_lastPosition;
    SignedCoordType *t_lcDiffPtr                = &t_state->t_lcDiff;
    size_t *t_nExtractionBuffer                 = &t_state->t_nExtractionBuffer;
    size_t *t_nExtractionBufferPos              = &t_state->t_nExtractionBufferPos;

#ifdef __cplusplus
    unsigned char *retransformedLineBuffer      = nullptr;
    char *resizedExtractionBuffer               = nullptr;
#else
    unsigned char *retransformedLineBuffer      = NULL;
    char *resizedExtractionBuffer               = NULL;
#endif
    int64_t nRetransformedLineBuffer            = 0;
    int64_t nRetransformedLineBufferPosition    = 0;
    size_t nResizedExtractionBuffer             = 0U;

    if (!*bzStream) {
        fprintf(stderr, "ERROR: Bzip2 stream is NULL. Something went wrong setting up extraction!\n");
        return STARCHCAT_EXIT_FAILURE;
    }

    if (!finalBuffer) {
        fprintf(stderr, "ERROR: Extraction buffer is NULL. Something went wrong setting up extraction!\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    bzReadBuf = static_cast<unsigned char *>( malloc(STARCH_BZ_BUFFER_MAX_LENGTH) );
#else
    bzReadBuf = malloc(STARCH_BZ_BUFFER_MAX_LENGTH);
#endif

    if (!bzReadBuf) {
        fprintf(stderr, "ERROR: Could not allocate space for bzip2 transform buffer data. Could not merge.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    bzLineBuf = static_cast<unsigned char *>( malloc(TOKENS_MAX_LENGTH) );
#else
    bzLineBuf = malloc(TOKENS_MAX_LENGTH);
#endif

    if (!bzLineBuf) {
        fprintf(stderr, "ERROR: Could not allocate space for bzip2 line buffer data.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    retransformedLineBuffer = static_cast<unsigned char *>( malloc(sizeof(unsigned char) * STARCH_STREAM_METADATA_MAX_LENGTH) );
#else
    retransformedLineBuffer = malloc(sizeof(unsigned char) * STARCH_STREAM_METADATA_MAX_LENGTH);
#endif

    if (!retransformedLineBuffer) {
        fprintf(stderr, "ERROR: Could not allocate space for bzip2 retransformation buffer data.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

    if (!*bzStream) {
        fprintf(stderr, "ERROR: *bzStream is not available.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef __cplusplus
    *nBzRead = static_cast<size_t>( BZ2_bzRead(&bzError, *bzStream, bzReadBuf, static_cast<int>( nBzReadBuf )) );
#else
    *nBzRead = (size_t) BZ2_bzRead(&bzError, *bzStream, bzReadBuf, (int) nBzReadBuf);
#endif

    if (bzError == BZ_STREAM_END)
        *eofFlag = kStarchTrue;

    else if (bzError != BZ_OK) {
        switch (bzError) {
            case BZ_PARAM_ERROR: {
                fprintf(stderr, "ERROR: Input bzip2 file stream is NULL, bzip read buffer is NULL, or nBzReadBuf is less than zero\n");
                break;
            }
            case BZ_SEQUENCE_ERROR: {
                fprintf(stderr, "ERROR: Input bzip2 file stream was opened with BZ2_bzWriteOpen()\n");
                break;
            }
            case BZ_IO_ERROR: {
                fprintf(stderr, "ERROR: Could not read from input bzip2 file stream\n");
                break;
            }
            case BZ_UNEXPECTED_EOF: {
                fprintf(stderr, "ERROR: Input bzip2 file stream ended before expected EOF\n");
                break;
            }
            case BZ_DATA_ERROR: {
                fprintf(stderr, "ERROR: Data integrity error detected in input bzip2 file stream\n");
                break;
            }
            case BZ_DATA_ERROR_MAGIC: {
                fprintf(stderr, "ERROR: Input data stream does not begin with bzip2 magic bytes. Possible offset error\n");
                break;
            }
            case BZ_MEM_ERROR: {
                fprintf(stderr, "ERROR: Insufficient memory was available to extract data from input bzip2 file stream\n");
                break;
            }
            default: {
                fprintf(stderr, "ERROR: Unknown error extracting data from input bzip2 file stream\n");
                break;
            }
        }
        return STARCHCAT_EXIT_FAILURE;
    }

    /* process bz buffer */
    /* put any remainder of bz-stream output into the line buffer */

#ifdef DEBUG
    if (*eofFlag == kStarchTrue)
        fprintf(stderr, "BZ_STREAM_END - setting EOF flag\n");
#endif

    bzCharIndex = 0;
    if (*nBzRemainderBuf > 0) {
#ifdef __cplusplus
        memcpy(reinterpret_cast<char *>( bzLineBuf ), const_cast<const char *>( bzRemainderBuf ), *nBzRemainderBuf);
#else
        memcpy((char *) bzLineBuf, (const char *) bzRemainderBuf, *nBzRemainderBuf);
#endif
        bzCharIndex = *nBzRemainderBuf;
    }

    *t_nExtractionBuffer = 0;
    *t_nExtractionBufferPos = 0;

    bzBufIndex = 0;
    while (bzBufIndex < *nBzRead) {
        bzLineBuf[bzCharIndex++] = bzReadBuf[bzBufIndex++];
        if (bzLineBuf[bzCharIndex - 1] == '\n') {
            bzLineBuf[bzCharIndex - 1] = '\0';

            UNSTARCH_extractRawLine(recordChromosome,
                                    bzLineBuf,
                                    tab,
                                    t_startPtr, 
                                    t_pLengthPtr, 
                                    t_lastEndPtr,
                                    t_firstInputToken, 
                                    t_secondInputToken,
                                    &t_currentChromosome, 
                                    t_currentChromosomeLengthPtr, 
                                    t_currentStartPtr, 
                                    t_currentStopPtr,
                                    &t_currentRemainder, 
                                    t_currentRemainderLengthPtr);

            if (bzLineBuf[0] != 'p') {
                (*t_lineIdxPtr)++;
#ifdef __cplusplus
                UNSTARCH_reverseTransformCoordinates( static_cast<const LineCountType>( *t_lineIdxPtr ),
#else
                UNSTARCH_reverseTransformCoordinates( (const LineCountType) *t_lineIdxPtr,
#endif
                                                      t_lastPositionPtr,
                                                      t_lcDiffPtr,
                                                      t_currentStartPtr, 
                                                      t_currentStopPtr, 
                                                      &t_currentRemainder, 
                                                      retransformedLineBuffer, 
                                                      &nRetransformedLineBuffer, 
                                                      &nRetransformedLineBufferPosition );
                /* resize the extraction buffer, if we're getting too close to the maximum size of a line */
#ifdef __cplusplus
                if (static_cast<unsigned int>( *nExtractionBuffer - *t_nExtractionBufferPos ) < TOKENS_MAX_LENGTH) {
#else
                if ((unsigned int) (*nExtractionBuffer - *t_nExtractionBufferPos) < TOKENS_MAX_LENGTH) {
#endif
                    nResizedExtractionBuffer = *nExtractionBuffer * 2;
#ifdef __cplusplus
                    resizedExtractionBuffer = static_cast<char *>( realloc(extractionBuffer, nResizedExtractionBuffer) );
#else
                    resizedExtractionBuffer = realloc(extractionBuffer, nResizedExtractionBuffer);
#endif
                    if (!resizedExtractionBuffer) {
                        fprintf(stderr, "ERROR: Could not allocate space for resized bzip2 extraction buffer!\n");
                        return STARCHCAT_EXIT_FAILURE;
                    }
                    extractionBuffer = resizedExtractionBuffer;
                    *nExtractionBuffer = nResizedExtractionBuffer;
                }

#ifdef __cplusplus
                *t_nExtractionBuffer = (strlen(t_currentRemainder) > 0) ? 
                    static_cast<size_t>( sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                         "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", 
                         t_currentChromosome, 
                         *t_currentStartPtr, 
                         *t_currentStopPtr, 
                         t_currentRemainder) ) 
            : 
                    static_cast<size_t>( sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                         "%s\t%" PRId64 "\t%" PRId64 "\n", 
                         t_currentChromosome, 
                         *t_currentStartPtr, 
                         *t_currentStopPtr) );
#else
                *t_nExtractionBuffer = (strlen(t_currentRemainder) > 0) ? 
                    (size_t) sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                     "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", 
                     t_currentChromosome, 
                     *t_currentStartPtr, 
                     *t_currentStopPtr, 
                     t_currentRemainder) : 
                    (size_t) sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                     "%s\t%" PRId64 "\t%" PRId64 "\n", 
                     t_currentChromosome, 
                     *t_currentStartPtr, 
                     *t_currentStopPtr);
#endif
                *t_nExtractionBufferPos += *t_nExtractionBuffer;
                *(extractionBuffer + *t_nExtractionBufferPos) = '\0';
            }
            t_firstInputToken[0] = '\0';
            t_secondInputToken[0] = '\0';
            bzCharIndex = 0;
        }
    }
    bzLineBuf[bzCharIndex] = '\0';
#ifdef __cplusplus
    strncpy(static_cast<char *>( bzRemainderBuf ), reinterpret_cast<const char *>(reinterpret_cast<char *>( bzLineBuf )), bzCharIndex);
#else
    strncpy((char *) bzRemainderBuf, (const char *) bzLineBuf, bzCharIndex);
#endif
    *nBzRemainderBuf = bzCharIndex;

    /* cleanup */
    if (bzReadBuf) {
        free(bzReadBuf);
#ifdef __cplusplus
        bzReadBuf = nullptr;
#else
        bzReadBuf = NULL;
#endif
    }

    if (bzLineBuf) {
        free(bzLineBuf);
#ifdef __cplusplus
        bzLineBuf = nullptr;
#else
        bzLineBuf = NULL;
#endif
    }

    if (retransformedLineBuffer) {
        free(retransformedLineBuffer);
#ifdef __cplusplus
        retransformedLineBuffer = nullptr;
#else
        retransformedLineBuffer = NULL;
#endif
    }

#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_fillExtractionBufferFromBzip2Stream() (%s) --- start\n", recordChromosome);
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_fillExtractionBufferFromGzipStream(Boolean *eofFlag, FILE **inputFp, char *recordChromosome, char *extractionBuffer, size_t *nExtractionBuffer, z_stream *zStream, size_t *nZRead, char **zRemainderBuf, size_t *nZRemainderBuf, TransformState *t_state)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_fillExtractionBufferFromGzipStream() ---\n");
#endif

#ifdef __cplusplus
    unsigned char *zInBuf                       = nullptr;
    unsigned char *zReadBuf                     = nullptr;
    unsigned char *zLineBuf                     = nullptr;
#else
    unsigned char *zInBuf                       = NULL;
    unsigned char *zReadBuf                     = NULL;
    unsigned char *zLineBuf                     = NULL;
#endif
    int zError                                  = Z_OK;
    size_t zCharIndex                           = 0;
    size_t zBufIndex                            = 0;
    static const char tab                       = '\t';

    LineCountType *t_lineIdxPtr                 = &t_state->t_lineIdx;
    SignedCoordType *t_startPtr                 = &t_state->t_start;
    SignedCoordType *t_pLengthPtr               = &t_state->t_pLength;
    SignedCoordType *t_lastEndPtr               = &t_state->t_lastEnd;
    char *t_firstInputToken                     = t_state->t_firstInputToken;
    char *t_secondInputToken                    = t_state->t_secondInputToken;
    char *t_currentChromosome                   = t_state->t_currentChromosome;
    size_t *t_currentChromosomeLengthPtr        = &t_state->t_currentChromosomeLength;
    SignedCoordType *t_currentStartPtr          = &t_state->t_currentStart;
    SignedCoordType *t_currentStopPtr           = &t_state->t_currentStop;
    char *t_currentRemainder                    = t_state->t_currentRemainder;
    size_t *t_currentRemainderLengthPtr         = &t_state->t_currentRemainderLength;
    SignedCoordType *t_lastPositionPtr          = &t_state->t_lastPosition;
    SignedCoordType *t_lcDiffPtr                = &t_state->t_lcDiff;
    size_t *t_nExtractionBuffer                 = &t_state->t_nExtractionBuffer;
    size_t *t_nExtractionBufferPos              = &t_state->t_nExtractionBufferPos;

#ifdef __cplusplus
    unsigned char *retransformedLineBuffer      = nullptr;
    char *resizedExtractionBuffer               = nullptr;
#else
    unsigned char *retransformedLineBuffer      = NULL;
    char *resizedExtractionBuffer               = NULL;
#endif
    char *extractionBufferStart                 = extractionBuffer;
    int64_t nRetransformedLineBuffer            = 0;
    int64_t nRetransformedLineBufferPosition    = 0;
    size_t nResizedExtractionBuffer             = 0U;

#ifdef DEBUG
    fprintf(stderr, "ALLOC in STARCHCAT2_fillExtractionBufferFromGzipStream()\n");
#endif

#ifdef __cplusplus
    zInBuf = static_cast<unsigned char *>( malloc(STARCH_Z_CHUNK) );
#else
    zInBuf = malloc(STARCH_Z_CHUNK);
#endif

    if (!zInBuf) {
        fprintf(stderr, "ERROR: Could not allocate space for z-input buffer!\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    fprintf(stderr, "ALLOC'ed zInBuf\n");
#endif

#ifdef __cplusplus
    zReadBuf = static_cast<unsigned char *>( malloc(STARCH_Z_CHUNK*5) );
#else
    zReadBuf = malloc(STARCH_Z_CHUNK*5);
#endif

    if (!zReadBuf) {
        fprintf(stderr, "ERROR: Could not allocate space for z-output buffer!\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    fprintf(stderr, "ALLOC'ed zReadBuf\n");
#endif

#ifdef __cplusplus
    zLineBuf = static_cast<unsigned char *>( malloc(TOKENS_MAX_LENGTH) );
#else
    zLineBuf = malloc(TOKENS_MAX_LENGTH);
#endif

    if (!zLineBuf) {
        fprintf(stderr, "ERROR: Could not allocate space for z-output line buffer!\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    fprintf(stderr, "ALLOC'ed zLineBuf\n");
#endif

#ifdef __cplusplus
    retransformedLineBuffer = static_cast<unsigned char *>( malloc(sizeof(unsigned char) * (STARCHCAT_RETRANSFORM_LINE_COUNT_MAX * TOKENS_MAX_LENGTH)) );
#else
    retransformedLineBuffer = malloc(sizeof(unsigned char) * (STARCHCAT_RETRANSFORM_LINE_COUNT_MAX * TOKENS_MAX_LENGTH));
#endif

    if (!retransformedLineBuffer) {
        fprintf(stderr, "ERROR: Could not allocate space for gzip retransformation buffer data.\n");
        return STARCHCAT_EXIT_FAILURE;
    }

#ifdef DEBUG
    fprintf(stderr, "ALLOC'ed retransformedLineBuffer\n");
#endif
    
    zStream->next_in = zInBuf;

#ifdef __cplusplus
    zStream->avail_in = static_cast<unsigned int>( fread(zInBuf, sizeof(unsigned char), STARCH_Z_IN_BUFFER_MAX_LENGTH, *inputFp) );
#else
    zStream->avail_in = (unsigned int) fread(zInBuf, sizeof(unsigned char), STARCH_Z_IN_BUFFER_MAX_LENGTH, *inputFp);
#endif

#ifdef DEBUG
    fprintf(stderr, "READ bytes into zStream\n");
#endif

    if (zStream->avail_in == 0) {
#ifdef __cplusplus
        free(zInBuf);
        zInBuf = nullptr;
        free(zReadBuf);
        zReadBuf = nullptr;
#else
        free(zInBuf);
        zInBuf = NULL;
        free(zReadBuf);
        zReadBuf = NULL;
#endif
        *eofFlag = kStarchTrue;
        return STARCHCAT_EXIT_SUCCESS;
    }

#ifdef DEBUG
    if (feof(*inputFp))
        fprintf(stderr, "EOF\n");
#endif

    do {
#ifdef DEBUG
        fprintf(stderr, "Z-LOOP\n");
#endif
        zStream->avail_out = STARCH_Z_CHUNK;
        zStream->next_out = zReadBuf;
        zError = inflate(zStream, feof(*inputFp) ? Z_FINISH : Z_NO_FLUSH);

        if ((zError != Z_OK) && (zError != Z_STREAM_END)) {
            switch (zError) {
                case Z_STREAM_ERROR: {
                    fprintf(stderr, "ERROR: Z-stream clobbered!\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
                case Z_NEED_DICT: {
                    zError = Z_DATA_ERROR;
                    inflateEnd(zStream);
                    fprintf(stderr, "ERROR: Could not complete extraction of input stream! (Z_NEED_DICT)\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
                case Z_DATA_ERROR: {
                    inflateEnd(zStream);
                    fprintf(stderr, "ERROR: Could not complete extraction of input stream! (Z_DATA_ERROR)\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
                case Z_MEM_ERROR: {
                    inflateEnd(zStream);
                    fprintf(stderr, "ERROR: Could not complete extraction of input stream! (Z_MEM_ERROR)\n");
                    return STARCHCAT_EXIT_FAILURE;
                }
                default: {
                    fprintf(stderr, "ERROR: Unknown z-inflate error (%d)!\n", zError);
                    return STARCHCAT_EXIT_FAILURE;
                }
            }
        }

        *nZRead = STARCH_Z_CHUNK - zStream->avail_out;

        if (zError == Z_STREAM_END)
            *eofFlag = kStarchTrue;

        zCharIndex = 0;

        if (*nZRemainderBuf > 0) {
#ifdef DEBUG
            fprintf(stderr,"\tadding remainder [%zd | %s] to zLineBuf\n", *nZRemainderBuf, *zRemainderBuf);
#endif
            memcpy(zLineBuf, *zRemainderBuf, *nZRemainderBuf);
            zCharIndex = *nZRemainderBuf;
            memset(*zRemainderBuf, 0, *nZRemainderBuf);
            *nZRemainderBuf = 0;
        }

        *t_nExtractionBuffer = 0;
        *t_nExtractionBufferPos = 0;

        zBufIndex = 0;

        while (zBufIndex < *nZRead) {
            zLineBuf[zCharIndex++] = zReadBuf[zBufIndex++];
            if (zLineBuf[zCharIndex - 1] == '\n') {
                zLineBuf[zCharIndex - 1] = '\0';
#ifdef DEBUG
                fprintf(stderr, "\tzLineBuf -> [%s]\n", zLineBuf);
#endif
                UNSTARCH_extractRawLine( recordChromosome,
                                         zLineBuf,
                                         tab,
                                         t_startPtr, 
                                         t_pLengthPtr, 
                                         t_lastEndPtr,
                                         t_firstInputToken, 
                                         t_secondInputToken,
                                         &t_currentChromosome, 
                                         t_currentChromosomeLengthPtr, 
                                         t_currentStartPtr, 
                                         t_currentStopPtr,
                                         &t_currentRemainder, 
                                         t_currentRemainderLengthPtr);
                if (zLineBuf[0] != 'p') {
                    (*t_lineIdxPtr)++;
#ifdef __cplusplus
                    UNSTARCH_reverseTransformCoordinates( static_cast<const LineCountType>( *t_lineIdxPtr ),
#else
                    UNSTARCH_reverseTransformCoordinates( (const LineCountType) *t_lineIdxPtr,
#endif
                                                          t_lastPositionPtr,
                                                          t_lcDiffPtr,
                                                          t_currentStartPtr, 
                                                          t_currentStopPtr, 
                                                          &t_currentRemainder, 
                                                          retransformedLineBuffer, 
                                                          &nRetransformedLineBuffer, 
                                                          &nRetransformedLineBufferPosition);

                    /* resize the extraction buffer, if we're getting too close to the maximum size of a line */
                    if ((*nExtractionBuffer - *t_nExtractionBufferPos) < TOKENS_MAX_LENGTH) {
#ifdef __cplusplus
                        nResizedExtractionBuffer = static_cast<size_t>( *nExtractionBuffer ) * 2;
                        resizedExtractionBuffer = static_cast<char *>( realloc(extractionBuffer, nResizedExtractionBuffer) );
#else
                        nResizedExtractionBuffer = (size_t) *nExtractionBuffer * 2;
                        resizedExtractionBuffer = realloc(extractionBuffer, nResizedExtractionBuffer);
#endif
                        if (!resizedExtractionBuffer) {
                            fprintf(stderr, "ERROR: Could not allocate space for resized gzip extraction buffer!\n");
                            return STARCHCAT_EXIT_FAILURE;
                        }
                        extractionBuffer = resizedExtractionBuffer;
                        // to reduce the possibility of overflow, we first test before casting
                        if (nResizedExtractionBuffer >= INT_MAX) {
                            fprintf(stderr, "ERROR: nResizedExtractionBuffer is a larger value than nExtractionBuffer can hold!\n");
                            return STARCHCAT_EXIT_FAILURE;
                        }
                        *nExtractionBuffer = nResizedExtractionBuffer;
                    }

#ifdef __cplusplus
                    *t_nExtractionBuffer = (t_currentRemainder) ? 
                        static_cast<size_t>( sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                             "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", 
                             t_currentChromosome, 
                             *t_currentStartPtr, 
                             *t_currentStopPtr, 
                             t_currentRemainder) ) 
            : 
                        static_cast<size_t>( sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                             "%s\t%" PRId64 "\t%" PRId64 "\n", 
                             t_currentChromosome, 
                             *t_currentStartPtr, 
                             *t_currentStopPtr) );
#else
                    *t_nExtractionBuffer = (t_currentRemainder) ? 
                        (size_t) sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                     "%s\t%" PRId64 "\t%" PRId64 "\t%s\n", 
                     t_currentChromosome, 
                     *t_currentStartPtr, 
                     *t_currentStopPtr, 
                     t_currentRemainder) 
            : 
                        (size_t) sprintf(extractionBuffer + *t_nExtractionBufferPos, 
                     "%s\t%" PRId64 "\t%" PRId64 "\n", 
                     t_currentChromosome, 
                     *t_currentStartPtr, 
                     *t_currentStopPtr);
#endif
                    *t_nExtractionBufferPos += *t_nExtractionBuffer;
                    *(extractionBufferStart + *t_nExtractionBufferPos) = '\0';
                }
                t_firstInputToken[0] = '\0';
                t_secondInputToken[0] = '\0';
                zCharIndex = 0;
            }
        }
        zLineBuf[zCharIndex] = '\0';
    } while (zStream->avail_out == 0);

#ifdef __cplusplus
    strncpy(static_cast<char *>( *zRemainderBuf ), const_cast<const char *>( reinterpret_cast<char *>( zLineBuf ) ), static_cast<size_t>( zCharIndex ) + 1);
#else
    strncpy((char *) *zRemainderBuf, (const char *) zLineBuf, (size_t) zCharIndex + 1);
#endif

    *nZRemainderBuf = zCharIndex;

#ifdef DEBUG
    fprintf(stderr, "zRemainderBuf -> %s | length -> %zd\n", *zRemainderBuf, *nZRemainderBuf);
#endif

    /* cleanup */
#ifdef __cplusplus
    if (zInBuf) {
        free(zInBuf);
        zInBuf = nullptr;
    }

    if (zReadBuf) {
        free(zReadBuf);
        zReadBuf = nullptr;
    }

    if (retransformedLineBuffer) {
        free(retransformedLineBuffer);
        retransformedLineBuffer = nullptr;
    }
#else
    if (zInBuf) {
        free(zInBuf);
        zInBuf = NULL;
    }

    if (zReadBuf) {
        free(zReadBuf);
        zReadBuf = NULL;
    }

    if (retransformedLineBuffer) {
        free(retransformedLineBuffer);
        retransformedLineBuffer = NULL;
    }
#endif

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_extractBedLine(Boolean *eobFlag, char *extractionBuffer, int *extractionBufferOffset, char **extractedElement) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_extractBedLine() ---\n");
#endif

#ifdef __cplusplus
    size_t previousOffset = static_cast<size_t>( *extractionBufferOffset );
#else
    size_t previousOffset = (size_t) *extractionBufferOffset;
#endif
    size_t size = 0U;

    if (*eobFlag == kStarchTrue)
        return STARCHCAT_EXIT_SUCCESS;

    if (extractionBuffer[*extractionBufferOffset] == '\0') {
        *eobFlag = kStarchTrue;
        memset(*extractedElement, 0, strlen(*extractedElement));
        return STARCHCAT_EXIT_SUCCESS;
    }

    while (extractionBuffer[*extractionBufferOffset] != '\n') {
        size++;
        (*extractionBufferOffset)++;
    }
    size++;
    (*extractionBufferOffset)++;

    memcpy(*extractedElement, extractionBuffer + previousOffset, size);
    (*extractedElement)[size] = '\0';

    return STARCHCAT_EXIT_SUCCESS;
}

int
STARCHCAT2_parseCoordinatesFromBedLineV2(Boolean *eobFlag, const char *extractedElement, SignedCoordType *start, SignedCoordType *stop)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_parseCoordinatesFromBedLineV2() ---\n");
#endif

    if (strlen(extractedElement) == 0) {
#ifdef DEBUG
        fprintf(stderr, "LEAVING EARLY\n");
#endif
        *eobFlag = kStarchTrue;
        return STARCHCAT_EXIT_SUCCESS;
    }

    errno = 0;
    int fieldIdx = 0;
    int charIdx = 0;
    int withinFieldIdx = 0;
    static const char tab = '\t';
    char startStr[MAX_DEC_INTEGERS + 1] = {0};
    char stopStr[MAX_DEC_INTEGERS + 1] = {0};
    SignedCoordType result = 0;

    while (extractedElement[charIdx] != '\0') {
        if (extractedElement[charIdx] == tab) {
            withinFieldIdx = 0;
            fieldIdx++;
            charIdx++;
            continue;
        }
        switch (fieldIdx) {
    case 1: {
        startStr[withinFieldIdx++] = extractedElement[charIdx];
        startStr[withinFieldIdx] = '\0';
        break;
    }
    case 2: {
        stopStr[withinFieldIdx++] = extractedElement[charIdx];
        stopStr[withinFieldIdx] = '\0';
        break;
    }
    default:
        break;
        }
        charIdx++;
    }

#ifdef __cplusplus
    result = static_cast<SignedCoordType>( strtoll(startStr, nullptr, STARCH_RADIX) );
#else
    result = (SignedCoordType) strtoll(startStr, NULL, STARCH_RADIX);
#endif

    switch (errno) {
        case EINVAL: {
            fprintf(stderr, "ERROR: Result from parsing start coordinate is not a valid number!\n");
            return STARCH_EXIT_FAILURE;
        }
        case ERANGE: {
            fprintf(stderr, "ERROR: Result from parsing start coordinate is not within range of SignedCoordType (int64_t)!\n");
            return STARCH_EXIT_FAILURE;
        }
    }
    *start = result;

#ifdef __cplusplus
    result = static_cast<SignedCoordType>( strtoll(stopStr, nullptr, STARCH_RADIX) );
#else
    result = (SignedCoordType) strtoll(stopStr, NULL, STARCH_RADIX);
#endif

    switch (errno) {
        case EINVAL: {
            fprintf(stderr, "ERROR: Result from parsing stop coordinate is not a valid number!\n");
            return STARCH_EXIT_FAILURE;
        }
        case ERANGE: {
            fprintf(stderr, "ERROR: Result from parsing stop coordinate is not within range of SignedCoordType (int64_t)!\n");
            return STARCH_EXIT_FAILURE;
        }
    }
    *stop = result;

    return STARCH_EXIT_SUCCESS;
}

int
STARCHCAT2_parseCoordinatesFromBedLineV2p2(Boolean *eobFlag, const char *extractedElement, SignedCoordType *start, SignedCoordType *stop, char **remainder)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_parseCoordinatesFromBedLineV2p2() ---\n");
#endif

    if (strlen(extractedElement) == 0) {
#ifdef DEBUG
        fprintf(stderr, "LEAVING EARLY\n");
#endif
        *eobFlag = kStarchTrue;
        return STARCHCAT_EXIT_SUCCESS;
    }

    errno = 0;
    int fieldIdx = 0;
    int charIdx = 0;
    int withinFieldIdx = 0;
    static const char tab = '\t';
    char startStr[MAX_DEC_INTEGERS + 1] = {0};
    char stopStr[MAX_DEC_INTEGERS + 1] = {0};
    SignedCoordType result = 0;

    while (extractedElement[charIdx] != '\0') {
        if (extractedElement[charIdx] == tab) {
            if (fieldIdx < 3) {
                withinFieldIdx = 0;
                fieldIdx++;
            }
            charIdx++;
            continue;
        }
        switch (fieldIdx) {
        case 1: {
            startStr[withinFieldIdx++] = extractedElement[charIdx];
            startStr[withinFieldIdx] = '\0';
            break;
        }
        case 2: {
            stopStr[withinFieldIdx++] = extractedElement[charIdx];
            stopStr[withinFieldIdx] = '\0';
            break;
        }
        case 3: {
            (*remainder)[withinFieldIdx++] = extractedElement[charIdx];
            (*remainder)[withinFieldIdx] = '\0';
            break;
        }
        default:
            break;
        }
        charIdx++;
    }

#ifdef __cplusplus
    result = static_cast<SignedCoordType>( strtoll(startStr, nullptr, STARCH_RADIX) );
#else
    result = (SignedCoordType) strtoll(startStr, NULL, STARCH_RADIX);
#endif

    switch (errno) {
        case EINVAL: {
            fprintf(stderr, "ERROR: Result from parsing start coordinate is not a valid number!\n");
            return STARCH_EXIT_FAILURE;
        }
        case ERANGE: {
            fprintf(stderr, "ERROR: Result from parsing start coordinate is not within range of SignedCoordType (int64_t)!\n");
            return STARCH_EXIT_FAILURE;
        }
    }
    *start = result;

#ifdef __cplusplus
    result = static_cast<SignedCoordType>( strtoll(stopStr, nullptr, STARCH_RADIX) );
#else
    result = (SignedCoordType) strtoll(stopStr, NULL, STARCH_RADIX);
#endif

    switch (errno) {
        case EINVAL: {
            fprintf(stderr, "ERROR: Result from parsing stop coordinate is not a valid number!\n");
            return STARCH_EXIT_FAILURE;
        }
        case ERANGE: {
            fprintf(stderr, "ERROR: Result from parsing stop coordinate is not within range of SignedCoordType (int64_t)!\n");
            return STARCH_EXIT_FAILURE;
        }
    }
    *stop = result;

    return STARCH_EXIT_SUCCESS;
}

int
STARCHCAT2_addLowestBedElementToCompressionBuffer (char *compressionBuffer, const char *extractedElement, LineCountType *compressionLineCount)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_addLowestBedElementToCompressionBuffer() ---\n");
#endif

    size_t compressionBufferLength = strlen(compressionBuffer);
    size_t extractedElementLength = strlen(extractedElement);

    memcpy(compressionBuffer + compressionBufferLength, extractedElement, extractedElementLength);
    compressionBuffer[compressionBufferLength + extractedElementLength] = '\0';
    (*compressionLineCount)++;

    return STARCH_EXIT_SUCCESS;
}

int
STARCHCAT2_transformCompressionBuffer (const char *compBuf, char *retransBuf, TransformState *retransState)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_transformCompressionBuffer() ---\n");
#endif

#if HAS_GNU == 0
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpointer-bool-conversion"
#endif
    if ((!retransState->r_chromosome) || (strlen(retransState->r_chromosome) == 0))
        return STARCH_EXIT_FAILURE;
#if HAS_GNU == 0
#pragma clang diagnostic pop
#endif

    static const char tab                                  = '\t';
    size_t nCompBuf                                        = strlen(compBuf);
    size_t compBufIdx                                      = 0U;
    /* size_t *nRetransBuf                                 = 0U; */
    size_t *nRetransBuf                                    = &retransState->r_nRetransBuf;
#ifdef DEBUG
    int nChar                                              = 0;
#endif

    char retransLineBuf[TOKENS_MAX_LENGTH + 1]             = {0};
    size_t retransLineBufIdx                               = 0U;
    LineCountType retransLineIdx                           = 0;

    /* retransform parameters */
    char *retransChromosome                                = STARCH_strdup(retransState->r_chromosome);
    char *retransRemainder                                 = STARCH_strdup(retransState->r_remainder);
    SignedCoordType *retransStart                          = &retransState->r_start;
    SignedCoordType *retransStop                           = &retransState->r_stop;
    SignedCoordType *retransPStart                         = &retransState->r_pStart;
    SignedCoordType *retransPStop                          = &retransState->r_pStop;
    SignedCoordType *retransCoordDiff                      = &retransState->r_coordDiff;
    SignedCoordType *retransLcDiff                         = &retransState->r_lcDiff;
    SignedCoordType *retransLastPosition                   = &retransState->r_lastPosition;
    SignedCoordType *retransPreviousStop                   = &retransState->r_previousStop;
    BaseCountType   *retransTotalNonUniqueBases            = &retransState->r_totalNonUniqueBases;
    BaseCountType   *retransTotalUniqueBases               = &retransState->r_totalUniqueBases;
    Boolean         *retransDuplicateElementExists         = &retransState->r_duplicateElementExists;
    Boolean         *retransNestedElementExists            = &retransState->r_nestedElementExists;

#ifdef DEBUG
    fprintf(stderr, "retransform parameters ->\nretransChromosome - [%s]\nretransRemainder - [%s]\nretransStart - [%" PRId64 "]\nretransStop - [%" PRId64 "]\nretransCoordDiff - [%" PRId64 "]\nretransLcDiff - [%" PRId64 "]\nretransLastPosition - [%" PRId64 "]\nretransPreviousStop - [%" PRId64 "]\nretransTotalNonUniqueBases - [%" PRIu64 "]\nretransTotalUniqueBases - [%" PRIu64 "]\n", retransChromosome, retransRemainder, *retransStart, *retransStop, *retransCoordDiff, *retransLcDiff, *retransLastPosition, *retransPreviousStop, *retransTotalNonUniqueBases, *retransTotalUniqueBases);
    fprintf(stderr, "COMPRESSION BUFFER -> [%s]\n", compBuf);
#endif

    *nRetransBuf = strlen(retransBuf);

    for (compBufIdx = 0U; compBufIdx < nCompBuf; compBufIdx++) {
        retransLineBuf[retransLineBufIdx++] = compBuf[compBufIdx];
        if (compBuf[compBufIdx] == '\n') {
            retransLineIdx++;
            retransLineBuf[retransLineBufIdx - 1] = '\0';
            retransLineBufIdx = 0U;
#ifdef DEBUG
            fprintf(stderr, "compressing -> [%s]\n", retransLineBuf);
#endif
            if (STARCH_createTransformTokensForHeaderlessInput(retransLineBuf, 
                                                               tab, 
                                                               &retransChromosome, 
                                                               retransStart, 
                                                               retransStop, 
                                                               &retransRemainder) == 0) 
            {
                if (*retransStop > *retransStart)
                    *retransCoordDiff = *retransStop - *retransStart;
                else {
                    fprintf(stderr, "ERROR: BED data is corrupt (stop: %" PRId64 " should be greater than start: %" PRId64 ")\n", *retransStop, *retransStart);
                    exit(-1);
                    //return STARCHCAT_EXIT_FAILURE;                        
                }
                if (*retransCoordDiff != *retransLcDiff) {
                    *retransLcDiff = *retransCoordDiff;
#ifdef DEBUG
                    fprintf(stderr, "\told *nRetransBuf -> %zu\n", *nRetransBuf);
                    fprintf(stderr, "\told retransBuf -> [%s]\n", retransBuf);
#endif
#ifndef DEBUG
                    sprintf(retransBuf + *nRetransBuf, "p%" PRId64 "\n", *retransCoordDiff);
#else
                    nChar = sprintf(retransBuf + *nRetransBuf, "p%" PRId64 "\n", *retransCoordDiff);
                    fprintf(stderr, "\twrote %d characters to retransBuf\n", nChar);
                    fprintf(stderr, "\tsprintf'ing - [p%" PRId64 "]\n", *retransCoordDiff);
                    fprintf(stderr, "\tpost-p retransBuf -> [%s]\n", retransBuf);
#endif
                    *nRetransBuf = strlen(retransBuf);
#ifdef DEBUG
                    fprintf(stderr, "\tnew *nRetransBuf -> %zu\n", *nRetransBuf);
                    fprintf(stderr, "\tnew retransBuf -> [%s]\n", retransBuf);
#endif
                }
                if (*retransLastPosition != 0) {
                    if (retransRemainder) {
                        sprintf(retransBuf + *nRetransBuf, "%" PRId64 "\t%s\n", (*retransStart - *retransLastPosition), retransRemainder);
#ifdef DEBUG
                        fprintf(stderr, "\tsprintf'ing - [%" PRId64 "]-tab-[%s]\n", (*retransStart - *retransLastPosition), retransRemainder);
#endif
                    }
                    else {
                        sprintf(retransBuf + *nRetransBuf, "%" PRId64 "\n", (*retransStart - *retransLastPosition));
#ifdef DEBUG
                        fprintf(stderr, "\tsprintf'ing - [%" PRId64 "]\n", *retransStart - *retransLastPosition);
#endif
                    }
                }
                else {
                    if (retransRemainder) {
                        sprintf(retransBuf + *nRetransBuf, "%" PRId64 "\t%s\n", *retransStart, retransRemainder);
#ifdef DEBUG
                        fprintf(stderr, "\tsprintf'ing - [%" PRId64 "]-tab-[%s]\n", *retransStart, retransRemainder);
#endif
                    }
                    else {
                        sprintf(retransBuf + *nRetransBuf, "%" PRId64 "\n", *retransStart);
#ifdef DEBUG
                        fprintf(stderr, "\tsprintf'ing - [%" PRId64 "]\n", *retransStart);
#endif
                    }
                }
                *nRetransBuf = strlen(retransBuf);

                /* statistics */
                *retransLastPosition = *retransStop;
#ifdef __cplusplus
                *retransTotalNonUniqueBases += static_cast<BaseCountType>( *retransStop - *retransStart );
                if (*retransPreviousStop <= *retransStart)
                    *retransTotalUniqueBases += static_cast<BaseCountType>( *retransStop - *retransStart );
                else if (*retransPreviousStop < *retransStop)
                    *retransTotalUniqueBases += static_cast<BaseCountType>( *retransStop - *retransPreviousStop );
#else
                *retransTotalNonUniqueBases += (BaseCountType) (*retransStop - *retransStart);
                if (*retransPreviousStop <= *retransStart)
                    *retransTotalUniqueBases += (BaseCountType) (*retransStop - *retransStart);
                else if (*retransPreviousStop < *retransStop)
                    *retransTotalUniqueBases += (BaseCountType) (*retransStop - *retransPreviousStop);
#endif
                *retransPreviousStop = (*retransStop > *retransPreviousStop) ? *retransStop : *retransPreviousStop;

                /* test for duplicate element */
                if ((*retransPStart == *retransStart) && (*retransPStop == *retransStop))
                    *retransDuplicateElementExists = kStarchTrue;

                /* test for nested element */
                if ((*retransPStart < *retransStart) && (*retransPStop > *retransStop))
                    *retransNestedElementExists = kStarchTrue;

                /* set pElement values */
                *retransPStart = *retransStart;
                *retransPStop = *retransStop;
#ifdef DEBUG
                fprintf(stderr, "\tretransLineBuf - [%s]\n", retransLineBuf);
#endif
            }
        }
    }

#ifdef __cplusplus
    if (retransChromosome) {
        free(retransChromosome);
        retransChromosome = nullptr;
    }
    if (retransRemainder) {
        free(retransRemainder);
        retransRemainder = nullptr;
    }
#else
    if (retransChromosome) {
        free(retransChromosome);
        retransChromosome = NULL;
    }
    if (retransRemainder) {
        free(retransRemainder);
        retransRemainder = NULL;
    }
#endif

    return STARCH_EXIT_SUCCESS;
}

int
STARCHCAT2_squeezeRetransformedOutputBufferToBzip2Stream(BZFILE **bzStream, char *transformedBuffer)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_squeezeRetransformedOutputBufferToBzip2Stream() ---\n");
#endif

    int bzError = BZ_OK;
    size_t nTransformedBuffer = 0;

    if (transformedBuffer)
        nTransformedBuffer = strlen(transformedBuffer);

    if ((transformedBuffer) && (*bzStream) && (nTransformedBuffer > 0)) {
#ifdef __cplusplus
        BZ2_bzWrite(&bzError, *bzStream, transformedBuffer, static_cast<int>( nTransformedBuffer ));
#else
        BZ2_bzWrite(&bzError, *bzStream, transformedBuffer, (int) nTransformedBuffer);
#endif

        if (bzError != BZ_OK) {
            switch (bzError) {
                case BZ_PARAM_ERROR: {
                    fprintf(stderr, "ERROR: Stream is NULL, transformedBuffer is NULL, or transformedBuffer length is negative\n");
                    break;
                }
                case BZ_SEQUENCE_ERROR: {
                    fprintf(stderr, "ERROR: Bzip2 streams are out of sequence\n");
                    break;
                }
                case BZ_IO_ERROR: {
                    fprintf(stderr, "ERROR: There is an error writing the compressed data to the bz stream\n");
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR: Unknown error with BZ2_bzWrite() (err: %d)\n", bzError);
                    break;
                }
            }
            return STARCH_EXIT_FAILURE;
        }
    }

    return STARCH_EXIT_SUCCESS;
}

int      
STARCHCAT2_squeezeRetransformedOutputBufferToGzipStream (z_stream *zStream, const Boolean flushZStreamFlag, char *transformedBuffer, uint64_t *finalStreamSize, size_t *cumulativeOutputSize)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_squeezeRetransformedOutputBufferToGzipStream() ---\n");
#endif

    z_stream *zStreamPtr = zStream;
    int zError = Z_OK;
    size_t zOutHave;
    FILE *outFp = stdout;
    unsigned char zBuffer[STARCH_Z_BUFFER_MAX_LENGTH] = {0};

#ifdef DEBUG
    fprintf(stderr, "transformedBuffer: [%s]\n", transformedBuffer);
#endif

#ifdef __cplusplus
    zStreamPtr->next_in = reinterpret_cast<unsigned char *>( transformedBuffer );
    zStreamPtr->avail_in = static_cast<unsigned int>( strlen(transformedBuffer) );
#else
    zStreamPtr->next_in = (unsigned char *) transformedBuffer;
    zStreamPtr->avail_in = (unsigned int) strlen(transformedBuffer);
#endif
    do {
        zStreamPtr->avail_out = STARCH_Z_BUFFER_MAX_LENGTH;
        zStreamPtr->next_out = zBuffer;
        zError = deflate(zStreamPtr, (flushZStreamFlag == kStarchFalse) ? Z_NO_FLUSH : Z_FINISH);
        switch (zError) {
            case Z_MEM_ERROR: {
                fprintf(stderr, "ERROR: Not enough memory to compress data\n");
                return STARCHCAT_EXIT_FAILURE;
            }
            case Z_BUF_ERROR:
            default:
                break;
        }
#ifdef __cplusplus
        zOutHave = static_cast<size_t>( STARCH_Z_BUFFER_MAX_LENGTH - zStreamPtr->avail_out );
#else
        zOutHave = (size_t) (STARCH_Z_BUFFER_MAX_LENGTH - zStreamPtr->avail_out);
#endif
        *finalStreamSize += zOutHave;
        *cumulativeOutputSize += zOutHave;
        fwrite(zBuffer, sizeof(unsigned char), zOutHave, outFp);
        fflush(outFp);
#ifdef DEBUG
        fprintf(stderr, "POST zStream->avail_out -> %d\n", zStreamPtr->avail_out);
#endif
    } while (zStreamPtr->avail_out == 0);

    return STARCH_EXIT_SUCCESS;
}

int
STARCHCAT2_resetCompressionBuffer(char *compressionBuffer, LineCountType *compressionLineCount)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_resetCompressionBuffer() ---\n");
#endif

    compressionBuffer[0] = '\0';
    *compressionLineCount = 0;

    return STARCH_EXIT_SUCCESS;
}

int
STARCHCAT2_finalizeMetadata(Metadata **outMd, char *finalChromosome, char *finalOutTagFn, uint64_t finalStreamSize, LineCountType finalLineCount, uint64_t finalTotalNonUniqueBases, uint64_t finalTotalUniqueBases, Boolean finalDuplicateElementExists, Boolean finalNestedElementExists, char *finalSignature, LineLengthType finalLineMaxStringLength)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCHCAT2_finalizeMetadata() ---\n");
#endif

    if (!*outMd)
        *outMd = STARCH_createMetadata( finalChromosome, 
                                        finalOutTagFn, 
                                        finalStreamSize, 
                                        finalLineCount, 
                                        finalTotalNonUniqueBases, 
                                        finalTotalUniqueBases,
                                        finalDuplicateElementExists,
                                        finalNestedElementExists,
                                        finalSignature,
                                        finalLineMaxStringLength );
    else
        *outMd = STARCH_addMetadata( *outMd, 
                                     finalChromosome, 
                                     finalOutTagFn, 
                                     finalStreamSize, 
                                     finalLineCount, 
                                     finalTotalNonUniqueBases, 
                                     finalTotalUniqueBases,
                                     finalDuplicateElementExists,
                                     finalNestedElementExists,
                                     finalSignature,
                                     finalLineMaxStringLength );

    return STARCH_EXIT_SUCCESS;
}

#ifdef __cplusplus
} // namespace starch
#endif
