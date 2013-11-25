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
# Author:       Alex Reynolds and Eric Haugen
#
# Project:      Converts 0-based, half-open [a-1,b) headered or headerless BAM input
#               into 0-based, half-open [a-1,b) extended BED
#
# Version:      2.4
#
# Notes:        The BAM format is an indexed, binary representation of a SAM (Sequence
#               Alignment/Map) file. Internally, it is a 0-based, half-open [a-1,b)
#               file, but printing it to text via samtools turns it into a SAM file, which
#               is 1-based, closed [a,b]. We convert this indexing back to 0-based, half-
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
#               This script also validates the CIGAR strings in a sequencing dataset, in the
#               course of converting to BED.
#
#               Example usage:
#
#               $ bam2bed < foo.bam > sorted-foo.bam.bed
#
#               We make no assumptions about sort order from converted output. Apply
#               the usage case displayed to pass data to the BEDOPS sort-bed application, 
#               which generates lexicographically-sorted BED data as output.
#
#               If you want to skip sorting, use the --do-not-sort option:
#
#               $ bam2bed --do-not-sort < foo.bam > unsorted-foo.bam.bed
#

import getopt, sys, os, stat, subprocess, signal, tempfile, re

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
        if customTagsAdded:
            customTags = customTagsStr.split(",")
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
            

