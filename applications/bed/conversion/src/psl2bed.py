#!/usr/bin/env python

#
# Author:       Alex Reynolds
#
# Project:      Converts 0-based, half-open [a-1,b) headered or headerless PSL input
#               into 0-based, half-open [a-1,b) extended BED
#
# Version:      2.2
#
# Notes:        The PSL specification (http://genome.ucsc.edu/goldenPath/help/blatSpec.html)
#               contains 21 columns, some which map to UCSC BED columns and some which do not.
#
#               PSL input can contain a header or be headerless, if the BLAT search was
#               performed with the -noHead option. By default, we assume the PSL input is headerless.
#               If your PSL data contains a header, use the --headered option with this script.
#
#               We describe below how we map columns to BED, so that BLAT results can be losslessly
#               transformed back into PSL format with a simple awk statement or other similar
#               command that permutes columns into PSL-ordering.
#
#               We map the following PSL columns to their equivalent BED column:
#
#               - tName    <-->   chromosome
#               - tStart   <-->   start
#               - tEnd     <-->   stop
#               - qName    <-->   id
#               - qSize    <-->   score
#               - strand   <-->   strand
#
#               Remaining PSL columns are mapped, in order, to columns 7 through 21 in the
#               BED output:
#
#               - matches
#               - misMatches
#               - repMatches
#               - nCount
#               - qNumInsert
#               - qBaseInsert
#               - tNumInsert
#               - tBaseInsert
#               - qStart
#               - qEnd
#               - tSize
#               - blockCount
#               - blockSizes
#               - qStarts
#               - tStarts
#
#               To convert the output from this script back to headerless PSL, for example, a simple
#               awk statement will suffice:
#
#               $ awk 'BEGIN {OFS="\t"} { print $7" "$8" "$9" "$10" "$11" "$12" "$13" "$14" "$6" "$4" "$5" "$15" "$16" "$1" "$17" "$2" "$3" "$18" "$19" "$20" "$21 }' converted.bed > original.psl
#
#               Example usage:
#
#               $ psl2bed < headerless-foo.psl > sorted-headerless-foo.psl.bed
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ psl2bed --do-not-sort < headerless-foo.psl > unsorted-headerless-foo.psl.bed
#

import getopt, sys, os, stat, subprocess, signal

