Overview
========
BEDOPS is an open-source command-line toolkit that performs efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale.

The suite includes tools for set and statistical operations (``bedops``, ``bedmap`` and ``closest-features``) and compression of large inputs into a novel lossless format (``starch``) that can provide greater space savings and faster data extractions than current alternatives. We offer native support for this compression format to these and other BEDOPS tools.

BEDOPS also offers logarithmic speedups in access to per-chromosome regions in sorted BED data (in ``bedextract`` and core BEDOPS tools). This feature makes whole-genome analyses "embarassingly parallel", in that per-chromosome computations can be distributed onto separate work nodes, with results collated at the end in `map-reduce <http://en.wikipedia.org/wiki/MapReduce>`_ fashion.

Sorting arbitrarily large BED files is easy with ``sort-bed``, which easily scales beyond available system memory, as needed. We also offer portable conversion scripts that transform data in common genomic formats (SAM/BAM, GFF/GTF, PSL, WIG, and VCF) to sorted BED data that are ready to use with core BEDOPS utilities.

All of these tools are made to be glued together with common UNIX input and output streams. This helps make your pipeline design and maintenance easy, fast and flexible.
