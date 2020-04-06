.. _revision_history:

Revision history
================

This page summarizes some of the more important changes between releases.

.. _revision_history_of_current_version:

===============
Current version
===============

-------
v2.4.39
-------

Released: **April 6, 2020**

* :ref:`sort-bed <sort-bed>`

  * Patched :code:`--unique` to report output identical to :code:`sort -u`, in order to resolve `Issue 236 <https://github.com/bedops/bedops/issues/236>`_.
  
* :ref:`unstarch <unstarch>`

  * Patched :code:`--is-starch` test option to read only up to, at most, 8kb to check for v2 or v1 (legacy) Starch archive data, to resolve `Issue 209 <https://github.com/bedops/bedops/issues/209>`_.
  
* General

  * Updated main :code:`Makefile` to use `Homebrew <https://brew.sh/>`_ GNU :code:`coreutils` and :code:`findutils` tools on the OS X target. If you build BEDOPS on OS X, you can add these tools with :code:`brew install coreutils findutils`.

=================
Previous versions
=================

-------
v2.4.38
-------

Released: **March 31, 2020**

* :ref:`convert2bed <convert2bed>`

  * Patched segmentation fault in malformed RepeatMasker input conversion. Thanks to `Mark Diekhans <https://github.com/bedops/bedops/issues/235>`_ for the bug report.
  * Patched abort and segmentation faults in malformed GVF, GFF, GTF, and WIG input conversion. Thanks to `Hongxu Chen <https://github.com/bedops/bedops/issues/217>`_ for the bug report.
  * Patched documentation and help message for BAM and SAM conversion. Thanks to `Zhuoer Dong <https://github.com/bedops/bedops/issues/215>`_ for the report.
  * Patched GTF conversion test suite.

* General

  * Updated outdated date information.

-------
v2.4.37
-------

Released: **October 11, 2019**

* :ref:`starchcat <starchcat>`

  * A bug was introduced in v2.4.36 that would cause segmentation faults when concatenating disjoint Starch files, which is fixed in this version. Thanks to Eric Rynes for the bug report.
  * Added a unit test to :code:`tests/starch` to test this particular issue.

* :ref:`bedmap <bedmap>`

  * Running :code:`bedmap --version` now exits with a zero (non-error/success) status.

* :ref:`starch <starch>`

  * When a Starch file with a header is provided as input to :code:`bedops` or :code:`bedmap`, the line is errantly processed as a BED interval. Thanks to `André M. Ribeiro-dos-Santos <https://github.com/bedops/bedops/pull/229>`_ for patching the Starch C++ API to skip headers.
  * Added a unit test to :code:`tests/starch` to test headered Starch mapped against itself.

* General

  * Applied a placeholder workaround to whatever stupid bug was introducted in `Issue 5709 <https://github.com/readthedocs/readthedocs.org/issues/5709>`_ that broke image serving for the document index (front page).
  * Improved speed of generating random intervals in :code:`tests/starch` unit tests.
  
-------
v2.4.36
-------

Released: **May 2, 2019**

* :ref:`bedmap <bedmap>`

  * Resolved an issue preventing use of a :code:`bash` process substitution or Unix named pipe in the reference position: *i.e.*, :code:`bedmap --options <(processToGenerateReferenceElements) map.bed` and similar would issue incorrect output. Thanks to Wouter Meuleman and others for reports and test input.

  * To avoid mapping problems, map elements should not contain spaces in the ID or subsequent non-interval fields. Use of the :code:`--ec` can help identify problems in map input, at the cost of a longer runtime. The documentation is clarified to warn users about avoiding spaces in map input. Thanks to Wouter Meuleman for the report and providing test input.

  * Added :code:`--unmapped-val <val>` option, where :code:`<val>` replaces the empty string output of :code:`--echo-map*` operations when there are no mapped elements. The :code:`--min/max-element` operators will give results as before (the empty string).

* General

  * Reduced :code:`warning: zero as null pointer constant [-Wzero-as-null-pointer-constant]` compiler warnings via Clang.

  * Begun work on a comprehensive test suite for BEDOPS applications. Tests are available via source distribution in :code:`${root}/tests` and can be run by entering :code:`make` in this directory.

-------
v2.4.35
-------

Released: **May 2, 2018**

* :ref:`starch <starch>`

  * When compressing records, if the last interval in the former chromosome is identical to the first interval of the next chromosome, then a test on the sort order of the remainder string of that interval is applied (incorrectly). This is patched to test that chromosome names are identical before applying sort order rules. Thanks to Andrew Nishida for the report and for providing test input.

-------
v2.4.34
-------

Released: **April 26, 2018**

* :ref:`convert2bed <convert2bed>`

  * In `Issue 208 <https://github.com/bedops/bedops/issues/208>`_ builds of :ref:`convert2bed <convert2bed>` would exit with an error state when converting SAM input with newline-delimited records longer than the 5 MB per-thread I/O buffer. The :code:`C2B_THREAD_IO_BUFFER_SIZE` constant is now set to the suite-wide :code:`TOKENS_MAX_LENGTH` value, which should make it easier to compile custom builds of BEDOPS that support very-long line lengths. Thanks to Erich Schwarz for the initial report.

* :ref:`starchstrip <starchstrip>`

  * When `starchstrip` is compiled with a C compiler, :code:`qsort` uses a comparator that works correctly on the input chromosome list. When compiled with a C++ compiler (such as when building the larger BEDOPS toolkit), a different comparator is used that does not make variables of the correct type, and so the :code:`qsort` result is garbage, leading to missing chromosomes. Thanks to Jemma Nelson for the initial report.

-------
v2.4.33
-------

Released: **April 9, 2018**

* :ref:`convert2bed <convert2bed>`

  * Resolved `Issue 207 <https://github.com/bedops/bedops/issues/207>`_ where a megarow build of :ref:`convert2bed <convert2bed>` would raise segmentation faults when converting SAM input. This build uses constants that define a longer BED line length. Arrays ended up using more stack than available, resulting in segmentation faults. This issue could potentially affect conversion of any data with the megarow build of :ref:`convert2bed <convert2bed>`. Arrays using megarow-constants were replaced with heap- or dynamically-allocated pointers. Thanks to Erich Schwarz for the initial report.

-------
v2.4.32
-------

Released: **March 14, 2018**

* New build type (128-bit precision floating point arithmetic, :code:`float128`)

  * A new build type adds support for :code:`long double` or 128-bit floating point operations on measurement values in :ref:`bedmap <bedmap>`, such as is used with score operators like: :code:`--min`, :code:`--max`, :code:`--min-element`, :code:`--max-element`, :code:`--mean`, and so on.

  * This build includes support for measurements on values ranging from approximately |plusminus| 6.48e−4966 to |plusminus| 6.48e4966 (`subnormal <https://en.wikipedia.org/wiki/Denormal_number>`_), or |plusminus| 1.19e-4932 to |plusminus| 1.19e4932 (normal), which enables :ref:`bedmap <bedmap>` to handle, for example, lower p-values without log- or other transformation preprocessing steps. The article on `quadruple precision <https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format>`_ can be useful for technical review.

  * For comparison, the current "non-float128" typical and megarow builds allow measurements on values from approximately |plusminus| 5e−324 to |plusminus| 5e324 (subnormal) or |plusminus| 2.23e-308 to |plusminus| 2.23e308 (normal). Please refer to the article on `double precision <https://en.wikipedia.org/wiki/Double-precision_floating-point_format>`_ for more technical detail.

  * Please use :code:`make float128 && make install_float128` to install this build type.

  * This build type combines support for quadruple, 128-bit precision floats with the :code:`typical` build type for handling "typical" BED4+ style line lengths. At this time, "megarow" support is not enabled with higher precision floats.

  * This build will use more memory to store floating-point values with higher precision, and processing those data will require more computation time. It is recommended that this build be used only if analyses require a higher level of precision than what the :code:`double` type allows.

