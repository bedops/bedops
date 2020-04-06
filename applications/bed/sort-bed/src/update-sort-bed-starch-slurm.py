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
import random
import string
import logging

name = "update-sort-bed-starch-slurm"
citation = "  citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract"
authors = "  authors:  Alex Reynolds and Shane Neph"
version = "  version:  2.4.39"
usage = """  $ update-sort-bed-starch-slurm [ --slurm-memory <MB> ] 
                                 [ --slurm-partition <SLURM partition> ] 
                                 [ --slurm-workdir <working directory> ]
                                 [ --slurm-output <SLURM output directory> ]
                                 [ --slurm-error <SLURM error directory> ]
                                 [ --bedextract-path <path to bedextract> ]
                                 [ --sort-bed-path <path to sort-bed> ]
                                 [ --unstarch-path <path to unstarch> ]
                                 [ --starch-path <path to starch> ]
                                 [ --starchcat-path <path to starchcat> ]
                                 [ --debug ]
                                 --input-original <old-bed-file> 
                                 --input-backup <renamed-old-bed-file>
                                 --output-temp <intermediate-new-bed-file>
                                 --output-final <new-bed-file>"""
help = """
  The 'update-sort-bed-starch-slurm' utility applies an updated sort order on 
  Starch files sorted per pre-v2.4.20 sort-bed, using a SLURM job scheduler to 
  coordinate resorting each chromosome in "--input-original" per post-v2.4.20 
  sort-bed and writing the result to a Starch file at "--output-final". 

  When migration is finished, "--input-backup" specifies the new name of the 
  original input file.

  As migration progresses, intermediate results are written to "--output-temp"
  and then written to "--output-final" upon completion.

  Each sort task is given 8 GB of memory and is assigned to the "queue0"
  partition, unless the "--slurm-memory" and "--slurm-partition" options are 
  set. If your input is larger than 8 GB, you will need to allocate more 
  memory.

  Because this launches all work on the specified cluster partition, the paths
  specified by "--input-original", "--input-backup", "--output-final", and 
  "--output-temp" must be accessible to all computational nodes. For example, 
  using /tmp may fail, as the /tmp path is almost certainly unique to a node; 
  it is necesssary to use a path shared among all nodes.

  Note that this utility will not work on entirely unsorted BED files, but only 
  on files with a sort order from pre-v2.4.20 sort-bed, where there are ties on 
  the first three columns. 

  In fact, until further refinements are made, this convenience utility could 
  fail silently on inputs which are not BED, or which are not sorted per pre-
  v2.4.20 order, or which do not follow exact specification, all of which can 
  lead a per-chromosome resort task to fail.
"""

