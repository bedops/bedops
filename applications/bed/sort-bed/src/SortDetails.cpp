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

#include <algorithm>
#include <cinttypes>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include <map>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include "suite/BEDOPS.Constants.hpp"

#include "Structures.hpp"

using namespace std;

int
mergeSort(FILE* output, FILE **tmpFiles, unsigned int numFiles);

FILE *
createTmpFile(char const* path, char** fileName);

void
freeTmpFiles(unsigned int fcount, FILE **tmpFiles, char **tmpFileNames);

int
createDir(char* dir);

// probably linux-specific.  From
//   http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
// just used them to help debug my overestimates of internal memory allocated
namespace dbug_help
{
    int parseLine(char* line);
    int getVirtMemValue();
    int getResMemValue();

    int
    parseLine(char* line)
    {
        int i = static_cast<int>( strlen(line) ); // perhaps better to cast strlen to an int, in order to fulfill parseline contract
        while (*line < '0' || *line > '9') line++;
        line[i-3] = '\0';
        i = atoi(line);
        return i;
    }
    
    int
    getVirtMemValue()
    { //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];
    
        while (fgets(line, 128, file) != NULL)
            {
                if (strncmp(line, "VmSize:", 7) == 0)
                    {
                        result = parseLine(line);
                        break;
                    }
            }
        fclose(file);
        return result;
    }
    
    int getResMemValue()
    { //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];
    
        while (fgets(line, 128, file) != NULL)
            {
                if (strncmp(line, "VmRSS:", 6) == 0)
                    {
                        result = parseLine(line);
                        break;
                    }
            } /* while */
        fclose(file);
        return result;
    }
} // end namespace dbug_help


FILE *
createTmpFile(char const* path, char** fileName)
{
    FILE* fp;
    int fd;
    char* tmpl;

    if (path == NULL)
        {
            fileName = NULL;
            return tmpfile();
        }

    tmpl = static_cast<char*>( malloc(1 + strlen(path) + L_tmpnam) );
    strcpy(tmpl, path);
    strcpy(tmpl+strlen(path), "/sb.XXXXXX");
    fd = mkstemp(tmpl);
    if(fd == -1)
        {
            fprintf(stderr, "unable to create temp file!\n");
            return NULL;
        }
    fp = fdopen(fd, "wb+");
    *fileName = static_cast<char*>( malloc(strlen(tmpl) + 1) );
    strcpy(*fileName, tmpl);
    free(tmpl);
    return fp;
}

int
createDir(char* dir)
{
    /* could use boost filesystem, but it requires compilation, and
         likely another dependency - jam.
    */
    struct stat mystat;
    bool ok = true;
    ifstream ifcheck(dir);
    bool makeit = !ifcheck.good();
    FILE *fptr = NULL;
    char *fname = NULL;

    if (ifcheck.good()) /* already exists */
        {
            if (stat(dir, &mystat) == 0)
                {
                    if (S_ISDIR(mystat.st_mode))
                        {
                            makeit = false;
                            /* make sure it's writable */
                            fptr = createTmpFile(dir, &fname);
                            if (fptr == NULL)
                                {
                                    fprintf(stderr, "Unable to create a file in existing directory: %s\n.Check permissions.\n", dir);
                                    ok = false;
                                }
                            else
                                {
                                    remove(fname);
                                }
                        }
                    else
                        {
                            fprintf(stderr, "This already exists, but it is not a directory: %s\n", dir);
                            ok = false;
                        }
                }
            else
                {
                    fprintf(stderr, "(odd) Trouble finding: %s\n", dir);
                    ok = false;
                }
        }

    if (!ok)
        return EXIT_FAILURE;
    else if (makeit)
        {
            if (mkdir(dir, 0700) != 0)
               {
                  fprintf(stderr, "Unknown problem creating directory.  Check permissions.  %s\n", dir);
                  return EXIT_FAILURE;
               }
        }
    return EXIT_SUCCESS;
}

BedData * 
initializeBedData(double *bytes) 
{

    BedData * beds;

    beds = static_cast<BedData*>( malloc(sizeof(BedData)) );
    *bytes += sizeof(BedData);

    if (beds == NULL) 
        {
            return NULL;
        }
    beds->numChroms = 0;
    beds->chroms = static_cast<ChromBedData**>( malloc(sizeof(ChromBedData *) * (NUM_CHROM_EST)) );
    *bytes += (sizeof(ChromBedData*) * NUM_CHROM_EST);
    if (beds->chroms == NULL) 
        {
            return NULL;
        }

    return beds;
}


ChromBedData * 
initializeChromBedData(char *chromBuf, double *bytes) {
    ChromBedData *chrom;
    size_t chromBufLen;

    if(chromBuf == NULL) 
        {
            return NULL;
        }

    chrom = static_cast<ChromBedData*>( malloc(sizeof(ChromBedData)) );
    *bytes += sizeof(ChromBedData);
    if(chrom == NULL) 
        {
            return NULL;
        }

    /* Coords */
    chrom->coords = NULL;
  
    /* Chrom name*/
    chromBufLen = strlen(chromBuf); // we know >= 1
    strncpy(chrom->chromName, chromBuf, chromBufLen);
    chrom->chromName[chromBufLen] = '\0';
    chrom->numCoords = 0;
    return chrom;
}



