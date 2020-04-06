.. _starch_diff:

`starch-diff`
=============

This tool allows the end user to quickly compare the compressed chromosomes in two or more v2.2+ Starch archives.

==================
Inputs and outputs
==================

-----
Input
-----

The `starch-diff` utility takes in two or more Starch v2.2+ archives as input. The end user may also add `--chr <chr>` to compare one chromosome directly; otherwise, all chromosomes in specified archives are compared.

------
Output
------

The typical output of `starch-diff` is a message indicating the archives' chromosome(s) are identical or dissimilar. 

In addition, if the chromosomes are identical, `starch-diff` exits with a zero status code. Likewise, if any chromosomes are dissimilar, `starch-diff` exits with a non-zero status code.

============
Requirements
============

If the user passes in a pre-v2.2 archive, the utility will exit with a fatal error.

=====
Usage
=====

Use the ``--help`` option to list all options:

::

    starch-diff
      citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
      version:  2.4.39
      authors:  Alex Reynolds and Shane Neph

      $ starch-diff [ --chr <chr> ] starch-file-1 starch-file-2 [ starch-file-3 ... ]

      The 'starch-diff' utility compares the signatures of two or more specified 
      Starch v2.2+ archives for all chromosomes, or for a specified chromosome.


.. role:: bash(code)
   :language: bash
