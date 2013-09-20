//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchMetadataHelpers.c
//=========

#ifdef __cplusplus
#include <cinttypes>
#include <cstdint>
#else
#include <inttypes.h>
#include <stdint.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <limits.h> /* CHAR_BIT */
#include <time.h>

#include "data/starch/starchBase64Coding.h"
#include "data/starch/starchSha1Digest.h"
#include "data/starch/starchMetadataHelpers.h"
#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchHelpers.h"

Metadata * 
STARCH_createMetadata(char const *chr, char const *fn, uint64_t size, Bed::LineCountType lineCount, Bed::BaseCountType totalNonUniqueBases, Bed::BaseCountType totalUniqueBases)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_createMetadata() ---\n");
#endif
    Metadata *newMetadata = NULL;
    size_t fnLength = 0;
    size_t chrLength = 0;

    if (chr)
        chrLength = strlen(chr);
    if (fn)
        fnLength = strlen(fn);

    newMetadata = (Metadata *) malloc(sizeof(Metadata));

    if ((newMetadata != NULL) && (chr != NULL) && (fn != NULL)) {
        newMetadata->chromosome = NULL;
        newMetadata->chromosome = (char *) malloc(chrLength + 1);
        if (!newMetadata->chromosome) {
            fprintf(stderr, "ERROR: Cannot instantiate new chromosome for metadata record\n");
            exit(EXIT_FAILURE);
        }
        strncpy(newMetadata->chromosome, chr, chrLength + 1);
        newMetadata->filename = NULL;
        newMetadata->filename = (char *) malloc(fnLength + 1);
        if (!newMetadata->filename) {
            fprintf(stderr, "ERROR: Cannot instantiate new filename for metadata record\n");
            exit(EXIT_FAILURE);
        }            
        strncpy(newMetadata->filename, fn, fnLength + 1);
        newMetadata->size = size;
        newMetadata->lineCount = lineCount;
        newMetadata->totalNonUniqueBases = totalNonUniqueBases;
        newMetadata->totalUniqueBases = totalUniqueBases;
        newMetadata->next = NULL;
    }
    else {
        fprintf(stderr, "ERROR: Could not allocate memory for metadata!\n");
        exit (EXIT_FAILURE);
    }

    return newMetadata;
}

Metadata * 
STARCH_addMetadata(Metadata *md, char *chr, char *fn, uint64_t size, Bed::LineCountType lineCount, Bed::BaseCountType totalNonUniqueBases, Bed::BaseCountType totalUniqueBases) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_addMetadata() ---\n");
#endif
    Metadata *newMd = STARCH_createMetadata(chr, 
                                            fn, 
                                            size,
                                            lineCount,
                                            totalNonUniqueBases, 
                                            totalUniqueBases);

    if ((newMd != NULL) && (md->next == NULL))
        md->next = newMd;
    else {
        fprintf(stderr, "ERROR: Could not allocate memory for metadata!\n");
        exit (EXIT_FAILURE);
    }

    return newMd;
}

Metadata * 
STARCH_copyMetadata(const Metadata *md) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_copyMetadata() ---\n");
#endif
    Metadata *copy = NULL;
    Metadata *firstRec = NULL;
    const Metadata *iter = NULL;

    /* copy first record */
    copy = STARCH_createMetadata(md->chromosome,
                                 md->filename,
                                 md->size,
                                 md->lineCount,
                                 md->totalNonUniqueBases,
                                 md->totalUniqueBases);
    firstRec = copy;
    md = md->next;

    /* copy remainder */
    for (iter = md; iter != NULL; iter = iter->next) {
        copy = STARCH_addMetadata(copy,
                                  iter->chromosome,
                                  iter->filename,
                                  iter->size,
                                  iter->lineCount,
                                  iter->totalNonUniqueBases,
                                  iter->totalUniqueBases);
    }

    if (!firstRec) {
        fprintf(stderr, "ERROR: Could not allocate memory for copy of metadata!\n");
        exit (EXIT_FAILURE);
    }

    return firstRec;
}

int 
STARCH_updateMetadataForChromosome(Metadata **md, char *chr, char *fn, uint64_t size, Bed::LineCountType lineCount, Bed::BaseCountType totalNonUniqueBases, Bed::BaseCountType totalUniqueBases) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_updateMetadataForChromosome() ---\n");
#endif
    Metadata *iter;

    /* 
       It is possible for a NULL Metadata record to occur with empty set input 
       from file or stream, so we return early.
    */

    if (! *md)
        return STARCH_NONFATAL_ERROR; 

    for (iter = *md; iter != NULL; iter = iter->next) {
        if (strcmp((const char *)iter->chromosome, chr) == 0) {
            free(iter->filename); 
            iter->filename = NULL;
            iter->filename = (char *) malloc(strlen(fn) + 1);
            strncpy(iter->filename, fn, strlen(fn) + 1);
            if (!iter->filename) {
                fprintf(stderr, "ERROR: Ran out of memory for updating metadata with filename\n");
                return STARCH_EXIT_FAILURE;
            }
            iter->size = size;
            iter->lineCount = lineCount;
            iter->totalNonUniqueBases =  totalNonUniqueBases;
            iter->totalUniqueBases = totalUniqueBases;

            break;
        }
    }

    return STARCH_EXIT_SUCCESS;
}