def main():
    slurm_memory = "8000"
    slurm_partition = "queue0"
    slurm_workdir = os.getcwd()
    slurm_output = None
    slurm_error = None

    # concatenation and other minor steps do not require much memory
    slurm_backup_input_memory = "500"
    slurm_concatenation_memory = "500"
    slurm_cleanup_memory = "500"
    slurm_output_move_memory = "500"
    
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help',            '-h', action='store_true', dest='help')
    parser.add_argument('--slurm-memory',    '-m', type=str, action="store", dest='slurm_memory')
    parser.add_argument('--slurm-partition', '-p', type=str, action="store", dest='slurm_partition')
    parser.add_argument('--slurm-workdir',   '-w', type=str, action="store", dest='slurm_workdir')
    parser.add_argument('--slurm-output',    '-u', type=str, action="store", dest='slurm_output')
    parser.add_argument('--slurm-error',     '-e', type=str, action="store", dest='slurm_error')    
    parser.add_argument('--input-original',  '-i', type=str, action="store", dest='input_original_fn')
    parser.add_argument('--input-backup',    '-b', type=str, action="store", dest='input_backup_fn')
    parser.add_argument('--output-temp',     '-t', type=str, action="store", dest='output_temp_fn')
    parser.add_argument('--output-final',    '-o', type=str, action="store", dest='output_final_fn')
    parser.add_argument('--sort-bed-path',   '-s', type=str, action="store", dest='sort_bed_path')
    parser.add_argument('--unstarch-path',   '-a', type=str, action="store", dest='unstarch_path')
    parser.add_argument('--starch-path',     '-c', type=str, action="store", dest='starch_path')
    parser.add_argument('--starchcat-path',  '-z', type=str, action="store", dest='starchcat_path')
    parser.add_argument('--debug',           '-d', action="store_true", dest='debug')    
    args = parser.parse_args()

    if args.help or (not args.input_original_fn or not args.input_backup_fn or not args.output_temp_fn or not args.output_final_fn):
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

    if logger: logger.info('Locating original input file [%s]' % (args.input_original_fn))
    if not os.path.exists(args.input_original_fn):
        if logger: logger.info('Not able to locate original input file')
        sys.stderr.write("ERROR: Input file [%s] does not exist\n" % (args.input_original_fn))
        sys.exit(errno.EINVAL)

    if logger: logger.info('Locating final output file [%s]' % (args.output_final_fn))
    if os.path.exists(args.output_final_fn):
        if logger: logger.info('Found output file [%s] which will be replaced' % (args.output_final_fn))
        sys.stderr.write("Note: Output file [%s] exists and will be replaced\n" % (args.output_final_fn))

    if logger: logger.info('Verfying that \"sbatch\" can be found')
    if not cmd_exists('sbatch'):
        sys.stderr.write("ERROR: This script must be run on a system with SLURM binaries available\n")
        sys.exit(errno.EEXIST)

    if logger: logger.info('Testing if original input and backup input file paths are equal')
    if args.input_original_fn == args.input_backup_fn:
        sys.stderr.write("ERROR: Input filename [%s] cannot be the same as the input backup filename [%s]\n" % (args.input_original_fn, args.input_backup_fn))
        sys.exit(errno.EINVAL)

    if logger: logger.info('Testing if temporary output and final output file paths are equal')
    if args.output_temp_fn == args.output_final_fn:
        sys.stderr.write("ERROR: Output filename [%s] cannot be the same as the output temporary filename [%s]\n" % (args.output_final_fn, args.output_temp_fn))
        sys.exit(errno.EINVAL)

    # parse args
    if args.slurm_memory:
        slurm_memory = args.slurm_memory
    if logger: logger.info('SLURM memory allocation set to [%s]' % (slurm_memory))

    if args.slurm_partition:
        slurm_partition = args.slurm_partition
    if logger: logger.info('SLURM partition set to [%s]' % (slurm_partition))

    if args.slurm_workdir:
        slurm_workdir = args.slurm_workdir
    if logger: logger.info('SLURM work directory set to [%s]' % (slurm_workdir))

    if args.slurm_output:
        slurm_output = args.slurm_output
    if logger: logger.info('SLURM output log set to [%s]' % (slurm_output))

    if args.slurm_error:
        slurm_error = args.slurm_error
    if logger: logger.info('SLURM error log set to [%s]' % (slurm_error))

    if not slurm_output:
        slurm_output = slurm_workdir
        if logger: logger.info('SLURM output log reassigned to [%s]' % (slurm_output))

    if not slurm_error:
        slurm_error = slurm_workdir
        if logger: logger.info('SLURM error log reassigned to [%s]' % (slurm_error))

    sort_bed_path = None
    if logger: logger.info('Locating \"sort-bed\" binary')
    if not args.sort_bed_path:
        sort_bed_path = find_binary('sort-bed')
        if not sort_bed_path:
            if logger: logger.info('Could not locate \"sort-bed\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS sort-bed\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('sort-bed'):
        if logger: logger.info('Could not locate \"sort-bed\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS sort-bed\n")
        sys.exit(errno.EEXIST)
    else:
        sort_bed_path = args.sort_bed_path
    if logger: logger.info('Location of \"sort-bed\" is set to [%s]' % (sort_bed_path))

    unstarch_path = None
    if logger: logger.info('Locating \"unstarch\" binary')
    if not args.unstarch_path:
        unstarch_path = find_binary('unstarch')
        if not unstarch_path:
            if logger: logger.info('Could not locate \"unstarch\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS unstarch\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('unstarch'):
        if logger: logger.info('Could not locate \"unstarch\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS unstarch\n")
        sys.exit(errno.EEXIST)
    else:
        unstarch_path = args.unstarch_path
    if logger: logger.info('Location of \"unstarch\" is set to [%s]' % (unstarch_path))

    starch_path = None
    if logger: logger.info('Locating \"starch\" binary')
    if not args.starch_path:
        starch_path = find_binary('starch')
        if not starch_path:
            if logger: logger.info('Could not locate \"starch\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starch\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('starch'):
        if logger: logger.info('Could not locate \"starch\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starch\n")
        sys.exit(errno.EEXIST)
    else:
        starch_path = args.starch_path
    if logger: logger.info('Location of \"starch\" is set to [%s]' % (starch_path))
    
    starchcat_path = None
    if logger: logger.info('Locating \"starchcat\" binary')
    if not args.starchcat_path:
        starchcat_path = find_binary('starchcat')
        if not starchcat_path:
            if logger: logger.info('Could not locate \"starchcat\" binary')
            sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starchcat\n")
            sys.exit(errno.EEXIST)
    elif not cmd_exists('starchcat'):
        if logger: logger.info('Could not locate \"starchcat\" binary')
        sys.stderr.write("ERROR: This script must be run on a system with BEDOPS starchcat\n")
        sys.exit(errno.EEXIST)
    else:
        starchcat_path = args.starchcat_path
    if logger: logger.info('Location of \"starchcat\" is set to [%s]' % (starchcat_path))
        
    # build a list of chromosomes upon which to do work
    list_chromosome_cmd_components = [
        unstarch_path,
        '--list-chr',
        args.input_original_fn
    ]
    if logger: logger.info('Listing Starch chromosomes via [%s]' % (' '.join(list_chromosome_cmd_components)))
    try:
        list_chromosome_cmd_result = subprocess.check_output(list_chromosome_cmd_components)
    except subprocess.CalledProcessError as err:
        list_chromosome_cmd_result = "ERROR: Command '{}' returned with error (code {}): {}".format(err.cmd, err.returncode, err.output)
        raise
    chromosome_list = list_chromosome_cmd_result.decode('utf-8').rstrip('\n').split('\n')
    if logger: logger.info('Chromosomes to sort are [%s]' % (str(chromosome_list)))

    # fire up the mid-range saloons^H^H^H per-chromosome sort tasks
    job_prefix = ''.join(random.choice(string.lowercase) for x in range(8))
    if logger: logger.info('Setting job prefix to [%s]' % (job_prefix))
    
    per_chr_job_ids = []
    per_chr_job_fns = []

    for chromosome in chromosome_list:
        temp_dest = os.path.join(os.getcwd(), '_'.join([job_prefix, chromosome]))
        if logger: logger.info('Per-chromosome [%s] job name set to [%s]' % (chromosome, temp_dest))
        per_chromosome_sort_cmd_components = [
            'sbatch',
            '--parsable',
            '--workdir',
            slurm_workdir,
            '--output',
            os.path.join(slurm_output, '_'.join([job_prefix, 'out', chromosome])),
            '--error',
            os.path.join(slurm_error, '_'.join([job_prefix, 'err', chromosome])),
            '--mem',
            slurm_memory,
            '--partition',
            slurm_partition,
            '--wrap',
            '"srun ' + unstarch_path + ' ' + chromosome + ' ' + args.input_original_fn + ' | ' + sort_bed_path + ' - | ' + starch_path + ' - > ' + temp_dest + '"'
        ]
        if logger: logger.info('Submitting job via [%s]' % (' '.join(per_chromosome_sort_cmd_components)))
        try:
            per_chromosome_process = subprocess.Popen(' '.join(per_chromosome_sort_cmd_components),
                                                      shell=True,
                                                      stdin=subprocess.PIPE,
                                                      stdout=subprocess.PIPE,
                                                      stderr=subprocess.STDOUT,
                                                      close_fds=True)
            (per_chromosome_stdout, per_chromosome_stderr) = per_chromosome_process.communicate()
            per_chr_job_id = per_chromosome_stdout.decode('utf-8').rstrip('\n')
            per_chr_job_ids.append(per_chr_job_id)
            if logger: logger.info('Per-chromosome [%s] job id set to [%s]' % (chromosome, per_chr_job_id))
            per_chr_job_fns.append(temp_dest)
        except subprocess.CalledProcessError as err:
            sys.stderr.write("ERROR: Per-chromosome sort task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
            sys.exit(errno.EINVAL)
        if logger: logger.info('Per-chromosome sort task outputs [%s] to be concatenated' % (str(per_chr_job_fns)))
        per_chr_dependency_string = 'afterok:' + ':'.join(per_chr_job_ids)

    # concatenate per-chromosome resorted files to output temp file
    concatenation_cmd_components = [
        'sbatch',
        '--parsable',
        '--workdir',
        slurm_workdir,
        '--output',
        os.path.join(slurm_output, '_'.join([job_prefix, 'out', 'concatenation'])),
        '--error',
        os.path.join(slurm_error, '_'.join([job_prefix, 'err', 'concatenation'])),
        '--mem',
        slurm_concatenation_memory,
        '--partition',
        slurm_partition,
        '--dependency',
        per_chr_dependency_string,
        '--wrap',
        '"srun ' + starchcat_path + ' ' + ' '.join(per_chr_job_fns) + ' > ' + args.output_temp_fn + '"'
    ]
    if logger: logger.info('Submitting concatenation job via [%s]' % (' '.join(concatenation_cmd_components)))
    try:
        concatenation_process = subprocess.Popen(' '.join(concatenation_cmd_components),
                                                 shell=True,
                                                 stdin=subprocess.PIPE,
                                                 stdout=subprocess.PIPE,
                                                 stderr=subprocess.STDOUT,
                                                 close_fds=True)
        (concatenation_stdout, concatenation_stderr) = concatenation_process.communicate()
        concatenation_job_id = concatenation_stdout.decode('utf-8').rstrip('\n')
        if logger: logger.info('Concatenation job id set to [%s]' % (concatenation_job_id))
        concatenation_job_dependency_string = 'afterok:' + concatenation_job_id
    except subprocess.CalledProcessError as err:
        sys.stderr.write("ERROR: Concatenation task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
        sys.exit(errno.EINVAL)

    # move original input file to backup file
    backup_input_cmd_components = [
        'sbatch',
        '--parsable',
        '--workdir',
        slurm_workdir,
        '--output',
        os.path.join(slurm_output, '_'.join([job_prefix, 'out', 'backup_input'])),
        '--error',
        os.path.join(slurm_error, '_'.join([job_prefix, 'err', 'backup_input'])),
        '--mem',
        slurm_backup_input_memory,
        '--partition',
        slurm_partition,
        '--dependency',
        concatenation_job_dependency_string,
        '--wrap',
        '"srun mv ' + args.input_original_fn + ' ' + args.input_backup_fn + '"'
    ]
    if logger: logger.info('Submitting backup job via [%s]' % (' '.join(backup_input_cmd_components)))
    try:
        backup_input_process = subprocess.Popen(' '.join(backup_input_cmd_components),
                                                shell=True,  
                                                stdin=subprocess.PIPE,
                                                stdout=subprocess.PIPE,
                                                stderr=subprocess.STDOUT,
                                                close_fds=True)
        (backup_input_stdout, backup_input_stderr) = backup_input_process.communicate()
        backup_input_job_id = backup_input_stdout.decode('utf-8').rstrip('\n')
        if logger: logger.info('Backup job id set to [%s]' % (backup_input_job_id))
        backup_input_job_dependency_string = 'afterok:' + backup_input_job_id
    except subprocess.CalledProcessError as err:
        sys.stderr.write("ERROR: Backup input task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
        sys.exit(errno.EINVAL)

    # move output temp file to output final file
    output_move_cmd_components = [
        'sbatch',
        '--parsable',
        '--workdir',
        slurm_workdir,
        '--output',
        os.path.join(slurm_output, '_'.join([job_prefix, 'out', 'concatenation'])),
        '--error',
        os.path.join(slurm_error, '_'.join([job_prefix, 'err', 'concatenation'])),
        '--mem',
        slurm_output_move_memory,
        '--partition',
        slurm_partition,
        '--dependency',
        backup_input_job_dependency_string,
        '--wrap',
        '"srun mv ' + args.output_temp_fn + ' ' + args.output_final_fn + '"'
    ]
    if logger: logger.info('Submitting final move job via [%s]' % (' '.join(output_move_cmd_components)))
    try:
        output_move_process = subprocess.Popen(' '.join(output_move_cmd_components),
                                               shell=True,
                                               stdin=subprocess.PIPE,
                                               stdout=subprocess.PIPE,
                                               stderr=subprocess.STDOUT,
                                               close_fds=True)
        (output_move_stdout, output_move_stderr) = output_move_process.communicate()
        output_move_job_id = output_move_stdout.decode('utf-8').rstrip('\n')
        if logger: logger.info('Final move job id set to [%s]' % (output_move_job_id))
        output_move_job_dependency_string = 'afterok:' + output_move_job_id
    except subprocess.CalledProcessError as err:
        sys.stderr.write("ERROR: Output move task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
        sys.exit(errno.EINVAL)

    # perform cleanup
    cleanup_cmd_components = [
        'sbatch',
        '--parsable',
        '--workdir',
        slurm_workdir,
        '--output',
        os.path.join(slurm_output, '_'.join([job_prefix, 'out', 'concatenation'])),
        '--error',
        os.path.join(slurm_error, '_'.join([job_prefix, 'err', 'concatenation'])),
        '--mem',
        slurm_cleanup_memory,
        '--partition',
        slurm_partition,
        '--dependency',
        output_move_job_dependency_string,
        '--wrap',
        '"srun rm -f ' + os.path.join(slurm_workdir, job_prefix) + '_* ' + os.path.join(slurm_output, job_prefix) + '_*"'
    ]
    try:
        cleanup_process = subprocess.Popen(' '.join(cleanup_cmd_components),
                                           shell=True,
                                           stdin=subprocess.PIPE,
                                           stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT,
                                           close_fds=True)
        (cleanup_stdout, cleanup_stderr) = cleanup_process.communicate()
        cleanup_job_id = cleanup_stdout.decode('utf-8').rstrip('\n')
        if logger: logger.info('Cleanup job id set to [%s]' % (cleanup_job_id))
    except subprocess.CalledProcessError as err:
        sys.stderr.write("ERROR: Cleanup task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
        sys.exit(errno.EINVAL)

def find_binary(binary_to_find):
    for p in os.environ['PATH'].split(':'):
        for r, d, f in os.walk(p):
            for filename in f:
                if filename == binary_to_find:
                    return os.path.join(r, filename)
    return None

def cmd_exists(cmd):
    return subprocess.call("type " + cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) == 0

if __name__ == "__main__":
    main()
