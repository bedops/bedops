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
# Project:      Converts 1-based, closed [a, b] headered or headerless SAM input
#               into 0-based, half-open [a-1, b) extended BED that is subsequently 
#               compressed into a Starch v2 archive.
#
# Version:      2.4.2
#
# Notes:        The SAM format is Sequence Alignment/Map file that is a 1-based, closed 
#               [a, b]. This script converts this indexing back to 0-based, half-
#               open when creating BED output.
#
#               We process SAM columns from mappable reads (as described by 
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
#               By default, we only process mapped reads. If you also want to convert unmapped 
#               reads, add the --all-reads option.
#
#               In the case of RNA-seq data with skipped regions ('N' components in the
#               read's CIGAR string), the --split option will split the read into two or more 
#               separate BED elements. 
#
#               The header section is normally stripped from the output. You can use the
#               --keep-header option to preserve the header data from the SAM input as
#               pseudo-BED elements.
#
#               This script also validates the CIGAR strings in a sequencing dataset, in the
#               course of converting to BED.
#
#               Example usage:
#
#               $ sam2starch < foo.sam > sorted-foo.sam.bed.starch
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ sam2starch --do-not-sort < foo.sam > unsorted-foo.sam.bed.starch
#

import getopt, sys, os, stat, subprocess, signal, re, threading

#
# Optional field tag names
# cf. pgs. 6-7, http://samtools.sourceforge.net/SAMv1.pdf
#

allSpecificationSamTags = ['AM', 
                           'AS', 
                           'BC', 
                           'BQ', 
                           'CC', 
                           'CM', 
                           'CO', 
                           'CP', 
                           'CQ', 
                           'CS', 
                           'CT', 
                           'E2', 
                           'FI', 
                           'FS', 
                           'FZ', 
                           'LB', 
                           'H0', 
                           'H1', 
                           'H2', 
                           'HI', 
                           'IH', 
                           'MD', 
                           'MQ', 
                           'NH', 
                           'NM', 
                           'OQ', 
                           'OP', 
                           'OC', 
                           'PG', 
                           'PQ', 
                           'PT', 
                           'PU', 
                           'QT', 
                           'Q2', 
                           'R2', 
                           'RG', 
                           'RT', 
                           'SA', 
                           'SM', 
                           'TC', 
                           'U2', 
                           'UQ']


class SamTags:
    def __init__(self):
        self.tags = []
        self.containsMismatches = False
        self.containsMultipleReads = False
        self.customTagsAdded = False
        self.customTagsStr = ""

    def __str__(self):
        res = ""
        for tag in self.tags:
            res += str(tag) + " "
        return res.rstrip()

    def append(self, tagStr):
        tagElems = tagStr.split(":")
        tagName = tagElems[0]
        tagType = tagElems[1]
        tagValue = tagElems[2]
        customTags = []
        if self.customTagsAdded:
            customTags = self.customTagsStr.split(",")
        if str(tagName) in (allSpecificationSamTags + customTags) or tagName.startswith('X') or tagName.startswith('Y') or tagName.startswith('Z'):
            tag = SamTag()
            tag.tag = (tagName, tagValue)
            self.tags.append(tag)
            self.switchFlags(tag)
        else:
            raise Exception("Appended tag (%s) is not valid per SAMtools specification\nConsider using --custom-tags <value> option (see --help)" % tagName)

    def switchFlags(self, tag):
        if tag.key == 'NH':
            self.containsMultipleReads = True
        if tag.key == 'NM':
            self.containsMismatches = True


class SamTag(object):
    def __str__(self):
        return "%s" % self._tag

    @property
    def tag(self):
        return self._tag

    @property
    def key(self):
        for k in self._tag:
            return str(k)

    @property
    def value(self):
        return self._tag[self.key]

    @tag.setter
    def tag(self, tagTuple):
        tagKey = tagTuple[0]
        tagValue = tagTuple[1]
        try:
            self._tag = { str(tagKey) : str(tagValue) }
        except ValueError:
            raise Exception("ValueError: invalid key (%s) or value (%s)" % (str(tagKey), str(tagValue)))