def printUsage(stream):
    usage = ("Usage:\n"
             "  %s [ --help ] [ --headered ] [ --do-not-sort | --max-mem <value> ] < foo.psl\n\n"
             "Options:\n"
             "  --help              Print this help message and exit\n"
             "  --headered          Convert headered PSL input to BED (default is headerless)\n"
             "  --do-not-sort       Do not sort converted data with BEDOPS sort-bed\n"
             "  --max-mem <value>   Sets aside <value> memory for sorting BED output. For example,\n"
             "                      <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory\n"
             "                      (default: 2G).\n\n"                          
             "About:\n"
             "  This script converts 0-based, half-open [a-1,b) PSL data from standard input into\n"
             "  0-based, half-open [a-1,b) extended BED, sent to standard output.\n\n"
             "  The PSL specification (http://genome.ucsc.edu/goldenPath/help/blatSpec.html)\n"
             "  contains 21 columns, some which map to UCSC BED columns and some which do not.\n\n"
             "  PSL input can contain a header or be headerless, if the BLAT search was\n"
             "  performed with the -noHead option. By default, we assume the PSL input is headerless.\n"
             "  If your PSL data contains a header, use the --headered option with this script.\n\n"
             "  We describe below how we map columns to BED, so that BLAT results can be losslessly\n"
             "  transformed back into PSL format with a simple awk statement or other similar\n"
             "  command that permutes columns into PSL-ordering.\n\n"
             "  We map the following PSL columns to their equivalent BED column:\n\n"
             "  - tName    <-->   chromosome\n"
             "  - tStart   <-->   start\n"
             "  - tEnd     <-->   stop\n"
             "  - qName    <-->   id\n"
             "  - qSize    <-->   score\n"
             "  - strand   <-->   strand\n\n"
             "  Remaining PSL columns are mapped, in order, to columns 7 through 21 in the\n"
             "  BED output:\n\n"
             "  - matches\n"
             "  - misMatches\n"
             "  - repMatches\n"
             "  - nCount\n"
             "  - qNumInsert\n"
             "  - qBaseInsert\n"
             "  - tNumInsert\n"
             "  - tBaseInsert\n"
             "  - tBaseInsert\n"
             "  - qStart\n"
             "  - qEnd\n"
             "  - tSize\n"
             "  - blockCount\n"
             "  - blockSizes\n"
             "  - qStarts\n"
             "  - tStarts\n\n"
             "  To convert the output from this script back to headerless PSL, for example, a simple\n"
             "  awk statement will suffice:\n\n"
             "  $ awk 'BEGIN { OFS=\"\\t\" } { print $7\" \"$8\" \"$9\" \"$10\" \"$11\" \"$12\" \"$13\" \"$14\" \"$6\" \"$4\" \"$5\" \"$15\" \"$16\" \"$1\" \"$17\" \"$2\" \"$3\" \"$18\" \"$19\" \"$20\" \"$21 }' converted.bed > original.psl\n\n"
             "Example:\n"
             "  $ %s < headerless-foo.psl > sorted-headerless-foo.psl.bed\n\n"
             "  We make no assumptions about sort order from converted output. Apply\n"
             "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n"
             "  which generates lexicographically-sorted BED data as output.\n\n"
             "  If you want to skip sorting, use the --do-not-sort option:\n\n"
             "  $ %s --do-not-sort < headerless-foo.psl > unsorted-headerless-foo.psl.bed\n\n"
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
    inputIsHeadered = False
    maxMem = "2G"
    maxMemChanged = False

    # fixes bug with Python handling of SIGPIPE signal from UNIX head, etc.
    # http://coding.derkeiler.com/Archive/Python/comp.lang.python/2004-06/3823.html
    signal.signal(signal.SIGPIPE,signal.SIG_DFL)

    optstr = ""
    longopts = ["do-not-sort", "headered", "help", "max-mem="]
    try:
        (options, args) = getopt.getopt(sys.argv[1:], optstr, longopts)
    except getopt.GetoptError as error:
        sys.stderr.write( "[%s] - Error: %s\n" % (sys.argv[0], str(error)) )
        printUsage("stderr")
        return -1
    for key, value in options:
        if key in ("--help"):
            printUsage("stdout")
            sys.exit(0)
        elif key in ("--headered"):
            inputIsHeadered = True
        elif key in ("--do-not-sort"):
            sortOutput = False
        elif key in ("--max-mem"):
            maxMem = str(value)
            maxMemChanged = True

    if maxMemChanged and not sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --max-mem parameters\n" % sys.argv[0] )
        printUsage("stderr")
        return -1            

    mode = os.fstat(0).st_mode
    inputIsNotAvailable = True
    if stat.S_ISFIFO(mode) or stat.S_ISREG(mode):
        inputIsNotAvailable = False
    if inputIsNotAvailable:
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in PSL-formatted data\n" % sys.argv[0] )
        printUsage("stderr")
        return -1

    lineCounter = 0
    
    if inputIsHeadered:
        headerLineCounter = 0
        for line in sys.stdin:
            if (headerLineCounter == 4):
                break
            headerLineCounter += 1
            lineCounter += 1

    if sortOutput:
        sortProcess = subprocess.Popen(['sort-bed', '--max-mem', maxMem, '-'], stdin=subprocess.PIPE)
        
    for line in sys.stdin:
        lineCounter += 1
        chomped_line = line.rstrip(os.linesep)
        elems = chomped_line.split('\t')
        cols = dict()
        try:
            cols['matches'] = str(int(elems[0]))
        except ValueError:
            sys.stderr.write ('[%s] - Error: Corrupt input on line %d? If input is headered, use the --headered option.\n' % (sys.argv[0], lineCounter) )
            printUsage("stderr")
            return -1
        cols['misMatches'] = str(int(elems[1]))
        cols['repMatches'] = str(int(elems[2]))
        cols['nCount'] = str(int(elems[3]))
        cols['qNumInsert'] = str(int(elems[4]))
        cols['qBaseInsert'] = str(int(elems[5]))
        cols['tNumInsert'] = str(int(elems[6]))
        cols['tBaseInsert'] = str(int(elems[7]))
        cols['strand'] = str(elems[8])
        cols['qName'] = str(elems[9])
        cols['qSize'] = str(int(elems[10]))
        cols['qStart'] = str(int(elems[11]))
        cols['qEnd'] = str(int(elems[12]))
        cols['tName'] = str(elems[13])
        cols['tSize'] = str(int(elems[14]))
        cols['tStart'] = str(int(elems[15]))
        cols['tEnd'] = str(int(elems[16]))
        cols['blockCount'] = str(int(elems[17]))
        cols['blockSizes'] = str(elems[18])
        cols['qStarts'] = str(elems[19])
        cols['tStarts'] = str(elems[20])

        if (int(cols['tStart']) >= int(cols['tEnd'])):
            sys.stderr.write ('[%s] - Error: Corrupt input on line %d? Start coordinate must be less than end coordinate.\n' % (sys.argv[0], lineCounter) )
            return -1

        line = '\t'.join([
            cols['tName'],
            cols['tStart'],
            cols['tEnd'],
            cols['qName'],
            cols['qSize'],
            cols['strand'],
            cols['matches'],
            cols['misMatches'],
            cols['repMatches'],
            cols['nCount'],
            cols['qNumInsert'],
            cols['qBaseInsert'],
            cols['tNumInsert'],
            cols['tBaseInsert'],
            cols['qStart'],
            cols['qEnd'],
            cols['tSize'],
            cols['blockCount'],
            cols['blockSizes'],
            cols['qStarts'],
            cols['tStarts']])

        if sortOutput:
            line = line + '\n'
            sortProcess.stdin.write(line)
            sortProcess.stdin.flush()
        else:
            sys.stdout.write( "%s\n" % line )

    if sortOutput:
        sortProcess.stdin.close()
        sortProcess.wait()
    
    return 0

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
