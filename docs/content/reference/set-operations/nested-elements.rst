.. _nested_elements:

Nested elements
===============

This page describes nested BED elements and their impact on BEDOPS tools.

.. _what_are_nested_elements:

========================
What are nested elements
========================

A *nested element* is defined as a BED element from a sorted BED file, where it has a genomic range that is entirely enclosed by the previous element's range.

More rigorously, we define a basic, three-column BED element :math:`A` with chromosome name :math:`chrN` and range :math:`{[a_1, a_2]}`.

=======================================
Why do nested elements matter in BEDOPS
=======================================