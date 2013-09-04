Working with many input files at once with ``bedops`` and ``bedmap``
====================================================================

BEDOPS is designed to work with as many input files at once as you need, either through the :ref:`bedops` program, or through a combined use of that program with others in the suite.

==========
Discussion
==========

Say we have five input BED files (``A``, ``B``, ``C``, ``D``, ``E``), and I need to identify those regions where any two (or more) of the input files (``{A,B}``, ``{A,C}``, ``{A,D}``, ``{A,E}``, ``{B,C}``, ...) overlap reciprocally by 30% or more.

One concrete application may be where we have multiple biological replicates, and we take any repeatable result (in two or more inputs, in this case) as true signal. Similarly, we might be interested in a problem like this if we have multiple related (or even unrelated) cell type samples and we want to be confident in peak calls for DNaseI sequencing of ChIP-seq experiments.

These sorts of problems often have efficient solutions in BEDOPS. Here, the solution is independent of how many inputs we start with, what overlap criteria we use, and whether the requirement calls for two or more files of overlap (or whether it is 4 or more files in the overlap, or 9, or whatever).

Consider a case study of one such problem that utilizes both :ref:`bedops` and :ref:`bedmap` together to create an efficient solution.
