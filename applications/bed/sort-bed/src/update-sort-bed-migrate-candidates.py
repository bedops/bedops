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

name = "update-sort-bed-migrate-candidates"
citation = "  citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract"
authors = "  authors:  Alex Reynolds and Shane Neph"
version = "  version:  2.4.39"
usage = """  $ update-sort-bed-migrate-candidates [ --dry-run ] [ --debug ]
                                       [ --write-list |
                                         --resort-immediately |
                                         --resort-in-parallel-via-slurm 
                                          [ --slurm-memory <MB> ]
                                          [ --slurm-partition <SLURM partition> ]
                                          [ --slurm-workdir <working directory> ]
                                          [ --slurm-output <SLURM output directory> ]
                                          [ --slurm-error <SLURM error directory> ] 
                                       ]
                                       [ --bedops-root-dir <bedops directory> ]
                                       [ --bedextract-path <path> ]
                                       [ --sort-bed-path <path> ]
                                       [ --unstarch-path <path> ]
                                       [ --starch-path <path> ]
                                       [ --starchcat-path <path> ]
                                       [ --update-sort-bed-slurm-path <path> ]
                                       [ --update-sort-bed-starch-slurm-path <path> ]
                                       --parent-dir <parent directory>
                                       [ --non-recursive-search ]"""
help = """
  The "update-sort-bed-migrate-candidates" utility recursively locates BED and 
  Starch files in the specified parent directory and tests if they require 
  re-sorting to conform to the updated, post-v2.4.20 "sort-bed" order. 

  Files with the extensions starch, bstarch, gstarch, bed, or bed[g|G]raph in 
  the parent directory are tested. Files without these extensions are ignored.

  If the "--non-recursive-search" option is added, this utility will only search
  within the specified parent directory, and go no deeper.

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

  Note that one of these three options must be chosen, to perform the 
  stated action, and only one option can be selected.

  When using "--resort-immediately" or "--resort-in-parallel-via-slurm", the
  resorted files will have the name of the original BED or Starch file. The 
  original files will have their old name, with the ".backup" extension.

  Use the "--bedops-root-dir" option to specify the directory containing the
  BEDOPS toolkit binaries to be used for migration. You can provide more specific
  paths to individual binaries, using the following options:

  Add the "--bedextract-path", "--sort-bed-path", "--unstarch-path",  
  "--starch-path", "--starchcat-path", "--update-sort-bed-slurm", and/or 
  "--update-sort-bed-starch-slurm" options to specify custom paths to versions 
  of these tools, if desired. These values will be passed along to downstream 
  helper scripts that use them.

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

  3. Add "--debug" option to log debug statements to get more detail about
     internal operation of update process.

"""

