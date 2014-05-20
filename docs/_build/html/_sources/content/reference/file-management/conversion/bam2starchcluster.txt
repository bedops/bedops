.. _bam2starchcluster:

`bam2starchcluster`
===================

The ``bam2starchcluster`` script uses a Sun Grid Engine (SGE) job scheduler to parallelize the work of ``bam2starch``, which converts an **indexed**, 0-based, half-open ``[start-1, end)`` `Binary (Sequence) Alignment/Map <http://samtools.sourceforge.net/SAM1.pdf>`_ (BAM) file to a sorted, 0-based, half-open ``[start-1, end)`` UCSC BED dataset, and thence converts this to a :ref:`Starch-formatted <starch_specification>` archive.

This script splits the indexed BAM file by chromosome name. Each chromosome of BAM records is converted to a :ref:`Starch-formatted <starch_specification>` archive with ``bam2starch``. Once all per-chromosome archives are made, they are collated into one final Starch archive with :ref:`starchcat <starchcat>`.

.. tip:: A :ref:`Starch-formatted <starch_specification>` archive can save a lot of space relative to the original BAM format, up to 33% of the original BAM dataset, while offering per-chromosome random access. Further, use of a computational grid practically reduces the total compression time to that of the largest chromosome (*e.g.*, ``chr1`` or similar), an order of magnitude reduction over ``bam2starch`` alone.

============
Dependencies
============

This ``python`` shell script is dependent upon the installation of `SAMtools <http://samtools.sourceforge.net/>`_ and Python, version 2.6.2 or greater (and less than Python3). It also requires a computational grid that is managed with Sun Grid Engine 6.1u5 (or higher).

======
Source
======

The ``bam2starchcluster`` conversion script is part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

.. note:: Please review and edit the contents of this script before use with your data. Customization will be required to match your SGE installation and environment, as well as the nature of your BAM data.

At minimum, use of this script with your computational cluster will require editing of the ``queue`` parameter, and possibly adjustments to ``qsub`` options.

You will also need to make sure your BAM data are indexed. There should be a second BAI file with the same name as the BAM file you wish to compress, located in the same working directory.

Further, if your indexed BAM data contain custom tags, edit the ``bam2starch`` call within this script to append the ``--custom-tags <value>`` argument (see the :ref:`bam2bed <bam2bed>` documentation for more details). You may also wish to review other parameters available with the ``bam2starch`` script, applying them in this script as needed.