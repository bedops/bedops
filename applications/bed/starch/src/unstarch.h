//=========
// Author:  Alex Reynolds & Shane Neph
// Date:    Wed Nov  3 17:22:50 PDT 2010
// Project: starch
// File:    unstarch.h
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

#ifndef UNSTARCH_H
#define UNSTARCH_H

#include "data/starch/starchMetadataHelpers.h"

static const char *name = "unstarch";
static const char *authors = "Alex Reynolds and Shane Neph";
static const char *usage = "\n" \
    "USAGE: unstarch [ <chromosome> ]  [ --elements | \n" \
    "                                    --elements-max-string-length |\n" \
    "                                    --bases | --bases-uniq |\n" \
    "                                    --has-duplicates | --has-nested | --list |\n" \
    "                                    --list-json | --list-chromosomes |\n" \
    "                                    --archive-timestamp | --note |\n" \
    "                                    --archive-version | --is-starch |\n" \
    "                                    --signature | --verify-signature ]\n" \
    "                                    <starch-file>\n" \
    "\n" \
    "    Modifiers\n" \
    "    --------------------------------------------------------------------------\n" \
    "    <chromosome>                     Optional. Either unarchives chromosome-\n" \
    "                                     specific records from the starch archive\n" \
    "                                     file or restricts action of operator to\n" \
    "                                     chromosome (e.g., chr1, chrY, etc.).\n\n" \
    "    Process Flags\n" \
    "    --------------------------------------------------------------------------\n" \
    "    --elements                       Show total element count for archive. If\n" \
    "                                     <chromosome> is specified, the result\n" \
    "                                     shows the element count for the\n" \
    "                                     chromosome.\n\n" \
    "    --elements-max-string-length     Show the maximum string length over all\n" \
    "                                     elements in <chromosome>, if specified.\n" \
    "                                     If <chromosome> is not specified, the\n" \
    "                                     maximum string length is shown over all\n" \
    "                                     chromosomes.\n\n" \
    "    --bases,\n" \
    "    --bases-uniq                     Show total and unique base counts,\n" \
    "                                     respectively, for archive. If\n" \
    "                                     <chromosome> is specified, the count is\n"
    "                                     specific to the chromosome, if available.\n\n" \
    "    --has-duplicate-as-string, \n" \
    "    --has-duplicate                  Show whether there is one or more\n" \
    "                                     duplicate elements in the specified\n" \
    "                                     chromosome, either as a numerical (1/0)\n" \
    "                                     or string (true/false) value. If no\n" \
    "                                     <chromosome> is specified, the value\n" \
    "                                     given indicates if there is one or more\n" \
    "                                     duplicate elements across all chromosome\n" \
    "                                     records.\n\n" \
    "    --has-nested-as-string, \n" \
    "    --has-nested                     Show whether there is one ore more nested\n" \
    "                                     elements in the specified chromosome,\n" \
    "                                     either as a numerical (1/0) or string\n" \
    "                                     (true/false) value. If no <chromosome> is\n" \
    "                                     specified, the value given indicates if\n" \
    "                                     there is one or more nested elements\n" \
    "                                     across all chromosome records.\n\n" \
    "    --list                           List archive metadata (output is in text\n" \
    "                                     format). If chromosome is specified, the\n" \
    "                                     attributes of the given chromosome are\n" \
    "                                     shown.\n\n" \
    "    --list-json, \n" \
    "    --list-json-no-trailing-newline  List archive metadata (output is in JSON\n" \
    "                                     format)\n\n" \
    "    --list-chr,                      \n" \
    "    --list-chromosomes               List all or specified chromosome in\n" \
    "                                     starch archive (like \"bedextract --list-\n" \
    "                                     chr\"). If <chromosome> is specified but\n" \
    "                                     is not in the output list, nothing is\n" \
    "                                     returned.\n\n" \
    "    --note                           Show descriptive note, if available.\n\n" \
    "    --signature                      Display the Base64-encoded SHA-1 data\n" \
    "                                     integrity signature for specified\n" \
    "                                     <chromosome>, or the signatures of the\n" \
    "                                     metadata and all available chromosomes,\n" \
    "                                     if the <chromosome> is unspecified.\n\n" \
    "    --verify-signature               Verify data integrity of specified\n" \
    "                                     <chromosome>, or the integrity of all\n" \
    "                                     available chromosomes, if the\n" \
    "                                     <chromosome> is unspecified.\n\n" \
    "    --archive-timestamp              Show archive creation timestamp (ISO 8601\n" \
    "                                     format).\n\n" \
    "    --archive-type                   Show archive compression type.\n\n" \
    "    --archive-version                Show archive version.\n\n" \
    "    --is-starch                      Test if <starch-file> is a valid archive\n" \
    "                                     and print 0/1 (false/true) to standard\n" \
    "                                     output. Unstarch will also return a non-\n" \
    "                                     zero error code if the input file is not\n" \
    "                                     a valid archive.\n\n" \
    "    --version                        Show binary version.\n\n" \
    "    --help                           Show this usage message.\n";

#ifdef __cplusplus
namespace starch {
#endif

int                  UNSTARCH_parseCommandLineInputs(int argc,
                                                    char **argv,
                                                    char **chr,
                                                    char **fn,
                                                    char **optn,
                                                     int *pval);
void                 UNSTARCH_printUsage(int t);

void                 UNSTARCH_printRevision();

void                 UNSTARCH_printArchiveVersion(const ArchiveVersion *av);

void                 UNSTARCH_printArchiveTimestamp(const char *ct);

void                 UNSTARCH_printNote(const char *note);

void                 UNSTARCH_printCompressionType(const CompressionType t);

void                 UNSTARCH_printMetadataSha1Signature(unsigned char *sha1Buffer);

#ifdef __cplusplus
} // namespace starch
#endif

#endif
