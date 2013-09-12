#!/bin/sh
# FILE: testPy1.sh
# AUTHOR: Scott Kuehn, Shane Neph, Alex Reynolds
# CREATE DATE: Tue Mar 12 18:00:00 PDT 2013
# PROJECT: utility

# Test conversions

bindir=../../src

mkdir -p results

$bindir/wig2bed.py t1.wig \
    > results/t1.out
diff -q t1.result results/t1.out
if [ $? != 0 ]; then
    exit 1
fi

$bindir/wig2bed.py t2.wig \
    > results/t2.out
diff -q t2.result results/t2.out
if [ $? != 0 ]; then
    exit 1
fi

$bindir/wig2bed.py t3.wig \
    > results/t3.out
diff -q t3.result results/t3.out
if [ $? != 0 ]; then
    exit 1
fi

$bindir/wig2bed.py t4.wig \
    > results/t4.out
diff -q t4.result results/t4.out
if [ $? != 0 ]; then
    exit 1
fi

$bindir/wig2bed.py t5.wig \
    > results/t5.out
diff -q t5.result results/t5.out
if [ $? != 0 ]; then
    exit 1
fi

echo "tests passed!"

exit 0