int 
STARCH_listMetadata(const Metadata *md, const char *chr) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_listMetadata() ---\n");
#endif
    const Metadata *iter;
    Boolean chrFound = kStarchFalse;

    if (!md) {
        fprintf(stderr, "ERROR: Could not list metadata (metadata structure is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    if (strcmp("all", chr) == 0) 
        chrFound = kStarchTrue;
    else {
        for (iter = md; iter != NULL; iter = iter->next) {
            if (strcmp((const char *)iter->chromosome, chr) == 0) {
                chrFound = kStarchTrue;
                break;
            }
        }
    }

    if (chrFound == kStarchTrue) {
        fprintf(stdout, "%-25s| %-65s\t| %-15s\t| %-20s\t| %-20s\t| %-20s\n", "chr", "filename", "compressedSize", "uncompressedLineCount", "totalNonUniqueBases", "totalUniqueBases");
        for (iter = md; iter != NULL; iter = iter->next) {
            if ( (strcmp((const char *)iter->chromosome, chr) == 0) || (strcmp("all", chr) == 0) )
                fprintf(stdout, "%-25s| %-65s\t| %-15" PRIu64 "\t| %-20" PRIu64 "\t| %-20" PRIu64 "\t| %-20" PRIu64 "\n", iter->chromosome, iter->filename, iter->size, iter->lineCount, iter->totalNonUniqueBases, iter->totalUniqueBases);
        }
    }

    return STARCH_EXIT_SUCCESS;
}

int
STARCH_listAllChromosomes(const Metadata *md) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_listAllChromosomes() ---\n");
#endif
    const Metadata *iter;

    if (!md) {
        fprintf(stderr, "ERROR: Could not list chromosomes (metadata structure is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    for (iter = md; iter != NULL; iter = iter->next)
        if (strcmp(kStarchNullChromosome, iter->chromosome) != 0) 
            fprintf(stdout, "%s\n", iter->chromosome);

    return STARCH_EXIT_SUCCESS;
}

int
STARCH_listChromosome(const Metadata *md, const char *chr) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_listChromosome() ---\n");
#endif
    const Metadata *iter;

    if (!md) {
        fprintf(stderr, "ERROR: Could not list chromosome (metadata structure is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    for (iter = md; iter != NULL; iter = iter->next) {
        if (strcmp(iter->chromosome, chr) == 0) {
            if (strcmp(kStarchNullChromosome, iter->chromosome) != 0) 
                fprintf(stdout, "%s\n", iter->chromosome);
            break;
        }
    }

    return STARCH_EXIT_SUCCESS;
}

void 
STARCH_freeMetadata(Metadata **md) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_freeMetadata() ---\n");
#endif
    Metadata *iter;
    Metadata *prev = NULL;

    if (! *md)
        return;

    for (iter = *md; iter != NULL; iter = iter->next) {
        if (iter->chromosome != NULL)
            free(iter->chromosome);
        if (iter->filename != NULL)
            free(iter->filename);
        if (prev != NULL)
            free(prev);
        
        prev = iter;
    }

    if (prev != NULL) {
        free(prev);
        prev = NULL;
    }
}

int 
STARCH_deleteCompressedFiles(const Metadata *md) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_deleteCompressedFiles() ---\n");
#endif
    const Metadata *iter;

    if (!md) {
        fprintf(stderr, "ERROR: Could not delete per-chromosome compressed files (metadata structure is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    for (iter = md; iter != NULL; iter = iter->next) {
        if (remove(iter->filename) != 0) {
            fprintf(stderr, "ERROR: Could not delete per-chromosome compressed file %s. Is your BED input not sorted lexographically? Does it contain custom UCSC headers (see --header)?\n", iter->filename);
            return STARCH_EXIT_FAILURE;
        }
    }

    return STARCH_EXIT_SUCCESS;
}

char * 
STARCH_generateJSONMetadata(const Metadata *md, const CompressionType type, const ArchiveVersion *av, const char *cTime, const char *note, const Boolean headerFlag)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_generateJSONMetadata() ---\n");
#endif
    const Metadata *iter = NULL;
    json_t *metadataJSON = NULL;
    json_t *compressionFormat = json_integer((json_int_t)type);
    json_t *streams = json_array();
    json_t *stream = NULL; 
    json_t *streamChromosome = NULL;
    json_t *streamFilename = NULL;
    json_t *streamSize = NULL;
    json_t *streamLineCount = NULL;
    json_t *streamTotalNonUniqueBases = NULL;
    json_t *streamTotalUniqueBases = NULL;
    json_t *streamCustomHeaderFlag = NULL;
    json_t *streamArchive = NULL;
    json_t *streamArchiveType = NULL;
    json_t *streamArchiveNote = NULL;
    json_t *streamArchiveCreationTimestamp = NULL;
    json_t *streamArchiveVersion = NULL;
    json_t *streamArchiveVersionMajor = NULL;
    json_t *streamArchiveVersionMinor = NULL;
    json_t *streamArchiveVersionRevision = NULL;
    char *recordFilenameCopy = NULL;
    char *recordChromosome = NULL;
    char *recordToken = NULL;
    char *recordSize = NULL;
    char *creationTimestamp = NULL;
    char *jsonString = NULL;
    size_t creationTimestampLength = STARCH_CREATION_TIMESTAMP_LENGTH;
    uint64_t filenameSize = 0LLU;
    Bed::LineCountType filenameLineCount = UINT64_C(0);
    Bed::BaseCountType totalNonUniqueBases = UINT64_C(0);
    Bed::BaseCountType totalUniqueBases = UINT64_C(0);
    time_t creationTime;
    struct tm *creationTimeInformation = NULL;

    if (!md)
        return NULL;
    
    /* cf. http://www.digip.org/jansson/doc/2.3/apiref.html */

    metadataJSON = json_object();
    if (!metadataJSON) {
        fprintf(stderr, "ERROR: Could not instantiate metadata root object\n");
        return NULL;    
    }

    streamArchive = json_object();
    if (!streamArchive) {
        fprintf(stderr, "ERROR: Could not instantiate stream archive object\n");
        return NULL;
    }
    
    streamArchiveType = json_string(STARCH_METADATA_STREAM_ARCHIVE_TYPE_VALUE);        
    if (!streamArchiveType) {
        fprintf(stderr, "ERROR: Could not instantiate stream archive type object\n");
        return NULL;
    }
    json_object_set_new(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_TYPE_KEY, streamArchiveType);

    streamCustomHeaderFlag = json_boolean(headerFlag == kStarchTrue); /* returns json_true() if true, else json_false() */
    if (!streamCustomHeaderFlag) {
        fprintf(stderr, "ERROR: Could not instantiate stream header flag object\n");
        return NULL;
    }
    json_object_set_new(streamArchive, STARCH_METADATA_STREAM_HEADER_BED_TYPE_KEY, streamCustomHeaderFlag);
    
    streamArchiveVersion = json_object();
    if (!av) {
        /* starch - create defaults */
        streamArchiveVersionMajor = json_integer(STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_VALUE);
        streamArchiveVersionMinor = json_integer(STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_VALUE);
        streamArchiveVersionRevision = json_integer(STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_VALUE);
    }
    else {
        /* unstarch - read from archive */
        streamArchiveVersionMajor = json_integer(av->major);
        streamArchiveVersionMinor = json_integer(av->minor);
        streamArchiveVersionRevision = json_integer(av->revision);
    }
    if ((!streamArchiveVersionMajor) || (!streamArchiveVersionMinor) || (!streamArchiveVersionRevision)) {  
        fprintf(stderr, "ERROR: Could not instantiate stream archive version objects\n");
        return NULL;
    }

    /* 1.5+ archive */
    if ((json_integer_value(streamArchiveVersionMajor) > 1) || ((json_integer_value(streamArchiveVersionMajor) == 1) && (json_integer_value(streamArchiveVersionMinor) >= 5))) 
    {
        /* creation timestamp */
        if (!cTime) {
            /* starch - create timestamp */    
            time(&creationTime);
            creationTimeInformation = localtime(&creationTime);
            creationTimestamp = (char *) malloc(creationTimestampLength);
            if (!creationTimestamp) {
                fprintf(stderr, "ERROR: Could not instantiate stream archive creation timestamp string\n");
                return NULL;
            }
            strftime(creationTimestamp, creationTimestampLength, "%Y-%m-%dT%H:%M:%S%z", creationTimeInformation);
            streamArchiveCreationTimestamp = json_string(creationTimestamp);
            free(creationTimestamp);
            creationTimestamp = NULL;
            if (!streamArchiveCreationTimestamp) {
                fprintf(stderr, "ERROR: Could not instantiate stream archive creation timestamp JSON object\n");
                return NULL;
            }
        }
        else {
            /* unstarch - read from archive */
            streamArchiveCreationTimestamp = json_string(cTime);
        }
        json_object_set_new(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_CREATION_TIMESTAMP_KEY, streamArchiveCreationTimestamp);

        /* note */
        if (note) {
            streamArchiveNote = json_string(note);
            if (!streamArchiveNote) {
                fprintf(stderr, "ERROR: Could not instantiate stream archive note object\n");
                return NULL;
            }
            json_object_set_new(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_NOTE_KEY, streamArchiveNote);
        }
    }

    json_object_set_new(streamArchiveVersion, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_KEY, streamArchiveVersionMajor);
    json_object_set_new(streamArchiveVersion, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_KEY, streamArchiveVersionMinor);
    json_object_set_new(streamArchiveVersion, STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_KEY, streamArchiveVersionRevision);
    json_object_set_new(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY, streamArchiveVersion);
    json_object_set_new(streamArchive, STARCH_METADATA_STREAM_COMPRESSION_FORMAT_KEY, compressionFormat);
    json_object_set_new(metadataJSON, STARCH_METADATA_STREAM_ARCHIVE_KEY, streamArchive);

    for (iter = md; iter != NULL; iter = iter->next) {
        stream = json_object();

        recordFilenameCopy = STARCH_strndup(iter->filename, strlen(iter->filename) + 1);
        recordToken = strtok(recordFilenameCopy, ".");
        recordChromosome = STARCH_strndup(recordToken, strlen(recordToken) + 1);
        streamChromosome = json_string(recordChromosome);
        json_object_set_new(stream, STARCH_METADATA_STREAM_CHROMOSOME_KEY, streamChromosome);
        free(recordFilenameCopy);
        free(recordChromosome);

        streamFilename = json_string(iter->filename);
        json_object_set_new(stream, STARCH_METADATA_STREAM_FILENAME_KEY, streamFilename);

        filenameSize = iter->size;
        recordSize = (char *) malloc((CHAR_BIT * sizeof (filenameSize) + 2) / 3 + 1);
        if (!recordSize)
            filenameSize = 0LLU;
        sprintf(recordSize, "%" PRIu64, filenameSize);        
        streamSize = json_string(recordSize); /* we use json_string() to avoid int overflow, as Jansson (JSON library) has no uint64_t type */
        json_object_set_new(stream, STARCH_METADATA_STREAM_SIZE_KEY, streamSize);
        free(recordSize);

        /* 1.3+ archive */
        if ((json_integer_value(streamArchiveVersionMajor) > 1) || ((json_integer_value(streamArchiveVersionMajor) == 1) && (json_integer_value(streamArchiveVersionMinor) >= 3))) {
            filenameLineCount = iter->lineCount;
            streamLineCount = json_integer((json_int_t)filenameLineCount);
            json_object_set_new(stream, STARCH_METADATA_STREAM_LINECOUNT_KEY, streamLineCount);
        }
        
        /* 1.4+ archive */
        if ((json_integer_value(streamArchiveVersionMajor) > 1) || ((json_integer_value(streamArchiveVersionMajor) == 1) && (json_integer_value(streamArchiveVersionMinor) >= 4))) {
            totalNonUniqueBases = iter->totalNonUniqueBases;
            streamTotalNonUniqueBases = json_integer((json_int_t)totalNonUniqueBases);
            json_object_set_new(stream, STARCH_METADATA_STREAM_TOTALNONUNIQUEBASES_KEY, streamTotalNonUniqueBases);
            totalUniqueBases = iter->totalUniqueBases;
            streamTotalUniqueBases = json_integer((json_int_t)totalUniqueBases);
            json_object_set_new(stream, STARCH_METADATA_STREAM_TOTALUNIQUEBASES_KEY, streamTotalUniqueBases);
        }

        json_array_append_new(streams, stream);
    }

    json_object_set_new(metadataJSON, STARCH_METADATA_STREAM_LIST_KEY, streams);

    jsonString = (char *) json_dumps(metadataJSON, JSON_INDENT(2)|JSON_PRESERVE_ORDER);

    /* cleanup */
    json_decref(metadataJSON);

    return jsonString;
}

