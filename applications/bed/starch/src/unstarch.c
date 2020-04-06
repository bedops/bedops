//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    unstarch.c
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
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>
#else
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#endif

#include "unstarch.h"

#include "data/starch/starchBase64Coding.h"
#include "data/starch/starchSha1Digest.h"
#include "data/starch/unstarchHelpers.h"
#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchHelpers.h"
#include "suite/BEDOPS.Version.hpp"

#ifdef __cplusplus
namespace {
  using namespace starch;
} //  unnamed
#endif

int
main(int argc, char **argv)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- unstarch main() enter ---\n");
#endif
#ifdef __cplusplus
    char *archiveTimestamp = nullptr;
    char *note = nullptr;
    char *whichChromosome = nullptr;
    char *inFile = nullptr;
    char *option = nullptr;
    FILE *inFilePtr = nullptr;
    Metadata *records = nullptr;
    ArchiveVersion *archiveVersion = nullptr;
    json_t *metadataJSON = nullptr;
    char *jsonString = nullptr;
#else
    char *archiveTimestamp = NULL;
    char *note = NULL;
    char *whichChromosome = NULL;
    char *inFile = NULL;
    char *option = NULL;
    FILE *inFilePtr = NULL;
    Metadata *records = NULL;
    ArchiveVersion *archiveVersion = NULL;
    json_t *metadataJSON = NULL;
    char *jsonString = NULL;
