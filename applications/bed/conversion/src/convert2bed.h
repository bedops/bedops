/* 
   convert2bed.h
   -----------------------------------------------------------------------
   Copyright (C) 2014 Alex Reynolds

   wig2bed components, (C) 2011-2014 Scott Kuehn and Shane Neph

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef C2B_H
#define C2B_H

#define __STDC_FORMAT_MACROS

#ifdef __cplusplus
#include <cstdio>
#include <cstdlib>
#include <cgetopt>
#include <cstring>
#include <cassert>
#include <cctype>
#include <cinttypes>
#include <cerrno>
#else
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>
#endif
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/wait.h>

#define C2B_VERSION "2.4.3"

typedef int boolean;
extern const boolean kTrue;
extern const boolean kFalse;
const boolean kTrue = 1;
const boolean kFalse = 0;

#define C2B_MAX_FIELD_COUNT_VALUE 64
#define C2B_MAX_FIELD_LENGTH_VALUE 8192
#define C2B_MAX_OPERATION_FIELD_LENGTH_VALUE 32
#define C2B_MAX_STRAND_LENGTH_VALUE 4
#define C2B_MAX_LINE_LENGTH_VALUE 131072
#define C2B_MAX_LINES_VALUE 32
#define C2B_MAX_OPERATIONS_VALUE 32
#define C2B_MAX_CHROMOSOME_LENGTH 32

extern const char *c2b_samtools;
extern const char *c2b_sort_bed;
extern const char *c2b_starch;
extern const char *c2b_cat;
extern const char *c2b_default_output_format;
extern const char *c2b_unmapped_read_chr_name;
extern const char *c2b_header_chr_name;
extern const char *sort_bed_max_mem_arg;
extern const char *sort_bed_max_mem_default_arg;
extern const char *sort_bed_tmpdir_arg;
extern const char *sort_bed_stdin;
extern const char *starch_bzip2_arg;
extern const char *starch_gzip_arg;
extern const char *starch_note_prefix_arg;
extern const char *starch_note_suffix_arg;
extern const char *starch_stdin_arg;
extern const char c2b_tab_delim;
extern const char c2b_line_delim;
extern const char c2b_sam_header_prefix;
extern const char *c2b_gff_header;
extern const char *c2b_gff_fasta;
extern const int c2b_gff_field_min;
extern const int c2b_gff_field_max;
extern const char *c2b_gff_zero_length_insertion_attribute;
extern const int c2b_gtf_field_min;
extern const int c2b_gtf_field_max;
extern const char c2b_gtf_comment;
extern const char *c2b_gtf_zero_length_insertion_attribute;
extern const int c2b_psl_field_min;
extern const int c2b_psl_field_max;
extern const int c2b_vcf_field_min;
extern const char c2b_vcf_header_prefix;
extern const char c2b_vcf_alt_allele_delim;
extern const char c2b_vcf_id_prefix;
extern const char c2b_vcf_id_suffix;
extern const char c2b_wig_header_prefix;
extern const char *c2b_wig_track_prefix;
extern const char *c2b_wig_browser_prefix;
extern const char *c2b_wig_variable_step_prefix;
extern const char *c2b_wig_fixed_step_prefix;
extern const char *c2b_wig_chr_prefix;

const char *c2b_samtools = "samtools";
const char *c2b_sort_bed = "sort-bed";
const char *c2b_starch = "starch";
const char *c2b_cat = "cat";
const char *c2b_default_output_format = "bed";
const char *c2b_unmapped_read_chr_name = "_unmapped";
const char *c2b_header_chr_name = "_header";
const char *sort_bed_max_mem_arg = " --max-mem ";
const char *sort_bed_max_mem_default_arg = " --max-mem 2G ";
const char *sort_bed_tmpdir_arg = " --tmpdir ";
const char *sort_bed_stdin = " - ";
const char *starch_bzip2_arg = " --bzip2 ";
const char *starch_gzip_arg = " --gzip ";
const char *starch_note_prefix_arg = " --note=\"";
const char *starch_note_suffix_arg = "\" ";
const char *starch_stdin_arg = " - ";
const char c2b_tab_delim = '\t';
const char c2b_line_delim = '\t';
const char c2b_sam_header_prefix = '@';
const char *c2b_gff_header = "##gff-version 3";
const char *c2b_gff_fasta = "##FASTA";
const int c2b_gff_field_min = 9;
const int c2b_gff_field_max = 9;
const char *c2b_gff_zero_length_insertion_attribute = ";zero_length_insertion=True";
const int c2b_gtf_field_min = 9;
const int c2b_gtf_field_max = 10;
const char c2b_gtf_comment = '#';
const char *c2b_gtf_zero_length_insertion_attribute = "; zero_length_insertion=True";
const int c2b_psl_field_min = 21;
const int c2b_psl_field_max = 21;
const int c2b_vcf_field_min = 8;
const char c2b_vcf_header_prefix = '#';
const char c2b_vcf_alt_allele_delim = ',';
const char c2b_vcf_id_prefix = '<';
const char c2b_vcf_id_suffix = '>';
const char c2b_wig_header_prefix = '#';
const char *c2b_wig_track_prefix = "track";
const char *c2b_wig_browser_prefix = "browser";
const char *c2b_wig_variable_step_prefix = "variableStep";
const char *c2b_wig_fixed_step_prefix = "fixedStep";
const char *c2b_wig_chr_prefix = "chr";

/* 
   Allowed input and output formats
*/

typedef enum format {
    BED_FORMAT,
    STARCH_FORMAT,
    BAM_FORMAT,
    GFF_FORMAT,
    GTF_FORMAT,
    PSL_FORMAT,
    SAM_FORMAT,
    VCF_FORMAT,
    WIG_FORMAT,
    UNDEFINED_FORMAT
} c2b_format_t;

/* 
   BAM/SAM CIGAR operations
   -------------------------------------------------------------------------
   Allowed ops: \*|([0-9]+[MIDNSHPX=])+
*/

