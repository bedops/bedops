//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starch.h
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

#ifndef STARCH_H
#define STARCH_H

#include <getopt.h>
#include <inttypes.h>
#include <errno.h>

#include "data/starch/starchMetadataHelpers.h"

#ifdef __cplusplus
namespace {
  using namespace starch;
} // unnamed namespace
#endif

static const char *name = "starch";
static const char *authors = "Alex Reynolds and Shane Neph";
static const char *usage = "\n" \
    "USAGE: starch [ --note=\"foo bar...\" ]\n" \
    "              [ --bzip2 | --gzip ]\n" \
    "              [ --omit-signature ]\n" \
    "              [ --report-progress=N ]\n" \
    "              [ --header ] [ <unique-tag> ] <bed-file>\n" \
    "    \n" \
    "    * BED input must be sorted lexicographically (e.g., using BEDOPS sort-bed).\n" \
    "    * Please use '-' to indicate reading BED data from standard input.\n" \
    "    * Output must be directed to a regular file.\n" \
    "    * The bzip2 compression type makes smaller archives, while gzip extracts\n" \
    "      faster.\n" \
    "    \n" \
    "    Process Flags\n" \
    "    --------------------------------------------------------------------------\n" \
    "    --note=\"foo bar...\"   Append note to output archive metadata (optional).\n\n" \
    "    --bzip2 | --gzip      Specify backend compression type (optional, default\n" \
    "                          is bzip2).\n\n" \
    "    --omit-signature      Skip generating per-chromosome data integrity signature\n" \
    "                          (optional, default is to generate signature).\n\n" \
    "    --report-progress=N   Report compression progress every N elements per\n" \
    "                          chromosome to standard error stream (optional)\n\n" \
    "    --header              Support BED input with custom UCSC track, SAM or VCF\n" \
    "                          headers, or generic comments (optional).\n\n" \
    "    <unique-tag>          Optional. Specify unique identifier for transformed\n" \
    "                          data.\n\n" \
    "    --version             Show binary version.\n\n" \
    "    --help                Show this usage message.\n";

static struct starch_client_global_args_t {
    char *note;
    CompressionType compressionType;
    Boolean generatePerChromosomeSignatureFlag;
    Boolean reportProgressFlag;
    LineCountType reportProgressN;
    Boolean headerFlag;
    char *inputFile;
    char *uniqueTag;
    char *tag;
    char **inputFiles;
    size_t numberInputFiles;
} starch_client_global_args;

#ifdef __cplusplus
static struct option starch_client_long_options[] = {    
    {"note",            required_argument,    nullptr, 'n'},
    {"bzip2",           no_argument,          nullptr, 'b'},
    {"gzip",            no_argument,          nullptr, 'g'},
    {"omit-signature",  no_argument,          nullptr, 'o'},
    {"report-progress", required_argument,    nullptr, 'r'},
    {"header",          no_argument,          nullptr, 'e'},
    {"version",         no_argument,          nullptr, 'v'},
    {"help",            no_argument,          nullptr, 'h'},
    {nullptr,           no_argument,          nullptr,  0 }
};
#else
static struct option starch_client_long_options[] = {    
    {"note",            required_argument,    NULL, 'n'},
    {"bzip2",           no_argument,          NULL, 'b'},
    {"gzip",            no_argument,          NULL, 'g'},
    {"omit-signature",  no_argument,          NULL, 'o'},
    {"report-progress", required_argument,    NULL, 'r'},
    {"header",          no_argument,          NULL, 'e'},
    {"version",         no_argument,          NULL, 'v'},
    {"help",            no_argument,          NULL, 'h'},
    {NULL,              no_argument,          NULL,  0 }
};
#endif

static const char *starch_client_opt_string = "n:bgorevh?";

#ifdef __cplusplus
namespace starch {
#endif

void          STARCH_initializeGlobals();

int           STARCH_parseCommandLineOptions(int argc, 
                                            char **argv);

void          STARCH_printUsage(int t);

void          STARCH_printRevision();

#ifdef __cplusplus
} // namespace starch
#endif

#endif
