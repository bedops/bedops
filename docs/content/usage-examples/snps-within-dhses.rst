.. _finding_elements_within_elements:

Finding the subset of SNPs within DHSes
=======================================

In this example, we would like to identify the set of SNPs that are within a DHS, printing out both the SNP element *and* the DHS it is contained within.

===================
BEDOPS tools in use
===================

We use :ref:`bedmap` to answer this question, as it traverses a *reference* BED file (in this example, SNPs), and identifies overlapping elements from the *mapping* BED file (in this example, DHSs).

======
Script
======

SNPs are in a BED-formatted file called ``SNPs.bed`` sorted lexicographically with :ref:`sort-bed`. The DNase-hypersensitive sites are stored in a sorted BED-formatted file called ``DHSs.bed``. These two files are available in the :ref:`snps_within_dhses_downloads` section.

::

  bedmap --skip-unmapped --echo --echo-map SNPs.bed DHSs.bed \
    > subsetOfSNPsWithinAssociatedDHS.bed

==========
Discussion
==========

The output of this :ref:`bedmap` statement might look something like this:

::

  chr1    10799576    10799577    rs12.4.398  Systolic_blood_pressure Cardiovascular|chr1 10799460    10799610    MCV-1   9.18063

The output is delimited by pipe symbols (``|``), showing the reference element (SNP) and the mapped element (DHS). 

If multiple elements are mapped onto a single reference element, the mapped elements are further separated by semicolons, by default.

.. _snps_within_dhses_downloads:

=========
Downloads
=========

* :download:`SNP <../../assets/usage-examples/Frequencies-SNPs.bed.starch>` elements
* :download:`DNase-hypersensitive <../../assets/usage-examples/Frequencies-DHSs.bed.starch>` elements

The :ref:`bedmap` tool can operate directly on Starch-formatted archives. Alternatively, use the :ref:`unstarch` tool to decompress Starch data files to sorted BED format.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