Bed::SignedCoordType
appendChromBedEntry(ChromBedData *chrom, Bed::SignedCoordType startPos, Bed::SignedCoordType endPos,
                    char *data, double *bytes, double maxMem)
{

    /* can realloc a lot of data in this function
         need to be mindful of whether we could dynamically get to maxMem.
       in particular, might get to maxMem upon reallocation and then drop back below it.
         in such a case, set *bytes to maxMem (when applicable).
    */
    Bed::LineCountType index;
    size_t dataBufLen, newSize;
    char *dataPtr;

    if(chrom == NULL)
        {
            fprintf(stderr, "Error: %s, %d: Bad 'chrom' variable.\n", __FILE__, __LINE__);
            return static_cast<Bed::SignedCoordType>(-1);
        }
  
    index = chrom->numCoords;

    // requires INIT_NUM_BED_ITEMS_EST < NUM_BED_ITEMS_EST
    if (index == 0)
        { // first initialization
            chrom->coords = static_cast<BedCoordData*>( malloc(sizeof(BedCoordData) * INIT_NUM_BED_ITEMS_EST) );
            if(chrom->coords == NULL)
                {
                    fprintf(stderr, "Error: %s, %d: Unable to create BedCoordData structure. Out of memory.\n", __FILE__, __LINE__);
                    return static_cast<Bed::SignedCoordType>(-1);
                }
            *bytes += (sizeof(BedCoordData) * INIT_NUM_BED_ITEMS_EST);
        }
    else if (index == (INIT_NUM_BED_ITEMS_EST-1))
        {
            newSize = sizeof(BedCoordData) * static_cast<size_t>(NUM_BED_ITEMS_EST);
            chrom->coords = static_cast<BedCoordData*>( realloc(chrom->coords, newSize) );
            if(chrom->coords == NULL)
                {
                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                    return static_cast<Bed::SignedCoordType>(-1);
                }
            if ((maxMem > 0) && (*bytes + newSize >= maxMem))
                { // may have temporarily reach maxMem or more
                    *bytes = maxMem;
                }
            else
                {
                    *bytes += (sizeof(BedCoordData) * NUM_BED_ITEMS_EST);
                    *bytes -= (sizeof(BedCoordData) * INIT_NUM_BED_ITEMS_EST);
                }
        }
    else if((index % (NUM_BED_ITEMS_EST-1)) == 0)
        {
            //fprintf(stderr, "Reallocating...\n");
            newSize = sizeof(BedCoordData) * static_cast<size_t>(index + NUM_BED_ITEMS_EST);
            chrom->coords = static_cast<BedCoordData*>( realloc(chrom->coords, newSize) );
            if(chrom->coords == NULL)
                {
                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                    return static_cast<Bed::SignedCoordType>(-1);
                }
            if ((maxMem > 0) && (*bytes + newSize >= maxMem))
                { // may have temporarily reach maxMem or more
                    *bytes = maxMem;
                }
            else
                { // only increasing by NUM_BED_ITEMS_EST since last time around
                    *bytes += (sizeof(BedCoordData) * NUM_BED_ITEMS_EST);
                }
        }

    /* Coords */
    chrom->coords[index].startCoord = startPos;
    chrom->coords[index].endCoord = endPos;
  
    /* Copy in data */
    if(data)
        {
            dataBufLen = strlen(data);
            if(dataBufLen <= 0)
                {
                    fprintf(stderr, "Error: %s, %d: Bad 'data' variable.\n", __FILE__, __LINE__);
                    return static_cast<Bed::SignedCoordType>(-1);
                }
            dataPtr = static_cast<char*>( calloc(dataBufLen + 1, sizeof(char)) );
            *bytes += dataBufLen + 1;
            if(dataPtr == NULL) 
                {
                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                    return static_cast<Bed::SignedCoordType>(-1);
                }
            chrom->coords[index].data = strncpy(dataPtr, data, dataBufLen + 1);
            chrom->coords[index].data[dataBufLen] = '\0';
        }
    else
        chrom->coords[index].data = NULL;

    return static_cast<Bed::SignedCoordType>(++chrom->numCoords);
}

int
checkFiles(const char **bedFileNames, unsigned int numFiles)
{
    FILE* bedFile;
    int notStdin = 0, stdinCount = 0;
    unsigned int i = 0U;

    for(i = 0; i < numFiles; i++) 
        {
            notStdin = strcmp(bedFileNames[i], "-");
            if(notStdin) 
                {
                    bedFile = fopen(bedFileNames[i], "r");
                    if (!bedFile)
                        {
                            fprintf(stderr, "Unable to access %s\n", bedFileNames[i]);
                            return -1;
                        }
                    fclose(bedFile);
                }
            else if (++stdinCount > 1)
                {
                    fprintf(stderr, "stdin specified multiple times\n");
                    return -1;
                }
        } /* for */
    return 0;
}

