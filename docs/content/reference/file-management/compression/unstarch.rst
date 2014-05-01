.. _unstarch:

`unstarch`
==========

With high-throughput sequencing generating large amounts of genomic data, archiving can be a critical part of an analysis toolkit. BEDOPS includes the ``unstarch`` utility to recover original BED input and whole-file or per-chromosome data attributes from archives created with :ref:`starch` (these can be v1.x or :ref:`v2 archives <starch_specification>`).

The :ref:`unstarch` utility includes `large file support <http://en.wikipedia.org/wiki/Large_file_support>`_ on 64-bit operating systems, enabling extraction of more than 2 GB of data (a common restriction on 32-bit systems).

Starch data can be stored with one of two open-source backend compression methods, either ``bzip2`` or ``gzip``. The :ref:`unstarch` utility will transparently extract data, without the end user needing to specify the backend type.

==================
Inputs and outputs
==================

-----
Input
-----

The :ref:`unstarch` utility takes in a Starch v1.x or v2.x archive as input.

------
Output
------

The typical output of :ref:`unstarch` is :ref:`sorted <sort-bed>` BED data, which is sent to standard output.

Specifying certain options will instead send :ref:`archive metadata <unstarch_archive_metadata>` to standard output, either in text or JSON format, or export :ref:`whole-file or per-chromosome attributes <unstarch_stream_attributes>` (also to standard output).

============
Requirements
============

The metadata of a Starch v2 archive must pass an integrity check before :ref:`unstarch` can extract data. Any manual changes to the metadata will cause extraction to fail.

=====
Usage
=====

Use the ``--help`` option to list all options:

::

  unstarch
   citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
   binary version: 2.5.0 (extracts archive version: 2.1.0 or older)
   authors: Alex Reynolds and Shane Neph

  USAGE: unstarch [ <chromosome> ]  [ --elements | --bases | --bases-uniq | --duplicatesExist | --nestedsExist | --list | --list-json | --list-chromosomes | --archive-timestamp | --note | --archive-version ] <starch-file>

      Process Flags:

      <chromosome>                     Optional. Either unarchives chromosome-specific records from the starch archive file or restricts action of operator to chromosome (e.g., chr1, chrY, etc.).
      --elements                       Show total element count for archive. If <chromosome> is specified, the result shows the element count for the chromosome.
      --bases,
      --bases-uniq                     Show total and unique base counts, respectively, for archive. If <chromosome> is specified, the count is specific to the chromosome, if available.
      --duplicatesExist,               Show whether there are duplicate elements in the specified chromosome, either as numerical (1/0) or string (true/false) value. If no <chromosome> is specified, values are reported for all chromosome records.
      --duplicatesExistAsString 
      --nestedsExist,                  Show whether there are nested elements in the specified chromosome, either as numerical (1/0) or string (true/false) value. If no <chromosome> is specified, values are reported for all chromosome records.
      --nestedsExistAsString
      --list                           List archive metadata (output is in text format). If chromosome is specified, the attributes of the given chromosome are shown.
      --list-json,                     List archive metadata (output is in JSON format)
      --list-json-no-trailing-newline  
      --list-chr,                      List all or specified chromosome in starch archive (similar to "bedextract --list-chr"). If <chromosome> is specified but is not in the output list, nothing is returned.
      --list-chromosomes 
      --note                           Show descriptive note, if available.
      --sha1-signature                 Show SHA1 signature of JSON-formatted metadata (Base64-encoded).
      --archive-timestamp              Show archive creation timestamp (ISO 8601 format).
      --archive-type                   Show archive compression type.
      --archive-version                Show archive version.
      --version                        Show binary version.
      --help                           Show this usage message.

----------
Extraction
----------

Specify a specific chromosome to extract data only from that chromosome. This is optional; if a chromosome is not specified, data are extracted from all chromosomes in the archive.

::

  $ unstarch chr12 example.starch
  ...

.. _unstarch_archive_metadata:

------------------
Archive attributes
------------------

Archive attributes are described in greater depth in the :ref:`Starch specification <starch_specification>` page. We provide an overview here of the major points.

^^^^^^^^
Metadata
^^^^^^^^

Use the ``--list-json`` or ``--list`` options to export the archive metadata as a JSON- or table-formatted text string, sent to standard output:

::

  $ unstarch --list-json example.starch
  {
    "archive": {
      "type": "starch",
      "customUCSCHeaders": false,
      "creationTimestamp": "2014-05-01T14:09:29-0700",
      "version": {
        "major": 2,
        "minor": 1,
        "revision": 0
      },
      "compressionFormat": 0
    },
    "streams": [
      {
        "chromosome": "chr1",
        "filename": "chr1.pid31740.fiddlehead.regulomecorp.com",
        "size": "88330",
        "uncompressedLineCount": 10753,
        "nonUniqueBaseCount": 549829,
        "uniqueBaseCount": 548452,
        "duplicateElementExists": false,
        "nestedElementExists": false
      },
      ...
    ]
  }

The ``--list-chr`` (or ``--list-chromosomes``) option exports a list of chromosomes stored in the Starch archive.

