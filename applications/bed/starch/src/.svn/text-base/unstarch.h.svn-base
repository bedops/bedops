//=========
// Author:  Alex Reynolds & Shane Neph
// Date:    Wed Nov  3 17:22:50 PDT 2010
// Project: starch
// File:    unstarch.h
//=========

#ifndef UNSTARCH_H
#define UNSTARCH_H

#include "data/starch/starchMetadataHelpers.h"

static const char *name = "unstarch";
static const char *authors = "Alex Reynolds and Shane Neph";
static const char *usage = "\n" \
    "USAGE: unstarch [ <chromosome> ]  [ --elements | --bases | --bases-uniq | --list | --list-json | --list-chromosomes | --archive-timestamp | --note | --archive-version ] <starch-file>\n" \
    "\n" \
    "    Process Flags:\n\n" \
    "    <chromosome>                     Optional. Either unarchives chromosome-specific records from the starch archive file or restricts action of operator to chromosome (e.g., chr1, chrY, etc.).\n" \
    "    --elements                       Show total element count for archive. If <chromosome> is specified, the result shows the element count for the chromosome.\n" \
    "    --bases,\n" \
    "    --bases-uniq                     Show total and unique base counts, respectively, for archive. If <chromosome> is specified, the count is specific to the chromosome, if available.\n" \
    "    --list                           List archive metadata (output is in text format). If chromosome is specified, the attributes of the given chromosome are shown.\n" \
    "    --list-json,                     List archive metadata (output is in JSON format)\n" \
    "    --list-json-no-trailing-newline  \n" \
    "    --list-chr,                      List all or specified chromosome in starch archive (similar to \"bedextract --list-chr\"). If <chromosome> is specified but is not in the output list, nothing is returned.\n" \
    "    --list-chromosomes \n" \
    "    --note                           Show descriptive note, if available.\n" \
    "    --sha1-signature                 Show SHA1 signature of JSON-formatted metadata (Base64-encoded).\n" \
    "    --archive-timestamp              Show archive creation timestamp (ISO 8601 format).\n" \
    "    --archive-type                   Show archive compression type.\n" \
    "    --archive-version                Show archive version.\n" \
    "    --version                        Show binary version.\n" \
    "    --help                           Show this usage message.\n";

/* 
   On Darwin, file I/O is 64-bit by default (OS X 10.5 at least) so we use standard 
   types and calls 
*/

#ifdef __APPLE__
#define off64_t off_t
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

#endif
