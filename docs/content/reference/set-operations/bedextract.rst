.. _bedextract:

`bedextract`
============

The ``bedextract`` utility performs three primary tasks, with the goal of doing them very quickly:

1. Lists all the chromosomes in a sorted input BED file.
2. Extracts all the elements in a sorted input BED file, for a given chromosome.
3. Finds elements of one BED file, which overlap elements in a second, reference BED file (when specific element criteria are satisfied).

One might ask why use this utility, when the first two tasks can already be performed with common UNIX text processing tools, such as ``cut``, ``sort``, ``uniq``, and ``awk``, and the third task can be performed with :ref:`bedops` with the ``--element-of -1`` options?

The ``bedextract`` utility does the work of all those tools without streaming through an entire BED file, resulting in massive performance improvements. By using the hints provided by sorted BED input, the :ref:`bedextract` tool can jump around, seeking very quick answers to these questions about your data.

============
How it works
============

Specifically, sorting with :ref:`sort-bed` allows us to perform a `binary search <http://en.wikipedia.org/wiki/Binary_search_algorithm>`_: 

1. We jump to the middle byte of the BED file, stream to the nearest element, then parse and test the chromosome name. 
2. Either we have a match, or we jump to the middle of the remaining left or right half (decided by dictionary order), parse and test again. 
3. We repeat steps 1 and 2 until we have matches that define the bounds of the target chromosome.

.. image:: ../../../assets/reference/set-operations/reference_bedextract_mechanism.png
   :width: 75%

To indicate the kind of speed gain that the :ref:`bedextract` tool provides, in local testing, a naïve listing of chromosomes from a 36 GB BED input using UNIX ``cut`` and ``uniq`` utilities took approximately 20 minutes to complete on a typical Core 2 Duo-based Linux workstation. Retrieval of the same chromosome listing with ``bedextract --list-chr`` took only 2 seconds (cache flushed |---| no cheating!).

.. tip:: While listing chromosomes is perhaps a trivial task, 1200 seconds to 2 seconds is a 600-fold speedup. Similar improvements are gained from using ``--chrom`` and ``--faster`` options with other core BEDOPS tools like :ref:`bedops` and :ref:`bedmap`. If your data meet the criteria for using this approach |---| and a lot of genomic datasets do |---| we strongly encourage adding this to your toolkit.

==================
Inputs and outputs
==================

-----
Input
-----

Depending on specified options, :ref:`bedextract` requires one or two :ref:`sorted <sort-bed>` BED files.

.. note:: It is critical that inputs are :ref:`sorted <sort-bed>` as the information in a sorted file allows :ref:`bedextract` to do its work correctly. If your datasets are output from other BEDOPS tools, then they are already sorted!

------
Output
------

Depending on specified options, the :ref:`bedextract` program will send a list of chromosomes or BED elements to standard output. 

.. tip:: The use of UNIX-like standard streams allows easy downstream analysis or post-processing with other tools and scripts, including other BEDOPS utilities.

=====
Usage
=====

The ``--help`` option describes the functionality available to the end user:

::

  bedextract
    citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
    version:  2.3.0
    authors:  Shane Neph & Alex Reynolds

      Every input file must be sorted per sort-bed.

   USAGE:
     0) --help or --version           Print requested info and exit successfully.
     1) --list-chr <input.bed>        Print all unique chromosome names found in <input.bed>.
     2) <chromosome> <input.bed>      Retrieve all rows for chr8 with:  bedextract chr8 <input.bed>.
     3) <query.bed> <target>          Grab elements from the <query.bed> that overlap elements in <target>. Same as
                                       `bedops -e -1 <query.bed> <target>`, except that this option fails silently
                                        if <query.bed> contains fully-nested BED elements.  If no fully-nested
                                        element exists, bedextract can vastly improve upon the performance of bedops.
                                        <target> may be a BED or Starch file (with or without fully-nested elements).
                                        Using '-' for <target> indicates input (in BED format) comes from stdin.

-------------------
Listing chromosomes
-------------------

Use the ``--list-chr`` option to quickly retrieve a listing of chromosomes from a given sorted BED input. 

For example, the following lists the chromosomes in an example BED file of FIMO motif hits (see the :ref:`Downloads <bedextract_downloads>` section):

::

  $ bedextract --list-chr motifs.starch
  chr1
  chr10
  chr11
  chr12
  ...
  chr9
  chrX

----------------------------------------------
Retrieving elements from a specific chromosome
----------------------------------------------

To quickly retrieve the subset of elements from a sorted BED file associated with a given chromosome, apply the second usage case and specify the chromosome as the argument. 

For example, to retrieve ``chrX`` from the same motif sample:

::

  $ bedextract chrX motifs.starch
  chrX    6775077 6775092 +V_SPZ1_01      4.92705e-06     +       GTTGGAGGGAAGGGC
  chrX    6775168 6775179 +V_ELF5_01      8.57585e-06     +       TCAAGGAAGTA
  chrX    6777790 6777799 +V_CKROX_Q2     8.90515e-06     +       TCCCTCCCC
  ...

-------------------------------------------------
Retrieving elements which overlap target elements
-------------------------------------------------

A common :ref:`bedops` query involves asking which elements overlap one or more bases between two BED datasets, which we will call here ``Query`` and ``Target``. 

One can already use ``bedops --element-of -1`` to accomplish this task, but if certain specific criteria are met (which we will describe shortly) then a much faster result can often be obtained by instead using :ref:`bedextract`. 

Three criteria make the use of :ref:`bedextract` in this mode very successful in practice, with potentially massive speed improvements:

1. ``Query`` is a huge file.
2. There are relatively few regions of interest in ``Target`` (say, roughly 30,000 or fewer).
3. There are **no fully-nested elements** in ``Query`` (but duplicate coordinates are fine).

^^^^^^^^^^^^^^^^^^^^^^^^^
What are nested elements?
^^^^^^^^^^^^^^^^^^^^^^^^^

An example of a sorted BED file which contains a nested element follows:

::

  chr1    1      100
  chr1    100    200
  chr1    125    150
  chr1    150    1000

While this dataset is sorted, the element ``chr1:125-150`` is entirely nested within ``chr1:100-200``:



.. _bedextract_downloads:

=========
Downloads
=========

* Sample :download:`FIMO motifs <../../../assets/reference/set-operations/reference_bedextract_motifs.starch>`


.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim: