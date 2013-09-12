#!/usr/bin/env sh

# FILE: test.sh
# AUTHOR: Alex Reynolds
# CREATE DATE: Wed Mar 13 12:00:00 PDT 2013
# PROJECT: BEDOPS 2

binDir=../../src
gff2BedBin=${binDir}/gff2bed.py
gff2StarchBin=${binDir}/gff2starch.py
resultsDir=results

mkdir -p ${resultsDir}

#
# test unsorted UCSC-genome-browser sourced GFF output, without sorting, against unsorted UCSC-sourced BED
#
${gff2BedBin} --do-not-sort < via_bedops.gff \
    > results/via_bedops.unsorted.out
diff -q via_bedops.unsorted.out results/via_bedops.unsorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted UCSC-genome-browser sourced GFF output, with sorting, against sorted UCSC-sourced BED
#
${gff2BedBin} < via_bedops.gff \
    > results/via_bedops.sorted.out
diff -q via_bedops.sorted.out results/via_bedops.sorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test --max-mem sorting option with lowest memory setting possible
#
${gff2BedBin} --max-mem 500M < via_bedops.gff \
    > results/via_bedops.sorted.500M.out
diff -q via_bedops.sorted.out results/via_bedops.sorted.500M.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test gff2starch default functionality
#
${gff2StarchBin} < via_bedops.gff \
    > results/via_bedops.sorted.bzip2.compressed.out
unstarch via_bedops.sorted.bzip2.compressed.out \
    > via_bedops.sorted.bzip2.compressed.out.uncompressed
unstarch results/via_bedops.sorted.bzip2.compressed.out \
    | diff -q via_bedops.sorted.bzip2.compressed.out.uncompressed -
if [ $? != 0 ]; then
    rm via_bedops.sorted.bzip2.compressed.out.uncompressed
    exit 1
fi

#
# test gff2starch gzip functionality
#
${gff2StarchBin} --starch-format gzip < via_bedops.gff \
    > results/via_bedops.sorted.gzip.compressed.out
unstarch via_bedops.sorted.gzip.compressed.out \
    > via_bedops.sorted.gzip.compressed.out.uncompressed
unstarch results/via_bedops.sorted.gzip.compressed.out \
    | diff -q via_bedops.sorted.gzip.compressed.out.uncompressed -
if [ $? != 0 ]; then
    rm via_bedops.sorted.gzip.compressed.out.uncompressed
    exit 1
fi

#
# cleanup
#
rm -R ${resultsDir}

echo "tests passed!"

exit 0
