#!/usr/bin/env python

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
# Author:       Alex Reynolds
#
# Project:      Converts 0-based, half-open [a-1, b) headered or headerless PSL input
#               into 0-based, half-open [a-1, b) extended BED and thence compressed into
#               a BEDOPS Starch archive sent to standard output.
#
# Version:      2.4.2
#
# Notes:        The PSL specification (http://genome.ucsc.edu/goldenPath/help/blatSpec.html)
#               contains 21 columns, some which map to UCSC BED columns and some which do not.
#
#               PSL input can contain a header or be headerless, if the BLAT search was
#               performed with the -noHead option. By default, we assume the PSL input is headerless.
#               If your PSL data contains a header, use the --headered option with this script.
#
#               If you use --headered, you can use the --keep-header option to preserve the header
#               data as pseudo-BED elements that use the "_header" chromosome name. We expect this
#               should not cause any collision problems since PSL data should use the UCSC chromosome
#               naming convention.
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
#               $ psl2starch < headerless-foo.psl > sorted-headerless-foo.psl.bed.starch
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ psl2starch --do-not-sort < headerless-foo.psl > unsorted-headerless-foo.psl.bed.starch
#

import getopt, sys, os, stat, subprocess, signal, threading

def which(program):
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def printUsage(stream):
    usage = ("Usage:\n"
             "  %s [ --help ] [ --keep-header ] [ --headered ] [ --do-not-sort | --max-mem <value> (--sort-tmpdir <dir>) ] [ --starch-format <bzip2|gzip> ] < foo.psl > sorted-foo.psl.bed.starch\n\n"
             "Options:\n"
             "  --help                        Print this help message and exit\n"
             "  --keep-header                 Preserve header information as pseudo-BED elements (requires --headered)\n"
             "  --do-not-sort                 Do not sort converted data with BEDOPS sort-bed\n"
             "  --max-mem <value>             Sets aside <value> memory for sorting BED output.\n"
             "                                For example, <value> can be 8G, 8000M or 8000000000\n"
             "                                to specify 8 GB of memory (default: 2G).\n"
             "  --sort-tmpdir <dir>           Optionally sets <dir> as temporary directory for sort data, when\n"
             "                                used in conjunction with --max-mem <value>. For example, <dir> can\n"
             "                                be $PWD to store intermediate sort data in the current working\n"
             "                                directory, in place of the host operating system default\n"
             "                                temporary directory.\n"
             "  --starch-format <bzip2|gzip>  Specify backend compression format of starch\n"
             "                                archive (default: bzip2).\n\n"
             "About:\n"
             "  This script converts 0-based, half-open [a-1, b) PSL data from standard input into\n"
             "  0-based, half-open [a-1, b) extended BED, sorted and thence made into a BEDOPS Starch\n"
             "  archive sent to standard output.\n\n"
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
             "  $ %s < headerless-foo.psl > sorted-headerless-foo.psl.bed.starch\n\n"
             "  We make no assumptions about sort order from converted output. Apply\n"
             "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n"
             "  which generates lexicographically-sorted BED data as output.\n\n"
             "  If you want to skip sorting, use the --do-not-sort option:\n\n"
             "  $ %s --do-not-sort < headerless-foo.psl > unsorted-headerless-foo.psl.bed.starch\n\n"
             % (sys.argv[0], sys.argv[0], sys.argv[0]))
    if stream is "stdout":
        sys.stdout.write(usage)
    elif stream is "stderr":
        sys.stderr.write(usage)
    return os.EX_OK
             
def checkInstallation(rv):
    currentVersion = sys.version_info
    if currentVersion[0] == rv[0] and currentVersion[1] > rv[1]:
        pass
    elif currentVersion[0] == rv[0] and currentVersion[1] == rv[1] and currentVersion[2] >= rv[2]:
        pass
    else:
        sys.stderr.write( "[%s] - Error: Your Python interpreter must be %d.%d.%d or greater (within major version %d)\n" % (sys.argv[0], rv[0], rv[1], rv[2], rv[0]) )
        sys.exit(os.EX_CONFIG)
    return os.EX_OK

