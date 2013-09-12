#!/usr/bin/env python

#
# Author:       Alex Reynolds
#
# Project:      Converts 1-based, closed [start, end] VCF v4 input
#               into 0-based, half-open [start-1, end) extended BED
#               and thence compressed into a BEDOPS Starch archive sent
#               to standard output.
#
# Version:      2.2
#
# Notes:        This conversion script relies on the VCF v4 format, with its
#               specifications outlined here by the 1000 Genomes project:
#
#               http://www.1000genomes.org/wiki/Analysis/Variant%20Call%20Format/vcf-variant-call-format-version-41
#
#               -- The "meta-information" (starting with '##') and "header"
#                  lines (starting with '#') are discarded.
#
#               -- The header line must be tab-delimited. The eight, fixed mandatory
#                  columns are converted to BED data as follows:
#
#                  - Data in the #CHROM column are mapped to the
#                    first column of the BED output
#                  - The POS column is mapped to the second and third
#                    BED columns
#                  - The ID and QUAL columns are mapped to the fourth
#                    and fifth BED columns, respectively
#                  - The REF, ALT, FILTER and INFO are mapped to the sixth
#                    through ninth BED columns, respectively
#
#               -- If present, genotype data in FORMAT and subsequence sample IDs
#                  are placed into tenth and subsequent columns
#
#               -- Data rows must also be tab-delimited.
#
#               -- Any missing data or non-standard delimiters may cause
#                  problems. It may be useful to validate the VCF v4 input
#                  before conversion.
#
#               Example usage:
#
#               $ vcf2starch < foo.vcf > sorted-foo.vcf.bed.starch
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ vcf2starch --do-not-sort < foo.vcf > unsorted-foo.vcf.bed.starch
#

import getopt, sys, os, stat, subprocess

def printUsage(stream):
    usage = ("Usage:\n"
             "  %s [ --help ] [ --do-not-sort | --max-mem <value> ] [ --starch-format <bzip2|gzip> ] < foo.vcf > sorted-foo.vcf.bed.starch\n\n"
             "Options:\n"
             "  --help                        Print this help message and exit\n"
             "  --do-not-sort                 Do not sort converted data with BEDOPS sort-bed\n"
             "  --max-mem <value>             Sets aside <value> memory for sorting BED output.\n"
             "                                For example, <value> can be 8G, 8000M or 8000000000\n"
             "                                to specify 8 GB of memory (default: 2G).\n"
             "  --starch-format <bzip2|gzip>  Specify backend compression format of starch\n"
             "                                archive (default: bzip2).\n\n"             
             "About:\n"
             "  This script converts 1-based, closed [a,b] VCF v4 data from standard input\n"
             "  into 0-based, half-open [a-1,b) extended BED, sorted and thence made into a BEDOPS\n"
             "  Starch archive sent to standard output.\n\n"
             "  This conversion script relies on the VCF v4 format, with its\n"
             "  specifications outlined here by the 1000 Genomes project:\n\n"
             "  http://www.1000genomes.org/wiki/Analysis/Variant%%20Call%%20Format/vcf-variant-call-format-version-41\n\n"
             "  -- The 'meta-information' (starting with '##') and 'header'\n"
             "     lines (starting with '#') are discarded.\n\n"
             "  -- The header line must be tab-delimited. The eight, fixed mandatory\n"
             "     columns are converted to BED data as follows:\n\n"
             "     - Data in the #CHROM column are mapped to the first column of the BED output\n"
             "     - The POS column is mapped to the second and third BED columns\n"
             "     - The ID and QUAL columns are mapped to the fourth and fifth BED columns, respectively\n"
             "     - The REF, ALT, FILTER and INFO are mapped to the sixth through ninth BED columns, respectively\n\n"
             "  -- If present, genotype data in FORMAT and subsequence sample IDs\n"
             "     are placed into tenth and subsequent columns\n\n"
             "  -- Data rows must also be tab-delimited.\n\n"
             "  -- Any missing data or non-standard delimiters may cause\n"
             "     problems. It may be useful to validate the VCF v4 input\n"
             "     before conversion.\n\n"
             "Example:\n"
             "  $ %s < foo.vcf > sorted-foo.vcf.bed.starch\n\n"
             "  We make no assumptions about sort order from converted output. Apply\n"
             "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n"
             "  which generates lexicographically-sorted BED data as output.\n\n"
             "  If you want to skip sorting, use the --do-not-sort option:\n\n"
             "  $ %s --do-not-sort < foo.vcf > unsorted-foo.vcf.bed.starch\n\n"
             % (sys.argv[0], sys.argv[0], sys.argv[0]) )
    if stream is "stdout":
        sys.stdout.write(usage)
    elif stream is "stderr":
        sys.stderr.write(usage)
    return 0

