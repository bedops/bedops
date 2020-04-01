/*
  Author: Scott Kuehn
   Mods:  Shane Neph
  Date:   Tue May 16 09:15:54 PDT 2006
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

#ifndef BED_STRUCTURES_H
#define BED_STRUCTURES_H

#include <cstdio>

#include "suite/BEDOPS.Constants.hpp"

static const unsigned long CHROM_NAME_LEN         = Bed::TOKEN_CHR_MAX_LENGTH;
static const unsigned long ID_NAME_LEN            = Bed::TOKEN_ID_MAX_LENGTH;
static const unsigned long BED_LINE_LEN           = Bed::TOKENS_MAX_LENGTH;
static const unsigned long NUM_BED_ITEMS_EST      = 100000;
static const unsigned long INIT_NUM_BED_ITEMS_EST = 10;
static const unsigned long NUM_CHROM_EST          = 32;

#define GT(A,B) ((A) > (B) ? 1 : 0)

/* Data Structures */
struct BedCoordData;
int bcd_cmp(BedCoordData const& b1, BedCoordData const& b2);

struct BedCoordData {
    Bed::SignedCoordType startCoord;
    Bed::SignedCoordType endCoord;
    char *data;

    inline friend
    int bcd_cmp(BedCoordData const& b1, BedCoordData const& b2)
        {
            Bed::SignedCoordType diff = (b1.startCoord - b2.startCoord);
            if(diff)
                {
                    return (diff > 0) ? 1 : -1;
                }
            diff = (b1.endCoord - b2.endCoord);
            if(diff)
                {
                    return (diff > 0) ? 1 : -1;
                }

            // so far equivalent, check data member for differences
            if(b1.data != NULL)
                {
                    if(b2.data != NULL)
                        {
                            return strcmp(b1.data, b2.data);
                        }
                    else
                        {
                            return 1;
                        }
                }
            else if(b2.data != NULL)
                {
                    return -1;
                }
            return 0;
        }

    inline friend bool operator<(BedCoordData const& b1, BedCoordData const& b2) { return bcd_cmp(b1,b2) < 0; }
    inline friend bool operator>(BedCoordData const& b1, BedCoordData const& b2) { return bcd_cmp(b1,b2) > 0; }
    inline friend bool operator==(BedCoordData const& b1, BedCoordData const& b2) { return bcd_cmp(b1,b2) == 0; }
};

typedef struct {
    char chromName[CHROM_NAME_LEN + 1];
    Bed::LineCountType numCoords;
    BedCoordData *coords;
} ChromBedData;

typedef struct {
    Bed::SignedCoordType numChroms;
    ChromBedData **chroms; // struct is padded on 64-bit OS X system - cf. http://stackoverflow.com/questions/15031061/alignas-for-struct-members-using-clang-c11 for possible portable solution for warning
} BedData;

/* Function Prototypes */
int
checkSort(char const **bedFileNames, unsigned int numFiles);

int
mergeSort(FILE **tmpFiles, unsigned int numFiles);

int
processData(char const **bedFileNames, unsigned int numFiles, double maxMem, char *tmpPath, 
            const bool printUniques, const bool printDuplicates);

void
printBed(FILE *out, BedData *beds, const bool printUniques, const bool printDuplicates);

void
freeBedData(BedData *beds);

void
sortBedData(BedData *beds);

void
numSortBedData(BedData *beds);

void
lexSortBedData(BedData *beds);

Bed::SignedCoordType
appendChromBedEntry(ChromBedData *chrom, Bed::SignedCoordType startPos, Bed::SignedCoordType endPos,
                    char *data, double* bytes, double maxMem);

ChromBedData*
initializeChromBedData(char * chromName, double* bytes);

BedData*
initializeBedData(double* bytes);

int
numCompareBedData(const void *pos1, const void *pos2);

int
lexCompareBedData(const void *pos1, const void *pos2);

#endif /* BED_STRUCTURES_H */
