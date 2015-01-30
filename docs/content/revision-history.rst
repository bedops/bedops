.. _revision_history:

Revision history
================

This page summarizes some of the more important changes between releases.

.. _revision_history_of_current_version:

===============
Current version
===============

------
v2.4.6
------

Released: **TBD**

* :ref:`convert2bed` fixes and improvements
  
  * Added support for conversion of the `GVF file format <http://www.sequenceontology.org/resources/gvf.html#summary>`_, including wrapper scripts and unit tests.

  * Fixed bug in string copy of zero-length element attribute for :ref:`gff2bed` and :ref:`gtf2bed` (GFF and GTF) formats

* General fixes and improvements

  * Fixed possibly corrupt Jansson tarball (thanks to rekado, Shane N. and Richard S.)

  * Fixed typo in :ref:`bedextract <bedextract>` documentation

  * Fixed broken image in :ref:`Overview <overview>`

  * Removed 19 MB ``_build`` intermediate result directory (which should improve overall ``git clone`` time considerably!)

=================
Previous versions
=================

------
v2.4.6
------

Released: **January 28, 2015**

* ``convert2bed`` improvements

  * Addition of RepeatMasker annotation output (``.out``) file conversion support, ``rmsk2bed`` and ``rmsk2starch`` wrappers, and unit tests

------
v2.4.4
------

Released: **January 25, 2015**

* Documentation improvements

  * Implemented substantial style changes via `A Better Sphinx Theme <http://github.com/irskep/sphinx-better-theme>`_ and various customizations. We also include responsive web style elements to help improve browsing on mobile devices.

  * Fixes to typos in conversion and other documents.

------
v2.4.3
------

Released: **December 18, 2014**

* Compilation improvements

  * Shane Neph put in a great deal of work to enable parallel builds (*e.g.*, ``make -j N`` to build various targets in parallel). Depending on the end user's environment, this can speed up compilation time by a factor of 2, 4 or more.

  * Fixed numerous compilation warnings of debug builds of :ref:`starch` toolkit under RHEL6/GCC and OS X 10.10.1/LLVM.

* New :ref:`bedops` features

  * Added ``--chop`` and ``--stagger`` options to "melt" inputs into contiguous or staggered disjoint regions of equivalent size.

  * For less confusion, arguments for ``--element-of``, ``--chop`` and other ``bedops`` operations that take numerical modifiers no longer require a leading hyphen character. For instance, ``--element-of 1`` is now equivalent to the former usage of ``--element-of -1``.

* New :ref:`bedmap` features

  * The ``--sweep-all`` option reads through the entire map file without early termination and can help deal with ``SIGPIPE`` errors. It adds to execution time, but the penalty is not as severe as with the use of ``--ec``. Using ``--ec`` alone will enable error checking, but will now no longer read through the entire map file. The ``--ec`` option can be used in conjunction with ``--sweep-all``, with the associated time penalties. (Another method for dealing with issue this is to override how ``SIGPIPE`` errors are caught by the interpreter (bash, Python, etc.) and retrapping them or ignoring them. However, it may not a good idea to do this as other situations may arise in production pipelines where it is ideal to trap and handle all I/O errors in a default manner.)

  * New ``--echo-ref-size`` and ``--echo-ref-name`` operations report genomic length of reference element, and rename the reference element in ``chrom:start-end`` (useful for labeling rows for input for ``matrix2png`` or ``R`` or other applications).

* :ref:`bedextract`

  * Fixed upper bound bug that would cause incorrect output in some cases

* :ref:`conversion scripts <conversion_scripts>`

  * Brand new C99 binary called :ref:`convert2bed`, which wrapper scripts (``bam2bed``, etc.) now call. No more Python version dependencies, and the C-based rewrite offers massive performance improvements over old Python-based scripts.

  * Added :ref:`parallel_bam2starch` script, which parallelizes creation of :ref:`Starch <starch_specification>` archive from very large BAM files in SGE environments.

  * Added bug fix for missing code in :ref:`starchcluster.gnu_parallel <starchcluster>` script, where the final collation step was missing.

  * The :ref:`vcf2bed` script now accepts the ``--do-not-split`` option, which prints one BED element for all alternate alleles.

