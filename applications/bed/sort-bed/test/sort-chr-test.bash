#!/bin/bash

# 
# Oct 21 2013
# Alex Reynolds
#
# This script creates BED files containing a simple genomic coordinate pair
# for each of one to one hundred thousand chromosomes, and then it sorts that 
# file.
#
# Because the result of a numerical sort (which comes out of this script by 
# default) is different from the lexicographical sort out of sort-bed, this 
# helps test how sort-bed handles BED data with large numbers of chromosomes.
#

CHROM_START=1
CHROM_END=10000
TEST_FN=.test.bed

rm -f ${TEST_FN}
    
for ((idx = CHROM_START; idx <= CHROM_END; idx++))
do
    echo "testing sorting file with chrs ${CHROM_START}-${idx}..." > /dev/stderr
    echo -e "chr${idx}\t1\t2" >> ${TEST_FN}
    ../bin/sort-bed --max-mem 2G ${TEST_FN} > /dev/null
    if [ $? -ne 0 ]
    then
        echo "failed on chr count [ ${idx} ]"
        exit -1
    fi
done

rm -f ${TEST_FN}

echo "success!"
exit 0
