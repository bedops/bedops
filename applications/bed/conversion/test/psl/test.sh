#!/usr/bin/env sh

# FILE: test.sh
# AUTHOR: Alex Reynolds
# CREATE DATE: Wed Mar 13 12:00:00 PDT 2013
# PROJECT: BEDOPS 2

binDir=../../src
psl2BedBin=${binDir}/psl2bed.py
psl2StarchBin=${binDir}/psl2starch.py
resultsDir=results

mkdir -p ${resultsDir}

#
# test unsorted, headerless blat-sourced PSL output, without sorting, against unsorted BED
#
${psl2BedBin} --do-not-sort < via_blat.headerless.psl \
    > results/via_blat.headerless.unsorted.out 2> /dev/null
diff -q via_blat.headerless.unsorted.out results/via_blat.headerless.unsorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted, headerless blat-sourced PSL output, without sorting, against unsorted BED
#
${psl2BedBin} < via_blat.headerless.psl \
    > results/via_blat.headerless.sorted.out 2> /dev/null
diff -q via_blat.headerless.sorted.out results/via_blat.headerless.sorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted, headerless blat-sourced PSL output, without sorting, compressed (Starch bzip2) against unsorted, compressed BED (Starch bzip2)
#
${psl2StarchBin} --do-not-sort --starch-format bzip2 \
    < via_blat.headerless.psl \
    > results/via_blat.headerless.unsorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_blat.headerless.unsorted.bzip2.compressed.out \
    > results/via_blat.headerless.unsorted.bzip2.compressed.out.uncompressed
unstarch via_blat.headerless.unsorted.bzip2.compressed.out \
    | diff -q - results/via_blat.headerless.unsorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted, headerless blat-sourced PSL output, compressed (Starch bzip2) against sorted, compressed BED (Starch bzip2)
#
${psl2StarchBin} --starch-format bzip2 \
    < via_blat.headerless.psl \
    > results/via_blat.headerless.sorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_blat.headerless.sorted.bzip2.compressed.out \
    > results/via_blat.headerless.sorted.bzip2.compressed.out.uncompressed
unstarch via_blat.headerless.sorted.bzip2.compressed.out \
    | diff -q - results/via_blat.headerless.sorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted, headerless blat-sourced PSL output, without sorting, compressed (Starch gzip) against unsorted, compressed BED (Starch gzip)
#
${psl2StarchBin} --do-not-sort --starch-format gzip \
    < via_blat.headerless.psl \
    > results/via_blat.headerless.unsorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_blat.headerless.unsorted.gzip.compressed.out \
    > results/via_blat.headerless.unsorted.gzip.compressed.out.uncompressed
unstarch via_blat.headerless.unsorted.gzip.compressed.out \
    | diff -q - results/via_blat.headerless.unsorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted, headerless blat-sourced PSL output, compressed (Starch gzip) against sorted, compressed BED (Starch gzip)
#
${psl2StarchBin} --starch-format gzip \
    < via_blat.headerless.psl \
    > results/via_blat.headerless.sorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_blat.headerless.sorted.gzip.compressed.out \
    > results/via_blat.headerless.sorted.gzip.compressed.out.uncompressed
unstarch via_blat.headerless.sorted.gzip.compressed.out \
    | diff -q - results/via_blat.headerless.sorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted, headered blat-sourced PSL output, without sorting, against unsorted BED
#
${psl2BedBin} --do-not-sort --headered < via_blat.headered.psl \
    > results/via_blat.headered.unsorted.out 2> /dev/null
diff -q via_blat.headered.unsorted.out results/via_blat.headered.unsorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted, headered blat-sourced PSL output, without sorting, against unsorted BED
#
${psl2BedBin} --headered < via_blat.headered.psl \
    > results/via_blat.headered.sorted.out 2> /dev/null
diff -q via_blat.headered.sorted.out results/via_blat.headered.sorted.out
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted, headered blat-sourced PSL output, without sorting, compressed (Starch bzip2) against unsorted, compressed BED (Starch bzip2)
#
${psl2StarchBin} --do-not-sort --starch-format bzip2 --headered \
    < via_blat.headered.psl \
    > results/via_blat.headered.unsorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_blat.headered.unsorted.bzip2.compressed.out \
    > results/via_blat.headered.unsorted.bzip2.compressed.out.uncompressed
unstarch via_blat.headered.unsorted.bzip2.compressed.out \
    | diff -q - results/via_blat.headered.unsorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted, headered blat-sourced PSL output, compressed (Starch bzip2) against sorted, compressed BED (Starch bzip2)
#
${psl2StarchBin} --starch-format bzip2 --headered \
    < via_blat.headered.psl \
    > results/via_blat.headered.sorted.bzip2.compressed.out \
    2> /dev/null
unstarch results/via_blat.headered.sorted.bzip2.compressed.out \
    > results/via_blat.headered.sorted.bzip2.compressed.out.uncompressed
unstarch via_blat.headered.sorted.bzip2.compressed.out \
    | diff -q - results/via_blat.headered.sorted.bzip2.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test unsorted, headered blat-sourced PSL output, without sorting, compressed (Starch gzip) against unsorted, compressed BED (Starch gzip)
#
${psl2StarchBin} --do-not-sort --starch-format gzip --headered \
    < via_blat.headered.psl \
    > results/via_blat.headered.unsorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_blat.headered.unsorted.gzip.compressed.out \
    > results/via_blat.headered.unsorted.gzip.compressed.out.uncompressed
unstarch via_blat.headered.unsorted.gzip.compressed.out \
    | diff -q - results/via_blat.headered.unsorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# test sorted, headered blat-sourced PSL output, compressed (Starch gzip) against sorted, compressed BED (Starch gzip)
#
${psl2StarchBin} --starch-format gzip --headered \
    < via_blat.headered.psl \
    > results/via_blat.headered.sorted.gzip.compressed.out \
    2> /dev/null
unstarch results/via_blat.headered.sorted.gzip.compressed.out \
    > results/via_blat.headered.sorted.gzip.compressed.out.uncompressed
unstarch via_blat.headered.sorted.gzip.compressed.out \
    | diff -q - results/via_blat.headered.sorted.gzip.compressed.out.uncompressed
if [ $? != 0 ]; then
    exit 1
fi

#
# cleanup
#

rm -R ${resultsDir}

echo "tests passed!"

exit 0
