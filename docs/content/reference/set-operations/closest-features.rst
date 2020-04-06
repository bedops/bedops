.. _closest-features:

`closest-features`
==================

The ``closest-features`` program efficiently associates nearest features between two sorted inputs, based upon genomic distance measures.

An application of this tool in our own research is :ref:`finding the nearest DNase hypersensitive sites <distance_frequencies>` upstream and downstream from a given SNP, as well as signed distances. The :ref:`closest-features` program can report both results.

As another example of what one can do with this utility, we can identify the closest transcriptional start site for a given putative replication origin. Suppose we have a sorted BED file named ``TSS.bed`` that contains all transcriptional start sites of all genes in some genome. Further, suppose that we have a set of measurements showing probable replication origins for the same species in a sorted BED file named ``RepOrigins.bed``. The following command gives the closest TSS to each origin:

::

  $ closest-features --closest RepOrigins.bed TSS.bed

By default, the program will echo each entry from ``RepOrigins.bed``, followed by the two closest elements in ``TSS.bed`` (the closest element to each side of the entry from ``RepOrigins.bed``), with output columns separated by a pipe (``|``). With the ``--shortest`` option, the echoed entry from ``RepOrigins.bed`` and only the single nearest element in ``TSS.bed`` will be part of the output.

==================
Inputs and outputs
==================

-----
Input
-----

The :ref:`closest-features` program takes two sorted BED files (a so-called *reference* file and a *map* file), as well as optional arguments for modifying behavior and outputs.

Alternatively, :ref:`closest-features` can accept :ref:`Starch-formatted archives <starch>` as inputs, with no need to extract archive data to intermediate BED files!

Support for common headers (such as UCSC track headers) is offered through the ``--header`` option. Headers are stripped from output.

------
Output
------

The :ref:`closest-features` program returns summary data to standard output, which may include reference and nearest elements and distance values (depending on provided options).

=====
Usage
=====

The ``--help`` option describes the various operations and options available to the end user:

::

  closest-features
    citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
    version:  2.4.39 (typical)
    authors:  Shane Neph & Scott Kuehn

  USAGE: closest-features [Process-Flags] <input-file> <query-file>
     All input files must be sorted per sort-bed.
     The program accepts BED and Starch file formats
     May use '-' for a file to indicate reading from standard input (BED format only).

     For every element in <input-file>, determine the two elements from <query-file> falling
       nearest to its left and right edges (See NOTES below).  By default, echo the <input-file>
       element, followed by those left and right elements found in <query-file>.

    Process Flags:
      --chrom <chromosome>   : Process data for given <chromosome> only.
      --closest              : Choose the closest element for output only.  Ties go the left element.
      --delim <delim>        : Change output delimiter from '|' to <delim> between columns (e.g. '\t')
      --dist                 : Print the signed distances to the <input-file> element as additional
                                 columns of output.  An overlapping element has a distance of 0.
      --ec                   : Error check all input files (slower).
      --header               : Accept headers (VCF, GFF, SAM, BED, WIG) in any input file.
      --help                 : Print this message and exit successfully.
      --no-overlaps          : Overlapping elements from <query-file> will not be reported.
      --no-ref               : Do not echo elements from <input-file>.
      --version              : Print program information.

    NOTES:
      If an element from <query-file> overlaps the <input-file> element, its distance is zero.
        An overlapping element takes precedence over all non-overlapping elements.  This is true
        even when the overlapping element's edge-to-edge distance to the <input-file>'s element
        is greater than the edge-to-edge distance from a non-overlapping element.
      Overlapping elements may be ignored completely (no precedence) with --no-overlaps.
      Elements reported as closest to the left and right edges are never the same.
      When no qualifying element from <query-file> exists as a closest feature, 'NA' is reported.

===================================
Per-chromosome operations (--chrom)
===================================

All operations on inputs can be restricted to one chromosome, by adding the ``--chrom <val>`` operator. 

.. tip:: This option is highly useful for cluster-based work, where operations on large BED inputs can be split up by chromosome and pushed to separate cluster nodes.

To demonstrate the use of this option, we take two sample Starch-archived BED datasets ``A`` and ``B`` (refer to the :ref:`Downloads <closest-features_downloads>` section for sample inputs) which contain regions from multiple chromosomes:

::

  $ unstarch A.starch 
  chr1    100     200     id-001A
  chr1    400     500     id-002A
  chr2    100     300     id-003A

  $ unstarch B.starch
  chr1    150     300     id-001B
  chr1    500     600     id-002B
  chr2    100     150     id-003B
  chr2    180     500     id-004B

Now we want to ask, what is the closest element from ``chr2`` in ``A``, to ``chr2`` elements in ``B``:

::

  $ closest-features --chrom chr2 --closest A.starch B.starch
  chr2    100     300     id-003A|chr2    100     150     id-003B

As we expect, element ``id-003A`` is closest to element ``id-003B`` between the two datasets. 

==============
Error checking
==============

For performance reasons, no error checking of input is done, by default. Add ``--ec`` for stringent error checking and debugging purposes.

.. note:: Using ``--ec`` will slow down analysis considerably. We recommend using this option to test and debug pipelines and then removing it for use in production.

.. _closest-features_downloads:

=========
Downloads
=========

* Sample dataset :download:`A <../../../assets/reference/set-operations/reference_closestfeatures_a.starch>`
* Sample dataset :download:`B <../../../assets/reference/set-operations/reference_closestfeatures_b.starch>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
