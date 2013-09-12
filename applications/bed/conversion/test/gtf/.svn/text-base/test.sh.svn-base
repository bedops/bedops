#!/usr/bin/env sh

# FILE: test.sh
# AUTHOR: Alex Reynolds
# CREATE DATE: Wed Mar 13 12:00:00 PDT 2013
# PROJECT: BEDOPS 2

binDir=../../src
resultsDir=results

mkdir -p ${resultsDir}

#
# test unsorted UCSC-genome-browser sourced GTF output, without sorting, against unsorted UCSC-sourced BED
#
${binDir}/gtf2bed.py --do-not-sort < t1.gtf \
    > results/t1.out
diff -q t1.result results/t1.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted UCSC-genome-browser sourced GTF output, with sorting, against sorted UCSC-sourced BED
#
${binDir}/gtf2bed.py < t2.gtf \
    > results/t2.out
sort-bed t2.result | diff -q - results/t2.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test --max-mem sorting option with lowest memory setting possible
#
${binDir}/gtf2bed.py --max-mem 500M < t3.gtf \
    > results/t3.out
diff -q t3.result results/t3.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test gtf2starch default functionality
#
${binDir}/gtf2starch.py < t4.gtf \
    > results/t4.out
unstarch t4.bzip2.result \
    > t4.bzip2.result.bed
unstarch results/t4.out | diff -q t4.bzip2.result.bed -
if [ $? != 0 ]; then
    rm t4.bzip2.result.bed
    exit 1
fi
rm t4.bzip2.result.bed

#
# test gtf2starch gzip functionality
#
${binDir}/gtf2starch.py --starch-format gzip < t5.gtf \
    > results/t5.out
unstarch t5.gzip.result \
    > t5.gzip.result.bed
unstarch results/t5.out | diff -q t5.gzip.result.bed -
if [ $? != 0 ]; then
    rm t5.gzip.result.bed
    exit 1
fi
rm t5.gzip.result.bed

rm -R ${resultsDir}

echo "tests passed!"

exit 0

