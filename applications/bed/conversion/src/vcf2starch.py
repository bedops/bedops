#!/usr/bin/env python

#
#    BEDOPS
#    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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
#               half-open [a-1, b) extended BED and thence compressed into a
#               BEDOPS Starch archive sent to standard output.
#
# Version:      2.4
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

import getopt, sys, os, stat, subprocess, signal, tempfile, math

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
             "  %s [ --help ] [ --snvs | --insertions | --deletions ] [ --do-not-sort | --max-mem <value> ] [ --starch-format <bzip2|gzip> ] < foo.vcf > sorted-foo.vcf.bed.starch\n\n"
             "Options:\n"
             "  --help                        Print this help message and exit\n"
             "  --snvs                        Filter on single nucleotide variants\n"
             "  --insertions                  Filter on insertion variants\n"
             "  --deletions                   Filter on deletion variants\n"
             "  --do-not-sort                 Do not sort converted data with BEDOPS sort-bed\n"
             "  --max-mem <value>             Sets aside <value> memory for sorting BED output. For example,\n"
             "                                <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory\n"
             "                                (default: 2G).\n"
             "  --starch-format <bzip2|gzip>  Specify backend compression format of starch\n"
             "                                archive (default: bzip2).\n\n"                          
             "About:\n"
             "  This script converts 1-based, closed [a, b] VCF v4 data from standard input\n"
             "  into 0-based, half-open [a-1, b) extended BED, sent to standard output.\n\n"
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
             "  $ %s --do-not-sort < foo.vcf > unsorted-foo.vcf.bed\n\n"
             % (sys.argv[0], sys.argv[0], sys.argv[0]) )
    if stream is "stdout":
        sys.stdout.write(usage)
    elif stream is "stderr":
        sys.stderr.write(usage)
    return os.EX_OK

def checkInstallation(rv):
    currentVersion = sys.version_info
    if currentVersion[0] == rv[0] and currentVersion[1] >= rv[1]:
        pass
    else:
        sys.stderr.write( "[%s] - Error: Your Python interpreter must be %d.%d or greater (within major version %d)\n" % (sys.argv[0], rv[0], rv[1], rv[0]) )
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

