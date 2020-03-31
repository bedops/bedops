.. _bam2bed:

`bam2bed`
=========

The ``bam2bed`` script converts 0-based, half-open ``[start-1, end)`` `Binary (Sequence) Alignment/Map <http://samtools.sourceforge.net/SAM1.pdf>`_ (BAM) to sorted, 0-based, half-open ``[start-1, end)`` UCSC BED data.

For convenience, we also offer ``bam2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

The ``bam2bed`` script is "non-lossy" (with the use of specific options, described below). Other toolkits tend to throw out information from the original BAM input upon conversion; ``bam2bed`` can retain everything, facilitating reuse of converted data and conversion to other formats.

.. tip:: Doing the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive can save a lot of space relative to the original BAM format, up to 33% of the original BAM dataset, while offering per-chromosome random access.

============
Dependencies
============

The ``bam2bed`` wrapper script is dependent upon the installation of `SAMtools <http://samtools.sourceforge.net/>`_ and :ref:`convert2bed <convert2bed>`. The ``bam2starch`` wrapper script is further dependent on the installation of the :ref:`starch <starch>` binary, part of a typical BEDOPS installation.

======
Source
======

The ``bam2bed`` and ``bam2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``bam2bed`` script parses BAM data from standard input and prints :ref:`sorted <sort-bed>` BED to standard output. The ``bam2starch`` script uses an extra step to parse BAM to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a BAM file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

.. tip:: If you work with RNA-seq data, you may use the ``--split`` option to process reads with ``N``-CIGAR operations, splitting them into separate BED elements.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If sorting converted data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample binary input called ``foo.bam`` (see the :ref:`Downloads <bam2bed_downloads>` section to grab this file). 

We can convert it to sorted BED data in the following manner (omitting standard error messages):

::

  $ bam2bed < foo.bam
  seq1    0       36      B7_591:4:96:693:509     99      +       73       36M     *       0       0       CACTAGTGGCTCATTGTAAATGTGTGGTTTAACTCG    <<<<<<<<<<<<<<<;<<<<<<<<<5<<<<<;:<;7    MF:i:18 Aq:i:73 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    2       37      EAS54_65:7:152:368:113  99      +       73       35M     *       0       0       CTAGTGGCTCATTGTAAATGTGTGGTTTAACTCGT     <<<<<<<<<<0<<<<655<<7<<<:9<<3/:<6):     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    4       39      EAS51_64:8:5:734:57     99      +       137      35M     *       0       0       AGTGGCTCATTGTAAATGTGTGGTTTAACTCGTCC     <<<<<<<<<<<7;71<<;<;;<7;<<3;);3*8/5     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    5       41      B7_591:1:289:587:906    63      +       137      36M     *       0       0       GTGGCTCATTGTAATTTTTTGTTTTAACTCTTCTCT    (-&----,----)-)-),'--)---',+-,),''*,    MF:i:130        Aq:i:63 NM:i:5  UQ:i:38 H0:i:0  H1:i:0
  ...

Note that we strip the header section from the output. If we want to keep this, the use of the ``--keep-header`` option will preserve the BAM file's header, turning it into BED elements that use ``_header`` as a chromosome name. 

Here's an example:

::

  $ bam2bed --keep-header < foo.bam
  _header 0       1       @HD     VN:1.0 SO:coordinate
  _header 1       2       @SQ     SN:seq1 LN:5000
  _header 2       3       @SQ     SN:seq2 LN:5000
  _header 3       4       @CO     Example of SAM/BAM file format.
  seq1    0       36      B7_591:4:96:693:509     99      +       73       36M     *       0       0       CACTAGTGGCTCATTGTAAATGTGTGGTTTAACTCG    <<<<<<<<<<<<<<<;<<<<<<<<<5<<<<<;:<;7    MF:i:18 Aq:i:73 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    2       37      EAS54_65:7:152:368:113  99      +       73       35M     *       0       0       CTAGTGGCTCATTGTAAATGTGTGGTTTAACTCGT     <<<<<<<<<<0<<<<655<<7<<<:9<<3/:<6):     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    4       39      EAS51_64:8:5:734:57     99      +       137      35M     *       0       0       AGTGGCTCATTGTAAATGTGTGGTTTAACTCGTCC     <<<<<<<<<<<7;71<<;<;;<7;<<3;);3*8/5     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    5       41      B7_591:1:289:587:906    63      +       137      36M     *       0       0       GTGGCTCATTGTAATTTTTTGTTTTAACTCTTCTCT    (-&----,----)-)-),'--)---',+-,),''*,    MF:i:130        Aq:i:63 NM:i:5  UQ:i:38 H0:i:0  H1:i:0
  ...

With this option, the ``bam2bed`` and ``bam2starch`` scripts are completely "non-lossy" (with the exception of unmapped reads; see note below). Use of ``awk`` or other scripting tools can munge these data back into a SAM-formatted file.

.. note:: The provided scripts **strip out unmapped reads** from the BAM file. We believe this makes sense under most circumstances. Add the ``--all-reads`` option if you need unmapped and mapped reads.

.. _bam2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how non-header BAM data (converted to SAM columns) are mapped to BED columns. We start with the first six UCSC BED columns as follows:

+---------------------------+---------------------+---------------+
| SAM field                 | BED column index    | BED field     |
+===========================+=====================+===============+
| RNAME                     | 1                   | chromosome    |
+---------------------------+---------------------+---------------+
| POS - 1                   | 2                   | start         |
+---------------------------+---------------------+---------------+
| POS + length(SEQ) - 1     | 3                   | stop          |
+---------------------------+---------------------+---------------+
| QNAME                     | 4                   | id            |
+---------------------------+---------------------+---------------+
| MAPQ                      | 5                   | score         |
+---------------------------+---------------------+---------------+
| 16 & FLAG                 | 6                   | strand        |
+---------------------------+---------------------+---------------+

The remaining SAM-converted columns are mapped as-is, in same order, to adjacent BED columns:

+---------------------------+---------------------+---------------+
| SAM field                 | BED column index    | BED field     |
+===========================+=====================+===============+
| FLAG                      | 7                   |               |
+---------------------------+---------------------+---------------+
| CIGAR                     | 8                   |               |
+---------------------------+---------------------+---------------+
| RNEXT                     | 9                   |               |
+---------------------------+---------------------+---------------+
| PNEXT                     | 10                  |               |
+---------------------------+---------------------+---------------+
| TLEN                      | 11                  |               |
+---------------------------+---------------------+---------------+
| SEQ                       | 12                  |               |
+---------------------------+---------------------+---------------+
| QUAL                      | 13                  |               |
+---------------------------+---------------------+---------------+

Because we have mapped all columns, we can translate converted BED data back to headered or headerless SAM reads with a simple ``awk`` statement (or other script) that reverts back to 1-based coordinates and permutes columns to SAM-based ordering.

.. _bam2bed_downloads:

=========
Downloads
=========

* Sample BAM dataset: :download:`foo.bam <../../../../assets/reference/file-management/conversion/reference_bam2bed_foo.bam>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