* :ref:`Starch <starch_specification>` archival format and compression/extraction tools

  * Added duplicate- and :ref:`nested-element <nested_elements>` flags in v2.1 of Starch metadata, which denote if a chromosome contains one or more duplicate and/or nested elements. BED files compressed with :ref:`starch` v2.5 or greater, or Starch archives updated with :ref:`starchcat` v2.5 or greater will include these values in the archive metadata. The :ref:`unstarch` extraction tool offers ``--has-duplicate`` and ``--has-nested`` options to retrieve these flag values for a specified chromosome (or for all chromosomes).

  * Added ``--is-starch`` option to :ref:`unstarch` to test if specified input file is a Starch v1 or v2 archive.
 
  * Added bug fix for compressing BED files with :ref:`starch`, where the archive would not include the last element of the BED input, if the BED input lacked a trailing newline. The compression tools now include a routine for capturing the last line, if there is no newline.

* Documentation improvements

  * Remade some image assets throughout the documents to support Retina-grade displays

------
v2.4.2
------

Released: **April 10, 2014**

* :ref:`conversion scripts <conversion_scripts>`

  * Added support for :ref:`sort-bed` ``--tmpdir`` option to conversion scripts, to allow specification of alternative temporary directory for sorted results when used in conjunction with ``--max-mem`` option.

  * Added support for GFF3 files which include a FASTA directive in ``gff2bed`` and ``gff2starch`` (thanks to Keith Hughitt).

  * Extended support for Python-based conversion scripts to support use with Python v2.6.2 and forwards, except for ``sam2bed`` and ``sam2starch``, which still require Python v2.7 or greater (and under Python3).

  * Fixed ``--insertions`` option in :ref:`vcf2bed` to now report a single-base BED element (thanks to Matt Maurano).

------
v2.4.1
------

Released: **February 26, 2014**

* :ref:`bedmap`

  * Added ``--fraction-both`` and ``--exact`` (``--fraction-both 1``) to list of compatible overlap options with ``--faster``.

  * Added 5% performance improvement with `bedmap` operations without ``--faster``.

  * Fixed scenario that can yield incorrect results (cf. `Issue 43 <https://github.com/bedops/bedops/issues/43>`_).

* :ref:`sort-bed`

  * Added ``--tmpdir`` option to allow specification of an alternative temporary directory, when used in conjunction with ``--max-mem`` option. This is useful if the host operating system's standard temporary directory (*e.g.*, ``/tmp`` on Linux or OS X) does not have sufficient space to hold intermediate results.

* All :ref:`conversion scripts <conversion_scripts>`

  * Improvements to error handling in Python-based conversion scripts, in the case where no input is specified.

  * Fixed typos in :ref:`gff2bed` and :ref:`psl2bed` documentation (cf. `commit a091e18 <https://github.com/bedops/bedops/commit/a091e18>`_).

* OS X compilation improvements

  * We have completed changes to the OS X build process for the remaining half of the BEDOPS binaries, which now allows direct, full compilation with Clang/LLVM (part of the Apple Xcode distribution). 

    All OS X BEDOPS binaries now use Apple's system-level C++ library, instead of GNU's ``libstdc++``. It is no longer required (or recommended) to use GNU gcc to compile BEDOPS on OS X.

    Compilation is faster and simpler, and we can reduce the size and complexity of Mac OS X builds and installer packages. By using Apple's C++ library, we also eliminate the likelihood of missing library errors. 

    In the longer term, this gets us closer to moving BEDOPS to using the CMake build system, to further abstract and simplify the build process.

* Cleaned up various compilation warnings found with ``clang``/``clang++`` and GCC kits.

------
v2.4.0
------

Released: **January 9, 2014**

* :ref:`bedmap`

  * Added new ``--echo-map-size`` and ``--echo-overlap-size`` options to calculate sizes of mapped elements and overlaps between mapped and reference elements.

  * Improved performance for all ``--echo-map-*`` operations.

  * Updated documentation.

* Major enhancements and fixes to :ref:`sort-bed`:

  * Improved performance.

  * Fixed memory leak.

  * Added support for millions of distinct chromosomes.

  * Improved internal estimation of memory usage with ``--max-mem`` option.

* Added support for compilation on Cygwin (64-bit). Refer to the :ref:`installation documentation <installation_via_source_code_on_cygwin>` for build instructions.

* :ref:`starchcat`

  * Fixed embarassing buffer overflow condition that caused segmentation faults on Ubuntu 13. 

