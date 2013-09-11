.. _wig2bed:

`wig2bed`
=========

The ``wig2bed`` script converts 1-based, closed ``[start, end]`` `UCSC Wiggle format <http://genome.ucsc.edu/goldenPath/help/wiggle.html>`_ (WIG) to sorted, 0-based, half-open ``[start-1, end)`` extended BED data.

For convenience, we also offer ``wig2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

The utility also supports multiple embedded WIG sections in a single file, which are output to a single file or split into multiple BED files with the ``--multisplit`` option.

======
Source
======

The ``wig2bed`` and ``wig2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``wig2bed`` script parses WIG from standard input and prints sorted BED to standard output. The ``wig2starch`` script uses an extra step to parse WIG to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If you are sorting data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample WIG input called ``foo.wig`` (see the :ref:`Downloads <wig2bed_downloads>` section to grab this file). We can convert WIG to sorted BED data in the following manner:

::

  $ wig2bed < foo.wig
  chr1    147971109       147971159       id-1    -0.590000
  chr1    147971147       147971197       id-2    0.120000
  chr1    147971185       147971235       id-3    0.110000
  chr1    147971223       147971273       id-4    -0.760000
  ...

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from WIG to BED. While BEDOPS supports 0- and 1-based coordinate indexing, the coordinate change made here is believed to be convenient for most end users.

.. note:: Multiple WIG sections in the input file are merged together by the default ``wig2bed`` behavior. When using the ``--multisplit`` option, each WIG section instead receives its own output file.

.. _wig2bed_downloads:

=========
Downloads
=========

* Sample WIG dataset: :download:`foo.wig <../../../../assets/reference/file-management/conversion/reference_wig2bed_foo.wig>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
