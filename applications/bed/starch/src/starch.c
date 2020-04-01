//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starch.c
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

#include "starch.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/starch/starchHelpers.h"
#include "data/starch/starchConstants.h"
#include "data/starch/starchFileHelpers.h"
#include "suite/BEDOPS.Version.hpp"

#ifdef __cplusplus
namespace {
  using namespace starch;
} // unnamed namespace
#endif

int
main(int argc, char **argv)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- starch main() - enter ---\n");
#endif
#ifdef __cplusplus
    char *note = nullptr;
    char *tag = nullptr;
    char *bedFn = nullptr;
    FILE *bedFnPtr = nullptr;
    Metadata *metadata = nullptr;
    unsigned char *starchHeader = nullptr;
#else
    char *note = NULL;
    char *tag = NULL;
    char *bedFn = NULL;
    FILE *bedFnPtr = NULL;
    Metadata *metadata = NULL;
    unsigned char *starchHeader = NULL;
#endif
    int parseValue = 0;
    CompressionType type;
    Boolean bedGeneratePerChrSignatureFlag = kStarchFalse;
    Boolean bedReportProgressFlag = kStarchFalse;
    LineCountType bedReportProgressN = 0;
    Boolean bedHeaderFlag = kStarchFalse;

    setlocale (LC_ALL, "POSIX");

    parseValue = STARCH_parseCommandLineOptions (argc, argv);
    switch (parseValue) {
        case STARCH_HELP_ERROR: {
            STARCH_printUsage(STARCH_HELP_ERROR);
            return EXIT_SUCCESS;
        }
        case STARCH_VERSION_ERROR: {
            STARCH_printRevision();
            return EXIT_SUCCESS;
        }
        case STARCH_FATAL_ERROR: {
            STARCH_printUsage(STARCH_FATAL_ERROR);
            return EXIT_FAILURE;
        }
    }
    note = starch_client_global_args.note;
    bedFn = starch_client_global_args.inputFile;
    type = starch_client_global_args.compressionType;
    tag = starch_client_global_args.uniqueTag;
    bedGeneratePerChrSignatureFlag = starch_client_global_args.generatePerChromosomeSignatureFlag;
    bedReportProgressFlag = starch_client_global_args.reportProgressFlag;
    bedReportProgressN = starch_client_global_args.reportProgressN;
    bedHeaderFlag = starch_client_global_args.headerFlag;

    if (STARCH_MAJOR_VERSION == 1)
    {
        if (strcmp (bedFn, "-") == 0) // this condition is preserved in case of test-builds of legacy Starch binaries
        {
            /* process stdin */
            if ((bedHeaderFlag == kStarchTrue) &&
#ifdef __cplusplus
                 (STARCH_transformInput(&metadata, 
                                       nullptr, 
                                       static_cast<const CompressionType>( type ), 
                                       reinterpret_cast<const char *>( tag ), 
                                       reinterpret_cast<const char *>( note )) != 0))
#else
                 (STARCH_transformInput(&metadata, 
                                       NULL, 
                                       (const CompressionType) type, 
                                       (const char *) tag, 
                                       (const char *) note) != 0))
#endif
                exit (EXIT_FAILURE);
            else if ((bedHeaderFlag == kStarchFalse) &&
#ifdef __cplusplus
                (STARCH_transformHeaderlessInput(&metadata, 
                                                 nullptr, 
                                                 static_cast<const CompressionType>( type ), 
                                                 reinterpret_cast<const char *>( tag ), 
                                                 static_cast<const Boolean>( kStarchFinalizeTransformTrue ), 
                                                 reinterpret_cast<const char *>( note )) != 0))
#else
                (STARCH_transformHeaderlessInput(&metadata, 
                                                 NULL, 
                                                 (const CompressionType) type, 
                                                 (const char *) tag, 
                                                 (const Boolean) kStarchFinalizeTransformTrue, 
                                                 (const char *) note) != 0))
#endif
                exit (EXIT_FAILURE);
        }
        else {
            /* process file input */
            bedFnPtr = STARCH_fopen (bedFn, "r");
            if (!bedFnPtr) {
                fprintf (stderr, "ERROR: Could not open bed file %s\n", bedFn);
                exit (EXIT_FAILURE);
            }
            if ((bedHeaderFlag == kStarchTrue) &&
#ifdef __cplusplus
                (STARCH_transformInput(&metadata, 
                                       reinterpret_cast<const FILE *>( bedFnPtr ), 
                                       static_cast<const CompressionType>( type ), 
                                       reinterpret_cast<const char *>( tag ), 
                                       reinterpret_cast<const char *>( note )) != 0))
#else
                (STARCH_transformInput(&metadata, 
                                       (const FILE *) bedFnPtr, 
                                       (const CompressionType) type, 
                                       (const char *) tag, 
                                       (const char *) note) != 0))