def printUsage(stream):
    usage = ("Usage:\n"
             "  %s [ --help ] [ --split ] [ --all-reads ] [ --do-not-sort | --max-mem <value> ] < foo.bam\n\n"
             "Options:\n"
             "  --help                 Print this help message and exit\n"
             "  --split                Split reads with 'N' CIGAR operations into separate BED elements\n"
             "  --all-reads            Include both unmapped and mapped reads in output\n"
             "  --do-not-sort          Do not sort converted data with BEDOPS sort-bed\n"
             "  --custom-tags <value>  Add a comma-separated list of custom SAM tags\n"
             "  --max-mem <value>      Sets aside <value> memory for sorting BED output. For example,\n"
             "                         <value> can be 8G, 8000M or 8000000000 to specify 8 GB of memory\n"
             "                         (default: 2G)\n\n"
             "About:\n"
             "  This script converts 0-based, half-open [a-1, b) binary BAM data from standard    \n"
             "  input into 0-based, half-open [a-1, b) extended BED, which is sorted and sent     \n"
             "  to standard output.                                                             \n\n"
             "  We process BAM columns from mappable reads (as described by specifications at     \n"
             "  http://samtools.sourceforge.net/SAM1.pdf) converting them into the first six      \n"
             "  UCSC BED columns as follows:                                                    \n\n"
             "  - RNAME                     <-->   chromosome (1st column)                        \n"
             "  - POS - 1                   <-->   start (2nd column)                             \n"
             "  - POS + length(CIGAR) - 1   <-->   stop (3rd column)                              \n"
             "  - QNAME                     <-->   id (4th column)                                \n"
             "  - FLAG                      <-->   score (5th column)                             \n"
             "  - 16 & FLAG                 <-->   strand (6th column)                          \n\n"
             "  The remaining BAM columns are mapped intact, in order, to adjacent BED columns: \n\n"
             "  - MAPQ                                                                            \n"
             "  - CIGAR                                                                           \n"
             "  - RNEXT                                                                           \n"
             "  - PNEXT                                                                           \n"
             "  - TLEN                                                                            \n"
             "  - SEQ                                                                             \n"
             "  - QUAL                                                                          \n\n"
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
             "  $ bam2bed < foo.bam > sorted-foo.bam.bed                                        \n\n"
             "  Because we make no assumptions about the sort order of your input, we apply the   \n"
             "  sort-bed application to output to generate lexicographically-sorted BED data.   \n\n"
             "  If you want to skip sorting, use the --do-not-sort option:                      \n\n"
             "  $ bam2bed --do-not-sort < foo.bam > unsorted-foo.bam.bed                        \n\n"
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
    if currentVersion[0] == rv[0] and currentVersion[1] >= rv[1]:
        pass
    else:
        sys.stderr.write( "[%s] - Error: Your Python interpreter must be %d.%d or greater (within major version %d)\n" % (sys.argv[0], rv[0], rv[1], rv[0]) )
        sys.exit(os.EX_CONFIG)
    return os.EX_OK

def printRecord():
    pass

def main(*args):
    requiredVersion = (2,7)
    checkInstallation(requiredVersion)

    sortOutput = True
    inputNeedsSplitting = False
    includeAllReads = False
    maxMem = "2G"
    maxMemChanged = False
    customTagsStr = ""
    customTagsAdded = False

    # fixes bug with Python handling of SIGPIPE signal from UNIX head, etc.
    # http://coding.derkeiler.com/Archive/Python/comp.lang.python/2004-06/3823.html
    signal.signal(signal.SIGPIPE,signal.SIG_DFL)

    optstr = ""
    longopts = ["do-not-sort", "split", "all-reads", "help", "custom-tags=", "max-mem="]
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
        elif key in ("--split"):
            inputNeedsSplitting = True
        elif key in ("--all-reads"):
            includeAllReads = True
        elif key in ("--do-not-sort"):
            sortOutput = False
        elif key in ("--custom-tags"):
            customTagsStr = str(value)
            customTagsAdded = True
        elif key in ("--max-mem"):
            maxMem = str(value)
            maxMemChanged = True

    if maxMemChanged:
        sys.stderr.write( "[%s] - Warning: The --max-mem parameter is currently ignored (cf. https://github.com/bedops/bedops/issues/1 )\n" % sys.argv[0] )

    if inputNeedsSplitting and not sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --split parameters\n" % sys.argv[0] )
        printUsage("stderr")
        return os.EX_USAGE 

    if maxMemChanged and not sortOutput:
        sys.stderr.write( "[%s] - Error: Cannot specify both --do-not-sort and --max-mem parameters\n" % sys.argv[0] )
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

    samTF = tempfile.NamedTemporaryFile(mode='wb')
    if sortOutput:
        sortTF = tempfile.NamedTemporaryFile(mode='wb', delete=False)

    #
    # read BAM data into samtools process
    #

    samProcess = subprocess.Popen(['samtools', 'view', '-'], stdin=subprocess.PIPE, stdout=samTF)
    while True:
        bamByte = sys.stdin.read()
        if not bamByte:
            break
        samProcess.stdin.write(bamByte)
        samProcess.stdin.flush()

    samTF.seek(0)
    with open(samTF.name) as samData:

        samRecord = SamRecord()

        for line in samData:

            chomped_line = line.rstrip(os.linesep)            
            elems = chomped_line.split('\t')

            # parse CIGAR string into operations and do validation
            # bring in flag and set strand value

            samRecord.cigar = str(elems[5])
            samRecord.flag = int(elems[1])

            # if read is mappable (as determined by FLAG value) or we 
            # use the --all-reads option, then process the read

            if includeAllReads or samRecord.isMappedRead():

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
                if len(elems) >= 11:
                    for idx in range(11, len(elems)):
                        samTagsList.append(elems[idx])
                        samTags.append(elems[idx])
                samRecord.tags = str('\t'.join(samTagsList))

                # if we don't need to split the read, or even if we do, but the
                # read does not have a split operation in its CIGAR string, then
                # we set the stop coordinate and print out the converted read

                if not inputNeedsSplitting:
                    samRecord.stop = samRecord.start + samRecord.cigarOps.readLength()
                    # write output
                    if sortOutput:
                        sortTF.write(samRecord.asBed())
                    else:
                        sys.stdout.write(samRecord.asBed())

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
                                    if sortOutput:
                                        sortTF.write(samRecord.asBed())
                                    else:
                                        sys.stdout.write(samRecord.asBed())
                                    # set new coordinates, reset ID and increment block index
                                    samRecord.start = samRecord.stop
                                    samRecord.qname = samRecord.originalQname
                                    samRecordBlockIdx += 1
                        elif op.key == 'N':
                            # print record
                            samRecord.qname += '/' + str(samRecordBlockIdx)
                            if sortOutput:
                                sortTF.write(samRecord.asBed())
                            else:
                                sys.stdout.write(samRecord.asBed())
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
                        if sortOutput:
                            sortTF.write(samRecord.asBed())
                        else:
                            sys.stdout.write(samRecord.asBed())

    if sortOutput:
        sortTF.close()
        sortProcess = subprocess.Popen(['sort-bed', sortTF.name])
        sortProcess.wait()
        try:
            os.remove(sortTF.name)
        except OSError:
            sys.stderr.write( "[%s] - Warning: Could not delete intermediate sorted file [%s]\n" % (sys.argv[0], sortTF.name) )        

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