* OS X (Darwin) megarow build

  * Some applications packaged in the OS X installer or compiled via the OS X command-line developer toolkit lacked `megarow <http://bedops.readthedocs.io/en/latest/content/revision-history.html#v2-4-27>`_ build support, despite those flags being specified in the parent Makefile. These applications specifically were affected: :code:`bedextract`, :code:`bedmap`, and :code:`convert2bed`. These binaries rely on wider suite-wide constants and data types that the megarow build variety specifies. The Darwin-specific Makefiles have been fixed to resolve this build issue, so that all OS X BEDOPS binaries should now be able to compile in the correct megarow-specific settings.

-------
v2.4.31
-------

Released: **March 8, 2018**

* User forum

  * BEDOPS user forum moved domains from http://bedops.stamlab.org to https://bedops.altius.org

  * Testing out administrator approval requirement for new forum accounts, to help try to reduce visits from spammers.

* Documentation

  * Updated Homebrew installation instructions per `issue 202 <https://github.com/bedops/bedops/issues/202>`_ (thanks to user EricFromCanada).

* :ref:`wig2bed <wig2bed>`

  * Increased maximum length of chromosome name buffer to suite-wide :code:`TOKEN_CHR_MAX_LENGTH` value, to reduce likelihood of segmentation faults (thanks to user ma-diroma).

* General

  * Updated copyright dates in source and headers.

-------
v2.4.30
-------

Released: **November 25, 2017**

* :ref:`bedmap <bedmap>`
  
  * Errors are no longer reported when error checking is enabled and running in non-fast mode, when a fully-nested element is detected. This follows up on `issue 199 <https://github.com/bedops/bedops/issues/199>`_.

* :ref:`starch <starch>`

  * Previously, a chromosome record in a Starch archive would result in corrupted metadata, if the chromosome is larger than :code:`UINT32_MAX` bytes (~4.3GB) in size when compressed. This limitation is now removed, and a single chromosome (when compressed in a Starch archive) can be up to :code:`UINT64_MAX` bytes in size.

  * The :code:`starch` binary does more stringent input checks for the character lengths of ID and remainder strings, which must be no larger than 2\ :sup:`ID_EXPONENT` - 1 and 2\ :sup:`REST_EXPONENT` - 1 characters in length. (These constants are specific to the build-time settings in the Makefile and in the app-wide constants.) This follows up on `issue 195 <https://github.com/bedops/bedops/issues/195>`_.

* :ref:`starchcat <starchcat>`

  * Previously, a chromosome record in a Starch archive would result in corrupted metadata, if the chromosome is larger than :code:`UINT32_MAX` bytes (~4.3GB) in size when compressed. This limitation is now removed, and a single chromosome (when compressed in a Starch archive) can be up to :code:`UINT64_MAX` bytes in size.

  * More stringent memory management and stricter adherance to BEDOPS-wide constants, to help reduce likelihood of pointer access out of bounds and incidence of segfaults.

* :ref:`unstarch <unstarch>`

  * The :code:`unstarch` binary implements the character length constants of ID and remainder strings, specific to the build-time settings in the Makefile and in the app-wide constants. This follows up on `issue 195 <https://github.com/bedops/bedops/issues/195>`_.

* :ref:`sort-bed <sort-bed>`

  * Added :code:`--unique` (:code:`-u`) and :code:`--duplicates` (:code:`-d`) options to only print unique and duplicate in sorted output, to mimic the behavior of :code:`sort -u` and :code:`uniq -d` Unix tools. This follows up on `issue 196 <https://github.com/bedops/bedops/issues/196>`_.

  * Switched compile-time, stack-allocated :code:`char` arrays to runtime, heap-based pointers. Timing tests on shuffled FIMO datasets suggest the impact from having to allocate space for buffers at runtime is very minimal. Moving from stack to heap will help avoid segfaults from running into OS-level stack limits, when BEDOPS-level constants change the maximum line length to something larger than the stack.

* Revision testing
  
  * Starch suite tests were updated for v2.2 archives created via v2.4.30 binaries (Linux, libc 2.22).

-------
v2.4.29
-------

Released: **September 26, 2017**

* :ref:`bedmap <bedmap>`

  * Increased megarow build ID length up to 2\ :sup:`18`.

  * Changed behavior of mapping to return mapped items in sort order provided in inputs. This follows up on `issue 198 <https://github.com/bedops/bedops/issues/198>`_.

* :ref:`unstarch <unstarch>`

  * Changed behavior of :code:`--is-starch` option to always return a successful exit code of :code:`0` whether or not the input file is a Starch archive. It will now be up to the person running this option to test the 0 (false) or 1 (true) value printed to the standard output stream. This follows up on `issue 197 <https://github.com/bedops/bedops/issues/197>`_. 

-------
v2.4.28
-------

Released: **August 18, 2017**

* :ref:`bedmap <bedmap>`

  * Patched `issue 191 <https://github.com/bedops/bedops/issues/191>`_ where :code:`--wmean` option was not recognized.

* :ref:`bedextract <bedextract>`

  * Updated documentation with fixed usage statement.

* :ref:`sort-bed <sort-bed>`

  * Patched typo in :code:`update-sort-bed-starch-slurm.py` script.

  * Fixed bug with :code:`--max-mem` on properly ordering things on fourth and subsequent columns, when the genomic intervals are the same.

* :ref:`starch <starch>`

  * Updated Makefiles to remove `lib` on `clean` target and to help prevent :code:`ARCH` variable from getting clobbered by third-party package managers.

* Build process

  * Updated the OS X installer XML to resolve missing asset links.
  
  * Updated the :code:`module_binaries` target to copy over :code:`starchcluster_*` and :code:`starch-diff` assets for :code:`modules` distributions.

-------
v2.4.27
-------

Released: **July 17, 2017**

This revision of BEDOPS includes significant performance improvements for core tools: :code:`bedops`, :code:`bedmap`, and :code:`closest-features`. Performance tests were done with whole-genome TRANSFAC FIMO scans, with cache purges in between trials. 

Pre-built binaries for Darwin and GNU/Linux targets include both the default :code:`typical` and :code:`megarow` builds of BEDOPS. The program names that you are accustomed to will remain as-is, but the binaries will exist as symbolic links pointing to the :code:`typical` builds. These links can be repointed to the :code:`megarow` builds by calling :code:`switch-BEDOPS-binary-type --megarow`, which will set the usual BEDOPS binaries to link to the :code:`megarow` builds. One can run :code:`switch-BEDOPS-binary-type --typical` at any time to revert to the default (:code:`typical`) builds.

The top-level Makefile includes some new variables for those who choose to build from source. The :code:`JPARALLEL` variable sets the number of CPUs to use in parallel when compiling BEDOPS, which can speed compilation time dramatically. The :code:`MASSIVE_REST_EXP`, :code:`MASSIVE_ID_EXP`, and :code:`MASSIVE_CHROM_EXP` are used when building the :code:`megarow` to support any required row lengths (build using :code:`make megarow`).  These are the exponents (the *n* in 2\ :sup:`n`\ ) for holding all characters after chromosome, start, and stop fields, the ID field (column 4, typically), and the chromosome field (column 1). 

To simplify distribution and support, we have removed pre-built 32-bit program versions in this release. These can be built from source by specifying the correct :code:`ARCH` value in the top-level Makefile. For OS X, our package installer now requires OS X version 10.10 or greater.

Application-level notes follow:

* :ref:`bedops <bedops>`

  * Performance of :code:`bedops` tool improved, doing typical work in **76.5%** of the time of all previous versions.

  * Performance of :code:`-u`/:code:`--everything` has improved, doing the same work in only **55.6%** of the time of previous versions when given a large number of input files.

  * The :code:`megarow` build of this application handles input files with very long rows (4M+ characters). Such input might arise from conversion of very-long-read BAM files to BED via :code:`bam2bed`, such as those that may come from Nanopore or PacBio MinION platforms. This build requires more runtime memory than the default (:code:`typical`) build. Pertinent variables for :code:`megarow` execution can be modified through the make system without changing source.

* :ref:`bedmap <bedmap>`

  * Performance of :code:`bedmap` tool improved, doing the same work in **86.7%** of the time of all previous versions.

  * Automatically use :code:`--faster` option when :code:`--exact` is used as the overlap criterion, or if the input files are formatted as Starch archives, no fully-nested elements exist in the archives, and the overlap criterion supports :code:`--faster` (such as :code:`--bp-ovr`, :code:`--exact`, and :code:`--range`).

  * The :code:`megarow` build target handles input files with very long rows (4M+ characters). Such input might arise from conversion of very-long-read BAM files to BED via :code:`bam2bed`, such as those that may come from Nanopore or PacBio MinION platforms. This build requires more runtime memory than the default (:code:`typical`) build. Pertinent variables for :code:`megarow` execution can be modified through the make system without changing source.

  * New :code:`--min-memory` option for use when the reference file has very large regions, and the map file has many small regions that fall within those larger regions. One example is when :code:`--range 100000` is used and the map file consists of whole-genome motif scan hits.  Memory overhead can be reduced to that used by all previous versions, up to and including v2.4.26.

  * Added :code:`--faster` automatically when :code:`--exact` is used, which is robust even when nested elements exist in inputs.  Similarly, :code:`--faster` is used automatically when inputs are Starch-formatted archives, none of which have nested elements (see :code:`unstarch --has-nested`) when the overlap criterion allows for :code:`--faster`.

* :ref:`closest-features <closest-features>`

  * Performance of :code:`closest-features` tool has been improved, doing the same work in **87.7%** of the time of all previous versions.

  * The :code:`megarow` build target is available to compile a version of the program that can handle input files with very long rows (4M+ characters).  This requires more runtime memory than the default build.  Pertinent variables can be modified through the make system without editing source.

* :ref:`convert2bed <convert2bed>`

  Numerous internal changes, including giving line functors the ability to resize the destination (write) buffer in mid-stream, along with increased integration with BEDOPS-wide constants. Destination buffer resizing is particularly useful when converting very-long-read BAM files containing numerous D (deletion) operations, such as when used with the new :code:`--split-with-deletions` option.

  * :ref:`psl2bed <psl2bed>`

    * Migrated storage of PSL conversion state from stack to heap, which helps address segmentation faults on OS X (thanks to rmartson@Biostars for the bug report).

  * :ref:`bam2bed <bam2bed>` and :ref:`sam2bed <sam2bed>`

    * Increased thread I/O heap buffer size to reduce likelihood of overflows while parsing reads from Nanopore and PacBio platforms.

    * Added :code:`--split-with-deletions` option to split spliced junctions by :code:`N` and :code:`D` CIGAR operations. The :code:`--split` option now splits only on :code:`N` operations.

    * Added :code:`--reduced` option to print first six columns of BED data to standard output.

  * :ref:`gff2bed <gff2bed>`

    * Resolved issue parsing GFF input with :code:`##FASTA` directive.

* :ref:`sort-bed <sort-bed>`

  * The :code:`megarow` build target is available to compile a version of the program that can handle input files with very long rows (4M+ characters).  This requires more runtime memory than the default build.  The pertinent variables can be modified through the make system without changing source.  This is useful for converting ultra-long reads from Nanopore and PacBio sequencing platforms to BED via :code:`bam2bed` / :code:`convert2bed`.
  
* :ref:`starch <starch>`

  * Fixed a potential segmentation fault result with :code:`--header` usage.
  
* Starch C++ API

  * Fixed output from :code:`bedops -u` (:code:`--everything`, or multiset union) on two or more Starch archives, where the remainder string was not being cleared correctly.
  
* :ref:`starch-diff <starch_diff>`
  
  * Improved usage statement to clarify output (cf. `Issue 180 <https://github.com/bedops/bedops/issues/180>`_).

* Clang warnings

  * Resolved compilation warnings for several binaries.

-------
v2.4.26
-------

Released: **March 14, 2017**

* :ref:`starchstrip <starchstrip>`

  * New utility to efficiently filter a Starch archive, including or excluding records by specified chromosome names, without doing expensive extraction and recompression. This follows up on `internal discussion <https://stamlab.slack.com/archives/bedops/p1487878245000103>`_ on the Altius Slack channel.

* :ref:`starch-diff <starch_diff>`

  * Fixed testing logic in :code:`starch-diff` for certain archives. Thanks to Shane Neph for the report.

* :ref:`starchcat <starchcat>`

  * Fixed possible condition where too many variables on the stack can cause a stack overload on some platforms, leading to a fatal segmentation fault. Improved logic for updating v2.1 to v2.2 Starch archives.

* Starch C++ API

  * Patched gzip-backed Starch archive extraction issue. Thanks to Matt Maurano for the bug report.

* :ref:`update-sort-bed-migrate-candidates <sort-bed>`

  * Added detailed logging via :code:`--debug` option.

  * Added :code:`--bedops-root-dir` option to allow specifying where all BEDOPS binaries are stored. This setting can be overruled on a per-binary basis by adding :code:`--bedextract-path`, :code:`--sort-bed-path`, etc.

  * Added :code:`--non-recursive-search` option to restrict search for BED and Starch candidates to the top-level of the specified parent directory :code:`--parent-dir` option.
    
  * Further simplification and customization of parameters sent to :code:`update-sort-bed-slurm` and :code:`update-sort-bed-starch-slurm` cluster scripts, as well as logging and variable name improvements to those two scripts.

  * Thanks again to Matt Maurano for ongoing feedback and suggestions on functionality and fixes.

* :ref:`gtf2bed <gtf2bed>`

  * Resolved segmentation fault with certain inputs, in follow-up to `this BEDOPS Forum post <http://bedops.uwencode.org/forum/index.php?topic=136.0>`_. Thanks to zebasilio for the report and feedback.

-------
v2.4.25
-------

Released: **February 15, 2017**

* :ref:`convert2bed <convert2bed>`

  * Patch for RepeatMasker inputs with blank lines that have no spaces. This follows up on `Issue 173 <https://github.com/bedops/bedops/issues/173>`_. Thanks to saketkc for the bug report.

* :ref:`update-sort-bed-migrate-candidates <sort-bed>`

  The :code:`update-sort-bed-migrate-candidates` utility recursively searches into the specified directory for BED and Starch files which fail a :code:`sort-bed --check-sort` test. Those files which fail this test can have their paths written to a text file for further downstream processing, or the end user can decide to apply an immediate resort on those files, either locally or via a SLURM-managed cluster. Grateful thanks to Matt Maurano for input and testing.

  See :code:`update-sort-bed-migrate-candidates --help` for more information, or review the :ref:`sort-bed <sort-bed>` documentation.

* :ref:`update-sort-bed-starch-slurm <sort-bed>`

  This is an adjunct to the :code:`update-sort-bed-slurm` utility, which resorts the provided Starch file and writes a new file. (The :code:`update-sort-bed-slurm` utility only takes in BED files as input and writes BED as output.)

-------
v2.4.24
-------

Released: **February 6, 2017**

* :ref:`starch-diff <starch_diff>`

  * The :code:`starch-diff` utility compares signatures of two or more v2.2+ Starch archives. This tool tests all chromosomes or one specified chromosome. It returns a zero exit code, if the signature(s) are identical, or a non-zero error exit code, if one or more signature(s) are dissimilar.