typedef struct cigar_op {
    unsigned int bases;
    char operation;
} c2b_cigar_op_t;

typedef struct cigar {
    c2b_cigar_op_t *ops;
    ssize_t size;
    ssize_t length;
} c2b_cigar_t;

extern const unsigned int default_cigar_op_bases;
const unsigned int default_cigar_op_bases = 0;

extern const char default_cigar_op_operation;
const char default_cigar_op_operation = '-';

/* 
   The SAM format is described at:
   
   http://samtools.github.io/hts-specs/SAMv1.pdf
   
   SAM fields are in the following ordering:
   
   Index   SAM field
   ---------------------------------------------------------
   0       QNAME
   1       FLAG
   2       RNAME
   3       POS
   4       MAPQ
   5       CIGAR
   6       RNEXT
   7       PNEXT
   8       TLEN
   9       SEQ
   10      QUAL
   11+     Optional alignment section fields (TAG:TYPE:VALUE)
*/

typedef struct sam {
    char *qname;
    int flag;
    char *strand;
    char *rname;
    uint64_t start;
    uint64_t stop;
    char *mapq;
    char *cigar;
    char *rnext;
    char *pnext;
    char *tlen;
    char *seq;
    char *qual;
    char *opt;
} c2b_sam_t;

/* 
   The GFF format is described at:

   http://www.sequenceontology.org/gff3.shtml

   GFF fields are in the following ordering:

   Index   GFF field
   ---------------------------------------------------------
   0       seqid
   1       source
   2       type
   3       start
   4       end
   5       score
   6       strand
   7       phase
   8       attributes
*/

typedef struct gff {
    char *seqid;
    char *source;
    char *type;
    uint64_t start;
    uint64_t end;
    char *score;
    char *strand;
    char *phase;
    char *attributes;
    char *id;
} c2b_gff_t;

/* 
   The GTF format is described at:

   http://mblab.wustl.edu/GTF22.html

   GTF fields are in the following ordering:

   Index   GTF field
   ---------------------------------------------------------
   0       seqname
   1       soure
   2       feature
   3       start
   4       end
   5       score
   6       strand
   7       frame
   8       (optional) attributes
   9       (optional) comments
*/

typedef struct gtf {
    char *seqname;
    char *source;
    char *feature;
    uint64_t start;
    uint64_t end;
    char *score;
    char *strand;
    char *frame;
    char *attributes;
    char *id;
    char *comments;
} c2b_gtf_t;

/* 
   The PSL format is described at:

   http://genome.ucsc.edu/FAQ/FAQformat.html#format2

   PSL fields are in the following ordering:

   Index   PSL field
   ---------------------------------------------------------
   0       matches
   1       misMatches
   2       repMatches
   3       nCount
   4       qNumInsert
   5       qBaseInsert
   6       tNumInsert
   7       tBaseInsert
   8       strand
   9       qName
   10      qSize
   11      qStart
   12      qEnd
   13      tName
   14      tSize
   15      tStart
   16      tEnd
   17      blockCount
   18      blockSizes
   19      qStarts
   20      tStarts
*/

typedef struct psl {
    uint64_t matches;
    uint64_t misMatches;
    uint64_t repMatches;
    uint64_t nCount;
    uint64_t qNumInsert;
    uint64_t qBaseInsert;
    uint64_t tNumInsert;
    uint64_t tBaseInsert;
    char *strand;
    char *qName;
    uint64_t qSize;
    uint64_t qStart;
    uint64_t qEnd;
    char *tName;
    uint64_t tSize;
    uint64_t tStart;
    uint64_t tEnd;
    uint64_t blockCount;
    char *blockSizes;
    char *qStarts;
    char *tStarts;
} c2b_psl_t;

/* 
   The VCF v4.2 format is described at:

   http://samtools.github.io/hts-specs/VCFv4.2.pdf

   VCF fields are in the following ordering:

   Index   VCF field
   ---------------------------------------------------------
   0       CHROM
   1       POS
   2       ID
   3       REF
   4       ALT
   5       QUAL
   6       FILTER
   7       INFO

   If genotype data is present in the file, these are followed 
   by a FORMAT column header, then an arbitrary number of sample 
   IDs. The header line is tab-delimited.

   Index   VCF field
   ---------------------------------------------------------
   8       FORMAT
   9       Sample 1
   10      Sample 2
   11+      ...
*/

typedef struct vcf {
    char *chrom;
    uint64_t pos;
    uint64_t start;
    uint64_t end;
    char *id;
    char *ref;
    char *alt;
    char *qual;
    char *filter;
    char *info;
    char *format;
    char *samples;
} c2b_vcf_t;

/* 
   At most, we need 4 pipes to handle the most complex conversion
   pipeline used with the BEDOPS suite: 
   
    BAM -> SAM -> BED (unsorted) -> BED (sorted) -> Starch
   
   Here, each arrow represents a unidirectional path of data 
   between processing steps. 
   
   The other two possibilities are:
     
    BAM -> SAM -> BED (unsorted) -> BED (sorted)
    BAM -> SAM -> BED (unsorted)
   
   More generically, other formats typically follow one of these 
   three paths:

    XYZ -> BED (unsorted) -> BED (sorted) -> Starch
    XYZ -> BED (unsorted) -> BED (sorted)
    XYZ -> BED (unsorted)

   Here, XYZ is one of GFF, GTF, PSL, SAM, VCF, or WIG.
   
   If a more complex pipeline ever arises, we could just increase the
   value of MAX_PIPES.

   Each pipe has a read and write stream. The write stream handles
   data sent via the out and err file handles. We bundle all the pipes
   into a "pipeset" for use at a processing stage, described below.
*/

#define PIPE_READ 0
#define PIPE_WRITE 1
#define PIPE_STREAMS 2
#define MAX_PIPES 4

