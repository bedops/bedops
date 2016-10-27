//=========
// Author:  Alex Reynolds & Shane Neph
// Date:    Wed Nov  3 17:22:50 PDT 2010
// Project: starch
// File:    unstarch.h
//=========

//
//    BEDOPS
//    Copyright (C) 2011-2016 Shane Neph, Scott Kuehn and Alex Reynolds
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
    "USAGE: unstarch [ <chromosome> ]  [ --elements | --bases | --bases-uniq | --has-duplicates | --has-nested | --list | --list-json | --list-chromosomes | --archive-timestamp | --note | --archive-version | --is-starch | --signature | --verify-signature ] <starch-file>\n" \
    "\n" \
    "    Modifiers:\n\n" \
    "    <chromosome>                     Optional. Either unarchives chromosome-specific records from the starch archive file or restricts action of operator to chromosome (e.g., chr1, chrY, etc.).\n\n" \
    "    Process Flags:\n\n" \
    "    --elements                       Show total element count for archive. If <chromosome> is specified, the result shows the element count for the chromosome.\n" \
    "    --bases,\n" \
    "    --bases-uniq                     Show total and unique base counts, respectively, for archive. If <chromosome> is specified, the count is specific to the chromosome, if available.\n" \
    "    --has-duplicate,                 Show whether there is one or more duplicate elements in the specified chromosome, either as a numerical (1/0) or string (true/false) value. If no <chromosome> is specified, the value given indicates if there is one or more duplicate elements across all chromosome records.\n" \
    "    --has-duplicate-as-string \n" \
    "    --has-nested,                    Show whether there is one ore more nested elements in the specified chromosome, either as a numerical (1/0) or string (true/false) value. If no <chromosome> is specified, the value given indicates if there is one or more nested elements across all chromosome records.\n" \
    "    --has-nested-as-string \n" \
    "    --list                           List archive metadata (output is in text format). If chromosome is specified, the attributes of the given chromosome are shown.\n" \
    "    --list-json,                     List archive metadata (output is in JSON format)\n" \
    "    --list-json-no-trailing-newline  \n" \
    "    --list-chr,                      List all or specified chromosome in starch archive (similar to \"bedextract --list-chr\"). If <chromosome> is specified but is not in the output list, nothing is returned.\n" \
    "    --list-chromosomes \n" \
    "    --note                           Show descriptive note, if available.\n" \
    "    --signature                      Display the Base64-encoded SHA-1 data integrity signature for specified <chromosome>, or the signatures of the metadata and all available chromosomes, if the <chromosome> is unspecified.\n" \
    "    --verify-signature               Verify data integrity of specified <chromosome>, or the integrity of all available chromosomes, if the <chromosome> is unspecified.\n" \
    "    --archive-timestamp              Show archive creation timestamp (ISO 8601 format).\n" \
    "    --archive-type                   Show archive compression type.\n" \
    "    --archive-version                Show archive version.\n" \
    "    --is-starch                      Test if <starch-file> is a valid archive and print 0/1 (false/true) to standard output.\n" \
    "    --version                        Show binary version.\n" \
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
