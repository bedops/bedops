#!/usr/bin/env perl

use strict;
use warnings;
use Data::Dumper;

my $cnt = 0;
my $line = -1;
my ($currChr, $currStart, $currStop, $currRemainder, $prevChr, $prevStart, $prevStop, $prevRemainder);
while (<>) {
    chomp;
    ($currChr, $currStart, $currStop, $currRemainder) = split("\t", $_);
    $line++;
    if ($line == 0) {
        $prevChr = $currChr;
        $prevStop = $currStop;
        $cnt = $currStop - $currStart;
        next;
    }
    else {
        if ($currChr eq $prevChr) {
            if ($prevStop <= $currStart) {
                $cnt += ($currStop - $currStart);
            }
            elsif ($prevStop < $currStop) {
                $cnt += ($currStop - $prevStop);
            }
            if ($currStop > $prevStop) {
                $prevStop = $currStop;
            }
        }
        else {
            $cnt += ($currStop - $currStart);
            $prevChr = $currChr;
            $prevStop = $currStop;
        }
    }
}

print STDOUT "$cnt\n";