#endif
                exit (EXIT_FAILURE);
            else if ((bedHeaderFlag == kStarchFalse) &&
#ifdef __cplusplus
                (STARCH_transformHeaderlessInput(&metadata, 
                                                 reinterpret_cast<const FILE *>( bedFnPtr ), 
                                                 static_cast<const CompressionType>( type ), 
                                                 reinterpret_cast<const char *>( tag ), 
                                                 static_cast<const Boolean>( kStarchFinalizeTransformTrue ), 
                                                 reinterpret_cast<const char *>( note )) != 0))
#else
                (STARCH_transformHeaderlessInput(&metadata, 
                                                 (const FILE *) bedFnPtr, 
                                                 (const CompressionType) type, 
                                                 (const char *) tag, 
                                                 (const Boolean) kStarchFinalizeTransformTrue, 
                                                 (const char *) note) != 0))
#endif
                exit (EXIT_FAILURE);
        }
    }
    else if (STARCH_MAJOR_VERSION == 2) {
        if (strcmp (bedFn, "-") == 0)
            bedFnPtr = stdin;
        else {
            bedFnPtr = STARCH_fopen (bedFn, "r");
            if (!bedFnPtr) {
                fprintf (stderr, "ERROR: Could not open input BED file pointer from (%s)\n", bedFn);
                exit (EXIT_FAILURE);
            }
        }

#ifdef __cplusplus
        if (STARCH2_transformInput(&starchHeader, 
                                   &metadata, 
                                   reinterpret_cast<const FILE *>( bedFnPtr ), 
                                   static_cast<const CompressionType>( type ), 
                                   reinterpret_cast<const char *>( tag ), 
                                   reinterpret_cast<const char *>( note ), 
                                   static_cast<const Boolean>( bedGeneratePerChrSignatureFlag ),
                                   static_cast<const Boolean>( bedHeaderFlag ),
                                   static_cast<const Boolean>( bedReportProgressFlag ),
                                   static_cast<const LineCountType>( bedReportProgressN )) != STARCH_EXIT_SUCCESS) 
        {
            exit (EXIT_FAILURE);
        }
#else
        if (STARCH2_transformInput(&starchHeader, 
                                   &metadata, 
                                   (const FILE *) bedFnPtr, 
                                   (const CompressionType) type, 
                                   (const char *) tag, 
                                   (const char *) note, 
                                   (const Boolean) bedGeneratePerChrSignatureFlag,
                                   (const Boolean) bedHeaderFlag,
                                   (const Boolean) bedReportProgressFlag,
                                   (const LineCountType) bedReportProgressN) != STARCH_EXIT_SUCCESS) 
        {
            exit (EXIT_FAILURE);
        }
#endif
    }

    else if (STARCH_MAJOR_VERSION > 2) {
        fprintf (stderr, "ERROR: Starch does not yet support making archives in this major version release (built as: %d.%d.%d)\n", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION); // this condition is preserved in case of test-builds of future-proofed Starch binaries
        exit (EXIT_FAILURE);
    }

    /* cleanup */
#ifdef __cplusplus
    if (bedFnPtr != nullptr) {
        fclose (bedFnPtr);
        bedFnPtr = nullptr;
    }
    if (metadata != nullptr)
        STARCH_freeMetadata (&metadata);
    if (starchHeader) {
        free (starchHeader);
        starchHeader = nullptr;
    }
    if (starch_client_global_args.uniqueTag) {
        free (starch_client_global_args.uniqueTag);
        starch_client_global_args.uniqueTag = nullptr;
    }
#else
    if (bedFnPtr != NULL) {
        fclose (bedFnPtr);
        bedFnPtr = NULL;
    }
    if (metadata != NULL)
        STARCH_freeMetadata (&metadata);
    if (starchHeader) {
        free (starchHeader);
        starchHeader = NULL;
    }
    if (starch_client_global_args.uniqueTag) {
        free (starch_client_global_args.uniqueTag);
        starch_client_global_args.uniqueTag = NULL;
    }
#endif

#ifdef DEBUG
    fprintf (stderr, "\n--- starch main() - exit ---\n");
#endif

    return EXIT_SUCCESS;
}

#ifdef __cplusplus
namespace starch {
#endif

void
STARCH_initializeGlobals()
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCH_initializeGlobals() ---\n");
#endif
#ifdef __cplusplus
    starch_client_global_args.note = nullptr;
    starch_client_global_args.inputFile = nullptr;
    starch_client_global_args.uniqueTag = nullptr;
    starch_client_global_args.inputFiles = nullptr;
#else
    starch_client_global_args.note = NULL;
    starch_client_global_args.inputFile = NULL;
    starch_client_global_args.uniqueTag = NULL;
    starch_client_global_args.inputFiles = NULL;
#endif
    starch_client_global_args.compressionType = STARCH_DEFAULT_COMPRESSION_TYPE;
    starch_client_global_args.generatePerChromosomeSignatureFlag = kStarchTrue;
    starch_client_global_args.reportProgressFlag = kStarchFalse;
    starch_client_global_args.reportProgressN = 0;
    starch_client_global_args.headerFlag = kStarchFalse;
    starch_client_global_args.numberInputFiles = 0;
}