typedef struct pipeset {
    int **in;
    int **out;
    int **err;
    size_t num;
} c2b_pipeset_t;

/* 
   A pipeline stage contains a pipeset (set of I/O pipes), source
   and destination stage IDs, and a "line functor" which generally 
   processes fields from a precursor format to BED. This functor is
   specific to the specified input format. This stage is passed to 
   each processing thread.
*/

typedef struct pipeline_stage {
    c2b_pipeset_t *pipeset;
    unsigned int src;
    unsigned int dest;
    void (*line_functor)();
    int status;
    char *description;
    pid_t pid;
} c2b_pipeline_stage_t;

#define PIPE4_FLAG_NONE       (0U)
#define PIPE4_FLAG_RD_CLOEXEC (1U << 0)
#define PIPE4_FLAG_WR_CLOEXEC (1U << 1)

#define c2b_pipe4_cloexec(fd) c2b_pipe4((fd), PIPE4_FLAG_RD_CLOEXEC | PIPE4_FLAG_WR_CLOEXEC)

#define POPEN4_FLAG_NONE                        (0U)
#define POPEN4_FLAG_NOCLOSE_PARENT_STDIN        (1U << 0)
#define POPEN4_FLAG_NOCLOSE_PARENT_STDOUT       (1U << 1)
#define POPEN4_FLAG_NOCLOSE_PARENT_STDERR       (1U << 2)
#define POPEN4_FLAG_CLOSE_CHILD_STDIN           (1U << 3)
#define POPEN4_FLAG_CLOSE_CHILD_STDOUT          (1U << 4)
#define POPEN4_FLAG_CLOSE_CHILD_STDERR          (1U << 5)

static const char *general_name = "convert2bed";

static const char *version = C2B_VERSION;

static const char *authors = "Alex Reynolds";

static const char *general_usage = "\n"                                 \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ convert2bed --input=fmt [--output=fmt] [options] < input > output\n";

static const char *general_description =                                \
    "  Convert BAM, GFF, GTF, PSL, SAM, VCF and WIG genomic formats to BED or BEDOPS Starch (compressed BED)\n" \
    "\n"                                                                \
    "  Input can be a regular file or standard input piped in using the hyphen character ('-'):\n" \
    "\n"                                                                \
    "  $ upstream_process ... | convert2bed --input=fmt - > output\n";

static const char *general_io_options =                                 \
    "  Input (required):\n\n"                                           \
    "  --input=[bam|gff|gtf|psl|sam|vcf|wig] (-i <fmt>)\n"              \
    "      Genomic format of input file (required)\n\n"                 \
    "  Output:\n\n"                                                     \
    "  --output=[bed|starch] (-o <fmt>)\n"                              \
    "      Format of output file, either BED or BEDOPS Starch (optional, default is BED)\n";

static const char *general_options =                                    \
    "  Other processing options:\n\n"                                   \
    "  --do-not-sort (-d)\n"                                            \
    "      Do not sort BED output with sort-bed (not compatible with --output=starch)\n" \
    "  --max-mem=<value> (-m <val>)\n"                                  \
    "      Sets aside <value> memory for sorting BED output. For example, <value> can\n" \
    "      be 8G, 8000M or 8000000000 to specify 8 GB of memory (default is 2G)\n" \
    "  --sort-tmpdir=<dir> (-r <dir>)\n"                                \
    "      Optionally sets [dir] as temporary directory for sort data, when used in\n" \
    "      conjunction with --max-mem=[value], instead of the host's operating system\n" \
    "      default temporary directory\n"                               \
    "  --starch-bzip2 (-z)\n"                                           \
    "      Used with --output=starch, the compressed output explicitly applies the bzip2\n" \
    "      algorithm to compress intermediate data (default is bzip2)\n" \
    "  --starch-gzip (-g)\n"                                            \
    "      Used with --output=starch, the compressed output applies gzip compression on\n" \
    "      intermediate data\n"                                         \
    "  --starch-note=\"xyz...\" (-e \"xyz...\")\n"                      \
    "      Used with --output=starch, this adds a note to the Starch archive metadata\n" \
    "  --help | --help[-bam|-gff|-gtf|-psl|-sam|-vcf|-wig] (-h | -h <fmt>)\n" \
    "      Show general help message (or detailed help for a specified input format)\n";

static const char *bam_name = "convert2bed -i bam";

static const char *bam_description =                                    \
    "  The BAM format is an indexed, binary representation of a SAM (Sequence\n" \
    "  Alignment/Map) file. Internally, it is a 0-based, half-open [a-1,b)\n" \
    "  file, but printing it to text via samtools turns it into a SAM file, which\n" \
    "  is 1-based, closed [a,b]. We convert this indexing back to 0-based, half-\n" \
    "  open when creating BED output.\n"                                \
    "\n"                                                                \
    "  We process SAM columns from mappable reads (as described by \n"  \
    "  http://samtools.github.io/hts-specs/SAMv1.pdf) converting them into the first\n" \
    "  six UCSC BED columns as follows:\n"                              \
    "\n"                                                                \
    "  - RNAME                     <-->   chromosome (1st column)\n"    \
    "  - POS - 1                   <-->   start (2nd column)\n"         \
    "  - POS + length(CIGAR) - 1   <-->   stop (3rd column)\n"          \
    "  - QNAME                     <-->   id (4th column)\n"            \
    "  - FLAG                      <-->   score (5th column)\n"         \
    "  - 16 & FLAG                 <-->   strand (6th column)\n"        \
    "\n"                                                                \
    "  The remaining SAM columns are mapped intact, in order, to adjacent BED\n" \
    "  columns:\n"                                                      \
    "\n"                                                                \
    "  - MAPQ\n"                                                        \
    "  - CIGAR\n"                                                       \
    "  - RNEXT\n"                                                       \
    "  - PNEXT\n"                                                       \
    "  - TLEN\n"                                                        \
    "  - SEQ\n"                                                         \
    "  - QUAL\n"                                                        \
    "\n"                                                                \
    "  Because we have mapped all columns, we can translate converted BED data back\n" \
    "  to headerless SAM reads with a simple awk statement or other script that\n" \
    "  calculates 1-based coordinates and permutes columns.\n"          \
    "\n"                                                                \
    "  By default, we only process mapped reads. If you also want to convert unmapped\n" \
    "  reads, add the --all-reads option.\n"                            \
    "\n"                                                                \
    "  In the case of RNA-seq data with skipped regions ('N' components in the\n" \
    "  read's CIGAR string), the --split option will split the read into two or more\n" \
    "  separate BED elements.\n"                                        \
    "\n"                                                                \
    "  The header section is normally stripped from the output. You can use the\n" \
    "  --keep-header option to preserve the header data from the SAM input as\n" \
    "  pseudo-BED elements that use the \"_header\" chromosome name.\n";

