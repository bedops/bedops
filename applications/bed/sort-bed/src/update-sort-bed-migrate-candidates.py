#!/usr/bin/env python

#
#    BEDOPS
#    Copyright (C) 2011-2017 Shane Neph, Scott Kuehn and Alex Reynolds
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

name = "update-sort-bed-migrate-candidates"
citation = "  citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract"
authors = "  authors:  Alex Reynolds and Shane Neph"
version = "  version:  2.4.26"
usage = """  $ update-sort-bed-migrate-candidates [ --dry-run ]
                                       [ --write-list |
                                         --resort-immediately |
                                         --resort-in-parallel-via-slurm 
                                          [ --slurm-memory <MB> ]
                                          [ --slurm-partition <SLURM partition> ]
                                          [ --slurm-workdir <working directory> ]
                                          [ --slurm-output <SLURM output directory> ]
                                          [ --slurm-error <SLURM error directory> ] 
                                       ]
                                       [ --bedextract-path <path to bedextract> ]
                                       [ --sort-bed-path <path to sort-bed> ]
                                       [ --unstarch-path <path to unstarch> ]
                                       [ --starch-path <path to starch> ]
                                       [ --starchcat-path <path to starchcat> ]
                                       --parent-dir <parent directory>"""
help = """
  The "update-sort-bed-migrate-candidates" utility recursively locates BED and 
  Starch files in the specified parent directory and tests if they require 
  re-sorting to conform to the updated, post-v2.4.20 "sort-bed" order. 

  Files with the extensions starch, bstarch, gstarch, bed, or bed[g|G]raph in 
  the parent directory are tested. Files without these extensions are ignored.

  This utility offers one of three (exclusive) actions for migration:

  1. Using "--write-list", files that require re-sorting can have their paths
     written to standard output, which can be written to a file to be processed
     later on, as desired.

  2. Using "--resort-immediately", qualifying files can be resorted immediately
     after all candidates are found, through a local, serial application of 
     'sort-bed'.

  3. Using "--resort-in-parallel-via-slurm", candidate files can be migrated by 
     applying the 'update-sort-bed-slurm' script to resort in parallel on a 
     computational cluster managed with a SLURM job scheduler.

  Add the "--bedextract-path", "--sort-bed-path", "--unstarch-path",  
  "--starch-path", and "--starchcat-path" options to specify custom paths to 
  versions of these tools, if desired. These values will be passed along to 
  downstream helper scripts that use them.

  Note that one of these three options must be chosen, to perform the 
  stated action, and only one option can be selected.

  When using "--resort-immediately" or "--resort-in-parallel-via-slurm", the
  resorted files will have the name of the original BED or Starch file. The 
  original files will have their old name, with the ".backup" extension.

  ---

  Suggestions: 

  1. Add the "--dry-run" option to "--resort-immediately" or the
     "--resort-in-parallel-via-slurm" options to see the behavior before any
     filesystem actions are performed. Remove "--dry-run" to perform the 
     specified work.

  2. If you use the resort-via-SLURM option, consider using "--slurm-memory", 
     "--slurm-partition", "--slurm-workdir", "--slurm-output", and 
     "--slurm-error" options to match the setup of your particular cluster 
     environment and inputs.
"""

