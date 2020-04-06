.. _starch:

`starch`
========

With high-throughput sequencing generating large amounts of genomic data, archiving can be a critical part of an analysis toolkit. BEDOPS includes the ``starch`` utility to provide a method for efficient and lossless compression of `UCSC BED-formatted data <http://genome.ucsc.edu/FAQ/FAQformat.html#format1>`_ into the :ref:`Starch v2 format <starch_specification>`.

Starch v2 archives can be extracted with :ref:`unstarch` to recover the original BED input, or processed as inputs to :ref:`bedops` and :ref:`bedmap`, where set operations and element calculations can be performed directly and without the need for intermediate file extraction.

The :ref:`starch` utility includes `large file support <http://en.wikipedia.org/wiki/Large_file_support>`_ on 64-bit operating systems, enabling compression of more than 2 GB of data (a common restriction on 32-bit systems).

Data can be stored with one of two open-source backend compression methods, either ``bzip2`` or ``gzip``, providing the end user with a reasonable tradeoff between speed and storage performance that can be useful for working with constrained storage situations or slower hardware.

==================
Inputs and outputs
==================

-----
Input
-----

As with other BEDOPS utilities, :ref:`starch` takes in :ref:`sorted <sort-bed>` BED data as input. You can use :ref:`sort-bed` to sort BED data, piping it into :ref:`starch` as standard input (see :ref:`Example <starch_example>` section below).

.. note:: While more than three columns may be specified, most of the space savings in the Starch format are derived from from a pre-processing step on the coordinates. Therefore, minimizing or removing unnecessary columnar data from the fourth column on (*e.g.*, with ``cut -f1-3`` or similar) can help improve compression efficiency considerably.

------
Output
------

This utility outputs a :ref:`Starch v2-formatted <starch_specification>` archive file.

.. _starch_example:

============
Requirements
============

The :ref:`starch` tool requires data in a relaxed variation of the BED format as described by `UCSC’s browser documentation <http://genome.ucsc.edu/FAQ/FAQformat.html#format1>`_. BED data should be sorted before compression, *i.e.* with BEDOPS :ref:`sort-bed`. 

At a minimum, three columns are required to specify the chromosome name and start and stop positions. Additional columns may be specified, containing up to 128 kB of data per row (including tab delimiters).

=====
Usage
=====

Use the ``--help`` option to list all options:

::

  starch
   citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
   binary version: 2.4.39 (typical) (creates archive version: 2.2.0)
   authors:  Alex Reynolds and Shane Neph

  USAGE: starch [ --note="foo bar..." ]
                [ --bzip2 | --gzip ]
                [ --omit-signature ]
                [ --report-progress=N ]
                [ --header ] [ <unique-tag> ] <bed-file>
      
      * BED input must be sorted lexicographically (e.g., using BEDOPS sort-bed).
      * Please use '-' to indicate reading BED data from standard input.
      * Output must be directed to a regular file.
      * The bzip2 compression type makes smaller archives, while gzip extracts
        faster.
      
      Process Flags
      --------------------------------------------------------------------------
      --note="foo bar..."   Append note to output archive metadata (optional).

      --bzip2 | --gzip      Specify backend compression type (optional, default
                            is bzip2).

      --omit-signature      Skip generating per-chromosome data integrity signature
                            (optional, default is to generate signature).

      --report-progress=N   Report compression progress every N elements per
                            chromosome to standard error stream (optional)

      --header              Support BED input with custom UCSC track, SAM or VCF
                            headers, or generic comments (optional).

      <unique-tag>          Optional. Specify unique identifier for transformed
                            data.

      --version             Show binary version.

      --help                Show this usage message.

=======
Options
=======

------------------------
Backend compression type
------------------------

Use the ``--bzip2`` or ``--gzip`` operators to use the ``bzip2`` or ``gzip`` compression algorithms on transformed BED data. By default, :ref:`starch` uses the ``bzip2`` method.

----
Note
----

Use the ``--note="xyz..."`` option to add a custom string that describes the archive. This data can be retrieved with ``unstarch --note``.

.. tip:: Examples of usage might include a description of the experiment associated with the data, a URL to a UCSC Genome Browser session, or a bar code or other unique identifier for internal lab or LIMS use.

.. note:: The only limitation on the length of a note is the command-line shell's maximum argument length parameter (as found on most UNIX systems with the command ``getconf ARG_MAX``) minus the length of the non- ``--note="..."`` command components. On most desktop systems, this value will be approximately 256 kB.

---------------------------------------
Per-chromosome data integrity signature
---------------------------------------

By default, a data integrity signature is generated for each chromosome. This can be used to verify if chromosome streams from two or more Starch archives are identical, or used to test the integrity of a chromosome, to identify potential data corruption. 

Generating this signature adds to the computational cost of compression, or an integrity signature may not be useful for all archives. Add the ``--omit-signature`` option, if the compression time is too high or the data integrity signature is not needed.

--------------------
Compression progress
--------------------

To optionally track the progress of compression, use the ``--report-progress=N`` option, specifying a positive integer ``N`` to report the compression of the *N* -th element for the current chromosome. The report is printed to the standard error stream.

.. note:: For instance, specifying a value of ``1`` reports the compression of every input element of all chromosomes, while a value of ``1000`` would report the compression of every 1000th element of the current chromosome.

-------
Headers
-------

Add the ``--header`` flag if the BED data being compressed contain `extra header data <http://genome.ucsc.edu/FAQ/FAQformat.html#format1.7>`_ that are exported from a UCSC Genome Browser session.

.. note:: If the BED data contain custom headers and ``--header`` is not specified, :ref:`starch` will be unable to read chromosome data correctly and exit with an error state.

----------
Unique tag
----------

Adding a ``<unique-tag>`` string replaces portions of the `filename` key in the archive's :ref:`stream metadata <starch_archive_metadata_stream>`.

.. note:: This feature is largely obsolete and included for legacy support. It is better to use the ``--note="xyz..."`` option to add identifiers or other custom data.

=======
Example
=======

To compress unsorted BED data (or data of unknown sort order), we feed :ref:`starch` a :ref:`sorted <sort-bed>` stream, using the hyphen (``-``) to specify standard input:

::

  $ sort-bed unsorted.bed | starch - > sorted.starch

This creates the file ``sorted.starch``, which uses the ``bzip2`` algorithm to compress transformed BED data from a sorted permutation of data in ``unsorted.bed``. No note or custom tag data is added.

It is possible to speed up the compression of a BED file by using a cluster. Start by reviewing our :ref:`starchcluster <starchcluster>` script.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