def consumePSL(from_stream, to_stream, params):
    while True:
        psl_line = from_stream.readline()
        if not psl_line:
            from_stream.close()
            to_stream.close()
            break
        bed_line = convertPSLToBed(psl_line, params)
        # bed_line could be None if --headered but not --keep-header, so we only print to output stream if it is not None
        if bed_line: 
            try:
                to_stream.write(bed_line)
                to_stream.flush()
            except TypeError as te:
                sys.stderr.write( "[%s] - Error: Could not import PSL data (ensure input is PSL-formatted)\n" % (sys.argv[0]))
                to_stream.close()
                sys.exit(os.EX_DATAERR)

def consumeBED(from_stream, to_stream, params):
    while True:
        bed_line = from_stream.readline()
        if not bed_line:
            from_stream.close()
            to_stream.close()
            break
        to_stream.write(bed_line)
        to_stream.flush()

def convertPSLToBed(line, params):
    convertedLine = None

    params.lineCounter += 1

    if params.inputIsHeadered and params.keepHeader and params.lineCounter <= 5:
        elem_chr = params.keepHeaderChr
        elem_start = str(params.keepHeaderIdx)
        elem_stop = str(params.keepHeaderIdx + 1)
        elem_id = line
        convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id]) + '\n'
        params.keepHeaderIdx += 1

    elif params.inputIsHeadered and params.lineCounter <= 5:
        pass

    else:
        chomped_line = line.rstrip(os.linesep)
        elems = chomped_line.split('\t')
        cols = dict()
        try:
            cols['matches'] = str(int(elems[0]))
        except ValueError:
            sys.stderr.write ('[%s] - Error: Corrupt input on line %d? If input is headered, use the --headered option.\n' % (sys.argv[0], params.lineCounter) )
            printUsage("stderr")
            return os.EX_DATAERR

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
            sys.stderr.write ('[%s] - Error: Corrupt input on line %d? Start coordinate must be less than end coordinate.\n' % (sys.argv[0], params.lineCounter) )
            return os.EX_DATAERR

        convertedLine = '\t'.join([cols['tName'],
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
                                   cols['tStarts']]) + '\n'

    return convertedLine

class Parameters:
    def __init__(self):
        self._lineCounter = 0
        self._inputIsHeadered = False
        self._keepHeader = False
        self._keepHeaderIdx = 0
        self._keepHeaderChr = "_header"
        self._sortOutput = True
        self._sortTmpdir = None
        self._sortTmpdirSet = False
        self._maxMem = "2G"
        self._maxMemChanged = False
        self._starchFormat = "--bzip2"

    @property
    def lineCounter(self):
        return self._lineCounter
    @lineCounter.setter
    def lineCounter(self, val):
        self._lineCounter = val

    @property
    def inputIsHeadered(self):
        return self._inputIsHeadered
    @inputIsHeadered.setter
    def inputIsHeadered(self, flag):
        self._inputIsHeadered = flag

    @property
    def keepHeader(self):
        return self._keepHeader
    @keepHeader.setter
    def keepHeader(self, flag):
        self._keepHeader = flag

    @property
    def keepHeaderIdx(self):
        return self._keepHeaderIdx
    @keepHeaderIdx.setter
    def keepHeaderIdx(self, val):
        self._keepHeaderIdx = val

    @property
    def keepHeaderChr(self):
        return self._keepHeaderChr
    @keepHeaderChr.setter
    def keepHeaderChr(self, val):
        self._keepHeaderChr = val

    @property
    def sortOutput(self):
        return self._sortOutput
    @sortOutput.setter
    def sortOutput(self, flag):
        self._sortOutput = flag

    @property
    def sortTmpdir(self):
        return self._sortTmpdir
    @sortTmpdir.setter
    def sortTmpdir(self, val):
        self._sortTmpdir = val

    @property
    def sortTmpdirSet(self):
        return self._sortTmpdirSet
    @sortTmpdirSet.setter
    def sortTmpdirSet(self, flag):
        self._sortTmpdirSet = flag

    @property
    def maxMem(self):
        return self._maxMem
    @maxMem.setter
    def maxMem(self, val):
        self._maxMem = val

    @property
    def maxMemChanged(self):
        return self._maxMemChanged
    @maxMemChanged.setter
    def maxMemChanged(self, flag):
        self._maxMemChanged = flag

    @property
    def starchFormat(self):
        return self._starchFormat
    @starchFormat.setter
    def starchFormat(self, val):
        self._starchFormat = val

