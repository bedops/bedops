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
# Project:      Converts 1-based, closed [a, b] VCF v4 input into 0-based, 
#               half-open [a-1, b) extended BED
#
# Version:      2.4.2
#
# Notes:        This conversion script relies on the VCF v4 format, with its
#               specifications outlined here by the 1000 Genomes project:
#
#               http://www.1000genomes.org/wiki/Analysis/Variant%20Call%20Format/vcf-variant-call-format-version-41
#
#               -- The "meta-information" (starting with '##') and "header"
#                  lines (starting with '#') are discarded.
#
#                  To preserve metadata and header as BED elements, use the 
#                  --keep-header option, which munges these data into pseudo-elements 
#                  that should sort to the top (when chromosomes follow UCSC naming
#                  conventions) by using the "_header" chromosome name.
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
#               $ vcf2bed < foo.vcf > sorted-foo.vcf.bed
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ vcf2bed --do-not-sort < foo.vcf > unsorted-foo.vcf.bed
#

import getopt, sys, os, stat, subprocess, signal, threading, math

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
             "  %s [ --help ] [ --keep-header ] [ --snvs | --insertions | --deletions ] [ --do-not-sort | --max-mem <value> (--sort-tmpdir <dir>) ] < foo.vcf\n\n"
             "Options:\n"
             "  --help                 Print this help message and exit\n"
             "  --keep-header          Preserve metadata and header information as pseudo-BED elements\n"
             "  --snvs                 Filter on single nucleotide variants\n"
             "  --insertions           Filter on insertion variants\n"
             "  --deletions            Filter on deletion variants\n"
             "  --do-not-sort          Do not sort converted data with BEDOPS sort-bed\n"
             "  --max-mem <value>      Sets aside <value> memory for sorting BED output. For example,\n"
             "                         <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory\n"
             "                         (default: 2G).\n"
             "  --sort-tmpdir <dir>    Optionally sets <dir> as temporary directory for sort data, when\n"
             "                         used in conjunction with --max-mem <value>. For example, <dir> can\n"
             "                         be $PWD to store intermediate sort data in the current working\n"
             "                         directory, in place of the host operating system default\n"
             "                         temporary directory.\n\n"             
             "About:\n"
             "  This script converts 1-based, closed [a, b] VCF v4 data from standard input\n"
             "  into 0-based, half-open [a-1, b) extended BED, sent to standard output.\n\n"
             "  This conversion script relies on the VCF v4 format, with its\n"
             "  specifications outlined here by the 1000 Genomes project:\n\n"
             "  http://www.1000genomes.org/wiki/Analysis/Variant%%20Call%%20Format/vcf-variant-call-format-version-41\n\n"
             "  -- The 'meta-information' (starting with '##') and 'header'\n"
             "     lines (starting with '#') are discarded, unless --keep-header is specified.\n\n"
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
             "  $ %s < foo.vcf > sorted-foo.vcf.bed\n\n"
             "  We make no assumptions about sort order from converted output. Apply\n"
             "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n"
             "  which generates lexicographically-sorted BED data as output.\n\n"
             "  If you want to skip sorting, use the --do-not-sort option:\n\n"
             "  $ %s --do-not-sort < foo.vcf > unsorted-foo.vcf.bed\n\n"
             % (sys.argv[0], sys.argv[0], sys.argv[0]) )
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

def isId(altAllele):
    idCharacters = set('<>')
    return any((idCharacter in idCharacters) for idCharacter in altAllele)

def isSnv(refAllele, altAllele):
    if isId(altAllele):
        return False
    delta = len(refAllele) - len(altAllele)
    return delta == 0

def isInsertion(refAllele, altAllele):
    if isId(altAllele):
        return False
    delta = len(refAllele) - len(altAllele)
    return delta < 0

def isDeletion(refAllele, altAllele):
    if isId(altAllele):
        return False
    delta = len(refAllele) - len(altAllele)
    return delta > 0

def isMixedRecord(altAllele):
    return ',' in altAllele

def consumeVCF(from_stream, to_stream, params):
    while True:
        vcf_line = from_stream.readline()
        if not vcf_line:
            from_stream.close()
            to_stream.close()
            break
        bed_line = convertVCFToBed(vcf_line, params, to_stream)
        if bed_line:
            to_stream.write(bed_line)
            to_stream.flush()