int
mergeSort(FILE* output, FILE **tmpFiles, unsigned int numFiles)
{
    /* error checking in processData() has already been performed, headers and empty rows removed, etc. */
    unsigned int i = 0U;
    int done = 0, currMin = 0, val = 0;
    char* currRest = NULL;
    char **chroms = static_cast<char**>( malloc(sizeof(char*) * static_cast<size_t>(numFiles)) );
 
   if(chroms == NULL)
        return -1;

    Bed::SignedCoordType *starts = static_cast<Bed::SignedCoordType*>( malloc(sizeof(Bed::SignedCoordType) * static_cast<size_t>(numFiles)) );
    if(starts == NULL)
        return -1;

    Bed::SignedCoordType *ends = static_cast<Bed::SignedCoordType*>( malloc(sizeof(Bed::SignedCoordType) * static_cast<size_t>(numFiles)) );
    if(ends == NULL)
        return -1;

    int *fields = static_cast<int*>( malloc(sizeof(int) * static_cast<size_t>(numFiles)) );
    if(fields == NULL)
        return -1;

    char **rests = static_cast<char**>( malloc(sizeof(char*) * static_cast<size_t>(numFiles)) );
    if(rests == NULL)
        return -1;

    for(i=0; i < numFiles; ++i) {
        fseek(tmpFiles[i], 0, SEEK_SET);
        chroms[i] = static_cast<char*>( malloc(sizeof(char) * (CHROM_NAME_LEN+1)) );
        if(chroms[i] == NULL)
            return -1;
        rests[i] = static_cast<char*>( malloc(sizeof(char) * (BED_LINE_LEN+1)) );
        if(rests[i] == NULL)
            return -1;
        rests[i][0] = '\0';
    } /* for */

    for(i=0; i < numFiles; ++i)
        {
            fields[i] = fscanf(tmpFiles[i], "%s\t%" SCNd64 "\t%" SCNd64 "%[^\n]s\n",
                               chroms[i], &starts[i], &ends[i], rests[i]);
            if(fields[i] == EOF)
                {
                    starts[i] = -1;
                    ends[i] = -1;
                    fields[i] = 0;
                }
        } /* for */

    /* find minimum, over and over again */
    while(!done)
        {
            currMin = -1;
            currRest = NULL;
            for(i=0; i < numFiles; ++i)
                {
                    if(starts[i]>=0)
                        {
                            if(currMin<0 || (val = strcmp(chroms[i], chroms[currMin]))<0)
                                currMin = static_cast<int>(i), currRest = rests[i];
                            else if(0 == val)
                                {
                                    if(starts[i] < starts[currMin])
                                        currMin = static_cast<int>(i), currRest = rests[i];
                                    else if(starts[i] == starts[currMin] && ends[i] < ends[currMin])
                                        currMin = static_cast<int>(i), currRest = rests[i];
                                    else if(starts[i] == starts[currMin] && ends[i] == ends[currMin])
                                        {
                                            if (currRest == NULL)
                                                {
                                                    currMin = static_cast<int>(i);
                                                    currRest = rests[i];
                                                }
                                            else if (strcmp(rests[i], currRest) < 0)
                                                {
                                                    currMin = static_cast<int>(i);
                                                    currRest = rests[i];
                                                }
                                        }
                                }
                        }
                } /* for */
            if(currMin < 0)
                break;

            if(3 == fields[currMin])
                fprintf(output, "%s\t%" PRId64 "\t%" PRId64 "\n", chroms[currMin],
                        starts[currMin], ends[currMin]);
            else
                fprintf(output, "%s\t%" PRId64 "\t%" PRId64 "%s\n", chroms[currMin],
                        starts[currMin], ends[currMin], rests[currMin]);

            rests[currMin][0] = '\0';
            fields[currMin] = fscanf(tmpFiles[currMin], "%s\t%" SCNd64 "\t%" SCNd64 "%[^\n]s\n",
                                     chroms[currMin], &starts[currMin],
                                     &ends[currMin], rests[currMin]);
            if(fields[currMin] == EOF)
                {
                    starts[currMin] = -1;
                    ends[currMin] = -1;
                    fields[currMin] = 0;
                }
        } /* while */

    for(i=0; i < numFiles; ++i)
        {
            free(chroms[i]);
            free(rests[i]);
        }
    free(chroms);
    free(starts);
    free(ends);
    free(rests);
    free(fields);

    return 0;
}

void
freeTmpFiles(unsigned int fcount, FILE **tmpFiles, char **tmpFileNames) {
    if (fcount)
        {
            unsigned int i = 0;
            while (i < fcount)
                {
                    /* tmpFileNames[i] will be NULL if temp file was created through C's tmpfile() */
                    fclose(tmpFiles[i]);
                    if ( tmpFileNames[i] != NULL )
                        {
                            remove(tmpFileNames[i]);
                            free(tmpFileNames[i]);
                        }
                    ++i;
                } /* while */
            free(tmpFileNames);
            free(tmpFiles);
        }
}

