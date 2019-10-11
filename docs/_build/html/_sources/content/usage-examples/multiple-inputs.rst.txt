.. _multiple_inputs:

Working with many input files at once with ``bedops`` and ``bedmap``
====================================================================

BEDOPS is designed to work with as many input files at once as you need, either through the :ref:`bedops` program, or through a combined use of that program with others in the suite.

==========
Discussion
==========

Say we have five input BED files (``A``, ``B``, ``C``, ``D``, ``E``), and we need to identify those regions where any two (or more) of the input files (``{A,B}``, ``{A,C}``, ``{A,D}``, ``{A,E}``, ``{B,C}``, ...) overlap reciprocally by 30% or more.

One concrete application may be where we have multiple biological replicates, and we take any repeatable result (in two or more inputs, in this case) as true signal. Similarly, we might be interested in a problem like this if we have multiple related (or even unrelated) cell type samples and we want to be confident in peak calls for DNaseI sequencing of ChIP-seq experiments.

These sorts of problems often have efficient solutions in BEDOPS. Here, the solution is independent of how many inputs we start with, what overlap criteria we use, and whether the requirement calls for two or more files of overlap (or whether it is 4 or more files in the overlap, or 9, or whatever).

Consider a case study of one such problem that utilizes both :ref:`bedops` and :ref:`bedmap` together to create an efficient solution:

::

  $ bedops -u file1.bed file2.bed ... fileN.bed \
      | bedmap --echo --echo-map-id-uniq --fraction-both 0.5 - \
      | awk -F"|" '(split($2, a, ";") > 1)' \
      > answer.bed

Here, we pass in as many files as we have to :ref:`bedops`. The requirement of elements overlapping reciprocally is met by using ``--fraction-both``, and the requirement that overlapping elements must come from two or more (distinct) files is satisfied by checking how many elements there are via the ``--echo-map-id-uniq`` operator.

The requirements for ``file1.bed`` through ``fileN.bed`` are that each is properly :ref:`sorted <sort-bed>` (as expected for any BEDOPS input) and that their respective fourth-column ID fields identify the file. For example:

::

  $ head -2 file1.bed
  chr1 1   50   1  anything-else
  chr1 230 400  1  whatever-you-like

  $ head -2 file2.bed
  chr1 23  78  2  other-fields
  chr1 56  98  2  5.678  +  peak-2

As a nice side-effect, ``answer.bed`` will show from which file each entry originated. If we don't want that extra information, we simply cut it out:

::

  cut -f1-3,5- answer.bed >! my-final-answer.bed

There is also a column that shows exactly which files are part of the per-row intersection. If we don't want that information, then we just cut that:

::

  cut -f1 -d'|' my-final-answer.bed

While this is just one example of how the tools can be used together to answer complicated questions efficiently, it demonstrates why it is worthwhile to learn about the relatively few core programs in BEDOPS. 

If we look at what is required to answer this kind of question using other tool suites, we will quickly find that solutions do not scale to the number of files, nor with the requirement that overlaps must come from *k* or more distinct input files. Even in the simplest case of just requiring the regions overlap in 2 of *n* inputs, we must build on the order of *n*:sup:`2`/2 intermediate files (and sweep through the *n* original inputs *n*:sup:`2` times as well). If our requirement is 3 of *n* inputs, the polynomials increase accordingly. 

*The solution with BEDOPS is far more efficient than this and requires no intermediate results.*
