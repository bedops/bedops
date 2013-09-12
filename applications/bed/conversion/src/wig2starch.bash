#!/usr/bin/env bash

#
# Author:       Scott Kuehn, Shane Neph (original C code) and Alex Reynolds (bash wrapper)
#
# Project:      Convert UCSC Wiggle to UCSC BED and thence compressed into a BEDOPS Starch
#               archive sent to standard output.
#
# Version:      2.2
#
# Notes:        The UCSC Wiggle format (http://genome.ucsc.edu/goldenPath/help/wiggle.html)
#               is 1-based, closed [a,b] and is offered in variable or fixed step varieties.
#               We convert either variety to 0-based, half-open [a-1,b) indexing when creating 
#               BED output.
#
#               By default, data are passed to sort-bed to provide sorted output
#               ready for use with other BEDOPS utilities.
#
#               Example usage:
#
#               $ wig2starch < foo.wig > sorted-foo.wig.bed.starch
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output. 
# 
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ wig2starch --do-not-sort < foo.wig > unsorted-foo.wig.bed.starch
#

printUsage () {
    cat <<EOF
Usage:
  wig2starch [ --help ] [ --do-not-sort | --max-mem <value> ] [ --starch-format <bzip2|gzip> ] < foo.wig

Options:
  --help                        Print this help message and exit
  --do-not-sort                 Do not sort converted data with BEDOPS sort-bed
  --max-mem <value>             Sets aside <value> memory for sorting BED output. For example,
                                <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory
                                (default: 2G).
  --multisplit <basename>       A single input file may have multiple wig sections, a user may
                                pass in more than one file, or both may occur. With this option, 
                                every separate input goes to a separate output, starting with 
                                <basename>.1, then <basename>.2, and so on.
  --starch-format <bzip2|gzip>  Specify backend compression format of starch archive (default: 
                                bzip2).

About:
  This script converts UCSC WIG data to lexicographically-sorted BED. The WIG format is a  
  1-based, closed [a,b] file. We convert this indexing back to 0-based, half-open when creating 
  BED output, which is thence converted into a BEDOPS Starch archive sent to standard output.

Example:
  $ wig2starch < foo.wig > sorted-foo.wig.bed.starch

  We make no assumptions about sort order from converted output. Apply the usage case displayed
  to pass data to the BEDOPS sort-bed application, which generates lexicographically-sorted BED
  data as output.

  If you want to skip sorting, use the --do-not-sort option:

  $ wig2starch --do-not-sort < foo.wig > unsorted-foo.wig.bed.starch

  We strongly advise against using this option unless you know the sort order of your WIG input
  with certainty.

EOF
}

# default sort-bed memory usage
maxMem="2G"

# default multisplit basename
multisplitBasename="foo"

# default starch format
starchFormat="--bzip2"

# default flags
convertWithoutSortingFlag=false
multisplitFlag=false
maxMemFlag=false
starchFormatSpecifiedFlag=false

# cf. http://stackoverflow.com/a/7680682/19410
optspec=":hm-:"
while getopts "$optspec" optchar; do
    case "${optchar}" in
        -)
            case "${OPTARG}" in
                help)
                    printUsage;
                    exit 0;
                    ;;
                do-not-sort)
                    convertWithoutSortingFlag=true;
                    ;;
                max-mem)
                    maxMemFlag=true;
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[wig2starch] - Error: Must specify value for --max-mem" >&2
                        printUsage;
                        exit -1;
                    fi
                    maxMem="${val}";
                    ;;
                multisplit)
                    multisplitFlag=true;
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[wig2starch] - Error: Must specify value for --multisplit" >&2
                        printUsage;
                        exit -1;
                    fi
                    multisplitBasename="${val}";
                    ;;
                starch-format)
                    starchFormatSpecifiedFlag=true;
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[wig2starch] - Error: Must specify value for --starch-format" >&2
                        printUsage;
                        exit -1;
                    fi
                    starchFormat="--${val}";
                    ;;
                *)
                    if [ "$OPTERR" = 1 ] && [ "${optspec:0:1}" != ":" ]; then
                        echo "[wig2starch] - Error: Unknown option --${OPTARG}" >&2
                        printUsage;
                        exit -1;
                    fi
                    ;;
            esac;;
        *)
            if [ "$OPTERR" != 1 ] || [ "${optspec:0:1}" = ":" ]; then
                echo "[wig2starch] - Error: Non-option argument: '-${OPTARG}'" >&2
                printUsage;
                exit -1;
            fi
            ;;
    esac
done

if ${convertWithoutSortingFlag}; then
    if ${multisplitFlag}; then
        if ${starchFormatSpecifiedFlag}; then
            wig2bed_bin --multisplit ${multisplitBasename} - | starch ${starchFormat} - 
        else
            wig2bed_bin --multisplit ${multisplitBasename} - | starch - 
        fi
    else
        if ${starchFormatSpecifiedFlag}; then
            wig2bed_bin - | starch ${starchFormat} -
        else
            wig2bed_bin - | starch -
        fi
    fi
else
    if ${multisplitFlag}; then
        if ${starchFormatSpecifiedFlag}; then
            wig2bed_bin --multisplit ${multisplitBasename} - | sort-bed --max-mem ${maxMem} - | starch ${starchFormat} -
        else
            wig2bed_bin --multisplit ${multisplitBasename} - | sort-bed --max-mem ${maxMem} - | starch -
        fi
    else
        if ${starchFormatSpecifiedFlag}; then
            wig2bed_bin - | sort-bed --max-mem ${maxMem} - | starch ${starchFormat} -
        else
            wig2bed_bin - | sort-bed --max-mem ${maxMem} - | starch -
        fi
    fi  
fi

exit 0;