def checkInstallation(rv):
    currentVersion = sys.version_info
    if currentVersion[0] == rv[0] and currentVersion[1] >= rv[1]:
        pass
    else:
        sys.stderr.write( "[%s] - Error: Your Python interpreter must be %d.%d or greater (within major version %d)\n" % (sys.argv[0], rv[0], rv[1], rv[0]) )
        sys.exit(-1)
    return 0

def main(*args):
    requiredVersion = (2,5)
    checkInstallation(requiredVersion)

    sortOutput = True
    maxMem = "2G"
    maxMemChanged = False
    starchFormat = "bzip2"

    optstr = ""
    longopts = ["help", "do-not-sort", "max-mem=", "starch-format="]
    try:
        (options, args) = getopt.getopt(sys.argv[1:], optstr, longopts)
    except getopt.GetoptError as error:
        sys.stderr.write( "[%s] - Error: %s\n" % (sys.argv[0], str(error)) )
        printUsage("stderr")
        return -1
    for key, value in options:
        if key in ("--help"):
            printUsage("stdout")
            return 0
        elif key in ("--do-not-sort"):
            sortOutput = False
        elif key in ("--max-mem"):
            maxMem = str(value)
            maxMemChanged = True
        elif key in ("--starch-format"):
            starchFormat = str(value)

    starchFormat = "--" + starchFormat            

    if maxMemChanged and not sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --max-mem parameters\n" % sys.argv[0] )
        printUsage("stderr")
        return -1

    mode = os.fstat(0).st_mode
    inputIsNotAvailable = True
    if stat.S_ISFIFO(mode) or stat.S_ISREG(mode):
        inputIsNotAvailable = False
    if inputIsNotAvailable:
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in VCF-formatted data\n" % sys.argv[0] )
        printUsage("stderr")
        return -1

    if sortOutput:
        sortProcess = subprocess.Popen(' '.join(['sort-bed', '--max-mem', maxMem, '-', '|', 'starch', starchFormat, '-']), stdin=subprocess.PIPE, shell=True)
    else:
        starchProcess = subprocess.Popen(' '.join(['starch', starchFormat, '-']), stdin=subprocess.PIPE, shell=True)
            
    for line in sys.stdin:
        chomped_line = line.rstrip(os.linesep)
        if chomped_line.startswith('##'):
            pass
        elif chomped_line.startswith('#'):
            columns = chomped_line.split('\t')
        else:
            elems = chomped_line.split('\t')
            metadata = dict()
            try:
                for columnIdx in range(len(columns)):
                    metadata[columns[columnIdx]] = elems[columnIdx];
            except IndexError:
                print 'ERROR: Could not map data values to VCF header keys (perhaps missing or bad delimiters in header line?)'
                raise
            try:
                elem_chr = metadata['#CHROM']
                elem_start = str(int(metadata['POS']) - 1)
                elem_stop = str(metadata['POS'])
                elem_id = metadata['ID']
                elem_score = str(metadata['QUAL'])
                elem_ref = metadata['REF']
                elem_alt = metadata['ALT']
                elem_filter = metadata['FILTER']
                elem_info = metadata['INFO']
            except KeyError:
                print 'ERROR: Could not map data value from VCF header key (perhaps missing or bad delimiters in header line or data row?)'
                raise
            try:
                elem_genotype = '\t'.join(elems[8:])
            except IndexError:
                pass
            if not elem_genotype:
                line = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info])
            else:
                line = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info, elem_genotype])

            line = line + '\n'                
            if sortOutput:
                sortProcess.stdin.write(line)
                sortProcess.stdin.flush()
            else:
                starchProcess.stdin.write(line)
                starchProcess.stdin.flush()
                
    if sortOutput:
        sortProcess.stdin.close()
        sortProcess.wait()
    else:
        starchProcess.stdin.close()
        starchProcess.wait()
        
    return 0

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