#
# CIGAR Operation categories 
# cf. pg. 5, http://samtools.sourceforge.net/SAMv1.pdf
#

allValidSamCigarOps = 'MIDNSHP=X*'
allLengthSamCigarOps = 'MDN=X'
allSeqSamCigarOps = 'MIS=X'
allSplitSamCigarOps = 'N'


class SamCigarOps:
    def __init__(self):
        self.operations = []
        self.containsSplitOp = False

    def __str__(self):
        res = ""
        for op in self.operations:
            res += str(op) + " "
        return res.rstrip()

    def append(self, op):
        if not isinstance(op, SamCigarOp):
            raise Exception("Appended operation is not of type SamCigarOp")
        self.operations.append(op)

    def readLength(self):
        if not self.operations:
            raise Exception("CIGAR string not set: cannot calculate read length -- first set CIGAR string")
        readLength = 0
        for op in self.operations:
            if allLengthSamCigarOps.find(op.key) != -1:
                readLength += op.value
        return readLength

    def testSeq(self, seqValue):
        if not self.operations:
            raise Exception("CIGAR string not set: cannot validate SEQ -- first set CIGAR string")
        seqCalc = 0
        for op in self.operations:
            if allSeqSamCigarOps.find(op.key) != -1:
                seqCalc += op.value
        if seqCalc != seqValue:
            raise Exception("CIGAR string's sum of lengths of M/I/S/=/X operations not equal to SEQ")
        return True

    def cigarStr(self, cigarStr):
        del self.operations[:] # blank out any pre-existing operations
        cigarOps = re.findall(r'(\d+)(\w)', cigarStr)
        for opTuple in cigarOps:
            opKey = opTuple[1]
            opValue = opTuple[0]
            op = SamCigarOp()
            try:
                op.operation = (opKey, opValue)
            except Exception as e:
                raise Exception("CIGAR string %s contains an error -> %s" % (cigarStr, e))
            if allSplitSamCigarOps.find(opKey) != -1:
                op.containsSplitOp = True
            self.append(op)
        

class SamCigarOp(object):
    def __str__(self):
        return "%s" % self._operation

    @property
    def operation(self):
        return self._operation

    @property
    def key(self):
        for k in self._operation:
            return str(k)

    @property
    def value(self):
        return self._operation[self.key]

    @operation.setter
    def operation(self, opTuple):
        opKey = opTuple[0]
        opValue = opTuple[1]
        if allValidSamCigarOps.find(opKey) == -1:
            raise Exception("Illegal CIGAR operation: %s" % ("".join((opValue, opKey))))
        self._operation = { str(opKey) : int(opValue) }


