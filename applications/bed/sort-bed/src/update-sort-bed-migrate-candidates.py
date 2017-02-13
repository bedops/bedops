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
version = "  version:  2.4.25"
usage = """  $ update-sort-bed-migrate-candidates [ --dry-run ]
                                       [ --write-list ]
                                       [ --resort-immediately ]
                                       [ --resort-in-parallel-via-slurm ]
                                       --parent-dir <parent directory>"""
help = """
  The 'update-sort-bed-migrate-candidates' utility recursively locates BED and 
  pre-v2.4.20 Starch files in the specified parent directory, tests if they 
  require re-sorting to conform to the updated, post-v2.4.20 'sort-bed' order,
  and offers one of three actions to migrate them:

  1. Files that requires re-sorting can have their paths written to standard
     output, which can be written to a regular file or iterated over with 
     downstream custom logic, as the end user wishes.

  2. The specified files can be resorted immediately after all candidates are 
     found through a local, serial application of 'sort-bed'.

  3. Candidate files can be migrated by applying the 'update-sort-bed-slurm' 
     script to resort in parallel on a cluster managed with a SLURM job 
     scheduler.

  Note that one of these three options must be chosen, to perform the 
  stated action. 

  Recommended: Add the '--dry-run' option to '--resort-immediately' or the
  '--resort-in-parallel-via-slurm' options to see the behavior before any
  filesystem actions are performed. Remove `--dry-run` to actually peform
  the specified work.
"""

def main():
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help', '-h', action='store_true', dest='help')
    parser.add_argument('--write-list', '-w', action="store_true", dest='write_list')
    parser.add_argument('--resort-immediately', '-i', action="store_true", dest='resort_immediately')
    parser.add_argument('--resort-in-parallel-via-slurm', '-s', action="store_true", dest='resort_in_parallel_via_slurm')
    parser.add_argument('--dry-run', '-d', action="store_true", dest='dry_run')
    parser.add_argument('--parent-dir', '-p', type=str, action="store", dest='parent_dir')
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

    bed_candidates = []
    starch_candidates = []

    for root, directories, filenames in os.walk(args.parent_dir):
        for filename in filenames: 
            candidate = os.path.join(root, filename)
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                starch_candidates.append(candidate)
            if candidate.lower().endswith(('.bed', '.bedgraph')):
                bed_candidates.append(candidate)

    filtered_bed_candidates = []
    filtered_starch_candidates = []

    if len(bed_candidates) > 0:
        # run sort-bed --check-sort on each BED candidate
        for candidate in bed_candidates:
            test_bed_sort_cmd_components = [
                'sort-bed',
                '--check-sort',
                candidate,
                '>'
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
                'unstarch',
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
                    'unstarch',
                    candidate,
                    '|'
                    'sort-bed',
                    '--check-sort',
                    '-',
                    '>'
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
            src_fn = candidate
            src_backup_fn = src_fn + '.backup'
            dest_fn = src_fn

            # BED
            if candidate.lower().endswith(('.bed', '.bedgraph')):
                sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                sys.stderr.write("Debug: Planning to resort [%s] to [%s]\n" % (src_backup_fn, dest_fn))
                if not args.dry_run:
                    try:
                        os.rename(src_fn, src_backup_fn)
                    except OSError as err:
                        sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                        raise
                    bed_resort_cmd_components = [
                        'sort-bed',
                        src_backup_fn,
                        '>',
                        dest_fn
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
                    sys.stderr.write("Debug: Resorted [%s] to [%s]\n" % (src_backup_fn, dest_fn))

            # Starch
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                sys.stderr.write("Debug: Planning to resort [%s] to [%s]\n" % (src_backup_fn, dest_fn))
                if not args.dry_run:
                    try:
                        os.rename(src_fn, src_backup_fn)
                    except OSError as err:
                        sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                        raise
                    starch_resort_cmd_components = [
                        'unstarch',
                        src_backup_fn,
                        '|'
                        'sort-bed',
                        '-',
                        '|',
                        'starch',
                        '-',
                        '>',
                        dest_fn
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
                    sys.stderr.write("Debug: Resorted [%s] to [%s]\n" % (src_backup_fn, dest_fn))

    if args.resort_in_parallel_via_slurm:
        for candidate in all_candidates:
            src_fn = candidate
            src_backup_fn = src_fn + '.backup'
            dest_fn = src_fn

            # BED
            if candidate.lower().endswith(('.bed', '.bedgraph')):
                sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                sys.stderr.write("Debug: Planning to resort [%s] to [%s]\n" % (src_backup_fn, dest_fn))
                if not args.dry_run:
                    try:
                        os.rename(src_fn, src_backup_fn)
                    except OSError as err:
                        sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                        raise
                    bed_slurm_resort_cmd_components = [
                        'update-sort-bed-slurm',
                        '--input',
                        src_backup_fn,
                        '--output',
                        dest_fn
                    ]
                    bed_slurm_resort_process = subprocess.Popen(' '.join(bed_slurm_resort_cmd_components),
                                                                shell=True,
                                                                stdin=subprocess.PIPE,
                                                                stdout=subprocess.PIPE,
                                                                stderr=subprocess.STDOUT,
                                                                close_fds=True)
                    (bed_slurm_resort_stdout, bed_slurm_resort_stderr) = bed_slurm_resort_process.communicate()
                    if bed_slurm_resort_process.returncode != 0:
                        raise
                    sys.stderr.write("Debug: Task submitted to resort [%s] to [%s]\n" % (src_backup_fn, dest_fn))

            # Starch
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                sys.stderr.write("Debug: Planning to resort [%s] to [%s]\n" % (src_backup_fn, dest_fn))
                if not args.dry_run:
                    try:
                        os.rename(src_fn, src_backup_fn)
                    except OSError as err:
                        sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_fn, src_backup_fn))
                        raise
                    starch_slurm_resort_cmd_components = [
                        'update-sort-bed-starch-slurm',
                        '--input',
                        src_backup_fn,
                        '--output',
                        dest_fn
                    ]
                    starch_slurm_resort_process = subprocess.Popen(' '.join(starch_slurm_resort_cmd_components),
                                                                   shell=True,
                                                                   stdin=subprocess.PIPE,
                                                                   stdout=subprocess.PIPE,
                                                                   stderr=subprocess.STDOUT,
                                                                   close_fds=True)
                    (starch_slurm_resort_stdout, starch_slurm_resort_stderr) = starch_slurm_resort_process.communicate()
                    if starch_slurm_resort_process.returncode != 0:
                        raise
                    sys.stderr.write("Debug: Task submitted to resort [%s] to [%s]\n" % (src_backup_fn, dest_fn))

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