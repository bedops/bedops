.. _smoothing_raw_tags:

Smoothing raw tag count data across the genome
==============================================

In this example, we generate smoothed density signal by binning the genome into 20 bp intervals and counting the number of non-paired-end tag reads falling within 75 bp of each interval. A simple follow-on script marks up results to wig or bigWig format for loading into a track of a local UCSC Genome Browser.

===================
BEDOPS tools in use
===================

For this script, we use :ref:`bam2bed` to convert a BAM file to BED, then we use :ref:`bedmap` to run a sliding density window over input genomic regions. Finally :ref:`starch` compresses the results.

======
Script
======

::

  #!/bin/tcsh -ef
  # author : Richard Sandstrom

  if ( $#argv != 5 ) then
    printf "Wrong number of arguments\n"
    printf "<bam-file> <out-file> <window-size> <step-size> <chromosome-file>\n"
    printf "  where <chromosome-file> contains whole chromosome BED items for the\n"
    printf "  genome, e.g., sort-bed formatted output from the UCSC hg19.chromInfo table.\n"
    exit -1
  endif

  # BAM file
  set inBam = $argv[1]
  # resulting density file
  set outDensity = $argv[2]
  # +/- window for counting read 5' ends
  set window = $argv[3]
  # step size across genome
  set binI = $argv[4]
  # chromosome file for organism of interest
  set chromsfile = $argv[5]

  set outDir = $outDensity:h
  mkdir -p $outDir

  set tmpDir = /tmp/`whoami`/scratch/$$
  if ( -d $tmpDir ) then
    rm -rf $tmdDir
  endif
  mkdir -p $tmpDir

  # clip tags to single 5' end base
  bam2bed < $inBam \
      | awk '{if($6=="+"){s=$2; e=$2+1}else{s=$3-1; e=$3}print $1"\t"s"\t"e}' \
      | sort-bed --max-mem 2G - \
     >! $tmpDir/tags.bed

  # create genome-wide bins and count how many tags fall within range of each
  awk -v binI=$binI -v win=$window \
        '{ \
           i = 0; \
           for(i = $2; i <= $3-binI; i += binI) { print $1"\t"i"\t"i + binI } \
           # end of chrome may include a bin of size < binI \
           if ( i < $3 ) { print $1"\t"i"\t"$3; } \
        }' $chromsfile \
      | bedmap --faster --range $window --echo --count --delim "\t" - $tmpDir/tags.bed \
      | starch - \
     >! $outDensity

  rm -rf $tmpDir

  exit 0

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
