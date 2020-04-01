//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starchstrip
// File:    starchstrip.c
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

#include "starchstrip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "data/starch/starchHelpers.h"
#include "data/starch/starchConstants.h"
#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchBase64Coding.h"
#include "data/starch/starchSha1Digest.h"
#include "suite/BEDOPS.Version.hpp"

#ifdef __cplusplus
namespace {
  using namespace starch;
} // unnamed namespace
#endif

int
main(int argc, char** argv)
{
#ifdef DEBUG
    fprintf(stderr, "\n--- starchstrip main() - enter ---\n");
#endif

    setlocale(LC_ALL, "POSIX");
    STARCHSTRIP_init_globals();
    STARCHSTRIP_init_command_line_options(argc, argv);
    STARCHSTRIP_init_chromosomes_list();
    /* validate input archive */
    STARCHSTRIP_init_archive_metadata();
    STARCHSTRIP_check_archive_version();
    /* validate input chromosome names against query names */
    STARCHSTRIP_check_chromosome_stream_names();
    /* write output archive */
    STARCHSTRIP_write_header(stdout);
    STARCHSTRIP_write_chromosome_streams(stdout);
    STARCHSTRIP_write_updated_metadata(stdout);
    /* cleanup */
    STARCHSTRIP_delete_globals();

#ifdef DEBUG
    fprintf(stderr, "\n--- starchstrip main() - exit ---\n");
#endif

    return EXIT_SUCCESS;
}

#ifdef __cplusplus
namespace starch {
#endif

static void
STARCHSTRIP_write_updated_metadata(FILE* os)
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_write_updated_metadata() - enter ---\n");
#endif
#ifdef __cplusplus
    char *md_json_buffer = nullptr;
    char *base64_encoded_sha1_digest = nullptr;
#else
    char *md_json_buffer = NULL;
    char *base64_encoded_sha1_digest = NULL;
#endif
    unsigned char sha1_digest[STARCH2_MD_FOOTER_SHA1_LENGTH] = {0};
    char footer_cumulative_record_size_buffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + 1] = {0};
    char footer_remainder_buffer[STARCH2_MD_FOOTER_REMAINDER_LENGTH + 1] = {0};
    char footer_buffer[STARCH2_MD_FOOTER_LENGTH + 1] = {0};

    if (!starchstrip_globals.output_records) {
        fprintf(stderr, "Error: Output metadata structure is empty after stripping records from input -- something went wrong in mid-stream\n");
        exit(EIO);
    }

    /* archive version and creation timestamp are NULL, in order to write default values */
    md_json_buffer = STARCH_generateJSONMetadata(starchstrip_globals.output_records,
                                                 starchstrip_globals.archive_type,
                                                 starchstrip_globals.archive_version,
#ifdef __cplusplus
						 nullptr,
#else
                                                 NULL,
#endif
                                                 starchstrip_globals.archive_note,
                                                 starchstrip_globals.archive_header_flag);
    if (!md_json_buffer) {
        fprintf(stderr, "Error: Could not write JSON-formatted metadata to buffer\n");
        exit(EIO);
    }
    fwrite(md_json_buffer, 1, strlen(md_json_buffer), os);
    fflush(os);

#ifdef __cplusplus
    STARCH_SHA1_All(reinterpret_cast<const unsigned char *>( md_json_buffer ), strlen(md_json_buffer), sha1_digest);
    STARCH_encodeBase64(&base64_encoded_sha1_digest,
            static_cast<const size_t>( STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH ),
            reinterpret_cast<const unsigned char *>( sha1_digest ),
            static_cast<const size_t>( STARCH2_MD_FOOTER_SHA1_LENGTH ));
    sprintf(footer_cumulative_record_size_buffer, "%020llu", static_cast<unsigned long long>( starchstrip_globals.cumulative_output_size ));
#else
    STARCH_SHA1_All((const unsigned char *) md_json_buffer, strlen(md_json_buffer), sha1_digest);
    STARCH_encodeBase64(&base64_encoded_sha1_digest,
            (const size_t) STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH,
            (const unsigned char *) sha1_digest,
            (const size_t) STARCH2_MD_FOOTER_SHA1_LENGTH);
    sprintf(footer_cumulative_record_size_buffer, "%020llu", (unsigned long long) starchstrip_globals.cumulative_output_size);
#endif

    memcpy(footer_buffer, footer_cumulative_record_size_buffer, strlen(footer_cumulative_record_size_buffer));
    memcpy(footer_buffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH, base64_encoded_sha1_digest, STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1); /* strip trailing null */
#ifdef __cplusplus
    memset(footer_remainder_buffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, static_cast<size_t>( STARCH2_MD_FOOTER_REMAINDER_LENGTH ));
#else
    memset(footer_remainder_buffer, STARCH2_MD_FOOTER_REMAINDER_UNUSED_CHAR, (size_t) STARCH2_MD_FOOTER_REMAINDER_LENGTH);
