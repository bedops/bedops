.. _psl2bed:

`psl2bed`
=========

The ``psl2bed`` script converts 0-based, half-open ``[start-1, end)`` `Pattern Space Layout <http://genome.ucsc.edu/FAQ/FAQformat.html#format2>`_ (PSL) to sorted, 0-based, half-open ``[start-1, end)`` extended BED-formatted data.

For convenience, we also offer ``psl2starch``, which performs the extra step of creating a Starch-formatted archive.

============
Dependencies
============

The ``psl2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``psl2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

This script is also dependent on input that follows the `PSL specification <http://genome.ucsc.edu/FAQ/FAQformat.html#format2>`_. 

.. tip:: Conversion of data which are PSL-like, but which do not follow the specification can cause parsing issues. If you run into problems, please check that your input follows the PSL specification.

======
Source
======

The ``psl2bed`` and ``psl2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``psl2bed`` script parses PSL from standard input and prints sorted BED to standard output. The ``psl2starch`` script uses an extra step to parse GFF to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a headered PSL file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

If your data contains a record with multiple blocks (``block count`` is greater than one, and the ``tStarts`` field has multiple target start positions), you can use the ``--split`` option to print that record to separate BED elements, each with a start position defined by ``tStarts`` and a length defined by the associated value in the ``blockSizes`` string.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If you are sorting data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample PSL input called ``foo.psl`` (see the :ref:`Downloads <psl2bed_downloads>` section to grab this file). 

::

  psLayout version 3

  match   mis-    rep.    N's     Q gap   Q gap   T gap   T gap   strand  Q               Q       Q       Q       T               T       T       T       block   blockSizes      qStarts  tStarts
          match   match           count   bases   count   bases           name            size    start   end     name            size    start   end     count
  ---------------------------------------------------------------------------------------------------------------------------------------------------------------
  35      0       0       0       0       0       0       0       +       foo     50      15      50      chrX    155270560       40535836        40535871        1       35,     15,     40535836,
  34      2       0       0       0       0       0       0       +       foo     50      14      50      chrX    155270560       68019028        68019064        1       36,     14,     68019028,
  33      2       0       0       0       0       0       0       +       foo     50      14      49      chrX    155270560       43068135        43068170        1       35,     14,     43068135,
  35      2       0       0       0       0       0       0       +       foo     50      13      50      chr8    146364022       131572122       131572159       1       37,     13,     131572122,
  30      0       0       0       0       0       0       0       +       foo     50      14      44      chr6    171115067       127685756       127685786       1       30,     14,     127685756,
  30      0       0       0       0       0       0       0       +       foo     50      14      44      chr6    171115067       93161871        93161901        1       30,     14,     93161871,
  31      0       0       0       0       0       0       0       +       foo     50      13      44      chr5    180915260       119897315       119897346       1       31,     13,     119897315,
  30      0       0       0       0       0       0       0       +       foo     50      14      44      chr5    180915260       1232.4.39       1232.4.395       1       30,     14,     1232.4.39,
  ...

We can convert it to sorted BED data in the following manner:

::

  $ psl2bed < foo.psl
  chr1    30571100        30571135        foo     50      -       35      0       0       0       0       0       0       0       15      50      249250621       1       35,     0,      30571100,
  chr1    69592160        69592195        foo     50      -       34      1       0       0       0       0       0       0       15      50      249250621       1       35,     0,      69592160,
  chr1    107200050       107200100       foo     50      +       50      0       0       0       0       0       0       0       0       50      249250621       1       50,     0,      107200050,
  chr11   12618347        12618389        foo     50      +       39      3       0       0       0       0       0       0       8       50      135006516       1       42,     8,      12618347,
  chr11   32933028        32933063        foo     50      +       35      0       0       0       1       1       0       0       8       44      135006516       2       4,31,   8,13,   32933028,32933032,
  chr11   80116421        80116457        foo     50      +       35      1       0       0       0       0       0       0       14      50      135006516       1       36,     14,     80116421,
  chr11   133952291       133952327       foo     50      +       34      2       0       0       0       0       0       0       14      50      135006516       1       36,     14,     133952291,
  chr13   99729482        99729523        foo     50      +       39      2       0       0       0       0       0       0       8       49      115169878       1       41,     8,      99729482,
  chr13   111391852       111391888       foo     50      +       34      2       0       0       0       0       0       0       14      50      115169878       1       36,     14,     111391852,
  chr16   8149657 8149694 foo     50      +       36      1       0       0       0       0       0       0       13      50      90354753        1       37,     13,     8149657,
  ...

As you see here, the header data of a headered PSL file is discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

Here is a demonstration of conversion of the same headered input, adding the ``--keep-header`` option:

::

  $ psl2bed --keep-header < foo.psl
  _header 0       1       psLayout version 3
  _header 1       2
  _header 2       3       match   mis-    rep.    N's     Q gap   Q gap   T gap   T gap   strand  Q               Q       Q       Q       T               T       T       T       block   blockSizes      qStarts  tStarts
  _header 3       4       match   match           count   bases   count   bases           name            size    start   end     name            size    start   end     count
  _header 4       5       ---------------------------------------------------------------------------------------------------------------------------------------------------------------
  chr1    30571100        30571135        foo     50      -       35      0       0       0       0       0       0       0       15      50      249250621       1       35,     0,      30571100,
  chr1    69592160        69592195        foo     50      -       34      1       0       0       0       0       0       0       15      50      249250621       1       35,     0,      69592160,
  chr1    107200050       107200100       foo     50      +       50      0       0       0       0       0       0       0       0       50      249250621       1       50,     0,      107200050,
  chr11   12618347        12618389        foo     50      +       39      3       0       0       0       0       0       0       8       50      135006516       1       42,     8,      12618347,
  chr11   32933028        32933063        foo     50      +       35      0       0       0       1       1       0       0       8       44      135006516       2       4,31,   8,13,   32933028,32933032,
  chr11   80116421        80116457        foo     50      +       35      1       0       0       0       0       0       0       14      50      135006516       1       36,     14,     80116421,
  chr11   133952291       133952327       foo     50      +       34      2       0       0       0       0       0       0       14      50      135006516       1       36,     14,     133952291,
  chr13   99729482        99729523        foo     50      +       39      2       0       0       0       0       0       0       8       49      115169878       1       41,     8,      99729482,
  chr13   111391852       111391888       foo     50      +       34      2       0       0       0       0       0       0       14      50      115169878       1       36,     14,     111391852,
  chr16   8149657 8149694 foo     50      +       36      1       0       0       0       0       0       0       13      50      90354753        1       37,     13,     8149657,
  ...

With this option, the ``psl2bed`` and ``psl2starch`` scripts are completely "non-lossy". Use of ``awk`` or other scripting tools can munge these data back into a PSL-formatted file.

This example PSL file contains one record with a block count of 2. If we were to add the ``--split`` option, this record would be split into two separate BED elements that have start positions ``32933028`` and ``32933032``, with lengths ``4`` and ``31``, respectively. These elements fall within the genomic range already defined by the ``tStart`` and ``tEnd`` fields (``32933028`` and ``32933063``).

.. note:: The ``psl2bed`` and ``psl2starch`` scripts work with headered or headerless PSL data. 

.. note:: By default, the ``psl2bed`` and ``psl2starch`` scripts assume that PSL data do *not* need splitting. If you expect your data to contain multiple blocks, add the ``--split`` option.

.. _psl2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how PSL columns are mapped to BED columns. We start with the first six UCSC BED columns as follows:

+---------------------------+---------------------+---------------+
| PSL field                 | BED column index    | BED field     |
+===========================+=====================+===============+
| tName                     | 1                   | chromosome    |
+---------------------------+---------------------+---------------+
| tStart(*)                 | 2                   | start         |
+---------------------------+---------------------+---------------+
| tEnd(*)                   | 3                   | stop          |
+---------------------------+---------------------+---------------+
| qName                     | 4                   | id            |
+---------------------------+---------------------+---------------+
| matches                   | 5                   | score         |
+---------------------------+---------------------+---------------+
| strand                    | 6                   | strand        |
+---------------------------+---------------------+---------------+

The remaining PSL columns are mapped, in order, to the remaining columns of the BED output:

+---------------------------+---------------------+---------------+
| PSL field                 | BED column index    | BED field     |
+===========================+=====================+===============+
| qSize                     | 7                   |               |
+---------------------------+---------------------+---------------+
| misMatches                | 8                   |               |
+---------------------------+---------------------+---------------+
| repMatches                | 9                   |               |
+---------------------------+---------------------+---------------+
| nCount                    | 10                  |               |
+---------------------------+---------------------+---------------+
| qNumInsert                | 11                  |               |
+---------------------------+---------------------+---------------+
| qBaseInsert               | 12                  |               |
+---------------------------+---------------------+---------------+
| tNumInsert                | 13                  |               |
+---------------------------+---------------------+---------------+
| tBaseInsert               | 14                  |               |
+---------------------------+---------------------+---------------+
| qStart                    | 15                  |               |
+---------------------------+---------------------+---------------+
| qEnd                      | 16                  |               |
+---------------------------+---------------------+---------------+
| tSize                     | 17                  |               |
+---------------------------+---------------------+---------------+
| blockCount                | 18                  |               |
+---------------------------+---------------------+---------------+
| blockSizes                | 19                  |               |
+---------------------------+---------------------+---------------+
| qStarts                   | 20                  |               |
+---------------------------+---------------------+---------------+
| tStarts                   | 21                  |               |
+---------------------------+---------------------+---------------+

This is a lossless mapping. Because we have mapped all columns, we can translate converted BED data back to headerless PSL with a simple ``awk`` statement that permutes columns to PSL-based ordering:

::

  $ awk 'BEGIN { OFS = "\t" } \
    { \
      print $5" "$8" "$9" "$10" "$11" "$12" "$13" "$14" "$6" "$4" "$7" "$15" "$16" "$1" "$17" "$2" "$3" "$18" "$19" "$20" "$21 }' converted.bed > original.psl

In the case where the ``--split`` option is added, the ``tStart`` and ``tEnd`` fields are replaced with each of the values in the larger ``tStarts`` string, added to the respective values in the larger ``blockSizes`` string. This is still a lossless conversion, but modifications to the ``awk`` script printed above would be required to rebuild the original PSL.

.. _psl2bed_downloads:

=========
Downloads
=========

* Sample PSL dataset: :download:`foo.psl <../../../../assets/reference/file-management/conversion/reference_psl2bed_foo.psl>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