* All :ref:`conversion scripts <conversion_scripts>`

  * Python-based scripts no longer use temporary files, which reduces file I/O and improves performance. This change also reduces the need for large amounts of free space in a user's ``/tmp`` folder, particularly relevant for users converting multi-GB BAM files.

  * We now test for ability to locate ``starch``, ``sort-bed``, ``wig2bed_bin`` and ``samtools`` in user environment, quitting with the appropriate error state if the dependencies cannot be found.

  * Improved documentation. In particular, we have added descriptive tables to each script's documentation page which describe how columns map from original data input to BED output.

  * :ref:`bam2bed` and :ref:`sam2bed`

    * Added ``--custom-tags <value>`` command-line option to support a comma-separated list of custom tags (cf. `Biostars discussion <http://www.biostars.org/p/87062/>`_), *i.e.*, tags which are not part of the original SAMtools specification.

    * Added ``--keep-header`` option to preserve header and metadata as BED elements that use ``_header`` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`vcf2bed`

    * Added new ``--snvs``, ``--insertions`` and ``--deletions`` options that filter VCF variants into three separate subcategories.

    * Added ``--keep-header`` option to preserve header and metadata as BED elements that use ``_header`` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`gff2bed`

    * Added ``--keep-header`` option to preserve header and metadata as BED elements that use ``_header`` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`psl2bed`

    * Added ``--keep-header`` option to preserve header and metadata as BED elements that use ``_header`` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`wig2bed`

    * Added ``--keep-header`` option to :ref:`wig2bed` binary and ``wig2bed``/``wig2starch`` wrapper scripts, to preserve header and metadata as BED elements that use ``_header`` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

* Added OS X uninstaller project to allow end user to more easily remove BEDOPS tools from this platform.

* Cleaned up various compilation warnings found with ``clang``/``clang++`` and GCC kits.

------
v2.3.0
------

Released: **October 2, 2013**

* Migration of BEDOPS code and documentation from Google Code to Github.

  * Due to changes with Google Code hosting policies at the end of the year, we have decided to change our process for distributing code, packages and documentation. While most of the work is done, we appreciate feedback on any problems you may encounter. Please email us at `bedops@stamlab.org <mailto:bedops@stamlab.org>`_ with details.

  * Migration to Github should facilitate requests for code by those who are familiar with ``git`` and want to fork our project to submit `pull requests <https://help.github.com/articles/using-pull-requests>`_.

* :ref:`bedops`

  * General ``--ec`` performance improvements.

* :ref:`bedmap`

  * Adds support for the new ``--skip-unmapped`` option, which filters out reference elements which do not have mapped elements associated with them. See the end of the :ref:`score operations <bedmap_score_operations>` section of the :ref:`bedmap` documentation for more detail.

  * General ``--ec`` performance improvements.

* :ref:`starch`

  * Fixed bug with :ref:`starch` where zero-byte BED input (*i.e.*, an "empty set") created a truncated and unusable archive. We now put in a "dummy" chromosome for zero-byte input, which :ref:`unstarch` can now unpack. 

    This should simplify error handling with certain pipelines, specifically where set or other BEDOPS operations yield an "empty set" BED file that is subsequently compressed with :ref:`starch`.

* :ref:`unstarch`

  * Can now unpack zero-byte ("empty set") compressed :ref:`starch` archive (see above).

  * Changed ``unstarch --list`` option to print to ``stdout`` stream (this was previously sent to ``stderr``).

* :ref:`starch` metadata library

  * Fixed array overflow bug with BEDOPS tools that take :ref:`starch <starch_specification>` archives as inputs, which affected use of archives as inputs to :ref:`closest-features`, :ref:`bedops` and :ref:`bedmap`.

* All :ref:`conversion scripts <conversion_scripts>`

  * Python scripts require v2.7+ or greater.

  * Improved (more "Pythonic") error code handling.

  * Disabled support for ``--max-mem`` sort parameter until :ref:`sort-bed` `issue <https://github.com/bedops/bedops/issues/1>`_ is resolved. Scripts will continue to sort, but they will be limited to available system memory. If you are processing files larger than system memory, please contact us at `bedops@stamlab.org <mailto:bedops@stamlab.org>`_ for details of a temporary workaround.

* :ref:`gff2bed` conversion script

  * Resolved ``IndexError`` exceptions by fixing header support, bringing script in line with `v1.21 GFF3 spec <http://www.sequenceontology.org/gff3.shtml>`_.

