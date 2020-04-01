/*
  Author: Scott Kuehn
    Mods: Shane Neph
    Date: Thu Sep  7 08:48:35 PDT 2006
*/
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

#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/stat.h>

#include "Structures.hpp"
#include "suite/BEDOPS.Version.hpp"

using namespace std;

#define MAX_INFILES 10000

static const char *name = "sort-bed";
static const char *authors = "Scott Kuehn";
static const char *usage = "\nUSAGE: sort-bed [--help] [--version] [--check-sort] [--max-mem <val>] [--tmpdir <path>] [--unique] [--duplicates] <file1.bed> <file2.bed> <...>\n        Sort BED file(s).\n        May use '-' to indicate stdin.\n        Results are sent to stdout.\n\n        <val> for --max-mem may be 8G, 8000M, or 8000000000 to specify 8 GB of memory.\n        --tmpdir is useful only with --max-mem.\n        --unique can be used to print only unique BED elements (similar to 'sort -u'). Cannot be used with --duplicates.\n        --duplicates can be used to print only duplicated or repeated elements (similar to 'uniq -d'). Cannot be used with --unique.\n";

static void
getArgs(int argc, char **argv, const char **inFiles, unsigned int *numInFiles, int *justCheck, double* maxMem, char **tmpPath, bool *printUniques, bool *printDuplicates)
{
    int numFiles, i, j, stdincnt = 0, changeMem = 0, units = 0, changeTDir = 0;
    size_t k;
    size_t lng = 0U;
    double factor = 1;
    char *tmp;
    numFiles = argc - 1;
    if(numFiles < 1)
        {
            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n",
                    name, BEDOPS::citation(), BEDOPS::version(), authors, usage);
            exit(EXIT_FAILURE);
        }
    else if (numFiles > MAX_INFILES)
        {
            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\nToo Many Files\n",
                    name, BEDOPS::citation(), BEDOPS::version(), authors, usage);
            exit(EXIT_FAILURE);
        }
    else
        {
            for(i = 1, j = 0; i < argc; i++, j++)
                {
                    /* Check for --help */
                    if(strcmp(argv[i], "--help") == 0) 
                        {
                            fprintf(stdout, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n",
                                    name, BEDOPS::citation(), BEDOPS::version(), authors, usage);
                            exit(EXIT_SUCCESS);
                        }
                    else if (strcmp(argv[i], "--version") == 0)
                        {
                            fprintf(stdout, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n",
                                    name, BEDOPS::citation(), BEDOPS::version(), authors);
                            exit(EXIT_SUCCESS);
                        }
                    /* Check for max memory before merge-sort */
                    else if(strcmp(argv[i], "--max-mem") == 0)
                        {
                            units = 0;
                            factor = 1;
                            if(changeMem != 0)
                                {
                                    fprintf(stderr, "Specify --max-mem at most one time!\n");
                                    exit(EXIT_FAILURE);
                                }
                            changeMem = 1;
                            if(++i == argc)
                                {
                                    fprintf(stderr, "No value given for --max-mem.\n");
                                    exit(EXIT_FAILURE);
                                }

                            lng = strlen(argv[i]);
                            for(k=0; k < lng; ++k)
                                {
                                    if(!isdigit(argv[i][k]))
                                        {
                                            if(0 == k ||  k != lng-1) /* bad number? just G? M? */
                                                {
                                                    fprintf(stderr, "Bad number for --max-mem.  Expect value to be like 10G (for 10 gigabytes) or 1000M (for 1000 megabytes) or just 1000000000 (for 1 gigabyte).\n");
                                                    exit(EXIT_FAILURE);
                                                }
                                            if(argv[i][k] == 'G')
                                                factor = 1000000000, --lng;
                                            else if(argv[i][k] == 'M')
                                                factor = 1000000, --lng;
                                            else
                                                {
                                                    fprintf(stderr, "Unrecognized units for --max-mem.  Expect value to be like 10G (for 10 gigabytes) or 1000M (for 1000 megabytes) or just 1000000000 (for 1 gigabyte).\n");
                                                    exit(EXIT_FAILURE);
                                                }

                                            units = 1;
                                            tmp = static_cast<char*>( malloc(lng + 1) );
                                            strncpy(tmp, argv[i], lng);
                                            tmp[lng] = '\0';
                                            *maxMem = factor * strtod(tmp, NULL);
                                            free(tmp);
                                        }
                                } /* for */
                            if(!units)
                                {
                                    *maxMem = strtod(argv[i], NULL);
                                }
                            if(*maxMem > 128000000000.0)
                                {
                                    fprintf(stderr, "\nSetting memory > 128 GB probably isn't practical.\nIf you remove --max-mem, the program will use up to all available system memory.\nContinuing.\n\n");
                                    /* just going to send a warning exit(EXIT_FAILURE); */
                                }
                            if(*maxMem < 500000000.0)
                                {
                                    fprintf(stderr, "While theoretically possible to sort with less memory, we expect at least 500 megabytes for --max-mem\n");
                                    exit(EXIT_FAILURE);
                                }
                            --j;
                            numFiles -= 2;
                            continue;
                        }
                    else if(strcmp(argv[i], "--tmpdir") == 0)
                        {
                            if(changeTDir != 0)
                                {
                                    fprintf(stderr, "Specify --tmpdir at most one time!\n");
                                    exit(EXIT_FAILURE);
                                }
                            changeTDir = 1;
                            if(++i == argc)
                                {
                                    fprintf(stderr, "No value given for --tmpdir.\n");
                                    exit(EXIT_FAILURE);
                                }
                            *tmpPath = static_cast<char*>( malloc(strlen(argv[i])+1) );
                            strcpy(*tmpPath, argv[i]);
                            --j;
                            numFiles -= 2;
                            continue;
                        }
                    else if(strcmp(argv[i], "--check-sort") == 0)
                        {
                            *justCheck = 1;
                            --j;
                            numFiles -= 1;
                            continue;
                        }
                    else if((strcmp(argv[i], "--unique") == 0) || (strcmp(argv[i], "-u") == 0))
                        {
                            *printUniques = true;
                            --j;
                            numFiles -= 1;
                            continue;
                        }
                    else if((strcmp(argv[i], "--duplicates") == 0) || (strcmp(argv[i], "-d") == 0))
                        {
                            *printDuplicates = true;
                            --j;
                            numFiles -= 1;
                            continue;
                        }
                    else if(strcmp(argv[i], "-") == 0) /* stdin */
                        {
                            stdincnt++;
                        }
                    inFiles[j] = argv[i];
                    (*numInFiles)++;
                } /* for */
        }

    if(stdincnt > 1)
        {
            fprintf(stderr, "Cannot specify '-' more than once\n");
            exit(EXIT_FAILURE);
        }
    else if((numFiles < 1) || (*printUniques && *printDuplicates)) /* can be different from before if --max-mem was used, for example*/
        {
            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n%s\n",
                    name, BEDOPS::citation(), BEDOPS::version(), authors, usage, "No file given.");
            exit(EXIT_FAILURE);
        }
    return;
}

int
main(int argc, char **argv)
{
    unsigned int numInFiles = 0U;
    double maxMemory = -1;
    const char *inFiles[MAX_INFILES];
    char* tmpPath = NULL;
    bool clean = false;
    int justCheck = 0;
    int rval = EXIT_FAILURE;
    bool printUniques = false;
    bool printDuplicates = false;

    getArgs(argc, argv, inFiles, &numInFiles, &justCheck, &maxMemory, &tmpPath, &printUniques, &printDuplicates);
    if(justCheck) /* just checking inputs */
        rval = checkSort(inFiles, numInFiles);
    else /* sorting */
        {
            if(tmpPath != NULL)
                {
                    if(maxMemory > 0)
                        {
                            clean = true;
                        }
                    else /* tmpPath only useful when --max-mem is used */
                        {
                            free(tmpPath);
                            tmpPath = NULL;
                        }
                }
            else if (maxMemory > 0)
                {
                    tmpPath = getenv("TMPDIR");
                }

            // sort
            rval = processData(inFiles, numInFiles, maxMemory, tmpPath, printUniques, printDuplicates);

            if(clean)
                free(tmpPath);
        }

    return rval;
}
