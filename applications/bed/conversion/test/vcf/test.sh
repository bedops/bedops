#!/usr/bin/env sh

# FILE: test.sh
# AUTHOR: Alex Reynolds
# CREATE DATE: Wed Mar 13 12:00:00 PDT 2013
# PROJECT: BEDOPS 2

binDir=../../src
vcf2BedBin=${binDir}/vcf2bed.py
vcf2StarchBin=${binDir}/vcf2starch.py
resultsDir=results

mkdir -p ${resultsDir}

#
# test unsorted 1000genomes-sourced VCF output, without sorting, against unsorted BED
#
${vcf2BedBin} --do-not-sort < via_1000genomes.vcf \
    > results/via_1000genomes.unsorted.out 2> /dev/null
diff -q via_1000genomes.unsorted.out results/via_1000genomes.unsorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted 1000genomes-sourced VCF output, without sorting, against unsorted BED
#
${vcf2BedBin} < via_1000genomes.vcf \
    > results/via_1000genomes.sorted.out 2> /dev/null
diff -q via_1000genomes.sorted.out results/via_1000genomes.sorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted 1000genomes-sourced VCF output, without sorting, compressed (Starch bzip2) against unsorted, compressed BED (Starch bzip2)
#
${vcf2StarchBin} --do-not-sort --starch-format bzip2 \
    < via_1000genomes.vcf \
    > results/via_1000genomes.unsorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_1000genomes.unsorted.bzip2.compressed.out \
    > results/via_1000genomes.unsorted.bzip2.compressed.out.uncompressed
unstarch via_1000genomes.unsorted.bzip2.compressed.out \
    | diff -q - results/via_1000genomes.unsorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted 1000genomes-sourced VCF output, compressed (Starch bzip2) against sorted, compressed BED (Starch bzip2)
#
${vcf2StarchBin} --starch-format bzip2 \
    < via_1000genomes.vcf \
    > results/via_1000genomes.sorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_1000genomes.sorted.bzip2.compressed.out \
    > results/via_1000genomes.sorted.bzip2.compressed.out.uncompressed
unstarch via_1000genomes.sorted.bzip2.compressed.out \
    | diff -q - results/via_1000genomes.sorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted 1000genomes-sourced VCF output, without sorting, compressed (Starch gzip) against unsorted, compressed BED (Starch gzip)
#
${vcf2StarchBin} --do-not-sort --starch-format gzip \
    < via_1000genomes.vcf \
    > results/via_1000genomes.unsorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_1000genomes.unsorted.gzip.compressed.out \
    > results/via_1000genomes.unsorted.gzip.compressed.out.uncompressed
unstarch via_1000genomes.unsorted.gzip.compressed.out \
    | diff -q - results/via_1000genomes.unsorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted 1000genomes-sourced VCF output, compressed (Starch gzip) against sorted, compressed BED (Starch gzip)
#
${vcf2StarchBin} --starch-format gzip \
    < via_1000genomes.vcf \
    > results/via_1000genomes.sorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_1000genomes.sorted.gzip.compressed.out \
    > results/via_1000genomes.sorted.gzip.compressed.out.uncompressed
unstarch via_1000genomes.sorted.gzip.compressed.out \
    | diff -q - results/via_1000genomes.sorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# cleanup
#

rm -R ${resultsDir}

echo "tests passed!"

exit 0