static const char *bam_options =                                        \
    "  BAM conversion options:\n\n"                                     \
    "  --all-reads (-a)\n"                                              \
    "      Include both unmapped and mapped reads in output\n"          \
    "  --keep-header (-k)\n"                                            \
    "      Preserve header section as pseudo-BED elements\n"            \
    "  --split (-s)\n"                                                  \
    "      Split reads with 'N' CIGAR operations into separate BED\n"   \
    "      elements\n";

static const char *bam_usage =                                          \
    "  Converts 0-based, half-open [a-1,b) headered or headerless BAM input\n" \
    "  into 0-based, half-open [a-1,b) extended BED or BEDOPS Starch\n\n" \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ bam2bed < foo.bam > sorted-foo.bam.bed\n"                      \
    "  $ bam2starch < foo.bam > sorted-foo.bam.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i bam < foo.bam > sorted-foo.bam.bed\n"           \
    "  $ convert2bed -i bam -o starch < foo.bam > sorted-foo.bam.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ bam2bed --do-not-sort < foo.bam > unsorted-foo.bam.bed\n";

static const char *gff_name = "convert2bed -i gff";

static const char *gff_description =                                    \
    "  The GFF3 specification (http://www.sequenceontology.org/gff3.shtml) \n" \
    "  contains columns that do not map directly to common or UCSC BED columns.\n" \
    "  Therefore, we add the following columns to preserve the ability to\n" \
    "  seamlessly convert back to GFF3 after performing operations with\n" \
    "  bedops, bedmap, or other BEDOPS or BED-processing tools.\n"      \
    "\n"                                                                \
    "  - The 'source' GFF column data maps to the 7th BED column\n"     \
    "  - The 'type' data maps to the 8th BED column\n"                  \
    "  - The 'phase' data maps to the 9th BED column\n"                 \
    "  - The 'attributes' data maps to the 10th BED column\n"           \
    "\n"                                                                \
    "  We make the following assumptions about the GFF3 input data:\n"  \
    "\n"                                                                \
    "  - The 'seqid' GFF column data maps to the chromosome label (1st BED column)\n" \
    "  - The 'ID' attribute in the 'attributes' GFF column (if present) maps to\n" \
    "    the element ID (4th BED column)\n"                             \
    "  - The 'score' and 'strand' GFF columns (if present) are mapped to the\n" \
    "    5th and 6th BED columns, respectively\n"                       \
    "\n"                                                                \
    "  If we encounter zero-length insertion elements (which are defined\n" \
    "  where the start and stop GFF column data values are equivalent), the\n" \
    "  start coordinate is decremented to convert to 0-based, half-open indexing,\n" \
    "  and a 'zero_length_insertion' attribute is added to the 'attributes' GFF\n" \
    "  column data.\n"                                                  \
    "\n"                                                                \
    "  Metadata and header fields are usually stripped. Use the --keep-header\n" \
    "  option to preserve these data as pseudo-BED elements that use the \"_header\"\n" \
    "  chromosome name.\n";

static const char *gff_options =                                        \
    "  GFF conversion options:\n\n"                                     \
    "  --keep-header (-k)\n"                                            \
    "      Preserve header section as pseudo-BED elements\n";

static const char *gff_usage =                                          \
    "  Converts 1-based, closed [a, b] GFF3 input into 0-based, half-\n" \
    "  open [a-1, b) six-column extended BED or BEDOPS Starch\n"        \
    "\n"                                                                \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ gff2bed < foo.gff > sorted-foo.gff.bed\n"                      \
    "  $ gff2starch < foo.gff > sorted-foo.gff.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i gff < foo.gff > sorted-foo.gff.bed\n"           \
    "  $ convert2bed -i gff -o starch < foo.gff > sorted-foo.gff.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the sort-bed application,\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ gff2bed --do-not-sort < foo.gff > unsorted-foo.gff.bed\n";

static const char *gtf_name = "convert2bed -i gtf";

static const char *gtf_usage =                                          \
    "  Converts 1-based, closed [a, b] GTF2.2 input into 0-based, half-\n" \
    "  open [a-1, b) six-column extended BED and BEDOPS Starch\n"       \
    "\n"                                                                \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ gtf2bed < foo.gtf > sorted-foo.gtf.bed\n"                      \
    "  $ gtf2starch < foo.gtf > sorted-foo.gtf.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i gtf < foo.gtf > sorted-foo.gtf.bed\n"           \
    "  $ convert2bed -i gtf -o starch < foo.gtf > sorted-foo.gtf.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the sort-bed application,\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ gtf2bed --do-not-sort < foo.gtf > unsorted-foo.gtf.bed\n";

