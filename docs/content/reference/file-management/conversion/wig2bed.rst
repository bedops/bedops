.. _wig2bed:

`wig2bed`
=========

The ``wig2bed`` script converts both *variable* - and *fixed* -step, 1-based, closed ``[start, end]`` `UCSC Wiggle format <http://genome.ucsc.edu/goldenPath/help/wiggle.html>`_ (WIG) to sorted, 0-based, half-open ``[start-1, end)`` extended BED data.

In the case where WIG data are sourced from ``bigWigToWig`` or other tools that generate 0-based, half-open ``[start-1, end)`` WIG, a ``--zero-indexed`` option is provided to generate coordinate output without any re-indexing.

For convenience, we also offer ``wig2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

The utility also supports multiple embedded WIG sections in a single file, which are output to the BED file with modified ID fields, using the ``--multisplit`` option.

======
Source
======

The ``wig2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``wig2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

=====
Usage
=====

The ``wig2bed`` script parses WIG from standard input and prints sorted BED to standard output. The ``wig2starch`` script uses an extra step to parse WIG to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a WIG file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

If the input data contain WIG elements with a start position of 0, the default use of ``wig2bed`` and ``wig2starch`` will exit early with an ``EINVAL`` error. Add the ``--zero-indexed`` option to denote that the input WIG data are zero-indexed, and re-run the conversion tool to print unmodified output coordinates.

.. tip:: If your WIG input is potentially zero-indexed, *e.g.*, if derived from ``bigWigToWig``, where the ``bigWig`` data are themselves sourced from BAM- or bedGraph-formatted data, then it is recommended to use the ``--zero-indexed`` option as a safety measure.

If your data contain multiple WIG sections, use the ``--multisplit <basename>`` option to split sections out to BED elements with modified ID fields. This option can be used in conjunction with the ``--keep-header`` option to preserve metadata.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If sorting converted data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample multi-section WIG input called ``foo.wig`` (see the :ref:`Downloads <wig2bed_downloads>` section to grab this file). We can convert WIG to sorted BED data in the following manner:

::

  $ wig2bed < foo.wig
  chr1    147971108       147971158       id-1    -0.590000
  chr1    147971146       147971196       id-2    0.120000
  chr1    147971184       147971234       id-3    0.110000
  chr1    147971222       147971272       id-4    -0.760000
  ...

.. note:: Even though our WIG input ``foo.wig`` has multiple sections, we can omit the use of ``--multisplit``, because conversion and sorting puts everything into one sorted BED file. However, the header data of the WIG file is discarded. 

If we want to preserve the header data, we can add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field). 

In the case of the sample input ``foo.wig``, we will also need to add the ``--multisplit`` option, as header BED elements from each section will otherwise be collated in a non-sensical way. Adding ``--multisplit`` ensures that header data are converted and stored in separate BED files.

To demonstrate, we next repeat the above conversion, adding the ``--keep-header`` and ``--multisplit`` options:

::

  $ wig2bed --multisplit bar --keep-header < foo.wig > foo.bed

Conversion of this two-section WIG input results in output with modified ID fields to denote their section association:

::

  $ more foo.bed
  _header 0       1       bar.1           track type=wiggle_0 name=foo description=foo
  _header 1       2       bar.2           track type=wiggle_0 name=testfixed
  _header 2       3       bar.2           fixedStep chrom=chrX start=100 step=10 span=5
  chr1    147971108       147971158       bar.1-id-1    -0.590000
  chr1    147971146       147971196       bar.1-id-2    0.120000
  chr1    147971184       147971234       bar.1-id-3    0.110000
  chr1    147971222       147971272       bar.1-id-4    -0.760000
  chrX    99      104     bar.2-id-11   1.900000
  chrX    109     114     bar.2-id-12   2.300000
  chrX    119     124     bar.2-id-13   -0.100000
  chrX    129     134     bar.2-id-14   1.100000
  chrX    139     144     bar.2-id-15   4.100000

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from WIG to BED. While BEDOPS supports 0- and 1-based coordinate indexing, the coordinate change made here is believed to be convenient for most end users.

   In the case where the WIG data contain elements that have a start position of 0, the default use of ``wig2bed`` and ``wig2starch`` will exit early with an ``EINVAL`` error. Add the ``--zero-indexed`` option to denote that the WIG input is zero-indexed and re-run to convert without any coordinate shift.

.. note:: Multiple WIG sections in the input file are merged together by the default ``wig2bed`` behavior. When using the ``--multisplit`` option, each WIG section instead receives its own ID prefix.

.. _wig2bed_downloads:

=========
Downloads
=========

* Sample WIG dataset: :download:`foo.wig <../../../../assets/reference/file-management/conversion/reference_wig2bed_foo.wig>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
