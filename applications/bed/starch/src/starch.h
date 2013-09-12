//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starch.h
//=========


#ifndef STARCH_H
#define STARCH_H

#include <getopt.h>

#include "data/starch/starchMetadataHelpers.h"

static const char *name = "starch";
static const char *authors = "Alex Reynolds and Shane Neph";
static const char *usage = "\n" \
    "USAGE: starch [--note=\"foo bar...\"] [--bzip2 | --gzip] [--header] [<unique-tag>] <bed-file>\n" \
    "    \n" \
    "    * BED input must be sorted lexicographically (e.g., using BEDOPS sort-bed).\n" \
    "    * Please use '-' to indicate reading BED data from standard input.\n" \
    "    * Output must be directed to a regular file.\n" \
    "    * The bzip2 compression type makes smaller archives, while gzip extracts faster.\n" \
    "    \n" \
    "    Process Flags:\n\n" \
    "    --note=\"foo bar...\"   Append note to output archive metadata (optional)\n" \
    "    --bzip2 | --gzip      Specify backend compression type (optional, default is bzip2)\n" \
    "    --header              Support BED input with custom UCSC track, SAM or VCF headers, or generic comments (optional)\n" \
    "    <unique-tag>          Specify unique identifier for transformed data (optional)\n" \
    "    --help                Show this usage message\n" \
    "    --version             Show binary version";

struct starch_client_global_args_t {
    char *note;
    CompressionType compressionType;
    Boolean headerFlag;
    char *inputFile;
    char *uniqueTag;
    char *tag;
    char **inputFiles;
    size_t numberInputFiles;
} starch_client_global_args;

static struct option starch_client_long_options[] = {    
    {"note",    required_argument, NULL, 'n'},
    {"bzip2",   no_argument,       NULL, 'b'},
    {"gzip",    no_argument,       NULL, 'g'},
    {"header",  no_argument,       NULL, 'e'},
    {"version", no_argument,       NULL, 'v'},
    {"help",    no_argument,       NULL, 'h'},
    {NULL,      no_argument,       NULL, 0}
};

static const char *starch_client_opt_string = "n:bgevh?";

/* 
   On Darwin, file I/O is 64-bit by default (OS X 10.5 at least) so we use standard 
   types and calls 
*/

#ifdef __APPLE__
#define off64_t off_t
#endif

void          STARCH_initializeGlobals();

int           STARCH_parseCommandLineOptions(int argc, 
                                            char **argv);

void          STARCH_printUsage(int t);

void          STARCH_printRevision();

#endif
