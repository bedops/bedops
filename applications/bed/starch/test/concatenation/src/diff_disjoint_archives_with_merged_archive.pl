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
my $observedElementsFn = "$tempDir/observedElements.bed";

my $observedElementsCmd = "$unstarchBin $mergedFn > $observedElementsFn";
#print STDERR "CMD -> $observedElementsCmd\n";
system("$unstarchBin $mergedFn > $observedElementsFn") == 0 or die "ERROR: could not extract merged archive data\n";
foreach my $disjointFn (@disjointFns) {
    my $tempDisjointFn = "$tempDir/disjoint.".basename($disjointFn).".bed";
    my $tempDisjointCmd = "$unstarchBin $disjointFn > $tempDisjointFn";
    #print STDERR "CMD -> $tempDisjointCmd\n";
    system("$tempDisjointCmd") == 0 or die "ERROR: could not extract disjoint archive data\n";
}
my $expectedElementsFn = "$tempDir/expectedElements.bed";
my $expectedElementsCmd = "sort-bed $tempDir/disjoint.*.bed > $expectedElementsFn";
#print STDERR "CMD -> $expectedElementsCmd\n";
system("sort-bed $tempDir/disjoint.*.bed > $expectedElementsFn") == 0 or die "ERROR: could not sort extracted disjoint data\n";

system("diff --brief $observedElementsFn $expectedElementsFn") == 0 or die "ERROR: could not diff observed and expected archive data\n";

print STDOUT "[STARCH] no differences found!\n";
