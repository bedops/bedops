#!/usr/bin/env sh

# FILE: test.sh
# AUTHOR: Alex Reynolds
# CREATE DATE: Wed Mar 13 12:00:00 PDT 2013
# PROJECT: BEDOPS 2

binDir=../../src
sam2BedBin=${binDir}/sam2bed.bash
sam2StarchBin=${binDir}/sam2starch.bash
resultsDir=results

mkdir -p ${resultsDir}

#
# test unsorted bowtie-sourced SAM output, without sorting, against unsorted BED
#
${sam2BedBin} --do-not-sort < via_bowtie.sam \
    > results/via_bowtie.unsorted.out 2> /dev/null
diff -q via_bowtie.unsorted.out results/via_bowtie.unsorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted bowtie-sourced SAM output, without sorting, against unsorted BED
#
${sam2BedBin} < via_bowtie.sam \
    > results/via_bowtie.sorted.out 2> /dev/null
diff -q via_bowtie.sorted.out results/via_bowtie.sorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted bowtie-sourced SAM output, without sorting, compressed (Starch bzip2) against unsorted, compressed BED (Starch bzip2)
#
${sam2StarchBin} --do-not-sort --starch-format bzip2 \
    < via_bowtie.sam \
    > results/via_bowtie.unsorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_bowtie.unsorted.bzip2.compressed.out \
    > results/via_bowtie.unsorted.bzip2.compressed.out.uncompressed
unstarch via_bowtie.unsorted.bzip2.compressed.out \
    | diff -q - results/via_bowtie.unsorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted bowtie-sourced SAM output, compressed (Starch bzip2) against sorted, compressed BED (Starch bzip2)
#
${sam2StarchBin} --starch-format bzip2 \
    < via_bowtie.sam \
    > results/via_bowtie.sorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_bowtie.sorted.bzip2.compressed.out \
    > results/via_bowtie.sorted.bzip2.compressed.out.uncompressed
unstarch via_bowtie.sorted.bzip2.compressed.out \
    | diff -q - results/via_bowtie.sorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted bowtie-sourced SAM output, without sorting, compressed (Starch gzip) against unsorted, compressed BED (Starch gzip)
#
${sam2StarchBin} --do-not-sort --starch-format gzip \
    < via_bowtie.sam \
    > results/via_bowtie.unsorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_bowtie.unsorted.gzip.compressed.out \
    > results/via_bowtie.unsorted.gzip.compressed.out.uncompressed
unstarch via_bowtie.unsorted.gzip.compressed.out \
    | diff -q - results/via_bowtie.unsorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted bowtie-sourced SAM output, compressed (Starch gzip) against sorted, compressed BED (Starch gzip)
#
${sam2StarchBin} --starch-format gzip \
    < via_bowtie.sam \
    > results/via_bowtie.sorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_bowtie.sorted.gzip.compressed.out \
    > results/via_bowtie.sorted.gzip.compressed.out.uncompressed
unstarch via_bowtie.sorted.gzip.compressed.out \
    | diff -q - results/via_bowtie.sorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# cleanup
#

rm -R ${resultsDir}

echo "tests passed!"

exit 0
