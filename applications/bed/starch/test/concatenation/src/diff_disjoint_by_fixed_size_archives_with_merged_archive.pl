#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename;
use Data::Dumper;

my $argc = @ARGV;
my $unstarchBin = $ARGV[0];
my $mergedFn = $ARGV[1];
my @disjointFns = ($ARGV[2]);
my $username = (getpwuid($<))[0];
my $tempDir = "/tmp/$username/starch_regression_test/results/concatenation";
my $observedElementsFn = "$tempDir/observedSplitByFixedSizeElements.bed";

my $observedCmd = "$unstarchBin $mergedFn > $observedElementsFn";
#print STDERR "CMD -> $observedCmd\n";
system("$observedCmd") == 0 or die "ERROR: could not extract merged archive data\n";
my @fns = ();
foreach my $disjointFn (@disjointFns) {
    my $tempDisjointFn = "$tempDir/temp_fixed_size_test.".basename($disjointFn).".bed";
    my $tempDisjointCmd = "$unstarchBin $disjointFn > $tempDisjointFn";
    #print STDERR "CMD -> $tempDisjointCmd\n";
    system("$tempDisjointCmd") == 0 or die "ERROR: could not extract disjoint archive data\n";
    push @fns, $tempDisjointFn;
}

my $expectedElementsFn = "$tempDir/expectedSplitByFixedSizeElements.bed";
my $bedToSort = join(" ", @fns);
my $expectedCmd = "sort-bed $bedToSort > $expectedElementsFn";
#print STDERR "CMD -> $expectedCmd\n";
system("$expectedCmd") == 0 or die "ERROR: could not sort extracted disjoint data\n";

my $diffCmd = "diff --brief $observedElementsFn $expectedElementsFn";
#print STDERR "CMD -> $diffCmd\n";
system("$diffCmd") == 0 or die "ERROR: could not diff observed and expected archive data\n";

print STDOUT "[STARCH] no differences found!\n";
