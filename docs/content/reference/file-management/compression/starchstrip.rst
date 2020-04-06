.. _starchstrip:

`starchstrip`
=============

The ``starchstrip`` utility efficiently pulls out per-chromosome records contained within a BEDOPS :ref:`Starch-formatted <starch_specification>` archive and writes the filtered result to a new Starch archive. This utility allows either exclusion or inclusion of one or more specified chromosome names.

Previously, it would be necessary to extract records with :ref:`unstarch <unstarch>`, use `awk` or similar to filter down to the desired set of records, and recompress with :ref:`starch <starch>`. In contrast, ``starchstrip`` identifies just the pieces of data of interest within an archive and writes them to a new archive, with an updated metadata payload, avoiding the need for costly and wasteful extraction and re-compression. 

==================
Inputs and outputs
==================

-----
Input
-----

The input to :ref:`starchstrip` consists of a BEDOPS :ref:`Starch-formatted <starch_specification>` archive file, along with the specification of either ``--include`` or ``--exclude`` for inclusion or exlusion of chromosome records from the archive. One or more chromosome names are provided as a comma-separated string. 

.. note:: If the chromosome listing contains chromosome names not in the input archive, they will be ignored.

------
Output
------

The :ref:`starchstrip` tool writes a :ref:`starch` -formatted archive to the standard output stream, which is usually redirected to a regular file. The output contains the same compressed data from the original file (no extraction or recompression is performed) and so preserves the archive version, compression type, and other archive attributes.

.. note:: If the archive's metadata attributes need updating (to gain updated metadata features, for instance, such as data integrity signatures), the :ref:`starchcat <starchcat>` utility should be used to update older archives.

.. note:: If the specified combination of operation and chromosome names would result in output that is identical to the original file, or output that would be an empty file, ``starchstrip`` will exit early with a fatal error.

=====
Usage
=====

Use the ``--help`` option to list all options:

::

  starchstrip
    citation: http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract
    version:  2.4.39 (typical)
    authors:  Alex Reynolds and Shane Neph

  USAGE: starchstrip [ --include | --exclude ] <chromosome-list> <starch-file>
      
      * Add either the --include or --exclude argument to filter the specified
        <starch-file> for chromosomes in <chromosome-list> for inclusion or
        exclusion, respectively. Note that you can only specify either inclusion
        or exclusion.

      * The <chromosome-list> argument is a comma-separated list of chromosome names
        to be included or excluded. This list is a *required* argument to either of the
        two --include and --exclude options.

      * The output is a Starch archive containing those chromosomes specified for inclusion
        or what chromosomes remain after exclusion from the original <starch-file>. A new
        metadata payload is appended to the output Starch archive.

      * The output is written to the standard output stream -- use the output redirection
        operator to write the result to a regular file, e.g.:

          $ starchstrip --exclude chrN in.starch > out.starch

      * Filtering simply copies over raw bytes from the input Starch archive and
        no extraction or recompression is performed. Use 'starchcat' to update the
        metadata, if new attributes are required.

      Process Flags
      --------------------------------------------------------------------------
      --include <chromosome-list>     Include specified chromosomes from <starch-file>.

      --exclude <chromosome-list>     Exclude specified chromosomes from <starch-file>.

      --version                       Show binary version.

      --help                          Show this usage message.

-------
Example
-------

Let's say we have an archive containing 23 chromosomes, one for each of the human genome: ``chr1``, ``chr2``, and so on, to ``chrY``. (To simplify this example, we leave out mitochondrial, random, pseudo- and other chromosomes.) As an example, say we want a new Starch archive that contains chromosomes ``chr4``, ``chr8``, and ``chr17``. We can use ``starchstrip`` to efficiently write out a new archive with just those three chromosomes:

::

  $ starchstrip --include chr4,chr8,chr17 humanGenome.starch > humanGenome.chrs4_8_and_17.starch

The :ref:`starchstrip` utility parses the metadata from the input ``humanGenome.starch`` and uses its details to decide how to write out the subset of chromosomes, along with a metadata payload specific to the three chromosomes. No extraction or recompression is performed; this is as fast as copying just the parts of the file we are interested in.

As a second example, we can instead use the ``--exclude`` operand to copy over all chromosomes *except* those we choose. To continue the example above, we can get the "inverse" of  ``humanGenome.chrs4_8_and_17.starch`` with the following:

::

  $ starchstrip --exclude chr4,chr8,chr17 humanGenome.starch > humanGenome.all_chrs_except_chrs4_8_and_17.starch



.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