int
processData(char const **bedFileNames, unsigned int numFiles, const double maxMem, char *tmpPath, const bool printUniques, const bool printDuplicates)
{
    /* maxMem will be ignored if <= 0 */
    /* function does not do a great job of cleaning up memory on failure (including user input problems).
         But, failure leads to quick program termination and cleanup by the OS. */

    FILE *bedFile = NULL;
    Bed::SignedCoordType chromEntryCount;
    int notStdin = 0,
        fields = 0,
        headCheck = 1,
        val = 0;
    unsigned int iidx, jidx, kidx, tidx, newChrom;
    unsigned int tmpFileCount = 0U;
    size_t chromAllocs = 1;
    Bed::SignedCoordType lastidx = 0;

    Bed::SignedCoordType startPos = 0, endPos = 0;
    Bed::LineCountType lines = 1;
    FILE* tmpX;

    BedData *beds;
    ChromBedData *chrom;

    char **tmpFileNames = NULL;
    FILE **tmpFiles = NULL;
    char *tfile = NULL;

    double **chromBytes = NULL;
    const int chromCrossover = 1000;
    const unsigned int maxTmpFiles = 120; // can hit max open file descriptors in extreme cases.  use hierarchial merge-sort
    double diffBytes = 0;
    double maxChromBytes = 0;
    bool firstCross = true;
    std::map<std::string, unsigned int> chrNames;
    std::map<std::string, unsigned int>::iterator siter;

    /*Line reading buffers*/
    char *bedLine = NULL;
    char *chromBuf = NULL;
    char *tmpArr = NULL;

    bedLine = static_cast<char *>( malloc(BED_LINE_LEN + 1) );
    if(!bedLine) 
        {
            fprintf(stderr, "Error: Could not allocate space for BED line input buffer\n");
            return EXIT_FAILURE;
        }
    chromBuf = static_cast<char *>( malloc(CHROM_NAME_LEN + 1) );
    if(!chromBuf)
        {
            fprintf(stderr, "Error: Could not allocate space for chromosome name input buffer\n");
            return EXIT_FAILURE;
        }
    tmpArr = static_cast<char *>( malloc(BED_LINE_LEN + 1) );
    if(!tmpArr)
        {
            fprintf(stderr, "Error: Could not allocate space for temporary BED line input buffer\n");
            return EXIT_FAILURE;
        }

    bedLine[0] = '\0';
    chromBuf[0] = '\0';
    tmpArr[0] = '\0';

    char *cptr = NULL;
    char *dptr = NULL;

    /* check input files */
    if(0 != checkFiles(bedFileNames, numFiles))
        {
            return EXIT_FAILURE;
        }

    /* if we'll perform file system merge sort, create or check tmp dir */
    if ((tmpPath != NULL) && (createDir(tmpPath) == EXIT_FAILURE))
        {
            return EXIT_FAILURE;
        }

    chromBytes = static_cast<double**>( malloc(sizeof(double *)) );
    if(chromBytes == NULL)
        {
            fprintf(stderr, "Error: %s, %d: Unable to create double* array. Out of memory.\n", __FILE__, __LINE__);
            return EXIT_FAILURE;
        }

    /* a guess for general overhead for local vars, function call stacks, etc. */
    const int overhead = 50000000 + (2 * (BED_LINE_LEN + 1)) + CHROM_NAME_LEN + 1;
    double totalBytes = overhead;

    beds = initializeBedData(&totalBytes);
    if(beds == NULL) 
        {
            fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
            return EXIT_FAILURE;
        }



    for(iidx = 0; iidx < numFiles; iidx++) 
        {
            headCheck = 1;
            lines = 1;
            notStdin = strcmp(bedFileNames[iidx], "-");
            if(notStdin)
                {
                    bedFile = fopen(bedFileNames[iidx], "r");
                }
            else
                {
                    bedFile = stdin;
                }

            /* error check if your line length (including the newline) is BED_LINE_LEN or more */
            bedLine[BED_LINE_LEN] = '1';
            bedLine[0] = '\n';
            while(fgets(bedLine, BED_LINE_LEN+1, bedFile))
                {
                    if('\n' == bedLine[0])
                        { /* only a new line was found */
                            lines++;
                            continue;
                        }
                    else if('\0' == bedLine[BED_LINE_LEN])
                        {
                            fprintf(stderr, "BED row length exceeds capacity at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKENS_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                            return EXIT_FAILURE;
                        }
                    else if(' ' == bedLine[0] || '\t' == bedLine[0])
                        {
                            fprintf(stderr, "Row begins with a tab or space at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }

                    if(headCheck &&
                       (strstr(bedLine, "browser") == bedLine ||
                        strstr(bedLine, "track") == bedLine ||
                        strstr(bedLine, "#") == bedLine ||
                        strstr(bedLine, "@") == bedLine))
                        { /* allow silly headers on input; delete them on output */
                            lines++;
                            continue;
                        }

                    /* chromosome check */
                    cptr = strpbrk(bedLine, "\t "); /* we'll convert spaces to tabs in the first 3 fields */
                    if(cptr == NULL)
                        {
                            fprintf(stderr, "No tabs/spaces found at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    if(static_cast<size_t>(cptr - bedLine) > CHROM_NAME_LEN)
                        {
                            fprintf(stderr, "Chromosome name too long at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_CHR_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                            return EXIT_FAILURE;
                        }

                    // reverse chrom name for faster lookup in common case that everything looks like chrBLAH
                    jidx = 0;
                    for ( kidx = static_cast<unsigned int>(cptr-bedLine); kidx > 0; )
                      chromBuf[jidx++] = bedLine[--kidx];
                    chromBuf[static_cast<size_t>(cptr-bedLine)-1] = bedLine[0];
                    //  memcpy(chromBuf, bedLine, static_cast<size_t>(cptr-bedLine));
                    chromBuf[cptr-bedLine] = '\0';

                    /* start coord check */
                    dptr = strpbrk(++cptr, "\t "); /* we'll convert spaces to tabs in the first 3 fields */
                    if(dptr == NULL)
                        {
                            fprintf(stderr, "No tabs/spaces found after the start coordinate (or no start coordinate at all) at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    if(dptr - cptr > static_cast<double>( Bed::MAX_DEC_INTEGERS ))
                        {
                            fprintf(stderr, "Start coordinate is too large.  Max decimal digits allowed is %ld in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_DEC_INTEGERS, lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    else if(0 == dptr - cptr)
                        {
                            fprintf(stderr, "Consecutive tabs and/or spaces between chromosome and start coordinate.  See line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    memcpy(tmpArr, cptr, static_cast<size_t>(dptr-cptr));
                    tmpArr[dptr-cptr] = '\0';
                    for(kidx=0; kidx < static_cast<unsigned int>(dptr-cptr); ++kidx)
                        {
                            if(!isdigit(tmpArr[kidx]))
                                {
                                    fprintf(stderr, "Non-numeric start coordinate.  See line %" PRIu64 " in %s.\n(remember that chromosome names should not contain spaces.)\n",
                                            lines, bedFileNames[iidx]);
                                    return EXIT_FAILURE;
                                }
                        } /* for */
                    if(atof(tmpArr) > double(Bed::MAX_COORD_VALUE))
                        {
                            fprintf(stderr, "Start coordinate is too large.  Max allowed value is %" PRIu64 " in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_COORD_VALUE, lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    sscanf(tmpArr, "%" SCNd64, &startPos);

                    /* end coord check */
                    cptr = strpbrk(++dptr, "\t ");
                    if(cptr == NULL)
                        { /* eol */
                            cptr = strchr(dptr, '\n');
                            if(cptr == NULL)
                                {
                                    fprintf(stderr, "No end of line found at %" PRIu64 " in %s.\nMay need to increase BED_LINE_LEN and recompile.\nFirst check that you have unix newlines (cat -A).",
                                            lines, bedFileNames[iidx]);
                                    return EXIT_FAILURE;
                                }
                        }
                    if(cptr - dptr > static_cast<double>( Bed::MAX_DEC_INTEGERS ))
                        {
                            fprintf(stderr, "End coordinate is too large.  Max decimal digits allowed is %ld in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_DEC_INTEGERS, lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    else if(cptr == dptr)
                        {
                            fprintf(stderr, "Extra tab and/or space found in between start and end coordinates.  See line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    memcpy(tmpArr, dptr, static_cast<size_t>(cptr-dptr));
                    tmpArr[cptr-dptr] = '\0';
                    for(kidx=0; kidx < static_cast<unsigned int>(cptr-dptr); ++kidx)
                        {
                            if(!isdigit(tmpArr[kidx]))
                                {
                                    fprintf(stderr, "Non-numeric end coordinate.  See line %" PRIu64 " in %s.\n",
                                            lines, bedFileNames[iidx]);
                                    return EXIT_FAILURE;
                                }
                        } /* for */
                    if(atof(tmpArr) > double(Bed::MAX_COORD_VALUE))
                        {
                            fprintf(stderr, "End coordinate is too large.  Max allowed value is %" PRIu64 " in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_COORD_VALUE, lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    sscanf(tmpArr, "%" SCNd64, &endPos);

                    /* rest of the line goes into bedLine */
                    fields = 3;
                    if ( (val = sscanf(cptr, "\t%[^\n]s\n", bedLine)) != EOF )
                      fields += val;
                    headCheck = 0;

                    /* Validate Coords */
                    if ((startPos < 0) || (endPos < 0)) 
                        {
                            fprintf(stderr, "Error on line %" PRIu64 " in %s. Genomic position must be greater than 0.\n", 
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }
                    if (endPos <= startPos)
                        {
                            fprintf(stderr, "Error on line %" PRIu64 " in %s. Genomic end coordinate is less than (or equal to) start coordinate.\n", 
                                    lines, bedFileNames[iidx]);
                            return EXIT_FAILURE;
                        }


                    /*Find the chrom*/
                    newChrom = 1;
                    if (beds->numChroms < chromCrossover)
                        {
                            if (beds->numChroms > 0 && (strcmp(beds->chroms[lastidx]->chromName, chromBuf) == 0))
                                { /* same chr as last row which often happens in practice */
                                    jidx = static_cast<unsigned int>(lastidx);
                                    newChrom = 0;
                                }
                            else /* linear search */
                                {
                                    for(jidx = 0; jidx < beds->numChroms; jidx++)
                                        {
                                            if(strcmp(beds->chroms[jidx]->chromName, chromBuf) == 0)
                                                {
                                                   lastidx = jidx;
                                                   newChrom = 0;
                                                   break;
                                                }
                                        } /* for */
                                }
                        }
                    else /* map search since there are a ridiculous # distinct chrom names */
                        {
                            if (firstCross)
                                { /* copy over what we have up until this point into our map */
                                    firstCross = false;
                                    chrNames.clear();
                                    for(tidx = 0; tidx < beds->numChroms; ++tidx)
                                       chrNames.insert(std::make_pair(std::string(beds->chroms[tidx]->chromName), tidx));
                                }

                            /*Find the chrom*/
                            siter = chrNames.find(std::string(chromBuf));
                            if ( siter != chrNames.end() )
                                { /* chrom already exists */
                                    jidx = siter->second;
                                    newChrom = 0;
                                }
                            else /* Create a new chrom */
                                {
                                    chrNames.insert(std::make_pair(std::string(chromBuf), beds->numChroms));
                                }
                        }

                    if (!newChrom)
                        {
                            /* Append data to current chrom */
                            diffBytes = totalBytes;
                            if(fields > 3)
                                { /* check ID column is <= ID_NAME_LEN for the benefit of downstream programs */
                                    cptr = strpbrk(bedLine, "\t "); /* bedops/bedmap do not differentiate these whitespace characters */
                                    if(cptr == NULL)
                                        {
                                            if(strlen(bedLine) > ID_NAME_LEN)
                                                {
                                                    fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                            lines, bedFileNames[iidx]);
                                                    fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                                    fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                                    return EXIT_FAILURE;
                                                }
                                        }
                                    else if(cptr - bedLine > static_cast<double>( ID_NAME_LEN ))
                                        {
                                            fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                    lines, bedFileNames[iidx]);
                                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                            fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                            return EXIT_FAILURE;
                                        }
                                    chromEntryCount = appendChromBedEntry(beds->chroms[jidx], startPos, endPos, bedLine, &totalBytes, maxMem);
                                }
                            else
                                {
                                    chromEntryCount = appendChromBedEntry(beds->chroms[jidx], startPos, endPos, NULL, &totalBytes, maxMem);
                                }

                            if (static_cast<int>(chromEntryCount) < 0)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure.\n", __FILE__, __LINE__);
                                    return EXIT_FAILURE;
                                }
                            diffBytes = totalBytes - diffBytes;
                            *chromBytes[jidx] += diffBytes;
                            maxChromBytes = (*chromBytes[jidx] < maxChromBytes) ? maxChromBytes : *chromBytes[jidx]; 
                        }
                    else /* new chrom */
                        {
                            errno = 0;
                            if(beds->numChroms >= static_cast<double>( ((NUM_CHROM_EST * chromAllocs)) ))
                                {   /* Resize Chrom Structure */
                                    chromAllocs++;
                                    beds->chroms = static_cast<ChromBedData**>( realloc(beds->chroms, sizeof(ChromBedData*) * NUM_CHROM_EST * chromAllocs) );
                                    totalBytes += sizeof(ChromBedData*) * NUM_CHROM_EST;
                                    if(beds->chroms == NULL)
                                        {
                                            fprintf(stderr, "Error: %s, %d: Unable to expand Chrom structure: %s. Out of memory.\n", __FILE__, 
                                                    __LINE__, strerror(errno));
                                            return EXIT_FAILURE;
                                        }
                                }

                            diffBytes = totalBytes;
                            chrom = initializeChromBedData(chromBuf, &totalBytes);
                            if(chrom == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create Chrom structure: %s. Out of memory.\n", __FILE__, 
                                            __LINE__, strerror(errno));
                                    return EXIT_FAILURE;
                                }
                            diffBytes = totalBytes - diffBytes;
                            chromBytes = static_cast<double**>( realloc(chromBytes, sizeof(double*) * (static_cast<size_t>(beds->numChroms) + 1)) );
                            if(chromBytes == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create double* array. Out of memory.\n", __FILE__, __LINE__);
                                    return EXIT_FAILURE;
                                }
                            chromBytes[beds->numChroms] = static_cast<double*>( malloc(sizeof(double)) );
                            if(chromBytes[beds->numChroms] == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create double. Out of memory.\n", __FILE__, __LINE__);
                                    return EXIT_FAILURE;
                                }
                            *chromBytes[beds->numChroms] = diffBytes;
                            totalBytes += sizeof(double*) + sizeof(double); // sizeof(double*) increments in realloc of chromBytes + sizeof(double) for malloc
                            diffBytes = totalBytes;
                            if(fields > 3)
                                { /* check ID column is <= ID_NAME_LEN for the benefit of downstream programs */
                                    cptr = strpbrk(bedLine, "\t "); /* bedops/bedmap do not differentiate these whitespace characters */
                                    if(cptr == NULL)
                                        {
                                            if(strlen(bedLine) > ID_NAME_LEN)
                                                {
                                                    fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                            lines, bedFileNames[iidx]);
                                                    fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                                    fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                                    return EXIT_FAILURE;
                                                }
                                        }
                                    else if(cptr - bedLine > static_cast<double>( ID_NAME_LEN ))
                                        {
                                            fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                    lines, bedFileNames[iidx]);
                                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                            fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                            return EXIT_FAILURE;
                                        }
                                    chromEntryCount = appendChromBedEntry(chrom, startPos, endPos, bedLine, &totalBytes, maxMem);
                                }
                            else
                                {
                                    chromEntryCount = appendChromBedEntry(chrom, startPos, endPos, NULL, &totalBytes, maxMem);
                                }

                            if(static_cast<int>(chromEntryCount) < 0) 
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure.\n", __FILE__, __LINE__);
                                    return EXIT_FAILURE;
                                }        
                            diffBytes = totalBytes - diffBytes;
                            *chromBytes[beds->numChroms] += diffBytes;
                            maxChromBytes = (*chromBytes[beds->numChroms] < maxChromBytes) ? maxChromBytes : *chromBytes[beds->numChroms];
        
                            beds->chroms[beds->numChroms] = chrom;
                            lastidx = beds->numChroms++;
                        }

                     /* check memory */
                     if(maxMem > 0 && (totalBytes + maxChromBytes >= maxMem))
                         {
                             /* worst case quicksort memory is O(2*n),
                                yet we sort by a single chrom at a time and totalBytes already
                                accounts for 1*n maxChromBytes.  totalBytes is a conservative
                                measure of memory used.
                             */

                             errno = 0;
                             tmpFiles = static_cast<FILE**>( realloc(tmpFiles, sizeof(FILE*) * (tmpFileCount+1)) );
                             if(tmpFiles == NULL)
                                 {
                                     fprintf(stderr, "Error: %s, %d: Unable to create FILE* array: %s. Out of memory.\n", __FILE__, 
                                             __LINE__, strerror(errno));
                                     return EXIT_FAILURE;
                                 }
                             errno = 0;
                             tmpFileNames = static_cast<char**>( realloc(tmpFileNames, sizeof(char*) * (tmpFileCount+1)) );
                             if(tmpFileNames == NULL)
                                 {
                                     fprintf(stderr, "Error: %s, %d: Unable to create char* array: %s. Out of memory.\n", __FILE__, 
                                             __LINE__, strerror(errno));
                                     return EXIT_FAILURE;
                                 }
                             totalBytes += sizeof(FILE*) * (tmpFileCount+1);
                             totalBytes += sizeof(char*) * (tmpFileCount+1);
                             tfile = NULL;
                             tmpFiles[tmpFileCount] = createTmpFile(tmpPath, &tfile);
                             if(tmpFiles[tmpFileCount] == NULL)
                                 {
                                     fprintf(stderr, "Error: %s, %d: Unable to create FILE* for temp file: %s. Out of memory.\n", __FILE__, 
                                             __LINE__, strerror(errno));
                                     return EXIT_FAILURE;
                                 }
                             totalBytes += (tfile == NULL) ? 0 : (strlen(tfile)+1);
                             tmpFileNames[tmpFileCount] = tfile;
                             lexSortBedData(beds);
                             printBed(tmpFiles[tmpFileCount], beds, printUniques, printDuplicates);
                             for(tidx = 0; tidx < beds->numChroms; ++tidx)
                                 free(chromBytes[tidx]);
                             free(chromBytes);

                             freeBedData(beds);
                             chromAllocs = 1;
                             chrNames.clear();
                             firstCross = true;
                             chromBytes = static_cast<double**>( malloc(sizeof(double *)) );
                             if(chromBytes == NULL)
                                 {
                                     fprintf(stderr, "Error: %s, %d: Unable to create double* array. Out of memory.\n", __FILE__, __LINE__);
                                     return EXIT_FAILURE;
                                 }
                             maxChromBytes = 0;
                             totalBytes = overhead; /* already includes chromBytes array */
                             if ( ++tmpFileCount == maxTmpFiles )
                                 { /* hierarchial merge sort to keep # open file descriptors low */
                                     tfile = NULL;
                                     tmpX = createTmpFile(tmpPath, &tfile);
                                     if(tmpX == NULL)
                                         {
                                             fprintf(stderr, "Error: %s, %d: Unable to create FILE* for temp file: %s. Out of memory.\n", __FILE__, 
                                                     __LINE__, strerror(errno));
                                             return EXIT_FAILURE;
                                         }

                                     if(0 != mergeSort(tmpX, tmpFiles, tmpFileCount))
                                         {
                                             fprintf(stderr, "Error: %s, %d.  Out of memory.\n", __FILE__, __LINE__);
                                             return EXIT_FAILURE;
                                         }

                                     freeTmpFiles(tmpFileCount, tmpFiles, tmpFileNames);
                                     tmpFiles = static_cast<FILE**>( malloc(sizeof(FILE *)) );
                                     if(tmpFiles == NULL)
                                         {
                                             fprintf(stderr, "Error: %s, %d: Unable to create FILE* array. Out of memory.\n", __FILE__, __LINE__);
                                             return EXIT_FAILURE;
                                         }
                                     tmpFileNames = static_cast<char**>( malloc(sizeof(char *)) );
                                     if(tmpFileNames == NULL)
                                         {
                                             fprintf(stderr, "Error: %s, %d: Unable to create char* array. Out of memory.\n", __FILE__, __LINE__);
                                             return EXIT_FAILURE;
                                         }
                                     totalBytes += sizeof(FILE *);
                                     totalBytes += sizeof(char *);
                                     totalBytes += (tfile == NULL) ? 0 : (strlen(tfile)+1);
                                     tmpFileCount = 1U;
                                     tmpFiles[0] = tmpX;
                                     tmpFileNames[0] = tfile;
                                     tmpX = NULL;
                                 }

                             beds = initializeBedData(&totalBytes);
                             if(beds == NULL) 
                                 {
                                     fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                                     return EXIT_FAILURE;
                                 }
                         }

                     bedLine[BED_LINE_LEN] = '1';
                     bedLine[0] = '\n';
                     lines++;
                 } /* while */

             if(notStdin)
                 {
                     fclose(bedFile);
                 }
         } /* for */

    if(tmpFileCount > 0)
        {
            if(beds->numChroms > 0)
                { /* sort and spit out what's in memory */
                    errno = 0;
                    tmpFiles = static_cast<FILE **>( realloc(tmpFiles, sizeof(FILE*) * (tmpFileCount+1)) );
                    if(tmpFiles == NULL)
                        {
                            fprintf(stderr, "Error: %s, %d: Unable to expand Chrom structure: %s. Out of memory.\n", __FILE__, 
                                    __LINE__, strerror(errno));
                            return EXIT_FAILURE;
                        }
                    tmpFileNames = static_cast<char**>( realloc(tmpFileNames, sizeof(char*) * (tmpFileCount+1)) );
                    if(tmpFileNames == NULL)
                        {
                            fprintf(stderr, "Error: %s, %d: Unable to create char* array: %s. Out of memory.\n", __FILE__, 
                                    __LINE__, strerror(errno));
                            return EXIT_FAILURE;
                        }
                    tfile = NULL;
                    tmpFiles[tmpFileCount] = createTmpFile(tmpPath, &tfile);
                    if(tmpFiles[tmpFileCount] == NULL)
                        {
                            fprintf(stderr, "Error: %s, %d: Unable to create FILE* for temp file: %s. Out of memory.\n", __FILE__, 
                                    __LINE__, strerror(errno));
                            return EXIT_FAILURE;
                        }
                    tmpFileNames[tmpFileCount] = tfile;
                    lexSortBedData(beds);
                    printBed(tmpFiles[tmpFileCount], beds, printUniques, printDuplicates);
                    ++tmpFileCount;
                    for(tidx = 0; tidx < beds->numChroms; ++tidx)
                        free(chromBytes[tidx]);
                    free(chromBytes);
                    freeBedData(beds);
                }
            if(0 != mergeSort(stdout, tmpFiles, tmpFileCount))
                {
                    fprintf(stderr, "Error: %s, %d.  Out of memory.\n", __FILE__, __LINE__);
                    return EXIT_FAILURE;
                }
            freeTmpFiles(tmpFileCount, tmpFiles, tmpFileNames);
        }
    else
        {
            lexSortBedData(beds);
            printBed(stdout, beds, printUniques, printDuplicates);
            for(tidx = 0; tidx < beds->numChroms; ++tidx)
                free(chromBytes[tidx]);
            free(chromBytes);
            /* freeBedData(beds); let the OS clean up - takes significant time to do this step manually */
        }

    free(bedLine);
    bedLine = NULL;
    free(chromBuf);
    chromBuf = NULL;
    free(tmpArr);
    tmpArr = NULL;
    
    return EXIT_SUCCESS;
}

void 
printBed(FILE *out, BedData *beds, const bool printUniques, const bool printDuplicates)
{
    unsigned int i = 0U;
    Bed::LineCountType j = 0;

    if(beds == NULL) 
        return;

    if (!printUniques && !printDuplicates)
        for(i = 0; i < beds->numChroms; i++)
            for(j = 0; j < beds->chroms[i]->numCoords; j++) 
                {
                    fprintf(out, 
                            "%s\t%" PRId64 "\t%" PRId64, 
                            beds->chroms[i]->chromName, 
                            beds->chroms[i]->coords[j].startCoord, 
                            beds->chroms[i]->coords[j].endCoord);
                    if(beds->chroms[i]->coords[j].data)
                        fprintf(out, "\t%s\n", beds->chroms[i]->coords[j].data);
                    else
                        fprintf(out, "\n");
                }
    else 
        {
            char *currElem = NULL;
            char *prevElem = NULL;
            char *nextElem = NULL;

            prevElem = static_cast<char *>( malloc(BED_LINE_LEN + 1) );
            if(!prevElem)
                {
                    fprintf(stderr, "Error: Could not allocate space for previous BED line input buffer\n");
                    exit(EXIT_FAILURE);
                }
            
            currElem = static_cast<char *>( malloc(BED_LINE_LEN + 1) );
            if(!currElem)
                {
                    fprintf(stderr, "Error: Could not allocate space for current BED line input buffer\n");
                    exit(EXIT_FAILURE);
                }
            
            nextElem = static_cast<char *>( malloc(BED_LINE_LEN + 1) );
            if(!nextElem)
                {
                    fprintf(stderr, "Error: Could not allocate space for next BED line input buffer\n");
                    exit(EXIT_FAILURE);
                }

            for (i = 0; i < beds->numChroms; i++) 
                {
                    bool duplicateFound = false;
                    for (j = 0; j < beds->chroms[i]->numCoords; j++) 
                        {
                            if (j == 0) {
                                sprintf(currElem,
                                        "%s\t%" PRId64 "\t%" PRId64,
                                        beds->chroms[i]->chromName, 
                                        beds->chroms[i]->coords[j].startCoord, 
                                        beds->chroms[i]->coords[j].endCoord);
                                if(beds->chroms[i]->coords[j].data)
                                    sprintf(currElem + strlen(currElem), "\t%s\n", beds->chroms[i]->coords[j].data);
                                else
                                    sprintf(currElem + strlen(currElem), "\n");
                                if (beds->chroms[i]->numCoords > 1) {
                                    sprintf(nextElem,
                                            "%s\t%" PRId64 "\t%" PRId64,
                                            beds->chroms[i]->chromName, 
                                            beds->chroms[i]->coords[j+1].startCoord, 
                                            beds->chroms[i]->coords[j+1].endCoord);
                                    if(beds->chroms[i]->coords[j].data)
                                        sprintf(nextElem + strlen(nextElem), "\t%s\n", beds->chroms[i]->coords[j+1].data);
                                    else
                                        sprintf(nextElem + strlen(nextElem), "\n");
                                }
                                else {
                                    nextElem[0] = '\0';
                                }
                            }
                            else if (j < beds->chroms[i]->numCoords - 1) {
                                memcpy(currElem, nextElem, strlen(nextElem)+1); 
                                sprintf(nextElem,
                                        "%s\t%" PRId64 "\t%" PRId64,
                                        beds->chroms[i]->chromName, 
                                        beds->chroms[i]->coords[j+1].startCoord, 
                                        beds->chroms[i]->coords[j+1].endCoord);
                                if(beds->chroms[i]->coords[j].data)
                                    sprintf(nextElem + strlen(nextElem), "\t%s\n", beds->chroms[i]->coords[j+1].data);
                                else
                                    sprintf(nextElem + strlen(nextElem), "\n");
                            }
                            else {
                                memcpy(currElem, nextElem, strlen(nextElem)+1); 
                                nextElem[0] = '\0';
                            }
    
                            if (printUniques)
                                {
                                    if((prevElem[0] == '\0') && (strcmp(currElem, nextElem) != 0))
                                        {
                                            fprintf(out, "%s", currElem);
                                        }
                                    else if ((strcmp(prevElem, currElem) != 0) && (strcmp(currElem, nextElem) != 0))
                                        {
                                            fprintf(out, "%s", currElem);
                                        }
                                    else if ((strcmp(prevElem, currElem) == 0) && (strcmp(currElem, nextElem) != 0) && duplicateFound)
                                        {
                                            fprintf(out, "%s", currElem);
                                            duplicateFound = false;
                                        }
                                    else if ((strcmp(currElem, nextElem) == 0) && !duplicateFound)
                                        {
                                            duplicateFound = true;
                                        }
                                    else if ((strcmp(currElem, nextElem) != 0) && duplicateFound)
                                        {
                                            duplicateFound = false;
                                        }
                                }
                            else if (printDuplicates)
                                {
                                    if((strcmp(prevElem, currElem) == 0) && (strcmp(currElem, nextElem) == 0))
                                        {
                                            continue;
                                        }
                                    else if(strcmp(currElem, prevElem) == 0)
                                        {
                                            fprintf(out, "%s", currElem);
                                        }
                                }
    
                            memcpy(prevElem, currElem, strlen(currElem)+1);
                        }
                }
            
            free(prevElem);
            prevElem = NULL;
            
            free(currElem);
            currElem = NULL;
            
            free(nextElem);
            nextElem = NULL;
        }

    return;
}

void 
freeBedData(BedData *beds) 
{
    unsigned int i = 0;
    Bed::LineCountType j = 0;

    if(beds == NULL) 
        {
            return;
        }
  
    for(i = 0; i < beds->numChroms; i++) 
        {

            for(j = 0; j < beds->chroms[i]->numCoords; j++) 
                {
                    free(beds->chroms[i]->coords[j].data);
                }
            free(beds->chroms[i]->coords);
            free(beds->chroms[i]);
        }
    free(beds->chroms);
    free(beds);
}

void
lexSortBedData(BedData *beds)
{
    unsigned int i, j, k;
    char chromBuf[CHROM_NAME_LEN + 1];
    chromBuf[0] = '\0';
    size_t chromBufLen;

    if(beds == NULL) 
        {
            return;
        }

    /* reverse chromosome names (back to correct names) before comparisons */
    for(i = 0; i < beds->numChroms; ++i)
        {
            k = 0;
            chromBufLen = strlen(beds->chroms[i]->chromName); // we know >= 1
            strncpy(chromBuf, beds->chroms[i]->chromName, chromBufLen);
            for ( j = static_cast<unsigned int>(chromBufLen); j > 0; )
                beds->chroms[i]->chromName[k++] = chromBuf[--j];
            /* terminating null is already in correct spot */
        }

    /* sort coords */
    for(i = 0; i < beds->numChroms; ++i)
        {
            std::sort(beds->chroms[i]->coords, (beds->chroms[i]->coords+static_cast<size_t>(beds->chroms[i]->numCoords)));
        }

    /* sort chroms */
    qsort(beds->chroms, static_cast<size_t>(beds->numChroms), sizeof(ChromBedData *), lexCompareBedData);
    return;
}

int 
lexCompareBedData(const void *chrPos1, const void *chrPos2) 
{
    ChromBedData* const* chrPos1Cbd = static_cast<ChromBedData* const*>(chrPos1);
    ChromBedData* const* chrPos2Cbd = static_cast<ChromBedData* const*>(chrPos2);
    return strcmp((*chrPos1Cbd)->chromName, (*chrPos2Cbd)->chromName);
}