int 
STARCH_listJSONMetadata(FILE *out, FILE *err, const Metadata *md, const CompressionType type, const ArchiveVersion *av, const char *cTime, const char *note, const Boolean headerFlag, const Boolean showNewlineFlag) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_listJSONMetadata() ---\n");
#endif
    char *jsonString = NULL;

    if (!out)
        out = stdout;
    if (!err)
        err = stderr;

    jsonString = STARCH_generateJSONMetadata(md, type, av, cTime, note, headerFlag);

    if (!jsonString) {
        fprintf(err, "ERROR: Could not list metadata (metadata structure is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    if (showNewlineFlag == kStarchTrue)
        fprintf(out, "%s\n", jsonString);
    else
        fprintf(out, "%s", jsonString);
    free(jsonString);

    return STARCH_EXIT_SUCCESS;
}

int 
STARCH_writeJSONMetadata(const Metadata *md, char **buf, CompressionType *type, const Boolean headerFlag, const char *note) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_writeJSONMetadata() ---\n");
#endif
    char *jsonString = NULL;
    size_t jsonLength;

    /* archive version and creation timestamp are NULL, in order to write default values */
    jsonString = STARCH_generateJSONMetadata(md, *type, NULL, NULL, note, headerFlag); 

    if (!jsonString) {
        fprintf(stderr, "ERROR: Could not write JSON-formatted metadata to buffer (JSON string is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    jsonLength = strlen(jsonString) + 1;

    /* dynamic buffer */
    if (! *buf)
        *buf = (char *) malloc(jsonLength + 2);

    if (*buf != NULL) {
        memcpy(*buf, jsonString, jsonLength);
        free(jsonString);
        jsonString = NULL;
    }
    else {
        fprintf(stderr, "ERROR: Could not allocate space for JSON-formatted metadata buffer\n");
        return STARCH_EXIT_FAILURE;
    }       

    if ((STARCH_MAJOR_VERSION == 1) && (STARCH_MINOR_VERSION == 0) && (STARCH_REVISION_VERSION == 0)) {
        /* pad out the metadata in legacy version */
        memset(*buf + jsonLength, '\n', STARCH_LEGACY_METADATA_SIZE - jsonLength - 1);
        *(*buf + STARCH_LEGACY_METADATA_SIZE - 1) = '\0';
    }
    else {
        *(*buf + jsonLength) = '\n';
        *(*buf + jsonLength + 1) = '\0';
    }

    return STARCH_EXIT_SUCCESS;
}

int 
STARCH_readJSONMetadata(json_t **metadataJSON, FILE **fp, const char *fn, Metadata **rec, CompressionType *type, ArchiveVersion **version, char **cTime, char **note, uint64_t *mdOffset, Boolean *headerFlag, const Boolean suppressErrorMsgs, const Boolean preserveJSONRef)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_readJSONMetadata() ---\n");
#endif
    int charCnt = 0;
    char *dynamicBuffer = NULL;
    char *dynamicBufferCopy = NULL;
    char legacyBuffer[STARCH_LEGACY_METADATA_SIZE];
    char testMagicBuffer[STARCH_TEST_BYTE_COUNT] = {0};
    char footerBuffer[STARCH2_MD_FOOTER_LENGTH + 1] = {0};
    char currC = '\0';
    char prevC = '\0';
    uint64_t testMagicOffset = 0ULL;
    uint64_t mdOffsetIndex = 0ULL;
    Metadata *firstRec = NULL;
    json_t *streamArchive;
    json_t *streamArchiveVersion = NULL;
    json_t *streamArchiveCreationTimestamp = NULL;
    json_t *streamArchiveNote = NULL;
    json_t *streams = NULL;
    json_t *stream = NULL;
    json_t *streamChromosome = NULL;
    json_t *streamFilename = NULL;
    json_t *streamSize = NULL;
    json_t *streamLineCount = NULL;
    json_t *streamTotalNonUniqueBases = NULL;
    json_t *streamTotalUniqueBases = NULL;
    json_t *streamsCompressionType = NULL;
    size_t streamIdx;
    char *streamChr = NULL;
    char *streamFn = NULL;
    char *streamCTime = NULL;
    char *streamNote = NULL;
    uint64_t streamSizeValue = UINT64_C(0);
    char *testMagicPrecursor = NULL;
    json_error_t jsonParseError;
    const char *jsonObjKey = NULL;
    json_t *jsonObjValue = NULL;
    const char *jsonObjAvKey = NULL;
    json_t *jsonObjAvValue = NULL;
    Bed::LineCountType streamLineCountValue = STARCH_DEFAULT_LINE_COUNT;
    Bed::BaseCountType streamTotalNonUniqueBasesValue = STARCH_DEFAULT_NON_UNIQUE_BASE_COUNT;
    Bed::BaseCountType streamTotalUniqueBasesValue = STARCH_DEFAULT_UNIQUE_BASE_COUNT;
    json_t *mdJSON = NULL;
    size_t mdHashIndex;
    char offsetBuffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + 1] = {0};
    unsigned char mdHashBuffer[STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH] = {0};
    char *testMdHashBuffer = NULL;
    unsigned char testMdHashSha1Buffer[STARCH2_MD_FOOTER_SHA1_LENGTH];
    off_t fSize;
    off_t currentMdOffset;
    size_t mdLength;
    Boolean starchRevision2Flag = kStarchFalse;
    unsigned char starchRevision2HeaderTestBytes[sizeof(starchRevision2HeaderBytes)] = {0};
    size_t nReadBytes;

    if (! *version) {
        *version = (ArchiveVersion *) malloc (sizeof(ArchiveVersion));
        if (! *version) {
            if (suppressErrorMsgs == kStarchFalse) 
                fprintf(stderr, "ERROR: Could not allocate memory for archive version data\n");
            return STARCH_EXIT_FAILURE;
        }
    }

    if (! *fp)
        *fp = STARCH_fopen(fn, "rbR");

    if (! *fp) {
        if (suppressErrorMsgs == kStarchFalse) 
            fprintf(stderr, "ERROR: Archive file %s could not be opened\n", fn);
        return STARCH_EXIT_FAILURE;
    }

    /* read first four bytes to test if starch v2 file */
#ifdef DEBUG
    fprintf(stderr, "\treading first four bytes to test if starch v2 file...\n");
#endif
    nReadBytes = fread(starchRevision2HeaderTestBytes, sizeof(unsigned char), sizeof(starchRevision2HeaderBytes), *fp);
    if (nReadBytes != (sizeof(unsigned char) * sizeof(starchRevision2HeaderBytes))) {
        fprintf(stderr, "ERROR: Total amount of bytes read not equal to Starch magic byte header length!\n");
        return STARCH_EXIT_FAILURE;
    }
    if (memcmp(starchRevision2HeaderTestBytes, starchRevision2HeaderBytes, sizeof(starchRevision2HeaderBytes)) == 0)
        starchRevision2Flag = kStarchTrue;
#ifdef DEBUG
    if (starchRevision2Flag == kStarchTrue)
        fprintf(stderr, "\t\tstarchRevision2Flag == kStarchTrue\n");
    else
        fprintf(stderr, "\t\tstarchRevision2Flag == kStarchFalse\n");
#endif

    STARCH_fseeko(*fp, 0, SEEK_SET);

    if ((STARCH_MAJOR_VERSION == 1) && (STARCH_MINOR_VERSION == 0) && (STARCH_REVISION_VERSION == 0)) {
        do {
            currC = (char) fgetc(*fp);
            if ((prevC == currC) && (currC == '\n'))
                break;
             else {
                legacyBuffer[charCnt++] = currC;
                prevC = currC;
            }
        } while (currC != EOF);
        legacyBuffer[charCnt - 1] = '\0';

        /* turn buffer into JSON entity */
        *metadataJSON = json_loads(legacyBuffer, JSON_DISABLE_EOF_CHECK, &jsonParseError);
        if (!*metadataJSON) {
            if (suppressErrorMsgs == kStarchFalse) 
                fprintf(stderr, "ERROR: Could not turn buffer into (legacy) JSON entity\n");
            return STARCH_EXIT_FAILURE; 
        }

        /* set metadata offset */
        *mdOffset = STARCH_LEGACY_METADATA_SIZE;
    }

    else if ((starchRevision2Flag == kStarchFalse) || ((STARCH_MAJOR_VERSION == 1) && (STARCH_MINOR_VERSION > 0) && (STARCH_REVISION_VERSION == 0))) {
        /* Set aside pre-magic-byte terminator check for extra insurance */
        testMagicPrecursor = (char *) malloc ((size_t) mdTerminatorBytesLength);
        if (!testMagicPrecursor) {
            if (suppressErrorMsgs == kStarchFalse) 
                fprintf(stderr, "ERROR: Could not allocate space for test magic byte precursor\n");
            return STARCH_EXIT_FAILURE;
        }

        /* look through the archive for bzip2 and gzip magic numbers */
        do {
            if ((memcmp(testMagicBuffer, bzip2MagicBytes, sizeof(bzip2MagicBytes) - 1) == 0) || (memcmp(testMagicBuffer, gzipMagicBytes, sizeof(gzipMagicBytes) - 1) == 0)) {

                /* if the byte prior to the magic bytes is a metadata terminator byte, there is */
                /* greater certainty that these bytes split the metadata and compressed streams */
                /* this helps avoid issues where the metadata, for some reason, contains the    */
                /* same magic bytes, e.g. an unfortunate choice of chromosome name, or we add   */
                /* some metadata key down the road, and the user adds a value that contains our */
                /* special, magic bytes                                                         */

                STARCH_fseeko(*fp, (off_t)(testMagicOffset - mdTerminatorBytesLength), SEEK_SET);
                nReadBytes = fread(testMagicPrecursor, mdTerminatorBytesLength, mdTerminatorBytesLength, *fp);
                if (nReadBytes != (mdTerminatorBytesLength * mdTerminatorBytesLength)) {
                    fprintf(stderr, "ERROR: Total amount of bytes read not equal to terminator bytes!\n");
                    return STARCH_EXIT_FAILURE;
                }

                /* if we have a match, then we break at the 'real' offset; otherwise, the file  */
                /* pointer is back to where it started, so we can keep on walking               */

                if ( (memcmp(testMagicPrecursor, dynamicMdTerminatorBytes, sizeof(dynamicMdTerminatorBytes)) == 0) || 
                     (memcmp(testMagicPrecursor, legacyMdTerminatorBytes, sizeof(legacyMdTerminatorBytes)) == 0) ||
                     (memcmp(testMagicPrecursor, otherLegacyMdTerminatorBytes, sizeof(otherLegacyMdTerminatorBytes)) == 0) )
                    break;
            }            
            testMagicOffset += testElemSize * testElemCount;
            STARCH_fseeko(*fp, (off_t) (testMagicOffset - STARCH_TEST_BYTE_POSITION_RESET), SEEK_SET);
            testMagicOffset -= STARCH_TEST_BYTE_POSITION_RESET;
        } while (fread(testMagicBuffer, testElemSize, testElemCount, *fp));

        /* release pre-magic-byte terminator check */
        free(testMagicPrecursor);
        testMagicPrecursor = NULL;

        /* set metadata offset */
        *mdOffset = testMagicOffset;

        /* rewind file pointer to start of file */
        STARCH_fseeko(*fp, (off_t)0, SEEK_SET);

        /* read metadata into malloc'ed buffer */
        dynamicBuffer = (char *) malloc((size_t) testMagicOffset + 1);
        if (!dynamicBuffer) {
            if (suppressErrorMsgs == kStarchFalse)
                fprintf(stderr, "ERROR: Could not allocate space for dynamic buffer.\n");
            return STARCH_EXIT_FAILURE;
        }
        do {
            *(dynamicBuffer + mdOffsetIndex) = (char) fgetc(*fp);
            mdOffsetIndex++;
        } while (mdOffsetIndex < testMagicOffset);
        *(dynamicBuffer + mdOffsetIndex) = '\0';

        /* turn buffer into JSON entity, or retrieve legacy data */
        *metadataJSON = json_loads((const char *)dynamicBuffer, JSON_DISABLE_EOF_CHECK, &jsonParseError);
        if (!*metadataJSON) {
            if (STARCH_readLegacyMetadata((const char *)dynamicBuffer, &*rec, &*type, &*version, &*mdOffset, &*headerFlag, suppressErrorMsgs) != STARCH_EXIT_SUCCESS) {
                if (suppressErrorMsgs == kStarchFalse)
                     fprintf(stderr, "ERROR: Could not turn dynamic buffer into JSON entity (%s)\n", jsonParseError.text);
                return STARCH_EXIT_FAILURE;
            }
            return STARCH_EXIT_SUCCESS;
        }

        /* release buffer */
        if (dynamicBuffer)
            free(dynamicBuffer);
    }
    else if ((STARCH_MAJOR_VERSION == 2) && (STARCH_MINOR_VERSION >= 0) && (STARCH_REVISION_VERSION >= 0)) {
#ifdef DEBUG
        fprintf(stderr, "\treading footer bytes...\n");
#endif
        /* read STARCH_FOOTER_BYTE_LENGTH bytes from end of file to get md offset */
        /* seek to md offset, read in x - STARCH_FOOTER_BYTE_LENGTH bytes and import JSON */
        STARCH_fseeko(*fp, (off_t) -STARCH2_MD_FOOTER_LENGTH + 1, SEEK_END);
        /* the STARCH_fseeko call takes away a byte, if it works correctly, which we account for here */
        nReadBytes = fread(footerBuffer, 1, STARCH2_MD_FOOTER_LENGTH - 1, *fp);
        if (nReadBytes != (STARCH2_MD_FOOTER_LENGTH - 1)) { 
            fprintf(stderr, "ERROR: Total amount of bytes read (%" PRIu64 ") not equal to Starch v2 footer length!\n", (uint64_t)  nReadBytes);
            return STARCH_EXIT_FAILURE;
        }
#ifdef DEBUG
        fprintf(stderr, "\tfooterBuffer: %s\n", footerBuffer);
#endif
        for (mdOffsetIndex = 0; mdOffsetIndex < STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH; mdOffsetIndex++) {
            offsetBuffer[mdOffsetIndex] = footerBuffer[mdOffsetIndex];
        }
        offsetBuffer[mdOffsetIndex] = '\0';
        currentMdOffset = (off_t) strtoull((const char *)offsetBuffer, NULL, STARCH_RADIX);
        *mdOffset = (uint64_t) currentMdOffset;
#ifdef DEBUG
        fprintf(stderr, "\toffsetBuffer: %s\n", offsetBuffer);
        fprintf(stderr, "\tcurrentMdOffset: %" PRIu64 "\n", (uint64_t) currentMdOffset);
#endif
        for (mdHashIndex = 0; mdHashIndex < STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1; mdHashIndex++)
            mdHashBuffer[mdHashIndex] = (unsigned char) footerBuffer[(mdHashIndex + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH)];

        STARCH_fseeko(*fp, (off_t) 0, SEEK_END);
        fSize = ftell(*fp);
#ifdef DEBUG
        fprintf(stderr, "\tfSize: %" PRId64 "\n", fSize);
        fprintf(stderr, "\tmdHashBuffer: %s\n", mdHashBuffer);
#endif
        if (fSize <= (currentMdOffset - STARCH2_MD_FOOTER_LENGTH + 1)) {
	        fprintf(stderr, "ERROR: File size (%" PRIu64 ") is smaller than metadata offset value (%" PRIu64 "). Is the archive possibly corrupt?\n", (uint64_t) fSize, (uint64_t) currentMdOffset);
            return STARCH_EXIT_FAILURE;
        }
        mdLength = (size_t) (fSize - currentMdOffset - STARCH2_MD_FOOTER_LENGTH + 1);
#ifdef DEBUG
        fprintf(stderr, "\tmdLength: %zu\n", mdLength);
#endif
        dynamicBuffer = (char *) malloc(mdLength + 1);
        if (!dynamicBuffer) {
            fprintf(stderr, "ERROR: Could not allocate space for dynamic buffer (v2).\n");
            return STARCH_EXIT_FAILURE;
        }        
        STARCH_fseeko(*fp, (off_t) (-(mdLength + STARCH2_MD_FOOTER_LENGTH) + 1), SEEK_END);
        nReadBytes = fread(dynamicBuffer, 1, mdLength, *fp);
        if (nReadBytes != mdLength) {
            fprintf(stderr, "ERROR: Total amount of bytes read not equal to Starch v2 metadata length!\n");
            return STARCH_EXIT_FAILURE;
        }
        *(dynamicBuffer + mdLength) = '\0';
#ifdef DEBUG
        fprintf(stderr, "\tdynamicBuffer:\n%s\n", dynamicBuffer);
#endif
    
        /* validate metadata string against hash */        
        dynamicBufferCopy = strdup(dynamicBuffer);
        STARCH_SHA1_All((const unsigned char *) dynamicBufferCopy, strlen(dynamicBufferCopy), testMdHashSha1Buffer);
        free(dynamicBufferCopy);

        STARCH_encodeBase64(&testMdHashBuffer, (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH, (const unsigned char *) testMdHashSha1Buffer, (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);

#ifdef DEBUG
        fprintf(stderr, "\tmdHashBuffer:     [%s]\n", mdHashBuffer);
        fprintf(stderr, "\ttestMdHashBuffer: [%s]\n", testMdHashBuffer);
        fprintf(stderr, "\tdynamicBuffer:    [%s]\n", dynamicBuffer);
#endif

        if (memcmp(mdHashBuffer, testMdHashBuffer, STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH) != 0) {
            fprintf(stderr, "ERROR: Could not validate SHA1 hash of metadata record. Is the archive possibly corrupt?\n");
            return STARCH_EXIT_FAILURE;
        }        

        /* turn metadata string into JSON entity, or quit */
        *metadataJSON = json_loads((const char *)dynamicBuffer, JSON_DISABLE_EOF_CHECK, &jsonParseError);
        if (!*metadataJSON) {
            fprintf(stderr, "ERROR: Could not turn dynamic buffer into JSON entity\n");
            return STARCH_EXIT_FAILURE;
        }

        /* release buffer */
        if (dynamicBuffer)
            free(dynamicBuffer);
        if (testMdHashBuffer)
            free(testMdHashBuffer);
    }

    mdJSON = *metadataJSON;

    /* parse JSON entity for 'archive' key */
    streamArchive = json_object_get(mdJSON, STARCH_METADATA_STREAM_ARCHIVE_KEY);
    if (streamArchive) {
        json_object_foreach(streamArchive, jsonObjKey, jsonObjValue) 
        {
            /* compression format */
            if ( strcmp(jsonObjKey, STARCH_METADATA_STREAM_COMPRESSION_FORMAT_KEY) == 0 )
                *type = (CompressionType) json_integer_value(jsonObjValue);            

            /* header flag */
            else if ( strcmp(jsonObjKey, STARCH_METADATA_STREAM_HEADER_BED_TYPE_KEY) == 0 ) {
                switch (json_typeof(jsonObjValue)) {
                    case (JSON_TRUE): {
                        *headerFlag = kStarchTrue;
                        break;
                    }
                    case (JSON_FALSE): {
                        *headerFlag = kStarchFalse;
                        break;
                    }
                    case (JSON_OBJECT): {
                        if (suppressErrorMsgs == kStarchFalse)
                            fprintf(stderr, "ERROR: Could not extract header flag value (JSON_OBJECT)\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case (JSON_ARRAY): {
                        if (suppressErrorMsgs == kStarchFalse)
                            fprintf(stderr, "ERROR: Could not extract header flag value (JSON_ARRAY)\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case (JSON_STRING): {
                        if (suppressErrorMsgs == kStarchFalse)
                            fprintf(stderr, "ERROR: Could not extract header flag value (JSON_STRING)\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case (JSON_INTEGER): {
                        if (suppressErrorMsgs == kStarchFalse)
                            fprintf(stderr, "ERROR: Could not extract header flag value (JSON_INTEGER)\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case (JSON_REAL): {
                        if (suppressErrorMsgs == kStarchFalse)  
                            fprintf(stderr, "ERROR: Could not extract header flag value (JSON_REAL)\n");
                        return STARCH_EXIT_FAILURE;
                    }
                    case (JSON_NULL): {
                        if (suppressErrorMsgs == kStarchFalse)
                            fprintf(stderr, "ERROR: Could not extract header flag value (JSON_NULL)\n");
                        return STARCH_EXIT_FAILURE;
                    }
                }
            }

            /* creation timestamp */
            else if ( strcmp(jsonObjKey, STARCH_METADATA_STREAM_ARCHIVE_CREATION_TIMESTAMP_KEY) == 0 ) {
                streamArchiveCreationTimestamp = json_object_get(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_CREATION_TIMESTAMP_KEY);
                streamCTime = (char *) json_string_value(streamArchiveCreationTimestamp);
                *cTime = (char *) malloc(strlen(streamCTime) + 1);
                if (! *cTime) {
                    fprintf(stderr, "ERROR: Could not allocate space for creation timestamp\n");
                    return STARCH_EXIT_FAILURE;
                }
                strncpy(*cTime, streamCTime, strlen(streamCTime) + 1);
            }

            /* note */
            else if ( strcmp(jsonObjKey, STARCH_METADATA_STREAM_ARCHIVE_NOTE_KEY) == 0 ) {
                streamArchiveNote = json_object_get(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_NOTE_KEY);
                streamNote = (char *) json_string_value(streamArchiveNote);
                *note = (char *) malloc(strlen(streamNote) + 1);
                if (! *note) {
                    fprintf(stderr, "ERROR: Could not allocate space for note\n");
                    return STARCH_EXIT_FAILURE;
                }
                strncpy(*note, streamNote, strlen(streamNote) + 1);
            }
        
            /* archive version */
            else if ( strcmp(jsonObjKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY) == 0 ) {
                streamArchiveVersion = json_object_get(streamArchive, STARCH_METADATA_STREAM_ARCHIVE_VERSION_KEY);
                json_object_foreach(streamArchiveVersion, jsonObjAvKey, jsonObjAvValue) 
                {
                    if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MAJOR_KEY) == 0)
                        (*version)->major = (int) json_integer_value(jsonObjAvValue);
                    else if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_MINOR_KEY) == 0)
                        (*version)->minor = (int) json_integer_value(jsonObjAvValue);
                    else if ( strcmp(jsonObjAvKey, STARCH_METADATA_STREAM_ARCHIVE_VERSION_REVISION_KEY) == 0)
                        (*version)->revision = (int) json_integer_value(jsonObjAvValue);
                }
            }
        }
    }
    else {
        /* 
            Metadata is missing 'archive' object key, which means we have a very old 
            starch archive. Hopefully, we won't run into too many of these files, but
            in case we do, we set some reasonable defaults.
        */

        streamsCompressionType = json_object_get(mdJSON, STARCH_METADATA_STREAM_COMPRESSION_FORMAT_KEY);
        *type = (CompressionType) json_integer_value(streamsCompressionType);
        *headerFlag = kStarchFalse;
        (*version)->major = 1;
        (*version)->minor = 0;
        (*version)->revision = 0;
        *cTime = (char *) malloc(1);
        (*cTime)[0] = 0;
    }

    /* parse JSON entity into streams */    
    if ( ( (*version)->major < STARCH_MAJOR_VERSION ) ||
         ( ((*version)->major == STARCH_MAJOR_VERSION ) && ((*version)->minor <= STARCH_MINOR_VERSION) && ((*version)->revision <= STARCH_REVISION_VERSION)) ) {

        streams = json_object_get(mdJSON, STARCH_METADATA_STREAM_LIST_KEY);
        if (!streams) {
            if (suppressErrorMsgs == kStarchFalse)
                fprintf(stderr, "ERROR: Could not extract streams from JSON entity.\n");
            return STARCH_FATAL_ERROR;
        }

        streamChr = (char *) malloc( STARCH_STREAM_METADATA_MAX_LENGTH + 1 );
        if (!streamChr) {
            if (suppressErrorMsgs == kStarchFalse)
                fprintf(stderr, "ERROR: Could not instantiate memory for stream chromosome string.\n");
            return STARCH_FATAL_ERROR;
        }

        streamFn = (char *) malloc( STARCH_STREAM_METADATA_MAX_LENGTH + 1 );
        if (!streamFn) {
            if (suppressErrorMsgs == kStarchFalse)
                fprintf(stderr, "ERROR: Could not instantiate memory for stream filename string.\n");
            return STARCH_FATAL_ERROR;
        }

        for (streamIdx = 0; streamIdx < json_array_size(streams); streamIdx++) {
            stream = json_array_get(streams, streamIdx);        
            if (!stream) {
                if (suppressErrorMsgs == kStarchFalse)
                    fprintf(stderr, "ERROR: Could not retrieve stream object\n");
                return STARCH_EXIT_FAILURE;
            }
            streamChromosome = json_object_get(stream, STARCH_METADATA_STREAM_CHROMOSOME_KEY);
            if (!streamChromosome) {
                if (suppressErrorMsgs == kStarchFalse)
                    fprintf(stderr, "ERROR: Could not retrieve stream chromosome object\n");
                return STARCH_EXIT_FAILURE;
            }
            streamFilename = json_object_get(stream, STARCH_METADATA_STREAM_FILENAME_KEY);
            if (!streamFilename) {
                if (suppressErrorMsgs == kStarchFalse)
                    fprintf(stderr, "ERROR: Could not retrieve stream filename object\n");
                return STARCH_EXIT_FAILURE;
            }
            streamSize = json_object_get(stream, STARCH_METADATA_STREAM_SIZE_KEY);
            if (!streamSize) {
                if (suppressErrorMsgs == kStarchFalse)
                    fprintf(stderr, "ERROR: Could not retrieve stream size object\n");
                return STARCH_EXIT_FAILURE;
            }
            streamSizeValue = (uint64_t) strtoull(json_string_value(streamSize), NULL, STARCH_RADIX);

            streamLineCount = json_object_get(stream, STARCH_METADATA_STREAM_LINECOUNT_KEY);
            if (!streamLineCount) {
                if (((*version)->major >= 1) && ((*version)->minor >= 3)) {
                    if (suppressErrorMsgs == kStarchFalse)
                        fprintf(stderr, "ERROR: Could not retrieve stream line count object with compliant version\n");
                    return STARCH_EXIT_FAILURE;
                }
                else {
                    streamLineCount = json_integer(STARCH_DEFAULT_LINE_COUNT);
                    streamLineCountValue = (Bed::LineCountType) json_integer_value(streamLineCount);
                    json_decref(streamLineCount);
                }
            }
            else
                streamLineCountValue = (unsigned long) json_integer_value(streamLineCount);            

            streamTotalNonUniqueBases = json_object_get(stream, STARCH_METADATA_STREAM_TOTALNONUNIQUEBASES_KEY);
            if (!streamTotalNonUniqueBases) {
                if (((*version)->major >= 1) && ((*version)->minor >= 4)) {
                    if (suppressErrorMsgs == kStarchFalse)
                        fprintf(stderr, "ERROR: Could not retrieve stream non-unique base count object with compliant version\n");
                    return STARCH_EXIT_FAILURE;
                }
                else {
                    streamTotalNonUniqueBases = json_integer(STARCH_DEFAULT_NON_UNIQUE_BASE_COUNT);
                    streamTotalNonUniqueBasesValue = (Bed::BaseCountType) json_integer_value(streamTotalNonUniqueBases);
                    json_decref(streamTotalNonUniqueBases);
                }
            }
            else
                streamTotalNonUniqueBasesValue = (unsigned long) json_integer_value(streamTotalNonUniqueBases);

            streamTotalUniqueBases = json_object_get(stream, STARCH_METADATA_STREAM_TOTALUNIQUEBASES_KEY);
            if (!streamTotalUniqueBases) {
                if (((*version)->major >= 1) && ((*version)->minor >= 4)) {
                    if (suppressErrorMsgs == kStarchFalse)
                        fprintf(stderr, "ERROR: Could not retrieve stream unique base count object with compliant version\n");
                    return STARCH_EXIT_FAILURE;
                }
                else {
                    streamTotalUniqueBases = json_integer(STARCH_DEFAULT_UNIQUE_BASE_COUNT);
                    streamTotalUniqueBasesValue = (Bed::BaseCountType) json_integer_value(streamTotalUniqueBases);
                    json_decref(streamTotalUniqueBases);
                }
            }
            else
                streamTotalUniqueBasesValue = (Bed::BaseCountType) json_integer_value(streamTotalUniqueBases);
            
            strncpy(streamChr, json_string_value(streamChromosome), strlen(json_string_value(streamChromosome)) + 1);
            strncpy(streamFn, json_string_value(streamFilename), strlen(json_string_value(streamFilename)) + 1);

            if (streamIdx == 0) {
                *rec = STARCH_createMetadata(streamChr, streamFn, streamSizeValue, streamLineCountValue, streamTotalNonUniqueBasesValue, streamTotalUniqueBasesValue);
                firstRec = *rec;
            }
            else
                *rec = STARCH_addMetadata(*rec, streamChr, streamFn, streamSizeValue, streamLineCountValue, streamTotalNonUniqueBasesValue, streamTotalUniqueBasesValue);
        }

        /* reset Metadata record pointer to first record */
        if (!firstRec) {
            if (suppressErrorMsgs == kStarchFalse)
                fprintf(stderr, "ERROR: Could not set initial metadata record pointer\n");
            return STARCH_EXIT_FAILURE;
        }
        *rec = firstRec;

        /* cleanup */
        if (streamChr)
            free(streamChr);
        if (streamFn)
            free(streamFn);        
        if ((mdJSON != NULL) && (preserveJSONRef == kStarchFalse)) {
            json_decref(mdJSON); 
            mdJSON = NULL;
        }        
    }
    else {
        /* version error */
        if (suppressErrorMsgs == kStarchFalse)
            fprintf(stderr, "ERROR: Cannot process newer archive (v%d.%d.%d) with older version of tools (v%d.%d.%d). Please update your toolkit.\n", (*version)->major, (*version)->minor, (*version)->revision, STARCH_MAJOR_VERSION, STARCH_MINOR_VERSION, STARCH_REVISION_VERSION);
        return STARCH_EXIT_FAILURE;
    }

    return STARCH_EXIT_SUCCESS;
}

int 
STARCH_mergeMetadataWithCompressedFiles(const Metadata *md, char *mdHeader) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_mergeMetadataWithCompressedFiles() ---\n");
#endif
    const Metadata *iter = NULL;
    FILE *outPtr = NULL;
    FILE *chrFnPtr = NULL;
    int c;

    if ((outPtr = freopen(NULL, "wb", stdout)) != NULL) {
        fwrite(mdHeader, strlen(mdHeader), 1, outPtr);
        for (iter = md; iter != NULL; iter = iter->next) {  
            chrFnPtr = fopen(iter->filename, "rb");
            if (!chrFnPtr) {
                fprintf(stderr, "ERROR: Could not open per-chromosome data file %s\n", iter->filename);
                return STARCH_EXIT_FAILURE;
            }
            while ((c = fgetc(chrFnPtr)) != EOF) {
                if (fputc(c, outPtr) == EOF) {
                    fprintf(stderr, "ERROR: Could not write to standard output stream\n");
                }
            }
            fclose(chrFnPtr);
        }
        fclose(outPtr);
    }
    else {
        fprintf(stderr, "ERROR: Could not open standard output stream\n");
        return STARCH_EXIT_FAILURE;
    }
    
    return STARCH_EXIT_SUCCESS;
}

void
STARCH_buildProcessIDTag(char **tag) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_buildProcessIDTag() ---\n");
#endif
    pid_t pid;
    struct utsname uts;
    char _tag[STARCH_TAG_MAX_LENGTH];
    
    pid = getpid();
    uname( &uts );
    sprintf(_tag, "pid%d.%s", pid, uts.nodename);
    *tag = STARCH_strndup(_tag, strlen(_tag) + 1);
}