#endif

    memcpy(footer_buffer + STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1, footer_remainder_buffer, STARCH2_MD_FOOTER_REMAINDER_LENGTH); /* don't forget to offset pointer index by -1 for base64-sha1's null */
    footer_buffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 1] = '\0';
    footer_buffer[STARCH2_MD_FOOTER_CUMULATIVE_RECORD_SIZE_LENGTH + STARCH2_MD_FOOTER_BASE64_ENCODED_SHA1_LENGTH - 1 + STARCH2_MD_FOOTER_REMAINDER_LENGTH - 2] = '\n';
    fprintf(os, "%s", footer_buffer);
    fflush(os);

    /* cleanup */
#ifdef __cplusplus
    free(md_json_buffer);
    md_json_buffer = nullptr;
    free(base64_encoded_sha1_digest);
    base64_encoded_sha1_digest = nullptr;
#else
    free(md_json_buffer);
    md_json_buffer = NULL;
    free(base64_encoded_sha1_digest);
    base64_encoded_sha1_digest = NULL;
#endif

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_write_updated_metadata() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_write_chromosome_streams(FILE* os)
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_write_chromosome_streams() - enter ---\n");
#endif
#ifdef __cplusplus
    Metadata* iter = nullptr;
    Metadata* output_records_tail = nullptr;
#else
    Metadata* iter = NULL;
    Metadata* output_records_tail = NULL;
