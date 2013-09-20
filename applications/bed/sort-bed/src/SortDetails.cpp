/*
  FILE: Bed.c
  AUTHOR: Scott Kuehn
  CREATE DATE: Tue May 16 10:06:58 PDT 2006
  PROJECT: CompBio
  ID: '$Id: Bed.c,v 1.10 2010/08/19 22:18:54 sjn Exp $'
*/

#include <cinttypes>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "suite/BEDOPS.Constants.hpp"

#include "Structures.hpp"

using namespace std;

BedData * 
initializeBedData(double *bytes) 
{

    BedData * beds;

    beds = (BedData*)malloc(sizeof(BedData));
    *bytes += sizeof(BedData);

    if (beds == NULL) 
        {
            return NULL;
        }
    beds->numChroms = 0;
    beds->chroms = (ChromBedData**)malloc(sizeof(ChromBedData *) * (NUM_CHROM_EST));
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

    chrom = (ChromBedData*)malloc(sizeof(ChromBedData));
    *bytes += sizeof(ChromBedData);
    if(chrom == NULL) 
        {
            return NULL;
        }

    /* Coords */
    chrom->coords = (BedCoordData*)malloc(sizeof(BedCoordData) * NUM_BED_ITEMS_EST);
    *bytes += (sizeof(BedCoordData) * NUM_BED_ITEMS_EST);
    if(chrom->coords == NULL)
        {
            return NULL;
        }
  
    /* Chrom name*/
    chromBufLen = strlen(chromBuf);
    strncpy(chrom->chromName, chromBuf, chromBufLen);
    chrom->chromName[chromBufLen] = '\0';
    chrom->numCoords = 0;
    return chrom;
}



Bed::LineCountType
appendChromBedEntry(ChromBedData *chrom, Bed::SignedCoordType startPos, Bed::SignedCoordType endPos, char *data, double *bytes)
{

    Bed::LineCountType index;
    size_t dataBufLen;
    char *dataPtr;

    if(chrom == NULL)
        {
            fprintf(stderr, "Error: %s, %d: Bad 'chrom' variable.\n", __FILE__, __LINE__);
            return static_cast<Bed::LineCountType>(-1);
        }
  
    index = chrom->numCoords;

    if(((index + 1) % NUM_BED_ITEMS_EST) == 0)
        {
            //fprintf(stderr, "Reallocating...\n");
            chrom->coords = (BedCoordData*)realloc(chrom->coords, sizeof(BedCoordData) * static_cast<size_t>(index + NUM_BED_ITEMS_EST));
            *bytes += (sizeof(BedCoordData) * (index + NUM_BED_ITEMS_EST));
            if(chrom->coords == NULL)
                {
                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                    return static_cast<Bed::LineCountType>(-1);
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
                    return static_cast<Bed::LineCountType>(-1);
                }
            dataPtr = (char*)calloc(dataBufLen + 1, sizeof(char));
            *bytes += dataBufLen + 1;
            if(dataPtr == NULL) 
                {
                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                    return static_cast<Bed::LineCountType>(-1);
                }
            chrom->coords[index].data = strncpy(dataPtr, data, dataBufLen + 1);
            chrom->coords[index].data[dataBufLen] = '\0';
        }
    else
        chrom->coords[index].data = NULL;

    chrom->numCoords++;
  
    return chrom->numCoords;
}