ArchiveVersion * 
STARCH_copyArchiveVersion(const ArchiveVersion *oav)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_copyArchiveVersion() ---\n");
#endif
    ArchiveVersion *av = NULL;
    av = (ArchiveVersion *) malloc (sizeof(ArchiveVersion));
    if (av) {
        av->major = oav->major;
        av->minor = oav->minor;
        av->revision = oav->revision;
    }
    return av;
}

int
STARCH_readLegacyMetadata(const char *buf, Metadata **rec, CompressionType *type, ArchiveVersion **version, uint64_t *mdOffset, Boolean *headerFlag, const Boolean suppressErrorMsgs)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_readLegacyMetadata() ---\n");
#endif
    /*
        The legacy format predates the "official" (Google) release of starch
        and does not use any JSON. So we have to do things a little differently.

        Here's the structure of the 8192-byte header:

        --------------------------------------------------------------
        compressedFilename-1  \t  byteOffset-1  \n
        compressedFilename-2  \t  byteOffset-2  \n
        ...
        compressedFilename-N  \t  byteOffset-N  \n
        \n
        \n
        ...
        \n
        --------------------------------------------------------------

        We essentially walk through the char* 'buf' one character at a time,
        tokenizing on tabs. We obtain the extension of the filename (either .bz2 
        or .gz) to read the file's compression type. We then read the byte 
        offset. We leave out post-v1.2 features (line count, etc.).        
    */

    char *token = NULL;
    char *tokenCheck = NULL;
    char *recChromosome = NULL;
    char *recFilename = NULL;
    unsigned long recFileSize = 0UL;
    Bed::LineCountType recLineCountValue = STARCH_DEFAULT_LINE_COUNT;
    Bed::BaseCountType recNonUniqueBaseCountValue = STARCH_DEFAULT_NON_UNIQUE_BASE_COUNT;
    Bed::BaseCountType recUniqueBaseCountValue = STARCH_DEFAULT_UNIQUE_BASE_COUNT;
    char tokBuf[STARCH_LEGACY_METADATA_SIZE];
    char recTokBuf[STARCH_LEGACY_METADATA_SIZE];
    size_t bufIdx, tokBufIdx, recTokBufIdx;
    int tokenCount;
    int recIdx = 0;
    Metadata *firstRec = NULL;

    for (bufIdx = 0, tokBufIdx = 0; bufIdx < STARCH_LEGACY_METADATA_SIZE; bufIdx++, tokBufIdx++) {
        if (buf[bufIdx] == '\n') {
            if (tokBufIdx > 0)
                recIdx++;
            tokBuf[tokBufIdx] = '\0';
            token = strtok(tokBuf, "\t");
            for (tokenCount = 0; token != NULL; tokenCount++) {
                switch (tokenCount) {
                    case 0: {
                        tokenCheck = STARCH_strnstr((const char *)token, STARCH_LEGACY_EXTENSION_BZ2, strlen(token));
                        if (!tokenCheck) {
                            tokenCheck = STARCH_strnstr((const char *)token, STARCH_LEGACY_EXTENSION_GZIP, strlen(token));
                            if (!tokenCheck) {
                                if (suppressErrorMsgs == kStarchFalse)
                                    fprintf(stderr, "ERROR: Archive metadata is invalid -- check format (are you trying to open a newer archive with an older binary?)\n");
                                return STARCH_EXIT_FAILURE;
                            }
                            else 
                                *type = (CompressionType) kGzip; /* archive is gzip */
                        }
                        else
                            *type = (CompressionType) kBzip2; /* archive is bzip2 */

                        /* read chromosome name */
                        strncpy(recTokBuf, token, strlen(token) + 1);
                        for (recTokBufIdx = 0; recTokBufIdx < strlen(recTokBuf); recTokBufIdx++) {
                            if (recTokBuf[recTokBufIdx] == '.') {
                                recTokBuf[recTokBufIdx] = '\0';
                                break;
                            }
                        }
                        recChromosome = (char *) malloc(strlen(recTokBuf) + 1);
                        if (!recChromosome) {
                            fprintf(stderr, "ERROR: Could not allocate space for chromosome\n");
                            return STARCH_EXIT_FAILURE;
                        }
                        strncpy(recChromosome, recTokBuf, strlen(recTokBuf) + 1);

                        /* read filename */
                        recFilename = (char *) malloc(strlen(token) + 1);
                        if (!recFilename) {
                            fprintf(stderr, "ERROR: Could not allocate space for filename\n");
                            return STARCH_EXIT_FAILURE;
                        }
                        strncpy(recFilename, token, strlen(token) + 1);

                        break;
                    }
                    case 1: {
                        /* read file size */
                        recFileSize = strtoul(token, NULL, STARCH_RADIX);
                        break;
                    }
                }
                token = strtok(NULL, "\t");
            }

            /* if tokBufIdx is zero, then we are at the end of the metadata content */
            if (tokBufIdx != 0) {
                /* put record into metadata */
                if (recIdx == 1) {
                    *rec = STARCH_createMetadata(recChromosome, recFilename, recFileSize, recLineCountValue, recNonUniqueBaseCountValue, recUniqueBaseCountValue);
                    firstRec = *rec;
                }
                else
                    *rec = STARCH_addMetadata(*rec, recChromosome, recFilename, recFileSize, recLineCountValue, recNonUniqueBaseCountValue, recUniqueBaseCountValue);

                /* cleanup */
                if (recFilename)
                    free(recFilename), recFilename = NULL;
                if (recChromosome)
                    free(recChromosome), recChromosome = NULL;
            }
            else
                break;

            tokBufIdx = (size_t) -1;
        }
        else
            tokBuf[tokBufIdx] = buf[bufIdx];        
    }

    /* set archive version */
    (*version)->major = 1;
    (*version)->minor = 0;
    (*version)->revision = 0;

    /* set metadata offset */
    *mdOffset = STARCH_LEGACY_METADATA_SIZE;

    /* set header flag */
    *headerFlag = kStarchFalse;

    /* reset metadata pointer to first record */
    *rec = firstRec;

    return STARCH_EXIT_SUCCESS;
}

char * 
STARCH_strnstr(const char *haystack, const char *needle, size_t haystackLen) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_strnstr() ---\n");
#endif
    char *p;
    size_t pLen;
    size_t len = strlen(needle);

    /* everything matches empty string */
    if (*needle == '\0')    
        return (char *) haystack;

    pLen = haystackLen;
    for (p = (char *) haystack; p != NULL; p = (char *) memchr(p + 1, *needle, pLen-1)) {
        pLen = haystackLen - (size_t) (p - haystack);
        if (pLen < len)
            return NULL;
        if (strncmp(p, needle, len) == 0)
            return p;
    }

    return NULL;
}

int
STARCH_chromosomeInMetadataRecords(const Metadata *md, const char *chr) {
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_chromosomeInMetadataRecords() ---\n");
#endif
    const Metadata *iter;

    if (!md) {
        fprintf(stderr, "ERROR: Could not list chromosomes (metadata structure is empty)\n");
        return STARCH_EXIT_FAILURE;
    }

    for (iter = md; iter != NULL; iter = iter->next)
        if (strcmp(chr, iter->chromosome) == 0)
	    return STARCH_EXIT_SUCCESS;
    
    return STARCH_EXIT_FAILURE;
}
