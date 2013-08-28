BEDOPS
======
BEDOPS is a suite of tools to address common questions raised in genomic studies — mostly with regard to overlap and proximity relationships between data sets. It aims to be scalable and flexible, facilitating the efficient and accurate analysis and management of large-scale genomic data.

The suite includes tools for set and statistical operations (``bedops``, ``bedmap`` and ``closest-features``) and compression of large inputs into a novel lossless format (``starch``) that can provide greater space savings and faster data extractions than current alternatives. We offer native support for this compression format to these and other BEDOPS tools.

BEDOPS also offers logarithmic speedups in access to per-chromosome regions in sorted BED data (in ``bedextract`` and core BEDOPS tools). This feature makes whole-genome analyses "embarassingly parallel", in that per-chromosome computations can be distributed onto separate work nodes, with results collated at the end in `map-reduce <http://en.wikipedia.org/wiki/MapReduce>`_ fashion.

Sorting BED files is easy with ``sort-bed``, which scales to arbitrarily large datasets. We also offer portable conversion tools that convert common genomic formats (SAM/BAM, GFF/GTF, PSL, WIG, and VCF) to sorted BED data that are ready to use with core BEDOPS utilities.

All of these tools are made to be glued together with common UNIX input and output streams. This helps make your pipeline design and maintenance easy, fast and flexible.

=================
Table of contents
=================
.. toctree::
   :numbered:

   content/overview
   content/installation
   content/quick-start
   content/usage-examples
   content/performance
   content/reference
   content/faq
