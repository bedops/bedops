.. _vcf2bed:

`vcf2bed`
=========

The ``vcf2bed`` script converts 1-based, closed ``[start, end]`` `Variant Call Format v4.2 <http://vcftools.sourceforge.net/specs.html>`_ (VCF) to sorted, 0-based, half-open ``[start-1, start)`` extended BED data.

.. note:: Note that this script converts from ``[start, end]`` to ``[start-1, start)``. Unless the ``--snvs``, ``--insertions`` or ``--deletions`` options are added, we perform the equivalent of a *single-base insertion* to make BED output that is guaranteed to work with BEDOPS, **regardless of what the actual variant may be**, to allow operations to be performed. The converted output contains additional columns which allow reconstruction of the original VCF data and associated variant parameters.

For convenience, we also offer ``vcf2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

============
Dependencies
============

The ``vcf2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``vcf2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

This script is also dependent on input that follows the VCF v4.2 specification.

.. tip:: Conversion of data which are VCF-like, but which do not follow the specification can cause parsing issues. If you run into problems, please check that your input follows the VCF specification using validation tools, such as those packaged with `VCFTools <http://vcftools.sourceforge.net/perl_module.html#vcf-validator>`_.

======
Source
======

The ``vcf2bed`` and ``vcf2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``vcf2bed`` script parses VCF from standard input and prints sorted BED to standard output. The ``vcf2starch`` script uses an extra step to parse VCF to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a VCF file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

.. note:: By default, multiple BED annotations are printed if there are multiple alternate alleles in a variant call. Use the ``--do-not-split-alt-alleles`` option to preserve the alternate allele string and print only one BED element for the variant call.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If the converted BED output looks truncated or incomplete, and you are converting a VCF file that is larger than the capacity of your ``/tmp`` folder, then use the ``--sort-tmpdir`` option to specify an alternative directory to store intermediate sort results. Or, if you do not want to sort the converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If you are sorting data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

.. _vcf2bed_custom_variants:

===========================
Customized variant handling
===========================

By default, the ``vcf2bed`` script translates all variants to single-base positions in the resulting BED output. Depending on the category of variant you are interested in, however, you may want more specific categories handled differently. 

Based on the VCF v4.2 specification, we also provide three custom options for filtering input for each of the three types of variants listed: ``--snvs``, ``--insertions`` and ``--deletions``. In each case, we use the length of the reference and alternate alleles to determine which type of variant is being handled. 

In addition, using any of these three custom options automatically results in processing of mixed variant records for a microsatellite, where present. For instance, the following record contains a mixture of a deletion and insertion variant (``GTC -> G`` and ``GTC -> GTCT``, respectively):

::

  #CHROM POS     ID        REF    ALT     QUAL FILTER INFO                              FORMAT      NA00001        NA00002        NA00003
  20     12.4.397 microsat1 GTC    G,GTCT  50   PASS   NS=3;DP=9;AA=G                    GT:GQ:DP    0/1:35:4       0/2:17:2       1/1:40:3

When using ``--snvs``, ``--insertions`` or ``--deletions``, this record is split into two distinct BED records and filtered depending on which custom option was chosen. The ``--insertions`` option would only export the single-base position of the insertion in this mixed variant, while ``--deletions`` would show the deletion.

In this way, you can control what kinds of variants are translated into BED outputs |---| most importantly, there is also no confusion about what the length of the BED element signifies.

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

As you see here, the header data of the VCF file is discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field). 

Here we use ``--keep-header`` with our example dataset:

