.. _sam2bed:

`sam2bed`
=========

The ``sam2bed`` script converts 1-based `Sequence Alignment/Map <http://samtools.sourceforge.net/>`_ (SAM) to sorted, 0-based UCSC BED data.

For convenience, we also offer ``sam2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

The ``sam2bed`` script is "non-lossy". Similar tools in the world tend to throw out information from the original SAM input upon conversion; ``sam2bed`` retains everything, facilitating reuse of converted data and conversion to other formats.

.. tip:: Doing the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive can save a lot of space relative to the original SAM format, up to 33% of the original SAM dataset, while offering per-chromosome random access.

============
Dependencies
============

This ``bash`` shell script is dependent upon the installation of `SAMtools <http://samtools.sourceforge.net/>`_ and `GNU awk <http://www.gnu.org/software/gawk/>`_ tools.

.. note:: GNU ``awk`` is already installed on Linux systems, but is usually an additional installation on Mac OS X systems. Use `MacPorts <http://www.macports.org/>`_, `Homebrew <http://mxcl.github.com/homebrew/>`_, or compile from `source <http://www.gnu.org/software/gawk/>`_).

======
Source
======

The ``sam2bed`` and ``sam2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``sam2bed`` script parses SAM data from standard input and prints :ref:`sorted <sort-bed>` BED to standard output. The ``sam2starch`` script uses an extra step to parse SAM to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If you are sorting data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample binary input called ``foo.sam`` (see the :ref:`Downloads <sam2bed_downloads>` section to grab this file). 

::

  @HD     VN:1.0 SO:coordinate
  @SQ     SN:seq1 LN:5000
  @SQ     SN:seq2 LN:5000
  @CO     Example of SAM/BAM file format.
  B7_591:4:96:693:509     73      seq1    1       99      36M     *       0       0       CACTAGTGGCTCATTGTAAATGTGTGGTTTAACTCG    <<<<<<<<<<<<<<<;<<<<<<<<<5<<<<<;:<;7    MF:i:18 Aq:i:73 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  EAS54_65:7:152:368:113  73      seq1    3       99      35M     *       0       0       CTAGTGGCTCATTGTAAATGTGTGGTTTAACTCGT     <<<<<<<<<<0<<<<655<<7<<<:9<<3/:<6):     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  EAS51_64:8:5:734:57     137     seq1    5       99      35M     *       0       0       AGTGGCTCATTGTAAATGTGTGGTTTAACTCGTCC     <<<<<<<<<<<7;71<<;<;;<7;<<3;);3*8/5     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  ...


We can convert it to sorted BED data in the following manner (omitting standard error messages):

::

  $ sam2bed < foo.sam
  seq1    0       36      B7_591:4:96:693:509     73      +       99      36M     *       0       0       CACTAGTGGCTCATTGTAAATGTGTGGTTTAACTCG    <<<<<<<<<<<<<<<;<<<<<<<<<5<<<<<;:<;7    MF:i:18 Aq:i:73 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    2       37      EAS54_65:7:152:368:113  73      +       99      35M     *       0       0       CTAGTGGCTCATTGTAAATGTGTGGTTTAACTCGT     <<<<<<<<<<0<<<<655<<7<<<:9<<3/:<6):     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    4       39      EAS51_64:8:5:734:57     137     +       99      35M     *       0       0       AGTGGCTCATTGTAAATGTGTGGTTTAACTCGTCC     <<<<<<<<<<<7;71<<;<;;<7;<<3;);3*8/5     MF:i:18 Aq:i:66 NM:i:0  UQ:i:0  H0:i:1  H1:i:0
  seq1    5       41      B7_591:1:289:587:906    137     +       63      36M     *       0       0       GTGGCTCATTGTAATTTTTTGTTTTAACTCTTCTCT    (-&----,----)-)-),'--)---',+-,),''*,    MF:i:130        Aq:i:63 NM:i:5  UQ:i:38 H0:i:0  H1:i:0
  ...


.. note:: Note the conversion from 1- to 0-based coordinates. While BEDOPS fully supports 0- and 1-based coordinates, the coordinate change in BED is believed to be convenient to most end users.

.. _sam2bed_downloads:

=========
Downloads
=========

* Sample SAM dataset: :download:`foo.sam <../../../../assets/reference/file-management/conversion/reference_sam2bed_foo.sam>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