#endif
    int resultValue = 0;
    int parseValue = 0;
    CompressionType type = STARCH_DEFAULT_COMPRESSION_TYPE;
    uint64_t metadataOffset = UINT64_C(0);
    Boolean headerFlag = kStarchFalse;
    Boolean showNewlineFlag = kStarchTrue;
    const Boolean suppressErrorMsgs = kStarchFalse; /* we want to see error messages */
    const Boolean preserveJSONRef = kStarchFalse; /* we generally do not want to preserve JSON reference */
    unsigned char mdHashBuffer[STARCH2_MD_FOOTER_SHA1_LENGTH + 1] = {0};
    Boolean signatureVerificationFlag = kStarchFalse;

    /*
        unstarch overview

     -- check command-line inputs
     -- extract and parse metadata
     -- print list of chromosomes if --list/--listJSON/--list-chromosomes 
         option is set; else, extract data to stdout

    */

    setlocale(LC_ALL, "POSIX");
    if (UNSTARCH_parseCommandLineInputs( argc, argv, &whichChromosome, &inFile, &option, &parseValue ) != 0) {
        switch (parseValue) {
            case 0: {
                resultValue = EXIT_FAILURE;
                break;
            }
            case UNSTARCH_HELP_ERROR: {
                resultValue = UNSTARCH_HELP_ERROR;
                break;
            }
            case UNSTARCH_VERSION_ERROR: {
                resultValue = UNSTARCH_VERSION_ERROR;
                break;
            }
            case UNSTARCH_IS_STARCH_ARCHIVE_ERROR: {
                resultValue = UNSTARCH_IS_STARCH_ARCHIVE_ERROR;
                break;
            }
            case UNSTARCH_ARCHIVE_VERSION_ERROR: {
                resultValue = UNSTARCH_ARCHIVE_VERSION_ERROR;
                break;
            }
            case UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR: {
                resultValue = UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR;
                break;
            }
            case UNSTARCH_ARCHIVE_NOTE_ERROR: {
                resultValue = UNSTARCH_ARCHIVE_NOTE_ERROR;
                break;
            }
            case UNSTARCH_ARCHIVE_COMPRESSION_TYPE_ERROR: {
                resultValue = UNSTARCH_ARCHIVE_COMPRESSION_TYPE_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_COUNT_ALL_ERROR: {
                resultValue = UNSTARCH_ELEMENT_COUNT_ALL_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_COUNT_CHR_ERROR: {
                resultValue = UNSTARCH_ELEMENT_COUNT_CHR_ERROR;
                break;
            }
            case UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR: {
                resultValue = UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR;
                break;
            }
            case UNSTARCH_BASES_COUNT_ALL_ERROR: {
                resultValue = UNSTARCH_BASES_COUNT_ALL_ERROR;
                break;
            }
            case UNSTARCH_BASES_COUNT_CHR_ERROR: {
                resultValue = UNSTARCH_BASES_COUNT_CHR_ERROR;
                break;
            }
            case UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR: {
                resultValue = UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR;
                break;
            }
            case UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR: {
                resultValue = UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR;
                break;
            }
            case UNSTARCH_LIST_CHROMOSOMES_ERROR: {
                resultValue = UNSTARCH_LIST_CHROMOSOMES_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_CHR_INT_ERROR: {
                resultValue = UNSTARCH_ELEMENT_DUPLICATE_CHR_INT_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_ALL_INT_ERROR: {
                resultValue = UNSTARCH_ELEMENT_DUPLICATE_ALL_INT_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_CHR_STR_ERROR: {
                resultValue = UNSTARCH_ELEMENT_DUPLICATE_CHR_STR_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_ALL_STR_ERROR: {
                resultValue = UNSTARCH_ELEMENT_DUPLICATE_ALL_STR_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_CHR_INT_ERROR: {
                resultValue = UNSTARCH_ELEMENT_NESTED_CHR_INT_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_ALL_INT_ERROR: {
                resultValue = UNSTARCH_ELEMENT_NESTED_ALL_INT_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_CHR_STR_ERROR: {
                resultValue = UNSTARCH_ELEMENT_NESTED_CHR_STR_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_ALL_STR_ERROR: {
                resultValue = UNSTARCH_ELEMENT_NESTED_ALL_STR_ERROR;
                break;
            }
            case UNSTARCH_SIGNATURE_ERROR: {
                resultValue = UNSTARCH_SIGNATURE_ERROR;
                break;
            }
            case UNSTARCH_SIGNATURE_VERIFY_ERROR: {
                resultValue = UNSTARCH_SIGNATURE_VERIFY_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_MAX_STRING_LENGTH_CHR_ERROR: {
                resultValue = UNSTARCH_ELEMENT_MAX_STRING_LENGTH_CHR_ERROR;
                break;
            }
            case UNSTARCH_ELEMENT_MAX_STRING_LENGTH_ALL_ERROR: {
                resultValue = UNSTARCH_ELEMENT_MAX_STRING_LENGTH_ALL_ERROR;
                break;
            }
        }
    }

    if ((resultValue == 0) ||
        (resultValue == UNSTARCH_ARCHIVE_VERSION_ERROR) ||
        (resultValue == UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_COUNT_ALL_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_COUNT_CHR_ERROR) ||
        (resultValue == UNSTARCH_BASES_COUNT_ALL_ERROR) ||
        (resultValue == UNSTARCH_BASES_COUNT_CHR_ERROR) ||
        (resultValue == UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR) ||
        (resultValue == UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR) ||
        (resultValue == UNSTARCH_LIST_CHROMOSOMES_ERROR) ||
        (resultValue == UNSTARCH_ARCHIVE_NOTE_ERROR) ||
        (resultValue == UNSTARCH_ARCHIVE_COMPRESSION_TYPE_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_CHR_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_ALL_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_CHR_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_ALL_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_CHR_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_ALL_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_CHR_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_ALL_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_MAX_STRING_LENGTH_CHR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_MAX_STRING_LENGTH_ALL_ERROR))
    {
        if (STARCH_readJSONMetadata( &metadataJSON,
                     &inFilePtr,
#ifdef __cplusplus
                     reinterpret_cast<const char *>( inFile ),
#else
                     (const char *) inFile,
#endif
                     &records,
                     &type,
                     &archiveVersion,
                     &archiveTimestamp,
                     &note,
                     &metadataOffset,
                     &headerFlag,
                     suppressErrorMsgs,
                     preserveJSONRef) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not read metadata\n");
            resultValue = EXIT_FAILURE;
        }
    }
    else if (resultValue == UNSTARCH_IS_STARCH_ARCHIVE_ERROR)
    {
        /* we suppress warnings from STARCH_readJSONMetadata() */
        if (STARCH_JSONMetadataExists( &inFilePtr,
#ifdef __cplusplus
                     reinterpret_cast<const char *>( inFile )
#else
                     (const char *) inFile
#endif
                     ) != STARCH_EXIT_SUCCESS) {
            fprintf(stdout, "0\n"); /* false -- no valid metadata, therefore not a starch archive */
            return EXIT_SUCCESS;
        }
        else {
            fprintf(stdout, "1\n"); /* true -- valid metadata, therefore a starch archive */
            return EXIT_SUCCESS;
        }
    }
    else if ( (resultValue == UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR) || (resultValue == UNSTARCH_SIGNATURE_ERROR) || (resultValue == UNSTARCH_SIGNATURE_VERIFY_ERROR) )
    {
        if (STARCH_readJSONMetadata( &metadataJSON,
                     &inFilePtr,
#ifdef __cplusplus
                     reinterpret_cast<const char *>( inFile ),
#else
                     (const char *) inFile,
#endif
                     &records,
                     &type,
                     &archiveVersion,
                     &archiveTimestamp,
                     &note,
                     &metadataOffset,
                     &headerFlag,
                     suppressErrorMsgs,
                     kStarchTrue) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Could not read metadata\n");
            resultValue = EXIT_FAILURE;
        }

#ifdef __cplusplus
        jsonString = static_cast<char *>( json_dumps(metadataJSON, JSON_INDENT(2)|JSON_PRESERVE_ORDER) );
#else
        jsonString = (char *) json_dumps(metadataJSON, JSON_INDENT(2)|JSON_PRESERVE_ORDER);
#endif

#ifdef DEBUG
        fprintf(stderr, "%s\n", jsonString);
#endif
        if (jsonString) {
#ifdef __cplusplus
            STARCH_SHA1_All(reinterpret_cast<const unsigned char *>( reinterpret_cast<unsigned char *>( jsonString ) ),
                strlen(jsonString),
                mdHashBuffer);
            free(jsonString);
            jsonString = nullptr;
            json_decref(metadataJSON);
            metadataJSON = nullptr;
#else
            STARCH_SHA1_All((const unsigned char *) jsonString,
                strlen(jsonString),
                mdHashBuffer);
            free(jsonString);
            jsonString = NULL;
            json_decref(metadataJSON);
            metadataJSON = NULL;
#endif
        }
        else {
            fprintf(stderr, "ERROR: Could not encode JSON structure into string representation\n");
            resultValue = EXIT_FAILURE;
        }
    }

    if (resultValue == 0) {

        if (option && (strcmp(option, "list-json-no-trailing-newline") == 0))
            showNewlineFlag = kStarchFalse;

        if (option && (strcmp(option, "list") == 0)) {
            if (STARCH_listMetadata(records, whichChromosome) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Metadata extraction failed\n");
                UNSTARCH_printUsage(EXIT_FAILURE);
                return EXIT_FAILURE;
            }
        }
        else if (option && ((strcmp(option, "listJSON") == 0) || (strcmp(option, "list-json") == 0) || (strcmp(option, "list-json-no-trailing-newline") == 0))) {
            if (strcmp(whichChromosome, "all") != 0) {
                fprintf(stderr, "ERROR: JSON listing cannot be limited to one chromosome; please remove chromosome name from options.\n");
                UNSTARCH_printUsage(EXIT_FAILURE);
                return EXIT_FAILURE;
            }
#ifdef __cplusplus
            if (STARCH_listJSONMetadata(nullptr, 
					nullptr, 
					records, 
					type, 
					archiveVersion, 
					archiveTimestamp, 
					note, 
					headerFlag, 
					showNewlineFlag) != STARCH_EXIT_SUCCESS) {
#else
            if (STARCH_listJSONMetadata(NULL, 
					NULL, 
					records, 
					type, 
					archiveVersion, 
					archiveTimestamp, 
					note, 
					headerFlag, 
					showNewlineFlag) != STARCH_EXIT_SUCCESS) {
#endif
                fprintf(stderr, "ERROR: Metadata extraction failed\n");
                resultValue = EXIT_FAILURE;
                UNSTARCH_printUsage(EXIT_FAILURE);
            }
        }
        else if (option && (strcmp(option, "help") == 0))
            resultValue = UNSTARCH_HELP_ERROR;
        else if (option && (strcmp(option, "version") == 0))
            resultValue = UNSTARCH_VERSION_ERROR;
        else if (option && ((strcmp(option, "archiveVersion") == 0) || (strcmp(option, "archive-version") == 0)))
            resultValue = UNSTARCH_ARCHIVE_VERSION_ERROR;
        else {
            if ((STARCH_MAJOR_VERSION == 1) || (archiveVersion->major == 1)) {
                switch (type) {
                    case kBzip2: {
#ifdef __cplusplus
                        if (UNSTARCH_extractDataWithBzip2(&inFilePtr,
                                                          nullptr,
                                                          whichChromosome,
                                                          reinterpret_cast<const Metadata *>( records ),
                                                          static_cast<const unsigned long long>( metadataOffset ),
                                                          static_cast<const Boolean>( headerFlag )) != 0) {
#else
                        if (UNSTARCH_extractDataWithBzip2(&inFilePtr,
                                                          NULL,
                                                          whichChromosome,
                                                          (const Metadata *) records,
                                                          (const unsigned long long) metadataOffset,
                                                          (const Boolean) headerFlag) != 0) {
#endif
                            fprintf(stderr, "ERROR: Backend extraction failed (bzip2)\n");
                            resultValue = EXIT_FAILURE;
                        }
                        break;
                    }
                    case kGzip: {
#ifdef __cplusplus
                        if (UNSTARCH_extractDataWithGzip(&inFilePtr,
                                                         nullptr,
                                                         whichChromosome,
                                                         reinterpret_cast<const Metadata *>( records ),
                                                         static_cast<const unsigned long long>( metadataOffset ),
                                                         static_cast<const Boolean>( headerFlag )) != 0) {
#else
                        if (UNSTARCH_extractDataWithGzip(&inFilePtr,
                                                         NULL,
                                                         whichChromosome,
                                                         (const Metadata *) records,
                                                         (const unsigned long long) metadataOffset,
                                                         (const Boolean) headerFlag) != 0) {
#endif
                            fprintf(stderr, "ERROR: Backend extraction failed (gzip)\n");
                            resultValue = EXIT_FAILURE;
                        }
                        break;
                    }
                    case kUndefined: {
                        fprintf(stderr, "ERROR: Backend compression type is undefined\n");
                        resultValue = EXIT_FAILURE;
                        break;
                    }
                }
            }
            else if ((STARCH_MAJOR_VERSION == 2) || (archiveVersion->major == 2)) { // warning is false-positive
                switch (type) {
                    case kBzip2: {
#ifdef __cplusplus
                        if (UNSTARCH_extractDataWithBzip2(&inFilePtr,
                                                          nullptr,
                                                          whichChromosome,
                                                          reinterpret_cast<const Metadata *>( records ),
                                                          static_cast<const unsigned long long>( sizeof(starchRevision2HeaderBytes) ),
                                                          static_cast<const Boolean>( headerFlag )) != 0) {
#else
                        if (UNSTARCH_extractDataWithBzip2(&inFilePtr,
                                                          NULL,
                                                          whichChromosome,
                                                          (const Metadata *) records,
                                                          (const unsigned long long) sizeof(starchRevision2HeaderBytes),
                                                          (const Boolean) headerFlag) != 0) {
#endif
                            fprintf(stderr, "ERROR: Backend extraction failed (bzip2)\n");
                            resultValue = EXIT_FAILURE;
                        }
                        break;
                    }
                    case kGzip: {
#ifdef __cplusplus
                        if (UNSTARCH_extractDataWithGzip(&inFilePtr,
                                                         nullptr,
                                                         whichChromosome,
                                                         reinterpret_cast<const Metadata *>( records ),
                                                         static_cast<const unsigned long long>( sizeof(starchRevision2HeaderBytes) ),
                                                         static_cast<const Boolean>( headerFlag )) != 0) {
#else
                        if (UNSTARCH_extractDataWithGzip(&inFilePtr,
                                                         NULL,
                                                         whichChromosome,
                                                         (const Metadata *) records,
                                                         (const unsigned long long) sizeof(starchRevision2HeaderBytes),
                                                         (const Boolean) headerFlag) != 0) {
#endif
                            fprintf(stderr, "ERROR: Backend extraction failed (gzip)\n");
                            resultValue = EXIT_FAILURE;
                        }
                        break;
                    }
                    case kUndefined: {
                        fprintf(stderr, "ERROR: Backend compression type is undefined\n");
                        resultValue = EXIT_FAILURE;
                        break;
                    }
                }
            }
            else if (STARCH_MAJOR_VERSION > 2) {
                fprintf(stderr, "ERROR: Unstarch does not support extracting archives in this major version release (built as: %d.%d.%d)\n", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION); // this is preserved for future-proofing test builds of Starch binaries
                exit(EXIT_FAILURE);
            }
        }
    }

    else if (resultValue == EXIT_FAILURE)
        UNSTARCH_printUsage(EXIT_FAILURE);

    else if (resultValue == UNSTARCH_HELP_ERROR) {
        if (strcmp(whichChromosome, "all") != 0) {
            fprintf(stderr, "ERROR: Please remove chromosome from options\n");
            UNSTARCH_printUsage(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        if (inFile) {
            fprintf(stderr, "ERROR: Please remove filename from options\n");
            UNSTARCH_printUsage(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        UNSTARCH_printUsage(UNSTARCH_HELP_ERROR);
    }

    else if (resultValue == UNSTARCH_VERSION_ERROR) {
        if (strcmp(whichChromosome, "all") != 0) {
            fprintf(stderr, "ERROR: Please remove chromosome from options\n");
            UNSTARCH_printUsage(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        if (inFile) {
            fprintf(stderr, "ERROR: Please remove filename from options\n");
            UNSTARCH_printUsage(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        UNSTARCH_printRevision();
    }

    else if (resultValue == UNSTARCH_ARCHIVE_VERSION_ERROR) {
        if (strcmp(whichChromosome, "all") != 0) {
            fprintf(stderr, "ERROR: Please remove chromosome from options\n");
            UNSTARCH_printUsage(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
#ifdef __cplusplus
        UNSTARCH_printArchiveVersion(const_cast<const ArchiveVersion *>( archiveVersion ));
#else
        UNSTARCH_printArchiveVersion((const ArchiveVersion *) archiveVersion);
#endif
    }

    else if (resultValue == UNSTARCH_LIST_CHROMOSOMES_ERROR) {
        if (strcmp(whichChromosome, "all") != 0) {
            if (STARCH_listChromosome(records, whichChromosome) != STARCH_EXIT_SUCCESS) {
                fprintf(stderr, "ERROR: Metadata extraction failed\n");
                UNSTARCH_printUsage(EXIT_FAILURE);
                return EXIT_FAILURE;
            }
        }
        else if (STARCH_listAllChromosomes(records) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "ERROR: Metadata extraction failed\n");
            resultValue = EXIT_FAILURE;
        }
    }

    else if (resultValue == UNSTARCH_ARCHIVE_COMPRESSION_TYPE_ERROR) {
        if (strcmp(whichChromosome, "all") != 0) {
            fprintf(stderr, "ERROR: Please remove chromosome from options\n");
            UNSTARCH_printUsage(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
#ifdef __cplusplus
        UNSTARCH_printCompressionType(static_cast<const CompressionType>( type ));
#else
        UNSTARCH_printCompressionType((const CompressionType) type);
#endif
    }

    /* test version numbering against toolkit */
    else {
        switch (resultValue) {
            case UNSTARCH_ELEMENT_COUNT_CHR_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 3)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)))
                    UNSTARCH_printLineCountForChromosome(records, whichChromosome);
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support element counting (starchcat the archive to bring its version to v1.3.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_COUNT_ALL_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 3)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)))
                    UNSTARCH_printLineCountForAllChromosomes(records);
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support element counting (starchcat the archive to bring its version to v1.3.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_BASES_COUNT_CHR_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 4)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)))
                    UNSTARCH_printNonUniqueBaseCountForChromosome(records, whichChromosome);
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support base counting (starchcat the archive to bring its version to v1.4.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_BASES_COUNT_ALL_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 4)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)))
                    UNSTARCH_printNonUniqueBaseCountForAllChromosomes(records);
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support base counting (starchcat the archive to bring its version to v1.4.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 4)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)))
                    UNSTARCH_printUniqueBaseCountForChromosome(records, whichChromosome);
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support base counting (starchcat the archive to bring its version to v1.4.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 4)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)))
                    UNSTARCH_printUniqueBaseCountForAllChromosomes(records);
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support base counting (starchcat the archive to bring its version to v1.4.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 5)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0))) {
                    if (strcmp(whichChromosome, "all") != 0) {
                        fprintf(stderr, "ERROR: Please remove chromosome from options\n");
                        UNSTARCH_printUsage(EXIT_FAILURE);
                        return EXIT_FAILURE;
                    }
#ifdef __cplusplus
                    UNSTARCH_printArchiveTimestamp(const_cast<const char *>( archiveTimestamp ));
#else
                    UNSTARCH_printArchiveTimestamp((const char *)archiveTimestamp);
#endif
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support archive creation timestamping (starchcat the archive to bring its version to v1.5.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ARCHIVE_NOTE_ERROR: {
                if (((archiveVersion->major == 1) && (archiveVersion->minor >= 5)) ||
                    ((archiveVersion->major == 2) && (archiveVersion->minor >= 0))) {
                    if (note) {
                        if (strcmp(whichChromosome, "all") != 0) {
                            fprintf(stderr, "ERROR: Please remove chromosome from options\n");
                            UNSTARCH_printUsage(EXIT_FAILURE);
                            return EXIT_FAILURE;
                        }
#ifdef __cplusplus
                        UNSTARCH_printNote(const_cast<const char *>( note ));
#else
                        UNSTARCH_printNote((const char *)note);
#endif
                    }
                    else {
                        fprintf(stderr, "ERROR: Archive does not contain a note annotation (use --list-json to view attributes)\n");
                        resultValue = EXIT_FAILURE;
                    }
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support note annotations (starchcat the archive to bring its version to v1.5.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 0)) {
                    if (strcmp(whichChromosome, "all") != 0) {
                        fprintf(stderr, "ERROR: Please remove chromosome from options\n");
                        UNSTARCH_printUsage(EXIT_FAILURE);
                        return EXIT_FAILURE;
                    }
                    UNSTARCH_printMetadataSha1Signature(mdHashBuffer);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support metadata signing (starchcat the archive to bring its version to v2.0.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_CHR_INT_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printDuplicateElementExistsIntegerForChromosome(records, whichChromosome);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support duplicate element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_ALL_INT_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printDuplicateElementExistsIntegersForAllChromosomes(records);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support duplicate element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_CHR_STR_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printDuplicateElementExistsStringForChromosome(records, whichChromosome);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support duplicate element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_DUPLICATE_ALL_STR_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printDuplicateElementExistsStringsForAllChromosomes(records);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support duplicate element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_CHR_INT_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printNestedElementExistsIntegerForChromosome(records, whichChromosome);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support nested element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_ALL_INT_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printNestedElementExistsIntegersForAllChromosomes(records);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support nested element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_CHR_STR_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printNestedElementExistsStringForChromosome(records, whichChromosome);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support nested element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_NESTED_ALL_STR_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 1)) {
                    UNSTARCH_printNestedElementExistsStringsForAllChromosomes(records);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support nested element flag (starchcat the archive to bring its version to v2.1.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_SIGNATURE_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 2)) {
                    if (whichChromosome) {
                        UNSTARCH_printSignature(records, whichChromosome, mdHashBuffer);
                    }
                    else {
                        UNSTARCH_printMetadataSha1Signature(mdHashBuffer);
                    }
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support per-chromosome signatures (starchcat or extract/recompress the archive to bring its version to v2.2.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_SIGNATURE_VERIFY_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 2)) {
                    if (whichChromosome) {
                        signatureVerificationFlag = UNSTARCH_verifySignature(&inFilePtr,
                                                                             records,
#ifdef __cplusplus
                                                                             static_cast<const unsigned long long>( sizeof(starchRevision2HeaderBytes) ),
#else
                                                                             (const unsigned long long) sizeof(starchRevision2HeaderBytes),
#endif
                                                                             whichChromosome,
                                                                             type);
                    }
                    else {
                        signatureVerificationFlag = UNSTARCH_verifyAllSignatures(&inFilePtr,
                                                                                 records,
#ifdef __cplusplus
                                                                                 static_cast<const unsigned long long>( sizeof(starchRevision2HeaderBytes) ),
#else
                                                                                 (const unsigned long long) sizeof(starchRevision2HeaderBytes),
#endif
                                                                                 type);
                    }
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support per-chromosome signature verification (starchcat or extract/recompress the archive to bring its version to v2.2.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_MAX_STRING_LENGTH_CHR_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 2)) {
                    UNSTARCH_printLineMaxStringLengthForChromosome(records, whichChromosome);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support element maximum string length reporting (starchcat or extract/recompress the archive to bring its version to v2.2.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
            case UNSTARCH_ELEMENT_MAX_STRING_LENGTH_ALL_ERROR: {
                if ((archiveVersion->major == 2) && (archiveVersion->minor >= 2)) {
                    UNSTARCH_printLineMaxStringLengthForAllChromosomes(records);
                }
                else {
                    fprintf(stderr, "ERROR: Archive version (%d.%d.%d) does not support element maximum string length reporting (starchcat or extract/recompress the archive to bring its version to v2.2.0 or greater)\n", archiveVersion->major, archiveVersion->minor, archiveVersion->revision);
                    resultValue = EXIT_FAILURE;
                }
                break;
            }
        }
    }

    if ((resultValue == UNSTARCH_SIGNATURE_VERIFY_ERROR) && (signatureVerificationFlag == kStarchFalse)) {
        resultValue = EXIT_FAILURE;
    }
    else if ((resultValue == UNSTARCH_HELP_ERROR) ||
        (resultValue == UNSTARCH_VERSION_ERROR) ||
        (resultValue == UNSTARCH_ARCHIVE_VERSION_ERROR) ||
        (resultValue == UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_COUNT_CHR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_COUNT_ALL_ERROR) ||
        (resultValue == UNSTARCH_BASES_COUNT_CHR_ERROR) ||
        (resultValue == UNSTARCH_BASES_COUNT_ALL_ERROR) ||
        (resultValue == UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR) ||
        (resultValue == UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR) ||
        (resultValue == UNSTARCH_LIST_CHROMOSOMES_ERROR) ||
        (resultValue == UNSTARCH_ARCHIVE_NOTE_ERROR) ||
        (resultValue == UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_CHR_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_ALL_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_CHR_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_DUPLICATE_ALL_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_CHR_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_ALL_INT_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_CHR_STR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_NESTED_ALL_STR_ERROR) ||
        (resultValue == UNSTARCH_SIGNATURE_ERROR) ||
        (resultValue == UNSTARCH_SIGNATURE_VERIFY_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_MAX_STRING_LENGTH_CHR_ERROR) ||
        (resultValue == UNSTARCH_ELEMENT_MAX_STRING_LENGTH_ALL_ERROR) )
    {
        resultValue = EXIT_SUCCESS;
    }

    /* cleanup */
    if (option)
        free(option);
    if (whichChromosome)
        free(whichChromosome);
    if (inFile)
        free(inFile);
    if (inFilePtr)
        fclose(inFilePtr);
    if (records)
        STARCH_freeMetadata(&records);
    if (archiveVersion)
        free(archiveVersion);
    if (archiveTimestamp)
        free(archiveTimestamp);

#ifdef DEBUG
    fprintf(stderr, "\n--- unstarch main() exit ---\n");
#endif

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    return resultValue;
}

#ifdef __cplusplus
namespace starch {
#endif

int
UNSTARCH_parseCommandLineInputs(int argc, char **argv, char **chr, char **fn, char **optn, int *pval)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_parseCommandLineInputs() ---\n");
#endif

#ifdef __cplusplus
    char *hdr1 = nullptr;
    char *ftr1 = nullptr;
    char *hdr2 = nullptr;
    char *ftr2 = nullptr;
#else
    char *hdr1 = NULL;
    char *ftr1 = NULL;
    char *hdr2 = NULL;
    char *ftr2 = NULL;
#endif

#ifdef DEBUG
    fprintf(stderr, "\targc: %d\n", argc);
    for (int i = 0; i < argc; i++)
        fprintf(stderr, "\targv[%d]: %s\n", i, argv[i]);
#endif

    if ((argc <= 1) || (argc > 4)) {
        fprintf(stderr, "ERROR: Wrong number of arguments\n");
        return UNSTARCH_FATAL_ERROR;
    }

    else if (argc == 2) {
        if (strlen(argv[1]) == 1) {
            if (strcmp(argv[1], "-") == 0) {
                fprintf(stderr, "ERROR: Unstarch does not take standard input at this time\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        hdr1 = UNSTARCH_strndup(argv[1], 2);
        if (strcmp(hdr1, "--") == 0) {
            free(hdr1);
            ftr1 = UNSTARCH_strndup(argv[1] + 2, strlen(argv[1]) - 2);
            if (strcmp(ftr1, "help") == 0) {
                *optn = STARCH_strdup(ftr1);
                *chr = STARCH_strdup("all");
                free(ftr1);
                *pval = UNSTARCH_HELP_ERROR;
                return UNSTARCH_HELP_ERROR;
            }
            else if (strcmp(ftr1, "version") == 0) {
                *optn = STARCH_strdup(ftr1);
                *chr = STARCH_strdup("all");
                free(ftr1);
                *pval = UNSTARCH_VERSION_ERROR;
                return UNSTARCH_VERSION_ERROR;
            }
            else {
                fprintf(stderr, "ERROR: Please specify a filename or the --help or --version options\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        else {
            free(hdr1);
            *chr = STARCH_strdup("all");
            *fn = STARCH_strdup(argv[1]);
        }
    }

    else if (argc == 3) {
        if (strlen(argv[1]) == 1) {
            if (strcmp(argv[1], "-") == 0) {
                fprintf(stderr, "ERROR: Unstarch does not take standard input at this time\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        if (strlen(argv[2]) == 1) {
            if (strcmp(argv[2], "-") == 0) {
                fprintf(stderr, "ERROR: Unstarch does not take standard input at this time\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        hdr1 = UNSTARCH_strndup(argv[1], 2);
        if (strcmp(hdr1, "--") == 0) {
            hdr2 = UNSTARCH_strndup(argv[2], 2);
            if (strcmp(hdr2, "--") == 0) {
                free(hdr2);
                fprintf(stderr, "ERROR: Cannot mix arguments\n");
                return UNSTARCH_FATAL_ERROR;
            }
            ftr1 = UNSTARCH_strndup(argv[1] + 2, strlen(argv[1]) - 2);
            *chr = STARCH_strdup("all");
            *fn = STARCH_strdup(argv[2]);
        }
        else {
            if ((hdr1[0] == '-') && (hdr1[1] != '-')) {
                free(hdr1);
                fprintf(stderr, "ERROR: Malformed argument\n");
                return UNSTARCH_FATAL_ERROR;
            }
            if (strlen(argv[2]) > 1) {
                hdr2 = UNSTARCH_strndup(argv[2], 2);
                if (strcmp(hdr2, "--") == 0) {
                    free(hdr2);
                    fprintf(stderr, "ERROR: Please specify a filename\n");
                    return UNSTARCH_FATAL_ERROR;
                }
                free(hdr2);
            }
            *chr = STARCH_strdup(argv[1]);
            *fn = STARCH_strdup(argv[2]);
        }
        free(hdr1);
    }

    else if (argc == 4) {
        if (strlen(argv[1]) == 1) {
            if (strcmp(argv[1], "-") == 0) {
                fprintf(stderr, "ERROR: Unstarch does not take standard input at this time\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        if (strlen(argv[2]) == 1) {
            if (strcmp(argv[2], "-") == 0) {
                fprintf(stderr, "ERROR: Unstarch does not take standard input at this time\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        if (strlen(argv[3]) == 1) {
            if (strcmp(argv[3], "-") == 0) {
                fprintf(stderr, "ERROR: Unstarch does not take standard input at this time\n");
                return UNSTARCH_FATAL_ERROR;
            }
        }
        hdr1 = UNSTARCH_strndup(argv[1], 2);
        if (strcmp(hdr1, "--") == 0) {
            free(hdr1);
            fprintf(stderr, "ERROR: Cannot place option before chromosome name\n");
            return UNSTARCH_FATAL_ERROR;
        }
        else if ((hdr1[0] == '-') && (hdr1[1] != '-')) {
            free(hdr1);
            fprintf(stderr, "ERROR: Malformed argument\n");
            return UNSTARCH_FATAL_ERROR;
        }
        free(hdr1);
        hdr2 = UNSTARCH_strndup(argv[2], 2);
        if (strcmp(hdr2, "--") == 0) {
            free(hdr2);
            ftr2 = UNSTARCH_strndup(argv[2] + 2, strlen(argv[2]) - 2);
        }
        else {
            free(hdr2);
            fprintf(stderr, "ERROR: Must place option after chromosome name\n");
            return UNSTARCH_FATAL_ERROR;
        }
        *chr = STARCH_strdup(argv[1]);
        *fn = STARCH_strdup(argv[3]);
    }

#ifdef DEBUG
    fprintf(stderr, "chr -> %s \t ftr1 -> %s \t ftr2 -> %s \t fn -> %s \n", *chr, ftr1, ftr2, *fn);
#endif


    if ( (! *fn)                                               ||
         (! *chr)                                              ||
         (strcmp(*fn, "-") == 0)                               ||
         (strcmp(*fn, "--list") == 0)                          ||
         (strcmp(*fn, "--listJSON") == 0)                      ||
         (strcmp(*fn, "--list-json") == 0)                     ||
         (strcmp(*fn, "--list-json-no-trailing-newline") == 0) ||
         (strcmp(*fn, "--note") == 0)                          ||
         (strcmp(*fn, "--archive-type") == 0)                  ||
         (strcmp(*fn, "--archive-version") == 0)               ||
         (strcmp(*fn, "--archive-timestamp") == 0)             ||
         (strcmp(*fn, "--sha1-signature") == 0)                ||
         (strcmp(*fn, "--signature") == 0)                     ||
         (strcmp(*fn, "--verify-signature") == 0)              ||
         (strcmp(*fn, "--is-starch") == 0) )
    {
        if (ftr1)
            free(ftr1);
        if (ftr2)
            free(ftr2);
        return UNSTARCH_FATAL_ERROR;
    }

    if (ftr1 && ftr2) {
        fprintf(stderr, "ERROR: Something went wrong with parsing command-line arguments. Please contact BEDOPS developers!\n");
        return UNSTARCH_FATAL_ERROR;
    }

#ifdef __cplusplus
    *optn = (ftr1) ? STARCH_strdup(ftr1) : ((ftr2) ? STARCH_strdup(ftr2) : nullptr);
#else
    *optn = (ftr1) ? STARCH_strdup(ftr1) : ((ftr2) ? STARCH_strdup(ftr2) : NULL);
#endif

    if (*optn) {
        if (strcmp(*optn, "help") == 0) {
            *pval = UNSTARCH_HELP_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "version") == 0) {
            *pval = UNSTARCH_VERSION_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "is-starch") == 0) {
            *pval = UNSTARCH_IS_STARCH_ARCHIVE_ERROR;
            return *pval;
        }
        else if ((strcmp(*optn, "archiveVersion") == 0) || (strcmp(*optn, "archive-version") == 0)) {
            *pval = UNSTARCH_ARCHIVE_VERSION_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "archive-timestamp") == 0) {
            *pval = UNSTARCH_ARCHIVE_CREATION_TIMESTAMP_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "sha1-signature") == 0) {
            *pval = UNSTARCH_METADATA_SHA1_SIGNATURE_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "signature") == 0) {
            *pval = UNSTARCH_SIGNATURE_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "verify-signature") == 0) {
            *pval = UNSTARCH_SIGNATURE_VERIFY_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "note") == 0) {
            *pval = UNSTARCH_ARCHIVE_NOTE_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "archive-type") == 0) {
            *pval = UNSTARCH_ARCHIVE_COMPRESSION_TYPE_ERROR;
            return *pval;
        }
        else if ((strcmp(*optn, "listchr") == 0) || (strcmp(*optn, "list-chr") == 0) || (strcmp(*optn, "list-chromosomes") == 0)) {
            *pval = UNSTARCH_LIST_CHROMOSOMES_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "elements") == 0) {
            *pval = (strcmp(*chr, "--elements") == 0) ? UNSTARCH_ELEMENT_COUNT_ALL_ERROR : UNSTARCH_ELEMENT_COUNT_CHR_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "elements-max-string-length") == 0) {
            *pval = (strcmp(*chr, "--elements-max-string-length") == 0) ? UNSTARCH_ELEMENT_MAX_STRING_LENGTH_ALL_ERROR : UNSTARCH_ELEMENT_MAX_STRING_LENGTH_CHR_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "bases") == 0) {
            *pval = (strcmp(*chr, "--bases") == 0) ? UNSTARCH_BASES_COUNT_ALL_ERROR : UNSTARCH_BASES_COUNT_CHR_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "bases-uniq") == 0) {
            *pval = (strcmp(*chr, "--bases-uniq") == 0) ? UNSTARCH_BASES_UNIQUE_COUNT_ALL_ERROR : UNSTARCH_BASES_UNIQUE_COUNT_CHR_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "has-duplicate-as-string") == 0) {
            *pval = (strcmp(*chr, "--has-duplicate-as-string") == 0) ? UNSTARCH_ELEMENT_DUPLICATE_ALL_STR_ERROR : UNSTARCH_ELEMENT_DUPLICATE_CHR_STR_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "has-duplicate") == 0) {
            *pval = (strcmp(*chr, "--has-duplicate") == 0) ? UNSTARCH_ELEMENT_DUPLICATE_ALL_INT_ERROR : UNSTARCH_ELEMENT_DUPLICATE_CHR_INT_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "has-nested-as-string") == 0) {
            *pval = (strcmp(*chr, "--has-nested-as-string") == 0) ? UNSTARCH_ELEMENT_NESTED_ALL_STR_ERROR : UNSTARCH_ELEMENT_NESTED_CHR_STR_ERROR;
            return *pval;
        }
        else if (strcmp(*optn, "has-nested") == 0) {
            *pval = (strcmp(*chr, "--has-nested") == 0) ? UNSTARCH_ELEMENT_NESTED_ALL_INT_ERROR : UNSTARCH_ELEMENT_NESTED_CHR_INT_ERROR;
            return *pval;
        }
        else if ((strcmp(*optn, "list") != 0) &&
                 (strcmp(*optn, "listJSON") != 0) &&
                 (strcmp(*optn, "list-json") != 0) &&
                 (strcmp(*optn, "list-json-no-trailing-newline") != 0) ) {
            fprintf(stderr, "ERROR: Wrong option specified\n");
            return UNSTARCH_FATAL_ERROR;
        }
    }

    return EXIT_SUCCESS;
}

void
UNSTARCH_printUsage(int errorType)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printUsage() ---\n");
#endif
#ifdef __cplusplus
    char *avStr = nullptr;
    avStr = static_cast<char *>( malloc(STARCH_ARCHIVE_VERSION_STRING_LENGTH) );
    if (avStr != nullptr) {
#else
    char *avStr = NULL;
    avStr = malloc(STARCH_ARCHIVE_VERSION_STRING_LENGTH);
    if (avStr != NULL) {
#endif
        int result = sprintf(avStr, "%d.%d.%d", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        if (result != -1) {
            switch (errorType) {
                case EXIT_FAILURE:
                case UNSTARCH_HELP_ERROR:
                default:
                    fprintf(stderr,
                "%s\n  citation: %s\n  binary version: %s (extracts archive version: %s or older)\n  authors: %s\n%s\n",
                name,
                BEDOPS::citation(),
                BEDOPS::version(),
                avStr,
                authors,
                usage);
                    break;
            }
        }
        free(avStr);
    }
}

void
UNSTARCH_printRevision()
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printRevision() ---\n");
#endif
#ifdef __cplusplus
    char *avStr = nullptr;
    avStr = static_cast<char *>( malloc(STARCH_ARCHIVE_VERSION_STRING_LENGTH) );
    if (avStr != nullptr) {
#else
    char *avStr = NULL;
    avStr = malloc(STARCH_ARCHIVE_VERSION_STRING_LENGTH);
    if (avStr != NULL) {
#endif
        int result = sprintf(avStr, "%d.%d.%d", STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        if (result != -1)
            fprintf(stdout,
            "%s\n  binary version: %s (extracts archive version: %s or older)\n",
            name,
            BEDOPS::version(),
            avStr);
        free(avStr);
    }
}

void
UNSTARCH_printArchiveVersion(const ArchiveVersion *av)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printArchiveVersion() ---\n");
#endif
    if (av)
        fprintf(stdout,
        "%s\n  archive version: %d.%d.%d\n",
        name,
        av->major,
        av->minor,
        av->revision);
}

void
UNSTARCH_printArchiveTimestamp(const char *at)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printArchiveTimestamp() ---\n");
#endif
    if (at)
        fprintf(stdout,
        "%s\n",
        at);
}

void
UNSTARCH_printNote(const char *note)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printNote() ---\n");
#endif
    if (note)
        fprintf(stdout,
        "%s\n",
        note);
}

void
UNSTARCH_printCompressionType(const CompressionType t)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printCompressionType() ---\n");
#endif
    switch (t) {
        case kBzip2:
            fprintf(stdout, "%s\n  archive compression type: bzip2\n", name);
            break;
        case kGzip:
            fprintf(stdout, "%s\n  archive compression type: gzip\n", name);
            break;
        case kUndefined:
            fprintf(stdout, "ERROR: compression type is undefined\n");
            break;
    }
}

void
UNSTARCH_printMetadataSha1Signature(unsigned char *sha1Buffer)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- UNSTARCH_printMetadataSha1Signature() ---\n");
#endif
#ifdef __cplusplus
    char *jsonBase64String = nullptr;
#else
    char *jsonBase64String = NULL;
#endif
    size_t sha1BufferLength = STARCH2_MD_FOOTER_SHA1_LENGTH;

#ifdef DEBUG
    fwrite(sha1Buffer, sizeof(unsigned char), STARCH2_MD_FOOTER_SHA1_LENGTH, stderr);
    fprintf(stderr, "\n\tsha1BufferLength:    %zu\n", sha1BufferLength);
#endif

#ifdef __cplusplus
    STARCH_encodeBase64(&jsonBase64String,
            static_cast<const size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ),
            const_cast<const unsigned char *>( sha1Buffer ),
            static_cast<const size_t>( sha1BufferLength ));
#else
    STARCH_encodeBase64(&jsonBase64String,
            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH,
            (const unsigned char *) sha1Buffer,
            (const size_t) sha1BufferLength);
#endif

    if (!jsonBase64String) {
        fprintf(stderr, "ERROR: Could not allocate space for Base64-encoded metadata string representation\n");
        exit(-1);
    }
    fprintf(stdout, "%s\n", jsonBase64String);
    free(jsonBase64String);
#ifdef __cplusplus
    jsonBase64String = nullptr;
#else
    jsonBase64String = NULL;
#endif
}

#ifdef __cplusplus
} // namespace starch
#endif