static const char *gtf_description =                                    \
    "  The GTF2.2 specification (http://mblab.wustl.edu/GTF22.html)\n"  \
    "  contains columns that do not map directly to common or UCSC BED columns.\n" \
    "  Therefore, we add the following columns to preserve the ability to\n" \
    "  seamlessly convert back to GTF after performing operations with\n" \
    "  BEDOPS or other tools.\n"                                        \
    "\n"                                                                \
    "  - The 'source' GTF column data maps to the 7th BED column\n"     \
    "  - The 'feature' data maps to the 8th BED column\n"               \
    "  - The 'frame' data maps to the 9th BED column\n"                 \
    "  - The 'attributes' data maps to the 10th BED column\n"           \
    "  - The 'comments' data (if present) maps to the 11th BED column\n" \
    "\n"                                                                \
    "  We make the following assumptions about the GTF input data:\n"   \
    "\n"                                                                \
    "  - The 'seqname' GTF column data maps to the chromosome label (1st BED column)\n" \
    "  - The 'gene_id' attribute in the 'attributes' GTF column (if present) maps to \n" \
    "    the element ID (4th BED column)\n"                             \
    "  - The 'score' and 'strand' GFF columns (if present) are mapped to the\n" \
    "    5th and 6th BED columns, respectively\n"                       \
    "\n"                                                                \
    "  If we encounter zero-length insertion elements (which are defined\n" \
    "  where the start and stop GTF column data values are equivalent), the \n" \
    "  start coordinate is decremented to convert to 0-based, half-open indexing,\n" \
    "  and a 'zero_length_insertion' attribute is added to the 'attributes' GTF\n" \
    "  column data.\n";

static const char *gtf_options = NULL;

static const char *psl_name = "convert2bed -i psl";

static const char *psl_usage =                                          \
    "  Converts 0-based, half-open [a-1, b) headered or headerless PSL\n" \
    "  input into 0-based, half-open [a-1, b) extended BED or BEDOPS Starch\n" \
    "\n"                                                                \
    "  $ psl2bed < foo.psl > sorted-foo.psl.bed\n"                      \
    "  $ psl2starch < foo.psl > sorted-foo.psl.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i psl < foo.psl > sorted-foo.psl.bed\n"           \
    "  $ convert2bed -i psl -o starch < foo.psl > sorted-foo.psl.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the BEDOPS sort-bed application\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ psl2bed --do-not-sort < foo.psl > unsorted-foo.psl.bed\n";

static const char *psl_description =                                    \
    "  The PSL specification (http://genome.ucsc.edu/goldenPath/help/blatSpec.html)\n" \
    "  contains 21 columns, some which map to UCSC BED columns and some which do not.\n" \
    "\n"                                                                \
    "  PSL input can contain a header or be headerless, if the BLAT search was\n" \
    "  performed with the -noHead option. This program can accept input in\n" \
    "  either format.\n"                                                \
    "\n"                                                                \
    "  If input is headered, you can use the --keep-header option to preserve the header\n" \
    "  data as pseudo-BED elements that use the \"_header\" chromosome name. We expect this\n" \
    "  should not cause any collision problems since PSL data should use UCSC chromosome\n" \
    "  naming conventions.\n"                                           \
    "\n"                                                                \
    "  We describe below how we map columns to BED, so that BLAT results can be losslessly\n" \
    "  transformed back into PSL format with a simple awk statement or other similar\n" \
    "  command that permutes columns into PSL-ordering.\n"              \
    "\n"                                                                \
    "  We map the following PSL columns to their equivalent BED column, as follows:\n" \
    "\n"                                                                \
    "  - tName    <-->   chromosome\n"                                  \
    "  - tStart   <-->   start\n"                                       \
    "  - tEnd     <-->   stop\n"                                        \
    "  - qName    <-->   id\n"                                          \
    "  - qSize    <-->   score\n"                                       \
    "  - strand   <-->   strand\n"                                      \
    "\n"                                                                \
    "  Remaining PSL columns are mapped, in order, to columns 7 through 21 in the\n" \
    "  BED output:\n"                                                   \
    "\n"                                                                \
    "  - matches\n"                                                     \
    "  - misMatches\n"                                                  \
    "  - repMatches\n"                                                  \
    "  - nCount\n"                                                      \
    "  - qNumInsert\n"                                                  \
    "  - qBaseInsert\n"                                                 \
    "  - tNumInsert\n"                                                  \
    "  - tBaseInsert\n"                                                 \
    "  - qStart\n"                                                      \
    "  - qEnd\n"                                                        \
    "  - tSize\n"                                                       \
    "  - blockCount\n"                                                  \
    "  - blockSizes\n"                                                  \
    "  - qStarts\n"                                                     \
    "  - tStarts\n";

static const char *psl_options =                                        \
    "  PSL conversion options:\n\n"                                     \
    "  --keep-header (-k)\n"                                            \
    "      Preserve header section as pseudo-BED elements (requires --headered)\n" \
    "  --split (-s)\n"                                                  \
    "      Split record into multiple BED elements, based on tStarts field value\n";

static const char *sam_name = "convert2bed -i sam";

static const char *sam_usage =                                          \
    "  Converts 1-based, closed [a, b] headered or headerless SAM input\n" \
    "  into 0-based, half-open [a-1, b) extended BED or BEDOPS Starch\n" \
    "\n"                                                                \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ sam2bed < foo.sam > sorted-foo.sam.bed\n"                      \
    "  $ sam2starch < foo.sam > sorted-foo.sam.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i sam < foo.sam > sorted-foo.sam.bed\n"           \
    "  $ convert2bed -i sam -o starch < foo.sam > sorted-foo.sam.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ sam2bed --do-not-sort < foo.sam > unsorted-foo.sam.bed\n";