def main():
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help',                         '-h', action='store_true', dest='help')
    parser.add_argument('--write-list',                   '-l', action="store_true", dest='write_list')
    parser.add_argument('--resort-immediately',           '-i', action="store_true", dest='resort_immediately')
    parser.add_argument('--resort-in-parallel-via-slurm', '-s', action="store_true", dest='resort_in_parallel_via_slurm')
    parser.add_argument('--slurm-memory',                 '-m', type=str, action="store", dest='slurm_memory')
    parser.add_argument('--slurm-partition',              '-p', type=str, action="store", dest='slurm_partition')
    parser.add_argument('--slurm-workdir',                '-w', type=str, action="store", dest='slurm_workdir')
    parser.add_argument('--slurm-output',                 '-r', type=str, action="store", dest='slurm_output')
    parser.add_argument('--slurm-error',                  '-e', type=str, action="store", dest='slurm_error') 
    parser.add_argument('--bedextract-path',              '-x', type=str, action="store", dest='bedextract_path')
    parser.add_argument('--sort-bed-path',                '-o', type=str, action="store", dest='sort_bed_path')
    parser.add_argument('--unstarch-path',                '-u', type=str, action="store", dest='unstarch_path')
    parser.add_argument('--starch-path',                  '-t', type=str, action="store", dest='starch_path')
    parser.add_argument('--starchcat-path',               '-z', type=str, action="store", dest='starchcat_path')
    parser.add_argument('--dry-run',                      '-d', action="store_true", dest='dry_run')
    parser.add_argument('--parent-dir',                   '-a', type=str, action="store", dest='parent_dir')
    args = parser.parse_args()

    action_counter = 0
    if args.write_list:
        action_counter += 1
    if args.resort_immediately:
        action_counter += 1
    if args.resort_in_parallel_via_slurm:
        action_counter += 1

    if args.help or not args.parent_dir or action_counter != 1:
        sys.stdout.write(name + '\n')
        sys.stdout.write(citation + '\n')
        sys.stdout.write(version + '\n')
        sys.stdout.write(authors + '\n\n')
        sys.stdout.write(usage + '\n')
        sys.stdout.write(help)
        if args.help:
            sys.exit(os.EX_OK)
        else:
            sys.exit(errno.EINVAL)

    bedextract_path = None
    if not args.bedextract_path:
        bedextract_path = find_binary('bedextract')
        if not bedextract_path:
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS bedextract\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('bedextract'):
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS bedextract\n")
        sys.exit(errno.EEXIST)

    sort_bed_path = None
    if not args.sort_bed_path:
        sort_bed_path = find_binary('sort-bed')
        if not sort_bed_path:
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS sort-bed\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('sort-bed'):
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS sort-bed\n")
        sys.exit(errno.EEXIST)

    unstarch_path = None
    if not args.unstarch_path:
        unstarch_path = find_binary('unstarch')
        if not unstarch_path:
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS unstarch\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('unstarch'):
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS unstarch\n")
        sys.exit(errno.EEXIST)

    starch_path = None
    if not args.starch_path:
        starch_path = find_binary('starch')
        if not starch_path:
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starch\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('starch'):
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starch\n")
        sys.exit(errno.EEXIST)

    starchcat_path = None
    if not args.starchcat_path:
        starchcat_path = find_binary('starchcat')
        if not starchcat_path:
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starchcat\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('starchcat'):
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starchcat\n")
        sys.exit(errno.EEXIST)

    bed_candidates = []
    starch_candidates = []

    for root, directories, filenames in os.walk(args.parent_dir):
        for filename in filenames: 
            candidate = os.path.join(root, filename)
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                starch_candidates.append(candidate)
            if candidate.lower().endswith(('.bed', '.bedgraph', '.bedGraph')):
                bed_candidates.append(candidate)

    filtered_bed_candidates = []
    filtered_starch_candidates = []

    if len(bed_candidates) > 0:
        # run sort-bed --check-sort on each BED candidate
        for candidate in bed_candidates:
            test_bed_sort_cmd_components = [
                sort_bed_path,
                '--check-sort',
                candidate,
                '>',
                '/dev/null'
            ]
            test_bed_sort_process = subprocess.Popen(' '.join(test_bed_sort_cmd_components),
                                                     shell=True,
                                                     stdin=subprocess.PIPE,
                                                     stdout=subprocess.PIPE,
                                                     stderr=subprocess.STDOUT,
                                                     close_fds=True)
            (test_bed_sort_stdout, test_bed_sort_stderr) = test_bed_sort_process.communicate()
            if test_bed_sort_process.returncode != 0:
                filtered_bed_candidates.append(candidate)
        
    if len(starch_candidates) > 0:
        # run sort-bed --check-sort on the extracted output of a Starch candidate
        for candidate in starch_candidates:
            get_archive_version_cmd_components = [
                unstarch_path,
                '--list-json',
                candidate
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
                test_starch_sort_cmd_components = [
                    unstarch_path,
                    candidate,
                    '|',
                    sort_bed_path,
                    '--check-sort',
                    '-',
                    '>',
                    '/dev/null'
                ]
                test_starch_sort_process = subprocess.Popen(' '.join(test_starch_sort_cmd_components),
                                                           shell=True,
                                                           stdin=subprocess.PIPE,
                                                           stdout=subprocess.PIPE,
                                                           stderr=subprocess.STDOUT,
                                                           close_fds=True)
                (test_starch_sort_stdout, test_starch_sort_stderr) = test_starch_sort_process.communicate()
                if test_starch_sort_process.returncode != 0:
                    filtered_starch_candidates.append(candidate)


    all_candidates = merge(filtered_bed_candidates, filtered_starch_candidates)

    if args.write_list:
        for candidate in all_candidates:
            sys.stdout.write("%s\n" % (candidate))

    if args.resort_immediately:
        for candidate in all_candidates:
            #
            # Filenames
            #
            src_original_fn = candidate
            src_backup_fn = src_original_fn + '.backup'
            dest_new_fn = src_original_fn + '.new'
            dest_final_fn = src_original_fn
            #
            # BED
            #
            if candidate.lower().endswith(('.bed', '.bedgraph', '.bedGraph')):
                sys.stderr.write("Debug: Planning to resort [%s] to [%s]\n" % (src_original_fn, dest_new_fn))
                if not args.dry_run:
                    bed_resort_cmd_components = [
                        sort_bed_path,
                        src_original_fn,
                        '>',
                        dest_new_fn
                    ]
                    bed_resort_process = subprocess.Popen(' '.join(bed_resort_cmd_components),
                                                          shell=True,
                                                          stdin=subprocess.PIPE,
                                                          stdout=subprocess.PIPE,
                                                          stderr=subprocess.STDOUT,
                                                          close_fds=True)
                    (bed_resort_stdout, bed_resort_stderr) = bed_resort_process.communicate()
                    if bed_resort_process.returncode != 0:
                        raise
                    else:
                        sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (src_original_fn, src_backup_fn))
                        try:
                            os.rename(src_original_fn, src_backup_fn)
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_original_fn, src_backup_fn))
                            raise
                        sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                        try:
                            os.rename(dest_new_fn, dest_final_fn)
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                            raise
                        sys.stderr.write("Debug: Resorted [%s]\n" % (src_original_fn))
            #
            # Starch
            #
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                sys.stderr.write("Debug: Planning to resort [%s] to [%s]\n" % (src_original_fn, dest_new_fn))
                if not args.dry_run:
                    starch_resort_cmd_components = [
                        unstarch_path,
                        src_original_fn,
                        '|',
                        sort_bed_path,
                        '-',
                        '|',
                        starch_path,
                        '-',
                        '>',
                        dest_new_fn
                    ]
                    starch_resort_process = subprocess.Popen(' '.join(starch_resort_cmd_components),
                                                             shell=True,
                                                             stdin=subprocess.PIPE,
                                                             stdout=subprocess.PIPE,
                                                             stderr=subprocess.STDOUT,
                                                             close_fds=True)
                    (starch_resort_stdout, starch_resort_stderr) = starch_resort_process.communicate()
                    if starch_resort_process.returncode != 0:
                        raise
                    else:
                        sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (src_original_fn, src_backup_fn))
                        try:
                            os.rename(src_original_fn, src_backup_fn)
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_original_fn, src_backup_fn))
                            raise
                        sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                        try:
                            os.rename(dest_new_fn, dest_final_fn)
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                            raise
                        sys.stderr.write("Debug: Resorted [%s]\n" % (src_original_fn))

    if args.resort_in_parallel_via_slurm:
        for candidate in all_candidates:
            #
            # Filenames
            #
            src_original_fn = candidate
            src_backup_fn = src_original_fn + '.backup'
            dest_new_fn = src_original_fn + '.new'
            dest_final_fn = src_original_fn
            #
            # BED
            #
            if candidate.lower().endswith(('.bed', '.bedgraph', '.bedGraph')):
                sys.stderr.write("Debug: Planning to resort via SLURM [%s]\n" % (src_original_fn))
                if not args.dry_run:
                    bed_slurm_resort_cmd_components = [
                        'update-sort-bed-slurm',
                        '--input-original',
                        src_original_fn,
                        '--input-backup',
                        src_backup_fn,
                        '--output-new',
                        dest_new_fn,
                        '--output-final',
                        dest_final_fn
                    ]
                    slurm_options = customize_slurm_options(args)
                    if slurm_options:
                        bed_slurm_resort_cmd_components.append(slurm_options)
                    bed_slurm_resort_process = subprocess.Popen(' '.join(bed_slurm_resort_cmd_components),
                                                                shell=True,
                                                                stdin=subprocess.PIPE,
                                                                stdout=subprocess.PIPE,
                                                                stderr=subprocess.STDOUT,
                                                                close_fds=True)
                    (bed_slurm_resort_stdout, bed_slurm_resort_stderr) = bed_slurm_resort_process.communicate()
                    if bed_slurm_resort_process.returncode != 0:
                        raise
                    sys.stderr.write("Debug: Task submitted to resort [%s]\n" % (src_original_fn))

            # Starch
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                sys.stderr.write("Debug: Planning to resort via SLURM [%s]\n" % (src_fn))
                if not args.dry_run:
                    starch_slurm_resort_cmd_components = [
                        'update-sort-bed-starch-slurm',
                        '--input',
                        src_fn,
                        '--output',
                        dest_fn
                    ]
                    slurm_options = customize_slurm_options(args)
                    if slurm_options:
                        starch_slurm_resort_cmd_components.append(slurm_options)
                    starch_slurm_resort_process = subprocess.Popen(' '.join(starch_slurm_resort_cmd_components),
                                                                   shell=True,
                                                                   stdin=subprocess.PIPE,
                                                                   stdout=subprocess.PIPE,
                                                                   stderr=subprocess.STDOUT,
                                                                   close_fds=True)
                    (starch_slurm_resort_stdout, starch_slurm_resort_stderr) = starch_slurm_resort_process.communicate()
                    if starch_slurm_resort_process.returncode != 0:
                        raise
                    sys.stderr.write("Debug: Task submitted to resort [%s]\n" % (src_fn))

