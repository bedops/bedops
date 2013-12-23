#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename;

my $argc = @ARGV;
my $unstarchBin = $ARGV[0];
my $mergedFn = $ARGV[1];
my @disjointFns = @ARGV[2..($argc - 1)];
my $username = (getpwuid($<))[0];
my $tempDir = "/tmp/$username/starch_regression_test/results/concatenation";
my $observedElementsFn = "$tempDir/observedSplitByChromosomeElements.bed";

system("$unstarchBin $mergedFn > $observedElementsFn") == 0 or die "ERROR: could not extract merged archive data\n";
my @fns = ();
foreach my $disjointFn (@disjointFns) {
    my $tempDisjointFn = "$tempDir/disjoint.".basename($disjointFn).".bed";
    system("$unstarchBin $disjointFn > $tempDisjointFn") == 0 or die "ERROR: could not extract disjoint archive data\n";
    push @fns, $tempDisjointFn;
}
my $expectedElementsFn = "$tempDir/expectedSplitByChromosomeElements.bed";
my $bedToSort = join(" ", @fns);
system("sort-bed $bedToSort > $expectedElementsFn") == 0 or die "ERROR: could not sort extracted disjoint data\n";

system("diff --brief $observedElementsFn $expectedElementsFn") == 0 or die "ERROR: could not diff observed and expected archive data\n";

print STDOUT "[STARCH] no differences found!\n";
