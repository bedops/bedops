/*
  FILE: sort.c
  AUTHOR: Scott Kuehn
    MODS: Shane Neph
  CREATE DATE: Thu Sep  7 08:48:35 PDT 2006
  ID: $Id: sort.c,v 1.6 2010/08/20 05:05:32 sjn Exp $
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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

#include "Structures.hpp"
#include "suite/BEDOPS.Version.hpp"

using namespace std;

#define MAX_INFILES 10000

static const char *name = "sort-bed";
static const char *authors = "Scott Kuehn & Shane Neph";
static const char *usage = "\nUSAGE: sort-bed [--help] [--version] [--max-mem <val>] <file1.bed> <file2.bed> <...>\n        Sort BED file(s).\n        May use '-' to indicate stdin.\n        Results are sent to stdout.\n\n        <val> for --max-mem may be 8G, 8000M, or 8000000000 to specify 8 GB of memory, for example.\n\n";


static void 
getArgs(int argc, char **argv, const char **inFiles, unsigned int *numInFiles, double* maxMem)
{
    int numFiles, i, j, stdincnt = 0, changeMem = 0, units = 0;
    size_t k;
    size_t lng = 0U;
    double factor = 1;
    char *tmp;
    numFiles = argc - 1;
    if(numFiles < 1)
        {
            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n",
                    name, BEDOPS::citation(), BEDOPS::revision(), authors, usage);
            exit(EXIT_FAILURE);
        }
    else if (numFiles > MAX_INFILES)
        {
            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\nToo Many Files\n",
                    name, BEDOPS::citation(), BEDOPS::revision(), authors, usage);
            exit(EXIT_FAILURE);
        }
    else
        {
            for(i = 1, j = 0; i < argc; i++, j++)
                {
                    // Check for --help
                    if(strcmp(argv[i], "--help") == 0) 
                        {
                            fprintf(stdout, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n",
                                    name, BEDOPS::citation(), BEDOPS::revision(), authors, usage);
                            exit(EXIT_SUCCESS);
                        }
                    else if (strcmp(argv[i], "--version") == 0)
                        {
                            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n",
                                    name, BEDOPS::citation(), BEDOPS::revision(), authors);
                            exit(EXIT_SUCCESS);
                        }
                    // Check for max memory before merge-sort
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
                                            if( k != lng-1 || 0 == k) /* bad number? just G? M? */
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
                                            tmp = (char*)malloc(lng + 1);
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
                    // Check for stdin
                    else if(strcmp(argv[i], "-") == 0) 
                        {
                            stdincnt++;
                        }
                    inFiles[j] = argv[i];
                    (*numInFiles)++;
                }
        }

    if(stdincnt > 1)
        {
            fprintf(stderr, "Cannot specify '-' more than once\n");
            exit(EXIT_FAILURE);
        }
    else if(numFiles < 1) /* can be different from before if --max-mem was used */
        {
            fprintf(stderr, "%s\n  citation: %s\n  version:  %s\n  authors:  %s\n%s\n%s\n",
                    name, BEDOPS::citation(), BEDOPS::revision(), authors, usage, "No file given.");
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
    getArgs(argc, argv, inFiles, &numInFiles, &maxMemory);

    if(0 == processData(inFiles, numInFiles, maxMemory))
        return(EXIT_SUCCESS);
    return(EXIT_FAILURE);
}
