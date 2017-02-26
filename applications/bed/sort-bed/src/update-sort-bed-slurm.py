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
import random
import string

name = "update-sort-bed-slurm"
citation = "  citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract"
authors = "  authors:  Alex Reynolds and Shane Neph"
version = "  version:  2.4.26"
usage = """  $ update-sort-bed-slurm [ --slurm-memory <MB> ] 
                          [ --slurm-partition <SLURM partition> ] 
                          [ --slurm-workdir <working directory> ]
                          [ --slurm-output <SLURM output directory> ]
                          [ --slurm-error <SLURM error directory> ]
                          [ --bedextract-path <path to bedextract> ]
                          [ --sort-bed-path <path to sort-bed> ]
                          --input-original <old-bed-file> 
                          --input-backup <renamed-old-bed-file>
                          --output-temp <intermediate-new-bed-file>
                          --output-final <new-bed-file>"""
help = """
  The "update-sort-bed-slurm" utility applies an updated sort order on BED 
  files sorted per pre-v2.4.20 sort-bed, using a SLURM job scheduler to 
  coordinate resorting each chromosome in "--input-original" per post-v2.4.20 
  sort-bed and writing the result to "--output-final". 

  When migration is finished, "--input-backup" specifies the new name of the 
  original input.

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
    parser.add_argument('--bedextract-path', '-x', type=str, action="store", dest='bedextract_path')
    parser.add_argument('--sort-bed-path',   '-s', type=str, action="store", dest='sort_bed_path')
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

    if not os.path.exists(args.input_original_fn): 
        sys.stderr.write("ERROR: Input file [%s] does not exist\n" % (args.input_original_fn))
        sys.exit(errno.EINVAL)

    if os.path.exists(args.output_final_fn):
        sys.stderr.write("ERROR: Output file [%s] exists\n" % (args.output_final_fn))
        sys.exit(errno.EEXIST)

    if not cmd_exists('sbatch'):
        sys.stderr.write("ERROR: This script must be run on a system with SLURM binaries available\n")
        sys.exit(errno.EEXIST)

    if args.input_original_fn == args.input_backup_fn:
        sys.stderr.write("ERROR: Input filename [%s] cannot be the same as the input backup filename [%s]\n" % (args.input_original_fn, args.input_backup_fn))
        sys.exit(errno.EINVAL)

    if args.output_temp_fn == args.output_final_fn:
        sys.stderr.write("ERROR: Output filename [%s] cannot be the same as the output temporary filename [%s]\n" % (args.output_final_fn, args.output_temp_fn))
        sys.exit(errno.EINVAL)

    # parse args
    if args.slurm_memory:
        slurm_memory = args.slurm_memory

    if args.slurm_partition:
        slurm_partition = args.slurm_partition

    if args.slurm_workdir:
        slurm_workdir = args.slurm_workdir

    if args.slurm_output:
        slurm_output = args.slurm_output

    if args.slurm_error:
        slurm_error = args.slurm_error

    if not slurm_output:
        slurm_output = slurm_workdir

    if not slurm_error:
        slurm_error = slurm_workdir

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

    # build a list of chromosomes upon which to do work
    list_chromosome_cmd_components = [
        bedextract_path,
        '--list-chr',
        args.input_original_fn
    ]
    try:
        list_chromosome_cmd_result = subprocess.check_output(list_chromosome_cmd_components)
    except subprocess.CalledProcessError as err:
        list_chromosome_cmd_result = "ERROR: Command '{}' returned with error (code {}): {}".format(err.cmd, err.returncode, err.output)
        raise
    chromosome_list = list_chromosome_cmd_result.decode('utf-8').rstrip('\n').split('\n')

    # fire up the mid-range saloons^H^H^H per-chromosome sort tasks
    job_prefix = ''.join(random.choice(string.lowercase) for x in range(8))
    per_chr_job_ids = []
    per_chr_job_fns = []

    for chromosome in chromosome_list:
        temp_dest = os.path.join(os.getcwd(), '_'.join([job_prefix, chromosome]))
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
            '"srun ' + bedextract_path + ' ' + chromosome + ' ' + args.input_original_fn + ' | ' + sort_bed_path + ' - > ' + temp_dest + '"'
        ]
        try:
            per_chromosome_process = subprocess.Popen(' '.join(per_chromosome_sort_cmd_components),
                                                      shell=True,
                                                      stdin=subprocess.PIPE,
                                                      stdout=subprocess.PIPE,
                                                      stderr=subprocess.STDOUT,
                                                      close_fds=True)
            (per_chromosome_stdout, per_chromosome_stderr) = per_chromosome_process.communicate()
            per_chr_job_ids.append(per_chromosome_stdout.decode('utf-8').rstrip('\n'))
            per_chr_job_fns.append(temp_dest)
        except subprocess.CalledProcessError as err:
            sys.stderr.write("ERROR: Per-chromosome sort task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
            sys.exit(errno.EINVAL)

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
        per_chr_dependencies,
        '--wrap',
        '"srun cat ' + ' '.join(per_chr_job_fns) + ' > ' + args.output_temp_fn + '"'
    ]
    try:
        concatenation_process = subprocess.Popen(' '.join(concatenation_cmd_components),
                                                 shell=True,
                                                 stdin=subprocess.PIPE,
                                                 stdout=subprocess.PIPE,
                                                 stderr=subprocess.STDOUT,
                                                 close_fds=True)
        (concatenation_stdout, concatenation_stderr) = concatenation_process.communicate()
        concatenation_job_id = concatenation_stdout.decode('utf-8').rstrip('\n')
        concatenation_job_dependency_string = 'afterok:' + concatenation_job_id
    except subprocess.CalledProcessError as err:
        sys.stderr.write("ERROR: Concatenation task submission [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
        sys.exit(errno.EINVAL)

    # move original input file to backup file
    per_chr_dependencies = 'afterok:' + ':'.join(per_chr_job_ids)
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
    try:
        backup_input_process = subprocess.Popen(' '.join(backup_input_cmd_components),
                                                shell=True,  
                                                stdin=subprocess.PIPE,
                                                stdout=subprocess.PIPE,
                                                stderr=subprocess.STDOUT,
                                                close_fds=True)
        (backup_input_stdout, backup_input_stderr) = backup_input_process.communicate()
        backup_input_job_id = backup_input_stdout.decode('utf-8').rstrip('\n')
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
    try:
        output_move_process = subprocess.Popen(' '.join(output_move_cmd_components),
                                               shell=True,
                                               stdin=subprocess.PIPE,
                                               stdout=subprocess.PIPE,
                                               stderr=subprocess.STDOUT,
                                               close_fds=True)
        (output_move_stdout, output_move_stderr) = output_move_process.communicate()
        output_move_job_id = output_move_stdout.decode('utf-8').rstrip('\n')
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
        cleanup_move_process = subprocess.Popen(' '.join(output_move_cmd_components),
                                                shell=True,
                                                stdin=subprocess.PIPE,
                                                stdout=subprocess.PIPE,
                                                stderr=subprocess.STDOUT,
                                                close_fds=True)
        (output_move_stdout, output_move_stderr) = output_move_process.communicate()
        cleanup_job_id = cleanup_stdout.decode('utf-8').rstrip('\n')
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
