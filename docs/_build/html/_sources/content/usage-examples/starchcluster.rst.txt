.. _starchcluster:

Efficiently creating Starch-formatted archives with a cluster
=============================================================

In this example, we demonstrate how to use :ref:`bedextract` and :ref:`starchcat` to efficiently generate Starch-formatted archives from BED datasets.

===================
BEDOPS tools in use
===================

For this script, we use :ref:`bedextract` to quickly build a list of chromosomes in an input BED dataset and extract records for each chromosome to separate files. We then use :ref:`starch` to compress each per-chromosome file and :ref:`starchcat` to concatenate per-chromosome Starch archives into one file.

======
Script
======

Three versions of the ``starchcluster`` script are included with the source and package distributions of BEDOPS (see :ref:`Installation <installation>` for more detail). 

One version makes use of an `Oracle Grid Engine <http://en.wikipedia.org/wiki/Oracle_Grid_Engine>`_ (or Sun Grid Engine) cluster environment to distribute per-chromosome tasks, another script uses `GNU Parallel <http://www.gnu.org/software/parallel/>`_ to split the workload over cores or processors on the local host. Finally, we include a version that implements a `SLURM <https://en.wikipedia.org/wiki/Slurm_Workload_Manager>`_ -capable script.

==========
Discussion
==========

The overview that follows applies to the Grid Engine-based version of the `starchcluster` script. However, the general algorithm is identical for the Grid Engine-, GNU Parallel-, and SLURM-based compression scripts.

-------------------
Splitting BED files
-------------------

Whole-genome analyses are often "`embarassingly parallel <http://en.wikipedia.org/wiki/Embarrassingly_parallel>`_", in that per-chromosome computations can be placed onto separate work nodes of a computational cluster, with results collated at the end in "`map-reduce <http://en.wikipedia.org/wiki/MapReduce>`_" fashion.

If we want to filter any BED file to retrieve elements from a specific chromosome (say, to compress a BED file, one chromosome at a time), to arrange this kind of analysis, one trivial |---| but very slow |---| way to do this involves sequentially walking line by line through the file to parse and test each element. This can take a while to do. 

However, just as BEDOPS tools use the information in :ref:`sorted data <sort-bed>` to apply efficient set and statistical operations, we can use this same information to jump quickly through our data of interest.

Specifically, sorting allows us to perform a `binary search <http://en.wikipedia.org/wiki/Binary_search_algorithm>`_: 

1. We jump to the middle byte of the BED file, stream to the nearest element, then parse and test the chromosome name. 
2. Either we have a match, or we jump to the middle of the remaining left or right half (decided by dictionary order), parse and test again. 
3. We repeat steps 1 and 2 until we have matches that define the bounds of the target chromosome.

.. image:: ../../assets/reference/set-operations/reference_bedextract_mechanism.png
   :width: 99%

To indicate the kind of speed gain that the :ref:`bedextract` tool provides, in local testing, a naïve listing of chromosomes from a 36 GB BED input using UNIX ``cut`` and ``uniq`` utilities took approximately 20 minutes to complete on a typical Core 2 Duo-based Linux workstation. Retrieval of the same chromosome listing with ``bedextract --list-chr`` took only 2 seconds (cache flushed |---| no cheating!).

-----------------------
Compressing BED subsets
-----------------------

Now we can very quickly demarcate where chromosomes start and stop in a BED file, we can apply :ref:`starch` on those subsets on separate cluster nodes. 

----------------------------------
Stitching together compressed sets
----------------------------------

Once we have per-chromosome Starch-formatted archives, we need some way to put them all together into one archive. This is where :ref:`starchcat` comes in, taking all the per-chromosome archives as inputs and creating a new archive as output.

The big picture view is like this:

.. image:: ../../assets/reference/file-management/compression/starchcluster_mechanism.png
   :width: 99%

As the figure notes, the compression time for a very large BED file is reduced roughly to the time taken to compress the largest chromosome in the original file. Parallelization of this process is an order of magnitude faster than compressing chromosomes in serial.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
