#!/usr/bin/env perl

use strict;
use warnings;
use Data::Dumper;
use File::Basename;
use File::Path;
use List::Util qw(shuffle);

my $compr_extr_results_dir = $ARGV[0];
my $concat_results_dir     = $ARGV[1];
my $binaries_dir           = $ARGV[2];

my @archive_types = qw(bzip2
                       gzip);

my @version_dirs = qw(v1.2
                      v1.5
                      v2.0
                      v2.1
                      v2.2);

my @bzip2_fns = qw(random_1p2p0_bzip2.starch
                   random_1p5p0_bzip2.starch
                   random_2p0p0_bzip2.starch
                   random_2p1p0_bzip2.starch
                   random_2p2p0_bzip2.starch);

my @gzip_fns = qw(random_1p2p0_gzip.starch
                  random_1p5p0_gzip.starch
                  random_2p0p0_gzip.starch
                  random_2p1p0_gzip.starch
                  random_2p2p0_gzip.starch);

my $version_str = "v2.2";

my $fragment_size = 100000;
my $max_size = 1000000;
my $username = (getpwuid($<))[0];
my $tempDir = "/tmp/$username/starch_regression_test/results/concatenation";

# this script will randomly pick three archives from the bzip2- and gzip-based starch archive pool
# and pull disjoint sets of 100K elements from each, spanning across the 1M element set

foreach my $archive_type (@archive_types) {
    #print STDERR "making $archive_type archive...\n";
    if ($archive_type eq "bzip2") {
        foreach my $bzip2_fn (@bzip2_fns) {
            my $set_count = $max_size / $fragment_size;
            my @out_fns = ();
            foreach my $index (0..($set_count - 1)) {
                my $start = ($index * $fragment_size) + 1;
                my $stop = (($index + 1) * $fragment_size);
                my $out_fn = "$tempDir/fixed_".$bzip2_fn."_".$start."_".$stop.".starch";
                #print Dumper $out_fn;
                my $cmd = "$binaries_dir/$version_str/bin/unstarch $compr_extr_results_dir/$bzip2_fn | sed -n ".$start.",".$stop."p | $binaries_dir/$version_str/bin/starch - > $out_fn";
                #print STDERR "CMD -> $cmd\n";
                system ("$cmd") == 0 or die "could not split $bzip2_fn at boundaries [$start, $stop]\n";
                push @out_fns, $out_fn;
            }
            my $out_str = join(" ", @out_fns);
            #print STDERR "CAT - $out_str\n";
            my $out_final_fn = "$tempDir/merged_fixed_size_".$bzip2_fn;
            my $concat_cmd = "$binaries_dir/$version_str/bin/starchcat $out_str > $out_final_fn";
            `$concat_cmd`;
            system ("mv $out_final_fn $concat_results_dir/fixed_merged_".basename($out_final_fn));
            #print STDERR "BZIP - $concat_results_dir/fixed_merged_".basename($out_final_fn)."\n";
        }
    }
    if ($archive_type eq "gzip") {
        foreach my $gzip_fn (@gzip_fns) {
            my $set_count = $max_size / $fragment_size;
            my @out_fns = ();
            foreach my $index (0..($set_count - 1)) {
                my $start = ($index * $fragment_size) + 1;
                my $stop = (($index + 1) * $fragment_size);
                my $out_fn = "$tempDir/fixed_".$gzip_fn."_".$start."_".$stop.".starch";
		#print Dumper $out_fn;
                my $cmd = "$binaries_dir/$version_str/bin/unstarch $compr_extr_results_dir/$gzip_fn | sed -n ".$start.",".$stop."p | $binaries_dir/$version_str/bin/starch - > $out_fn";
                #print STDERR "CMD -> $cmd\n";
                system ("$cmd") == 0 or die "could not split $gzip_fn at boundaries [$start, $stop]\n";
                push @out_fns, $out_fn;
            }
            my $out_str = join(" ", @out_fns);
            my $out_final_fn = "$tempDir/merged_fixed_size_".$gzip_fn;
            my $concat_cmd = "$binaries_dir/$version_str/bin/starchcat $out_str > $out_final_fn";
            `$concat_cmd`;
            system ("mv $out_final_fn $concat_results_dir/fixed_merged_".basename($out_final_fn));
            #print STDERR "GZIP - $concat_results_dir/fixed_merged_".basename($out_final_fn)."\n";
        }
    }
}
