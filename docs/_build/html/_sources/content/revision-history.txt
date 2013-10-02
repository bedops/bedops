.. _revision_history:

Revision history
================

This page summarizes some of the more important changes between releases.

===============
Current version
===============

------
v2.3.0
------

Released: **October 1, 2013**

* General ``-ec`` performance improvements in :ref:`bedops`, :ref:`bedmap` and other applications.

* :ref:`bedmap`

  * Adds experimental support for ``--skip-unmapped`` option, which filters out reference elements which do not have mapped elements associated with them.

* :ref:`starch`

  * Fixed bug with :ref:`starch` where zero-byte BED input created a truncated and unusable archive. We now put in a "dummy" chromosome for zero-byte input, which :ref:`unstarch` can now unpack. This should simplify error handling with certain pipelines.

* :ref:`unstarch`

  * Can unpack zero-byte compressed :ref:`starch` archive (see above).
  * Changed ``unstarch --list`` option to print to ``stdout`` stream (previously sent to ``stderr``).

* :ref:`starch` metadata library

  * Fixes array overflow bug with BEDOPS tools that take :ref:`starch <starch_specification>` archives as inputs, which affected :ref:`closest-features`, :ref:`bedops` and :ref:`bedmap`.

* :ref:`bam2bed` and :ref:`sam2bed` conversion scripts

  * Rewritten ``bam2*`` and ``sam2*`` scripts from ``bash`` into Python (v2.7+).
  * Improved input validation against SAM v1 specification.
  * New ``--split`` option prints reads with 'N' CIGAR operations into separate BED elements.
  * New ``--all-reads`` option prints all reads mapped and unmapped.

* :ref:`bedextract`

  * Fixed ``stdin`` bug with :ref:`bedextract`.

* New documentation via `readthedocs.org <readthedocs.org>`_.

  * Documentation is now part of the BEDOPS distribution, instead of being a separate download. 
  * We use `readthedocs.org <readthedocs.org>`_ to host indexed and searchable HTML, PDF and eBook documents.
  * Documentation is refreshed and simplified, with a new installation/compilation guide.

* OS X compilation improvements

  * We have made changes to the OS X build process for half of the BEDOPS binaries, which allows direct compilation with Clang/LLVM (part of the Apple Xcode distribution). 

    This already makes compilation faster and easier, as well as reduces the size and complexity of Mac OS X builds and installer packages. When this process is complete, it will no longer be necessary to install GCC 4.7+ by way of MacPorts or other package managers, in order to build BEDOPS on OS X.

=================
Previous versions
=================

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