* :ref:`update-sort-bed-slurm <sort-bed>`

  * The :code:`update-sort-bed-slurm` convenience utility provides a parallelized update of the sort order on BED files sorted with pre-v2.4.20 sort-bed, for users with a SLURM job scheduler and associated cluster. See :code:`update-sort-bed-slurm --help` for more details.

* :ref:`convert2bed <convert2bed>`

  * Patched a memory leak in VCF conversion. Thanks to ehsueh for the bug report.

-------
v2.4.23
-------

Released: **January 30, 2017**

* :ref:`unstarch <unstarch>`
  
  * Fixed bug where missing signature from pre-v2.2 Starch archives would cause a fatal metadata error. Thanks to Shane Neph and Eric Rynes for the bug report.
  
  * Improved logic reporting signature mismatches when input v2.2 archive lacks signature (*e.g.*, for a v2.2 archive made with :code:`--omit-signature`).
  
* :ref:`starch <starch>` and :ref:`starchcat <starchcat>`
  
  * Added :code:`--omit-signature` option to compress without creating a per-chromosome data integrity signature. While this reduces compression time, this eliminates the verification benefits of the data integrity signature.

-------
v2.4.22
-------

Released: **January 25, 2017**

* :ref:`convert2bed <convert2bed>`

  * Fixed heap corruption in GFF conversion. Thanks to J. Miguel Mendez (ObjectiveTruth) for the bug report.
    
-------
v2.4.21
-------

Released: **January 23, 2017**

