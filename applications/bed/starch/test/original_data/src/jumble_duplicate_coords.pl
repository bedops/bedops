#!/usr/bin/env perl

use strict;
use warnings;
use Data::Dumper;

#
# input: sorted BED with possible duplicate coordinates
# output: possibly modified BED with nonredundant coordinates
#

my $max_coord = 100000000;

my $input;
while(<>) {
    chomp;
    my ($chr, $start, $stop, $id, $score, $strand) = split("\t", $_);
    my $key = "$chr:$start:$stop";
    my $value = "$_\n";
    if (defined $input->{$key}) {
	my $randomStart = 0;
	my $randomStop = 1;
	my $test_key = $key;
	while (defined $input->{$test_key}) {
	    $randomStart = int(rand($max_coord));
	    $randomStop = $randomStart + int(rand($max_coord));
	    $test_key = "$chr:$randomStart:$randomStop";
	}
	my $new_value = join("\t", ($chr, $randomStart, $randomStop, $id, $score, $strand))."\n";
	$input->{$test_key} = $new_value;
    }
    else {
	$input->{$key} = $value;
    }
}

foreach my $posKey (keys %{$input}) {
    print $input->{$posKey};
}
