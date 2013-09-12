#!/usr/bin/env perl

use strict;
use warnings;

my $cnt = 0;
while (<>) {
    chomp;
    my ($chr, $start, $stop, $remainder) = split("\t", $_);
    $cnt += ($stop - $start);
}

print STDOUT "$cnt\n";