* :ref:`bedmap <bedmap>`

  * New :code:`--wmean` operation offers a weighted mean calculation. The "weight" is derived from the proportion of the reference element covered by overlapping map elements: *i.e.*, a map element that covers more of the reference element has its signal given a larger weight or greater impact than another map element with a shorter overlap.

  * Measurement values in :code:`bedmap` did not allow :code:`+` in the exponent (both :code:`-` worked and no :code:`+` for a positive value.  Similarly, out in front of the number, :code:`+` was previously not allowed. Shane Neph posted the report and the fix.

  * The :code:`--min-element` and :code:`--max-element` operations in :ref:`bedmap <bedmap>` now process elements in unambiguous order. Former behavior is moved to the operations :code:`--min-element-rand` and :code:`--max-element-rand`, respectively.

  * Fixed issue with use of :code:`--echo-overlap-size` with :code:`--multidelim` (cf. `Issue 165 <https://github.com/bedops/bedops/issues/165>`_). Shane Neph posted the fix. Thanks to Jeff Vierstra for the bug report!

* :ref:`bedops <bedops>`

  * Fixed issue with :code:`--chop` where complement operation could potentially be included. Shane Neph posted the fix.

  * The :code:`bedops --everything` or :code:`bedops -u` (union) operation now writes elements to standard output in unambiguous sort order. If any data are contained in fourth or subsequent fields, a lexicographical sort on that data is applied for resolving order of interval matches.

* :ref:`sort-bed <sort-bed>`

  * Improved sort times from replacing quicksort (:code:`std::qsort`) with inlined C++ :code:`std::sort`.

  * Sorting of BED input now leads to unambiguous result when two or more elements have the same genomic interval (chromosome name and start and stop position), but different content in remaining columns (ID, score, etc.). 

    Formerly, elements with the same genomic interval that have different content in fourth and subsequent columns could be printed in a non-consistent ordering on repeated sorts. A deterministic sort order facilitates the use of data integrity functions on sorted BED and Starch data.

* :ref:`starchcluster <starchcluster>`

  * A SLURM-ready version of the :code:`starchcluster` script was added to help SLURM job scheduler users with parallelizing the creation of Starch archives.

* Parallel :ref:`bam2bed <parallel_bam2bed>` and :ref:`bam2starch <parallel_bam2starch>`

  * SLURM-ready versions of these scripts were added to help parallelize the conversion of BAM to BED files (:code:`bam2bed_slurm`) or to Starch archives (:code:`bam2starch_slurm`).

* :ref:`unstarch <unstarch>`

  * Added :code:`--signature` option to report the Base64-encoded SHA-1 data integrity signature of the Starch-transformed bytes of a specified chromosome, or to report the signature of the metadata string as well as the signatures of all chromosomes, if unspecified.

  * Added :code:`--verify-signature` option to compare the "expected" Base64-encoded SHA-1 data integrity signature stored within the archive's metadata with the "observed" data integrity signature generated from extracting the specified chromosome. 

    If the observed and expected signatures differ, then this suggests that the chromosome record may be corrupted in some way; :code:`unstarch` will exit with a non-zero error code. If the signatures agree, the archive data should be intact and `unstarch` will exit with a helpful notice and a zero error code.

    If no chromosome is specified, :code:`unstarch` will loop through all chromosomes in the archive metadata, comparing observed and expected values for each chromosome record. Upon completion, error and progress messages will be reported to the standard error stream, and :code:`unstarch` will exit with a zero error code, if all signatures match, or a non-zero exit state, if one or more signatures do not agree.

  * The output from the :code:`--list` option includes a :code:`signature` column to report the data integrity signature of all Starch-transformed chromosome data.

  * The output from the :code:`--list-json` option includes a :code:`signature` key in each chromosome record in the archive metadata, reporting the same information.

  * The :code:`--is-starch` option now quits with a non-zero exit code, if the specified input file is not a Starch archive.

  * The :code:`--elements-max-string-length` option reports the length of the longest string within the specified chromosome, or the longest string over all chromosomes (if no chromosome name is specified).

* :ref:`starch <starch>`

  * Added :code:`--report-progress=N` option to (optionally) report compression of the Nth element of the current chromosome to standard error stream.

  * As a chromosome is compressed, the input Starch-transform bytes are continually run through a SHA-1 hash function. The resulting data integrity signature is stored as a Base64-encoded string in the output archive's metadata. Signatures can be compared between and within archives to help better ensure the data integrity of the archive.

  * Fixed :code:`--header` transform bug reported in `Issue 161 <https://github.com/bedops/bedops/issues/161>`_. Thanks to Shane Neph for the bug report!

  * Added chromosome name and "remainder" order tests to :code:`STARCH2_transformHeaderlessBEDInput` and :code:`STARCH2_transformHeaderedBEDInput` functions. 

    Compression with :code:`starch` ends with a fatal error, should any of the following comparison tests fail:

    1. The chromosome names are not lexicographically ordered (*e.g.*, :code:`chr1` records coming after :code:`chr2` records indicates the data are not correctly sorted).

    2. The start position of an input element is less than the start position of a previous input element on the same chromosome (*e.g.*, :code:`chr1:1000-1234` coming after :code:`chr1:2000-2345` is not correctly sorted).

    3. The stop positions of two or more input elements are not in ascending order when their start positions are equal (*e.g.*, :code:`chr1:1000-1234` coming after :code:`chr1:1000-2345` is not correctly sorted). 
    
    4. The start and stop positions of two or more input elements are equivalent, and their "remainders" (fourth and subsequent columns) are not in ascending order (*e.g.*, :code:`chr1:1000-1234:id-0` coming after :code:`chr1:1000-1234:id-1` is not correctly sorted). 

    If the sort order of the input data is unknown or uncertain, simply use :code:`sort-bed` to generate the correct ordering and pipe the output from that to :code:`starch`, *e.g.* :code:`$ cat elements.bed | sort-bed - | starch - > elements.starch`.

* :ref:`starchcat <starchcat>`

  * Added :code:`--report-progress=N` option to (optionally) report compression of the *N* th element of the current chromosome to standard error stream.

  * As in :code:`starch`, at the conclusion of compressing a chromosome made from one or more input Starch archives, the input Starch-transform bytes are continually run through a SHA-1 hash function. The resulting data integrity signature is stored as a Base64-encoded string in the chromosome's entry in the new archive's metadata.

  * As in :code:`starch`, if data should need to be extracted and recompressed, the output is written so that the order is unambiguous: ascending lexicographic ordering on chromosome names, numerical ordering on start positions, the same ordering on stop positions where start positions match, and ascending lexicographic ordering on the remainder of the BED element (fourth and subsequent columns, where present).

* :ref:`convert2bed <convert2bed>`

  * Improvements in support for BAM/SAM inputs with larger-sized reads, as would come from alignments made from data collected from third-generation sequencers. Simulated read datasets were generated using `SimLoRD <https://bitbucket.org/genomeinformatics/simlord/>`_. Tests have been performed on simulated hg19 data up to 100kb read lengths.

    Improvements allow:

    * conversion of dynamic number of CIGAR operations (up to system memory)

    * conversion of dynamically-sized read fields (up to system memory and inter-thread buffer allocations)

    These patches follow up on bug reports in `Issue 157 <https://github.com/bedops/bedops/issues/157>`_.

  * Improvements in support for VCF inputs, to allow aribtrary-sized fields (up to system memory and inter-thread buffer allocations), which should reduce or eliminate segmentation faults from buffer overruns on fields larger than former stack defaults.

  * Improvements in support for GFF inputs, to allow aribtrary-sized fields (up to system memory and inter-thread buffer allocations), which should reduce or eliminate segmentation faults from buffer overruns on fields larger than former stack defaults.

  * Improvements in support for GTF inputs, to allow aribtrary-sized fields (up to system memory and inter-thread buffer allocations), which should reduce or eliminate segmentation faults from buffer overruns on fields larger than former stack defaults.

* Testing

  * Our use of Travis CI to automate testing of builds now includes Clang on `their OS X environment <https://docs.travis-ci.com/user/osx-ci-environment/>`_.

-------
v2.4.20
-------

Released: **July 27, 2016**

* :ref:`convert2bed <convert2bed>`

  * Increased memory allocation for maximum number of per-read CIGAR operations in BAM and SAM conversion to help improve stability. Thanks to Adam Freedman for the report!

  * Improved reliability of gene ID parsing from GTF input, where :code:`gene_id` field may be positioned at start, middle, or end of attributes string, or may be empty. Thanks to blaiseli for the report!

-------
v2.4.19
-------

Released: **May 9, 2016**

* :ref:`convert2bed <convert2bed>`

  * Fixed bug in BAM and SAM parallel conversion scripts (:code:`*_gnuParallel` and :code:`*_sge`) with inputs containing chromosome names without :code:`chr` prefix. Thanks to Eric Haugen for the bug report!

* Starch C++ API

  * Fixed bug with extraction of bzip2- and gzip-backed archives with all other non-primary Starch tools (all tools except :code:`starch`, :code:`unstarch`, :code:`starchcat`, and :code:`sort-bed`). Thanks to Eric Haugen for the bug report!

-------
v2.4.18
-------

Released: **April 28, 2016**

* :ref:`convert2bed <convert2bed>`

  * Fixed compile warnings.
  * Fixed bug in BAM and SAM conversion with optional field line overflow. Thanks to Jemma Nelson for the bug report!

* General documentation improvements

  * Updated OS X Installer and Github release instructions
  * Added thank-you to Feng Tian for bug report

-------
v2.4.17
-------

Released: **April 26, 2016**

* :ref:`bam2bed <bam2bed>` and :ref:`sam2bed <sam2bed>`

  * Improved parsing of non-split BAM and SAM inputs.

* Docker container build target added for Debian

  * Thanks to Leo Comitale (Poldo) for writing a Makefile target and spec for creating a BEDOPS Docker container for the Debian target.

* Starch C++ API

  * Fixed bug with extraction of bzip2- and gzip-backed archives with all other non-primary Starch tools (all tools except :code:`starch`, :code:`unstarch`, :code:`starchcat`, and :code:`sort-bed`). Thanks to Feng Tian for reports.

-------
v2.4.16
-------

Released: **April 5, 2016**

* :ref:`bedmap <bedmap>`

  * Added new :code:`--echo-ref-row-id` option to report reference row ID elements.

* Starch C++ API

  * Fixed bug with extraction of archives made with :code:`starch --gzip` (thanks to Brad Gulko for the bug report and Paul Verhoeven and Peter Weir for compile and testing assistance).

* General improvements

  * Small improvements to build cleanup targets.

-------
v2.4.15
-------

Released: **January 21, 2016**

* Docker container build target added for CentOS 7

  * Thanks to Leo Comitale (Poldo) for writing a Makefile target and spec for creating a BEDOPS Docker container for CentOS 7.

* :ref:`convert2bed <convert2bed>`

  * Fixed buffer overflows in :code:`convert2bed` to improve conversion reliability for VCF files (thanks to Jared Andrews and Kousik Kundu for bug reports).

* General improvements

  * Improved OS X 10.11 build process.

-------
v2.4.14
-------

Released: **April 21, 2015**

* :ref:`convert2bed <convert2bed>`

  * Fixed missing :code:`samtools` variable references in cluster conversion scripts (thanks to Brad Gulko for the bug report).

* General suite-wide improvements

  * Fixed exception error message for :code:`stdin` check (thanks to Brad Gulko for the bug report).


-------
v2.4.13
-------

Released: **April 20, 2015**

* :ref:`bedops <bedops>`

  * Resolved issue in using :code:`--ec` with :code:`bedops` when reading from :code:`stdin` (thanks to Brad Gulko for the bug report).

* General suite-wide improvements

  * Addressed inconsistency with constants defined for the suite at the extreme end of the limits we allow for coordinate values (thanks again to Brad Gulko for the report).

-------
v2.4.12
-------

Released: **March 13, 2015**

* :ref:`bedops <bedops>`

  * Checks have been added to determine if an integer argument is a file in the current working directory, before interpreting that argument as an overlap criterion for :code:`-e` and :code:`-n` options. 

    To reduce ambiguity, if an integer is used as a file input, :code:`bedops` issues a warning of the interpretation and provides guidance on how to force that value to instead be used as an overlap specification, if desired (thanks to E. Rynes for the pointer).

* :ref:`bedmap <bedmap>`

  * Added support for :code:`--prec` / :code:`--sci` with :code:`--min-element` and :code:`--max-element` operations (thanks to E. Rynes for the pointer).

* :REF:`bedops <bedops>` | :ref:`bedmap <bedmap>` | :ref:`closest-features <closest-features>`

  * Added support for :code:`bash` process substitution/named pipes with specification of :code:`--chrom` and/or :code:`--ec` options (thanks to B. Gulko for the bug report).

  * Fixed code that extracts :code:`gzip`-backed Starch archives from :code:`bedops` and other core tools (thanks again to B. Gulko for the bug report).

* :ref:`convert2bed <convert2bed>`

  * Switched :code:`matches` and :code:`qSize` fields in order of :code:`psl2bed` output. Refer to documentation for new field order.

  * Added null sentinel to GTF ID value.

  * To help reduce the chance of buffer overflows, the :code:`convert2bed` tool increases the maximum field length from 8191 to 24575 characters to allow parsing of inputs with longer field length, such as very long attributes from `mosquito GFF3 <https://www.vectorbase.org/download/aedes-aegypti-liverpoolbasefeaturesaaegl33gff3gz>`_ data (thanks to T. Karginov for the bug report).

-------
v2.4.11
-------

Released: **February 24, 2015**

* :ref:`convert2bed <convert2bed>`

  * Fixed bug in :code:`psl2bed` where :code:`matches` column value was truncated by one character. Updated unit tests. Thanks to M. Wirthlin for the bug report.

-------
v2.4.10
-------

Released: **February 23, 2015**

* :ref:`starch <starch>`

  * In addition to checking chromosome interleaving, the :code:`starch` tool now enforces :code:`sort-bed` sort ordering on BED input and exits with an :code:`EINVAL` POSIX error code if the data are not sorted correctly.

* :ref:`convert2bed <convert2bed>`

  * Added :code:`--zero-indexed` option to :code:`wig2bed` and :code:`wig2starch` wrappers and :code:`convert2bed` binary, which converts WIG data that are zero-indexed without any coordinate adjustments. This is useful for WIG data sourced from the UCSC Kent tool :code:`bigWigToWig`, where the :code:`bigWig` data can potentially be sourced from 0-indexed BAM- or bedGraph-formatted data. 

  * If the WIG input contains any element with a start coordinate of 0, the default use of :code:`wig2bed`, :code:`wig2starch` and :code:`convert2bed` will exit early with an error condition, suggesting the use of :code:`--zero-indexed`.

  * Updated copyright date range of wrapper scripts

------
v2.4.9
------

Released: **February 17, 2015**

* :ref:`sort-bed <sort-bed>`

  * Added support for :code:`--check-sort` to report if input is sorted (or not)

* Starch

  * Improved support for :code:`starch --header`, where header contains tab-delimited fields

* Starch C++ API

  * Fixed bug with :code:`starch --header` functionality, such that BEDOPS core tools (:code:`bedops`, etc.) would be unable to extract correct data from headered Starch archive

------
v2.4.8
------

Released: **February 7, 2015**

* Mac OS X packaging

  * Installer signed with `productsign <https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/productsign.1.html#//apple_ref/doc/man/1/productsign>`_ to pass `OS X Gatekeeper <http://support.apple.com/en-us/HT202491>`_

* Linux packaging

  * SHA1 hashes of each tarball are now part of the `BEDOPS Releases <https://github.com/bedops/bedops/releases/>`_ description page, going forwards

* Updated copyright dates in source code

------
v2.4.7
------

Released: **February 2, 2015**

* :ref:`convert2bed <convert2bed>` fixes and improvements

  * Fixed :code:`--split` support in :code:`psl2bed` (thanks to Marco A.)

  * Fixed compilation warning regarding comparison of signed and unsigned values

  * Fixed corrupted :code:`psl2bed` test inputs

------
v2.4.6
------

Released: **January 30, 2015**

* :ref:`convert2bed <convert2bed>` fixes and improvements
  
  * Added support for conversion of the `GVF file format <http://www.sequenceontology.org/resources/gvf.html#summary>`_, including wrapper scripts and unit tests. Refer to the :code:`gvf2bed` documentation for more information.

  * Fixed bug in string copy of zero-length element attribute for :code:`gff2bed` and :code:`gtf2bed` (GFF and GTF) formats

* General fixes and improvements

  * Fixed possibly corrupt bzip2, Jansson and zlib tarballs (thanks to rekado, Shane N. and Richard S.)

  * Fixed typo in :code:`bedextract` documentation

  * Fixed broken image in :ref:`Overview <overview>`

  * Removed 19 MB :code:`_build` intermediate result directory (which should improve overall :code:`git clone` time considerably!)

------
v2.4.5
------

Released: **January 28, 2015**

* :ref:`convert2bed <convert2bed>` improvements

  * Addition of RepeatMasker annotation output (:code:`.out`) file conversion support, :code:`rmsk2bed` and :code:`rmsk2starch` wrappers, and unit tests

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

  * Shane Neph put in a great deal of work to enable parallel builds (*e.g.*, :code:`make -j N` to build various targets in parallel). Depending on the end user's environment, this can speed up compilation time by a factor of 2, 4 or more.

  * Fixed numerous compilation warnings of debug builds of :code:`starch` toolkit under RHEL6/GCC and OS X 10.10.1/LLVM.

* New :ref:`bedops` features

  * Added :code:`--chop` and :code:`--stagger` options to "melt" inputs into contiguous or staggered disjoint regions of equivalent size.

  * For less confusion, arguments for :code:`--element-of`, :code:`--chop` and other :code:`bedops` operations that take numerical modifiers no longer require a leading hyphen character. For instance, :code:`--element-of 1` is now equivalent to the former usage of :code:`--element-of -1`.

* New :ref:`bedmap` features

  * The :code:`--sweep-all` option reads through the entire map file without early termination and can help deal with :code:`SIGPIPE` errors. It adds to execution time, but the penalty is not as severe as with the use of :code:`--ec`. Using :code:`--ec` alone will enable error checking, but will now no longer read through the entire map file. The :code:`--ec` option can be used in conjunction with :code:`--sweep-all`, with the associated time penalties. (Another method for dealing with issue this is to override how :code:`SIGPIPE` errors are caught by the interpreter (:code:`bash`, Python, *etc.*) and retrapping them or ignoring them. However, it may not a good idea to do this as other situations may arise in production pipelines where it is ideal to trap and handle all I/O errors in a default manner.)

  * New :code:`--echo-ref-size` and :code:`--echo-ref-name` operations report genomic length of reference element, and rename the reference element in :code:`chrom:start-end` (useful for labeling rows for input for :code:`matrix2png` or :code:`R` or other applications).

* :ref:`bedextract`

  * Fixed upper bound bug that would cause incorrect output in some cases

* :ref:`conversion scripts <conversion_scripts>`

  * Brand new C99 binary called :code:`convert2bed`, which wrapper scripts (:code:`bam2bed`, *etc.*) now call. No more Python version dependencies, and the C-based rewrite offers massive performance improvements over old Python-based scripts.

  * Added :code:`parallel_bam2starch` script, which parallelizes creation of :ref:`Starch <starch_specification>` archive from very large BAM files in SGE environments.

  * Added bug fix for missing code in :ref:`starchcluster.gnu_parallel <starchcluster>` script, where the final collation step was missing.

  * The :code:`vcf2bed` script now accepts the :code:`--do-not-split` option, which prints one BED element for all alternate alleles.

* :ref:`Starch <starch_specification>` archival format and compression/extraction tools

  * Added duplicate- and :ref:`nested-element <nested_elements>` flags in v2.1 of Starch metadata, which denote if a chromosome contains one or more duplicate and/or nested elements. BED files compressed with :code:`starch` v2.5 or greater, or Starch archives updated with :code:`starchcat` v2.5 or greater will include these values in the archive metadata. The :code:`unstarch` extraction tool offers :code:`--has-duplicate` and :code:`--has-nested` options to retrieve these flag values for a specified chromosome (or for all chromosomes).

  * Added :code:`--is-starch` option to :code:`unstarch` to test if specified input file is a Starch v1 or v2 archive.
 
  * Added bug fix for compressing BED files with :code:`starch`, where the archive would not include the last element of the BED input, if the BED input lacked a trailing newline. The compression tools now include a routine for capturing the last line, if there is no newline.

* Documentation improvements

  * Remade some image assets throughout the documents to support Retina-grade displays

------
v2.4.2
------

Released: **April 10, 2014**

* :ref:`conversion scripts <conversion_scripts>`

  * Added support for :code:`sort-bed --tmpdir` option to conversion scripts, to allow specification of alternative temporary directory for sorted results when used in conjunction with :code:`--max-mem` option.

  * Added support for GFF3 files which include a FASTA directive in :code:`gff2bed` and :code:`gff2starch` (thanks to Keith Hughitt).

  * Extended support for Python-based conversion scripts to support use with Python v2.6.2 and forwards, except for :code:`sam2bed` and :code:`sam2starch`, which still require Python v2.7 or greater (and under Python3).

  * Fixed :code:`--insertions` option in :code:`vcf2bed` to now report a single-base BED element (thanks to Matt Maurano).

------
v2.4.1
------

Released: **February 26, 2014**

* :ref:`bedmap`

  * Added :code:`--fraction-both` and :code:`--exact` (:code:`--fraction-both 1`) to list of compatible overlap options with :code:`--faster`.

  * Added 5% performance improvement with :code:`bedmap` operations without :code:`--faster`.

  * Fixed scenario that can yield incorrect results (cf. `Issue 43 <https://github.com/bedops/bedops/issues/43>`_).

* :ref:`sort-bed`

  * Added :code:`--tmpdir` option to allow specification of an alternative temporary directory, when used in conjunction with :code:`--max-mem` option. This is useful if the host operating system's standard temporary directory (*e.g.*, :code:`/tmp` on Linux or OS X) does not have sufficient space to hold intermediate results.

* All :ref:`conversion scripts <conversion_scripts>`

  * Improvements to error handling in Python-based conversion scripts, in the case where no input is specified.

  * Fixed typos in :code:`gff2bed` and :code:`psl2bed` documentation (cf. `commit a091e18 <https://github.com/bedops/bedops/commit/a091e18>`_).

* OS X compilation improvements

  * We have completed changes to the OS X build process for the remaining half of the BEDOPS binaries, which now allows direct, full compilation with Clang/LLVM (part of the Apple Xcode distribution). 

    All OS X BEDOPS binaries now use Apple's system-level C++ library, instead of GNU's :code:`libstdc++`. It is no longer required (or recommended) to use GNU :code:`gcc` to compile BEDOPS on OS X.

    Compilation is faster and simpler, and we can reduce the size and complexity of Mac OS X builds and installer packages. By using Apple's C++ library, we also eliminate the likelihood of missing library errors. 

    In the longer term, this gets us closer to moving BEDOPS to using the CMake build system, to further abstract and simplify the build process.

* Cleaned up various compilation warnings found with :code:`clang` / :code:`clang++` and GCC kits.

------
v2.4.0
------

Released: **January 9, 2014**

* :ref:`bedmap`

  * Added new :code:`--echo-map-size` and :code:`--echo-overlap-size` options to calculate sizes of mapped elements and overlaps between mapped and reference elements.

  * Improved performance for all :code:`--echo-map-*` operations.

  * Updated documentation.

* Major enhancements and fixes to :ref:`sort-bed`:

  * Improved performance.

  * Fixed memory leak.

  * Added support for millions of distinct chromosomes.

  * Improved internal estimation of memory usage with :code:`--max-mem` option.

* Added support for compilation on Cygwin (64-bit). Refer to the :ref:`installation documentation <installation_via_source_code_on_cygwin>` for build instructions.

* :ref:`starchcat`

  * Fixed embarassing buffer overflow condition that caused segmentation faults on Ubuntu 13. 

* All :ref:`conversion scripts <conversion_scripts>`

  * Python-based scripts no longer use temporary files, which reduces file I/O and improves performance. This change also reduces the need for large amounts of free space in a user's :code:`/tmp` folder, particularly relevant for users converting multi-GB BAM files.

  * We now test for ability to locate :code:`starch`, :code:`sort-bed`, :code:`wig2bed_bin` and :code:`samtools` in user environment, quitting with the appropriate error state if the dependencies cannot be found.

  * Improved documentation. In particular, we have added descriptive tables to each script's documentation page which describe how columns map from original data input to BED output.

  * :ref:`bam2bed` and :ref:`sam2bed`

    * Added :code:`--custom-tags <value>` command-line option to support a comma-separated list of custom tags (cf. `Biostars discussion <http://www.biostars.org/p/87062/>`_), *i.e.*, tags which are not part of the original SAMtools specification.

    * Added :code:`--keep-header` option to preserve header and metadata as BED elements that use :code:`_header` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`vcf2bed`

    * Added new :code:`--snvs`, :code:`--insertions` and :code:`--deletions` options that filter VCF variants into three separate subcategories.

    * Added :code:`--keep-header` option to preserve header and metadata as BED elements that use :code:`_header` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`gff2bed`

    * Added :code:`--keep-header` option to preserve header and metadata as BED elements that use :code:`_header` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`psl2bed`

    * Added :code:`--keep-header` option to preserve header and metadata as BED elements that use :code:`_header` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

  * :ref:`wig2bed`

    * Added :code:`--keep-header` option to :code:`wig2bed` binary and :code:`wig2bed` / :code:`wig2starch` wrapper scripts, to preserve header and metadata as BED elements that use :code:`_header` as the chromosome name. This now makes these conversion scripts fully "non-lossy".

* Added OS X uninstaller project to allow end user to more easily remove BEDOPS tools from this platform.

* Cleaned up various compilation warnings found with :code:`clang` / :code:`clang++` and GCC kits.

------
v2.3.0
------

Released: **October 2, 2013**

* Migration of BEDOPS code and documentation from Google Code to Github.

  * Due to changes with Google Code hosting policies at the end of the year, we have decided to change our process for distributing code, packages and documentation. While most of the work is done, we appreciate feedback on any problems you may encounter. Please email us at `bedops@stamlab.org <mailto:bedops@stamlab.org>`_ with details.

  * Migration to Github should facilitate requests for code by those who are familiar with :code:`git` and want to fork our project to submit `pull requests <https://help.github.com/articles/using-pull-requests>`_.

* :ref:`bedops`

  * General :code:`--ec` performance improvements.

* :ref:`bedmap`

  * Adds support for the new :code:`--skip-unmapped` option, which filters out reference elements which do not have mapped elements associated with them. See the end of the :ref:`score operations <bedmap_score_operations>` section of the :ref:`bedmap` documentation for more detail.

  * General :code:`--ec` performance improvements.

* :ref:`starch`

  * Fixed bug with :code:`starch` where zero-byte BED input (*i.e.*, an "empty set") created a truncated and unusable archive. We now put in a "dummy" chromosome for zero-byte input, which :code:`unstarch` can now unpack. 

    This should simplify error handling with certain pipelines, specifically where set or other BEDOPS operations yield an "empty set" BED file that is subsequently compressed with :code:`starch`.

* :ref:`unstarch`

  * Can now unpack zero-byte ("empty set") compressed :code:`starch` archive (see above).

  * Changed :code:`unstarch --list` option to print to :code:`stdout` stream (this was previously sent to :code:`stderr`).

* :ref:`starch` metadata library

  * Fixed array overflow bug with BEDOPS tools that take :ref:`starch <starch_specification>` archives as inputs, which affected use of archives as inputs to :code:`closest-features`, :code:`bedops` and :code:`bedmap`.

* All :ref:`conversion scripts <conversion_scripts>`

  * Python scripts require v2.7+ or greater.

  * Improved (more "Pythonic") error code handling.

  * Disabled support for :code:`--max-mem` sort parameter until :ref:`sort-bed` `issue <https://github.com/bedops/bedops/issues/1>`_ is resolved. Scripts will continue to sort, but they will be limited to available system memory. If you are processing files larger than system memory, please contact us at `bedops@stamlab.org <mailto:bedops@stamlab.org>`_ for details of a temporary workaround.

* :ref:`gff2bed` conversion script

  * Resolved :code:`IndexError` exceptions by fixing header support, bringing script in line with `v1.21 GFF3 spec <http://www.sequenceontology.org/gff3.shtml>`_.

* :ref:`bam2bed` and :ref:`sam2bed` conversion scripts

  * Rewritten :code:`bam2*` and :code:`sam2*` scripts from :code:`bash` into Python (v2.7+ support).

  * Improved BAM and SAM input validation against the `v1.4 SAM spec <http://samtools.sourceforge.net/SAMv1.pdf>`_.

  * New :code:`--split` option prints reads with :code:`N` CIGAR operations as separated BED elements.

  * New :code:`--all-reads` option prints all reads, mapped and unmapped.

* :ref:`bedextract`

  * Fixed :code:`stdin` bug with :code:`bedextract`.

* New documentation via `readthedocs.org <readthedocs.org>`_.

  * Documentation is now part of the BEDOPS distribution, instead of being a separate download.

  * We use `readthedocs.org <readthedocs.org>`_ to host indexed and searchable HTML. 

  * `PDF and eBook <https://readthedocs.org/projects/bedops/downloads/>`_ documents are also available for download.

  * Documentation is refreshed and simplified, with new installation and compilation guides.

* OS X compilation improvements

  * We have made changes to the OS X build process for half of the BEDOPS binaries, which allows direct compilation with Clang/LLVM (part of the Apple Xcode distribution). Those binaries now use Apple's system-level C++ library, instead of GNU's :code:`libstdc++`. 

    This change means that we require Mac OS X 10.7 ("Lion") or greater |---| we do not support 10.6 at this time.

    Compilation is faster and simpler, and we can reduce the size and complexity of Mac OS X builds and installer packages. By using Apple's C++ library, we also reduce the likelihood of missing library errors. When this process is completed for the remaining binaries, it will no longer be necessary to install GCC 4.7+ (by way of MacPorts or other package managers) in order to build BEDOPS on OS X, nor will we have to bundle :code:`libstdc++` with the installer.

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

  * We have added a test suite for the Starch archive toolkit with the source download. Test inputs include randomized BED data generated from chromosome and bounds data stored on UCSC servers as well as static FIMO search results. Tests put :code:`starch`, :code:`unstarch` and :code:`starchcat` through various usage scenarios. Please refer to the Starch-specific Makefiles and the test target and subfolder's `README` doc for more information.

* :ref:`starchcat`

  * Resolves bug with :code:`--gzip` option, allowing updates of :code:`gzip` -backed v1.2 and v1.5 archives to the :ref:`v2 Starch format <starch_specification>` (either :code:`bzip2` - or :code:`gzip` -backed).

* :ref:`unstarch`

  * Resolves bug with extraction of :ref:`Starch <starch>` archive made from BED files with four or more columns. A condition where the total length of additional columns exceeds a certain number of characters would result in extracted data in those columns being cut off. As an example, this could affect Starch archives made from the raw, uncut output of GTF- and GFF- :ref:`conversion scripts <conversion_scripts>`.

* :ref:`conversion scripts <conversion_scripts>`

  * We have partially reverted :code:`wig2bed`, providing a Bash shell wrapper to the original C binary. This preserves consistency of command-line options across the conversion suite, while making use of the C binary to recover performance lost from the Python-based v2.1 revision of :code:`wig2bed` (which at this time is no longer supported). (Thanks to Matt Maurano for reporting this issue.)

------
v2.1.1
------

Released: **May 3, 2013**

* :ref:`bedmap`

  * Major performance improvements made in v2.1.1, such that current :code:`bedmap` now operates as fast or faster than the v1.2.5 version of :code:`bedmap`!

* :ref:`bedops`

  * Resolves bug with :code:`--partition` option.

* :ref:`conversion scripts <conversion_scripts>`

  * All v2.1.0 Python-based scripts now include fix for :code:`SIGPIPE` handling, such that use of :code:`head` or other common UNIX utilities to process buffered standard output no longer yields :code:`IOError` exceptions. (Thanks to Matt Maurano for reporting this bug.)

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

  * New :code:`--partition` operator efficiently generates disjoint segments made from genomic boundaries of all overlapping inputs.

* :ref:`conversion scripts <conversion_scripts>`

  * All scripts now use :code:`sort-bed` behind the scenes to output sorted BED output, ready for use with BEDOPS utilities. It is no longer necessary to pipe data to or otherwise post-process converted data with :code:`sort-bed`.

  * New :code:`psl2bed` conversion script, converting `PSL-formatted UCSC BLAT output <http://genome.ucsc.edu/FAQ/FAQformat.html#format2>`_ to BED.

  * New :code:`wig2bed` conversion script written in Python.

  * New :code:`*2starch` :ref:`conversion scripts <conversion_scripts>` offered for all :code:`*2bed` scripts, which output Starch v2 archives.

* :ref:`closest-features`

  * Replaced :code:`--shortest` option name with :code:`--closest`, for clarity. (Old scripts which use :code:`--shortest` will continue to work with the deprecated option name for now. We advise editing pipelines, as needed.)

* :ref:`starch`

  * Improved error checking for interleaved records. This also makes use of :code:`*2starch` conversion scripts with the :code:`--do-not-sort` option safer.

* Improved Mac OS X support

  * New Mac OS X package installer makes installation of BEDOPS binaries and scripts very easy for OS X 10.6 - 10.8 hosts.

  * Installer resolves fatal library errors seen by some end users of older OS X BEDOPS releases.

-------
v2.0.0b
-------

Released: **February 19, 2013**

* Added :code:`starchcluster` script variant which supports task distribution with `GNU Parallel <http://www.gnu.org/software/parallel/>`_.

* Fixed minor problem with :code:`bam2bed` and :code:`sam2bed` conversion scripts.

-------
v2.0.0a
-------

Released: **February 7, 2013**

* :ref:`bedmap`

  * Takes in Starch-formatted archives as input, as well as raw BED (i.e., it is no longer required to extract a Starch archive to an intermediate, temporary file or named pipe before applying operations).

  * New :code:`--chrom` operator jumps to and operates on information for specified chromosome only.

  * New :code:`--echo-map-id-uniq` operator lists unique IDs from overlapping mapping elements.

  * New :code:`--max-element` and :code:`--min-element` operators return the highest or lowest scoring overlapping map element.

* :ref:`bedops`

  * Takes in Starch-formatted archives as input, as well as raw BED.

  * New :code:`--chrom` operator jumps to and operates on information for specified chromosome only.

* :ref:`closest-features`

  * Takes in Starch-formatted archives as input, as well as raw BED.

  * New :code:`--chrom` operator jumps to and operates on information for specified chromosome only.

* :ref:`sort-bed` and ``bbms``

  * New :code:`--max-mem` option to limit system memory on large BED inputs.

  * Incorporated :code:`bbms` functionality into :code:`sort-bed` with use of :code:`--max-mem` operator.

* :ref:`starch`, :ref:`starchcat` and :ref:`unstarch`

  * New metadata enhancements to Starch-format archival and extraction, including: :code:`--note`, :code:`--elements`, :code:`--bases`, :code:`--bases-uniq`, :code:`--list-chromosomes`, :code:`--archive-timestamp`, :code:`--archive-type` and :code:`--archive-version` (see :code:`--help` to :code:`starch`, :code:`starchcat` and :code:`unstarch` binaries, or view the documentation for these applications for more detail).

  * Adds 20-35% performance boost to creating Starch archives with :code:`starch` utility.

  * New documentation with technical overview of the Starch format specification.

* :ref:`conversion scripts <conversion_scripts>`

  * New :code:`gtf2bed` conversion script, converting GTF (v2.2) to BED.

* Scripts are now part of main download; it is no longer necessary to download the BEDOPS companion separately.

-------
v1.2.5b
-------

Released: **January 14, 2013**

* Adds support for Apple 32- and 64-bit Intel hardware running OS X 10.5 through 10.8.

* Adds :code:`README` for companion download.

* Removes some obsolete code.

------
v1.2.5
------

Released: **October 13, 2012**

* Fixed unusual bug with :code:`unstarch`, where an extra (and incorrect) line of BED data can potentially be extracted from an archive.

* Updated companion download with updated :code:`bam2bed` and :code:`sam2bed` conversion scripts to address 0-indexing error with previous revisions.

------
v1.2.3
------

Released: **August 17, 2012**

* Added :code:`--indicator` option to :code:`bedmap`.

* Assorted changes to conversion scripts and associated companion download.

.. |--| unicode:: U+2013        .. en dash
.. |---| unicode:: U+2014       .. em dash, trimming surrounding whitespace
   :trim:
.. |plusminus| unicode:: U+00B1 .. plus-minus symbol
   :rtrim:
.. role:: bash(code)
   :language: bash
