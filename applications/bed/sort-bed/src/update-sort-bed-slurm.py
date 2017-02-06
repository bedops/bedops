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
version = "  version:  2.4.24"
usage = """  $ update-sort-bed-slurm [ --slurm-memory <MB> ] 
                          [ --slurm-partition <SLURM partition> ] 
                          [ --slurm-workdir <working directory> ]
                          [ --slurm-output <SLURM output directory> ]
                          [ --slurm-error <SLURM error directory> ]
                          --input <old-bed-file> --output <new-bed-file>"""
help = """
  The 'update-sort-bed-slurm' utility applies an updated sort order on BED files
  sorted per pre-v2.4.20 sort-bed, using a SLURM job scheduler to coordinate
  resorting each chromosome in --input per post-v2.4.20 sort-bed, and writing
  the result to --output.

  Each sort job is given 4GB of memory and is assigned to the 'queue0' 
  partition, unless the --slurm-memory and --slurm-partition options are used.

  Because this launches all work on the specified cluster partition, the paths
  specified by --input and --output must be accessible to all computational
  nodes. For example, using /tmp may fail, as the /tmp path is almost certainly
  unique to a node; it is necesssary to use a path shared among all nodes.

  Note that this utility will not work on entirely unsorted BED files, but only 
  on files with a sort order from pre-v2.4.20 sort-bed, where there are ties on 
  the first three columns. 

  In fact, until further refinements are made, this convenience utility could 
  fail silently on inputs which are not BED, or which are not sorted per pre-
  v2.4.20 order, or which do not follow exact specification, all of which can 
  lead a per-chromosome resort task to fail.
"""

def main():
    slurm_memory = "4000"
    slurm_partition = "queue0"
    slurm_workdir = os.getcwd()
    slurm_output = None
    slurm_error = None
    slurm_concatenation_memory = "500" # concatenation step does not require much memory
    
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help', '-h', action='store_true', dest='help')
    parser.add_argument('--slurm-memory', '-m', type=str, action="store", dest='slurm_memory')
    parser.add_argument('--slurm-partition', '-p', type=str, action="store", dest='slurm_partition')
    parser.add_argument('--slurm-workdir', '-w', type=str, action="store", dest='slurm_workdir')
    parser.add_argument('--slurm-output', '-u', type=str, action="store", dest='slurm_output')
    parser.add_argument('--slurm-error', '-e', type=str, action="store", dest='slurm_error')    
    parser.add_argument('--input', '-i', type=str, action="store", dest='input_fn')
    parser.add_argument('--output', '-o', type=str, action="store", dest='output_fn')
    args = parser.parse_args()

    if args.help or (not args.input_fn or not args.output_fn):
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

    if not os.path.exists(args.input_fn): 
        sys.stderr.write("ERROR: Input file [%s] does not exist\n" % (args.input_fn))
        sys.exit(errno.EINVAL)

    if os.path.exists(args.output_fn):
        sys.stderr.write("ERROR: Output file [%s] exists\n" % (args.output_fn))
        sys.exit(errno.EEXIST)

    if not cmd_exists('sbatch') or not cmd_exists('bedextract'):
        sys.stderr.write("ERROR: This script must be run on a system with SLURM and BEDOPS binaries available\n")
        sys.exit(errno.EEXIST)

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

    # build a list of chromosomes upon which to do work
    list_chromosome_cmd_components = [
        'bedextract',
        '--list-chr',
        args.input_fn
    ]
    try:
        list_chromosome_cmd_result = subprocess.check_output(list_chromosome_cmd_components)
    except subprocess.CalledProcessError as err:
        list_chromosome_cmd_result = "ERROR: Command '{}' returned with error (code {}): {}".format(err.cmd, err.returncode, err.output)
        raise
    chromosome_list = list_chromosome_cmd_result.rstrip('\n').split('\n')

    # fire up the mid-range saloons^H^H^H per-chromosome sort tasks
    job_prefix = ''.join(random.choice(string.lowercase) for x in range(8))
    job_ids = []
    job_fns = []
    local_environment = os.environ.copy()

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
            '"module add bedops; srun bedextract ' + chromosome + ' ' + args.input_fn + ' | sort-bed - > ' + temp_dest + '"'
        ]
        try:
            per_chromosome_process = subprocess.Popen(' '.join(per_chromosome_sort_cmd_components),
                                                      shell=True,
                                                      stdin=subprocess.PIPE,
                                                      stdout=subprocess.PIPE,
                                                      stderr=subprocess.STDOUT,
                                                      close_fds=True)
            (per_chromosome_stdout, per_chromosome_stderr) = per_chromosome_process.communicate()
            job_ids.append(per_chromosome_stdout.rstrip('\n'))
            job_fns.append(temp_dest)
        except subprocess.CalledProcessError as err:
            sys.stderr.write("ERROR: Command [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
            sys.exit(errno.EINVAL)

    # concatenate the resorted chromosomes into the output product and perform cleanup
    dependencies = 'afterok:' + ':'.join(job_ids)
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
        dependencies,
        '--wrap',
        '"srun cat ' + ' '.join(job_fns) + ' > ' + args.output_fn + ' && srun rm -f ' + os.path.join(slurm_workdir, job_prefix) + '_* ' + os.path.join(slurm_output, job_prefix) + '_* ' + os.path.join(slurm_error, job_prefix) + '_*"'
    ]
    try:
        concatenation_process = subprocess.Popen(' '.join(concatenation_cmd_components),
                                                 shell=True,
                                                 stdin=subprocess.PIPE,
                                                 stdout=subprocess.PIPE,
                                                 stderr=subprocess.STDOUT,
                                                 close_fds=True)
        (concatenation_stdout, concatenation_stderr) = concatenation_process.communicate()
        
    except subprocess.CalledProcessError as err:
        sys.stderr.write("ERROR: Command [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
        sys.exit(errno.EINVAL)

    # issue notice to stderr stream
    sys.stderr.write("Note: Run `$ sacct -j %s` to track job status of concatenation step\n" % (concatenation_stdout.rstrip('\n')))

def cmd_exists(cmd):
    return subprocess.call("type " + cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) == 0

if __name__ == "__main__":
    main()
