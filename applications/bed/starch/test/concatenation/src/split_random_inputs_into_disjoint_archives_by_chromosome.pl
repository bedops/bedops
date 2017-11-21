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

my $version = "v2.2";

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
                  random_2p2p0_gzip.starch
                  random_2p1p0_gzip.starch);

# this script will randomly pick three archives from the bzip2- and gzip-based starch archive pool
# and pull disjoint sets of per-chromosome elements from these files, respectively. the resulting
# elements are then starch'ed into three archives.

foreach my $archive_type (@archive_types) {
    #print STDERR "making $archive_type archives...\n";
    if ($archive_type eq "bzip2") {
        foreach my $bzip2_fn (@bzip2_fns) {
            my $chr_list = `$binaries_dir/$version/bin/unstarch --list-chr $compr_extr_results_dir/$bzip2_fn`; chomp $chr_list;
            my @chrs = split("\n", $chr_list);
            my @result_fns = ();
            foreach my $chr (@chrs) {
                my $result_fn = "$concat_results_dir/disjoint_by_chr.$bzip2_fn.$chr.starch";
                my $cmd = "$binaries_dir/$version/bin/unstarch $chr $compr_extr_results_dir/$bzip2_fn | $binaries_dir/$version/bin/starch --bzip2 - > $result_fn";
                system ("$cmd") == 0 or die "could not extract $chr from $bzip2_fn\nCMD: $cmd\n";
                push (@result_fns, $result_fn);
            }
            
            my $fn_list = join(" ", @result_fns);
            system ("$binaries_dir/$version/bin/starchcat $fn_list > $concat_results_dir/$bzip2_fn.merge_test") == 0 or die "could not merge chr's from split $bzip2_fn\n";
        }
    }
    if ($archive_type eq "gzip") {
        foreach my $gzip_fn (@gzip_fns) {
            my $chr_list = `$binaries_dir/$version/bin/unstarch --list-chr $compr_extr_results_dir/$gzip_fn`; chomp $chr_list;
            my @chrs = split("\n", $chr_list);
            my @result_fns = ();
            foreach my $chr (@chrs) {
                my $result_fn = "$concat_results_dir/disjoint_by_chr.$gzip_fn.$chr.starch";
                my $cmd = "$binaries_dir/$version/bin/unstarch $chr $compr_extr_results_dir/$gzip_fn | $binaries_dir/$version/bin/starch --gzip - > $result_fn";
                system ("$cmd") == 0 or die "could not extract $chr from $gzip_fn\nCMD: $cmd\n";
                push (@result_fns, $result_fn);
            }
            my $fn_list = join(" ", @result_fns);
            system ("$binaries_dir/$version/bin/starchcat $fn_list > $concat_results_dir/$gzip_fn.merge_test") == 0 or die "could not merge chr's from split $gzip_fn\n";
        }
    }
}
