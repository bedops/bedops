.. _vcf2bed:

`vcf2bed`
=========

The ``vcf2bed`` script converts 1-based, closed ``[start, end]`` `Variant Call Format v4 <http://vcftools.sourceforge.net/specs.html>`_ (VCF) to sorted, 0-based, half-open ``[start-1, end)`` extended BED data.

For convenience, we also offer ``vcf2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

============
Dependencies
============

The ``vcf2bed`` script requires Python, version 2.5 or greater.

This script is also dependent on input that follows the VCF specification. 

.. tip:: Conversion of data which are VCF-like, but which do not follow the specification can cause ``IOError`` and other runtime exceptions. If you run into problems, please check that your input follows the VCF specification.

======
Source
======

The ``vcf2bed`` and ``vcf2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``vcf2bed`` script parses VCF from standard input and prints sorted BED to standard output. The ``vcf2starch`` script uses an extra step to parse VCF to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If you are sorting data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample VCF input called ``foo.vcf`` (see the :ref:`Downloads <vcf2bed_downloads>` section to grab this file). 

.. note:: This data is also publicly available from the `Broad Institute <http://www.broadinstitute.org/gsa/wiki/index.php/Understanding_the_Unified_Genotyper's_VCF_files>`_.

::

  ##fileformat=VCFv4.0
  ##FILTER=<ID=LowQual,Description="QUAL < 50.0">
  ##FORMAT=<ID=AD,Number=.,Type=Integer,Description="Allelic depths for the ref and alt alleles in the order listed">
  ##FORMAT=<ID=DP,Number=1,Type=Integer,Description="Read Depth (only filtered reads used for calling)">
  ##FORMAT=<ID=GQ,Number=1,Type=Float,Description="Genotype Quality">
  ##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
  ##FORMAT=<ID=PL,Number=3,Type=Float,Description="Normalized, Phred-scaled likelihoods for AA,AB,BB genotypes where A=ref and B=alt; not applicable if site is not biallelic">
  ##INFO=<ID=AC,Number=.,Type=Integer,Description="Allele count in genotypes, for each ALT allele, in the same order as listed">
  ##INFO=<ID=AF,Number=.,Type=Float,Description="Allele Frequency, for each ALT allele, in the same order as listed">
  ##INFO=<ID=AN,Number=1,Type=Integer,Description="Total number of alleles in called genotypes">
  ##INFO=<ID=DB,Number=0,Type=Flag,Description="dbSNP Membership">
  ##INFO=<ID=DP,Number=1,Type=Integer,Description="Total Depth">
  ##INFO=<ID=DS,Number=0,Type=Flag,Description="Were any of the samples downsampled?">
  ##INFO=<ID=Dels,Number=1,Type=Float,Description="Fraction of Reads Containing Spanning Deletions">
  ##INFO=<ID=HRun,Number=1,Type=Integer,Description="Largest Contiguous Homopolymer Run of Variant Allele In Either Direction">
  ##INFO=<ID=HaplotypeScore,Number=1,Type=Float,Description="Consistency of the site with two (and only two) segregating haplotypes">
  ##INFO=<ID=MQ,Number=1,Type=Float,Description="RMS Mapping Quality">
  ##INFO=<ID=MQ0,Number=1,Type=Integer,Description="Total Mapping Quality Zero Reads">
  ##INFO=<ID=QD,Number=1,Type=Float,Description="Variant Confidence/Quality by Depth">
  ##INFO=<ID=SB,Number=1,Type=Float,Description="Strand Bias">
  ##INFO=<ID=VQSLOD,Number=1,Type=Float,Description="log10-scaled probability of variant being true under the trained gaussian mixture model">
  ##UnifiedGenotyperV2="analysis_type=UnifiedGenotyperV2 input_file=[TEXT CLIPPED FOR CLARITY]"
  #CHROM  POS     ID      REF     ALT     QUAL    FILTER  INFO    FORMAT  NA12878
  chr1    873762  .       T       G       5231.78 PASS    AC=1;AF=0.50;AN=2;DP=315;Dels=0.00;HRun=2;HaplotypeScore=15.11;MQ=91.05;MQ0=15;QD=16.61;SB=-1533.02;VQSLOD=-1.5473      GT:AD:DP:GQ:PL  0/1:173,141:282:99:255,0,255
  chr1    877664  rs3828047       A       G       3931.66 PASS    AC=2;AF=1.00;AN=2;DB;DP=105;Dels=0.00;HRun=1;HaplotypeScore=1.59;MQ=92.52;MQ0=4;QD=37.44;SB=-1152.13;VQSLOD=0.1185      GT:AD:DP:GQ:PL  1/1:0,105:94:99:255,255,0
  chr1    899282  rs28548431      C       T       71.77   PASS    AC=1;AF=0.50;AN=2;DB;DP=4;Dels=0.00;HRun=0;HaplotypeScore=0.00;MQ=99.00;MQ0=0;QD=17.94;SB=-46.55;VQSLOD=-1.9148 GT:AD:DP:GQ:PL  0/1:1,3:4:25.92:103,0,26
  chr1    974165  rs9442391       T       C       29.84   LowQual AC=1;AF=0.50;AN=2;DB;DP=18;Dels=0.00;HRun=1;HaplotypeScore=0.16;MQ=95.26;MQ0=0;QD=1.66;SB=-0.98 GT:AD:DP:GQ:PL  0/1:14,4:14:60.91:61,0,255

We can convert VCF to sorted BED data in the following manner:

::

  $ vcf2bed < foo.vcf
  chr1    873761  873762  .       5231.78 T       G       PASS    AC=1;AF=0.50;AN=2;DP=315;Dels=0.00;HRun=2;HaplotypeScore=15.11;MQ=91.05;MQ0=15;QD=16.61;SB=-1533.02;VQSLOD=-1.5473      GT:AD:DP:GQ:PL  0/1:173,141:282:99:255,0,255
  chr1    877663  877664  rs3828047       3931.66 A       G       PASS    AC=2;AF=1.00;AN=2;DB;DP=105;Dels=0.00;HRun=1;HaplotypeScore=1.59;MQ=92.52;MQ0=4;QD=37.44;SB=-1152.13;VQSLOD=0.1185      GT:AD:DP:GQ:PL  1/1:0,105:94:99:255,255,0
  chr1    899281  899282  rs28548431      71.77   C       T       PASS    AC=1;AF=0.50;AN=2;DB;DP=4;Dels=0.00;HRun=0;HaplotypeScore=0.00;MQ=99.00;MQ0=0;QD=17.94;SB=-46.55;VQSLOD=-1.9148 GT:AD:DP:GQ:PL  0/1:1,3:4:25.92:103,0,26
  chr1    974164  974165  rs9442391       29.84   T       C       LowQual AC=1;AF=0.50;AN=2;DB;DP=18;Dels=0.00;HRun=1;HaplotypeScore=0.16;MQ=95.26;MQ0=0;QD=1.66;SB=-0.98 GT:AD:DP:GQ:PL  0/1:14,4:14:60.91:61,0,255

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from VCF to BED. While BEDOPS supports 0- and 1-based coordinate indexing, the coordinate change made here is believed to be convenient for most end users.

.. _vcf2bed_downloads:

=========
Downloads
=========

* Sample VCF dataset: :download:`foo.vcf <../../../../assets/reference/file-management/conversion/reference_vcf2bed_foo.vcf>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
