#!/usr/bin/env bash

#
#    BEDOPS
#    Copyright (C) 2011, 2012, 2013, 2014 Shane Neph, Scott Kuehn and Alex Reynolds
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

#
# Author:       Scott Kuehn, Shane Neph (original C code) and Alex Reynolds (bash wrapper)
#
# Project:      Convert UCSC Wiggle to UCSC BED
#
# Version:      2.4.2
#
# Notes:        The UCSC Wiggle format (http://genome.ucsc.edu/goldenPath/help/wiggle.html)
#               is 1-based, closed [a, b] and is offered in variable or fixed step varieties.
#               We convert either variety to 0-based, half-open [a-1, b) indexing when creating 
#               BED output.
#
#               By default, data are passed to sort-bed to provide sorted output
#               ready for use with other BEDOPS utilities.
#
#               Example usage:
#
#               $ wig2bed < foo.wig > sorted-foo.wig.bed
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output. 
# 
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ wig2bed --do-not-sort < foo.wig > unsorted-foo.wig.bed
#

printUsage () {
    cat <<EOF
Usage:
  wig2bed [ --help ] [ --keep-header ] [ --do-not-sort | --max-mem <value> (--sort-tmpdir <dir>) ] [ --multisplit <basename> ] < foo.wig

Options:
  --help                   Print this help message and exit
  --do-not-sort            Do not sort converted data with BEDOPS sort-bed
  --max-mem <value>        Sets aside <value> memory for sorting BED output. For example,
                           <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory
                           (default: 2G).
  --sort-tmpdir <dir>      Optionally sets <dir> as temporary directory for sort data, when
                           used in conjunction with --max-mem <value>. For example, <dir> can
                           be $PWD to store intermediate sort data in the current working
                           directory, in place of the host operating system default
                           temporary directory.
  --keep-header            Preserve header and metadata as BED elements (also works well with 
                           --multisplit <basename> option).
  --multisplit <basename>  A single input file may have multiple wig sections, a user may
                           pass in more than one file, or both may occur. With this option, 
                           every separate input goes to a separate output, starting with 
                           <basename>.1, then <basename>.2, and so on.

About:
  This script converts UCSC WIG data to lexicographically-sorted BED. The WIG format is a  
  1-based, closed [a, b] file. We convert this indexing back to 0-based, half-open when creating 
  BED output.

  If you use the --keep-header option, any header fields are retained as BED elements that use
  the chromosome name "_header". Otherwise, these are stripped from the output. 

  If your data have multiple WIG sections, be sure to use the --multisplit <basename> option. You
  can use the --keep-header option with --multisplit.

Example:
  $ wig2bed < foo.wig > sorted-foo.wig.bed

  We make no assumptions about sort order from converted output. Apply the usage case displayed
  to pass data to the BEDOPS sort-bed application, which generates lexicographically-sorted BED
  data as output.

  If you want to skip sorting, use the --do-not-sort option:

  $ wig2bed --do-not-sort < foo.wig > unsorted-foo.wig.bed

  We strongly advise against using the --do-not-sort option unless you know the sort order of 
  your WIG input with complete certainty.

  You can also pipe data into wig2bed, using the hyphen ("-") to denote standard input:

  $ someProcessThatMakesWigData | wig2bed - > someData.bed

EOF
}

# default sort-bed memory usage
maxMem="2G"

# default temporary sort data directory
sortTmpdir="/tmp"

# default multisplit basename
multisplitBasename="foo"

# default flags
convertWithoutSortingFlag=false
multisplitFlag=false
maxMemFlag=false
keepHeaderFlag=false
sortTmpdirFlag=false

# cf. http://stackoverflow.com/a/7680682/19410
optspec="hkm-:"
while getopts "$optspec" optchar; do
    case "${optchar}" in
        -)
            case "${OPTARG}" in
                help)
                    printUsage;
                    exit 0;
                    ;;
                keep-header)
                    keepHeaderFlag=true;
                    ;;
                do-not-sort)
                    convertWithoutSortingFlag=true;
                    ;;
                max-mem)
                    maxMemFlag=true;
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[wig2bed] - Error: Must specify value for --max-mem" >&2
                        printUsage;
                        exit -1;
                    fi
                    maxMem="${val}";
                    ;;
                sort-tmpdir)
                    sortTmpdirFlag=true;
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[wig2bed] - Error: Must specify value for --sort-tmpdir" >&2
                        printUsage;
                        exit -1;
                    fi
                    sortTmpdir="${val}";
                    ;;
                multisplit)
                    multisplitFlag=true;
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[wig2bed] - Error: Must specify value for --multisplit" >&2
                        printUsage;
                        exit -1;
                    fi
                    multisplitBasename="${val}";
                    ;;
                *)
                    if [ "$OPTERR" == 1 ] && [ "${optspec:0:1}" != ":" ]; then
                        echo "[wig2bed] - Error: Unknown option --${OPTARG}" >&2
                        printUsage;
                        exit -1;
                    fi
                    ;;
            esac;;
        *)
            if [ "$OPTERR" != 1 ] || [ "${optspec:0:1}" = ":" ]; then
                echo "[wig2bed] - Error: Non-option argument: '-${OPTARG}'" >&2
                printUsage;
                exit -1;
            fi
            ;;
    esac
done

if [ -t 0 ]; then
    echo "[wig2bed] - Error: Must send file or pipe in data via standard input -- see usage:" >&2
    printUsage;
    exit -1;
fi

if ${sortTmpdirFlag} && ! ${maxMemFlag}; then
    echo "[wig2bed] - Error: Must specify --max-mem when using --sort-tmpdir -- see usage:" >&2
    printUsage;
    exit -1;
fi

if ${sortTmpdirFlag}; then
    if [ ! -d ${sortTmpdir} ] || [ ! -w ${sortTmpdir} ]; then 
        echo "[wig2bed] - Error: Temporary sort data directory specified with --sort-tmpdir is a file, is non-existent, or its permissions do not allow access -- see usage:" >&2
        printUsage;
        exit -1;
    fi
fi

command -v wig2bed_bin > /dev/null 2>&1 || { echo "[wig2bed] - Error: Could not find wig2bed_bin binary" >&2; exit -1; }
if ${convertWithoutSortingFlag}; then
    if ${multisplitFlag}; then
        if ${keepHeaderFlag}; then
            wig2bed_bin --keep-header --multisplit ${multisplitBasename} - 
        else 
            wig2bed_bin --multisplit ${multisplitBasename} -            
        fi
    else
        if ${keepHeaderFlag}; then
            wig2bed_bin --keep-header -
        else
            wig2bed_bin -
        fi
    fi
else
    command -v sort-bed > /dev/null 2>&1 || { echo "[wig2bed] - Error: Could not find sort-bed binary" >&2; exit -1; }
    sortTmpdirStr=""
    if ${sortTmpdirFlag}; then
        sortTmpdirStr="--tmpdir ${sortTmpdir}"
    fi
    if ${multisplitFlag}; then
        if ${keepHeaderFlag}; then
            wig2bed_bin --keep-header --multisplit ${multisplitBasename} - | sort-bed --max-mem ${maxMem} ${sortTmpdirStr} -
        else
            wig2bed_bin --multisplit ${multisplitBasename} - | sort-bed --max-mem ${maxMem} ${sortTmpdirStr} -
        fi
    else
        if ${keepHeaderFlag}; then
            wig2bed_bin --keep-header - | sort-bed --max-mem ${maxMem} ${sortTmpdirStr} -
        else
            wig2bed_bin - | sort-bed --max-mem ${maxMem} ${sortTmpdirStr} -
        fi
    fi  
fi

exit 0;
