//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starchstrip
// File:    starchstrip.h
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

#ifndef STARCHSTRIP_H
#define STARCHSTRIP_H

#include <getopt.h>
#include <inttypes.h>
#include <errno.h>

#include "data/starch/starchMetadataHelpers.h"

#ifdef __cplusplus
namespace {
  using namespace starch;
} // unnamed namespace
#endif

static const char *name = "starchstrip";
static const char *authors = "Alex Reynolds and Shane Neph";
static const char *usage = "\n" \
    "USAGE: starchstrip [ --include | --exclude ] <chromosome-list> <starch-file>\n" \
    "    \n" \
    "    * Add either the --include or --exclude argument to filter the specified\n" \
    "      <starch-file> for chromosomes in <chromosome-list> for inclusion or\n" \
    "      exclusion, respectively. Note that you can only specify either inclusion\n" \
    "      or exclusion.\n\n" \
    "    * The <chromosome-list> argument is a comma-separated list of chromosome names\n" \
    "      to be included or excluded. This list is a *required* argument to either of the\n" \
    "      two --include and --exclude options.\n\n" \
    "    * The output is a Starch archive containing those chromosomes specified for inclusion\n" \
    "      or what chromosomes remain after exclusion from the original <starch-file>. A new\n" \
    "      metadata payload is appended to the output Starch archive.\n\n" \
    "    * The output is written to the standard output stream -- use the output redirection\n" \
    "      operator to write the result to a regular file, e.g.:\n\n" \
    "        $ starchstrip --exclude chrA,chrB,chrC in.starch > out.starch\n\n" \
    "    * If a specified chromosome is not in the input Starch archive, it will be ignored\n" \
    "      during processing.\n\n" \
    "    * Filtering simply copies over raw bytes from the input Starch archive and\n" \
    "      no extraction or recompression is performed. Use 'starchcat' to update the\n" \
    "      metadata, if new attributes are required.\n\n" \
    "    Process Flags\n" \
    "    --------------------------------------------------------------------------\n" \
    "    --include <chromosome-list>     Include specified chromosomes from <starch-file>.\n\n" \
    "    --exclude <chromosome-list>     Exclude specified chromosomes from <starch-file>.\n\n" \
    "    --version                       Show binary version.\n\n" \
    "    --help                          Show this usage message.\n";

static struct globals {
    char* chromosomes_str;
    char** chromosomes;
    size_t chromosomes_num;
    char** chromosomes_to_process;
    size_t chromosomes_to_process_num;
    Boolean inclusion_flag;
    Boolean exclusion_flag;
    size_t cumulative_output_size;
    Metadata* output_records;
    // --
    char* archive_fn;
    json_t *archive_metadata_json;
    FILE* archive_fp;
    Metadata* archive_records;
    CompressionType archive_type;
    ArchiveVersion* archive_version;
    char* archive_timestamp;
    char* archive_note;
    uint64_t archive_metadata_offset;
    Boolean archive_header_flag;
    Boolean archive_suppress_error_msgs;
    Boolean archive_preserve_json_ref;
} starchstrip_globals;

#ifdef __cplusplus
static struct option starchstrip_long_options[] = { 
    { "include",         required_argument,     nullptr,     'i' },
    { "exclude",         required_argument,     nullptr,     'x' },
    { "version",         no_argument,           nullptr,     'v' },
    { "help",            no_argument,           nullptr,     'h' },
    { nullptr,           no_argument,           nullptr,      0  }
};
#else
static struct option starchstrip_long_options[] = { 
    { "include",         required_argument,     NULL,     'i' },
    { "exclude",         required_argument,     NULL,     'x' },
    { "version",         no_argument,           NULL,     'v' },
    { "help",            no_argument,           NULL,     'h' },
    { NULL,              no_argument,           NULL,      0  }
};
#endif

static const char *starchstrip_opt_string = "i:x:vh?";

extern const char starchstrip_chromosomes_str_delimiter;
extern const int starchstrip_archive_version_major_minimum;
extern const int starchstrip_archive_version_minor_minimum;
extern const int starchstrip_archive_version_revision_minimum;
extern const size_t starchstrip_copy_buffer_size;

const char starchstrip_chromosomes_str_delimiter = ',';
const int starchstrip_archive_version_major_minimum = 2;
const int starchstrip_archive_version_minor_minimum = 1;
const int starchstrip_archive_version_revision_minimum = 0;
const size_t starchstrip_copy_buffer_size = 65536;

#ifdef __cplusplus
namespace starch {
#endif

static void              STARCHSTRIP_init_globals();
static void              STARCHSTRIP_delete_globals();
static void              STARCHSTRIP_init_command_line_options(int argc, char** argv);
static void              STARCHSTRIP_init_chromosomes_list();
static int               STARCHSTRIP_compare_chromosome_names(const void* a, const void* b);
static void              STARCHSTRIP_debug_chromosomes_to_query_list();
static void              STARCHSTRIP_debug_chromosomes_to_process_list();
static void              STARCHSTRIP_init_archive_metadata();
static void              STARCHSTRIP_check_archive_version();
static void              STARCHSTRIP_check_chromosome_stream_names();
static void              STARCHSTRIP_write_header(FILE* os);
static void              STARCHSTRIP_write_chromosome_streams(FILE* os);
static void              STARCHSTRIP_write_updated_metadata(FILE* os);
static void              STARCHSTRIP_print_version(FILE* os);
static void              STARCHSTRIP_print_usage(FILE* os);
static Boolean           STARCHSTRIP_file_exists(char* fn);

#ifdef __cplusplus
} // namespace starch
#endif

#endif