::

  $ vcf2bed --keep-header < foo.vcf
  _header 0       1       ##fileformat=VCFv4.0
  _header 1       2       ##FILTER=<ID=LowQual,Description="QUAL < 50.0">
  _header 2       3       ##FORMAT=<ID=AD,Number=.,Type=Integer,Description="Allelic depths for the ref and alt alleles in the order listed">
  _header 3       4       ##FORMAT=<ID=DP,Number=1,Type=Integer,Description="Read Depth (only filtered reads used for calling)">
  _header 4       5       ##FORMAT=<ID=GQ,Number=1,Type=Float,Description="Genotype Quality">
  _header 5       6       ##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
  _header 6       7       ##FORMAT=<ID=PL,Number=3,Type=Float,Description="Normalized, Phred-scaled likelihoods for AA,AB,BB genotypes where A=ref and B=alt; not applicable if site is not biallelic">
  _header 7       8       ##INFO=<ID=AC,Number=.,Type=Integer,Description="Allele count in genotypes, for each ALT allele, in the same order as listed">
  _header 8       9       ##INFO=<ID=AF,Number=.,Type=Float,Description="Allele Frequency, for each ALT allele, in the same order as listed">
  _header 9       10      ##INFO=<ID=AN,Number=1,Type=Integer,Description="Total number of alleles in called genotypes">
  _header 10      11      ##INFO=<ID=DB,Number=0,Type=Flag,Description="dbSNP Membership">
  _header 11      12      ##INFO=<ID=DP,Number=1,Type=Integer,Description="Total Depth">
  _header 12      13      ##INFO=<ID=DS,Number=0,Type=Flag,Description="Were any of the samples downsampled?">
  _header 13      14      ##INFO=<ID=Dels,Number=1,Type=Float,Description="Fraction of Reads Containing Spanning Deletions">
  _header 14      15      ##INFO=<ID=HRun,Number=1,Type=Integer,Description="Largest Contiguous Homopolymer Run of Variant Allele In Either Direction">
  _header 15      16      ##INFO=<ID=HaplotypeScore,Number=1,Type=Float,Description="Consistency of the site with two (and only two) segregating haplotypes">
  _header 16      17      ##INFO=<ID=MQ,Number=1,Type=Float,Description="RMS Mapping Quality">
  _header 17      18      ##INFO=<ID=MQ0,Number=1,Type=Integer,Description="Total Mapping Quality Zero Reads">
  _header 18      19      ##INFO=<ID=QD,Number=1,Type=Float,Description="Variant Confidence/Quality by Depth">
  _header 19      20      ##INFO=<ID=SB,Number=1,Type=Float,Description="Strand Bias">
  _header 20      21      ##INFO=<ID=VQSLOD,Number=1,Type=Float,Description="log10-scaled probability of variant being true under the trained gaussian mixture model">
  _header 21      22      ##UnifiedGenotyperV2="analysis_type=UnifiedGenotyperV2 input_file=[TEXT CLIPPED FOR CLARITY]"
  _header 22      23      #CHROM  POS     ID      REF     ALT     QUAL    FILTER  INFO    FORMAT  NA12878
  chr1    873761  873762  .       5231.78 T       G       PASS    AC=1;AF=0.50;AN=2;DP=315;Dels=0.00;HRun=2;HaplotypeScore=15.11;MQ=91.05;MQ0=15;QD=16.61;SB=-1533.02;VQSLOD=-1.5473      GT:AD:DP:GQ:PL  0/1:173,141:282:99:255,0,255
  chr1    877663  877664  rs3828047       3931.66 A       G       PASS    AC=2;AF=1.00;AN=2;DB;DP=105;Dels=0.00;HRun=1;HaplotypeScore=1.59;MQ=92.52;MQ0=4;QD=37.44;SB=-1152.13;VQSLOD=0.1185      GT:AD:DP:GQ:PL  1/1:0,105:94:99:255,255,0
  chr1    899281  899282  rs28548431      71.77   C       T       PASS    AC=1;AF=0.50;AN=2;DB;DP=4;Dels=0.00;HRun=0;HaplotypeScore=0.00;MQ=99.00;MQ0=0;QD=17.94;SB=-46.55;VQSLOD=-1.9148 GT:AD:DP:GQ:PL  0/1:1,3:4:25.92:103,0,26
  chr1    974164  974165  rs9442391       29.84   T       C       LowQual AC=1;AF=0.50;AN=2;DB;DP=18;Dels=0.00;HRun=1;HaplotypeScore=0.16;MQ=95.26;MQ0=0;QD=1.66;SB=-0.98 GT:AD:DP:GQ:PL  0/1:14,4:14:60.91:61,0,255

With this option, the ``vcf2*`` scripts are completely "non-lossy". Use of ``awk`` or other scripting tools can munge these data back into a VCF-formatted file.

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from VCF to BED. While BEDOPS supports 0- and 1-based coordinate indexing, the coordinate change made here is believed to be convenient for most end users.

.. _vcf2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how VCF v4.2 columns are mapped to BED columns. We start with the first five UCSC BED columns as follows:

+---------------------------+---------------------+---------------+
| VCF v4.2 field            | BED column index    | BED field     |
+===========================+=====================+===============+
| #CHROM                    | 1                   | chromosome    |
+---------------------------+---------------------+---------------+
| POS - 1                   | 2                   | start         |
+---------------------------+---------------------+---------------+
| POS (*)                   | 3                   | stop          |
+---------------------------+---------------------+---------------+
| ID                        | 4                   | id            |
+---------------------------+---------------------+---------------+
| QUAL                      | 5                   | score         |
+---------------------------+---------------------+---------------+

The remaining columns are mapped as follows:

+---------------------------+---------------------+---------------+
| VCF v4.2 field            | BED column index    | BED field     |
+===========================+=====================+===============+
| REF                       | 6                   |               |
+---------------------------+---------------------+---------------+
| ALT                       | 7                   |               |
+---------------------------+---------------------+---------------+
| FILTER                    | 8                   |               |
+---------------------------+---------------------+---------------+
| INFO                      | 9                   |               |
+---------------------------+---------------------+---------------+

If present in the VCF v4.2 input, the following columns are also mapped:

+---------------------------+---------------------+---------------+
| VCF v4.2 field            | BED column index    | BED field     |
+===========================+=====================+===============+
| FORMAT                    | 10                  |               |
+---------------------------+---------------------+---------------+
| Sample ID 1               | 11                  |               |
+---------------------------+---------------------+---------------+
| Sample ID 2               | 12                  |               |
+---------------------------+---------------------+---------------+
| ...                       | 13, 14, etc.        |               |
+---------------------------+---------------------+---------------+

When using ``--deletions``, the stop value of the BED output is determined by the length difference between ALT and REF alleles. Use of ``--insertions`` or ``--snvs`` yields a one-base BED element.

If the ALT field contains more than one allele, multiple BED records will be printed. Use the ``--do-not-split`` option if you only want one BED record per variant call.

The "meta-information" (starting with ``##``) and "header" lines (starting with ``#``) are discarded, unless the ``--keep-headers`` options is specified.

.. _vcf2bed_downloads:

=========
Downloads
=========

* Sample VCF dataset: :download:`foo.vcf <../../../../assets/reference/file-management/conversion/reference_vcf2bed_foo.vcf>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
