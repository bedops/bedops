.. _rmsk2bed:

`rmsk2bed`
==========

The ``rmsk2bed`` script converts 1-based, closed ``[start, end]`` `RepeatMasker annotation output <http://www.repeatmasker.org/webrepeatmaskerhelp.html>`_ (OUT) to sorted, 0-based, half-open ``[start-1, end)`` extended BED-formatted data.

For convenience, we also offer ``rmsk2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

============
Dependencies
============

The ``rmsk2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``rmsk2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

This script is also dependent on input that follows the RepeatMasker annotation output specification, outlined here: `http://www.repeatmasker.org/webrepeatmaskerhelp.html <http://www.repeatmasker.org/webrepeatmaskerhelp.html>`_.

======
Source
======

The ``rmsk2bed`` and ``rmsk2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``rmsk2bed`` script parses RepeatMasker annotation output from standard input and prints sorted BED to standard output. The ``rmsk2starch`` script uses an extra step to parse RepeatMasker annotation output to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a RepeatMasker annotation output file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If sorting converted data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample RepeatMasker annotation output input called ``foo.out`` (see the :ref:`Downloads <rmsk2bed_downloads>` section to grab this file). 

::

    SW  perc perc perc  query    position in query     matching repeat      position in  repeat
  score div. del. ins.  sequence begin  end  (left)    repeat  class/family   begin end (left) ID
  ...
   1320 15.6  6.2  0.0  HSU08988  6563 6781 (22462) C  MER7A   DNA/MER2_type    (0)  337  104  20
  12279 10.5  2.1  1.7  HSU08988  6782 7718 (21525) C  Tigger1 DNA/MER2_type    (0) 2418 1486  19
   1769 12.9  6.6  1.9  HSU08988  7719 8022 (21221) C  AluSx   SINE/Alu         (0)  317    1  17
  12279 10.5  2.1  1.7  HSU08988  8023 8694 (20549) C  Tigger1 DNA/MER2_type  (932) 1486  818  19
   2335 11.1  0.3  0.7  HSU08988  8695 9000 (20243) C  AluSg   SINE/Alu         (5)  305    1  18
  12279 10.5  2.1  1.7  HSU08988  9001 9695 (19548) C  Tigger1 DNA/MER2_type (1600)  818    2  19
    721 21.2  1.4  0.0  HSU08988  9696 9816 (19427) C  MER7A   DNA/MER2_type  (224)  122    2  20

We can convert it to sorted BED data in the following manner:

::

  $ rmsk2bed < foo.out
  HSU08988	6562	6781	MER7A	1320	-	15.6	6.2	0.0	(22462)	DNA/MER2_type	(0)	337	104	20
  HSU08988	6781	7718	Tigger1	12279	-	10.5	2.1	1.7	(21525)	DNA/MER2_type	(0)	2418	1486	19
  HSU08988	7718	8022	AluSx	1769	-	12.9	6.6	1.9	(21221)	SINE/Alu	(0)	317	1	17
  HSU08988	8022	8694	Tigger1	12279	-	10.5	2.1	1.7	(20549)	DNA/MER2_type	(932)	1486	818	19
  HSU08988	8694	9000	AluSg	2335	-	11.1	0.3	0.7	(20243)	SINE/Alu	(5)	305	1	18
  HSU08988	9000	9695	Tigger1	12279	-	10.5	2.1	1.7	(19548)	DNA/MER2_type	(1600)	818	2	19
  HSU08988	9695	9816	MER7A	721	-	21.2	1.4	0.0	(19427)	DNA/MER2_type	(224)	122	2	20

.. note:: Use :ref:`bedops --merge <bedops_merge>` to merge elements, *e.g.*: ``rmsk2bed < foo.out | bedops --merge - > merged_repeatmasker_elements.bed``

As shown above, we strip the header element, but adding the ``--keep-header`` option will preserve this header as a BED element that uses ``_header`` as a chromosome name:

::

  $ rmsk2bed --keep-header < foo.out
  HSU08988	6562	6781	MER7A	1320	-	15.6	6.2	0.0	(22462)	DNA/MER2_type	(0)	337	104	20
  HSU08988	6781	7718	Tigger1	12279	-	10.5	2.1	1.7	(21525)	DNA/MER2_type	(0)	2418	1486	19
  HSU08988	7718	8022	AluSx	1769	-	12.9	6.6	1.9	(21221)	SINE/Alu	(0)	317	1	17
  HSU08988	8022	8694	Tigger1	12279	-	10.5	2.1	1.7	(20549)	DNA/MER2_type	(932)	1486	818	19
  HSU08988	8694	9000	AluSg	2335	-	11.1	0.3	0.7	(20243)	SINE/Alu	(5)	305	1	18
  HSU08988	9000	9695	Tigger1	12279	-	10.5	2.1	1.7	(19548)	DNA/MER2_type	(1600)	818	2	19
  HSU08988	9695	9816	MER7A	721	-	21.2	1.4	0.0	(19427)	DNA/MER2_type	(224)	122	2	20
  _header	0	1	SW  perc perc perc  query    position in query     matching repeat      position in  repeat
  _header	1	2	score div. del. ins.  sequence begin  end  (left)    repeat  class/family   begin end (left) ID
  _header	2	3	...

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from RepeatMasker annotation output to BED. *BEDOPS supports operations on input with any coordinate indexing*, but the coordinate change made here is believed to be convenient for most end users.

.. _rmsk2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how RepeatMasker annotation columns are mapped to BED columns. We start with the first six UCSC BED columns as follows:

+-------------------------------+---------------------+---------------+
| RepeatMasker annotation field | BED column index    | BED field     |
+===============================+=====================+===============+
| Query sequence                | 1                   | chromosome    |
+-------------------------------+---------------------+---------------+
| Query start                   | 2                   | start         |
+-------------------------------+---------------------+---------------+
| Query end                     | 3                   | stop          |
+-------------------------------+---------------------+---------------+
| Repeat name                   | 4                   | id            |
+-------------------------------+---------------------+---------------+
| Smith-Waterman score          | 5                   | score         |
+-------------------------------+---------------------+---------------+
| Strand                        | 6                   | strand        |
+-------------------------------+---------------------+---------------+

The remaining columns are mapped as follows:

+-------------------------------+---------------------+---------------+
| RepeatMasker annotation field | BED column index    | BED field     |
+===============================+=====================+===============+
| Percentage, substitutions     | 7                   |               |
+-------------------------------+---------------------+---------------+
| Percentage, deleted bases     | 8                   |               |
+-------------------------------+---------------------+---------------+
| Percentage, inserted bases    | 9                   |               |
+-------------------------------+---------------------+---------------+
| Bases in query, past match    | 10                  |               |
+-------------------------------+---------------------+---------------+
| Repeat class                  | 11                  |               |
+-------------------------------+---------------------+---------------+
| Bases in complement of the    | 12                  |               |
| repeat consensus sequence     |                     |               |
+-------------------------------+---------------------+---------------+
| Match start                   | 13                  |               |
+-------------------------------+---------------------+---------------+
| Match end                     | 14                  |               |
+-------------------------------+---------------------+---------------+
| Unique ID                     | 15                  |               |
+-------------------------------+---------------------+---------------+
| Higher-scoring match          | 16                  |               |
| (optional)                    |                     |               |
+-------------------------------+---------------------+---------------+

.. _rmsk2bed_downloads:

=========
Downloads
=========

* Sample RepeatMasker annotation dataset: :download:`foo.out <../../../../assets/reference/file-management/conversion/reference_rmsk2bed_foo.out>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