static const char *sam_description =                                    \
    "  The SAM (Sequence Alignment/Map) format is 1-based and closed [a, b]\n" \
    "  which is converted to 0-based, half-closed [a-1, b) when creating\n" \
    "  BED output.\n"                                                   \
    "\n"                                                                \
    "  We process SAM columns from mappable reads (as described by \n"  \
    "  http://samtools.github.io/hts-specs/SAMv1.pdf) converting them into \n" \
    "  the first six UCSC BED columns, as follows:\n"                   \
    "\n"                                                                \
    "  - RNAME                     <-->   chromosome (1st column)\n"    \
    "  - POS - 1                   <-->   start (2nd column)\n"         \
    "  - POS + length(CIGAR) - 1   <-->   stop (3rd column)\n"          \
    "  - QNAME                     <-->   id (4th column)\n"            \
    "  - FLAG                      <-->   score (5th column)\n"         \
    "  - 16 & FLAG                 <-->   strand (6th column)\n"        \
    "\n"                                                                \
    "  The remaining SAM columns are mapped intact, in order, to adjacent\n" \
    "  BED columns:\n"                                                  \
    "\n"                                                                \
    "  - MAPQ\n"                                                        \
    "  - CIGAR\n"                                                       \
    "  - RNEXT\n"                                                       \
    "  - PNEXT\n"                                                       \
    "  - TLEN\n"                                                        \
    "  - SEQ\n"                                                         \
    "  - QUAL\n"                                                        \
    "\n"                                                                \
    "  Because we have mapped all columns, we can translate converted BED data back\n" \
    "  to headerless SAM reads with a simple awk statement or other script that\n" \
    "  calculates 1-based coordinates and permutes columns.\n"          \
    "\n"                                                                \
    "  By default, we only process mapped reads. If you also want to convert unmapped\n" \
    "  reads, add the --all-reads option.\n"                            \
    "\n"                                                                \
    "  In the case of RNA-seq data with skipped regions ('N' components in the\n" \
    "  read's CIGAR string), the --split option will split the read into two or more\n" \
    "  separate BED elements.\n"                                        \
    "\n"                                                                \
    "  The header section is normally stripped from the output. You can use the\n" \
    "  --keep-header option to preserve the header data from the SAM input as\n" \
    "  pseudo-BED elements.\n";

static const char *sam_options =                                        \
    "  SAM conversion options:\n\n"                                     \
    "  --all-reads (-a)\n"                                              \
    "      Include both unmapped and mapped reads in output\n"          \
    "  --keep-header (-k)\n"                                            \
    "      Preserve header section as pseudo-BED elements\n"            \
    "  --split (-s)\n"                                                  \
    "      Split reads with 'N' CIGAR operations into separate BED\n"   \
    "      elements\n";

static const char *vcf_name = "convert2bed -i vcf";

static const char *vcf_usage =                                          \
    "  Converts 1-based, closed [a, b] VCF v4 input into 0-based,\n"    \
    "  half-open [a-1, b) extended BED or BEDOPS Starch\n"              \
    "\n"                                                                \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ vcf2bed < foo.vcf > sorted-foo.vcf.bed\n"                      \
    "  $ vcf2starch < foo.vcf > sorted-foo.vcf.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i vcf < foo.vcf > sorted-foo.vcf.bed\n"           \
    "  $ convert2bed -i vcf -o starch < foo.vcf > sorted-foo.vcf.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ vcf2bed --do-not-sort < foo.vcf > unsorted-foo.vcf.bed\n";

static const char *vcf_description =                                    \
    "  This conversion utility relies on the VCF v4.2 format, with its\n" \
    "  specifications outlined here by the 1000 Genomes and Samtools projects:\n" \
    "\n"                                                                \
    "  http://samtools.github.io/hts-specs/VCFv4.2.pdf\n"               \
    "\n"                                                                \
    "  -- The \"meta-information\" (starting with '##') and \"header\"\n" \
    "     lines (starting with '#') are discarded, unless the --keep-header\n" \
    "     option is used.\n\n"                                          \
    "     To preserve metadata and header as BED elements, use the \n"  \
    "     --keep-header option, which munges these data into pseudo-elements\n" \
    "     that generally sort to the top (when chromosomes follow UCSC naming\n" \
    "     conventions) by using the \"_header\" chromosome name.\n"     \
    "\n"                                                                \
    "  -- The header line must be tab-delimited. The eight, fixed mandatory\n" \
    "     columns are converted to BED data as follows:\n"              \
    "\n"                                                                \
    "     - Data in the #CHROM column are mapped to the first column of\n" \
    "       the BED output\n"                                           \
    "     - The POS column is mapped to the second and third BED columns\n" \
    "     - The ID and QUAL columns are mapped to the fourth and fifth BED\n" \
    "       columns, respectively\n"                                    \
    "     - The REF, ALT, FILTER and INFO are mapped to the sixth through\n" \
    "       ninth BED columns, respectively\n"                          \
    "\n"                                                                \
    "  -- If present, genotype data in FORMAT and subsequence sample IDs\n" \
    "     are placed into tenth and subsequent columns.\n"              \
    "\n"                                                                \
    "  -- Data rows must also be tab-delimited.\n"                      \
    "\n"                                                                \
    "  -- Any missing data or non-standard delimiters may cause\n"      \
    "     problems. It may be useful to validate the VCF v4.2 input\n"  \
    "     before conversion.\n";

static const char *vcf_options =                                        \
    "  VCF conversion options:\n\n"                                     \
    "  --do-not-split (-p)\n"                                           \
    "      By default, this application prints multiple BED elements for each alternate\n" \
    "      allele. Use this flag to print one BED element for all alternate alleles\n" \
    "  --snvs (-v)\n"                                                   \
    "      Report only single nucleotide variants\n"                    \
    "  --insertions (-t)\n"                                             \
    "      Report only insertion variants\n"                            \
    "  --deletions (-n)\n"                                              \
    "      Report only deletion variants\n"                             \
    "  --keep-header (-k)\n"                                            \
    "      Preserve header section as pseudo-BED elements\n";

static const char *wig_name = "convert2bed -i wig";

