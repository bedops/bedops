#!/usr/bin/env python

#
#    BEDOPS
#    Copyright (C) 2011-2020 Shane Neph, Scott Kuehn and Alex Reynolds
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from __future__ import absolute_import, division, print_function, unicode_literals

import sys
import os
import argparse
import errno
import subprocess
import json
import logging

name = "starch-diff"
citation = "  citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract"
authors = "  authors:  Alex Reynolds and Shane Neph"
version = "  version:  2.4.39"
usage = "  $ starch-diff [ --chr <chr> ] starch-file-1 starch-file-2 [ starch-file-3 ... ]"
help = """
  The 'starch-diff' utility compares the signatures of two or more specified 
  Starch v2.2+ archives for all chromosomes, or for a specified chromosome.
  
  The typical output of 'starch-diff' is a message indicating that the 
  archives' chromosome(s) are identical or dissimilar, written to the standard
  error stream.
  
  In addition, if all chromosomes are identical, 'starch-diff' exits with a 
  zero status code. Likewise, if any chromosomes are dissimilar, 'starch-diff'
  exits with a non-zero status code. This facilitates testing in a scripting
  environment.
"""

default_chromosome = "all"

def main():
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help', '-h', action='store_true', dest='help')
    parser.add_argument('--chr', '-c', type=str, action="store", default=default_chromosome)
    parser.add_argument('--debug', '-d', action='store_true', dest='debug')
    parser.add_argument('file', type=argparse.FileType('r'), nargs='*')
    args = parser.parse_args()

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
        logger = logging.getLogger(__name__)

    if args.help or len(args.file) < 2:
        sys.stdout.write(name + '\n')
        sys.stdout.write(citation + '\n')
        sys.stdout.write(version + '\n')
        sys.stdout.write(authors + '\n\n')
        sys.stdout.write(usage + '\n')
        sys.stdout.write(help)
        if args.help:
            sys.exit(os.EX_OK)
        else:
            sys.stdout.write("\nERROR: Please specify two or more Starch archives as input\n")
            sys.exit(errno.EINVAL)

    all_tests_pass = True

    selected_chromosome = unicode(args.chr)
    if args.debug: logger.info('Selected chromosome [%s]' % (selected_chromosome))

    archive_paths = list()
    for i in range(0, len(args.file)):
        archive_handle = args.file[i]
        if not os.path.exists(archive_handle.name) or not os.path.isfile(archive_handle.name):
            sys.stdout.write("ERROR: Input [%s] is not a file or does not exist\n" % (archive_handle.name))
            sys.stdout.write("%s\n" % (usage))
            sys.exit(errno.ENOENT)
        archive_paths.append(archive_handle.name)
        if args.debug: logger.info('Appended input file to list [%s]' % (archive_handle.name))        
    num_files = len(args.file)
    if args.debug: logger.info('Number of input files [%d]' % (num_files))
    
    if selected_chromosome == default_chromosome:
        if args.debug: logger.info('Examining all chromosomes from all inputs...')
        all_chromosomes = {}
        for archive_path in archive_paths:
            get_archive_version_cmd_components = [
                'unstarch',
                '--list-json',
                archive_path
            ]
            try:
                get_archive_version_cmd_result = subprocess.check_output(get_archive_version_cmd_components)
            except subprocess.CalledProcessError as err:
                get_archive_version_cmd_result = "ERROR: Command '{}' returned with error (code {}): {}".format(err.cmd, err.returncode, err.output)
                raise
            archive_metadata = json.loads(get_archive_version_cmd_result.decode('utf-8'))
            try:
                archive_version = archive_metadata['archive']['version']
            except KeyError as err:
                sys.stderr.write("ERROR: Could not read archive version from Starch metadata\n")
                raise
            if archive_version['major'] < 2 or (archive_version['major'] == 2 and archive_version['minor'] < 2):
                sys.stderr.write("ERROR: Input [%s] must be a v2.2+ Starch archive -- use 'starchcat' or extract/recompress to update archive\n" % (archive_path))
                sys.exit(errno.EINVAL)
                
            list_chromosome_cmd_components = [
                'unstarch',
                '--list-chr',
                archive_path
            ]
            try:
                list_chromosome_cmd_result = subprocess.check_output(list_chromosome_cmd_components)
            except subprocess.CalledProcessError as err:
                list_chromosome_cmd_result = "ERROR: Command '{}' returned with error (code {}): {}".format(err.cmd, err.returncode, err.output)
                raise
            archive_chromosomes = list_chromosome_cmd_result.rstrip('\n').split('\n')
            for chromosome in archive_chromosomes:
                if chromosome not in all_chromosomes:
                    all_chromosomes[chromosome] = 0
                all_chromosomes[chromosome] += 1

        common_chromosomes = [k for k, v in all_chromosomes.iteritems() if v == num_files]
        common_chromosomes.sort()
        if args.debug: logger.info('Common chromosomes [%s]' % (','.join(common_chromosomes)))

        uncommon_chromosomes = [k for k, v in all_chromosomes.iteritems() if v != num_files]
        uncommon_chromosomes.sort()
        if args.debug: logger.info('Uncommon chromosomes [%s]' % (','.join(uncommon_chromosomes)))        

        if not common_chromosomes:
            sys.stderr.write("ERROR: Inputs share no records with a common chromosome name\n")
            sys.exit(errno.EINVAL)
    else:
        common_chromosomes = [selected_chromosome]

    tests_to_run = {}
    for common_chromosome in common_chromosomes:
        if common_chromosome not in tests_to_run: 
            tests_to_run[common_chromosome] = []

        for idx, archive_path in enumerate(archive_paths):
            get_chromosome_signature_cmd_components = [
                'unstarch',
                common_chromosome,
                '--signature',
                archive_path
            ]
            try:
                get_chromosome_signature_cmd_result = subprocess.check_output(get_chromosome_signature_cmd_components)
            except subprocess.CalledProcessError as err:
                sys.stderr.write("ERROR: Command [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
                sys.exit(errno.EINVAL)

            chromosome_signature = get_chromosome_signature_cmd_result.rstrip('\n')
            if len(chromosome_signature) == 0:
                chromosome_signature = None

            result = {}
            result['archive'] = archive_path
            result['signature'] = chromosome_signature
            tests_to_run[common_chromosome].append(result)
            if args.debug: logger.info('Test to run: [%s] [%s]' % (common_chromosome, str(result)))

    # for a given chromosome, we have a set of objects to test for equality
    for test_chromosome in sorted(tests_to_run.keys()):
        per_chromosome_tests_pass = True
        previous_signature = None
        current_signature = None
        previous_archive = None
        current_archive = None
        none_signature_found = False
        test_list = tests_to_run[test_chromosome]
        for test_item in test_list:
            test_signature = test_item['signature']
            test_archive = test_item['archive']
            if args.debug: logger.info('Examining archive [%s]' % (test_archive))
            if args.debug: logger.info('Setting current signature [%s] to [%s]' % (current_signature, test_signature))
            current_signature = test_signature
            current_archive = test_archive
            if not test_signature:
                none_signature_found = True
                per_chromosome_tests_pass = False
                break
            elif not previous_signature:
                if args.debug: logger.info('Setting previous signature [%s] to [%s]' % (previous_signature, test_signature))
                previous_signature = test_signature
                previous_archive = test_archive
            if args.debug: logger.info('Comparing chr [%s] previous signature [%s] current signature [%s]' % (test_chromosome, previous_signature, current_signature))
            if current_signature != previous_signature:
                per_chromosome_tests_pass = False
            if not per_chromosome_tests_pass:
                all_tests_pass = False
                if none_signature_found:
                    sys.stderr.write('WARNING: One or more signatures are not available for chromosome [%s] in archive [%s]\n' % (test_chromosome, current_archive))
                else:
                    sys.stderr.write('WARNING: Signatures do not match for chromosome [%s] between archives [%s] and [%s]\n' % (test_chromosome, previous_archive, current_archive))

    if not all_tests_pass:
        sys.exit(errno.EINVAL)

    if selected_chromosome == default_chromosome:
        sys.stderr.write('Compressed genomic streams in input files are identical\n')
    else:
        sys.stderr.write('Compressed genomic streams in input files are identical for chromosome [%s]\n' % (selected_chromosome))

    sys.exit(os.EX_OK)

if __name__ == "__main__":
    main()
