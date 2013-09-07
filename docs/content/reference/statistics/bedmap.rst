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

------
Output
------

Depending on specified options, the :ref:`bedmap` program can send a variety of delimited information about the reference and mapped elements (as well as analytical results) to standard output. If the ``--echo`` option is used, the output will be at least a three-column BED file. The use of predictable delimiters (which are customizable) and the use of UNIX-like standard streams allows easy downstream analysis or post-processing with other tools and scripts.

=====
Usage
=====

The ``--help`` option describes the various mapping and analytical operations and other options available to the end user:

::

  bedmap
    citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
    version:  2.3.0
    authors:  Shane Neph & Scott Kuehn

   USAGE: bedmap [process-flags] [overlap-option] <operation(s)...> <ref-file> [map-file]
       All input files must be sorted per the sort-bed utility.
       The program accepts BED and starch file formats.
       May use '-' for a file to indicate reading from standard input (BED format only).

       Traverse <ref-file>, while applying <operation(s)> on qualified, overlapping elements from
         <map-file>.  Output is one line for each line in <ref-file>, sent to standard output.  There
         is no limit on the number of operations you can specify to compute in one bedmap call.
       If <map-file> is omitted, the given file is treated as both the <ref-file> and <map-file>.
         This usage is more efficient than specifying the same file twice.
       Arguments may be given in any order before the input file(s).

      Process Flags:
       --------
        --chrom <chromosome>  Process data for given <chromosome> only.
        --delim <delim>       Change output delimiter from '|' to <delim> between columns (e.g. '\t').
        --ec                  Error check all input files (slower).
        --faster              (advanced) Strong input assumptions are made.  Review docs before use.
                                Compatible with --bp-ovr and --range overlap options only.
        --header              Accept headers (VCF, GFF, SAM, BED, WIG) in any input file.
        --help                Print this message and exit successfully.
        --multidelim <delim>  Change delimiter of multi-value output columns from ';' to <delim>.
        --prec <int>          Change the post-decimal precision of scores to <int>.  0 <= <int>.
        --sci                 Use scientific notation for score outputs.
        --version             Print program information.
  
  
      Overlap Options (At most, one may be selected.  By default, --bp-ovr 1 is used):
       --------
        --bp-ovr <int>           Require <int> bp overlap between elements of input files.
        --range <int>            Grab <map-file> elements within <int> bp of <ref-file>'s element,
                                   where 0 <= int.  --range 0 is an alias for --bp-ovr 1.
        --fraction-ref <val>     The fraction of the element's size from <ref-file> that must overlap
                                   the element in <map-file>.  Expect 0 < val <= 1.
        --fraction-map <val>     The fraction of the element's size from <map-file> that must overlap
                                   the element in <ref-file>.  Expect 0 < val <= 1.
        --fraction-both <val>    Both --fraction-ref <val> and --fraction-map <val> must be true to
                                   qualify as overlapping.  Expect 0 < val <= 1.
        --fraction-either <val>  Either --fraction-ref <val> or --fraction-map <val> must be true to
                                   qualify as overlapping.  Expect 0 < val <= 1.
        --exact                  Shorthand for --fraction-both 1.  First 3 fields from <map-file> must
                                   be identical to <ref-file>'s element.
  
  
      Operations:  (Any number of operations may be used any number of times.)
       ----------
        SCORE:
         <ref-file> must have at least 3 columns and <map-file> 5 columns.
  
        --cv                The result of --stdev divided by the result of --mean.
        --kth <val>         Generalized median. Report the value, x, such that the fraction <val>
                              of overlapping elements' scores from <map-file> is less than x,
                              and the fraction 1-<val> of scores is greater than x.  0 < val <= 1.
        --mad <mult=1>      The median absolute deviation of overlapping elements in <map-file>.
                              Multiply mad score by <mult>.  0 < mult, and mult is 1 by default.
        --max               The highest score from overlapping elements in <map-file>.
        --max-element       An element with the highest score from overlapping elements in <map-file>.
        --mean              The average score from overlapping elements in <map-file>.
        --median            The median score from overlapping elements in <map-file>.
        --min               The lowest score from overlapping elements in <map-file>.
        --min-element       An element with the lowest score from overlapping elements in <map-file>.
        --stdev             The square root of the result of --variance.
        --sum               Accumulated scores from overlapping elements in <map-file>.
        --tmean <low> <hi>  The mean score from overlapping elements in <map-file>, after
                              ignoring the bottom <low> and top <hi> fractions of those scores.
                              0 <= low <= 1.  0 <= hi <= 1.  low+hi <= 1.
        --variance          The variance of scores from overlapping elements in <map-file>.
  
       ----------
        NON-SCORE:
         <ref-file> must have at least 3 columns.
         For --echo-map-id/echo-map-id-uniq, <map-file> must have at least 4 columns.
         For --echo-map-score, <map-file> must have at least 5 columns.
         For all others, <map-file> requires at least 3 columns.
  
        --bases             The total number of overlapping bases from <map-file>.
        --bases-uniq        The number of distinct bases from <ref-file>'s element covered by
                              overlapping elements in <map-file>.
        --bases-uniq-f      The fraction of distinct bases from <ref-file>'s element covered by
                              overlapping elements in <map-file>.
        --count             The number of overlapping elements in <map-file>.
        --echo              Print each line from <ref-file>.
        --echo-map          List all overlapping elements from <map-file>.
        --echo-map-id       List IDs from all overlapping <map-file> elements.
        --echo-map-id-uniq  List unique IDs from overlapping <map-file> elements.
        --echo-map-range    Print genomic range of overlapping elements from <map-file>.
        --echo-map-score    List scores from overlapping <map-file> elements.
        --indicator         Print 1 if there exists an overlapping element in <map-file>, 0 otherwise.

==========
Operations
==========

.. _bedmap_overlap_criteria:

----------------
Overlap criteria
----------------

.. _bedmap_score_operations:

----------------
Score operations
----------------

.. _bedmap_non_score_operations:

--------------------
Non-score operations
--------------------

.. _bedmap_modifier_operations:

---------
Modifiers
---------

===================================
Per-chromosome operations (--chrom)
===================================

==============
Starch support
==============

==============
Error checking
==============

========
Endlines
========

.. _bedmap_downloads:

=========
Downloads
=========


.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
