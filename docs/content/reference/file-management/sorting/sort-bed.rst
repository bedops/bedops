.. _sort-bed:

`sort-bed`
==========

The ``sort-bed`` utility sorts BED files of any size, even larger than system memory. BED files that are in lexicographic-chromosome order allow BEDOPS utilities to work efficiently with data from any species without software modifications. Further, sorted files can be traversed very quickly.

Sorted BED order is defined first by lexicographic chromosome order, then ascending integer start coordinate order, and finally by ascending integer end coordinate order.

Other utilities in the BEDOPS suite require data in sorted order as described. You only need to sort once: BEDOPS utilities all read and write data in sorted order.

==================
Inputs and outputs
==================

-----
Input
-----

The :ref:`sort-bed` utility requires one or more three-column BED file(s). Support for common headers (such as `UCSC BED track headers <http://genome.ucsc.edu/FAQ/FAQformat.html#format1>`_) is included, although headers will be stripped from the output.

------
Output
------

The :ref:`sort-bed` utility sends lexicographically-sorted BED data to standard output, which can be redirected to a file or piped to other utilities, including core BEDOPS utilities like :ref:`bedops` and :ref:`bedmap`.

=====
Usage
=====

The ``--help`` option is fairly basic, but describes the usage:

::

  sort-bed
    citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
    version:  2.2.0
    authors:  Scott Kuehn

  USAGE: sort-bed [--help] [--version] [--max-mem <val>] <file1.bed> <file2.bed> <...>
          Sort BED file(s).
          May use '-' to indicate stdin.
          Results are sent to stdout.

          <val> for --max-mem may be 8G, 8000M, or 8000000000 to specify 8 GB of memory, for example.

A simple example of using :ref:`sort-bed` would be:

::

  $ sort-bed unsortedData.bed > sortedData.bed

The :ref:`sort-bed` program efficiently sorts BED inputs. By default, all input records are read into system memory and sorted. If your BED dataset is larger than available system memory, use the ``--max-mem`` option to limit the amount of memory :ref:`sort-bed` uses to do its work:

::

  $ sort-bed --max-mem 2G reallyHugeUnsortedData.bed > reallyHugeSortedData.bed

This option allows :ref:`sort-bed` to scale to input of any size.

.. note:: While `sort-bed` can sort BED files of any size, users may run into trouble sorting files with more than 1021 distinct chromosome names. This can be an issue if your dataset contains unassigned contigs, such as ``chrN_*`` and ``chrUn_*`` `pseudochromosome records <http://genome.ucsc.edu/FAQ/FAQdownloads.html#download11>`_. 

   In this case, you could also use the following command:

   ::

     $ sort --buffer-size=1G -k1,1 -k2,2n foo.bed > sortedFoo.bed

   to sort problematic data on a system with 1 Gb of available system memory.