* :ref:`bam2bed` and :ref:`sam2bed` conversion scripts

  * Rewritten ``bam2*`` and ``sam2*`` scripts from ``bash`` into Python (v2.7+ support).

  * Improved BAM and SAM input validation against the `v1.4 SAM spec <http://samtools.sourceforge.net/SAMv1.pdf>`_.

  * New ``--split`` option prints reads with ``N`` CIGAR operations as separated BED elements.

  * New ``--all-reads`` option prints all reads, mapped and unmapped.

* :ref:`bedextract`

  * Fixed ``stdin`` bug with :ref:`bedextract`.

* New documentation via `readthedocs.org <readthedocs.org>`_.

  * Documentation is now part of the BEDOPS distribution, instead of being a separate download.

  * We use `readthedocs.org <readthedocs.org>`_ to host indexed and searchable HTML. 

  * `PDF and eBook <https://readthedocs.org/projects/bedops/downloads/>`_ documents are also available for download.

  * Documentation is refreshed and simplified, with new installation and compilation guides.

* OS X compilation improvements

  * We have made changes to the OS X build process for half of the BEDOPS binaries, which allows direct compilation with Clang/LLVM (part of the Apple Xcode distribution). Those binaries now use Apple's system-level C++ library, instead of GNU's ``libstdc++``. 

    This change means that we require Mac OS X 10.7 ("Lion") or greater |---| we do not support 10.6 at this time.

    Compilation is faster and simpler, and we can reduce the size and complexity of Mac OS X builds and installer packages. By using Apple's C++ library, we also reduce the likelihood of missing library errors. When this process is completed for the remaining binaries, it will no longer be necessary to install GCC 4.7+ (by way of MacPorts or other package managers) in order to build BEDOPS on OS X, nor will we have to bundle ``libstdc++`` with the installer.

-------
v2.2.0b
-------

* Fixed bug with OS X installer's post-installation scripts.

------
v2.2.0
------

Released: **May 22, 2013**

* Updated packages

  * Precompiled packages are now available for Linux (32- and 64-bit) and Mac OS X 10.6-10.8 (32- and 64-bit) hosts.

* :ref:`Starch v2 test suite <starch_specification>`

  * We have added a test suite for the Starch archive toolkit with the source download. Test inputs include randomized BED data generated from chromosome and bounds data stored on UCSC servers as well as static FIMO search results. Tests put :ref:`starch`, :ref:`unstarch` and :ref:`starchcat` through various usage scenarios. Please refer to the Starch-specific Makefiles and the test target and subfolder's `README` doc for more information.

* :ref:`starchcat`

  * Resolves bug with ``--gzip`` option, allowing updates of ``gzip`` -backed v1.2 and v1.5 archives to the :ref:`v2 Starch format <starch_specification>` (either ``bzip2`` - or ``gzip`` -backed).

* :ref:`unstarch`

  * Resolves bug with extraction of :ref:`Starch <starch>` archive made from BED files with four or more columns. A condition where the total length of additional columns exceeds a certain number of characters would result in extracted data in those columns being cut off. As an example, this could affect Starch archives made from the raw, uncut output of GTF- and GFF- :ref:`conversion scripts <conversion_scripts>`.

* :ref:`conversion scripts <conversion_scripts>`

  * We have partially reverted :ref:`wig2bed`, providing a Bash shell wrapper to the original C binary. This preserves consistency of command-line options across the conversion suite, while making use of the C binary to recover performance lost from the Python-based v2.1 revision of :ref:`wig2bed` (which at this time is no longer supported). (Thanks to Matt Maurano for reporting this issue.)

------
v2.1.1
------

Released: **May 3, 2013**

* :ref:`bedmap`

  * Major performance improvements made in v2.1.1, such that current :ref:`bedmap` now operates as fast or faster than the v1.2.5 version of :ref:`bedmap`!

* :ref:`bedops`

  * Resolves bug with ``--partition`` option.

* :ref:`conversion scripts <conversion_scripts>`

  * All v2.1.0 Python-based scripts now include fix for ``SIGPIPE`` handling, such that use of ``head`` or other common UNIX utilities to process buffered standard output no longer yields ``IOError`` exceptions. (Thanks to Matt Maurano for reporting this bug.)

* 32-bit Linux binary support

  * Pre-built Linux binaries are now available for end users with 32-bit workstations.

Other issues fixed:

* Jansson tarball no longer includes already-compiled libraries that could potentially interfere with 32-bit builds.

* Minor changes to conversion script test suite to exit with useful error code on successful completion of test.

------
v2.1.0
------

Released: **April 22, 2013**