int
checkfiles(const char **bedFileNames, unsigned int numFiles)
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
mergeSort(FILE **tmpFiles, unsigned int numFiles)
{
    /* error checking in processData() has already been performed, headers and empty rows removed, etc. */
    unsigned int i = 0U;
    int done = 0, currMin = 0, val = 0;
    char **chroms = (char**)malloc(sizeof(char*) * static_cast<size_t>(numFiles));

    if(chroms == NULL)
        return -1;

    Bed::SignedCoordType *starts = (Bed::SignedCoordType*)malloc(sizeof(Bed::SignedCoordType) * static_cast<size_t>(numFiles));
    if(starts == NULL)
        return -1;

    Bed::SignedCoordType *ends = (Bed::SignedCoordType*)malloc(sizeof(Bed::SignedCoordType) * static_cast<size_t>(numFiles));
    if(ends == NULL)
        return -1;

    int *fields = (int*)malloc(sizeof(int) * static_cast<size_t>(numFiles));
    if(fields == NULL)
        return -1;

    char **rests = (char**)malloc(sizeof(char*) * static_cast<size_t>(numFiles));
    if(rests == NULL)
        return -1;

    for(i=0; i < numFiles; ++i) {
        fseek(tmpFiles[i], 0, SEEK_SET);
        chroms[i] = (char*)malloc(sizeof(char) * (CHROM_NAME_LEN+1));
        if(chroms[i] == NULL)
            return -1;
        rests[i] = (char*)malloc(sizeof(char) * (BED_LINE_LEN+1));
        if(rests[i] == NULL)
            return -1;
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
            for(i=0; i < numFiles; ++i)
                {
                    if(starts[i]>=0)
                        {
                            if(currMin<0 || (val = strcmp(chroms[i], chroms[currMin]))<0)
                                currMin = static_cast<int>(i);
                            else if(0 == val)
                                {
                                    if(starts[i] < starts[currMin])
                                        currMin = static_cast<int>(i);
                                    else if(starts[i] == starts[currMin] && ends[i] < ends[currMin])
                                        currMin = static_cast<int>(i);
                                }
                        }
                } /* for */
            if(currMin < 0)
                break;

            if(3 == fields[currMin])
                fprintf(stdout, "%s\t%" PRId64 "\t%" PRId64 "\n", chroms[currMin],
                        starts[currMin], ends[currMin]);
            else
                fprintf(stdout, "%s\t%" PRId64 "\t%" PRId64 "%s\n", chroms[currMin],
                        starts[currMin], ends[currMin], rests[currMin]);

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

int
processData(const char **bedFileNames, unsigned int numFiles, double maxMem)
{
    /* maxMem will be ignored if <= 0 */

    FILE *bedFile = NULL;
    Bed::LineCountType chromEntryCount;
    int k, notStdin = 0, newChrom,
        fields = 0,
        headCheck = 1;
    unsigned int i, j;
    unsigned int tmpFileCount = 0U;
    size_t chromAllocs = 1;

    Bed::SignedCoordType startPos = 0, endPos = 0;
    Bed::LineCountType lines = 1;

    BedData *beds;
    ChromBedData *chrom;

    FILE **tmpFiles = (FILE**)malloc(sizeof(FILE *));
    if(tmpFiles == NULL)
        {
            fprintf(stderr, "Error: %s, %d: Unable to create FILE* array. Out of memory.\n", __FILE__, __LINE__);
            return -1;
        }

    double **chromBytes = (double**)malloc(sizeof(double *));

    if(chromBytes == NULL)
        {
            fprintf(stderr, "Error: %s, %d: Unable to create double* array. Out of memory.\n", __FILE__, __LINE__);
            return -1;
        }

    /* a guess for general overhead for local vars, function call stacks, etc. */
    const int overhead = 50000 + sizeof(FILE *) + sizeof(double **);
    double totalBytes = overhead;
    double diffBytes = 0;
    double maxChromBytes = 0;

    /*Line reading buffers*/
    char bedLine[BED_LINE_LEN + 1];
    char chromBuf[CHROM_NAME_LEN + 1];
    char *cptr = NULL;
    char *dptr = NULL;
    char tmpArr[BED_LINE_LEN + 1];
    bedLine[0] = '\0';
    chromBuf[0] = '\0';
    totalBytes += BED_LINE_LEN + 1;
    totalBytes += CHROM_NAME_LEN + 1;

    if(0 != checkfiles(bedFileNames, numFiles))
        {
            return -1;
        }

    beds = initializeBedData(&totalBytes);
    if(beds == NULL) 
        {
            fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
            return -1;
        }

    for(i = 0; i < numFiles; i++) 
        {
            headCheck = 1;
            lines = 1;
            notStdin = strcmp(bedFileNames[i], "-");
            if(notStdin)
                {
                    bedFile = fopen(bedFileNames[i], "r");
                }
            else
                {
                    bedFile = stdin;
                }

            /* error check if your line length (including the newline) is BED_LINE_LEN or more */
            bedLine[BED_LINE_LEN] = '1';
            while(fgets(bedLine, BED_LINE_LEN+1, bedFile))
                {
                    if(1 == strlen(bedLine))
                        { /* only a new line was found */
                            lines++;
                            continue;
                        }
                    else if('\0' == bedLine[BED_LINE_LEN])
                        {
                            fprintf(stderr, "BED row length exceeds capacity at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[i]);
                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKENS_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                            fclose(bedFile);
                            return -1;
                        }
                    else if(' ' == bedLine[0] || '\t' == bedLine[0])
                        {
                            fprintf(stderr, "Row begins with a tab or space at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
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
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    if(static_cast<size_t>(cptr - bedLine) > CHROM_NAME_LEN)
                        {
                            fprintf(stderr, "Chromosome name too long at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[i]);
                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_CHR_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                            fclose(bedFile);
                            return -1;
                        }
                    memcpy(chromBuf, bedLine, static_cast<size_t>(cptr-bedLine));
                    chromBuf[cptr-bedLine] = '\0';

                    /* start coord check */
                    dptr = strpbrk(++cptr, "\t "); /* we'll convert spaces to tabs in the first 3 fields */
                    if(dptr == NULL)
                        {
                            fprintf(stderr, "No tabs/spaces found after the start coordinate (or no start coordinate at all) at line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    if(dptr - cptr > (double)Bed::MAX_DEC_INTEGERS)
                        {
                            fprintf(stderr, "Start coordinate is too large.  Max decimal digits allowed is %ld in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_DEC_INTEGERS, lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    else if(0 == dptr - cptr)
                        {
                            fprintf(stderr, "Consecutive tabs and/or spaces between chromosome and start coordinate.  See line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    memcpy(tmpArr, cptr, static_cast<size_t>(dptr-cptr));
                    tmpArr[dptr-cptr] = '\0';
                    for(k=0; k < dptr-cptr; ++k)
                        {
                            if(!isdigit(tmpArr[k]))
                                {
                                    fprintf(stderr, "Non-numeric start coordinate.  See line %" PRIu64 " in %s.\n(remember that chromosome names should not contain spaces.)\n",
                                            lines, bedFileNames[i]);
                                    fclose(bedFile);
                                    return -1;
                                }
                        } /* for */
                    if(atof(tmpArr) > double(Bed::MAX_COORD_VALUE))
                        {
                            fprintf(stderr, "Start coordinate is too large.  Max allowed value is %" PRIu64 " in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_COORD_VALUE, lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    sscanf(tmpArr, "%" SCNd64, &startPos);

                    /* end coord check */
                    cptr = strpbrk(++dptr, "\t ");
                    if(cptr == NULL)
                        { /* eol */
                            cptr = strchr(dptr, '\n');
                            if(cptr == NULL)
                                {
                                    fprintf(stderr, "No end of line found at %" PRIu64 " in %s.\n",
                                            lines, bedFileNames[i]);
                                    fclose(bedFile);
                                    return -1 ;
                                }
                        }
                    if(cptr - dptr > (double)Bed::MAX_DEC_INTEGERS)
                        {
                            fprintf(stderr, "End coordinate is too large.  Max decimal digits allowed is %ld in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_DEC_INTEGERS, lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    else if(cptr == dptr)
                        {
                            fprintf(stderr, "Extra tab and/or space found in between start and end coordinates.  See line %" PRIu64 " in %s.\n",
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    memcpy(tmpArr, dptr, static_cast<size_t>(cptr-dptr));
                    tmpArr[cptr-dptr] = '\0';
                    for(k=0; k < cptr-dptr; ++k)
                        {
                            if(!isdigit(tmpArr[k]))
                                {
                                    fprintf(stderr, "Non-numeric end coordinate.  See line %" PRIu64 " in %s.\n",
                                            lines, bedFileNames[i]);
                                    fclose(bedFile);
                                    return -1;
                                }
                        } /* for */
                    if(atof(tmpArr) > double(Bed::MAX_COORD_VALUE))
                        {
                            fprintf(stderr, "End coordinate is too large.  Max allowed value is %" PRIu64 " in BEDOPS.Constants.hpp.  See line %" PRIu64 " in %s.\n",
                                    Bed::MAX_COORD_VALUE, lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    sscanf(tmpArr, "%" SCNd64, &endPos);

                    /* rest of the line goes into bedLine */
                    fields = 3 + sscanf(cptr, "\t%[^\n]s\n", bedLine);
                    headCheck = 0;

                    /* Validate Coords */
                    if ((startPos < 0) || (endPos < 0)) 
                        {
                            fprintf(stderr, "Error on line %" PRIu64 " in %s. Genomic position must be greater than 0.\n", 
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }
                    if (endPos <= startPos)
                        {
                            fprintf(stderr, "Error on line %" PRIu64 " in %s. Genomic end coordinate is less than (or equal to) start coordinate.\n", 
                                    lines, bedFileNames[i]);
                            fclose(bedFile);
                            return -1;
                        }

                    /*Find the chrom*/
                    newChrom = 1;
                    for(j = 0; j < beds->numChroms; j++) 
                        {
                            if(strcmp(beds->chroms[j]->chromName, chromBuf) == 0) 
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
                                                                    lines, bedFileNames[i]);
                                                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                                            fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                                            fclose(bedFile);
                                                            return -1;
                                                        }
                                                }
                                            else if(cptr - bedLine > (double)ID_NAME_LEN)
                                                {
                                                    fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                            lines, bedFileNames[i]);
                                                    fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                                    fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                                    fclose(bedFile);
                                                    return -1;
                                                }
                                            chromEntryCount = appendChromBedEntry(beds->chroms[j], startPos, endPos, bedLine, &totalBytes);
                                        }
                                    else
                                        {
                                            chromEntryCount = appendChromBedEntry(beds->chroms[j], startPos, endPos, NULL, &totalBytes);
                                        }

                                    diffBytes = totalBytes - diffBytes;
                                    *chromBytes[j] += diffBytes;
                                    maxChromBytes = (*chromBytes[j] < maxChromBytes) ? maxChromBytes : *chromBytes[j]; 

                                    if (static_cast<int>(chromEntryCount) < 0)
                                        {
                                            fprintf(stderr, "Error: %s, %d: Unable to create BED structure.\n", __FILE__, __LINE__);
                                            fclose(bedFile);
                                            return -1;
                                        }
                                    newChrom = 0;
                                    break;
                                }
                        } /* for */

                    if((newChrom) || (beds->numChroms == 0)) 
                        {
                            errno = 0;

                            /* Create a new chrom */

                            /* Resize Chrom Structure */
                            if(beds->numChroms >= (double)((NUM_CHROM_EST * chromAllocs) - 1))
                                {
                                    /* fprintf(stderr, "Reallocating...\n"); */
                                    chromAllocs++;
                                    beds->chroms = (ChromBedData**)realloc(beds->chroms, sizeof(ChromBedData *) * NUM_CHROM_EST * chromAllocs);
                                    totalBytes += sizeof(ChromBedData *) * NUM_CHROM_EST * chromAllocs;
                                    if(beds->chroms == NULL)
                                        {
                                            fprintf(stderr, "Error: %s, %d: Unable to expand Chrom structure: %s. Out of memory.\n", __FILE__, 
                                                    __LINE__, strerror(errno));
                                            fclose(bedFile);
                                            return -1;
                                        }
                                }

                            diffBytes = totalBytes;
                            chrom = initializeChromBedData(chromBuf, &totalBytes);
                            if(chrom == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create Chrom structure: %s. Out of memory.\n", __FILE__, 
                                            __LINE__, strerror(errno));
                                    fclose(bedFile);
                                    return -1;
                                }
                            diffBytes = totalBytes - diffBytes;
                            chromBytes = (double**)realloc(chromBytes, sizeof(double *) * (beds->numChroms + 1));
                            if(chromBytes == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create double* array. Out of memory.\n", __FILE__, __LINE__);
                                    return -1;
                                }
                            chromBytes[beds->numChroms] = (double*)malloc(sizeof(double));
                            if(chromBytes[beds->numChroms] == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create double. Out of memory.\n", __FILE__, __LINE__);
                                    return -1;
                                }
                            *chromBytes[beds->numChroms] = diffBytes;
                            totalBytes += sizeof(double *) * (beds->numChroms + 1) + sizeof(double);

                            diffBytes = totalBytes;
                            if(fields > 3)
                                { /* check ID column is <= ID_NAME_LEN for the benefit of downstream programs */
                                    cptr = strpbrk(bedLine, "\t "); /* bedops/bedmap do not differentiate these whitespace characters */
                                    if(cptr == NULL)
                                        {
                                            if(strlen(bedLine) > ID_NAME_LEN)
                                                {
                                                    fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                            lines, bedFileNames[i]);
                                                    fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                                    fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                                    fclose(bedFile);
                                                    return -1;
                                                }
                                        }
                                    else if(cptr - bedLine > (double)ID_NAME_LEN)
                                        {
                                            fprintf(stderr, "ID field too long at line %" PRIu64 " in %s.\n",
                                                    lines, bedFileNames[i]);
                                            fprintf(stderr, "Check that you have unix newlines (cat -A) or increase TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                                            fprintf(stderr, "You may instead choose to put a dummy id column (like 'id') in as the 4th field to fix this.\n");
                                            fclose(bedFile);
                                            return -1;
                                        }
                                    chromEntryCount = appendChromBedEntry(chrom, startPos, endPos, bedLine, &totalBytes);
                                }
                            else
                                {
                                    chromEntryCount = appendChromBedEntry(chrom, startPos, endPos, NULL, &totalBytes);
                                }

                            diffBytes = totalBytes - diffBytes;
                            *chromBytes[beds->numChroms] += diffBytes;
                            maxChromBytes = (*chromBytes[beds->numChroms] < maxChromBytes) ? maxChromBytes : *chromBytes[beds->numChroms];

                            if(static_cast<int>(chromEntryCount) < 0) 
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure.\n", __FILE__, __LINE__);
                                    fclose(bedFile);
                                    return -1;
                                }

                            beds->chroms[beds->numChroms] = chrom;
                            beds->numChroms++;
                        }

                    /* worst case quicksort memory is O(2*n),
                       yet we sort by a single chrom at a time and totalBytes already
                       accounts for 1*n maxChromBytes.  totalBytes is a conservative
                       measure of memory used.
                    */
                    if(maxMem > 0 && (totalBytes + maxChromBytes >= maxMem))
                        {
                            errno = 0;
                            tmpFiles = (FILE**)realloc(tmpFiles, sizeof(FILE*) * (tmpFileCount+1));
                            if(tmpFiles == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create FILE* array: %s. Out of memory.\n", __FILE__, 
                                            __LINE__, strerror(errno));
                                    fclose(bedFile);
                                    return -1;
                                }
                            totalBytes += sizeof(FILE*) * (tmpFileCount+1);
                            tmpFiles[tmpFileCount] = tmpfile();
                            lexSortBedData(beds);
                            printBed(beds, tmpFiles[tmpFileCount]);
                            ++tmpFileCount;
                            for(i = 0; i < beds->numChroms; ++i)
                                free(chromBytes[i]);
                            free(chromBytes);

                            freeBedData(beds);
                            chromBytes = (double**)malloc(sizeof(double *));
                            if(chromBytes == NULL)
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create double* array. Out of memory.\n", __FILE__, __LINE__);
                                    return -1;
                                }
                            maxChromBytes = 0;

                            totalBytes = overhead; /* already includes chromBytes array */
                            beds = initializeBedData(&totalBytes);
                            if(beds == NULL) 
                                {
                                    fprintf(stderr, "Error: %s, %d: Unable to create BED structure. Out of memory.\n", __FILE__, __LINE__);
                                    return -1;
                                }
                        }
                    bedLine[BED_LINE_LEN] = '1';
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
                    tmpFiles = (FILE**)realloc(tmpFiles, sizeof(FILE*) * (tmpFileCount+1));
                    totalBytes += sizeof(FILE*) * (tmpFileCount+1);
                    if(tmpFiles == NULL)
                        {
                            fprintf(stderr, "Error: %s, %d: Unable to expand Chrom structure: %s. Out of memory.\n", __FILE__, 
                                    __LINE__, strerror(errno));
                            fclose(bedFile);
                            return -1;
                        }
                    tmpFiles[tmpFileCount] = tmpfile();
                    lexSortBedData(beds);
                    printBed(beds, tmpFiles[tmpFileCount]);
                    ++tmpFileCount;
                    for(i = 0; i < beds->numChroms; ++i)
                        free(chromBytes[i]);
                    free(chromBytes);
                    freeBedData(beds);
                }
            if(0 != mergeSort(tmpFiles, tmpFileCount))
                {
                    fprintf(stderr, "Error: %s, %d.  Out of memory.\n", __FILE__, __LINE__);
                    fclose(bedFile);
                    return -1;
                }
            for (i = 0; i < tmpFileCount; ++i)
                fclose(tmpFiles[i]); /* deletes temporary files for us */
        }
    else
        {
            lexSortBedData(beds);
            printBed(beds, stdout);
            for(i = 0; i < beds->numChroms; ++i)
                free(chromBytes[i]);
            free(chromBytes);
            /* freeBedData(beds); let the OS clean up - takes significant time to do this step manually */
        }
    free(tmpFiles);
    return 0;
}

void 
printBed(BedData *beds, FILE *out) 
{
    unsigned int i = 0U;
    Bed::LineCountType j = 0;

    if(beds == NULL) 
        return;

    for(i = 0; i < beds->numChroms; i++) 
        for(j = 0; j < beds->chroms[i]->numCoords; j++) 
            {
                fprintf(out, "%s\t%" PRId64 "\t%" PRId64, beds->chroms[i]->chromName, beds->chroms[i]->coords[j].startCoord, 
                        beds->chroms[i]->coords[j].endCoord);
                if(beds->chroms[i]->coords[j].data)
                    fprintf(out, "\t%s\n", beds->chroms[i]->coords[j].data);
                else
                    fprintf(out, "\n");
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
            free(beds->chroms[i]);
        }
    free(beds);
}

void
lexSortBedData(BedData *beds)
{
    unsigned int i;

    if(beds == NULL) 
        {
            return;
        }

    /* sort coords */
    for(i = 0; i < beds->numChroms; i++) 
        {            
            qsort(beds->chroms[i]->coords, static_cast<size_t>(beds->chroms[i]->numCoords), sizeof(BedCoordData), numCompareBedData);
        }

    /* sort chroms */
    qsort(beds->chroms, beds->numChroms, sizeof(ChromBedData *), lexCompareBedData); 
    return;

}

int
numCompareBedData(const void *pos1, const void *pos2) 
{
    Bed::SignedCoordType diff = ((BedCoordData *)pos1)->startCoord - ((BedCoordData *)pos2)->startCoord;
    if(diff)
        {
            return (diff > 0) ? 1 : -1;
        }
    diff = ((BedCoordData *)pos1)->endCoord - ((BedCoordData *)pos2)->endCoord;
    return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
}

int 
lexCompareBedData(const void *chrPos1, const void *chrPos2) 
{
    return(strcmp((*((ChromBedData **)chrPos1))->chromName, (*((ChromBedData **)chrPos2))->chromName));
}
