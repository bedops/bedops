/*
  FILE: Bed.h
  AUTHOR: Scott Kuehn
  CREATE DATE: Tue May 16 09:15:54 PDT 2006
  PROJECT: CompBio
*/

#ifndef BED_STRUCTURES_H
#define BED_STRUCTURES_H

#include <cstdio>

#include "suite/BEDOPS.Constants.hpp"

static const unsigned long CHROM_NAME_LEN    = Bed::TOKEN_CHR_MAX_LENGTH;
static const unsigned long ID_NAME_LEN       = Bed::TOKEN_ID_MAX_LENGTH;
static const unsigned long BED_LINE_LEN      = Bed::TOKENS_MAX_LENGTH;
static const unsigned long NUM_BED_ITEMS_EST = 100000;
static const unsigned long NUM_CHROM_EST     = 32;

#define GT(A,B) ((A) > (B) ? 1 : 0)

/* Data Structures */
typedef struct {
    Bed::SignedCoordType startCoord;
    Bed::SignedCoordType endCoord;
    char *data;
} BedCoordData;

typedef struct {
    char chromName[CHROM_NAME_LEN + 1];
    Bed::LineCountType numCoords;
    //BedCoordData coords[MAX_BED_ITEMS];
    BedCoordData *coords;
} ChromBedData;

typedef struct {
    unsigned int numChroms;
    ChromBedData **chroms; // struct is padded on 64-bit OS X system - cf. http://stackoverflow.com/questions/15031061/alignas-for-struct-members-using-clang-c11 for possible portable solution for warning
} BedData;

/* Function Prototypes */
int checkfiles(const char **bedFileNames, unsigned int numFiles);
int mergeSort(FILE **tmpFiles, unsigned int numFiles);
int processData(const char **bedFileNames, unsigned int numFiles, double maxMem);
void printBed(BedData *beds, FILE* out);
void freeBedData(BedData *beds);
void sortBedData(BedData *beds);
void numSortBedData(BedData *beds);
void lexSortBedData(BedData *beds);
Bed::LineCountType appendChromBedEntry(ChromBedData *chrom, Bed::SignedCoordType startPos, Bed::SignedCoordType endPos, char *data, double* bytes);
ChromBedData * initializeChromBedData(char * chromName, double* bytes);
BedData * initializeBedData(double* bytes);
int numCompareBedData(const void *pos1, const void *pos2);
int lexCompareBedData(const void *pos1, const void *pos2);



#endif /* BED_STRUCTURES_H */
