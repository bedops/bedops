BEDOPS
======
BEDOPS is a suite of tools to address common questions raised in genomic studies — mostly with regard to overlap and proximity relationships between data sets. It aims to be scalable and flexible, facilitating the efficient and accurate analysis and management of large-scale genomic data.

The suite includes tools for set and statistical operations (``bedops``, ``bedmap`` and ``closest-features``) and compression of large inputs into a novel lossless format (``starch``) that can provide greater space savings and faster data extractions than current alternatives. We offer native support for this compression format to these and other BEDOPS tools.

BEDOPS also offers logarithmic speedups in access to per-chromosome regions in sorted BED data (``bedextract``, core BEDOPS tools). These tools make whole-genome analyses "embarassingly parallel", in that per-chromosome computations can be placed onto separate work nodes, with results collated at the end in `map-reduce <http://en.wikipedia.org/wiki/MapReduce>`_ fashion.

=================
Table of contents
=================
.. toctree::
   :maxdepth: 3
   :numbered:

   content/overview
   content/installation
   content/quick-start
   content/usage-examples
   content/reference/set-operations
   content/reference/set-operations/bedops
   content/reference/set-operations/bedextract
   content/reference/set-operations/closest-features
   content/reference/statistics
   content/reference/statistics/bedmap
   content/reference/file-management
   content/reference/file-management/sorting
   content/reference/file-management/sorting/sort-bed
   content/reference/file-management/compression
   content/reference/file-management/compression/starch
   content/reference/file-management/compression/unstarch
   content/reference/file-management/compression/starchcat
   content/reference/file-management/conversion
   content/reference/file-management/conversion/bam2bed
   content/reference/file-management/conversion/gff2bed
   content/reference/file-management/conversion/gtf2bed
   content/reference/file-management/conversion/psl2bed
   content/reference/file-management/conversion/sam2bed
   content/reference/file-management/conversion/vcf2bed
   content/reference/file-management/conversion/wig2bed
   content/faq