def main():
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help',                              '-h', action='store_true', dest='help')
    parser.add_argument('--write-list',                        '-l', action="store_true", dest='write_list')
    parser.add_argument('--resort-immediately',                '-i', action="store_true", dest='resort_immediately')
    parser.add_argument('--resort-in-parallel-via-slurm',      '-s', action="store_true", dest='resort_in_parallel_via_slurm')
    parser.add_argument('--slurm-memory',                      '-m', type=str, action="store", dest='slurm_memory')
    parser.add_argument('--slurm-partition',                   '-p', type=str, action="store", dest='slurm_partition')
    parser.add_argument('--slurm-workdir',                     '-w', type=str, action="store", dest='slurm_workdir')
    parser.add_argument('--slurm-output',                      '-r', type=str, action="store", dest='slurm_output')
    parser.add_argument('--slurm-error',                       '-e', type=str, action="store", dest='slurm_error')
    parser.add_argument('--bedops-root-dir',                   '-b', type=str, action="store", dest='bedops_root_dir')
    parser.add_argument('--bedextract-path',                   '-x', type=str, action="store", dest='bedextract_path')
    parser.add_argument('--sort-bed-path',                     '-o', type=str, action="store", dest='sort_bed_path')
    parser.add_argument('--unstarch-path',                     '-u', type=str, action="store", dest='unstarch_path')
    parser.add_argument('--starch-path',                       '-t', type=str, action="store", dest='starch_path')
    parser.add_argument('--starchcat-path',                    '-z', type=str, action="store", dest='starchcat_path')
    parser.add_argument('--update-sort-bed-slurm-path',        '-y', type=str, action="store", dest='update_sort_bed_slurm_path')
    parser.add_argument('--update-sort-bed-starch-slurm-path', '-q', type=str, action="store", dest='update_sort_bed_starch_slurm_path')
    parser.add_argument('--dry-run',                           '-n', action="store_true", dest='dry_run')
    parser.add_argument('--debug',                             '-d', action="store_true", dest='debug')
    parser.add_argument('--parent-dir',                        '-a', type=str, action="store", dest='parent_dir')
    parser.add_argument('--non-recursive-search',              '-v', action="store_true", dest='non_recursive_search')
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

    logger = None
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
        logger = logging.getLogger(__name__)

    bedextract_path = None
    if logger: logger.info('Locating \"bedextract\" binary')
    if not args.bedextract_path and not args.bedops_root_dir:
        bedextract_path = find_binary('bedextract')
        if not bedextract_path:
            if logger: logger.info('Could not locate \"bedextract\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS bedextract\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.bedextract_path:
        bedextract_path = os.path.join(args.bedops_root_dir, 'bedextract')
    elif args.bedextract_path:
        bedextract_path = args.bedextract_path
    elif not cmd_exists('bedextract'):
        if logger: logger.info('Could not locate \"bedextract\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS bedextract\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"bedextract\" is set to [%s]' % (bedextract_path))

    sort_bed_path = None
    if logger: logger.info('Locating \"sort-bed\" binary')    
    if not args.sort_bed_path and not args.bedops_root_dir:
        sort_bed_path = find_binary('sort-bed')
        if not sort_bed_path:
            if logger: logger.info('Could not locate \"sort-bed\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS sort-bed\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.sort_bed_path:
        sort_bed_path = os.path.join(args.bedops_root_dir, 'sort-bed')
    elif args.sort_bed_path:
        sort_bed_path = args.sort_bed_path
    elif not cmd_exists('sort-bed'):
        if logger: logger.info('Could not locate \"sort-bed\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS sort-bed\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"sort-bed\" is set to [%s]' % (sort_bed_path))

    unstarch_path = None
    if logger: logger.info('Locating \"unstarch\" binary')
    if not args.unstarch_path and not args.bedops_root_dir:
        unstarch_path = find_binary('unstarch')
        if not unstarch_path:
            if logger: logger.info('Could not locate \"unstarch\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS unstarch\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.unstarch_path:
        unstarch_path = os.path.join(args.bedops_root_dir, 'unstarch')
    elif args.unstarch_path:
        unstarch_path = args.unstarch_path
    elif not cmd_exists('unstarch'):
        if logger: logger.info('Could not locate \"unstarch\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS unstarch\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"unstarch\" is set to [%s]' % (unstarch_path))

    starch_path = None
    if logger: logger.info('Locating \"starch\" binary')
    if not args.starch_path and not args.bedops_root_dir:
        starch_path = find_binary('starch')
        if not starch_path:
            if logger: logger.info('Could not locate \"starch\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starch\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.starch_path:
        starch_path = os.path.join(args.bedops_root_dir, 'starch')
    elif args.starch_path:
        starch_path = args.starch_path
    elif not cmd_exists('starch'):
        if logger: logger.info('Could not locate \"starch\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starch\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"starch\" is set to [%s]' % (starch_path))

    starchcat_path = None
    if logger: logger.info('Locating \"starchcat\" binary')
    if not args.starchcat_path and not args.bedops_root_dir:
        starchcat_path = find_binary('starchcat')
        if not starchcat_path:
            if logger: logger.info('Could not locate \"starchcat\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starchcat\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.starchcat_path:
        starchcat_path = os.path.join(args.bedops_root_dir, 'starchcat')
    elif args.starchcat_path:
        starchcat_path = args.starchcat_path
    elif not cmd_exists('starchcat'):
        if logger: logger.info('Could not locate \"starchcat\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starchcat\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"starchcat\" is set to [%s]' % (starchcat_path))

    update_sort_bed_slurm_path = None
    if logger: logger.info('Locating \"update-sort-bed-slurm\" script')
    if not args.update_sort_bed_slurm_path and not args.bedops_root_dir:
        update_sort_bed_slurm_path = find_binary('update-sort-bed-slurm')
        if not update_sort_bed_slurm_path:
            if logger: logger.info('Could not locate \"update-sort-bed-slurm\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS update-sort-bed-slurm\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.update_sort_bed_slurm_path:
        update_sort_bed_slurm_path = os.path.join(args.bedops_root_dir, 'update-sort-bed-slurm')
    elif args.update_sort_bed_slurm_path:
        update_sort_bed_slurm_path = args.update_sort_bed_slurm_path
    elif not cmd_exists('update-sort-bed-slurm'):
        if logger: logger.info('Could not locate \"update-sort-bed-slurm\" binary')        
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS update-sort-bed-slurm\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"update-sort-bed-slurm\" is set to [%s]' % (update_sort_bed_slurm_path))

    update_sort_bed_starch_slurm_path = None
    if logger: logger.info('Locating \"update-sort-bed-starch-slurm\" script')
    if not args.update_sort_bed_starch_slurm_path or not args.bedops_root_dir:
        update_sort_bed_starch_slurm_path = find_binary('update-sort-bed-starch-slurm')
        if not update_sort_bed_starch_slurm_path:
            if logger: logger.info('Could not locate \"update-sort-bed-starch-slurm\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS update-sort-bed-starch-slurm\n")
            sys.exit(errno.EEXIST)
    elif args.bedops_root_dir and not args.update_sort_bed_starch_slurm_path:
        update_sort_bed_starch_slurm_path = os.path.join(args.bedops_root_dir, 'update-sort-bed-starch-slurm')
    elif args.update_sort_bed_starch_slurm_path:
        update_sort_bed_starch_slurm_path = args.update_sort_bed_starch_slurm_path
    elif not cmd_exists('update-sort-bed-starch-slurm'):
        if logger: logger.info('Could not locate \"update-sort-bed-starch-slurm\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS update-sort-bed-starch-slurm\n")
        sys.exit(errno.EEXIST)
    if logger: logger.info('Location of \"update-sort-bed-starch-slurm\" is set to [%s]' % (update_sort_bed_starch_slurm_path))

    bed_candidates = []
    starch_candidates = []
    if logger: logger.info('Initializing unfiltered BED and Starch candidate lists')

    for root, directories, filenames in os.walk(args.parent_dir):
        for filename in filenames: 
            candidate = os.path.join(root, filename)
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                starch_candidates.append(candidate)
            if candidate.lower().endswith(('.bed', '.bedgraph', '.bedGraph')):
                bed_candidates.append(candidate)
        if args.non_recursive_search: break
    if logger: logger.info('Unfiltered BED candidates are [%s]' % (str(bed_candidates)))
    if logger: logger.info('Unfiltered Starch candidates are [%s]' % (str(starch_candidates)))
                
    filtered_bed_candidates = []
    filtered_starch_candidates = []
    if logger: logger.info('Initializing filtered BED and Starch candidate lists')
    
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
            if logger: logger.info('Checking BED candidate via [%s]' % (' '.join(test_bed_sort_cmd_components)))
            test_bed_sort_process = subprocess.Popen(' '.join(test_bed_sort_cmd_components),
                                                     shell=True,
                                                     stdin=subprocess.PIPE,
                                                     stdout=subprocess.PIPE,
                                                     stderr=subprocess.STDOUT,
                                                     close_fds=True)
            (test_bed_sort_stdout, test_bed_sort_stderr) = test_bed_sort_process.communicate()
            if test_bed_sort_process.returncode != 0:
                filtered_bed_candidates.append(candidate)
        if logger: logger.info('Filtered BED candidates are [%s]' % (str(filtered_bed_candidates)))
        
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
                if logger: logger.info('Checking Starch candidate via [%s]' % (' '.join(test_starch_sort_cmd_components)))
                test_starch_sort_process = subprocess.Popen(' '.join(test_starch_sort_cmd_components),
                                                           shell=True,
                                                           stdin=subprocess.PIPE,
                                                           stdout=subprocess.PIPE,
                                                           stderr=subprocess.STDOUT,
                                                           close_fds=True)
                (test_starch_sort_stdout, test_starch_sort_stderr) = test_starch_sort_process.communicate()
                if test_starch_sort_process.returncode != 0:
                    filtered_starch_candidates.append(candidate)
        if logger: logger.info('Filtered Starch candidates are [%s]' % (str(filtered_starch_candidates)))

    all_candidates = merge(filtered_bed_candidates, filtered_starch_candidates)
    if logger: logger.info('Merged filtered BED and Starch candidate lists into all-candidate list')

    if args.write_list:
        if logger: logger.info('Writing candidate paths to standard output stream')
        for candidate in all_candidates:
            sys.stdout.write("%s\n" % (candidate))

    if args.resort_immediately:
        if logger: logger.info('Resorting candidates immediately')
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
                if logger: logger.info('Planning to resort BED candidate [%s]' % (src_original_fn))
                bed_resort_cmd_components = [
                    sort_bed_path,
                    src_original_fn,
                    '>',
                    dest_new_fn
                ]
                if logger: logger.info('Planning to resort BED candidate via [%s]' % (' '.join(bed_resort_cmd_components)))
                if not args.dry_run:
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
                        if logger: logger.info('Planning to rename [%s] to [%s]' % (src_original_fn, src_backup_fn))
                        try:
                            os.rename(src_original_fn, src_backup_fn)
                            if logger: logger.info('Renamed [%s] to [%s]' % (src_original_fn, src_backup_fn))
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_original_fn, src_backup_fn))
                            raise
                        if logger: logger.info('Planning to rename [%s] to [%s]' % (dest_new_fn, dest_final_fn))
                        try:
                            os.rename(dest_new_fn, dest_final_fn)
                            if logger: logger.info('Renamed [%s] to [%s]' % (dest_new_fn, dest_final_fn))
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                            raise
                        if logger: logger.info('Resorted BED candidate [%s] to [%s]' % (src_original_fn, dest_final_fn))
            #
            # Starch
            #
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                if logger: logger.info('Planning to resort Starch candidate [%s]' % (src_original_fn))
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
                if logger: logger.info('Planning to resort Starch candidate via [%s]' % (' '.join(starch_resort_cmd_components)))
                if not args.dry_run:
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
                        if logger: logger.info('Debug: Planning to rename [%s] to [%s]' % (src_original_fn, src_backup_fn))
                        try:
                            os.rename(src_original_fn, src_backup_fn)
                            if logger: logger.info('Renamed [%s] to [%s]' % (src_original_fn, src_backup_fn))
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (src_original_fn, src_backup_fn))
                            raise
                        sys.stderr.write("Debug: Planning to rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                        try:
                            os.rename(dest_new_fn, dest_final_fn)
                            if logger: logger.info('Renamed [%s] to [%s]' % (dest_new_fn, dest_final_fn))
                        except OSError as err:
                            sys.stderr.write("Error: Could not rename [%s] to [%s]\n" % (dest_new_fn, dest_final_fn))
                            raise
                        if logger: logger.info('Resorted Starch candidate [%s] to [%s]' % (src_original_fn, dest_final_fn))

    if args.resort_in_parallel_via_slurm:
        if logger: logger.info('Resorting candidates in parallel via SLURM job scheduler')
        for candidate in all_candidates:
            #
            # Filenames
            #
            src_original_fn = candidate
            src_backup_fn = src_original_fn + '.backup'
            dest_temp_fn = src_original_fn + '.temp'
            dest_final_fn = src_original_fn
            #
            # BED
            #
            if candidate.lower().endswith(('.bed', '.bedgraph', '.bedGraph')):
                if logger: logger.info('Planning to resort BED candidate [%s]' % (src_original_fn))
                bed_slurm_resort_cmd_components = [
                    update_sort_bed_slurm_path,
                    '--input-original',
                    src_original_fn,
                    '--input-backup',
                    src_backup_fn,
                    '--output-temp',
                    dest_temp_fn,
                    '--output-final',
                    dest_final_fn,
                    '--bedextract-path',
                    bedextract_path,
                    '--sort-bed-path',
                    sort_bed_path
                ]
                slurm_options = customize_slurm_options(args)
                if slurm_options:
                    bed_slurm_resort_cmd_components.append(slurm_options)
                if args.debug:
                    bed_slurm_resort_cmd_components.append('--debug')
                if logger: logger.info('Planning to resort BED candidate via [%s]' % (' '.join(bed_slurm_resort_cmd_components)))
                if not args.dry_run:
                    bed_slurm_resort_process = subprocess.Popen(' '.join(bed_slurm_resort_cmd_components),
                                                                shell=True,
                                                                stdin=subprocess.PIPE,
                                                                stdout=subprocess.PIPE,
                                                                stderr=subprocess.STDOUT,
                                                                close_fds=True)
                    (bed_slurm_resort_stdout, bed_slurm_resort_stderr) = bed_slurm_resort_process.communicate()
                    if bed_slurm_resort_process.returncode != 0:
                        raise
                    if logger: logger.info('SLURM task submitted to resort [%s]' % (src_original_fn))

            # Starch
            if candidate.lower().endswith(('.starch', 'bstarch', 'gstarch')):
                if logger: logger.info('Planning to resort Starch candidate [%s]' % (src_original_fn))
                starch_slurm_resort_cmd_components = [
                    update_sort_bed_starch_slurm_path,
                    '--input-original',
                    src_original_fn,
                    '--input-backup',
                    src_backup_fn,
                    '--output-temp',
                    dest_temp_fn,
                    '--output-final',
                    dest_final_fn,
                    '--bedextract-path',
                    bedextract_path,
                    '--sort-bed-path',
                    sort_bed_path,
                    '--unstarch-path',
                    unstarch_path,
                    '--starch-path',
                    starch_path,
                    '--starchcat-path',
                    starchcat_path
                ]
                slurm_options = customize_slurm_options(args)
                if slurm_options:
                    starch_slurm_resort_cmd_components.append(slurm_options)
                if args.debug:
                    starch_slurm_resort_cmd_components.append('--debug')                
                if logger: logger.info('Planning to resort Starch candidate via [%s]' % (' '.join(starch_slurm_resort_cmd_components)))
                if not args.dry_run:
                    starch_slurm_resort_process = subprocess.Popen(' '.join(starch_slurm_resort_cmd_components),
                                                                   shell=True,
                                                                   stdin=subprocess.PIPE,
                                                                   stdout=subprocess.PIPE,
                                                                   stderr=subprocess.STDOUT,
                                                                   close_fds=True)
                    (starch_slurm_resort_stdout, starch_slurm_resort_stderr) = starch_slurm_resort_process.communicate()
                    if starch_slurm_resort_process.returncode != 0:
                        raise
                    if logger: logger.info('SLURM task submitted to resort [%s]' % (src_fn))

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