def convertVCFToBed(line, params, stream):
    convertedLine = None

    chomped_line = line.rstrip(os.linesep)
    if chomped_line.startswith('##') and not params.keepHeader:
        pass
    elif chomped_line.startswith('##') and params.keepHeader:
        elem_chr = params.keepHeaderChr
        elem_start = str(params.keepHeaderIdx)
        elem_stop = str(params.keepHeaderIdx + 1)
        elem_id = chomped_line
        convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id]) + '\n'
        params.keepHeaderIdx += 1
    elif chomped_line.startswith('#'):
        params.columns = chomped_line.split('\t')
        if params.keepHeader:
            elem_chr = params.keepHeaderChr
            elem_start = str(params.keepHeaderIdx)
            elem_stop = str(params.keepHeaderIdx + 1)
            elem_id = chomped_line
            convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id]) + '\n'
            params.keepHeaderIdx += 1
    else:
        elems = chomped_line.split('\t')
        metadata = dict()
        for columnIdx in range(len(params.columns)):
            try:
                metadata[params.columns[columnIdx]] = elems[columnIdx];
            except IndexError as ie:
                sys.stderr.write( "[%s] Error: Could not map data values to VCF header keys (perhaps missing or bad delimiters in header line?)" % (sys.argv[0]))
                stream.close()
                sys.exit(os.EX_DATAERR)
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
        except KeyError as ke:
            sys.stderr.write( '[%s] Error: Could not map data value from VCF header key (perhaps missing or bad delimiters in header line or data row?)' % (sys.argv[0]))
            stream.close()
            sys.exit(os.EX_DATAERR)

        try:
            elem_genotype = '\t'.join(elems[8:])
        except IndexError:
            pass

        if isMixedRecord(elem_alt):
            # write each variant in mixed record to separate BED element
            alt_alleles = elem_alt.split(",")
            convertedLine = ""
            for alt_allele in alt_alleles:
                elem_alt = alt_allele
                if params.filterCount != 0 and not params.filterOnInsertions:
                    elem_stop = str(int(elem_start) + int(math.fabs(len(elem_ref) - len(elem_alt))) + 1)

                if not elem_genotype:
                    alleleLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info]) + '\n'
                else:
                    alleleLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info, elem_genotype]) + '\n'
                    
                if params.filterCount == 0:
                    convertedLine += alleleLine
                elif params.filterOnSnvs and isSnv(elem_ref, elem_alt):
                    convertedLine += alleleLine
                elif params.filterOnInsertions and isInsertion(elem_ref, elem_alt):
                    convertedLine += alleleLine
                elif params.filterOnDeletions and isDeletion(elem_ref, elem_alt):
                    convertedLine += alleleLine
            if not convertedLine:
                convertedLine = None
        else:
            if params.filterCount != 0 and not params.filterOnInsertions:
                elem_stop = str(int(elem_start) + int(math.fabs(len(elem_ref) - len(elem_alt))) + 1)
                    
            if not elem_genotype:
                convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info]) + '\n'
            else:
                convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info, elem_genotype]) + '\n'
                    
            if params.filterCount == 0:
                pass
            elif params.filterOnSnvs and isSnv(elem_ref, elem_alt):
                pass
            elif params.filterOnInsertions and isInsertion(elem_ref, elem_alt):
                pass
            elif params.filterOnDeletions and isDeletion(elem_ref, elem_alt):
                pass
            else:
                convertedLine = None

    return convertedLine

class Parameters:
    def __init__(self):
        self._columns = None
        self._keepHeader = False
        self._keepHeaderIdx = 0
        self._keepHeaderChr = "_header"
        self._sortOutput = True
        self._sortTmpdir = None
        self._sortTmpdirSet = False
        self._maxMem = "2G"
        self._maxMemChanged = False
        self._filterOnSnvs = False
        self._filterOnInsertions = False
        self._filterOnDeletions = False
        self._filterCount = 0

    @property
    def columns(self):
        return self._columns
    @columns.setter
    def columns(self, val):
        self._columns = val

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
    def filterOnSnvs(self):
        return self._filterOnSnvs
    @filterOnSnvs.setter
    def filterOnSnvs(self, flag):
        self._filterOnSnvs = flag

    @property
    def filterOnInsertions(self):
        return self._filterOnInsertions
    @filterOnInsertions.setter
    def filterOnInsertions(self, flag):
        self._filterOnInsertions = flag

    @property
    def filterOnDeletions(self):
        return self._filterOnDeletions
    @filterOnDeletions.setter
    def filterOnDeletions(self, flag):
        self._filterOnDeletions = flag

    @property
    def filterCount(self):
        return self._filterCount
    @filterCount.setter
    def filterCount(self, val):
        self._filterCount = val

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
    longopts = ["help", "keep-header", "do-not-sort", "max-mem=", "snvs", "insertions", "deletions", "sort-tmpdir="]
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
        elif key in ("--snvs"):
            params.filterOnSnvs = True
        elif key in ("--insertions"):
            params.filterOnInsertions = True
        elif key in ("--deletions"):
            params.filterOnDeletions= True

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

    params.filterCount = 0
    if params.filterOnSnvs:
        params.filterCount += 1
    if params.filterOnInsertions:
        params.filterCount += 1
    if params.filterOnDeletions:
        params.filterCount += 1

    if params.filterCount > 1:
        sys.stderr.write( "[%s] - Error: Cannot specify more than one filter parameter\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_USAGE        

    mode = os.fstat(0).st_mode
    inputIsNotAvailable = True
    if stat.S_ISFIFO(mode) or stat.S_ISREG(mode):
        inputIsNotAvailable = False
    if inputIsNotAvailable:
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in VCF-formatted data\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_NOINPUT

    try:
        if which('sort-bed') is None:
            raise IOError("The sort-bed binary could not be found in your user PATH -- please locate and install this binary")
    except IOError, msg:
        sys.stderr.write( "[%s] - %s\n" % (sys.argv[0], msg) )
        return os.EX_OSFILE

    if params.sortOutput:
        if params.sortTmpdirSet:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '--tmpdir', params.sortTmpdir, '-'], stdin=subprocess.PIPE, stdout=sys.stdout)
        else:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '-'], stdin=subprocess.PIPE, stdout=sys.stdout)
        convert_vcf_to_bed_thread = threading.Thread(target=consumeVCF, args=(sys.stdin, sortbed_process.stdin, params))
    else:
        convert_vcf_to_bed_thread = threading.Thread(target=consumeVCF, args=(sys.stdin, sys.stdout, params))

    convert_vcf_to_bed_thread.start()
    convert_vcf_to_bed_thread.join()

    if params.sortOutput:
        sortbed_process.wait()
    
    #
    # Test for error exit from sort-bed process
    #

    if params.sortOutput and int(sortbed_process.returncode) != 0:
        return os.EX_IOERR

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