* :ref:`bedops`

  * New ``--partition`` operator efficiently generates disjoint segments made from genomic boundaries of all overlapping inputs.

* :ref:`conversion scripts <conversion_scripts>`

  * All scripts now use :ref:`sort-bed` behind the scenes to output sorted BED output, ready for use with BEDOPS utilities. It is no longer necessary to pipe data to or otherwise post-process converted data with :ref:`sort-bed`.

  * New :ref:`psl2bed` conversion script, converting `PSL-formatted UCSC BLAT output <http://genome.ucsc.edu/FAQ/FAQformat.html#format2>`_ to BED.

  * New :ref:`wig2bed` conversion script written in Python.

  * New ``*2starch`` :ref:`conversion scripts <conversion_scripts>` offered for all ``*2bed`` scripts, which output Starch v2 archives.

* :ref:`closest-features`

  * Replaced ``--shortest`` option name with ``--closest``, for clarity. (Old scripts which use ``--shortest`` will continue to work with the deprecated option name for now. We advise editing pipelines, as needed.)

* :ref:`starch`

  * Improved error checking for interleaved records. This also makes use of ``*2starch`` conversion scripts with the ``--do-not-sort`` option safer.

* Improved Mac OS X support

  * New Mac OS X package installer makes installation of BEDOPS binaries and scripts very easy for OS X 10.6 - 10.8 hosts.

  * Installer resolves fatal library errors seen by some end users of older OS X BEDOPS releases.

-------
v2.0.0b
-------

Released: **February 19, 2013**

* Added :ref:`starchcluster` script variant which supports task distribution with `GNU Parallel <http://www.gnu.org/software/parallel/>`_.

* Fixed minor problem with :ref:`bam2bed` and :ref:`sam2bed` conversion scripts.

-------
v2.0.0a
-------

Released: **February 7, 2013**

* :ref:`bedmap`

  * Takes in Starch-formatted archives as input, as well as raw BED (i.e., it is no longer required to extract a Starch archive to an intermediate, temporary file or named pipe before applying operations).

  * New ``--chrom`` operator jumps to and operates on information for specified chromosome only.

  * New ``--echo-map-id-uniq`` operator lists unique IDs from overlapping mapping elements.

  * New ``--max-element`` and ``--min-element`` operators return the highest or lowest scoring overlapping map element.

* :ref:`bedops`

  * Takes in Starch-formatted archives as input, as well as raw BED.

  * New ``--chrom`` operator jumps to and operates on information for specified chromosome only.

* :ref:`closest-features`

  * Takes in Starch-formatted archives as input, as well as raw BED.

  * New ``--chrom`` operator jumps to and operates on information for specified chromosome only.

* :ref:`sort-bed` and ``bbms``

  * New ``--max-mem`` option to limit system memory on large BED inputs.

  * Incorporated ``bbms`` functionality into :ref:`sort-bed` with use of ``--max-mem`` operator.

* :ref:`starch`, :ref:`starchcat` and :ref:`unstarch`

  * New metadata enhancements to Starch-format archival and extraction, including: ``--note``, ``--elements``, ``--bases``, ``--bases-uniq``, ``--list-chromosomes``, ``--archive-timestamp``, ``--archive-type`` and ``--archive-version`` (see ``--help`` to :ref:`starch`, :ref:`starchcat` and :ref:`unstarch` binaries, or view the documentation for these applications for more detail).

  * Adds 20-35% performance boost to creating Starch archives with :ref:`starch` utility.

  * New documentation with technical overview of the Starch format specification.

* :ref:`conversion scripts <conversion_scripts>`

  * New :ref:`gtf2bed` conversion script, converting GTF (v2.2) to BED.

* Scripts are now part of main download; it is no longer necessary to download the BEDOPS companion separately.

-------
v1.2.5b
-------

Released: **January 14, 2013**

* Adds support for Apple 32- and 64-bit Intel hardware running OS X 10.5 through 10.8.

* Adds ``README`` for companion download.

* Removes some obsolete code.

------
v1.2.5
------

Released: **October 13, 2012**

* Fixed unusual bug with :ref:`unstarch`, where an extra (and incorrect) line of BED data can potentially be extracted from an archive.

* Updated companion download with updated :ref:`bam2bed` and :ref:`sam2bed` conversion scripts to address 0-indexing error with previous revisions.

------
v1.2.3
------

Released: **August 17, 2012**

* Added ``--indicator`` option to :ref:`bedmap`.

* Assorted changes to conversion scripts and associated companion download.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