class SamRecord(object):

    def __init__(self):
        self._cigarOps = SamCigarOps()
        self._tags = ""
        self._strand = "+"

    @property
    def cigarOps(self):
        return self._cigarOps

    def updateCigarOps(self, cigar):
        self._cigarOps.cigarStr(cigar)

    @property
    def rname(self):
        return self._rname

    @rname.setter
    def rname(self, rname):
        self._rname = rname

    @property
    def start(self):
        return self._start

    @start.setter
    def start(self, start):
        self._start = start

    @property
    def stop(self):
        return self._stop

    @stop.setter
    def stop(self, stop):
        self._stop = stop

    @property
    def qname(self):
        return self._qname

    @qname.setter
    def qname(self, qname):
        self._qname = qname

    @property
    def originalQname(self):
        return self._originalQname

    @originalQname.setter
    def originalQname(self, originalQname):
        self._originalQname = originalQname

    @property
    def flag(self):
        return self._flag

    @flag.setter
    def flag(self, flag):
        self._flag = flag
        self._strand = '-' if 16 & self._flag else '+'

    @property
    def strand(self):
        return self._strand

    @strand.setter
    def strand(self, strand):
        self._strand = strand

    @property
    def mapq(self):
        return self._mapq

    @mapq.setter
    def mapq(self, mapq):
        self._mapq = mapq

    @property
    def cigar(self):
        return self._cigar

    @cigar.setter
    def cigar(self, cigar):
        self._cigar = cigar
        self.updateCigarOps(self._cigar)

    @property
    def rnext(self):
        return self._rnext

    @rnext.setter
    def rnext(self, rnext):
        self._rnext = rnext

    @property
    def pnext(self):
        return self._pnext

    @pnext.setter
    def pnext(self, pnext):
        self._pnext = pnext

    @property
    def tlen(self):
        return self._tlen

    @tlen.setter
    def tlen(self, tlen):
        self._tlen = tlen

    @property
    def seq(self):
        return self._seq

    @seq.setter
    def seq(self, seq):
        self._seq = seq
        if self._seq != '*':
            self._cigarOps.testSeq(len(self._seq))

    @property
    def qual(self):
        return self._qual

    @qual.setter
    def qual(self, qual):
        self._qual = qual

    @property
    def tags(self):
        return self._tags

    @tags.setter
    def tags(self, tags):
        self._tags = tags

    def isMappedRead(self):
        return not 4 & self._flag

    def asBed(self):
        if self._tags:
            return '\t'.join([self._rname, 
                              str(self._start),
                              str(self._stop),
                              self._qname,
                              str(self._flag),
                              self._strand,
                              self._mapq,
                              self._cigar,
                              self._rnext,
                              str(self._pnext),
                              str(self._tlen),
                              self._seq,
                              self._qual,
                              self._tags]) + '\n'
        else:
            return '\t'.join([self._rname, 
                              str(self._start),
                              str(self._stop),
                              self._qname,
                              str(self._flag),
                              self._strand,
                              self._mapq,
                              self._cigar,
                              self._rnext,
                              str(self._pnext),
                              str(self._tlen),
                              self._seq,
                              self._qual]) + '\n'


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
             "  %s [ --help ] [ --keep-header ] [ --split ] [ --all-reads ] [ --do-not-sort | --max-mem <value> (--sort-tmpdir <dir>) ] [ --starch-format <bzip2|gzip> ] < foo.sam\n\n"
             "Options:                                                                            \n"
             "  --help                 Print this help message and exit                           \n"
             "  --keep-header          Preserve header section as pseudo-BED elements             \n"
             "  --split                Split reads with 'N' CIGAR operations into separate BED elements\n"
             "  --all-reads            Include both unmapped and mapped reads in output           \n"
             "  --do-not-sort          Do not sort converted data with BEDOPS sort-bed            \n"
             "  --custom-tags <value>  Add a comma-separated list of custom SAM tags              \n"
             "  --max-mem <value>      Sets aside <value> memory for sorting BED output. For example,\n"
             "                         <value> can be 8G, 8000M or 8000000000 to specify 8 GB of  \n"
             "                         memory (default: 2G)                                       \n"
             "  --sort-tmpdir <dir>    Optionally sets <dir> as temporary directory for sort data, when\n"
             "                         used in conjunction with --max-mem <value>. For example, <dir> can\n"
             "                         be $PWD to store intermediate sort data in the current working\n"
             "                         directory, in place of the host operating system default   \n"
             "                         temporary directory.                                       \n"
             "  --starch-format <bzip2|gzip>                                                      \n"
             "                         Specify backend compression format of starch archive       \n"
             "                         (default: bzip2)                                         \n\n"
             "About:                                                                              \n"
             "  This script converts 1-based, closed [a, b] binary SAM data from standard         \n"
             "  input into 0-based, half-open [a-1, b) extended BED, which is sorted, converted   \n"
             "  to a BEDOPS Starch archive and sent to standard output.                         \n\n"
             "  We process SAM columns from mappable reads (as described by specifications at     \n"
             "  http://samtools.sourceforge.net/SAM1.pdf) converting them into the first six      \n"
             "  UCSC BED columns as follows:                                                    \n\n"
             "  - RNAME                     <-->   chromosome (1st column)                        \n"
             "  - POS - 1                   <-->   start (2nd column)                             \n"
             "  - POS + length(CIGAR) - 1   <-->   stop (3rd column)                              \n"
             "  - QNAME                     <-->   id (4th column)                                \n"
             "  - FLAG                      <-->   score (5th column)                             \n"
             "  - 16 & FLAG                 <-->   strand (6th column)                          \n\n"
             "  The remaining SAM columns are mapped intact, in order, to adjacent BED columns: \n\n"
             "  - MAPQ                                                                            \n"
             "  - CIGAR                                                                           \n"
             "  - RNEXT                                                                           \n"
             "  - PNEXT                                                                           \n"
             "  - TLEN                                                                            \n"
             "  - SEQ                                                                             \n"
             "  - QUAL                                                                          \n\n"
             "  Use the --keep-header option if you would like to preserve the SAM header section \n"
             "  as pseudo-BED elements; otherwise, these are stripped from output.              \n\n"
             "  Because we have mapped all columns, we can translate converted BED data back to   \n"
             "  headerless SAM reads with a simple awk statement or other script that calculates  \n"
             "  1-based coordinates and permutes columns.                                       \n\n"
             "  By default, we only process mapped reads. If you also want to convert unmapped    \n"
             "  reads, add the --all-reads option.                                              \n\n"
             "  In the case of RNA-seq data with skipped regions ('N' components in the read's    \n"
             "  CIGAR string), the --split option will split the read into two or more separate   \n"
             "  BED elements.                                                                   \n\n"
             "  This script also validates the CIGAR strings in a sequencing dataset, in the      \n"
             "  course of converting to BED.                                                    \n\n"
             "  Example usage:                                                                  \n\n"
             "  $ sam2starch < foo.sam > sorted-foo.sam.bed.starch                              \n\n"
             "  Because we make no assumptions about the sort order of your input, we apply the   \n"
             "  sort-bed application to output to generate lexicographically-sorted BED data.   \n\n"
             "  If you want to skip sorting, use the --do-not-sort option:                      \n\n"
             "  $ sam2starch --do-not-sort < foo.sam > unsorted-foo.sam.bed.starch              \n\n"
             "  This option is *not* recommended, however, as other BEDOPS tools require sorted   \n"
             "  inputs to process data correctly.                                                 \n"
             % (sys.argv[0]))
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

