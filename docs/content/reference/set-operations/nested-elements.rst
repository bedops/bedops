.. _nested_elements:

Nested elements
===============

This page describes nested BED elements and their impact on BEDOPS tools.

.. _what_are_nested_elements:

========================
What are nested elements
========================

A *nested element* is defined as a BED element from a sorted BED file, where it has a genomic range that is entirely enclosed by the previous element's range.

More rigorously, we define two BED elements :math:`A` and :math:`B`, both with the chromosome name :math:`chrN` and each with ranges :math:`{(a_{start}, a_{stop}]}` and :math:`{(b_{start}, b_{stop}]}`, respectively, with the conditions :math:`a_{start} < a_{stop}`, :math:`b_{start} < b_{stop}` and the condition :math:`a_{start} < b_{start}`. 

.. note:: Note that the last condition puts elements :math:`A` and :math:`B` into sort order, as applied by :ref:`sort-bed <sort-bed>`. 

=======================================
Why do nested elements matter in BEDOPS
=======================================