def main(*args):
    
    requiredVersion = (2,6,2)
    checkInstallation(requiredVersion)

    params = Parameters()

    #
    # Fixes bug with Python handling of SIGPIPE signal from UNIX head, etc.
    # http://coding.derkeiler.com/Archive/Python/comp.lang.python/2004-06/3823.html
    #

    signal.signal(signal.SIGPIPE,signal.SIG_DFL)

    #
    # Read in options
    #

    optstr = ""
    longopts = ["do-not-sort", "keep-header", "headered", "help", "max-mem=", "starch-format=", "sort-tmpdir="]
    try:
        (options, args) = getopt.getopt(sys.argv[1:], optstr, longopts)
    except getopt.GetoptError as error:
        sys.stderr.write( "[%s] - Error: %s\n" % (sys.argv[0], str(error)) )
        printUsage("stderr")
        return os.EX_USAGE
    for key, value in options:
        if key in ("--help"):
            printUsage("stdout")
            return os.EX_OK
        elif key in ("--keep-header"):
            params.keepHeader = True
        elif key in ("--headered"):
            params.inputIsHeadered = True
        elif key in ("--do-not-sort"):
            params.sortOutput = False
        elif key in ("--sort-tmpdir"):
            params.sortTmpdir = str(value)
            params.sortTmpdirSet = True
        elif key in ("--max-mem"):
            params.maxMem = str(value)
            params.maxMemChanged = True
        elif key in ("--starch-format"):
            params.starchFormat = "--" + str(value)

    if params.keepHeader and not params.inputIsHeadered:
        sys.stderr.write( "[%s] - Error: Cannot specify --keep-header without --headered\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_USAGE

    if params.maxMemChanged and not params.sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --max-mem parameters\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_USAGE 

    if params.sortTmpdirSet and not params.maxMemChanged:
        sys.stderr.write( "[%s] - Error: Cannot specify --sort-tmpdir parameter without specifying --max-mem parameter\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_USAGE
    
    if params.sortTmpdirSet:
        try:
            os.listdir(params.sortTmpdir)
        except OSError as error:
            sys.stderr.write( "[%s] - Error: Temporary sort data directory specified with --sort-tmpdir is a file, is non-existent, or its permissions do not allow access\n" % sys.argv[0] )
            printUsage("stderr")
            return os.EX_USAGE

    mode = os.fstat(0).st_mode
    inputIsNotAvailable = True
    if stat.S_ISFIFO(mode) or stat.S_ISREG(mode):
        inputIsNotAvailable = False
    if inputIsNotAvailable:
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in PSL-formatted data\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_NOINPUT

    try:
        if which('sort-bed') is None:
            raise IOError("The sort-bed binary could not be found in your user PATH -- please locate and install this binary")
    except IOError, msg:
        sys.stderr.write( "[%s] - %s\n" % (sys.argv[0], msg) )
        return os.EX_OSFILE

    starch_process = subprocess.Popen(['starch', params.starchFormat, '-'], stdin=subprocess.PIPE)

    if params.sortOutput:
        if params.sortTmpdirSet:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '--tmpdir', params.sortTmpdir, '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        else:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        convert_psl_to_bed_thread = threading.Thread(target=consumePSL, args=(sys.stdin, sortbed_process.stdin, params))
        pass_bed_to_starch_thread = threading.Thread(target=consumeBED, args=(sortbed_process.stdout, starch_process.stdin, params))
    else:
        convert_psl_to_bed_thread = threading.Thread(target=consumePSL, args=(sys.stdin, starch_process.stdin, params))

    convert_psl_to_bed_thread.start()
    convert_psl_to_bed_thread.join()

    if params.sortOutput:
        pass_bed_to_starch_thread.start()
        pass_bed_to_starch_thread.join()
        sortbed_process.wait()

    starch_process.wait()
    
    #
    # Test for error exit from sort-bed process
    #

    if params.sortOutput and int(sortbed_process.returncode) != 0:
        return os.EX_IOERR    

    if int(starch_process.returncode) != 0:
        return os.EX_IOERR

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