def find_binary(binary_to_find):
    for p in os.environ['PATH'].split(':'):
        for r, d, f in os.walk(p):
            for filename in f:
                if filename == binary_to_find:
                    return os.path.join(r, filename)
    return None

def cmd_exists(cmd):
    return subprocess.call("type " + cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) == 0

def customize_slurm_options(args):
    custom_slurm_options = ""
    if args.slurm_memory:
        custom_slurm_options += "--slurm-memory " + args.slurm_memory
    if args.slurm_partition:
        custom_slurm_options += "--slurm-partition " + args.slurm_partition
    if args.slurm_workdir:
        custom_slurm_options += "--slurm-workdir " + args.slurm_workdir
    if args.slurm_output:
        custom_slurm_options += "--slurm-output " + args.slurm_output
    if args.slurm_error:
        custom_slurm_options += "--slurm-error " + args.slurm_error
    return custom_slurm_options if len(custom_slurm_options) > 0 else None

def merge(l, m):
    result = []
    i = j = 0
    total = len(l) + len(m)
    while len(result) != total:
        if len(l) == i:
            result += m[j:]
            break
        elif len(m) == j:
            result += l[i:]
            break
        elif l[i] < m[j]:
            result.append(l[i])
            i += 1
        else:
            result.append(m[j])
            j += 1
    return result

if __name__ == "__main__":
    main()
