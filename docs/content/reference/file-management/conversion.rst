.. _conversion_scripts:

Data conversion
===============

These scripts convert a variety of common genomic data types to BED with no loss of information. In using these tools, you can easily prepare data from these formats for use with core BEDOPS tools, whether VCF, GFF/GTF, BAM/SAM etc.

Some other formats not covered here can be converted with, for instance, the `UCSC Kent toolset <http://genomewiki.ucsc.edu/index.php/Kent_source_utilities>`_ (*e.g.*, altGraphX, axt, bigWig, bigBed, etc.). Just remember to use the :ref:`sort-bed` utility to prepare BED output from external programs for use with BEDOPS core tools.

.. toctree::

   conversion/bam2bed
   conversion/gff2bed
   conversion/gtf2bed
   conversion/psl2bed
   conversion/sam2bed
   conversion/vcf2bed
   conversion/wig2bed
