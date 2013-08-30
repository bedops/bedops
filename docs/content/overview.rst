Overview
========

========
Overview
========
BEDOPS is an open-source command-line toolkit that performs efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale.

The suite includes tools for set and statistical operations (``bedops``, ``bedmap`` and ``closest-features``) and compression of large inputs into a novel lossless format (``starch``) that can provide greater space savings and faster data extractions than current alternatives. We offer native support for this compression format to these and other BEDOPS tools.

BEDOPS also offers logarithmic speedups in access to per-chromosome regions in sorted BED data (in ``bedextract`` and core BEDOPS tools). This feature makes whole-genome analyses "embarassingly parallel", in that per-chromosome computations can be distributed onto separate work nodes, with results collated at the end in `map-reduce <http://en.wikipedia.org/wiki/MapReduce>`_ fashion.

Sorting arbitrarily large BED files is easy with ``sort-bed``, which easily scales beyond available system memory, as needed. We also offer portable conversion scripts that transform data in common genomic formats (SAM/BAM, GFF/GTF, PSL, WIG, and VCF) to sorted BED data that are ready to use with core BEDOPS utilities.

All of these tools are made to be glued together with common UNIX input and output streams. This helps make your pipeline design and maintenance easy, fast and flexible.

===============
Why use BEDOPS?
===============

-------------------------
BEDOPS tools are flexible
-------------------------

Our tools fit easily into analysis pipelines, allow practically unlimited inputs, and reduce I/O overhead through standard UNIX input and output streams:::

  $ bedops --intersect A.bed B.bed C.bed \
      | bedmap --echo --mean - D.bed \
      | ... \
      > Answer.bed

Our ``bedops`` and ``bedmap`` core tools offer numerous operations of all kinds, including those in the slide below:

.. image:: ../assets/overview/BEDOPS_Presentation-5.small.png

.. image:: ../assets/overview/BEDOPS_Presentation-6.small.png

-----------------------------------
BEDOPS tools are fast and efficient
-----------------------------------

BEDOPS tools take advantage of the information in a sorted BED file to use only what data are needed to perform the analysis. Our tools are agnostic about genomes: Run BEDOPS tools on genomes as small as *Circovirus* or as large as *Polychaos dubium*!

`Independent tests <http://www.ncbi.nlm.nih.gov/pubmed/23277498>`_ comparing various kits show that BEDOPS offers the fastest operations with the lowest memory overhead:

.. image:: ../assets/overview/BEDOPS_Presentation-9.small.png

BEDOPS also introduces a novel and **lossless** compression format called *Starch* that reduces whole-genome BED datasets to ~5% of their original size (and BAM datasets to roughly 35% of their original size), while adding useful metadata and random access, allowing instantaneous retrieval of any compressed chromosome:

.. image:: ../assets/overview/BEDOPS_Presentation-10.small.png

--------------------------------------------------------------
BEDOPS tools make your work embarrassingly easy to parallelize
--------------------------------------------------------------

BEDOPS tools introduce the ``--chrom`` option to efficiently locate a specified chromosome within a sorted BED file, useful for “embarrassingly parallel” whole-genome analyses, where work can be logically divided by units of chromosome in a "map-reduce" fashion.

-----------------------------------------------
BEDOPS tools are open, documented and supported
-----------------------------------------------

`BEDOPS <https://github.com/alexpreynolds/bedops>`_ is available as GPL-licensed source code and precompiled binaries for Linux and Mac OS X. We offer support through online forums such as our `own <http://bedops.uwencode.org/forum/>`_ and `Biostars <http://www.biostars.org>`_ and `recipes <https://bedops.readthedocs.org/en/latest/content/usage-examples.html>`_ showing BEDOPS tools in use for answering common research questions.