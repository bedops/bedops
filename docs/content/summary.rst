.. _summary:

Summary
=======

These tables summarize BEDOPS utilities by option, file inputs and BED column requirements.

=======================================
Statistical and set operation utilities
=======================================

----------
bedextract
----------

* Efficiently extracts features from BED input.
* BEDOPS :ref:`bedextract` documentation

+-------------------------+----------------------------------------------------------------------+------------------+------------------+------------------+
| option                  | description                                                          | min. file inputs | max. file inputs | min. BED columns |
+=========================+======================================================================+==================+==================+==================+
| ``--list-chr``          | Print every chromosome found in ``input.bed``                        | 1                | 1                | 3                |
+-------------------------+----------------------------------------------------------------------+------------------+------------------+------------------+
| ``<chromosome>``        | Retrieve all rows for specified chromosome, *e.g.* ``bedextract chr8 | 1                | 1                | 3                |
|                         | input.bed``                                                          |                  |                  |                  |
+-------------------------+----------------------------------------------------------------------+------------------+------------------+------------------+
| ``<query> <reference>`` | Grab elements of query that overlap elements in reference. Same as   | 2                | 2                | 3                |
|                         | ``bedops -e -1 query reference``, except that this option fails when |                  |                  |                  |
|                         | ``query`` contains fully-nested BED elements. May use ``-`` to       |                  |                  |                  |
|                         | indicate ``stdin`` for ``reference`` only.                           |                  |                  |                  |
+-------------------------+----------------------------------------------------------------------+------------------+------------------+------------------+

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
