.. _starchcat:

`starchcat`
===========

The ``starchcat`` utility efficiently merges per-chromosome records contained within one or more BEDOPS :ref:`Starch-formatted <starch_specification>` archives. This is an equivalent operation to ``bedops --everything`` or ``bedops -u`` (a :ref:`multiset union <bedops_everything>`), but inputs are :ref:`starch` archives rather than uncompressed BED files.

As a further advantage to using this over :ref:`bedops`, in the case where a :ref:`starch` input contains BED elements exclusive to one chromosome, this utility will directly and quickly copy over compressed elements to a new archive, avoiding the need for costly and wasteful extraction and re-compression. 

In the general case, where two or more :ref:`starch` inputs contain BED elements from the same chromosome, a :ref:`sorted <sort-bed>` merge is performed and the stream reprocessed into a :ref:`Starch-formatted <starch_specification>` archive.

===============
Parallelization
===============

Those with access to a computational cluster such as an Oracle/Sun Grid Engine or a group of hosts running SSH services should find :ref:`starchcat` highly useful, as this facilitates:

* Much faster compression of an entire genome of BED data, using nodes of a computational cluster to compress separate chromosomes, followed by a collation step with :ref:`starchcat` (see the :ref:`starchcluster` documentation).

* Extraction, manipulation and reintegration of a :ref:`starch` -ed chromosome into a larger :ref:`starch` archive

* Refreshing metadata or re-compressing the data within a lone :ref:`starch` archive.

To demonstrate the first application of this utility, we have packaged a helper script with the BEDOPS suite called :ref:`starchcluster <starchcluster>`, which archives data much faster than :ref:`starch` alone. By distributing work across the nodes of a computational cluster, the upper bound on compression time is almost entirely determined by the largest chromosome, reducing compression time by an order of magnitude.

==================
Inputs and outputs
==================

-----
Input
-----

The input to :ref:`starchcat` consists of one or more BEDOPS :ref:`Starch-formatted <starch_specification>` archive files. 

.. note:: If a single archive is provided as input, it may be reprocessed with specified options. When two or more archives are specified, the output will be the equivalent of a multiset union of the inputs.

.. note:: This utility does not accept standard input. 

------
Output
------

The :ref:`starchcat` tool outputs a :ref:`starch` -formatted archive to standard output, which is usually redirected to a file.

Additionally, an optional compression flag specifies if the final :ref:`starch` output should be compressed with either the ``bzip2`` or ``gzip`` method (the default being ``bzip2``). 

.. note:: If :ref:`starch` inputs use a different backend compression method, the input stream is re-compressed before integrated into the larger archive. This will incur extra processing overhead.

=====
Usage
=====

Use the ``--help`` option to list all options:

::

  starchcat
   citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
   version:  2.4.39 (typical)
   authors:  Alex Reynolds and Shane Neph

  USAGE: starchcat [ --note="..." ]
                   [ --bzip2 | --gzip ]
                   [ --omit-signature ]
                   [ --report-progress=N ] <starch-file-1> [<starch-file-2> ...]

      * At least one lexicographically-sorted, headerless starch archive is
        required.

      * While two or more inputs make sense for a multiset union operation, you
        can starchcat one file in order to update its metadata, recompress it
        with a different backend method, or add a note annotation.

      * Compressed data are sent to standard output. Use the '>' operator to
        redirect to a file.

      Process Flags
      --------------------------------------------------------------------------
      --note="foo bar..."   Append note to output archive metadata (optional).

      --bzip2 | --gzip      Specify backend compression type (optional, default
                            is bzip2).

      --omit-signature      Skip generating per-chromosome data integrity signature
                            (optional, default is to generate signature).

      --report-progress=N   Report compression progress every N elements per
                            chromosome to standard error stream (optional)

      --version             Show binary version.

      --help                Show this usage message.

---------------------------------------
Per-chromosome data integrity signature
---------------------------------------

By default, a data integrity signature is generated for each chromosome. This can be used to verify if chromosome streams from two or more Starch archives are identical, or used to test the integrity of a chromosome, to identify potential data corruption. 

Generating this signature adds to the computational cost of compression, or an integrity signature may not be useful for all archives. Add the ``--omit-signature`` option, if the compression time is too high or the data integrity signature is not needed.

-------
Example
-------

Let's say we have a set of 23 :ref:`starch` archives, one for each chromosome of the human genome: ``chr1.starch``, ``chr2.starch``, and so on, to ``chrY.starch``. (To simplify this example, we leave out mitochondrial, random, pseudo- and other chromosomes.) We would like to build a new :ref:`starch` archive from these 23 separate files:

::

  $ starchcat chr1.starch chr2.starch ... chrY.starch > humanGenome.starch

The :ref:`starchcat` utility parses the metadata from each of the 23 inputs, determines what data to either simple copy or reprocess, and then it performs the merge. Cleanup is performed afterwards, as necessary, and the output is a brand new :ref:`starch` file, written to ``humanGenome.starch``.

.. note:: No filtering or processing is performed on extracted BED elements, before they are written to the final output. Thus, *it is possible for duplicate BED elements to occur*. It would be easy to use the ``--signature`` option to validate the expected content of a new Starch archive.

   However, the final archive is sorted per :ref:`sort-bed` ordering, so that data extracted from this archive will be ready for use with BEDOPS utilities.

.. note:: When input archives contain data on disjoint chromosomes, use of :ref:`starchcat` is very efficient as data are simply copied, instead of extracted and re-compressed.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
