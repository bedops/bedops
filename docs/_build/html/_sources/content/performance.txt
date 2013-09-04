Performance
===========

In this document, we compare the performance of our set operations and compression utilities with common alternatives. In-house performance measures include speed, memory usage, and compression efficiency on a dual-core machine with 18 GB of virtual memory. Additionally, we report independently-generated performance statistics collected by a research group that has recently released a similar analysis toolkit.

-------------------------
Test environment and data
-------------------------

Timed results were derived using actual running times (also known as wall-clock times), averaged over 3 runs. All timed tests were performed using a single 64-bit Linux machine with a dual-core 3 GHz Intel Xeon processor, 8 GB of physical RAM, and 18 GB of total virtual memory. All caches were purged in between sequential program runs to remove hardware biases.

Random subsamples of `phyloP conservation <http://compgen.bscb.cornell.edu/phast/>`_ for the human genome were used as inputs for testing whenever the full phyloP results were not used. The full phyloP results were downloaded from `UCSC <http://hgdownload.cse.ucsc.edu/goldenPath/hg19/phyloP46way/>`_.

==============================
Set operations with ``bedops``
==============================

In this section, we provide time and memory measurements of various :ref:`bedops` operations against analogous `BEDTools <http://code.google.com/p/bedtools/>` utilities.

---------------------
Direct merge (sorted)
---------------------

.. image:: ../assets/performance/performance_bedops_merge_sorted.png

The performance of the ``mergeBed`` program (with the ``-i`` option) from the BEDTools suite (v2.12.0) was compared with that of the ``--merge`` option of our :ref:`bedops` utility.

As measured, the ``mergeBed`` program loads all data from a file into memory and creates an index before computing results, incurring longer run times and higher memory costs that can lead to failures. The :ref:`bedops` utility minimizes memory consumption by retaining only the information required to compute the next line of output.

---------------------------
Complement and intersection
---------------------------

.. image:: ../assets/performance/performance_bedops_complement_sorted.png

.. image:: ../assets/performance/performance_bedops_complement_sorted.png

The ``complementBed`` (with ``-i`` and ``-g`` options) and ``intersectBed`` (with ``-u``, ``-a``, and ``-b`` options) programs from the BEDTools suite (v2.12.0) also were compared to our :ref:`bedops` program. 

Both BEDTools programs were unable to complete operations after 51M elements with the allocated 18 GB of memory. The :ref:`bedops` program continued operating on the full dataset.

+------------+------------------+
| |note_png| | |intersect_note| |
+------------+------------------+

----------
Discussion
----------

The :ref:`bedops` utility performs a wide range of set operations (merge, intersect, union, symmetric difference, and so forth). As with all main utilities in BEDOPS, the program requires :ref:`sorted <sort-bed>` inputs and creates sorted results on output. As such, sorting is, at most, a *one-time cost* to operate on data any number of times in the most efficient way. Also, as shown in an :ref:`independent study <independent_testing>`, BEDOPS also sorts data more efficiently than other tools. Further, our utility can sort BED inputs of any size.

Another important feature of :ref:`bedops` that separates it from the competition is its ability to work with :ref:`any number of inputs <multiple_inputs>` at once. Every operation (union, difference, intersection, and so forth) accepts an arbitrary number of inputs, and each input can be of any size.


=========================================
Compression characteristics of ``starch``
=========================================

Foo bar baz!

.. _independent_testing:

=====================================
Independent testing with GROK toolkit
=====================================

Foo bar baz!

.. |note_png| image:: assets/note.png
.. |intersect_note| replace:: replacement

It is our understanding that the BEDTools' intersectBed program was modified to accept (optionally) sorted data for improved performance some time after these results were published.

A more recent study suggests bedops --intersect still has better memory and running time performance characteristics than recent versions of BEDTools.
