.. _parallel_bam2starch:

Parallel `bam2starch`
=====================

The ``bam2starch_slurm``, ``bam2starch_sge``, and ``bam2starch_gnuParallel`` scripts use a SLURM, Sun or Oracle Grid Engine (SGE/OGE), or `GNU Parallel <https://en.wikipedia.org/wiki/GNU_parallel>`_ job scheduler, respectively, to parallelize the work of ``bam2starch``, which converts an **indexed**, 0-based, half-open ``[start-1, end)`` `Binary (Sequence) Alignment/Map <http://samtools.sourceforge.net/SAM1.pdf>`_ (BAM) file to a sorted, 0-based, half-open ``[start-1, end)`` UCSC BED dataset, and thence converts this to a :ref:`Starch-formatted <starch_specification>` archive.

This script splits the indexed BAM file by chromosome name. Each chromosome of BAM records is converted to a :ref:`Starch-formatted <starch_specification>` archive with ``bam2starch`` (via ``convert2bed``). Once all per-chromosome archives are made, they are collated into one final Starch archive with :ref:`starchcat <starchcat>`.

.. tip:: A :ref:`Starch-formatted <starch_specification>` archive can save a great deal of space relative to the original BAM format, up to 33% of the original BAM dataset, while offering per-chromosome random access. Further, use of a computational grid practically reduces the total compression time to that of the largest chromosome (*e.g.*, ``chr1`` or similar), an order of magnitude reduction over ``bam2starch`` alone.

============
Dependencies
============

This shell script is dependent upon a working computational grid that is managed with SLURM 16.05.0, Sun Grid Engine 6.1u5 (or higher), or installation of GNU Parallel v20130922 or greater.

======
Source
======

The ``bam2starch_slurm``, ``bam2starch_sge``, and ``bam2starch_gnuParallel`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

.. note:: Please review and edit the contents of this script before use with your data. Customization may be required to match your SLURM, SGE/OGE, or GNU Parallel installation and environment, as well as the nature of your BAM data.

At minimum, use of this script with an SGE/OGE computational cluster will require editing of the ``queue`` parameter, possible adjustments to ``qsub`` options, and may require adjustments to paths to working BEDOPS binaries.

You will also need to make sure your BAM data are indexed. There must be a second BAI file with the same prefix name as the BAM file you wish to compress, located in the same working directory. If this index file is not present, the script will exit early with an error.

You may also wish to review other parameters available with the ``bam2starch`` script, applying them in this script as needed (see the :ref:`bam2bed <bam2bed>` documentation for more details). 