#endif
    size_t chr_to_process_idx = 0;
    uint64_t start_offset = 0;
    uint64_t bytes_to_copy = 0;
    size_t bytes_read = 0;
    char byte_buffer[starchstrip_copy_buffer_size];
    int records_added = 0;

    if (starchstrip_globals.archive_version->major == 2) {
        start_offset += STARCH2_MD_HEADER_BYTE_LENGTH;
#ifdef __cplusplus
        for (iter = starchstrip_globals.archive_records; iter != nullptr; iter = iter->next) {
#else
        for (iter = starchstrip_globals.archive_records; iter != NULL; iter = iter->next) {
#endif
            if ((starchstrip_globals.exclusion_flag) && (strcmp(iter->chromosome, starchstrip_globals.chromosomes_to_process[chr_to_process_idx]) == 0)) {
                continue;
            }
            if (((starchstrip_globals.inclusion_flag) && (strcmp(iter->chromosome, starchstrip_globals.chromosomes_to_process[chr_to_process_idx]) == 0)) || ((starchstrip_globals.exclusion_flag) && (strcmp(iter->chromosome, starchstrip_globals.chromosomes_to_process[chr_to_process_idx]) != 0)) ) {
#ifdef __cplusplus
                fseeko(starchstrip_globals.archive_fp, static_cast<off_t>( start_offset ), SEEK_SET);
#else
                fseeko(starchstrip_globals.archive_fp, (off_t) start_offset, SEEK_SET);
#endif
                // copy 
                bytes_to_copy = iter->size;
                do {
                    if (bytes_to_copy > starchstrip_copy_buffer_size) {
                        bytes_read = fread(byte_buffer, sizeof(char), starchstrip_copy_buffer_size, starchstrip_globals.archive_fp);
                        if (bytes_read != starchstrip_copy_buffer_size) {
                            fprintf(stderr, "Error: Could not copy 'starchstrip_copy_buffer_size' bytes into intermediate buffer\n");
                            exit(EIO); /* Input/output error (POSIX.1) */
                        }
                        fwrite(byte_buffer, sizeof(char), starchstrip_copy_buffer_size, os);
                        bytes_to_copy -= starchstrip_copy_buffer_size;
                    }
                    else {
#ifdef __cplusplus
                        bytes_read = fread(byte_buffer, sizeof(char), static_cast<size_t>( bytes_to_copy ), starchstrip_globals.archive_fp);
#else
                        bytes_read = fread(byte_buffer, sizeof(char), (size_t) bytes_to_copy, starchstrip_globals.archive_fp);
#endif
                        if (bytes_read != bytes_to_copy) {
                            fprintf(stderr, "Error: Could not copy 'bytes_to_copy' bytes into intermediate buffer\n");
                            exit(EIO); /* Input/output error (POSIX.1) */
                        }
#ifdef __cplusplus
                       fwrite(byte_buffer, sizeof(char), static_cast<size_t>( bytes_to_copy ), os);
#else
                       fwrite(byte_buffer, sizeof(char), (size_t) bytes_to_copy, os);
#endif
                       bytes_to_copy = 0;
                    }
                } while (bytes_to_copy > 0);

                // increment cumulative output file size
                starchstrip_globals.cumulative_output_size += iter->size;

#ifdef __cplusplus
                if (records_added == 0) {
                    output_records_tail = STARCH_createMetadata( const_cast<char *>( iter->chromosome ),
                                                                 iter->filename,
                                                                 iter->size,
                                                                 iter->lineCount,
                                                                 iter->totalNonUniqueBases,
                                                                 iter->totalUniqueBases,
                                                                 iter->duplicateElementExists,
                                                                 iter->nestedElementExists,
                                                                 iter->signature,
                                                                 iter->lineMaxStringLength );
                    starchstrip_globals.output_records = output_records_tail;
                }
                else {
                    output_records_tail = STARCH_addMetadata( output_records_tail,
                                                              const_cast<char *>( iter->chromosome ),
                                                              iter->filename,
                                                              iter->size,
                                                              iter->lineCount,
                                                              iter->totalNonUniqueBases,
                                                              iter->totalUniqueBases,
                                                              iter->duplicateElementExists,
                                                              iter->nestedElementExists,
                                                              iter->signature,
                                                              iter->lineMaxStringLength );
                }
#else
                if (records_added == 0) {
                    output_records_tail = STARCH_createMetadata( (char *) iter->chromosome,
                                                                 iter->filename,
                                                                 iter->size,
                                                                 iter->lineCount,
                                                                 iter->totalNonUniqueBases,
                                                                 iter->totalUniqueBases,
                                                                 iter->duplicateElementExists,
                                                                 iter->nestedElementExists,
                                                                 iter->signature,
                                                                 iter->lineMaxStringLength );
                    starchstrip_globals.output_records = output_records_tail;
                }
                else {
                    output_records_tail = STARCH_addMetadata( output_records_tail,
                                                             (char *) iter->chromosome,
                                                             iter->filename,
                                                             iter->size,
                                                             iter->lineCount,
                                                             iter->totalNonUniqueBases,
                                                             iter->totalUniqueBases,
                                                             iter->duplicateElementExists,
                                                             iter->nestedElementExists,
                                                             iter->signature,
                                                             iter->lineMaxStringLength );
                }
#endif
                // increment records
                records_added++;

                // increment query chromosome within bounds
                if (chr_to_process_idx < (starchstrip_globals.chromosomes_to_process_num - 1)) {
                    chr_to_process_idx++;
                }
            }
            start_offset += iter->size;
        }
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_write_chromosome_streams() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_write_header(FILE* os)
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_write_header() - enter ---\n");
#endif

    if (starchstrip_globals.archive_version->major == 2) {
#ifdef __cplusplus
        unsigned char* archive_header = nullptr;
#else
        unsigned char* archive_header = NULL;
#endif
        if (STARCH2_initializeStarchHeader(&archive_header) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "Error: Could not initialize output archive header\n");
            exit(EIO); /* Input/output error (POSIX.1) */
        }
        if (STARCH2_writeStarchHeaderToOutputFp(archive_header, os) != STARCH_EXIT_SUCCESS) {
            fprintf(stderr, "Error: Could not write archive header to output file stream\n");
            exit(EIO); /* Input/output error (POSIX.1) */
        }
        free(archive_header);
#ifdef __cplusplus
        archive_header = nullptr;
#else
        archive_header = NULL;
#endif
        starchstrip_globals.cumulative_output_size += STARCH2_MD_HEADER_BYTE_LENGTH;
    }
    else {
        fprintf(stderr, "Error: Unable to write archive header (archive version unsupported)\n");
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_write_header() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_check_chromosome_stream_names()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_check_chromosome_stream_names() - enter ---\n");
#endif
#ifdef __cplusplus
    Metadata* iter = nullptr;
#else
    Metadata* iter = NULL;
#endif
    size_t chr_idx = 0;
    size_t chr_to_process_idx = 0;
    size_t num_records = 0;
    size_t num_records_to_include = 0;
    size_t num_records_to_exclude = 0;

    // loop through input metadata records, compare against chromosome name array
    if (starchstrip_globals.archive_version->major == 2) {
        // full count
#ifdef __cplusplus
        for (iter = starchstrip_globals.archive_records; iter != nullptr; iter = iter->next) {
#else
        for (iter = starchstrip_globals.archive_records; iter != NULL; iter = iter->next) {
#endif
            num_records++;
        }
        // inclusion
        if (starchstrip_globals.inclusion_flag) {
#ifdef __cplusplus
            for (iter = starchstrip_globals.archive_records; iter != nullptr; iter = iter->next) {
#else
            for (iter = starchstrip_globals.archive_records; iter != NULL; iter = iter->next) {
#endif
                while (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) > 0) {
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
                if (chr_idx == starchstrip_globals.chromosomes_num) {
                    break;
                }
                if (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) == 0) {
                    num_records_to_include++;
                    //fprintf(stderr, "Debug: Including [%s]\n", starchstrip_globals.chromosomes[chr_idx]);
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
            }
            //fprintf(stderr, "Debug: Including [%d] of [%d] records\n", num_records_to_include, num_records);
            if (num_records_to_include == 0) {
                fprintf(stderr, "Error: No chromosomes were found in input archive with matching names (output would be empty)\n");
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            if (num_records_to_include == num_records) {
                fprintf(stderr, "Error: All specified chromosome names were found in input archive (output would be identical to input)\n");
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            starchstrip_globals.chromosomes_to_process_num = num_records_to_include;
#ifdef __cplusplus
            starchstrip_globals.chromosomes_to_process = static_cast<char**>( malloc(sizeof(char*) * starchstrip_globals.chromosomes_to_process_num) );
            for (chr_to_process_idx = 0; chr_to_process_idx < starchstrip_globals.chromosomes_to_process_num; ++chr_to_process_idx) {
                starchstrip_globals.chromosomes_to_process[chr_to_process_idx] = nullptr;
            }
#else
            starchstrip_globals.chromosomes_to_process = malloc(sizeof(char*) * starchstrip_globals.chromosomes_to_process_num);
            for (chr_to_process_idx = 0; chr_to_process_idx < starchstrip_globals.chromosomes_to_process_num; ++chr_to_process_idx) {
                starchstrip_globals.chromosomes_to_process[chr_to_process_idx] = NULL;
            }
#endif
            chr_idx = 0;
            chr_to_process_idx = 0;
#ifdef __cplusplus
            for (iter = starchstrip_globals.archive_records; iter != nullptr; iter = iter->next) {
#else
            for (iter = starchstrip_globals.archive_records; iter != NULL; iter = iter->next) {
#endif
                while (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) > 0) {
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
                if (chr_idx == starchstrip_globals.chromosomes_num) {
                    break;
                }
                if (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) == 0) {
#ifdef __cplusplus
                    starchstrip_globals.chromosomes_to_process[chr_to_process_idx] = static_cast <char*>( malloc( strlen(iter->chromosome) + 1 ) );
#else
                    starchstrip_globals.chromosomes_to_process[chr_to_process_idx] = malloc(strlen(iter->chromosome) + 1);
#endif
                    if (!starchstrip_globals.chromosomes_to_process[chr_to_process_idx]) {
                        fprintf(stderr, "Error: Could not allocate space for chromosome-to-process list argument\n");
                        exit(ENOMEM); /* Not enough space (POSIX.1) */
                    }
                    memcpy(starchstrip_globals.chromosomes_to_process[chr_to_process_idx], iter->chromosome, strlen(iter->chromosome) + 1);
                    chr_to_process_idx++;
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
            }
        }

        // exclusion
        else if (starchstrip_globals.exclusion_flag) {
#ifdef __cplusplus
            for (iter = starchstrip_globals.archive_records; iter != nullptr; iter = iter->next) {
#else
            for (iter = starchstrip_globals.archive_records; iter != NULL; iter = iter->next) {
#endif
                while (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) > 0) {
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
                if (chr_idx == starchstrip_globals.chromosomes_num) {
                    break;
                }
                if (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) == 0) {
                    num_records_to_exclude++;
                    //fprintf(stderr, "Debug: Excluding [%s]\n", starchstrip_globals.chromosomes[chr_idx]);
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
            }
            //fprintf(stderr, "Debug: Excluding [%d] of [%d] records\n", num_records_to_exclude, num_records);
            if (num_records_to_exclude == 0) {
                fprintf(stderr, "Error: No chromosomes were found in input archive with matching names (output would be identical to input)\n");
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            if (num_records_to_exclude == num_records) {
                fprintf(stderr, "Error: All specified chromosome names were found in input archive (output would be empty)\n");
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            starchstrip_globals.chromosomes_to_process_num = num_records_to_exclude;
#ifdef __cplusplus
            starchstrip_globals.chromosomes_to_process = static_cast<char**>( malloc(sizeof(char*) * starchstrip_globals.chromosomes_to_process_num) );
            for (chr_idx = 0; chr_idx < starchstrip_globals.chromosomes_to_process_num; ++chr_idx) {
                starchstrip_globals.chromosomes_to_process[chr_idx] = nullptr;
            }
#else
            starchstrip_globals.chromosomes_to_process = malloc(sizeof(char*) * starchstrip_globals.chromosomes_to_process_num);
            for (chr_idx = 0; chr_idx < starchstrip_globals.chromosomes_to_process_num; ++chr_idx) {
                starchstrip_globals.chromosomes_to_process[chr_idx] = NULL;
            }
#endif
            chr_idx = 0;
            chr_to_process_idx = 0;
#ifdef __cplusplus
            for (iter = starchstrip_globals.archive_records; iter != nullptr; iter = iter->next) {
#else
            for (iter = starchstrip_globals.archive_records; iter != NULL; iter = iter->next) {
#endif
                while (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) > 0) {
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
                if (chr_idx == starchstrip_globals.chromosomes_num) {
                    break;
                }
                if (strcmp(iter->chromosome, starchstrip_globals.chromosomes[chr_idx]) == 0) {
#ifdef __cplusplus
                    starchstrip_globals.chromosomes_to_process[chr_to_process_idx] = static_cast <char*>( malloc( strlen(iter->chromosome) + 1 ) );
#else
                    starchstrip_globals.chromosomes_to_process[chr_to_process_idx] = malloc(strlen(iter->chromosome) + 1);
#endif
                    if (!starchstrip_globals.chromosomes_to_process[chr_to_process_idx]) {
                        fprintf(stderr, "Error: Could not allocate space for chromosome-to-process list argument\n");
                        exit(ENOMEM); /* Not enough space (POSIX.1) */
                    }
                    memcpy(starchstrip_globals.chromosomes_to_process[chr_to_process_idx], iter->chromosome, strlen(iter->chromosome) + 1);
                    chr_to_process_idx++;
                    chr_idx++;
                    if (chr_idx == starchstrip_globals.chromosomes_num) {
                        break;
                    }
                }
            }
        }
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_check_chromosome_stream_names() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_check_archive_version()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_check_archive_version() - enter ---\n");
#endif

    if ((starchstrip_globals.archive_version->major < starchstrip_archive_version_major_minimum)
        ||
        ((starchstrip_globals.archive_version->major == starchstrip_archive_version_major_minimum) && (starchstrip_globals.archive_version->minor < starchstrip_archive_version_minor_minimum))) {
        fprintf(stderr, "Error: Archive must be v2.1 or greater -- use 'starchcat' to update archive, if needed\n");
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_check_archive_version() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_init_archive_metadata()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_archive_metadata() - enter ---\n");
#endif

    if (STARCH_readJSONMetadata( &starchstrip_globals.archive_metadata_json,
                                 &starchstrip_globals.archive_fp,
#ifdef __cplusplus
                                 reinterpret_cast<const char *>( starchstrip_globals.archive_fn ),
#else
                                 (const char *) starchstrip_globals.archive_fn,
#endif
                                 &starchstrip_globals.archive_records,
                                 &starchstrip_globals.archive_type,
                                 &starchstrip_globals.archive_version,
                                 &starchstrip_globals.archive_timestamp,
                                 &starchstrip_globals.archive_note,
                                 &starchstrip_globals.archive_metadata_offset,
                                 &starchstrip_globals.archive_header_flag,
                                 starchstrip_globals.archive_suppress_error_msgs,
                                 starchstrip_globals.archive_preserve_json_ref) != STARCH_EXIT_SUCCESS) {
        fprintf(stderr, "Error: Could not read metadata from archive -- use 'unstarch --is-starch' to test if archive is valid\n");
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_archive_metadata() - exit  ---\n");
#endif
}

/* 
   specifying special attribute for STARCHSTRIP_debug_chromosomes_to_query_list() to avoid: "warning: unused 
   function 'STARCHSTRIP_debug_chromosomes_to_query_list' [-Wunused-function]" message during non-debug compilation
   cf. http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Function-Attributes.html#Function%20Attributes
*/
#if defined(__GNUC__)
static void STARCHSTRIP_debug_chromosomes_to_query_list() __attribute__ ((unused));
#endif

static void
STARCHSTRIP_debug_chromosomes_to_query_list()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_debug_chromosomes_to_query_list() - enter ---\n");
#endif

    fprintf(stderr, "number of chromosomes to query [%zu]\n", starchstrip_globals.chromosomes_num);
    for (size_t chr_idx = 0; chr_idx < starchstrip_globals.chromosomes_num; ++chr_idx) {
        fprintf(stderr, "chr [%s]\n", starchstrip_globals.chromosomes[chr_idx]);
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_debug_chromosomes_to_query_list() - exit  ---\n");
#endif
}

/* 
   specifying special attribute for STARCHSTRIP_debug_chromosomes_to_process_list() to avoid: "warning: unused 
   function 'STARCHSTRIP_debug_chromosomes_to_process_list' [-Wunused-function]" message during non-debug compilation
   cf. http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Function-Attributes.html#Function%20Attributes
*/
#if defined(__GNUC__)
static void STARCHSTRIP_debug_chromosomes_to_process_list() __attribute__ ((unused));
#endif

static void
STARCHSTRIP_debug_chromosomes_to_process_list()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_debug_chromosomes_to_process_list() - enter ---\n");
#endif

    fprintf(stderr, "number of chromosomes to process [%zu]\n", starchstrip_globals.chromosomes_to_process_num);
    for (size_t chr_to_process_idx = 0; chr_to_process_idx < starchstrip_globals.chromosomes_to_process_num; ++chr_to_process_idx) {
        fprintf(stderr, "chr [%s]\n", starchstrip_globals.chromosomes_to_process[chr_to_process_idx]);
    }

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_debug_chromosomes_to_process_list() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_init_chromosomes_list()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_chromosomes_list() - enter ---\n");
#endif

    /* count delimiters */
    int delimiters_found = 0;
    int pos = 0;
    while (starchstrip_globals.chromosomes_str[pos] != '\0') {
        if (starchstrip_globals.chromosomes_str[pos++] == starchstrip_chromosomes_str_delimiter) {
            delimiters_found++;
        }
    }
#ifdef __cplusplus
    starchstrip_globals.chromosomes_num = static_cast<size_t>( delimiters_found + 1 );
    starchstrip_globals.chromosomes = static_cast<char**>( malloc(sizeof(char*) * starchstrip_globals.chromosomes_num) );
    for (size_t chr_idx = 0; chr_idx < starchstrip_globals.chromosomes_num; ++chr_idx) {
        starchstrip_globals.chromosomes[chr_idx] = nullptr;
    }
#else
    starchstrip_globals.chromosomes_num = (size_t) delimiters_found + 1;
    starchstrip_globals.chromosomes = malloc(sizeof(char*) * starchstrip_globals.chromosomes_num);
    for (size_t chr_idx = 0; chr_idx < starchstrip_globals.chromosomes_num; ++chr_idx) {
        starchstrip_globals.chromosomes[chr_idx] = NULL;
    }
#endif

    /* populate chromosome name array */
    size_t start = 0;
    size_t end = 0;
    int chr_idx = 0;
    size_t chromosome_str_length = 0;
    while (starchstrip_globals.chromosomes_str[end] != '\0') {
        if (starchstrip_globals.chromosomes_str[end] == starchstrip_chromosomes_str_delimiter) {
            chromosome_str_length = end - start;
#ifdef __cplusplus
            starchstrip_globals.chromosomes[chr_idx] = static_cast<char*>( malloc(chromosome_str_length + 1) );
#else
            starchstrip_globals.chromosomes[chr_idx] = (char*) malloc(chromosome_str_length + 1);
#endif
            memcpy(starchstrip_globals.chromosomes[chr_idx], starchstrip_globals.chromosomes_str + start, chromosome_str_length);
            starchstrip_globals.chromosomes[chr_idx][chromosome_str_length] = '\0';
            chr_idx++;
            start = ++end;
        }
        end++;
    }
    chromosome_str_length = end - start;
#ifdef __cplusplus
    starchstrip_globals.chromosomes[chr_idx] = static_cast<char*>( malloc(chromosome_str_length + 1) );
#else
    starchstrip_globals.chromosomes[chr_idx] = (char*) malloc(chromosome_str_length + 1);
#endif
    memcpy(starchstrip_globals.chromosomes[chr_idx], starchstrip_globals.chromosomes_str + start, chromosome_str_length);
    starchstrip_globals.chromosomes[chr_idx][chromosome_str_length] = '\0';

    /* sort chromosome names */
    qsort(starchstrip_globals.chromosomes, starchstrip_globals.chromosomes_num, sizeof(char*), STARCHSTRIP_compare_chromosome_names);

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_chromosomes_list() - exit  ---\n");
#endif
}

static int
STARCHSTRIP_compare_chromosome_names(const void* a, const void* b)
{
#ifdef __cplusplus
    auto a_recast = const_cast<void *>(a);
    auto b_recast = const_cast<void *>(b);
    auto a_rerecast = const_cast<const char **>(reinterpret_cast<char **>(a_recast));
    auto b_rerecast = const_cast<const char **>(reinterpret_cast<char **>(b_recast));
    return strcmp(*a_rerecast, *b_rerecast);
#else
    return strcmp(*(const char**) a, *(const char**) b);
#endif
}

static void
STARCHSTRIP_init_globals()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_globals() - enter ---\n");
#endif

#ifdef __cplusplus
    starchstrip_globals.chromosomes_str = nullptr;
    starchstrip_globals.chromosomes = nullptr;
    starchstrip_globals.chromosomes_to_process = nullptr;
    starchstrip_globals.output_records = nullptr;
    starchstrip_globals.archive_fn = nullptr;
    starchstrip_globals.archive_metadata_json = nullptr;
    starchstrip_globals.archive_fp = nullptr;
    starchstrip_globals.archive_records = nullptr;
    starchstrip_globals.archive_version = nullptr;
    starchstrip_globals.archive_timestamp = nullptr;
    starchstrip_globals.archive_note = nullptr;
#else
    starchstrip_globals.chromosomes_str = NULL;
    starchstrip_globals.chromosomes = NULL;
    starchstrip_globals.chromosomes_to_process = NULL;
    starchstrip_globals.output_records = NULL;
    starchstrip_globals.archive_fn = NULL;
    starchstrip_globals.archive_metadata_json = NULL;
    starchstrip_globals.archive_fp = NULL;
    starchstrip_globals.archive_records = NULL;
    starchstrip_globals.archive_version = NULL;
    starchstrip_globals.archive_timestamp = NULL;
    starchstrip_globals.archive_note = NULL;
#endif
    starchstrip_globals.chromosomes_num = 0;
    starchstrip_globals.chromosomes_to_process_num = 0;
    starchstrip_globals.inclusion_flag = kStarchFalse;
    starchstrip_globals.exclusion_flag = kStarchFalse;
    starchstrip_globals.cumulative_output_size = 0;
    // --
    starchstrip_globals.archive_type = STARCH_DEFAULT_COMPRESSION_TYPE;
    starchstrip_globals.archive_metadata_offset = UINT64_C(0);
    starchstrip_globals.archive_header_flag = kStarchFalse;
    starchstrip_globals.archive_suppress_error_msgs = kStarchTrue;
    starchstrip_globals.archive_preserve_json_ref = kStarchTrue;

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_globals() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_delete_globals()
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_delete_globals() - enter ---\n");
#endif

#ifdef __cplusplus
    for (size_t idx = 0; idx < starchstrip_globals.chromosomes_num; ++idx) {
        free(starchstrip_globals.chromosomes[idx]);
        starchstrip_globals.chromosomes[idx] = nullptr;
    }
    free(starchstrip_globals.chromosomes);
    starchstrip_globals.chromosomes = nullptr;
    starchstrip_globals.chromosomes_num = 0;

    free(starchstrip_globals.chromosomes_str);
    starchstrip_globals.chromosomes_str = nullptr;

    for (size_t idx = 0; idx < starchstrip_globals.chromosomes_to_process_num; ++idx) {
        free(starchstrip_globals.chromosomes_to_process[idx]);
        starchstrip_globals.chromosomes_to_process[idx] = nullptr;
    }
    free(starchstrip_globals.chromosomes_to_process);
    starchstrip_globals.chromosomes_to_process = nullptr;
    starchstrip_globals.chromosomes_to_process_num = 0;

    free(starchstrip_globals.archive_fn);
    starchstrip_globals.archive_fn = nullptr;
    if (starchstrip_globals.archive_metadata_json) {
        json_decref(starchstrip_globals.archive_metadata_json);
        starchstrip_globals.archive_metadata_json = nullptr;
    }
    if (starchstrip_globals.archive_fp) {
        fclose(starchstrip_globals.archive_fp);
        starchstrip_globals.archive_fp = nullptr;
    }
    if (starchstrip_globals.archive_records) {
        STARCH_freeMetadata(&starchstrip_globals.archive_records);
        starchstrip_globals.archive_records = nullptr;
    }
    if (starchstrip_globals.archive_version) {
        free(starchstrip_globals.archive_version);
        starchstrip_globals.archive_version = nullptr;
    }
    if (starchstrip_globals.archive_timestamp) {
        free(starchstrip_globals.archive_timestamp);
        starchstrip_globals.archive_timestamp = nullptr;
    }
    if (starchstrip_globals.archive_note) {
        free(starchstrip_globals.archive_note);
        starchstrip_globals.archive_note = nullptr;
    }
#else
    for (size_t idx = 0; idx < starchstrip_globals.chromosomes_num; ++idx) {
        free(starchstrip_globals.chromosomes[idx]);
        starchstrip_globals.chromosomes[idx] = NULL;
    }
    free(starchstrip_globals.chromosomes);
    starchstrip_globals.chromosomes = NULL;
    starchstrip_globals.chromosomes_num = 0;

    free(starchstrip_globals.chromosomes_str);
    starchstrip_globals.chromosomes_str = NULL;

    for (size_t idx = 0; idx < starchstrip_globals.chromosomes_to_process_num; ++idx) {
        free(starchstrip_globals.chromosomes_to_process[idx]);
        starchstrip_globals.chromosomes_to_process[idx] = NULL;
    }
    free(starchstrip_globals.chromosomes_to_process);
    starchstrip_globals.chromosomes_to_process = NULL;
    starchstrip_globals.chromosomes_to_process_num = 0;

    free(starchstrip_globals.archive_fn);
    starchstrip_globals.archive_fn = NULL;
    if (starchstrip_globals.archive_metadata_json) {
        json_decref(starchstrip_globals.archive_metadata_json);
        starchstrip_globals.archive_metadata_json = NULL;
    }
    if (starchstrip_globals.archive_fp) {
        fclose(starchstrip_globals.archive_fp);
        starchstrip_globals.archive_fp = NULL;
    }
    if (starchstrip_globals.archive_records) {
        STARCH_freeMetadata(&starchstrip_globals.archive_records);
        starchstrip_globals.archive_records = NULL;
    }
    if (starchstrip_globals.archive_version) {
        free(starchstrip_globals.archive_version);
        starchstrip_globals.archive_version = NULL;
    }
    if (starchstrip_globals.archive_timestamp) {
        free(starchstrip_globals.archive_timestamp);
        starchstrip_globals.archive_timestamp = NULL;
    }
    if (starchstrip_globals.archive_note) {
        free(starchstrip_globals.archive_note);
        starchstrip_globals.archive_note = NULL;
    }
#endif

    STARCH_freeMetadata(&starchstrip_globals.output_records);

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_delete_globals() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_init_command_line_options(int argc, char** argv)
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_command_line_options() - enter ---\n");
#endif

    size_t optarg_length = 0;
    int client_operation_count = 0;
    int client_long_index;
    int client_opt = getopt_long(argc,
                                 argv,
                                 starchstrip_opt_string,
                                 starchstrip_long_options,
                                 &client_long_index);

    opterr = 0; /* disable error reporting by GNU getopt */

    while (client_opt != -1) {
        switch (client_opt) {
            case 'i':
                if (client_operation_count > 1) {
                    fprintf(stderr, "Error: Cannot specify both inclusion and exclusion operands\n");
                    STARCHSTRIP_print_usage(stderr);
                    exit(EINVAL); /* Invalid argument (POSIX.1) */
                }
                if (!optarg) {
                    fprintf(stderr, "Error: Chromosome list argument unspecified\n");
                    STARCHSTRIP_print_usage(stderr);
                    exit(EINVAL); /* Invalid argument (POSIX.1) */
                }
                optarg_length = strlen(optarg);
#ifdef __cplusplus
                starchstrip_globals.chromosomes_str = static_cast<char*>( malloc(optarg_length + 1) );
#else
                starchstrip_globals.chromosomes_str = malloc(optarg_length + 1);
#endif
                if (!starchstrip_globals.chromosomes_str) {
                    fprintf(stderr, "Error: Could not allocate space for chromosome list argument\n");
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(starchstrip_globals.chromosomes_str, optarg, optarg_length + 1);
                starchstrip_globals.inclusion_flag = kStarchTrue;
                client_operation_count++;
                break;
            case 'x':
                if (client_operation_count > 1) {
                    fprintf(stderr, "Error: Cannot specify both inclusion and exclusion operands\n");
                    STARCHSTRIP_print_usage(stderr);
                    exit(EINVAL); /* Invalid argument (POSIX.1) */
                }
                if (!optarg) {
                    fprintf(stderr, "Error: Chromosome list argument unspecified\n");
                    STARCHSTRIP_print_usage(stderr);
                    exit(EINVAL); /* Invalid argument (POSIX.1) */
                }
                optarg_length = strlen(optarg);
#ifdef __cplusplus
                starchstrip_globals.chromosomes_str = static_cast<char*>( malloc(optarg_length + 1) );
#else
                starchstrip_globals.chromosomes_str = malloc(optarg_length + 1);
#endif
                if (!starchstrip_globals.chromosomes_str) {
                    fprintf(stderr, "Error: Could not allocate space for chromosome list argument\n");
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(starchstrip_globals.chromosomes_str, optarg, optarg_length + 1);
                starchstrip_globals.exclusion_flag = kStarchTrue;
                client_operation_count++;
                break;
            case 'v':
                STARCHSTRIP_print_version(stdout);
                exit(EXIT_SUCCESS);
            case 'h':
            case '?':
                STARCHSTRIP_print_usage(stdout);
                exit(EXIT_SUCCESS);
            default:
                break;
        }
        client_opt = getopt_long(argc,
                                 argv,
                                 starchstrip_opt_string,
                                 starchstrip_long_options,
                                 &client_long_index);
    }

    if (argc != 4) {
        fprintf(stderr, "Error: Starch file not specified\n");
        STARCHSTRIP_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }
    char* input_filename = argv[3];
    if (!STARCHSTRIP_file_exists(input_filename)) {
        fprintf(stderr, "Error: Starch file [%s] does not exist or is not accessible\n", input_filename);
        STARCHSTRIP_print_usage(stderr);
        exit(ENOENT); /* No such file or directory (POSIX.1) */
    }
    size_t input_filename_length = strlen(input_filename);
#ifdef __cplusplus
    starchstrip_globals.archive_fn = static_cast<char *>( malloc(input_filename_length + 1) );
#else
    starchstrip_globals.archive_fn = malloc(input_filename_length + 1);
#endif
    memcpy(starchstrip_globals.archive_fn, input_filename, input_filename_length + 1);

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_init_command_line_options() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_print_version(FILE* os)
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_print_version() - enter ---\n");
#endif

    fprintf(os,
            "%s\n"                   \
            "  citation: %s\n"       \
            "  version:  %s\n"       \
            "  authors:  %s\n",
            name,
            BEDOPS::citation(),
            BEDOPS::version(),
            authors);

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_print_version() - exit  ---\n");
#endif
}

static void
STARCHSTRIP_print_usage(FILE* os)
{
#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_print_usage() - enter ---\n");
#endif

    fprintf(os,
            "%s\n"                  \
            "  citation: %s\n"      \
            "  version:  %s\n"      \
            "  authors:  %s\n"      \
            "%s\n",
            name,
            BEDOPS::citation(),
            BEDOPS::version(),
            authors,
            usage);

#ifdef DEBUG
    fprintf(stderr, "--- STARCHSTRIP_print_usage() - exit  ---\n");
#endif
}

static Boolean
STARCHSTRIP_file_exists(char* fn)
{
  struct stat buf;
  return stat(fn, &buf) == 0 ? kStarchTrue : kStarchFalse;
}

#ifdef __cplusplus
} // namespace starch
#endif