^^^^
Note
^^^^

Using ``--note`` will export any note stored with the archive, when created. 

.. tip:: One can use :ref:`starchcat` to add a new note to an existing Starch archive.

^^^^^^^^^
Timestamp
^^^^^^^^^

The ``--archive-timestamp`` option will report the archive's creation date and time as an `ISO 8601 <http://en.wikipedia.org/wiki/ISO-8601>`_ -formatted string.

^^^^^^^^^^^^^^^^
Compression type
^^^^^^^^^^^^^^^^

The ``--archive-type`` option will report the compression type of the archive, either ``bzip2`` or ``gzip``:

::

  $ unstarch --archive-type example.starch
  unstarch
   archive compression type: bzip2

^^^^^^^
Version
^^^^^^^

The ``--version`` option reports the Starch archive version. This value is different from the version of the :ref:`starch` binary used to create the archive.

.. _unstarch_stream_attributes:

---------------------------------------
Whole-file or per-chromosome attributes
---------------------------------------

^^^^^^^^
Elements
^^^^^^^^

The ``--elements`` operator reports the number of BED elements that were compressed into the chromosome stream, if specified. If no chromosome is specified, the sum of elements over all chromosomes is reported.

.. tip:: This option is equivalent to a ``wc -l`` (line count) operation performed on BED elements that match the given chromosome, but is much, much faster as data are precomputed and stored with the archive, retrieved from the metadata.

^^^^^
Bases
^^^^^

The ``--bases`` and ``--bases-uniq`` flags return the overall and unique base counts for a specified chromosome, or the sum of counts over all chromosomes, if no one chromosome is specified.

^^^^^^^^^^^^^^^^^^^
Duplicate element(s)
^^^^^^^^^^^^^^^^^^^

The ``--duplicatesExist`` operator reports whether the chromosome stream contains one or more duplicate elements, printing a ``0`` if the chromosome does *not* contain a duplicate element, and a ``1`` if the chromosome *does* contain a duplicate. 

.. note:: A duplicate element exists if there are two or more BED elements where the chromosome name and start and stop positions are identical. The id, score, strand and other option columns are ignored when determining if a duplicate exists.

.. tip:: To get a string value of ``true`` or ``false`` in place of ``1`` and ``0``, use the ``--duplicatesExistAsString`` operator, instead.

.. tip:: If the chromosome name argument to ``unstarch`` is omitted, or set to ``all``, the ``--duplicatesExist`` and ``--duplicatesExistAsString`` operators will return a list of results for each chromosome. If the chromosome name is provided and the archive does not contain metadata for the given chromosome, these operators will return ``0`` or ``false`` result.

^^^^^^^^^^^^^^^^^
Nested element(s)
^^^^^^^^^^^^^^^^^

The ``--nestedsExist`` operator reports whether the chromosome stream contains one or more :ref:`nested elements <nested_elements>`, printing a ``0`` if the chromosome does *not* contain a nested element, and a ``1`` if the chromosome *does* contain a nested element. 

.. note:: The definition of a nested element relies on coordinates and is explained in the :ref:`documentation for nested elements <nested_elements>`. The id, score, strand and other option columns are ignored when determining if a nested element exists.

.. tip:: To get a string value of ``true`` or ``false`` in place of ``1`` and ``0``, use the ``--nestedsExistAsString`` operator, instead.

.. tip:: If the chromosome name argument to ``unstarch`` is omitted, or set to ``all``, the ``--nestedsExist`` and ``--nestedsExistAsString`` operators will return a list of results for each chromosome. If the chromosome name is provided and the archive does not contain metadata for the given chromosome, these operators will return ``0`` or ``false`` result.

=======
Example
=======

To extract a generic Starch file input to a BED file:

::

  $ unstarch example.starch > example.bed

This creates the :ref:`sorted <sort-bed>` file ``example.bed``, containing BED data from extracting ``example.starch``. This can be a ``bzip2`` or ``gzip`` -formatted Starch archive |---| :ref:`unstarch` knows how to extract either type transparently.

To list the chromosomes in a Starch v2 archive, use the ``--list-chr`` (or ``--list-chromosomes``) option:

::

  $ unstarch --list-chr example.starch
  chr1
  chr10
  chr11
  chr11_gl000202_random
  chr12
  chr13
  chr14
  chr15
  chr16
  chr17
  ...

To show the number of BED elements in chromosome ``chr13``, use the ``--elements`` operator:

::

  $ unstarch chr13 --elements example.starch
  10753

To find the number of unique bases in chromosome ``chr8``:

::

  $ unstarch chr8 --bases-uniq example.starch
  545822

To report if the chromosome ``chr14`` contains at least one duplicate BED element:

::

  $ unstarch chr14 --duplicatesExistAsString example.starch
  true

To show when the archive was created:

::

  $ unstarch --archive-timestamp example.starch
  2013-01-17T13:44:54-0800

.. note:: Some option calls will not work with legacy v1.x or v2.0 archives. If you have a v1.x or v2.0 archive, use the :ref:`starchcat` utility to upgrade older archives to Starch v2.1 files, which will recalculate and make all attributes available.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