def produceSAM(from_stream, to_stream):
    while True:
        sam_line = from_stream.readline()
        if not sam_line:
            from_stream.close()
            to_stream.close()
            break
        to_stream.write(sam_line)
        to_stream.flush()

def consumeSAM(from_stream, to_stream, params):
    while True:
        sam_line = from_stream.readline()
        if not sam_line:
            from_stream.close()
            to_stream.close()
            break
        bed_line = convertSAMToBED(sam_line, params)
        to_stream.write(bed_line)
        to_stream.flush()

def consumeBED(from_stream, to_stream, params):
    while True:
        bed_line = from_stream.readline()
        if not bed_line:
            from_stream.close()
            to_stream.close()
            break
        to_stream.write(bed_line)
        to_stream.flush()

def convertSAMToBED(line, params):

    convertedLine = ""
    samRecord = SamRecord()

    if line.startswith('@') and params.keepHeader:
        elem_chr = params.keepHeaderChr
        elem_start = str(params.keepHeaderIdx)
        elem_stop = str(params.keepHeaderIdx + 1)
        elem_id = line
        convertedLine = '\t'.join([elem_chr, elem_start, elem_stop, elem_id])
        params.keepHeaderIdx += 1

    elif line.startswith('@') and not params.keepHeader:
        pass

    else:
        chomped_line = line.rstrip(os.linesep)
        elems = chomped_line.split('\t')
    
        # parse CIGAR string into operations and do validation; bring in flag and set strand value
    
        samRecord.cigar = str(elems[5])
        samRecord.flag = int(elems[1])
    
        # if read is mappable (as determined by FLAG value) or we use the --all-reads option, then process the read
        
        if params.includeAllReads or samRecord.isMappedRead():
        
            samRecord.rname = elems[2]
            samRecord.start = int(elems[3]) - 1
            samRecord.qname = elems[0]
            samRecord.mapq = elems[4]
            samRecord.rnext = elems[6]
            samRecord.pnext = int(elems[7])
            samRecord.tlen = int(elems[8])
            samRecord.seq = elems[9]
            samRecord.qual = elems[10]

            # optional fields are TAG:TYPE:VALUE triplets
            samTagsList = []
            samTags = SamTags()
            if params.customTagsAdded:
                samTags.customTagsAdded = params.customTagsAdded
                samTags.customTagsStr = params.customTagsStr
            if len(elems) >= 11:
                for idx in range(11, len(elems)):
                    samTagsList.append(elems[idx])
                    samTags.append(elems[idx])
            samRecord.tags = str('\t'.join(samTagsList))

            # if we don't need to split the read, or even if we do, but the
            # read does not have a split operation in its CIGAR string, then
            # we set the stop coordinate and print out the converted read
        
            if not params.inputNeedsSplitting:
                samRecord.stop = samRecord.start + samRecord.cigarOps.readLength()
                convertedLine = samRecord.asBed()

                # otherwise, if we need to split reads, then we write two converted
                # strings with adjusted coordinates and qname/ID value

            elif inputNeedsSplitting:
                samRecordBlockIdx = 1
                samRecordPreviousOp = ""
                samRecord.originalQname = samRecord.qname
            
                # loop through ops, one op at a time
                for op in samRecord.cigarOps.operations:
                    if op.key == 'M':
                        samRecord.stop = samRecord.start + op.value
                        if samRecordPreviousOp:
                            if samRecordPreviousOp == 'D' or samRecordPreviousOp == 'N':
                                # print record
                                samRecord.qname += '/' + str(samRecordBlockIdx)
                                convertedLine += samRecord.asBed()
                                # set new coordinates, reset ID and increment block index
                                samRecord.start = samRecord.stop
                                samRecord.qname = samRecord.originalQname
                                samRecordBlockIdx += 1
                    elif op.key == 'N':
                        # print record
                        samRecord.qname += '/' + str(samRecordBlockIdx)
                        convertedLine += samRecord.asBed()
                        # set new coordinates, reset ID and increment block index
                        samRecord.stop += op.value
                        samRecord.start = samRecord.stop
                        samRecord.qname = samRecord.originalQname
                        samRecordBlockIdx += 1
                    elif op.key == 'D':
                        # set new coordinates
                        samRecord.stop += op.value
                        samRecord.start = samRecord.stop                            
                    elif op.key == 'H' or op.key == 'I' or op.key == 'P' or op.key == 'S':
                        pass
                    samRecordPreviousOp = op.key
                        
                # if the CIGAR string does not contain a split or deletion 
                # op ('N', 'D') then just print out the record
                if samRecordBlockIdx == 1:
                    convertedLine = samRecord.asBed()

    return convertedLine

