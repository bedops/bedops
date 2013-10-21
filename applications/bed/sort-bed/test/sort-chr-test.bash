#!/bin/bash

# 
# Oct 21 2013
# Alex Reynolds
#
# This script creates BED files containing a simple genomic coordinate pair
# for each of one to one million chromosomes, and then it sorts that file.
#
# Because the result of a numerical sort (which comes out of this script by 
# default) is different from the lexicographical sort out of sort-bed, this 
# helps test how sort-bed handles BED data with large numbers of chromosomes.
#
# While it is inefficient to recreate a test file on each loop, this is done
# to ensure an early exit does not leave a whole bunch of test files around.
#

CHROM_START=1
CHROM_END=1024000

for ((i = CHROM_START; i <= CHROM_END; i++))
do
    echo "testing sorting file with chrs ${CHROM_START}-${i}..." > /dev/stderr
    for ((j = CHROM_START; j <= CHROM_END; j++))
    do
        if [ ${i} -ge ${j} ]
        then
            echo -e "chr${j}\t1\t2" >> .test_chr${i}.bed
        else
	    break
	fi
    done
    ../bin/sort-bed --max-mem 2G .test_chr${i}.bed > /dev/null
    if [ $? -ne 0 ]
    then
        echo "failed on chr count [ ${i} ]"
        exit -1
    else
        rm .test_chr${i}.bed
    fi
done

echo "success!"
exit 0
