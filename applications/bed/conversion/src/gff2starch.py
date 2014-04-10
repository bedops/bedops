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
# Project:      Converts 1-based, closed [a, b] GFF3 input
#               into 0-based, half-open [a-1, b) six-column extended BED
#               and thence compressed into a BEDOPS Starch archive sent
#               to standard output.
#
# Version:      2.4.2
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
#               Metadata and header fields are usually stripped. Use the --keep-header
#               option to preserve these data as pseudo-BED elements that use the "_header"
#               chromosome name.
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
             "  %s [ --help ] [ --keep-header ] [ --do-not-sort | --max-mem <value> (--sort-tmpdir <dir>) ] [ --starch-format <bzip2|gzip> ] < foo.gff > sorted-foo.gff.bed.starch\n\n"
             "Options:\n"
             "  --help                        Print this help message and exit\n"
             "  --keep-header                 Preserve metadata and header fields as pseudo-BED elements\n"
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
             "  This script converts 1-based, closed [a, b] GFF3 data from standard\n"
             "  input into 0-based, half-open [a-1, b) six-column extended BED, sorted and\n"
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
             "  The metadata and header fields are usually stripped. If you want to keep them,\n"
             "  use the --keep-header option.\n\n"
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

def consumeGFF(from_stream, to_stream, params):
    while True:
        gff_line = from_stream.readline()
        if not gff_line or gff_line.startswith("##FASTA"):
            from_stream.close()
            to_stream.close()
            break
        bed_line = convertGFFToBed(gff_line, params, to_stream)
        to_stream.write(bed_line)
        to_stream.flush()

def consumeBED(from_stream, to_stream, params):
    while True:
        try:
            bed_line = from_stream.readline()
            if not bed_line:
                from_stream.close()
                to_stream.close()
                break
            to_stream.write(bed_line)
            to_stream.flush()
        except AttributeError as e:
            to_stream.close()
            break

def convertGFFToBed(line, params, stream):

    convertedLine = ""

    chomped_line = line.rstrip(os.linesep)
    if chomped_line.startswith('##') or chomped_line.startswith('#'):
        if params.keepHeader:
            elem_chr = params.keepHeaderChr
            elem_start = str(params.keepHeaderIdx)
            elem_stop = str(params.keepHeaderIdx + 1)
            elem_id = chomped_line
            convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id]) + '\n'
            params.keepHeaderIdx += 1
        else:
            pass
    else:
        elems = chomped_line.split('\t')
        cols = dict()
        try:
            cols['seqid'] = elems[0].lstrip(' ') # strip leading whitespace
            cols['source'] = elems[1]
            cols['type'] = elems[2]
            cols['start'] = int(elems[3])
            cols['end'] = int(elems[4])
            cols['score'] = elems[5]
            cols['strand'] = elems[6]
            cols['phase'] = elems[7]
            cols['attributes'] = elems[8].rstrip(' ') # strip trailing whitespace
        except IndexError as ie:
            sys.stderr.write( "[%s] - Error: Could not import GFF data (ensure input is GFF-formatted)\n" % (sys.argv[0]))
            stream.close()
            sys.exit(os.EX_DATAERR)            
        
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

        convertedLine = '\t'.join([cols['chr'], 
                                   str(cols['start']),
                                   str(cols['end']),
                                   cols['id'],
                                   cols['score'],
                                   cols['strand'],
                                   cols['source'],
                                   cols['type'],
                                   cols['phase'],
                                   cols['attributes']]) + '\n'

    return convertedLine

class Parameters:
    def __init__(self):
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
    longopts = ["help", "keep-header", "do-not-sort", "max-mem=", "starch-format=", "sort-tmpdir="]
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
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in GFF-formatted data\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_NOINPUT

    try:
        if which('sort-bed') is None:
            raise IOError("The sort-bed binary could not be found in your user PATH -- please locate and install this binary")
        if which('starch') is None:
            raise IOError("The starch binary could not be found in your user PATH -- please locate and install this binary")
    except IOError, msg:
        sys.stderr.write( "[%s] - %s\n" % (sys.argv[0], msg) )
        return os.EX_OSFILE

    starch_process = subprocess.Popen(['starch', params.starchFormat, '-'], stdin=subprocess.PIPE)

    if params.sortOutput:
        if params.sortTmpdirSet:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '--tmpdir', params.sortTmpdir, '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        else:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        convert_gff_to_bed_thread = threading.Thread(target=consumeGFF, args=(sys.stdin, sortbed_process.stdin, params))
        pass_bed_to_starch_thread = threading.Thread(target=consumeBED, args=(sortbed_process.stdout, starch_process.stdin, params))
    else:
        convert_gff_to_bed_thread = threading.Thread(target=consumeGFF, args=(sys.stdin, starch_process.stdin, params))

    convert_gff_to_bed_thread.start()
    convert_gff_to_bed_thread.join()

    if params.sortOutput:
        pass_bed_to_starch_thread.start()
        pass_bed_to_starch_thread.join()
        sortbed_process.wait()

    starch_process.wait()
    
    #
    # Test for error exit from sort-bed process and starch
    #

    if params.sortOutput and int(sortbed_process.returncode) != 0:
        return os.EX_IOERR

    if int(starch_process.returncode) != 0:
        return os.EX_IOERR

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