int
STARCH_parseCommandLineOptions(int argc, char **argv)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCH_parseCommandLineOptions()  ---\n");
#endif

    int starch_client_long_index;
    int starch_client_opt = getopt_long (argc, argv, starch_client_opt_string, starch_client_long_options, &starch_client_long_index);

    if (argc > 6) {
        fprintf (stderr, "ERROR: Wrong number of arguments.\n");
        return STARCH_FATAL_ERROR;
    }

    /* disable error reporting by GNU getopt -- we handle this */
    opterr = 0;
    STARCH_initializeGlobals ();

    while (starch_client_opt != -1) {
        switch (starch_client_opt) {
        case 'v':
            return STARCH_VERSION_ERROR;
        case 'n':
            starch_client_global_args.note = optarg;
            break;
        case 'b':
            starch_client_global_args.compressionType = kBzip2;
            break;
        case 'g':
            starch_client_global_args.compressionType = kGzip;
            break;
        case 'o':
            starch_client_global_args.generatePerChromosomeSignatureFlag = kStarchFalse;
            break;
        case 'r':
            starch_client_global_args.reportProgressFlag = kStarchTrue;
            errno = 0;
#ifdef __cplusplus
            starch_client_global_args.reportProgressN = strtoumax(optarg, nullptr, 10);
#else
            starch_client_global_args.reportProgressN = strtoumax(optarg, NULL, 10);
#endif
            if (errno == ERANGE) {
                fprintf (stderr, "ERROR: Numerical value is outside of range.\n");
                return STARCH_FATAL_ERROR;
            }
            break;
        case 'e':
            starch_client_global_args.headerFlag = kStarchTrue;
            break;
        case 'h':
            return STARCH_HELP_ERROR;
        case '?':
            return STARCH_FATAL_ERROR;
        default:
            break;
        }
        starch_client_opt = getopt_long (argc, argv, starch_client_opt_string, starch_client_long_options, &starch_client_long_index);
    }

    STARCH_buildProcessIDTag (&(starch_client_global_args.uniqueTag));

    starch_client_global_args.inputFiles = argv + optind;
#ifdef __cplusplus
    starch_client_global_args.numberInputFiles = static_cast<size_t>( argc - optind );
#else
    starch_client_global_args.numberInputFiles = (size_t) (argc - optind);
#endif

    switch (starch_client_global_args.numberInputFiles) {
        case 0: {
            fprintf (stderr, "ERROR: Wrong number of arguments.\n");
            return STARCH_FATAL_ERROR;
        }
        case 1: {
            starch_client_global_args.inputFile = *(starch_client_global_args.inputFiles);
            break;
        }
        case 2: {
            starch_client_global_args.uniqueTag = *(starch_client_global_args.inputFiles);
            starch_client_global_args.inputFile = *(starch_client_global_args.inputFiles + 1);
            break;
        }
        default:
            break;
    }

#ifdef __cplusplus
    return static_cast<int>( kStarchTrue );
#else
    return (int) kStarchTrue;
#endif
}

void
STARCH_printUsage(int errorType)
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCH_printUsage() ---\n");
#endif
#ifdef __cplusplus
    char *avStr = nullptr;
    avStr = static_cast<char *>( malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH) );
    if (avStr != nullptr) {
#else
    char *avStr = NULL;
    avStr = malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH);
    if (avStr != NULL) {
#endif
        int result = sprintf (avStr, "%d.%d.%d", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        if (result != -1) {
            switch (errorType) {
                case STARCH_FATAL_ERROR:
                case STARCH_HELP_ERROR:
                default: {
                    fprintf(stderr, "%s\n  citation: %s\n  binary version: %s (creates archive version: %s)\n  authors:  %s\n%s\n\n", name, BEDOPS::citation(), BEDOPS::version(), avStr, authors, usage);
                    break;
                }
            }
        }
        free (avStr);
    }
}

void
STARCH_printRevision()
{
#ifdef DEBUG
    fprintf (stderr, "\n--- STARCH_printRevision() ---\n");
#endif
#ifdef __cplusplus
    char *avStr = nullptr;
    avStr = static_cast<char *>( malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH) );
    if (avStr != nullptr) {
#else
    char *avStr = NULL;
    avStr = malloc (STARCH_ARCHIVE_VERSION_STRING_LENGTH);
    if (avStr != NULL) {
#endif
        int result = sprintf (avStr, "%d.%d.%d", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        if (result != -1)
            fprintf (stderr, "%s\n  binary version: %s (creates archive version: %s)\n", name, BEDOPS::version(), avStr);
        free (avStr);
    }
}

#ifdef __cplusplus
} // unnamed namespace
#endif
