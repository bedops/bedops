#!/usr/bin/env perl

use strict;
use warnings;

my $argc = @ARGV;
my $unstarchBin = $ARGV[0];
my $mergedFn = $ARGV[1];
my @disjointFns = @ARGV[2..($argc - 1)];

my $mergedElementCount = `$unstarchBin --elements $mergedFn`; chomp $mergedElementCount;
my $disjointElementCount = 0;
foreach my $disjointFn (@disjointFns) {
    # we do not assume that the disjoint archives are starch v2 (i.e. that they support --elements operator)
    my $disjointFnElementCount = `$unstarchBin $disjointFn | wc -l`; chomp $disjointFnElementCount; 
    $disjointElementCount += $disjointFnElementCount;
}

if ($mergedElementCount != $disjointElementCount) {
    die "ERROR: Merged and summed disjoint element counts do not match!";
}

print STDOUT "[STARCH] no differences found between element counts!\n";