def main(*args):
    requiredVersion = (2,7)
    checkInstallation(requiredVersion)

    sortOutput = True
    maxMem = "2G"
    maxMemChanged = False
    filterOnSnvs = False
    filterOnInsertions = False
    filterOnDeletions = False
    starchFormat = "bzip2"

    # fixes bug with Python handling of SIGPIPE signal from UNIX head, etc.
    # http://coding.derkeiler.com/Archive/Python/comp.lang.python/2004-06/3823.html
    signal.signal(signal.SIGPIPE,signal.SIG_DFL)

    optstr = ""
    longopts = ["help", "do-not-sort", "max-mem=", "snvs", "insertions", "deletions", "starch-format="]
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
        elif key in ("--do-not-sort"):
            sortOutput = False
        elif key in ("--max-mem"):
            maxMem = str(value)
            maxMemChanged = True
        elif key in ("--snvs"):
            filterOnSnvs = True
        elif key in ("--insertions"):
            filterOnInsertions = True
        elif key in ("--deletions"):
            filterOnDeletions= True
        elif key in ("--starch-format"):
            starchFormat = str(value)

    if maxMemChanged and not sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --max-mem parameters\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_USAGE

    filterCount = 0
    if filterOnSnvs:
        filterCount += 1
    if filterOnInsertions:
        filterCount += 1
    if filterOnDeletions:
        filterCount += 1

    if filterCount > 1:
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

    starchTF = tempfile.NamedTemporaryFile(mode='wb')
    if sortOutput:
        sortTF = tempfile.NamedTemporaryFile(mode='wb', delete=False)
            
    for line in sys.stdin:
        chomped_line = line.rstrip(os.linesep)
        if chomped_line.startswith('##'):
            pass
        elif chomped_line.startswith('#'):
            columns = chomped_line.split('\t')
        else:
            elems = chomped_line.split('\t')
            metadata = dict()
            for columnIdx in range(len(columns)):
                try:
                    metadata[columns[columnIdx]] = elems[columnIdx];
                except IndexError:
                    print 'ERROR: Could not map data values to VCF header keys (perhaps missing or bad delimiters in header line?)'
                    return os.EX_DATAERR
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
                return os.EX_DATAERR

            try:
                elem_genotype = '\t'.join(elems[8:])
            except IndexError:
                pass

            if isMixedRecord(elem_alt):
                # write each variant in mixed record to separate BED element
                alt_alleles = elem_alt.split(",")
                for alt_allele in alt_alleles:
                    elem_alt = alt_allele
                    if filterCount != 0:
                        elem_stop = str(int(elem_start) + int(math.fabs(len(elem_ref) - len(elem_alt))) + 1)

                    if not elem_genotype:
                        convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info]) + '\n'
                    else:
                        convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info, elem_genotype]) + '\n'
                    
                    if sortOutput:
                        if filterCount == 0:
                            sortTF.write(convertedLine)
                        elif filterOnSnvs and isSnv(elem_ref, elem_alt):
                            sortTF.write(convertedLine)
                        elif filterOnInsertions and isInsertion(elem_ref, elem_alt):
                            sortTF.write(convertedLine)
                        elif filterOnDeletions and isDeletion(elem_ref, elem_alt):
                            sortTF.write(convertedLine)
                    else:
                        if filterCount == 0:
                            starchTF.write(convertedLine)                
                        elif filterOnSnvs and isSnv(elem_ref, elem_alt):
                            starchTF.write(convertedLine)
                        elif filterOnInsertions and isInsertion(elem_ref, elem_alt):
                            starchTF.write(convertedLine)
                        elif filterOnDeletions and isDeletion(elem_ref, elem_alt):
                            starchTF.write(convertedLine)
            else:
                if filterCount != 0:
                    elem_stop = str(int(elem_start) + int(math.fabs(len(elem_ref) - len(elem_alt))) + 1)
                    
                if not elem_genotype:
                    convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info]) + '\n'
                else:
                    convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id, elem_score, elem_ref, elem_alt, elem_filter, elem_info, elem_genotype]) + '\n'
                    
                if sortOutput:
                    if filterCount == 0:
                        sortTF.write(convertedLine)
                    elif filterOnSnvs and isSnv(elem_ref, elem_alt):
                        sortTF.write(convertedLine)
                    elif filterOnInsertions and isInsertion(elem_ref, elem_alt):
                        sortTF.write(convertedLine)
                    elif filterOnDeletions and isDeletion(elem_ref, elem_alt):
                        sortTF.write(convertedLine)
                else:
                    if filterCount == 0:
                        starchTF.write(convertedLine)                
                    elif filterOnSnvs and isSnv(elem_ref, elem_alt):
                        starchTF.write(convertedLine)
                    elif filterOnInsertions and isInsertion(elem_ref, elem_alt):
                        starchTF.write(convertedLine)
                    elif filterOnDeletions and isDeletion(elem_ref, elem_alt):
                        starchTF.write(convertedLine)
                
    if sortOutput:

        try:
            if which('sort-bed') is None:
                raise IOError("The sort-bed binary could not be found in your user PATH -- please locate and install this binary")
        except IOError, msg:
            sys.stderr.write( "[%s] - %s\n" % (sys.argv[0], msg) )
            return os.EX_OSFILE

        sortTF.close()
        sortProcess = subprocess.Popen(['sort-bed', '--max-mem', maxMem, sortTF.name], stdout=starchTF)
        sortProcess.wait()
        try:
            os.remove(sortTF.name)
        except OSError:
            sys.stderr.write( "[%s] - Warning: Could not delete intermediate sorted file [%s]\n" % (sys.argv[0], sortTF.name) )

    try:
        if which('starch') is None:
            raise IOError("The starch binary could not be found in your user PATH -- please locate and install this binary")
    except IOError, msg:
        sys.stderr.write( "[%s] - %s\n" % (sys.argv[0], msg) )
        return os.EX_OSFILE

    starchProcess = subprocess.Popen(["starch", starchFormat, starchTF.name])
    starchProcess.wait()

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
