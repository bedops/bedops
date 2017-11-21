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

my @elem_counts = qw(200000
                     200000
                     200000
                     200000
                     200000);

my $version_str = "v2.2";

# this script will randomly pick five archives from the bzip2- and gzip-based starch archive pool
# and pull disjoint sets of 200K elements from these files, respectively. the resulting elements 
# are then starch'ed into archives.

foreach my $archive_type (@archive_types) {
    #print STDERR "making $archive_type archive...\n";
    # total of ten files
    my @fn_indices = (0..9); 
    my @shuffled_indices = shuffle (@fn_indices);
    # we grab the first five indices
    my @inds = @shuffled_indices[0..4];
    my @fns = ();
    undef @fns;
    # grab either a bzip2 or gzip archive at random
    foreach my $ind (@inds) {
        if ($ind < 5) {
            push (@fns, $bzip2_fns[$ind]);
        }
        else {
            push (@fns, $gzip_fns[$ind - 5]);
        }
    }
    my @select_inds = (0..4);
    my $total_line_count = 0;
    foreach my $ind (@select_inds) {
        my $real_line_count = 0;
        my $current_line_count = $total_line_count;
        my $elem_count = $elem_counts[$ind];
        $total_line_count += $elem_count;
        # extract
        #print STDERR "extracting $elem_count elements from $fns[$ind] | $real_line_count | $current_line_count | $elem_count | $total_line_count\n";
        my $unstarch_binary = "$binaries_dir/$version_str/bin/unstarch";
        open ARCH, "$unstarch_binary $compr_extr_results_dir/$fns[$ind] |" or die "could not unstarch: $compr_extr_results_dir/$fns[$ind]\n";
        my $intermediate_out_fn = "$concat_results_dir/version_test_"."$archive_type"."_"."$current_line_count-$total_line_count.bed";        
        open OUT, "> $intermediate_out_fn" or die "could not open handle to intermediate output: $intermediate_out_fn\n";
        while (<ARCH>) {
            if ($real_line_count == $total_line_count) {
                next;
            }
            if (($real_line_count >= $current_line_count) && ($real_line_count < $total_line_count)) {
                print OUT $_;
            }
            $real_line_count++;
        }
        close OUT;
        close ARCH;
        # compress
        my $starch_binary = "$binaries_dir/$version_dirs[$ind]/bin/starch";
        #print STDERR "starch binary: $starch_binary\n";
        my $final_out_fn = "$intermediate_out_fn.starch";
        system ("$starch_binary --$archive_type $intermediate_out_fn > $final_out_fn") == 0 or die "could not create final starch output!\n$?\n";
        # cleanup
        system ("rm $intermediate_out_fn") == 0 or die "could not delete intermediate file: $intermediate_out_fn\n$?\n";
    }
}
