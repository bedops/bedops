.. _distance_frequencies:

Measuring the frequency of signed distances between SNPs and nearest DHSes
==========================================================================

In this example, we would like to find the **signed** distance between a single nucleotide repeat and the DNase-hypersensitive site nearest to it, as measured in base pairs (bp).

===================
BEDOPS tools in use
===================

To find nearest elements, we will use :ref:`closest-features` with the ``--dist``, ``--closest``, and ``--no-ref`` options.

======
Script
======

SNPs are in a BED-formatted file called ``SNPs.bed`` sorted lexicographically with :ref:`sort-bed`. The DNase-hypersensitive sites are stored in a sorted BED-formatted file called ``DHSs.bed``. These two files are available in the :ref:`distance_frequencies_downloads` section.

::

  # author : Eric Rynes
  closest-features --dist --closest --no-ref SNPs.bed DHSs.bed \
      | cut -f2 -d '|' \
      | grep -w -F -v -e "NA" \
      > answer.bed

==========
Discussion
==========

The ``--dist`` option returns signed distances between input elements and reference elements, ``--closest`` chooses the single closest element, and ``--no-ref`` keeps SNP coordinates from being printed out.

The output from :ref:`closest-features` contains coordinates and the signed distance to the closest DHS, separated by the pipe (``|``) character. Such output might look something like this:

::

  chr1    2513240 2513390 MCV-11  97.201400|25

This type of result is chopped up with the standard UNIX utility ``cut`` to get at the distances to the closest elements. Finally, we use ``grep -v`` to throw out any non-distance, denoted by ``NA``. This can occur if there exists some chromosome in the SNP dataset that does not exist in the DHSs.

Thus, for every SNP, we have a corresponding distance to nearest DHS. As an example, from this data we could build a histogram showing the frequencies of distances-to-nearest-DHS.

.. _distance_frequencies_downloads:

=========
Downloads
=========

* :download:`SNP <../../assets/usage-examples/Frequencies-SNPs.bed.starch>` elements
* :download:`DNase-hypersensitive <../../assets/usage-examples/Frequencies-DHSs.bed.starch>` elements

The :ref:`closest-features` tool can operate directly on Starch-formatted archives. Alternatively, use the :ref:`unstarch` tool to decompress Starch data files to sorted BED format.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
