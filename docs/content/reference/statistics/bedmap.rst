.. _bedmap:

`bedmap`
========

The ``bedmap`` program is used to retrieve and process signal or other features over regions of interest in BED files (including DNase hypersensitive regions, SNPs, transcription factor binding sites, etc.), performing tasks such as: :ref:`smoothing raw tag <smoothing_raw_tags>` count signal in preparation for uploading to the UCSC Genome Browser, :ref:`finding subsets of elements <finding_elements_within_elements>` within a larger coordinate set, :ref:`filtering multiple BED files <master_list>` by signal, :ref:`finding multi-input overlap <multiple_inputs>` solutions, and much, much more.

==================
Inputs and outputs
==================

-----
Input
-----

The :ref:`bedmap` program takes in *reference* and *mapping* ﬁles and calculates statistics for each reference element. These calculations |---| *operations* |---| are applied to overlapping elements from the mapped ﬁle:

.. image:: ../../../assets/reference/statistics/reference_bedmap_inputs.png
   :width: 75%

The :ref:`bedmap` program requires files in a relaxed variation of the BED format as described by `UCSC's browser documentation <http://genome.ucsc.edu/FAQ/FAQformat.html#format1>`_. The chromosome field can be any non-empty string, the score field can be any valid numeric value, and information is unconstrained beyond the minimum number of columns required by the chosen options.

Alternatively, :ref:`bedmap` can accept :ref:`Starch-formatted archives <starch>` of BED data as input |---| it is no longer necessary to extract Starch archive data to intermediate BED files!

Support for common headers (including UCSC browser track headers) is available with the ``--header`` option, although headers are stripped from output.

Most importantly, :ref:`bedmap` expects :ref:`sorted <sort-bed>` inputs. You can use the BEDOPS :ref:`sort-bed` program to ensure your inputs are properly sorted. 

.. note:: You only need to sort once, and only if your input data are unsorted, as all BEDOPS tools take in and export sorted BED data.

Operations are applied over map elements that overlap the coordinates of each reference element. You can use the default overlap criterion of one base, or define your own criteria using the overlap criteria operators.

Once you have overlapping elements, you can either perform numerical calculations on their scores or return identifiers or other non-score information. Additional modifier operators allow customization of how output is presented, to assist with downstream processing in a pipeline setting.

------
Output
------

Depending on specified options, the :ref:`bedmap` program can send a variety of delimited information about the reference and mapped elements (as well as analytical results) to standard output. If the ``--echo`` option is used, the output will be at least a three-column BED file. The use of predictable delimiters (which are customizable) and the use of UNIX-like standard streams allows easy downstream analysis or post-processing with other tools and scripts.

==========
Operations
==========

----------------
Overlap criteria
----------------

----------------
Score operations
----------------

--------------------
Non-score operations
--------------------

---------
Modifiers
---------

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
