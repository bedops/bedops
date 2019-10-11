.. _convert2bed:

`convert2bed`
=============

The ``convert2bed`` binary converts common binary and text genomic formats (BAM, GFF, GTF, GVF, PSL, RepeatMasker annotation output (OUT), SAM, VCF and WIG) to unsorted or sorted, extended BED or :ref:`BEDOPS Starch <starch_specification>` (compressed BED) with additional per-format options.

Convenience wrapper bash scripts are provided for each of these input formats, which convert standard input to unsorted or sorted BED, or to BEDOPS Starch (compressed BED). Scripts expose format-specific ``convert2bed`` options.

We also provide ``bam2bed_sge``, ``bam2bed_gnuParallel``, ``bam2starch_sge`` and ``bam2starch_gnuParallel`` convenience scripts, which parallelize the conversion of indexed BAM to BED or to BEDOPS Starch via a Sun Grid Engine-based computational cluster or local GNU Parallel installation.

============
Dependencies
============

Conversion of BAM and SAM input is dependent upon the installation of `SAMtools <http://samtools.sourceforge.net/>`_ and :ref:`convert2bed <convert2bed>`. All ``*2starch`` wrapper scripts are further dependent on the installation of the :ref:`starch <starch>` binary, part of a typical BEDOPS installation.

======
Source
======

The ``convert2bed`` conversion tool is part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

Generally, to convert data in format ``xyz`` to sorted BED:

::

  $ convert2bed -i xyz < input.xyz > output.bed

Add the ``-o starch`` option to write a BEDOPS Starch file, which stores compressed BED data and feature metadata:

::

  $ convert2bed -i xyz -o starch < input.xyz > output.starch

Wrappers are available for each of the supported formats to convert to BED or Starch, *e.g.*:

::

  $ bam2bed < reads.bam > reads.bed
  $ bam2starch < reads.bam > reads.starch

.. tip:: Format-specific options are available for each wrapper; use ``--help`` with a wrapper script or ``--help-bam``, ``--help-gff`` etc. with ``convert2bed`` to get a format-specific description of the conversion procedure and options.

=======
Example
=======

Please review documentation for each wrapper script to see format-specific examples of their use.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