static const char *wig_usage =                                          \
    "  Convert UCSC Wiggle to extended BED or BEDOPS Starch\n"          \
    "\n"                                                                \
    "  Usage:\n"                                                        \
    "\n"                                                                \
    "  $ wig2bed < foo.wig > sorted-foo.wig.bed\n"                      \
    "  $ wig2starch < foo.wig > sorted-foo.wig.starch\n"                \
    "\n"                                                                \
    "  Or:\n"                                                           \
    "\n"                                                                \
    "  $ convert2bed -i wig < foo.wig > sorted-foo.wig.bed\n"           \
    "  $ convert2bed -i wig -o starch < foo.wig > sorted-foo.wig.starch\n" \
    "\n"                                                                \
    "  We make no assumptions about sort order from converted output. Apply\n" \
    "  the usage case displayed to pass data to the BEDOPS sort-bed application,\n" \
    "  which generates lexicographically-sorted BED data as output.\n"  \
    "\n"                                                                \
    "  If you want to skip sorting, use the --do-not-sort option:\n"    \
    "\n"                                                                \
    "  $ wig2bed --do-not-sort < foo.wig > unsorted-foo.wig.bed\n";

static const char *wig_description =                                    \
    "  The UCSC Wiggle format (http://genome.ucsc.edu/goldenPath/help/wiggle.html)\n" \
    "  is 1-based, closed [a, b] and is offered in variable or fixed step varieties.\n" \
    "  We convert either variety to 0-based, half-open [a-1, b) indexing when creating\n" \
    "  BED output.\n"                                                   \
    "\n"                                                                \
    "  By default, data are passed internally to BEDOPS sort-bed to provide sorted\n" \
    "  output ready for use with other BEDOPS utilities.\n";

static const char *wig_options =                                        \
    "  WIG conversion options:\n\n"                                     \
    "  --multisplit=<basename> (-b <basename>)\n"                       \
    "      A single input file may have multiple WIG sections. With this option\n" \
    "      every section gets an ID prefix starting with <basename>.1, then\n" \
    "      <basename>.2, and so on\n";

static const char *format_undefined_usage =                             \
    "  Note: Please specify format to get detailed usage parameters:\n\n" \
    "  --help[-bam|-gff|-gtf|-psl|-sam|-vcf|-wig] (-h <fmt>)\n";

typedef struct gff_state {
    char *id;
} c2b_gff_state_t;

typedef struct gtf_state {
    char *id;
} c2b_gtf_state_t;

typedef struct psl_state {
    boolean is_headered;
} c2b_psl_state_t;

typedef struct sam_state {
    char *samtools_path;
    c2b_cigar_t *cigar;
} c2b_sam_state_t;

typedef struct vcf_state {
    boolean do_not_split;
    boolean only_snvs;
    boolean only_insertions;
    boolean only_deletions;
    unsigned int filter_count;
} c2b_vcf_state_t;

typedef struct wig_state {
    uint32_t section;
    uint32_t line;
    uint32_t pos_lines;
    uint64_t span;
    uint64_t step;
    uint64_t start_pos;
    uint64_t end_pos;
    double score;
    char *chr;
    char *id;
    boolean is_fixed_step;
    boolean start_write;
    char *basename;
} c2b_wig_state_t;

typedef struct cat_params {
    char *path;
} c2b_cat_params_t;

typedef struct sort_params {
    boolean is_enabled;
    char *sort_bed_path;
    char *max_mem_value;
    char *sort_tmpdir_path;
} c2b_sort_params_t;

typedef struct starch_params {
    char *path;
    boolean bzip2;
    boolean gzip;
    char *note;
} c2b_starch_params_t;

static struct globals {
    c2b_format_t help_format_idx;
    char *input_format;
    c2b_format_t input_format_idx;
    char *output_format;
    c2b_format_t output_format_idx;
    unsigned int header_line_idx;
    boolean all_reads_flag;
    boolean keep_header_flag;
    boolean split_flag;
    c2b_gff_state_t *gff;
    c2b_gtf_state_t *gtf;
    c2b_psl_state_t *psl;
    c2b_sam_state_t *sam;
    c2b_vcf_state_t *vcf;
    c2b_wig_state_t *wig;
    c2b_cat_params_t *cat;
    c2b_sort_params_t *sort;
    c2b_starch_params_t *starch;
} c2b_globals;

static struct option c2b_client_long_options[] = {
    { "input",          required_argument,   NULL,    'i' },
    { "output",         required_argument,   NULL,    'o' },
    { "do-not-sort",    no_argument,         NULL,    'd' },
    { "all-reads",      no_argument,         NULL,    'a' },
    { "keep-header",    no_argument,         NULL,    'k' },
    { "split",          no_argument,         NULL,    's' },
    { "do-not-split",   no_argument,         NULL,    'p' },
    { "snvs",           no_argument,         NULL,    'v' },
    { "insertions",     no_argument,         NULL,    't' },
    { "deletions",      no_argument,         NULL,    'n' },
    { "starch-bzip2",   no_argument,         NULL,    'z' },
    { "starch-gzip",    no_argument,         NULL,    'g' },
    { "starch-note",    required_argument,   NULL,    'e' },
    { "max-mem",        required_argument,   NULL,    'm' },
    { "sort-tmpdir",    required_argument,   NULL,    'r' },
    { "multisplit",     required_argument,   NULL,    'b' },
    { "help",           no_argument,         NULL,    'h' },
    { "help-bam",       no_argument,         NULL,    '1' },
    { "help-gff",       no_argument,         NULL,    '2' },
    { "help-gtf",       no_argument,         NULL,    '3' },
    { "help-psl",       no_argument,         NULL,    '4' },
    { "help-sam",       no_argument,         NULL,    '5' },
    { "help-vcf",       no_argument,         NULL,    '6' },
    { "help-wig",       no_argument,         NULL,    '7' },
    { NULL,             no_argument,         NULL,     0  }
};

static const char *c2b_client_opt_string = "i:o:dakspvtnzge:m:r:b:h1234567?";

