---
title: Revision history
...

This page summarizes some of the more important changes between
releases.

Current version
===============

v2.4.42
-------

Released: **April 10, 2025**

-   unstarch &lt;unstarch&gt;
    -   Patched [issue 284](https://github.com/bedops/bedops/issues/284)
        to resolve using `unstarch --gzip` on gzip-backed Starch archive
        with a zero-size chromosome, which can happen when a zero-byte
        file is passed into `starch`. The behavior now matches
        `unstarch --bzip2` on a bzip2-backed Starch archive, which
        returns no output and a zero exit status code (no error). Thanks
        to bgulko for the bug report.
-   convert2bed &lt;convert2bed&gt;
    -   Patched use of `gff2bed` for AUGUSTUS-formatted GFF annotations,
        addressing [issue
        275](https://github.com/bedops/bedops/issues/275). Thanks to
        shbrainard for the bug report.
    -   Patched use of `sam2bed` and `bam2bed` with soft-clipped reads
        and the --split operation, addressing [issue
        281](https://github.com/bedops/bedops/issues/281). Thanks to
        malyshev-andrey for the bug report.
-   starchcat &lt;starchcat&gt;
    -   Modifications to functions handling JSON metadata memory
        allocation and cleanup to reduce segmentation faults, addressing
        [issue 286](https://github.com/bedops/bedops/issues/286). Thanks
        to Mark Grivainis and Matt Maurano for the bug report.
-   General
    -   Updated readthedocs YAML setup per
        [documentation](https://blog.readthedocs.com/use-build-os-config/).
    -   Updated zlib from 1.2.7 to 1.3.1, which is the latest
        stable release. This is a minor update to the zlib library,
        which includes some bug fixes and performance improvements.

Previous versions
=================

v2.4.41
-------

Released: **July 13, 2022**

-   bedmap &lt;bedmap&gt;
    -   Patched [issue 267](https://github.com/bedops/bedops/issues/267)
        to remove unneeded `const` property from member functions, where
        it caused compilation problems with newer versions of
        Clang toolkit. Thanks to John Marshall and Shane Neph
        for feedback.
-   closest-features &lt;closest-features&gt;
    -   Added `--no-query` option to suppress printing "left" or "right"
        (or both) query regions to output. When combined with
        `--no-refs` the output will report raw distance values.
-   convert2bed &lt;convert2bed&gt;
    -   Patched `gtf2bed` and `gtf2starch` wrapper scripts to support
        `--attribute-key=foo` option, which previously only worked with
        `convert2bed --input=gtf --attribute-key=foo`.
    -   Addressed [issue
        255](https://github.com/bedops/bedops/issues/255) by printing a
        warning to standard error, which suggests using `--max-mem` and
        `--sort-tmpdir`, or `--do-not-sort`, to manage post-conversion
        sort order for inputs larger than half of system memory.
    -   Addressed [issue
        239](https://github.com/bedops/bedops/issues/239) by printing a
        warning to standard error, which suggests using
        `--multisplit=...` if Wiggle input contains multiple sections.
    -   Modified CIGAR string output of `bam2*` and `sam2*` conversion
        when used with the `--split` option, addressing [issue
        268](https://github.com/bedops/bedops/issues/268). Thanks to
        Lalit Mudgal for the bug report.
-   sort-bed &lt;sort-bed&gt;
    -   Resolved [issue
        265](https://github.com/bedops/bedops/issues/265) leading to
        overflow and early failure in sorting inputs with more than
        `MAX_INT` per-chromosome lines.
-   General
    -   Updated Ubuntu `main.yaml` test run on to `ubuntu-latest` image.
        Thanks to John Marshall for the advice.
    -   Modified `symlink_*` targets in `Makefile` to cull `xargs`
        warnings on Linux hosts. Thanks to Shane Neph for the report
        and fix.
    -   Removed C++11-deprecated calls to `binary_function<>` from
        utility library. Thanks to Shane Neph for the report and fix.
    -   Modified `switch-BEDOPS-binary-type` helper script to cull
        useless error messages.

v2.4.40
-------

Released: **July 21, 2021**

-   General
    -   Migrated from Travis CI to Github Actions for CI testing. Refer
        to
        [main.yml](https://github.com/bedops/bedops/actions/workflows/main.yml)
        for build logs.
    -   Modified Linux and Darwin makefiles to allow user-specified
        build flags (CXXFLAGS and CFLAGS) in conjunction with our
        preset values. Resolves [issue
        242](https://github.com/bedops/bedops/issues/242) for OS X and
        Linux targets.
-   convert2bed &lt;convert2bed&gt;
    -   Resolved [issue
        253](https://github.com/bedops/bedops/issues/253) preventing
        conversion of Wiggle-formatted data that use non-UCSC chromosome
        naming scheme.
    -   Modified wig2bed start and end shift arithmetic to ensure
        conversion from 1-based, fully-closed indexing to 0-based,
        half-open indexing.
    -   Added wig2bed integration tests. See tests/conversion/Makefile
        and wig2bed\_\* targets for more detail.
    -   In resolution of [issue
        244](https://github.com/bedops/bedops/issues/244) the `gtf2bed`
        and `gff2bed` conversion scripts now support copying a subset of
        reserved attributes to the ID field by keyname. By default,
        `gtf2bed` and `gff2bed` will parse the attributes string and
        copy the `gene_id` value to the output ID field (i.e.,
        fourth column). The `--attribute-key` option can be used to copy
        over `gene_name`, `transcript_name`, and several
        other attributes. See `gtf2bed --help`, `gff2bed --help`, or the
        online documentation for more information.
    -   Documentation updates for `gtf2bed` and `gff2bed`.
    -   Sample input updated for `gtf2bed` and `gff2bed` online
        documentation to resolve [issue
        240](https://github.com/bedops/bedops/issues/240).
    -   Conversion of GTF to BED would fail with an error where one of
        either the `gene_id` or `transcript_id` attribute is missing,
        such as in Ensembl-sourced GTF. This behavior has been changed
        to a warning.
    -   Application parameters that require strings are checked for
        `NULL` string values, returning with an error when a required
        parameter is missing. Resolves [issue
        256](https://github.com/bedops/bedops/issues/256) and more
        generally for other options.
-   starch &lt;starch&gt;
    -   Patched metadata generation function to resolve [issue
        248](https://github.com/bedops/bedops/issues/248), where the
        chromosome name would previously be truncated on a period
        character when creating a Starch archive.
-   bedmap &lt;bedmap&gt;
    -   Patched C++11-deprecated calls to `std::mem_fun` and
        `std::bind2nd` in `MultiVisitor.hpp` and
        `MedianAbsoluteDeviationVisitor.hpp` visitors to reduce
        compile-time warnings and improve C++11 compatibility.

v2.4.39
-------

Released: **April 6, 2020**

-   sort-bed &lt;sort-bed&gt;
    -   Patched `--unique`{.sourceCode} to report output identical to
        `sort -u`{.sourceCode}, in order to resolve [Issue
        236](https://github.com/bedops/bedops/issues/236).
-   unstarch &lt;unstarch&gt;
    -   Patched `--is-starch`{.sourceCode} test option to read only up
        to, at most, 8kb to check for v2 or v1 (legacy) Starch archive
        data, to resolve [Issue
        209](https://github.com/bedops/bedops/issues/209).
-   General
    -   Updated main `Makefile`{.sourceCode} to use
        [Homebrew](https://brew.sh/) GNU `coreutils`{.sourceCode} and
        `findutils`{.sourceCode} tools on the OS X target. If you build
        BEDOPS on OS X, you can add these tools with
        `brew install coreutils findutils`{.sourceCode}.

v2.4.38
-------

Released: **March 31, 2020**

-   convert2bed &lt;convert2bed&gt;
    -   Patched segmentation fault in malformed RepeatMasker
        input conversion. Thanks to [Mark
        Diekhans](https://github.com/bedops/bedops/issues/235) for the
        bug report.
    -   Patched abort and segmentation faults in malformed GVF, GFF,
        GTF, and WIG input conversion. Thanks to [Hongxu
        Chen](https://github.com/bedops/bedops/issues/217) for the
        bug report.
    -   Patched documentation and help message for BAM and
        SAM conversion. Thanks to [Zhuoer
        Dong](https://github.com/bedops/bedops/issues/215) for
        the report.
    -   Patched GTF conversion test suite.
-   General
    -   Updated outdated date information.

v2.4.37
-------

Released: **October 11, 2019**

-   starchcat &lt;starchcat&gt;
    -   A bug was introduced in v2.4.36 that would cause segmentation
        faults when concatenating disjoint Starch files, which is fixed
        in this version. Thanks to Eric Rynes for the bug report.
    -   Added a unit test to `tests/starch`{.sourceCode} to test this
        particular issue.
-   bedmap &lt;bedmap&gt;
    -   Running `bedmap --version`{.sourceCode} now exits with a
        zero (non-error/success) status.
-   starch &lt;starch&gt;
    -   When a Starch file with a header is provided as input to
        `bedops`{.sourceCode} or `bedmap`{.sourceCode}, the line is
        errantly processed as a BED interval. Thanks to [André M.
        Ribeiro-dos-Santos](https://github.com/bedops/bedops/pull/229)
        for patching the Starch C++ API to skip headers.
    -   Added a unit test to `tests/starch`{.sourceCode} to test
        headered Starch mapped against itself.
-   General
    -   Applied a placeholder workaround to whatever stupid bug was
        introducted in [Issue
        5709](https://github.com/readthedocs/readthedocs.org/issues/5709)
        that broke image serving for the document index (front page).
    -   Improved speed of generating random intervals in
        `tests/starch`{.sourceCode} unit tests.

v2.4.36
-------

Released: **May 2, 2019**

-   bedmap &lt;bedmap&gt;
    -   Resolved an issue preventing use of a `bash`{.sourceCode}
        process substitution or Unix named pipe in the reference
        position: *i.e.*,
        `bedmap --options <(processToGenerateReferenceElements) map.bed`{.sourceCode}
        and similar would issue incorrect output. Thanks to Wouter
        Meuleman and others for reports and test input.
    -   To avoid mapping problems, map elements should not contain
        spaces in the ID or subsequent non-interval fields. Use of the
        `--ec`{.sourceCode} can help identify problems in map input, at
        the cost of a longer runtime. The documentation is clarified to
        warn users about avoiding spaces in map input. Thanks to Wouter
        Meuleman for the report and providing test input.
    -   Added `--unmapped-val <val>`{.sourceCode} option, where
        `<val>`{.sourceCode} replaces the empty string output of
        `--echo-map*`{.sourceCode} operations when there are no
        mapped elements. The `--min/max-element`{.sourceCode} operators
        will give results as before (the empty string).
-   General
    -   Reduced
        `warning: zero as null pointer constant [-Wzero-as-null-pointer-constant]`{.sourceCode}
        compiler warnings via Clang.
    -   Begun work on a comprehensive test suite for
        BEDOPS applications. Tests are available via source distribution
        in `${root}/tests`{.sourceCode} and can be run by entering
        `make`{.sourceCode} in this directory.

v2.4.35
-------

Released: **May 2, 2018**

-   starch &lt;starch&gt;
    -   When compressing records, if the last interval in the former
        chromosome is identical to the first interval of the next
        chromosome, then a test on the sort order of the remainder
        string of that interval is applied (incorrectly). This is
        patched to test that chromosome names are identical before
        applying sort order rules. Thanks to Andrew Nishida for the
        report and for providing test input.

v2.4.34
-------

Released: **April 26, 2018**

-   convert2bed &lt;convert2bed&gt;
    -   In [Issue 208](https://github.com/bedops/bedops/issues/208)
        builds of convert2bed &lt;convert2bed&gt; would exit with an
        error state when converting SAM input with newline-delimited
        records longer than the 5 MB per-thread I/O buffer. The
        `C2B_THREAD_IO_BUFFER_SIZE`{.sourceCode} constant is now set to
        the suite-wide `TOKENS_MAX_LENGTH`{.sourceCode} value, which
        should make it easier to compile custom builds of BEDOPS that
        support very-long line lengths. Thanks to Erich Schwarz for the
        initial report.
-   starchstrip &lt;starchstrip&gt;
    -   When starchstrip is compiled with a C compiler,
        `qsort`{.sourceCode} uses a comparator that works correctly on
        the input chromosome list. When compiled with a C++ compiler
        (such as when building the larger BEDOPS toolkit), a different
        comparator is used that does not make variables of the correct
        type, and so the `qsort`{.sourceCode} result is garbage, leading
        to missing chromosomes. Thanks to Jemma Nelson for the
        initial report.

v2.4.33
-------

Released: **April 9, 2018**

-   convert2bed &lt;convert2bed&gt;
    -   Resolved [Issue
        207](https://github.com/bedops/bedops/issues/207) where a
        megarow build of convert2bed &lt;convert2bed&gt; would raise
        segmentation faults when converting SAM input. This build uses
        constants that define a longer BED line length. Arrays ended up
        using more stack than available, resulting in
        segmentation faults. This issue could potentially affect
        conversion of any data with the megarow build
        of convert2bed &lt;convert2bed&gt;. Arrays using
        megarow-constants were replaced with heap- or
        dynamically-allocated pointers. Thanks to Erich Schwarz for the
        initial report.

v2.4.32
-------

Released: **March 14, 2018**

-   New build type (128-bit precision floating point arithmetic,
    `float128`{.sourceCode})
    -   A new build type adds support for `long double`{.sourceCode} or
        128-bit floating point operations on measurement values in
        bedmap &lt;bedmap&gt;, such as is used with score operators
        like: `--min`{.sourceCode}, `--max`{.sourceCode},
        `--min-element`{.sourceCode}, `--max-element`{.sourceCode},
        `--mean`{.sourceCode}, and so on.
    -   This build includes support for measurements on values ranging
        from approximately ± 6.48e−4966 to ± 6.48e4966
        ([subnormal](https://en.wikipedia.org/wiki/Denormal_number)), or
        ± 1.19e-4932 to ± 1.19e4932 (normal), which enables
        bedmap &lt;bedmap&gt; to handle, for example, lower p-values
        without log- or other transformation preprocessing steps. The
        article on [quadruple
        precision](https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format)
        can be useful for technical review.
    -   For comparison, the current "non-float128" typical and megarow
        builds allow measurements on values from approximately ± 5e−324
        to ± 5e324 (subnormal) or ± 2.23e-308 to ± 2.23e308 (normal).
        Please refer to the article on [double
        precision](https://en.wikipedia.org/wiki/Double-precision_floating-point_format)
        for more technical detail.
    -   Please use `make float128 && make install_float128`{.sourceCode}
        to install this build type.
    -   This build type combines support for quadruple, 128-bit
        precision floats with the `typical`{.sourceCode} build type for
        handling "typical" BED4+ style line lengths. At this time,
        "megarow" support is not enabled with higher precision floats.
    -   This build will use more memory to store floating-point values
        with higher precision, and processing those data will require
        more computation time. It is recommended that this build be used
        only if analyses require a higher level of precision than what
        the `double`{.sourceCode} type allows.
-   OS X (Darwin) megarow build
    -   Some applications packaged in the OS X installer or compiled via
        the OS X command-line developer toolkit lacked
        [megarow](http://bedops.readthedocs.io/en/latest/content/revision-history.html#v2-4-27)
        build support, despite those flags being specified in the
        parent Makefile. These applications specifically were affected:
        `bedextract`{.sourceCode}, `bedmap`{.sourceCode}, and
        `convert2bed`{.sourceCode}. These binaries rely on wider
        suite-wide constants and data types that the megarow build
        variety specifies. The Darwin-specific Makefiles have been fixed
        to resolve this build issue, so that all OS X BEDOPS binaries
        should now be able to compile in the correct
        megarow-specific settings.

v2.4.31
-------

Released: **March 8, 2018**

-   User forum
    -   BEDOPS user forum moved domains from <http://bedops.stamlab.org>
        to <https://bedops.altius.org>
    -   Testing out administrator approval requirement for new forum
        accounts, to help try to reduce visits from spammers.
-   Documentation
    -   Updated Homebrew installation instructions per [issue
        202](https://github.com/bedops/bedops/issues/202) (thanks to
        user EricFromCanada).
-   wig2bed &lt;wig2bed&gt;
    -   Increased maximum length of chromosome name buffer to suite-wide
        `TOKEN_CHR_MAX_LENGTH`{.sourceCode} value, to reduce likelihood
        of segmentation faults (thanks to user ma-diroma).
-   General
    -   Updated copyright dates in source and headers.

v2.4.30
-------

Released: **November 25, 2017**

-   bedmap &lt;bedmap&gt;
    -   Errors are no longer reported when error checking is enabled and
        running in non-fast mode, when a fully-nested element
        is detected. This follows up on [issue
        199](https://github.com/bedops/bedops/issues/199).
-   starch &lt;starch&gt;
    -   Previously, a chromosome record in a Starch archive would result
        in corrupted metadata, if the chromosome is larger than
        `UINT32_MAX`{.sourceCode} bytes (\~4.3GB) in size
        when compressed. This limitation is now removed, and a single
        chromosome (when compressed in a Starch archive) can be up to
        `UINT64_MAX`{.sourceCode} bytes in size.
    -   The `starch`{.sourceCode} binary does more stringent input
        checks for the character lengths of ID and remainder strings,
        which must be no larger than 2^ID\_EXPONENT^ - 1 and
        2^REST\_EXPONENT^ - 1 characters in length. (These constants are
        specific to the build-time settings in the Makefile and in the
        app-wide constants.) This follows up on [issue
        195](https://github.com/bedops/bedops/issues/195).
-   starchcat &lt;starchcat&gt;
    -   Previously, a chromosome record in a Starch archive would result
        in corrupted metadata, if the chromosome is larger than
        `UINT32_MAX`{.sourceCode} bytes (\~4.3GB) in size
        when compressed. This limitation is now removed, and a single
        chromosome (when compressed in a Starch archive) can be up to
        `UINT64_MAX`{.sourceCode} bytes in size.
    -   More stringent memory management and stricter adherance to
        BEDOPS-wide constants, to help reduce likelihood of pointer
        access out of bounds and incidence of segfaults.
-   unstarch &lt;unstarch&gt;
    -   The `unstarch`{.sourceCode} binary implements the character
        length constants of ID and remainder strings, specific to the
        build-time settings in the Makefile and in the
        app-wide constants. This follows up on [issue
        195](https://github.com/bedops/bedops/issues/195).
-   sort-bed &lt;sort-bed&gt;
    -   Added `--unique`{.sourceCode} (`-u`{.sourceCode}) and
        `--duplicates`{.sourceCode} (`-d`{.sourceCode}) options to only
        print unique and duplicate in sorted output, to mimic the
        behavior of `sort -u`{.sourceCode} and `uniq -d`{.sourceCode}
        Unix tools. This follows up on [issue
        196](https://github.com/bedops/bedops/issues/196).
    -   Switched compile-time, stack-allocated `char`{.sourceCode}
        arrays to runtime, heap-based pointers. Timing tests on shuffled
        FIMO datasets suggest the impact from having to allocate space
        for buffers at runtime is very minimal. Moving from stack to
        heap will help avoid segfaults from running into OS-level stack
        limits, when BEDOPS-level constants change the maximum line
        length to something larger than the stack.
-   Revision testing
    -   Starch suite tests were updated for v2.2 archives created via
        v2.4.30 binaries (Linux, libc 2.22).

v2.4.29
-------

Released: **September 26, 2017**

-   bedmap &lt;bedmap&gt;
    -   Increased megarow build ID length up to 2^18^.
    -   Changed behavior of mapping to return mapped items in sort order
        provided in inputs. This follows up on [issue
        198](https://github.com/bedops/bedops/issues/198).
-   unstarch &lt;unstarch&gt;
    -   Changed behavior of `--is-starch`{.sourceCode} option to always
        return a successful exit code of `0`{.sourceCode} whether or not
        the input file is a Starch archive. It will now be up to the
        person running this option to test the 0 (false) or 1 (true)
        value printed to the standard output stream. This follows up on
        [issue 197](https://github.com/bedops/bedops/issues/197).

v2.4.28
-------

Released: **August 18, 2017**

-   bedmap &lt;bedmap&gt;
    -   Patched [issue 191](https://github.com/bedops/bedops/issues/191)
        where `--wmean`{.sourceCode} option was not recognized.
-   bedextract &lt;bedextract&gt;
    -   Updated documentation with fixed usage statement.
-   sort-bed &lt;sort-bed&gt;
    -   Patched typo in
        `update-sort-bed-starch-slurm.py`{.sourceCode} script.
    -   Fixed bug with `--max-mem`{.sourceCode} on properly ordering
        things on fourth and subsequent columns, when the genomic
        intervals are the same.
-   starch &lt;starch&gt;
    -   Updated Makefiles to remove lib on clean target and to help
        prevent `ARCH`{.sourceCode} variable from getting clobbered by
        third-party package managers.
-   Build process
    -   Updated the OS X installer XML to resolve missing asset links.
    -   Updated the `module_binaries`{.sourceCode} target to copy over
        `starchcluster_*`{.sourceCode} and `starch-diff`{.sourceCode}
        assets for `modules`{.sourceCode} distributions.

v2.4.27
-------

Released: **July 17, 2017**

This revision of BEDOPS includes significant performance improvements
for core tools: `bedops`{.sourceCode}, `bedmap`{.sourceCode}, and
`closest-features`{.sourceCode}. Performance tests were done with
whole-genome TRANSFAC FIMO scans, with cache purges in between trials.

Pre-built binaries for Darwin and GNU/Linux targets include both the
default `typical`{.sourceCode} and `megarow`{.sourceCode} builds of
BEDOPS. The program names that you are accustomed to will remain as-is,
but the binaries will exist as symbolic links pointing to the
`typical`{.sourceCode} builds. These links can be repointed to the
`megarow`{.sourceCode} builds by calling
`switch-BEDOPS-binary-type --megarow`{.sourceCode}, which will set the
usual BEDOPS binaries to link to the `megarow`{.sourceCode} builds. One
can run `switch-BEDOPS-binary-type --typical`{.sourceCode} at any time
to revert to the default (`typical`{.sourceCode}) builds.

The top-level Makefile includes some new variables for those who choose
to build from source. The `JPARALLEL`{.sourceCode} variable sets the
number of CPUs to use in parallel when compiling BEDOPS, which can speed
compilation time dramatically. The `MASSIVE_REST_EXP`{.sourceCode},
`MASSIVE_ID_EXP`{.sourceCode}, and `MASSIVE_CHROM_EXP`{.sourceCode} are
used when building the `megarow`{.sourceCode} to support any required
row lengths (build using `make megarow`{.sourceCode}). These are the
exponents (the *n* in 2^n^) for holding all characters after chromosome,
start, and stop fields, the ID field (column 4, typically), and the
chromosome field (column 1).

To simplify distribution and support, we have removed pre-built 32-bit
program versions in this release. These can be built from source by
specifying the correct `ARCH`{.sourceCode} value in the top-level
Makefile. For OS X, our package installer now requires OS X version
10.10 or greater.

Application-level notes follow:

-   bedops &lt;bedops&gt;
    -   Performance of `bedops`{.sourceCode} tool improved, doing
        typical work in **76.5%** of the time of all previous versions.
    -   Performance of `-u`{.sourceCode}/`--everything`{.sourceCode} has
        improved, doing the same work in only **55.6%** of the time of
        previous versions when given a large number of input files.
    -   The `megarow`{.sourceCode} build of this application handles
        input files with very long rows (4M+ characters). Such input
        might arise from conversion of very-long-read BAM files to BED
        via `bam2bed`{.sourceCode}, such as those that may come from
        Nanopore or PacBio MinION platforms. This build requires more
        runtime memory than the default (`typical`{.sourceCode}) build.
        Pertinent variables for `megarow`{.sourceCode} execution can be
        modified through the make system without changing source.
-   bedmap &lt;bedmap&gt;
    -   Performance of `bedmap`{.sourceCode} tool improved, doing the
        same work in **86.7%** of the time of all previous versions.
    -   Automatically use `--faster`{.sourceCode} option when
        `--exact`{.sourceCode} is used as the overlap criterion, or if
        the input files are formatted as Starch archives, no
        fully-nested elements exist in the archives, and the overlap
        criterion supports `--faster`{.sourceCode} (such as
        `--bp-ovr`{.sourceCode}, `--exact`{.sourceCode}, and
        `--range`{.sourceCode}).
    -   The `megarow`{.sourceCode} build target handles input files with
        very long rows (4M+ characters). Such input might arise from
        conversion of very-long-read BAM files to BED via
        `bam2bed`{.sourceCode}, such as those that may come from
        Nanopore or PacBio MinION platforms. This build requires more
        runtime memory than the default (`typical`{.sourceCode}) build.
        Pertinent variables for `megarow`{.sourceCode} execution can be
        modified through the make system without changing source.
    -   New `--min-memory`{.sourceCode} option for use when the
        reference file has very large regions, and the map file has many
        small regions that fall within those larger regions. One example
        is when `--range 100000`{.sourceCode} is used and the map file
        consists of whole-genome motif scan hits. Memory overhead can be
        reduced to that used by all previous versions, up to and
        including v2.4.26.
    -   Added `--faster`{.sourceCode} automatically when
        `--exact`{.sourceCode} is used, which is robust even when nested
        elements exist in inputs. Similarly, `--faster`{.sourceCode} is
        used automatically when inputs are Starch-formatted archives,
        none of which have nested elements (see
        `unstarch --has-nested`{.sourceCode}) when the overlap criterion
        allows for `--faster`{.sourceCode}.
-   closest-features &lt;closest-features&gt;
    -   Performance of `closest-features`{.sourceCode} tool has been
        improved, doing the same work in **87.7%** of the time of all
        previous versions.
    -   The `megarow`{.sourceCode} build target is available to compile
        a version of the program that can handle input files with very
        long rows (4M+ characters). This requires more runtime memory
        than the default build. Pertinent variables can be modified
        through the make system without editing source.
-   convert2bed &lt;convert2bed&gt;

    Numerous internal changes, including giving line functors the
    ability to resize the destination (write) buffer in mid-stream,
    along with increased integration with BEDOPS-wide constants.
    Destination buffer resizing is particularly useful when converting
    very-long-read BAM files containing numerous D (deletion)
    operations, such as when used with the new
    `--split-with-deletions`{.sourceCode} option.

    -   psl2bed &lt;psl2bed&gt;
        -   Migrated storage of PSL conversion state from stack to heap,
            which helps address segmentation faults on OS X (thanks to
            <rmartson@Biostars> for the bug report).
    -   bam2bed &lt;bam2bed&gt; and sam2bed &lt;sam2bed&gt;
        -   Increased thread I/O heap buffer size to reduce likelihood
            of overflows while parsing reads from Nanopore and
            PacBio platforms.
        -   Added `--split-with-deletions`{.sourceCode} option to split
            spliced junctions by `N`{.sourceCode} and `D`{.sourceCode}
            CIGAR operations. The `--split`{.sourceCode} option now
            splits only on `N`{.sourceCode} operations.
        -   Added `--reduced`{.sourceCode} option to print first six
            columns of BED data to standard output.
    -   gff2bed &lt;gff2bed&gt;
        -   Resolved issue parsing GFF input with
            `##FASTA`{.sourceCode} directive.
-   sort-bed &lt;sort-bed&gt;
    -   The `megarow`{.sourceCode} build target is available to compile
        a version of the program that can handle input files with very
        long rows (4M+ characters). This requires more runtime memory
        than the default build. The pertinent variables can be modified
        through the make system without changing source. This is useful
        for converting ultra-long reads from Nanopore and PacBio
        sequencing platforms to BED via `bam2bed`{.sourceCode} /
        `convert2bed`{.sourceCode}.
-   starch &lt;starch&gt;
    -   Fixed a potential segmentation fault result with
        `--header`{.sourceCode} usage.
-   Starch C++ API
    -   Fixed output from `bedops -u`{.sourceCode}
        (`--everything`{.sourceCode}, or multiset union) on two or more
        Starch archives, where the remainder string was not being
        cleared correctly.
-   starch-diff &lt;starch\_diff&gt;
    -   Improved usage statement to clarify output (cf. [Issue
        180](https://github.com/bedops/bedops/issues/180)).
-   Clang warnings
    -   Resolved compilation warnings for several binaries.

v2.4.26
-------

Released: **March 14, 2017**

-   starchstrip &lt;starchstrip&gt;
    -   New utility to efficiently filter a Starch archive, including or
        excluding records by specified chromosome names, without doing
        expensive extraction and recompression. This follows up on
        [internal
        discussion](https://stamlab.slack.com/archives/bedops/p1487878245000103)
        on the Altius Slack channel.
-   starch-diff &lt;starch\_diff&gt;
    -   Fixed testing logic in `starch-diff`{.sourceCode} for
        certain archives. Thanks to Shane Neph for the report.
-   starchcat &lt;starchcat&gt;
    -   Fixed possible condition where too many variables on the stack
        can cause a stack overload on some platforms, leading to a fatal
        segmentation fault. Improved logic for updating v2.1 to v2.2
        Starch archives.
-   Starch C++ API
    -   Patched gzip-backed Starch archive extraction issue. Thanks to
        Matt Maurano for the bug report.
-   update-sort-bed-migrate-candidates &lt;sort-bed&gt;
    -   Added detailed logging via `--debug`{.sourceCode} option.
    -   Added `--bedops-root-dir`{.sourceCode} option to allow
        specifying where all BEDOPS binaries are stored. This setting
        can be overruled on a per-binary basis by adding
        `--bedextract-path`{.sourceCode},
        `--sort-bed-path`{.sourceCode}, etc.
    -   Added `--non-recursive-search`{.sourceCode} option to restrict
        search for BED and Starch candidates to the top-level of the
        specified parent directory `--parent-dir`{.sourceCode} option.
    -   Further simplification and customization of parameters sent to
        `update-sort-bed-slurm`{.sourceCode} and
        `update-sort-bed-starch-slurm`{.sourceCode} cluster scripts, as
        well as logging and variable name improvements to those
        two scripts.
    -   Thanks again to Matt Maurano for ongoing feedback and
        suggestions on functionality and fixes.
-   gtf2bed &lt;gtf2bed&gt;
    -   Resolved segmentation fault with certain inputs, in follow-up to
        [this BEDOPS Forum
        post](http://bedops.uwencode.org/forum/index.php?topic=136.0).
        Thanks to zebasilio for the report and feedback.

v2.4.25
-------

Released: **February 15, 2017**

-   convert2bed &lt;convert2bed&gt;
    -   Patch for RepeatMasker inputs with blank lines that have
        no spaces. This follows up on [Issue
        173](https://github.com/bedops/bedops/issues/173). Thanks to
        saketkc for the bug report.
-   update-sort-bed-migrate-candidates &lt;sort-bed&gt;

    The `update-sort-bed-migrate-candidates`{.sourceCode} utility
    recursively searches into the specified directory for BED and Starch
    files which fail a `sort-bed --check-sort`{.sourceCode} test. Those
    files which fail this test can have their paths written to a text
    file for further downstream processing, or the end user can decide
    to apply an immediate resort on those files, either locally or via a
    SLURM-managed cluster. Grateful thanks to Matt Maurano for input
    and testing.

    See `update-sort-bed-migrate-candidates --help`{.sourceCode} for
    more information, or review the
    sort-bed &lt;sort-bed&gt; documentation.

-   update-sort-bed-starch-slurm &lt;sort-bed&gt;

    This is an adjunct to the `update-sort-bed-slurm`{.sourceCode}
    utility, which resorts the provided Starch file and writes a
    new file. (The `update-sort-bed-slurm`{.sourceCode} utility only
    takes in BED files as input and writes BED as output.)

v2.4.24
-------

Released: **February 6, 2017**

-   starch-diff &lt;starch\_diff&gt;
    -   The `starch-diff`{.sourceCode} utility compares signatures of
        two or more v2.2+ Starch archives. This tool tests all
        chromosomes or one specified chromosome. It returns a zero exit
        code, if the signature(s) are identical, or a non-zero error
        exit code, if one or more signature(s) are dissimilar.
-   update-sort-bed-slurm &lt;sort-bed&gt;
    -   The `update-sort-bed-slurm`{.sourceCode} convenience utility
        provides a parallelized update of the sort order on BED files
        sorted with pre-v2.4.20 sort-bed, for users with a SLURM job
        scheduler and associated cluster. See
        `update-sort-bed-slurm --help`{.sourceCode} for more details.
-   convert2bed &lt;convert2bed&gt;
    -   Patched a memory leak in VCF conversion. Thanks to ehsueh for
        the bug report.

v2.4.23
-------

Released: **January 30, 2017**

-   unstarch &lt;unstarch&gt;
    -   Fixed bug where missing signature from pre-v2.2 Starch archives
        would cause a fatal metadata error. Thanks to Shane Neph and
        Eric Rynes for the bug report.
    -   Improved logic reporting signature mismatches when input v2.2
        archive lacks signature (*e.g.*, for a v2.2 archive made with
        `--omit-signature`{.sourceCode}).
-   starch &lt;starch&gt; and starchcat &lt;starchcat&gt;
    -   Added `--omit-signature`{.sourceCode} option to compress without
        creating a per-chromosome data integrity signature. While this
        reduces compression time, this eliminates the verification
        benefits of the data integrity signature.

v2.4.22
-------

Released: **January 25, 2017**

-   convert2bed &lt;convert2bed&gt;
    -   Fixed heap corruption in GFF conversion. Thanks to J. Miguel
        Mendez (ObjectiveTruth) for the bug report.

v2.4.21
-------

Released: **January 23, 2017**

-   bedmap &lt;bedmap&gt;
    -   New `--wmean`{.sourceCode} operation offers a weighted
        mean calculation. The "weight" is derived from the proportion of
        the reference element covered by overlapping map elements:
        *i.e.*, a map element that covers more of the reference element
        has its signal given a larger weight or greater impact than
        another map element with a shorter overlap.
    -   Measurement values in `bedmap`{.sourceCode} did not allow
        `+`{.sourceCode} in the exponent (both `-`{.sourceCode} worked
        and no `+`{.sourceCode} for a positive value. Similarly, out in
        front of the number, `+`{.sourceCode} was previously
        not allowed. Shane Neph posted the report and the fix.
    -   The `--min-element`{.sourceCode} and
        `--max-element`{.sourceCode} operations in bedmap &lt;bedmap&gt;
        now process elements in unambiguous order. Former behavior is
        moved to the operations `--min-element-rand`{.sourceCode} and
        `--max-element-rand`{.sourceCode}, respectively.
    -   Fixed issue with use of `--echo-overlap-size`{.sourceCode} with
        `--multidelim`{.sourceCode} (cf. [Issue
        165](https://github.com/bedops/bedops/issues/165)). Shane Neph
        posted the fix. Thanks to Jeff Vierstra for the bug report!
-   bedops &lt;bedops&gt;
    -   Fixed issue with `--chop`{.sourceCode} where complement
        operation could potentially be included. Shane Neph posted
        the fix.
    -   The `bedops --everything`{.sourceCode} or
        `bedops -u`{.sourceCode} (union) operation now writes elements
        to standard output in unambiguous sort order. If any data are
        contained in fourth or subsequent fields, a lexicographical sort
        on that data is applied for resolving order of interval matches.
-   sort-bed &lt;sort-bed&gt;
    -   Improved sort times from replacing quicksort
        (`std::qsort`{.sourceCode}) with inlined C++
        `std::sort`{.sourceCode}.
    -   Sorting of BED input now leads to unambiguous result when two or
        more elements have the same genomic interval (chromosome name
        and start and stop position), but different content in remaining
        columns (ID, score, etc.).

        Formerly, elements with the same genomic interval that have
        different content in fourth and subsequent columns could be
        printed in a non-consistent ordering on repeated sorts. A
        deterministic sort order facilitates the use of data integrity
        functions on sorted BED and Starch data.

-   starchcluster &lt;starchcluster&gt;
    -   A SLURM-ready version of the `starchcluster`{.sourceCode} script
        was added to help SLURM job scheduler users with parallelizing
        the creation of Starch archives.
-   Parallel bam2bed &lt;parallel\_bam2bed&gt; and
    bam2starch &lt;parallel\_bam2starch&gt;
    -   SLURM-ready versions of these scripts were added to help
        parallelize the conversion of BAM to BED files
        (`bam2bed_slurm`{.sourceCode}) or to Starch archives
        (`bam2starch_slurm`{.sourceCode}).
-   unstarch &lt;unstarch&gt;
    -   Added `--signature`{.sourceCode} option to report the
        Base64-encoded SHA-1 data integrity signature of the
        Starch-transformed bytes of a specified chromosome, or to report
        the signature of the metadata string as well as the signatures
        of all chromosomes, if unspecified.
    -   Added `--verify-signature`{.sourceCode} option to compare the
        "expected" Base64-encoded SHA-1 data integrity signature stored
        within the archive's metadata with the "observed" data integrity
        signature generated from extracting the specified chromosome.

        If the observed and expected signatures differ, then this
        suggests that the chromosome record may be corrupted in some
        way; `unstarch`{.sourceCode} will exit with a non-zero
        error code. If the signatures agree, the archive data should be
        intact and unstarch will exit with a helpful notice and a zero
        error code.

        If no chromosome is specified, `unstarch`{.sourceCode} will loop
        through all chromosomes in the archive metadata, comparing
        observed and expected values for each chromosome record. Upon
        completion, error and progress messages will be reported to the
        standard error stream, and `unstarch`{.sourceCode} will exit
        with a zero error code, if all signatures match, or a non-zero
        exit state, if one or more signatures do not agree.

    -   The output from the `--list`{.sourceCode} option includes a
        `signature`{.sourceCode} column to report the data integrity
        signature of all Starch-transformed chromosome data.
    -   The output from the `--list-json`{.sourceCode} option includes a
        `signature`{.sourceCode} key in each chromosome record in the
        archive metadata, reporting the same information.
    -   The `--is-starch`{.sourceCode} option now quits with a non-zero
        exit code, if the specified input file is not a Starch archive.
    -   The `--elements-max-string-length`{.sourceCode} option reports
        the length of the longest string within the specified
        chromosome, or the longest string over all chromosomes (if no
        chromosome name is specified).

-   starch &lt;starch&gt;
    -   Added `--report-progress=N`{.sourceCode} option to (optionally)
        report compression of the Nth element of the current chromosome
        to standard error stream.
    -   As a chromosome is compressed, the input Starch-transform bytes
        are continually run through a SHA-1 hash function. The resulting
        data integrity signature is stored as a Base64-encoded string in
        the output archive's metadata. Signatures can be compared
        between and within archives to help better ensure the data
        integrity of the archive.
    -   Fixed `--header`{.sourceCode} transform bug reported in [Issue
        161](https://github.com/bedops/bedops/issues/161). Thanks to
        Shane Neph for the bug report!
    -   Added chromosome name and "remainder" order tests to
        `STARCH2_transformHeaderlessBEDInput`{.sourceCode} and
        `STARCH2_transformHeaderedBEDInput`{.sourceCode} functions.

        Compression with `starch`{.sourceCode} ends with a fatal error,
        should any of the following comparison tests fail:

        1.  The chromosome names are not lexicographically ordered
            (*e.g.*, `chr1`{.sourceCode} records coming after
            `chr2`{.sourceCode} records indicates the data are not
            correctly sorted).
        2.  The start position of an input element is less than the
            start position of a previous input element on the same
            chromosome (*e.g.*, `chr1:1000-1234`{.sourceCode} coming
            after `chr1:2000-2345`{.sourceCode} is not
            correctly sorted).
        3.  The stop positions of two or more input elements are not in
            ascending order when their start positions are equal
            (*e.g.*, `chr1:1000-1234`{.sourceCode} coming after
            `chr1:1000-2345`{.sourceCode} is not correctly sorted).
        4.  The start and stop positions of two or more input elements
            are equivalent, and their "remainders" (fourth and
            subsequent columns) are not in ascending order (*e.g.*,
            `chr1:1000-1234:id-0`{.sourceCode} coming after
            `chr1:1000-1234:id-1`{.sourceCode} is not correctly sorted).

        If the sort order of the input data is unknown or uncertain,
        simply use `sort-bed`{.sourceCode} to generate the correct
        ordering and pipe the output from that to `starch`{.sourceCode},
        *e.g.*
        `$ cat elements.bed | sort-bed - | starch - > elements.starch`{.sourceCode}.

-   starchcat &lt;starchcat&gt;
    -   Added `--report-progress=N`{.sourceCode} option to (optionally)
        report compression of the *N* th element of the current
        chromosome to standard error stream.
    -   As in `starch`{.sourceCode}, at the conclusion of compressing a
        chromosome made from one or more input Starch archives, the
        input Starch-transform bytes are continually run through a SHA-1
        hash function. The resulting data integrity signature is stored
        as a Base64-encoded string in the chromosome's entry in the new
        archive's metadata.
    -   As in `starch`{.sourceCode}, if data should need to be extracted
        and recompressed, the output is written so that the order is
        unambiguous: ascending lexicographic ordering on chromosome
        names, numerical ordering on start positions, the same ordering
        on stop positions where start positions match, and ascending
        lexicographic ordering on the remainder of the BED element
        (fourth and subsequent columns, where present).
-   convert2bed &lt;convert2bed&gt;
    -   Improvements in support for BAM/SAM inputs with larger-sized
        reads, as would come from alignments made from data collected
        from third-generation sequencers. Simulated read datasets were
        generated using
        [SimLoRD](https://bitbucket.org/genomeinformatics/simlord/).
        Tests have been performed on simulated hg19 data up to 100kb
        read lengths.

        Improvements allow:

        -   conversion of dynamic number of CIGAR operations (up to
            system memory)
        -   conversion of dynamically-sized read fields (up to system
            memory and inter-thread buffer allocations)

        These patches follow up on bug reports in [Issue
        157](https://github.com/bedops/bedops/issues/157).

    -   Improvements in support for VCF inputs, to allow aribtrary-sized
        fields (up to system memory and inter-thread buffer
        allocations), which should reduce or eliminate segmentation
        faults from buffer overruns on fields larger than former
        stack defaults.
    -   Improvements in support for GFF inputs, to allow aribtrary-sized
        fields (up to system memory and inter-thread buffer
        allocations), which should reduce or eliminate segmentation
        faults from buffer overruns on fields larger than former
        stack defaults.
    -   Improvements in support for GTF inputs, to allow aribtrary-sized
        fields (up to system memory and inter-thread buffer
        allocations), which should reduce or eliminate segmentation
        faults from buffer overruns on fields larger than former
        stack defaults.

-   Testing
    -   Our use of Travis CI to automate testing of builds now includes
        Clang on [their OS X
        environment](https://docs.travis-ci.com/user/osx-ci-environment/).

v2.4.20
-------

Released: **July 27, 2016**

-   convert2bed &lt;convert2bed&gt;
    -   Increased memory allocation for maximum number of per-read CIGAR
        operations in BAM and SAM conversion to help improve stability.
        Thanks to Adam Freedman for the report!
    -   Improved reliability of gene ID parsing from GTF input, where
        `gene_id`{.sourceCode} field may be positioned at start, middle,
        or end of attributes string, or may be empty. Thanks to blaiseli
        for the report!

v2.4.19
-------

Released: **May 9, 2016**

-   convert2bed &lt;convert2bed&gt;
    -   Fixed bug in BAM and SAM parallel conversion scripts
        (`*_gnuParallel`{.sourceCode} and `*_sge`{.sourceCode}) with
        inputs containing chromosome names without
        `chr`{.sourceCode} prefix. Thanks to Eric Haugen for the bug
        report!
-   Starch C++ API
    -   Fixed bug with extraction of bzip2- and gzip-backed archives
        with all other non-primary Starch tools (all tools except
        `starch`{.sourceCode}, `unstarch`{.sourceCode},
        `starchcat`{.sourceCode}, and `sort-bed`{.sourceCode}). Thanks
        to Eric Haugen for the bug report!

v2.4.18
-------

Released: **April 28, 2016**

-   convert2bed &lt;convert2bed&gt;
    -   Fixed compile warnings.
    -   Fixed bug in BAM and SAM conversion with optional field
        line overflow. Thanks to Jemma Nelson for the bug report!
-   General documentation improvements
    -   Updated OS X Installer and Github release instructions
    -   Added thank-you to Feng Tian for bug report

v2.4.17
-------

Released: **April 26, 2016**

-   bam2bed &lt;bam2bed&gt; and sam2bed &lt;sam2bed&gt;
    -   Improved parsing of non-split BAM and SAM inputs.
-   Docker container build target added for Debian
    -   Thanks to Leo Comitale (Poldo) for writing a Makefile target and
        spec for creating a BEDOPS Docker container for the
        Debian target.
-   Starch C++ API
    -   Fixed bug with extraction of bzip2- and gzip-backed archives
        with all other non-primary Starch tools (all tools except
        `starch`{.sourceCode}, `unstarch`{.sourceCode},
        `starchcat`{.sourceCode}, and `sort-bed`{.sourceCode}). Thanks
        to Feng Tian for reports.

v2.4.16
-------

Released: **April 5, 2016**

-   bedmap &lt;bedmap&gt;
    -   Added new `--echo-ref-row-id`{.sourceCode} option to report
        reference row ID elements.
-   Starch C++ API
    -   Fixed bug with extraction of archives made with
        `starch --gzip`{.sourceCode} (thanks to Brad Gulko for the bug
        report and Paul Verhoeven and Peter Weir for compile and
        testing assistance).
-   General improvements
    -   Small improvements to build cleanup targets.

v2.4.15
-------

Released: **January 21, 2016**

-   Docker container build target added for CentOS 7
    -   Thanks to Leo Comitale (Poldo) for writing a Makefile target and
        spec for creating a BEDOPS Docker container for CentOS 7.
-   convert2bed &lt;convert2bed&gt;
    -   Fixed buffer overflows in `convert2bed`{.sourceCode} to improve
        conversion reliability for VCF files (thanks to Jared Andrews
        and Kousik Kundu for bug reports).
-   General improvements
    -   Improved OS X 10.11 build process.

v2.4.14
-------

Released: **April 21, 2015**

-   convert2bed &lt;convert2bed&gt;
    -   Fixed missing `samtools`{.sourceCode} variable references in
        cluster conversion scripts (thanks to Brad Gulko for the
        bug report).
-   General suite-wide improvements
    -   Fixed exception error message for `stdin`{.sourceCode} check
        (thanks to Brad Gulko for the bug report).

v2.4.13
-------

Released: **April 20, 2015**

-   bedops &lt;bedops&gt;
    -   Resolved issue in using `--ec`{.sourceCode} with
        `bedops`{.sourceCode} when reading from `stdin`{.sourceCode}
        (thanks to Brad Gulko for the bug report).
-   General suite-wide improvements
    -   Addressed inconsistency with constants defined for the suite at
        the extreme end of the limits we allow for coordinate values
        (thanks again to Brad Gulko for the report).

v2.4.12
-------

Released: **March 13, 2015**

-   bedops &lt;bedops&gt;
    -   Checks have been added to determine if an integer argument is a
        file in the current working directory, before interpreting that
        argument as an overlap criterion for `-e`{.sourceCode} and
        `-n`{.sourceCode} options.

        To reduce ambiguity, if an integer is used as a file input,
        `bedops`{.sourceCode} issues a warning of the interpretation and
        provides guidance on how to force that value to instead be used
        as an overlap specification, if desired (thanks to E. Rynes for
        the pointer).

-   bedmap &lt;bedmap&gt;
    -   Added support for `--prec`{.sourceCode} / `--sci`{.sourceCode}
        with `--min-element`{.sourceCode} and
        `--max-element`{.sourceCode} operations (thanks to E. Rynes for
        the pointer).
-   bedops &lt;bedops&gt; | bedmap &lt;bedmap&gt; |
    closest-features &lt;closest-features&gt;
    -   Added support for `bash`{.sourceCode} process substitution/named
        pipes with specification of `--chrom`{.sourceCode} and/or
        `--ec`{.sourceCode} options (thanks to B. Gulko for the
        bug report).
    -   Fixed code that extracts `gzip`{.sourceCode}-backed Starch
        archives from `bedops`{.sourceCode} and other core tools (thanks
        again to B. Gulko for the bug report).
-   convert2bed &lt;convert2bed&gt;
    -   Switched `matches`{.sourceCode} and `qSize`{.sourceCode} fields
        in order of `psl2bed`{.sourceCode} output. Refer to
        documentation for new field order.
    -   Added null sentinel to GTF ID value.
    -   To help reduce the chance of buffer overflows, the
        `convert2bed`{.sourceCode} tool increases the maximum field
        length from 8191 to 24575 characters to allow parsing of inputs
        with longer field length, such as very long attributes from
        [mosquito
        GFF3](https://www.vectorbase.org/download/aedes-aegypti-liverpoolbasefeaturesaaegl33gff3gz)
        data (thanks to T. Karginov for the bug report).

v2.4.11
-------

Released: **February 24, 2015**

-   convert2bed &lt;convert2bed&gt;
    -   Fixed bug in `psl2bed`{.sourceCode} where `matches`{.sourceCode}
        column value was truncated by one character. Updated unit tests.
        Thanks to M. Wirthlin for the bug report.

v2.4.10
-------

Released: **February 23, 2015**

-   starch &lt;starch&gt;
    -   In addition to checking chromosome interleaving, the
        `starch`{.sourceCode} tool now enforces `sort-bed`{.sourceCode}
        sort ordering on BED input and exits with an
        `EINVAL`{.sourceCode} POSIX error code if the data are not
        sorted correctly.
-   convert2bed &lt;convert2bed&gt;
    -   Added `--zero-indexed`{.sourceCode} option to
        `wig2bed`{.sourceCode} and `wig2starch`{.sourceCode} wrappers
        and `convert2bed`{.sourceCode} binary, which converts WIG data
        that are zero-indexed without any coordinate adjustments. This
        is useful for WIG data sourced from the UCSC Kent tool
        `bigWigToWig`{.sourceCode}, where the `bigWig`{.sourceCode} data
        can potentially be sourced from 0-indexed BAM- or
        bedGraph-formatted data.
    -   If the WIG input contains any element with a start coordinate of
        0, the default use of `wig2bed`{.sourceCode},
        `wig2starch`{.sourceCode} and `convert2bed`{.sourceCode} will
        exit early with an error condition, suggesting the use of
        `--zero-indexed`{.sourceCode}.
    -   Updated copyright date range of wrapper scripts

v2.4.9
------

Released: **February 17, 2015**

-   sort-bed &lt;sort-bed&gt;
    -   Added support for `--check-sort`{.sourceCode} to report if input
        is sorted (or not)
-   Starch
    -   Improved support for `starch --header`{.sourceCode}, where
        header contains tab-delimited fields
-   Starch C++ API
    -   Fixed bug with `starch --header`{.sourceCode} functionality,
        such that BEDOPS core tools (`bedops`{.sourceCode}, etc.) would
        be unable to extract correct data from headered Starch archive

v2.4.8
------

Released: **February 7, 2015**

-   Mac OS X packaging
    -   Installer signed with
        [productsign](https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/productsign.1.html#//apple_ref/doc/man/1/productsign)
        to pass [OS X
        Gatekeeper](http://support.apple.com/en-us/HT202491)
-   Linux packaging
    -   SHA1 hashes of each tarball are now part of the [BEDOPS
        Releases](https://github.com/bedops/bedops/releases/)
        description page, going forwards
-   Updated copyright dates in source code

v2.4.7
------

Released: **February 2, 2015**

-   convert2bed &lt;convert2bed&gt; fixes and improvements
    -   Fixed `--split`{.sourceCode} support in `psl2bed`{.sourceCode}
        (thanks to Marco A.)
    -   Fixed compilation warning regarding comparison of signed and
        unsigned values
    -   Fixed corrupted `psl2bed`{.sourceCode} test inputs

v2.4.6
------

Released: **January 30, 2015**

-   convert2bed &lt;convert2bed&gt; fixes and improvements
    -   Added support for conversion of the [GVF file
        format](http://www.sequenceontology.org/resources/gvf.html#summary),
        including wrapper scripts and unit tests. Refer to the
        `gvf2bed`{.sourceCode} documentation for more information.
    -   Fixed bug in string copy of zero-length element attribute for
        `gff2bed`{.sourceCode} and `gtf2bed`{.sourceCode} (GFF and GTF)
        formats
-   General fixes and improvements
    -   Fixed possibly corrupt bzip2, Jansson and zlib tarballs (thanks
        to rekado, Shane N. and Richard S.)
    -   Fixed typo in `bedextract`{.sourceCode} documentation
    -   Fixed broken image in Overview &lt;overview&gt;
    -   Removed 19 MB `_build`{.sourceCode} intermediate result
        directory (which should improve overall `git clone`{.sourceCode}
        time considerably!)

v2.4.5
------

Released: **January 28, 2015**

-   convert2bed &lt;convert2bed&gt; improvements
    -   Addition of RepeatMasker annotation output (`.out`{.sourceCode})
        file conversion support, `rmsk2bed`{.sourceCode} and
        `rmsk2starch`{.sourceCode} wrappers, and unit tests

v2.4.4
------

Released: **January 25, 2015**

-   Documentation improvements
    -   Implemented substantial style changes via [A Better Sphinx
        Theme](http://github.com/irskep/sphinx-better-theme) and
        various customizations. We also include responsive web style
        elements to help improve browsing on mobile devices.
    -   Fixes to typos in conversion and other documents.

v2.4.3
------

Released: **December 18, 2014**

-   Compilation improvements
    -   Shane Neph put in a great deal of work to enable parallel builds
        (*e.g.*, `make -j N`{.sourceCode} to build various targets
        in parallel). Depending on the end user's environment, this can
        speed up compilation time by a factor of 2, 4 or more.
    -   Fixed numerous compilation warnings of debug builds of
        `starch`{.sourceCode} toolkit under RHEL6/GCC and OS
        X 10.10.1/LLVM.
-   New bedops features
    -   Added `--chop`{.sourceCode} and `--stagger`{.sourceCode} options
        to "melt" inputs into contiguous or staggered disjoint regions
        of equivalent size.
    -   For less confusion, arguments for `--element-of`{.sourceCode},
        `--chop`{.sourceCode} and other `bedops`{.sourceCode} operations
        that take numerical modifiers no longer require a leading
        hyphen character. For instance, `--element-of 1`{.sourceCode} is
        now equivalent to the former usage of
        `--element-of -1`{.sourceCode}.
-   New bedmap features
    -   The `--sweep-all`{.sourceCode} option reads through the entire
        map file without early termination and can help deal with
        `SIGPIPE`{.sourceCode} errors. It adds to execution time, but
        the penalty is not as severe as with the use of
        `--ec`{.sourceCode}. Using `--ec`{.sourceCode} alone will enable
        error checking, but will now no longer read through the entire
        map file. The `--ec`{.sourceCode} option can be used in
        conjunction with `--sweep-all`{.sourceCode}, with the associated
        time penalties. (Another method for dealing with issue this is
        to override how `SIGPIPE`{.sourceCode} errors are caught by the
        interpreter (`bash`{.sourceCode}, Python, *etc.*) and retrapping
        them or ignoring them. However, it may not a good idea to do
        this as other situations may arise in production pipelines where
        it is ideal to trap and handle all I/O errors in a
        default manner.)
    -   New `--echo-ref-size`{.sourceCode} and
        `--echo-ref-name`{.sourceCode} operations report genomic length
        of reference element, and rename the reference element in
        `chrom:start-end`{.sourceCode} (useful for labeling rows for
        input for `matrix2png`{.sourceCode} or `R`{.sourceCode} or
        other applications).
-   bedextract
    -   Fixed upper bound bug that would cause incorrect output in some
        cases
-   conversion scripts &lt;conversion\_scripts&gt;
    -   Brand new C99 binary called `convert2bed`{.sourceCode}, which
        wrapper scripts (`bam2bed`{.sourceCode}, *etc.*) now call. No
        more Python version dependencies, and the C-based rewrite offers
        massive performance improvements over old Python-based scripts.
    -   Added `parallel_bam2starch`{.sourceCode} script, which
        parallelizes creation of Starch &lt;starch\_specification&gt;
        archive from very large BAM files in SGE environments.
    -   Added bug fix for missing code in
        starchcluster.gnu\_parallel &lt;starchcluster&gt; script, where
        the final collation step was missing.
    -   The `vcf2bed`{.sourceCode} script now accepts the
        `--do-not-split`{.sourceCode} option, which prints one BED
        element for all alternate alleles.
-   Starch &lt;starch\_specification&gt; archival format and
    compression/extraction tools
    -   Added duplicate- and nested-element &lt;nested\_elements&gt;
        flags in v2.1 of Starch metadata, which denote if a chromosome
        contains one or more duplicate and/or nested elements. BED files
        compressed with `starch`{.sourceCode} v2.5 or greater, or Starch
        archives updated with `starchcat`{.sourceCode} v2.5 or greater
        will include these values in the archive metadata. The
        `unstarch`{.sourceCode} extraction tool offers
        `--has-duplicate`{.sourceCode} and `--has-nested`{.sourceCode}
        options to retrieve these flag values for a specified chromosome
        (or for all chromosomes).
    -   Added `--is-starch`{.sourceCode} option to
        `unstarch`{.sourceCode} to test if specified input file is a
        Starch v1 or v2 archive.
    -   Added bug fix for compressing BED files with
        `starch`{.sourceCode}, where the archive would not include the
        last element of the BED input, if the BED input lacked a
        trailing newline. The compression tools now include a routine
        for capturing the last line, if there is no newline.
-   Documentation improvements
    -   Remade some image assets throughout the documents to support
        Retina-grade displays

v2.4.2
------

Released: **April 10, 2014**

-   conversion scripts &lt;conversion\_scripts&gt;
    -   Added support for `sort-bed --tmpdir`{.sourceCode} option to
        conversion scripts, to allow specification of alternative
        temporary directory for sorted results when used in conjunction
        with `--max-mem`{.sourceCode} option.
    -   Added support for GFF3 files which include a FASTA directive in
        `gff2bed`{.sourceCode} and `gff2starch`{.sourceCode} (thanks to
        Keith Hughitt).
    -   Extended support for Python-based conversion scripts to support
        use with Python v2.6.2 and forwards, except for
        `sam2bed`{.sourceCode} and `sam2starch`{.sourceCode}, which
        still require Python v2.7 or greater (and under Python3).
    -   Fixed `--insertions`{.sourceCode} option in
        `vcf2bed`{.sourceCode} to now report a single-base BED element
        (thanks to Matt Maurano).

v2.4.1
------

Released: **February 26, 2014**

-   bedmap
    -   Added `--fraction-both`{.sourceCode} and `--exact`{.sourceCode}
        (`--fraction-both 1`{.sourceCode}) to list of compatible overlap
        options with `--faster`{.sourceCode}.
    -   Added 5% performance improvement with `bedmap`{.sourceCode}
        operations without `--faster`{.sourceCode}.
    -   Fixed scenario that can yield incorrect results (cf. [Issue
        43](https://github.com/bedops/bedops/issues/43)).
-   sort-bed
    -   Added `--tmpdir`{.sourceCode} option to allow specification of
        an alternative temporary directory, when used in conjunction
        with `--max-mem`{.sourceCode} option. This is useful if the host
        operating system's standard temporary directory (*e.g.*,
        `/tmp`{.sourceCode} on Linux or OS X) does not have sufficient
        space to hold intermediate results.
-   All conversion scripts &lt;conversion\_scripts&gt;
    -   Improvements to error handling in Python-based conversion
        scripts, in the case where no input is specified.
    -   Fixed typos in `gff2bed`{.sourceCode} and `psl2bed`{.sourceCode}
        documentation (cf. [commit
        a091e18](https://github.com/bedops/bedops/commit/a091e18)).
-   OS X compilation improvements
    -   We have completed changes to the OS X build process for the
        remaining half of the BEDOPS binaries, which now allows direct,
        full compilation with Clang/LLVM (part of the Apple
        Xcode distribution).

        All OS X BEDOPS binaries now use Apple's system-level C++
        library, instead of GNU's `libstdc++`{.sourceCode}. It is no
        longer required (or recommended) to use GNU `gcc`{.sourceCode}
        to compile BEDOPS on OS X.

        Compilation is faster and simpler, and we can reduce the size
        and complexity of Mac OS X builds and installer packages. By
        using Apple's C++ library, we also eliminate the likelihood of
        missing library errors.

        In the longer term, this gets us closer to moving BEDOPS to
        using the CMake build system, to further abstract and simplify
        the build process.

-   Cleaned up various compilation warnings found with
    `clang`{.sourceCode} / `clang++`{.sourceCode} and GCC kits.

v2.4.0
------

Released: **January 9, 2014**

-   bedmap
    -   Added new `--echo-map-size`{.sourceCode} and
        `--echo-overlap-size`{.sourceCode} options to calculate sizes of
        mapped elements and overlaps between mapped and
        reference elements.
    -   Improved performance for all
        `--echo-map-*`{.sourceCode} operations.
    -   Updated documentation.
-   Major enhancements and fixes to sort-bed:
    -   Improved performance.
    -   Fixed memory leak.
    -   Added support for millions of distinct chromosomes.
    -   Improved internal estimation of memory usage with
        `--max-mem`{.sourceCode} option.
-   Added support for compilation on Cygwin (64-bit). Refer to the
    installation documentation &lt;installation\_via\_source\_code\_on\_cygwin&gt;
    for build instructions.
-   starchcat
    -   Fixed embarassing buffer overflow condition that caused
        segmentation faults on Ubuntu 13.
-   All conversion scripts &lt;conversion\_scripts&gt;
    -   Python-based scripts no longer use temporary files, which
        reduces file I/O and improves performance. This change also
        reduces the need for large amounts of free space in a user's
        `/tmp`{.sourceCode} folder, particularly relevant for users
        converting multi-GB BAM files.
    -   We now test for ability to locate `starch`{.sourceCode},
        `sort-bed`{.sourceCode}, `wig2bed_bin`{.sourceCode} and
        `samtools`{.sourceCode} in user environment, quitting with the
        appropriate error state if the dependencies cannot be found.
    -   Improved documentation. In particular, we have added descriptive
        tables to each script's documentation page which describe how
        columns map from original data input to BED output.
    -   bam2bed and sam2bed
        -   Added `--custom-tags <value>`{.sourceCode} command-line
            option to support a comma-separated list of custom tags (cf.
            [Biostars discussion](http://www.biostars.org/p/87062/)),
            *i.e.*, tags which are not part of the original
            SAMtools specification.
        -   Added `--keep-header`{.sourceCode} option to preserve header
            and metadata as BED elements that use `_header`{.sourceCode}
            as the chromosome name. This now makes these conversion
            scripts fully "non-lossy".
    -   vcf2bed
        -   Added new `--snvs`{.sourceCode}, `--insertions`{.sourceCode}
            and `--deletions`{.sourceCode} options that filter VCF
            variants into three separate subcategories.
        -   Added `--keep-header`{.sourceCode} option to preserve header
            and metadata as BED elements that use `_header`{.sourceCode}
            as the chromosome name. This now makes these conversion
            scripts fully "non-lossy".
    -   gff2bed
        -   Added `--keep-header`{.sourceCode} option to preserve header
            and metadata as BED elements that use `_header`{.sourceCode}
            as the chromosome name. This now makes these conversion
            scripts fully "non-lossy".
    -   psl2bed
        -   Added `--keep-header`{.sourceCode} option to preserve header
            and metadata as BED elements that use `_header`{.sourceCode}
            as the chromosome name. This now makes these conversion
            scripts fully "non-lossy".
    -   wig2bed
        -   Added `--keep-header`{.sourceCode} option to
            `wig2bed`{.sourceCode} binary and `wig2bed`{.sourceCode} /
            `wig2starch`{.sourceCode} wrapper scripts, to preserve
            header and metadata as BED elements that use
            `_header`{.sourceCode} as the chromosome name. This now
            makes these conversion scripts fully "non-lossy".
-   Added OS X uninstaller project to allow end user to more easily
    remove BEDOPS tools from this platform.
-   Cleaned up various compilation warnings found with
    `clang`{.sourceCode} / `clang++`{.sourceCode} and GCC kits.

v2.3.0
------

Released: **October 2, 2013**

-   Migration of BEDOPS code and documentation from Google Code
    to Github.
    -   Due to changes with Google Code hosting policies at the end of
        the year, we have decided to change our process for distributing
        code, packages and documentation. While most of the work is
        done, we appreciate feedback on any problems you may encounter.
        Please email us at
        [<bedops@stamlab.org>](mailto:bedops@stamlab.org) with details.
    -   Migration to Github should facilitate requests for code by those
        who are familiar with `git`{.sourceCode} and want to fork our
        project to submit [pull
        requests](https://help.github.com/articles/using-pull-requests).
-   bedops
    -   General `--ec`{.sourceCode} performance improvements.
-   bedmap
    -   Adds support for the new `--skip-unmapped`{.sourceCode} option,
        which filters out reference elements which do not have mapped
        elements associated with them. See the end of the
        score operations &lt;bedmap\_score\_operations&gt; section of
        the bedmap documentation for more detail.
    -   General `--ec`{.sourceCode} performance improvements.
-   starch
    -   Fixed bug with `starch`{.sourceCode} where zero-byte BED input
        (*i.e.*, an "empty set") created a truncated and
        unusable archive. We now put in a "dummy" chromosome for
        zero-byte input, which `unstarch`{.sourceCode} can now unpack.

        This should simplify error handling with certain pipelines,
        specifically where set or other BEDOPS operations yield an
        "empty set" BED file that is subsequently compressed with
        `starch`{.sourceCode}.

-   unstarch
    -   Can now unpack zero-byte ("empty set") compressed
        `starch`{.sourceCode} archive (see above).
    -   Changed `unstarch --list`{.sourceCode} option to print to
        `stdout`{.sourceCode} stream (this was previously sent to
        `stderr`{.sourceCode}).
-   starch metadata library
    -   Fixed array overflow bug with BEDOPS tools that take
        starch &lt;starch\_specification&gt; archives as inputs, which
        affected use of archives as inputs to
        `closest-features`{.sourceCode}, `bedops`{.sourceCode} and
        `bedmap`{.sourceCode}.
-   All conversion scripts &lt;conversion\_scripts&gt;
    -   Python scripts require v2.7+ or greater.
    -   Improved (more "Pythonic") error code handling.
    -   Disabled support for `--max-mem`{.sourceCode} sort parameter
        until sort-bed
        [issue](https://github.com/bedops/bedops/issues/1) is resolved.
        Scripts will continue to sort, but they will be limited to
        available system memory. If you are processing files larger than
        system memory, please contact us at
        [<bedops@stamlab.org>](mailto:bedops@stamlab.org) for details of
        a temporary workaround.
-   gff2bed conversion script
    -   Resolved `IndexError`{.sourceCode} exceptions by fixing header
        support, bringing script in line with [v1.21 GFF3
        spec](http://www.sequenceontology.org/gff3.shtml).
-   bam2bed and sam2bed conversion scripts
    -   Rewritten `bam2*`{.sourceCode} and `sam2*`{.sourceCode} scripts
        from `bash`{.sourceCode} into Python (v2.7+ support).
    -   Improved BAM and SAM input validation against the [v1.4 SAM
        spec](http://samtools.sourceforge.net/SAMv1.pdf).
    -   New `--split`{.sourceCode} option prints reads with
        `N`{.sourceCode} CIGAR operations as separated BED elements.
    -   New `--all-reads`{.sourceCode} option prints all reads, mapped
        and unmapped.
-   bedextract
    -   Fixed `stdin`{.sourceCode} bug with `bedextract`{.sourceCode}.
-   New documentation via [readthedocs.org](readthedocs.org).
    -   Documentation is now part of the BEDOPS distribution, instead of
        being a separate download.
    -   We use [readthedocs.org](readthedocs.org) to host indexed and
        searchable HTML.
    -   [PDF and
        eBook](https://readthedocs.org/projects/bedops/downloads/)
        documents are also available for download.
    -   Documentation is refreshed and simplified, with new installation
        and compilation guides.
-   OS X compilation improvements
    -   We have made changes to the OS X build process for half of the
        BEDOPS binaries, which allows direct compilation with Clang/LLVM
        (part of the Apple Xcode distribution). Those binaries now use
        Apple's system-level C++ library, instead of GNU's
        `libstdc++`{.sourceCode}.

        This change means that we require Mac OS X 10.7 ("Lion") or
        greater — we do not support 10.6 at this time.

        Compilation is faster and simpler, and we can reduce the size
        and complexity of Mac OS X builds and installer packages. By
        using Apple's C++ library, we also reduce the likelihood of
        missing library errors. When this process is completed for the
        remaining binaries, it will no longer be necessary to install
        GCC 4.7+ (by way of MacPorts or other package managers) in order
        to build BEDOPS on OS X, nor will we have to bundle
        `libstdc++`{.sourceCode} with the installer.

v2.2.0b
-------

-   Fixed bug with OS X installer's post-installation scripts.

v2.2.0
------

Released: **May 22, 2013**

-   Updated packages
    -   Precompiled packages are now available for Linux (32-
        and 64-bit) and Mac OS X 10.6-10.8 (32- and 64-bit) hosts.
-   Starch v2 test suite &lt;starch\_specification&gt;
    -   We have added a test suite for the Starch archive toolkit with
        the source download. Test inputs include randomized BED data
        generated from chromosome and bounds data stored on UCSC servers
        as well as static FIMO search results. Tests put
        `starch`{.sourceCode}, `unstarch`{.sourceCode} and
        `starchcat`{.sourceCode} through various usage scenarios. Please
        refer to the Starch-specific Makefiles and the test target and
        subfolder's README doc for more information.
-   starchcat
    -   Resolves bug with `--gzip`{.sourceCode} option, allowing updates
        of `gzip`{.sourceCode} -backed v1.2 and v1.5 archives to the
        v2 Starch format &lt;starch\_specification&gt; (either
        `bzip2`{.sourceCode} - or `gzip`{.sourceCode} -backed).
-   unstarch
    -   Resolves bug with extraction of Starch &lt;starch&gt; archive
        made from BED files with four or more columns. A condition where
        the total length of additional columns exceeds a certain number
        of characters would result in extracted data in those columns
        being cut off. As an example, this could affect Starch archives
        made from the raw, uncut output of GTF- and
        GFF- conversion scripts &lt;conversion\_scripts&gt;.
-   conversion scripts &lt;conversion\_scripts&gt;
    -   We have partially reverted `wig2bed`{.sourceCode}, providing a
        Bash shell wrapper to the original C binary. This preserves
        consistency of command-line options across the conversion suite,
        while making use of the C binary to recover performance lost
        from the Python-based v2.1 revision of `wig2bed`{.sourceCode}
        (which at this time is no longer supported). (Thanks to Matt
        Maurano for reporting this issue.)

v2.1.1
------

Released: **May 3, 2013**

-   bedmap
    -   Major performance improvements made in v2.1.1, such that current
        `bedmap`{.sourceCode} now operates as fast or faster than the
        v1.2.5 version of `bedmap`{.sourceCode}!
-   bedops
    -   Resolves bug with `--partition`{.sourceCode} option.
-   conversion scripts &lt;conversion\_scripts&gt;
    -   All v2.1.0 Python-based scripts now include fix for
        `SIGPIPE`{.sourceCode} handling, such that use of
        `head`{.sourceCode} or other common UNIX utilities to process
        buffered standard output no longer yields
        `IOError`{.sourceCode} exceptions. (Thanks to Matt Maurano for
        reporting this bug.)
-   32-bit Linux binary support
    -   Pre-built Linux binaries are now available for end users with
        32-bit workstations.

Other issues fixed:

-   Jansson tarball no longer includes already-compiled libraries that
    could potentially interfere with 32-bit builds.
-   Minor changes to conversion script test suite to exit with useful
    error code on successful completion of test.

v2.1.0
------

Released: **April 22, 2013**

-   bedops
    -   New `--partition`{.sourceCode} operator efficiently generates
        disjoint segments made from genomic boundaries of all
        overlapping inputs.
-   conversion scripts &lt;conversion\_scripts&gt;
    -   All scripts now use `sort-bed`{.sourceCode} behind the scenes to
        output sorted BED output, ready for use with BEDOPS utilities.
        It is no longer necessary to pipe data to or otherwise
        post-process converted data with `sort-bed`{.sourceCode}.
    -   New `psl2bed`{.sourceCode} conversion script, converting
        [PSL-formatted UCSC BLAT
        output](http://genome.ucsc.edu/FAQ/FAQformat.html#format2)
        to BED.
    -   New `wig2bed`{.sourceCode} conversion script written in Python.
    -   New `*2starch`{.sourceCode}
        conversion scripts &lt;conversion\_scripts&gt; offered for all
        `*2bed`{.sourceCode} scripts, which output Starch v2 archives.
-   closest-features
    -   Replaced `--shortest`{.sourceCode} option name with
        `--closest`{.sourceCode}, for clarity. (Old scripts which use
        `--shortest`{.sourceCode} will continue to work with the
        deprecated option name for now. We advise editing pipelines,
        as needed.)
-   starch
    -   Improved error checking for interleaved records. This also makes
        use of `*2starch`{.sourceCode} conversion scripts with the
        `--do-not-sort`{.sourceCode} option safer.
-   Improved Mac OS X support
    -   New Mac OS X package installer makes installation of BEDOPS
        binaries and scripts very easy for OS X 10.6 - 10.8 hosts.
    -   Installer resolves fatal library errors seen by some end users
        of older OS X BEDOPS releases.

v2.0.0b
-------

Released: **February 19, 2013**

-   Added `starchcluster`{.sourceCode} script variant which supports
    task distribution with [GNU
    Parallel](http://www.gnu.org/software/parallel/).
-   Fixed minor problem with `bam2bed`{.sourceCode} and
    `sam2bed`{.sourceCode} conversion scripts.

v2.0.0a
-------

Released: **February 7, 2013**

-   bedmap
    -   Takes in Starch-formatted archives as input, as well as raw BED
        (i.e., it is no longer required to extract a Starch archive to
        an intermediate, temporary file or named pipe before
        applying operations).
    -   New `--chrom`{.sourceCode} operator jumps to and operates on
        information for specified chromosome only.
    -   New `--echo-map-id-uniq`{.sourceCode} operator lists unique IDs
        from overlapping mapping elements.
    -   New `--max-element`{.sourceCode} and
        `--min-element`{.sourceCode} operators return the highest or
        lowest scoring overlapping map element.
-   bedops
    -   Takes in Starch-formatted archives as input, as well as raw BED.
    -   New `--chrom`{.sourceCode} operator jumps to and operates on
        information for specified chromosome only.
-   closest-features
    -   Takes in Starch-formatted archives as input, as well as raw BED.
    -   New `--chrom`{.sourceCode} operator jumps to and operates on
        information for specified chromosome only.
-   sort-bed and `bbms`
    -   New `--max-mem`{.sourceCode} option to limit system memory on
        large BED inputs.
    -   Incorporated `bbms`{.sourceCode} functionality into
        `sort-bed`{.sourceCode} with use of
        `--max-mem`{.sourceCode} operator.
-   starch, starchcat and unstarch
    -   New metadata enhancements to Starch-format archival and
        extraction, including: `--note`{.sourceCode},
        `--elements`{.sourceCode}, `--bases`{.sourceCode},
        `--bases-uniq`{.sourceCode}, `--list-chromosomes`{.sourceCode},
        `--archive-timestamp`{.sourceCode},
        `--archive-type`{.sourceCode} and
        `--archive-version`{.sourceCode} (see `--help`{.sourceCode} to
        `starch`{.sourceCode}, `starchcat`{.sourceCode} and
        `unstarch`{.sourceCode} binaries, or view the documentation for
        these applications for more detail).
    -   Adds 20-35% performance boost to creating Starch archives with
        `starch`{.sourceCode} utility.
    -   New documentation with technical overview of the Starch
        format specification.
-   conversion scripts &lt;conversion\_scripts&gt;
    -   New `gtf2bed`{.sourceCode} conversion script, converting
        GTF (v2.2) to BED.
-   Scripts are now part of main download; it is no longer necessary to
    download the BEDOPS companion separately.

v1.2.5b
-------

Released: **January 14, 2013**

-   Adds support for Apple 32- and 64-bit Intel hardware running OS X
    10.5 through 10.8.
-   Adds `README`{.sourceCode} for companion download.
-   Removes some obsolete code.

v1.2.5
------

Released: **October 13, 2012**

-   Fixed unusual bug with `unstarch`{.sourceCode}, where an extra
    (and incorrect) line of BED data can potentially be extracted from
    an archive.
-   Updated companion download with updated `bam2bed`{.sourceCode} and
    `sam2bed`{.sourceCode} conversion scripts to address 0-indexing
    error with previous revisions.

v1.2.3
------

Released: **August 17, 2012**

-   Added `--indicator`{.sourceCode} option to `bedmap`{.sourceCode}.
-   Assorted changes to conversion scripts and associated
    companion download.