class Parameters:
    def __init__(self):
        self._keepHeader = False
        self._keepHeaderIdx = 0
        self._keepHeaderChr = "_header"
        self._sortOutput = True
        self._sortTmpdir = None
        self._sortTmpdirSet = False
        self._inputNeedsSplitting = False
        self._includeAllReads = False
        self._maxMem = "2G"
        self._maxMemChanged = False
        self._customTagsStr = ""
        self._customTagsAdded = False
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
    def inputNeedsSplitting(self):
        return self._inputNeedsSplitting
    @inputNeedsSplitting.setter
    def inputNeedsSplitting(self, flag):
        self._inputNeedsSplitting = flag

    @property
    def includeAllReads(self):
        return self._includeAllReads
    @includeAllReads.setter
    def includeAllReads(self, flag):
        self._includeAllReads = flag

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
    def customTagsStr(self):
        return self._customTagsStr
    @customTagsStr.setter
    def customTagsStr(self, val):
        self._customTagsStr = val

    @property
    def customTagsAdded(self):
        return self._customTagsAdded
    @customTagsAdded.setter
    def customTagsAdded(self, flag):
        self._customTagsAdded = flag

    @property
    def starchFormat(self):
        return self._starchFormat
    @starchFormat.setter
    def starchFormat(self, val):
        self._starchFormat = val