#ifdef __cplusplus
extern "C" {
#endif

    static void              c2b_init_conversion(c2b_pipeset_t *p);
    static void              c2b_init_gff_conversion(c2b_pipeset_t *p);
    static void              c2b_init_gtf_conversion(c2b_pipeset_t *p);
    static void              c2b_init_psl_conversion(c2b_pipeset_t *p);
    static void              c2b_init_sam_conversion(c2b_pipeset_t *p);
    static void              c2b_init_vcf_conversion(c2b_pipeset_t *p);
    static void              c2b_init_wig_conversion(c2b_pipeset_t *p);
    static void              c2b_init_generic_conversion(c2b_pipeset_t *p, void(*to_bed_line_functor)(char *, ssize_t *, char *, ssize_t));
    static void              c2b_init_bam_conversion(c2b_pipeset_t *p);
    static inline void       c2b_cmd_cat_stdin(char *cmd);
    static inline void       c2b_cmd_bam_to_sam(char *cmd);
    static inline void       c2b_cmd_sort_bed(char *cmd);
    static inline void       c2b_cmd_starch_bed(char *cmd);
    static void              c2b_line_convert_gff_to_bed_unsorted(char *dest, ssize_t *dest_size, char *src, ssize_t src_size);
    static inline void       c2b_line_convert_gff_to_bed(c2b_gff_t g, char *dest_line, ssize_t *dest_size);
    static void              c2b_line_convert_gtf_to_bed_unsorted(char *dest, ssize_t *dest_size, char *src, ssize_t src_size);
    static inline void       c2b_line_convert_gtf_to_bed(c2b_gtf_t g, char *dest_line, ssize_t *dest_size);
    static void              c2b_line_convert_psl_to_bed_unsorted(char *dest, ssize_t *dest_size, char *src, ssize_t src_size);
    static inline void       c2b_line_convert_psl_to_bed(c2b_psl_t p, char *dest_line, ssize_t *dest_size);
    static void              c2b_line_convert_sam_to_bed_unsorted_without_split_operation(char *dest, ssize_t *dest_size, char *src, ssize_t src_size);
    static void              c2b_line_convert_sam_to_bed_unsorted_with_split_operation(char *dest, ssize_t *dest_size, char *src, ssize_t src_size); 
    static inline void       c2b_sam_cigar_str_to_ops(char *s);
    static void              c2b_sam_init_cigar_ops(c2b_cigar_t **c, const ssize_t size);
    static void              c2b_sam_debug_cigar_ops(c2b_cigar_t *c);
    static void              c2b_sam_delete_cigar_ops(c2b_cigar_t *c);
    static inline void       c2b_line_convert_sam_to_bed(c2b_sam_t s, char *dest_line, ssize_t *dest_size);
    static void              c2b_line_convert_vcf_to_bed_unsorted(char *dest, ssize_t *dest_size, char *src, ssize_t src_size);
    static inline boolean    c2b_vcf_allele_is_id(char *s);
    static inline boolean    c2b_vcf_record_is_snv(char *ref, char *alt);
    static inline boolean    c2b_vcf_record_is_insertion(char *ref, char *alt);
    static inline boolean    c2b_vcf_record_is_deletion(char *ref, char *alt);
    static inline void       c2b_line_convert_vcf_to_bed(c2b_vcf_t v, char *dest_line, ssize_t *dest_size);
    static void              c2b_line_convert_wig_to_bed_unsorted(char *dest, ssize_t *dest_size, char *src, ssize_t src_size);
    static void *            c2b_read_bytes_from_stdin(void *arg);
    static void *            c2b_process_intermediate_bytes_by_lines(void *arg);
    static void *            c2b_write_in_bytes_to_in_process(void *arg);
    static void *            c2b_write_out_bytes_to_in_process(void *arg);
    static void *            c2b_write_in_bytes_to_stdout(void *arg);
    static void *            c2b_write_out_bytes_to_stdout(void *arg);
    static void              c2b_memrchr_offset(ssize_t *offset, char *buf, ssize_t buf_size, ssize_t len, char delim);
    static void              c2b_init_pipeset(c2b_pipeset_t *p, const size_t num);
    static void              c2b_debug_pipeset(c2b_pipeset_t *p, const size_t num);
    static void              c2b_delete_pipeset(c2b_pipeset_t *p);
    static void              c2b_set_close_exec_flag(int fd);
    static void              c2b_unset_close_exec_flag(int fd);
    static int               c2b_pipe4(int fd[2], int flags);
    static pid_t             c2b_popen4(const char* cmd, int pin[2], int pout[2], int perr[2], int flags);
    static void              c2b_test_dependencies();
    static boolean           c2b_print_matches(char *path, char *fn);
    static char *            c2b_strsep(char **stringp, const char *delim);
    static boolean           c2b_is_there(char *candidate);
    static void              c2b_init_globals();
    static void              c2b_delete_globals();
    static void              c2b_init_global_gff_state();
    static void              c2b_delete_global_gff_state();
    static void              c2b_init_global_gtf_state();
    static void              c2b_delete_global_gtf_state();
    static void              c2b_init_global_psl_state();
    static void              c2b_delete_global_psl_state();
    static void              c2b_init_global_sam_state();
    static void              c2b_delete_global_sam_state();
    static void              c2b_init_global_vcf_state();
    static void              c2b_delete_global_vcf_state();
    static void              c2b_init_global_wig_state();
    static void              c2b_delete_global_wig_state();
    static void              c2b_init_global_cat_params();
    static void              c2b_delete_global_cat_params();
    static void              c2b_init_global_sort_params();
    static void              c2b_delete_global_sort_params();
    static void              c2b_init_global_starch_params();
    static void              c2b_delete_global_starch_params();
    static void              c2b_init_command_line_options(int argc, char **argv);
    static void              c2b_print_usage(FILE *stream);
    static void              c2b_print_format_usage(FILE *stream);
    static char *            c2b_to_lowercase(const char *src);
    static c2b_format_t      c2b_to_input_format(const char *input_format);
    static c2b_format_t      c2b_to_output_format(const char *output_format);

#ifdef __cplusplus
}
#endif

#endif
