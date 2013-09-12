#!/usr/bin/env bash

#
# Author:       Eric Haugen and Alex Reynolds
#
# Project:      Convert BAM to BEDOPS Starch
#
# Version:      2.2
#
# Notes:        The BAM format is an indexed, binary representation of a SAM (Sequence 
#               Alignment/Map) file. Internally, it is a 0-based, half-open [a-1,b) 
#               file, but printing it to text via samtools turns it into a SAM file, which
#               is 1-based, closed [a,b]. We convert this indexing back to 0-based, half-
#               open when creating BED output. This output is passed to BEDOPS starch to 
#               create the archive.
#
#               We process mappable SAM columns (as described by 
#               http://samtools.sourceforge.net/SAM1.pdf) converting them into the first 
#               six UCSC BED columns as follows:
#
#               - RNAME                     <-->   chromosome (1st column)
#               - POS - 1                   <-->   start (2nd column)
#               - POS + length(CIGAR) - 1   <-->   stop (3rd column)
#               - QNAME                     <-->   id (4th column)
#               - FLAG                      <-->   score (5th column)
#               - 16 & FLAG                 <-->   strand (6th column)
#
#               The remaining SAM columns are mapped intact, in order, to adjacent BED 
#               columns:
#
#               - MAPQ
#               - CIGAR
#               - RNEXT
#               - PNEXT
#               - TLEN
#               - SEQ
#               - QUAL
#
#               Because we have mapped all columns, we can translate converted BED data back
#               to headerless SAM reads with a simple awk statement or other script that 
#               calculates 1-based coordinates and permutes columns.
#
#               Example usage:
#
#               $ bam2starch < foo.bam > sorted-foo.bam.bed.starch
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output. 
# 
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ bam2starch --do-not-sort < foo.bam > unsorted-foo.bam.bed.starch
#

printUsage () {
    cat <<EOF
Usage:
  bam2starch [ --help ] [ --do-not-sort | --max-mem <value> ] < foo.bam > sorted-foo.bam.bed.starch

Options:
  --help                        Print this help message and exit
  --do-not-sort                 Do not sort converted data with BEDOPS sort-bed
  --max-mem <value>             Sets aside <value> memory for sorting BED output. For example,
                                <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory
                                (default: 2G).
  --starch-format <bzip2|gzip>  Specify backend compression format of starch archive
                                (default: bzip2)                                

About:
  This script converts BAM data to lexicographically-sorted BED. The BAM format is an indexed, 
  binary representation of a SAM (Sequence Alignment/Map) file. Internally, it is a 0-based, 
  half-open [a-1,b) file, but printing it to text via samtools turns it into a SAM file, which 
  is 1-based and closed [a,b]. We convert this indexing back to 0-based, half-open when creating 
  BED output. This output is passed to BEDOPS starch to create the archive.

  We process mappable SAM columns (as described by http://samtools.sourceforge.net/SAM1.pdf) 
  converting them into the first six UCSC BED columns as follows:

  - RNAME                     <-->   chromosome (1st column)
  - POS - 1                   <-->   start (2nd column)
  - POS + length(CIGAR) - 1   <-->   stop (3rd column)
  - QNAME                     <-->   id (4th column)
  - FLAG                      <-->   score (5th column)
  - 16 & FLAG                 <-->   strand (6th column)

  The remaining SAM columns are mapped intact, in order, to adjacent BED columns:

  - MAPQ
  - CIGAR
  - RNEXT
  - PNEXT
  - TLEN
  - SEQ
  - QUAL

  Because we have mapped all columns, we can translate converted BED data back to headerless
  SAM reads with a simple awk statement or other script that calculates 1-based coordinates and 
  permutes columns.

Example:
  $ bam2starch < foo.bam > sorted-foo.bam.bed.starch

  We make no assumptions about sort order from converted output. Apply the usage case displayed
  to pass data to the BEDOPS sort-bed application, which generates lexicographically-sorted BED
  data as output to the starch application.

  If you want to skip sorting, use the --do-not-sort option:

  $ bam2starch --do-not-sort < foo.bam > unsorted-foo.bam.bed.starch

EOF
}

convertAndArchiveWithoutSorting () {
    samtools view - \
        | gawk -F "\t" '{ \
            if ( ! and(4, $2) ) { \
                sval = $6; \
                gsub("[0-9]+[ISHP]", "", sval ); \
                gsub("[MDN=X]", "|", sval); \
                num = split(sval, a, "|"); \
                lng = 0; \
                for ( i = 1; i <= num; ++i ) { \
                    lng += int(a[i]); \
                } \
                printf "%s\t%s\t%d\t%s\t%s", $3, int($4) - 1, int($4) - 1 + lng, $1, $2; \
                if ( and(16, $2) ) { printf "\t-"; } else { printf "\t+"; } \
                for ( i = 5; i <= NF; ++i ) { \
                    printf "\t%s", $(i); \
                } \
                printf "\n"; \
            } \
        }' \
        | starch --${starchFormat} -
}

convertAndArchiveWithSorting () {
    samtools view - \
        | gawk -F "\t" '{ \
            if ( ! and(4, $2) ) { \
                sval = $6; \
                gsub("[0-9]+[ISHP]", "", sval ); \
                gsub("[MDN=X]", "|", sval); \
                num = split(sval, a, "|"); \
                lng = 0; \
                for ( i = 1; i <= num; ++i ) { \
                    lng += int(a[i]); \
                } \
                printf "%s\t%s\t%d\t%s\t%s", $3, int($4) - 1, int($4) - 1 + lng, $1, $2; \
                if ( and(16, $2) ) { printf "\t-"; } else { printf "\t+"; } \
                for ( i = 5; i <= NF; ++i ) { \
                    printf "\t%s", $(i); \
                } \
                printf "\n"; \
            } \
        }' \
        | sort-bed --max-mem ${maxMem} - \
        | starch --${starchFormat} -
}

# default sort-bed memory usage
maxMem="2G"

# default backend compression method
starchFormat="bzip2"

# cf. http://stackoverflow.com/a/7680682/19410
optspec=":hms-:"
while getopts "$optspec" optchar; do
    case "${optchar}" in
        -)
            case "${OPTARG}" in
                help)
                    printUsage;
                    exit 0;
                    ;;
                do-not-sort)
                    convertAndArchiveWithoutSorting;
                    exit 0;
                    ;;
                max-mem)
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[bam2bed] - Error: Must specify value for --max-mem" >&2
                        printUsage;
                        exit -1;
                    fi
                    maxMem="${val}";
                    ;;
                starch-format)
                    val="${!OPTIND}"; 
                    OPTIND=$(( $OPTIND + 1 ));
                    if [[ ! ${val} ]]; then
                        echo "[bam2bed] - Error: Must specify either 'bzip2' or 'gzip' value for --starch-format" >&2
                        printUsage;
                        exit -1;
                    fi
                    starchFormat="${val}";
                    ;;
                *)
                    if [ "$OPTERR" = 1 ] && [ "${optspec:0:1}" != ":" ]; then
                        echo "[bam2starch] - Error: Unknown option --${OPTARG}" >&2
                        printUsage;
                        exit -1;
                    fi
                    ;;
            esac;;
        *)
            if [ "$OPTERR" != 1 ] || [ "${optspec:0:1}" = ":" ]; then
                echo "[bam2starch] - Error: Non-option argument: '-${OPTARG}'" >&2
                printUsage;
                exit -1;
            fi
            ;;
    esac
done

convertAndArchiveWithSorting;
exit 0;
