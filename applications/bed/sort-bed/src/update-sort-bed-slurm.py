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
usage = "  $ update-sort-bed-slurm [ --slurm-memory <MB> ] [ --slurm-partition <partition> ] --input <old-bed-file> --output <new-bed-file>"
help = """
  The 'update-sort-bed-slurm' utility applies an updated sort order on BED files
  sorted per pre-v2.4.20 sort-bed, using a SLURM job scheduler to coordinate
  per-chromosome resorting tasks.

  Each sort job is given 4GB of memory and is assigned to the 'queue0' 
  partition, unless the --slurm-memory and --slurm-partition options are used.

  Note that this will not work on entirely unsorted BED files, but only on
  files with a sort order from pre-v2.4.20 sort-bed.
"""

slurm_memory = "4000"
slurm_partition = "queue0"
slurm_options = "--parsable --workdir=" + os.getcwd() + " --output=/dev/null --error=/dev/null"

def main():
    parser = argparse.ArgumentParser(prog=name, usage=usage, add_help=False)
    parser.add_argument('--help', '-h', action='store_true', dest='help')
    parser.add_argument('--slurm-memory', '-m', type=str, action="store", dest=slurm_memory)
    parser.add_argument('--slurm-partition', '-p', type=str, action="store", dest=slurm_partition)
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

    # get list of chromosomes
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

    # fire up per-chromosome sorts
    job_prefix = ''.join(random.choice(string.lowercase) for x in range(8))
    job_ids = []
    job_fns = []
    for chromosome in chromosome_list:
        temp_dest = os.path.join(os.getcwd(), '_'.join([job_prefix, chromosome]))
        per_chromosome_sort_cmd_components = [
            'sbatch',
            slurm_options,
            '--mem',
            slurm_memory,
            '--partition',
            slurm_partition,
            '--wrap="module add bedops; bedextract ' + chromosome + ' ' + args.input_fn + ' | starch - > ' + temp_dest + '"'
        ]
        try:
            per_chromosome_sort_cmd_result = subprocess.check_output(per_chromosome_sort_cmd_components)
            job_ids.append(per_chromosome_sort_cmd_result)
            job_fns.append(temp_dest)
        except subprocess.CalledProcessError as err:
            sys.stderr.write("ERROR: Command [%s] returned with error (code %d)\n" % (' '.join(err.cmd), err.returncode))
            sys.exit(errno.EINVAL)
    
    # union result
    dependencies = ':'.join(job_ids)
    union_cmd_components = [
        'sbatch',
        slurm_options,
        '--mem',
        slurm_memory,
        '--partition',
        slurm_partition,
        '--dependency=afterok:' + dependencies,
        '--wrap="module add bedops; bedops -u ' + ' '.join(job_fns) + ' > ' + args.output_fn + '; rm -f ' + ' '.join(job_fns) + '"'
    ]
    try:
        union_cmd_result = subprocess.check_output(union_cmd_components)
    except subprocess.CalledProcessError as err:
        union_cmd_result = "ERROR: Command '{}' returned with error (code {}): {}".format(err.cmd, err.returncode, err.output)
        raise

    sys.stderr.write("Note: Run `$ sacct -j %s` to track job status of final union\n" % (union_cmd_result))

def cmd_exists(cmd):
    return subprocess.call("type " + cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) == 0

if __name__ == "__main__":
    main()