def main(*args):

    requiredVersion = (2,7,0)
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
    longopts = ["do-not-sort", "keep-header", "split", "all-reads", "help", "custom-tags=", "max-mem=", "starch-format=", "sort-tmpdir="]
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
        elif key in ("--split"):
            params.inputNeedsSplitting = True
        elif key in ("--all-reads"):
            params.includeAllReads = True
        elif key in ("--do-not-sort"):
            params.sortOutput = False
        elif key in ("--sort-tmpdir"):
            params.sortTmpdir = str(value)
            params.sortTmpdirSet = True
        elif key in ("--custom-tags"):
            params.customTagsStr = str(value)
            params.customTagsAdded = True
        elif key in ("--max-mem"):
            params.maxMem = str(value)
            params.maxMemChanged = True

    if params.inputNeedsSplitting and not params.sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --split parameters\n" % sys.argv[0] )
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
        sys.stderr.write( "[%s] - Error: Please redirect or pipe in BAM-formatted data\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_NOINPUT

    try:
        if which('samtools') is None:
            raise IOError("The samtools binary could not be found in your user PATH -- please locate and install this binary")
        if which('sort-bed') is None:
            raise IOError("The sort-bed binary could not be found in your user PATH -- please locate and install this binary")
        if which('starch') is None:
            raise IOError("The starch binary could not be found in your user PATH -- please locate and install this binary")
    except IOError as msg:
        sys.stderr.write( "[%s] - %s\n" % (sys.argv[0], msg) )
        return os.EX_OSFILE

    sam_process = subprocess.Popen(['samtools', 'view', '-h', '-S', '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    produce_sam_thread = threading.Thread(target=produceSAM, args=(sys.stdin, sam_process.stdin))

    starch_process = subprocess.Popen(['starch', params.starchFormat, '-'], stdin=subprocess.PIPE)

    if params.sortOutput:
        if params.sortTmpdirSet:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '--tmpdir', params.sortTmpdir, '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        else:
            sortbed_process = subprocess.Popen(['sort-bed', '--max-mem', params.maxMem, '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        convert_to_bed_thread = threading.Thread(target=consumeSAM, args=(sam_process.stdout, sortbed_process.stdin, params))
        pass_bed_to_starch_thread = threading.Thread(target=consumeBED, args=(sortbed_process.stdout, starch_process.stdin, params))
    else:
        convert_to_bed_thread = threading.Thread(target=consumeSAM, args=(sam_process.stdout, starch_process.stdin, params))
        
    produce_sam_thread.start()
    convert_to_bed_thread.start()
    if params.sortOutput:
        pass_bed_to_starch_thread.start()

    produce_sam_thread.join()
    convert_to_bed_thread.join()
    if params.sortOutput:
        pass_bed_to_starch_thread.join()

    sam_process.wait()
    if params.sortOutput:
        sortbed_process.wait()
    starch_process.wait()

    #
    # This helps ensure that an early error-bsaed exit from samtools or sort-bed will result in an error exit status code
    #

    if params.sortOutput and int(sortbed_process.returncode) != 0:
        return os.EX_IOERR

    if int(starch_process.returncode) != 0 or int(sam_process.returncode) != 0:
        return os.EX_IOERR

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
