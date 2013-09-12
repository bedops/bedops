#!/usr/bin/env python

#
# Author:       Alex Reynolds
#
# Project:      Converts 1-based, closed [a,b] GFF3 input
#               into 0-based, half-open [a-1,b) six-column extended BED
#               and thence compressed into a BEDOPS Starch archive sent
#               to standard output.
#
# Version:      2.2
#
# Notes:        The GFF3 specification (http://www.sequenceontology.org/gff3.shtml) 
#               contains columns that do not map directly to common or UCSC BED columns.
#               Therefore, we add the following columns to preserve the ability to 
#               seamlessly convert back to GFF3 after performing operations with 
#               bedops, bedmap, or other BEDOPS or BED-processing tools.
#
#               - The 'source' GFF column data maps to the 7th BED column
#               - The 'type' data maps to the 8th BED column
#               - The 'phase' data maps to the 9th BED column
#               - The 'attributes' data maps to the 10th BED column
#
#               We make the following assumptions about the GFF3 input data:
#
#               - The 'seqid' GFF column data maps to the chromosome label (1st BED column)
#               - The 'ID' attribute in the 'attributes' GFF column (if present) maps to 
#                 the element ID (4th BED column)
#               - The 'score' and 'strand' GFF columns (if present) are mapped to the
#                 5th and 6th BED columns, respectively
#
#               If we encounter zero-length insertion elements (which are defined
#               where the start and stop GFF column data values are equivalent), the
#               start coordinate is decremented to convert to 0-based, half-open indexing, 
#               and a 'zero_length_insertion' attribute is added to the 'attributes' GFF 
#               column data.
#
#               Example usage:
#
#               $ gff2starch < foo.gff > sorted-foo.gff.bed.starch
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ gff2starch --do-not-sort < foo.gff > unsorted-foo.gff.bed.starch
#

import getopt, sys, os, stat, subprocess

def printUsage(stream):
    usage = ("Usage:\n"
             "  %s [ --help ] [ --do-not-sort | --max-mem <value> ] [ --starch-format <bzip2|gzip> ] < foo.gff > sorted-foo.gff.bed.starch\n\n"
             "Options:\n"
             "  --help                        Print this help message and exit\n"
             "  --do-not-sort                 Do not sort converted data with BEDOPS sort-bed\n"
             "  --max-mem <value>             Sets aside <value> memory for sorting BED output.\n"
             "                                For example, <value> can be 8G, 8000M or 8000000000\n"
             "                                to specify 8 GB of memory (default: 2G).\n"
             "  --starch-format <bzip2|gzip>  Specify backend compression format of starch\n"
             "                                archive (default: bzip2).\n\n"
             "About:\n"
             "  This script converts 1-based, closed [a,b] GFF3 data from standard\n"
             "  input into 0-based, half-open [a-1,b) six-column extended BED, sorted and\n"
             "  thence made into a BEDOPS Starch archive sent to standard output.\n\n"
             "  The GFF3 specification (http://www.sequenceontology.org/gff3.shtml)\n"
             "  contains columns that do not map directly to common or UCSC BED columns.\n"
             "  Therefore, we add the following columns to preserve the ability to\n"
             "  seamlessly convert back to GFF3 after performing operations with\n"
             "  bedops, bedmap, or other BEDOPS or BED-processing tools.\n\n"
             "  - The 'source' GFF column data maps to the 7th BED column\n"
             "  - The 'type' data maps to the 8th BED column\n"
             "  - The 'phase' data maps to the 9th BED column\n"
             "  - The 'attributes' data maps to the 10th BED column\n\n"
             "  We make the following assumptions about the GFF3 input data:\n\n"
             "  - The 'seqid' GFF column data maps to the chromosome label (1st BED column)\n"
             "  - The 'ID' attribute in the 'attributes' GFF column (if present) maps to\n"
             "    the element ID (4th BED column)\n"
             "  - The 'score' and 'strand' GFF columns (if present) are mapped to the\n"
             "    5th and 6th BED columns, respectively\n\n"
             "  If we encounter zero-length insertion elements (which are defined\n"
             "  where the start and stop GFF column data values are equivalent), the\n"
             "  start coordinate is decremented to convert to 0-based, half-open indexing,\n"
             "  and a 'zero_length_insertion' attribute is added to the 'attributes' GFF\n"
             "  column data.\n\n"
             "Example:\n"
             "  $ %s < foo.gff > sorted-foo.gff.bed.starch\n\n"
             "  We make no assumptions about sort order from converted output. Apply\n"
             "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n"
             "  which generates lexicographically-sorted BED data as output.\n\n"
             "  If you want to skip sorting, use the --do-not-sort option:\n\n"
             "  $ %s --do-not-sort < foo.gff > unsorted-foo.gff.bed.starch\n\n"
             % (sys.argv[0], sys.argv[0], sys.argv[0]))
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
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in GFF-formatted data\n" % sys.argv[0] )
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
        else:
            elems = chomped_line.split('\t')
            cols = dict()
            cols['seqid'] = elems[0].lstrip(' ') # strip leading whitespace
            cols['source'] = elems[1]
            cols['type'] = elems[2]
            cols['start'] = int(elems[3])
            cols['end'] = int(elems[4])
            cols['score'] = elems[5]
            cols['strand'] = elems[6]
            cols['phase'] = elems[7]
            cols['attributes'] = elems[8].rstrip(' ') # strip trailing whitespace

            attrd = dict()
            attrs = map(lambda s: s.split('='), cols['attributes'].split(';'))
            for attr in attrs:
                attrd[attr[0]] = attr[1]

            cols['chr'] = cols['seqid']
            try:
                cols['id'] = attrd['ID']
            except KeyError:
                cols['id'] = '.'

            if cols['start'] == cols['end']:
                cols['start'] -= 1
                cols['attributes'] = ';'.join([cols['attributes'], "zero_length_insertion=True"])
            else:
                cols['start'] -= 1

            line = '\t'.join([cols['chr'], 
                              str(cols['start']),
                              str(cols['end']),
                              cols['id'],
                              cols['score'],
                              cols['strand'],
                              cols['source'],
                              cols['type'],
                              cols['phase'],
                              cols['attributes']])

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
