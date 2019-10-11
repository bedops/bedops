.. _parallel_bam2bed:

Parallel `bam2bed`
===================

The ``bam2bed_slurm``, ``bam2bed_sge``, and ``bam2bed_gnuParallel`` scripts use a SLURM, Sun or Oracle Grid Engine (SGE/OGE), or `GNU Parallel <https://en.wikipedia.org/wiki/GNU_parallel>`_ job scheduler, respectively, to parallelize the work of ``bam2bed``, which converts an **indexed**, 0-based, half-open ``[start-1, end)`` `Binary (Sequence) Alignment/Map <http://samtools.sourceforge.net/SAM1.pdf>`_ (BAM) file to a sorted, 0-based, half-open ``[start-1, end)`` UCSC BED dataset.

This script splits the indexed BAM file by chromosome name. Each chromosome of BAM records is converted to a BED-formatted dataset with ``bam2bed`` (via ``convert2bed``). Once all per-chromosome BED files are made, they are collated into one final BED file with a multiset union performed with :ref:`bedops --everything <bedops>`.

============
Dependencies
============

This shell script is dependent upon a working computational grid that is managed with SLURM 16.05.0, Sun Grid Engine 6.1u5 (or higher), or installation of GNU Parallel v20130922 or greater.

======
Source
======

The ``bam2bed_slurm``, ``bam2bed_sge``, and ``bam2bed_gnuParallel`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

.. note:: Please review and edit the contents of the relevant script before use with your data, particularly if you use a Sun or Oracle Grid Engine environment and make use of the SGE version of this script. Customization may be required to match your SLURM, SGE/OGE or GNU Parallel installation and environment, as well as the nature of your BAM data.

At minimum, use of this script with an SGE/OGE computational cluster will require editing of the ``queue`` parameter, possible adjustments to ``qsub`` options, and may also require adjustments to paths to working BEDOPS and Samtools binaries. If you use the `module` system, running `module add samtools` will take care of this dependency.

You will also need to make sure your BAM data are indexed. There must be a second BAI file with the same prefix name as the BAM file you wish to compress, located in the same working directory. If this index file is not present, the script will exit early with an error.

You may also wish to review other parameters available with the ``bam2bed`` script, applying them in this script as needed (see the :ref:`bam2bed <bam2bed>` documentation for more details). 
