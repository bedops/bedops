.. _gvf2bed:

`gvf2bed`
=========

The ``gvf2bed`` script converts 1-based, closed ``[start, end]`` `Genome Variation Format <http://www.sequenceontology.org/resources/gvf.html#summary>`_ (GVF, a type of `General Feature Format v3 <http://www.sequenceontology.org/gff3.shtml>`_ or GFF3) to sorted, 0-based, half-open ``[start-1, end)`` extended BED-formatted data.

For convenience, we also offer ``gvf2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

============
Dependencies
============

The ``gvf2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``gvf2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

This script is also dependent on input that follows the GFF3 specification. A GFF3-format validator is available `here <http://modencode.oicr.on.ca/cgi-bin/validate_gff3_online>`_ to ensure your input follows specification.

.. tip:: Conversion of data which are GFF-like, but which do not follow the specification can cause parsing issues. If you run into problems, please check that your input follows the GFF3 specification. Tools such as the `GFF3 Online Validator <http://genometools.org/cgi-bin/gff3validator.cgi>`_ are useful for this task.

======
Source
======

The ``gvf2bed`` and ``gvf2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``gvf2bed`` script parses GVF from standard input and prints sorted BED to standard output. The ``gvf2starch`` script uses an extra step to parse GVF to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a GVF file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If sorting converted data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample GVF input called ``foo.gvf`` (see the :ref:`Downloads <gvf2bed_downloads>` section to grab this file). 

::

  ##gvf-version 1.07
  ##feature-ontology http://www.sequenceontology.org/resources/obo_files/current_release.obo
  ##multi-individual NA19240,NA18507,NA12878,NA19238
  ##genome-build NCBI B36.3
  ##sequence-region chr16 1 88827254
  
  chr16	dbSNP	SNV	49291360	49291360	.	+	.	ID=ID_2;Variant_seq=C,G;Individual=0,1,2,3;Genotype=0:1,0:0,1:1,0:1;
  chr16	dbSNP	SNV	49302125	49302125	.	+	.	ID=ID_3;Variant_seq=C,T;Individual=0,1,3;Genotype=0:1,2:2,0:2;
  chr16	dbSNP	SNV	49302365	49302365	.	+	.	ID=ID_4;Variant_seq=G;Individual=0,1;Genotype=0:0,0:0;
  chr16	dbSNP	SNV	49302700	49302700	.	+	.	ID=ID_5;Variant_seq=C,T;Individual=2,3;Genotype=0:1,0:0;
  chr16	dbSNP	SNV	49303084	49303084	.	+	.	ID=ID_6;Variant_seq=T,G,A;Individual=3;Genotype=1,2:;
  chr16	dbSNP	SNV	49303427	49303427	.	+	.	ID=ID_8;Variant_seq=T;Individual=0;Genotype=0:0;
  chr16	dbSNP	SNV	49303596	49303596	.	+	.	ID=ID_9;Variant_seq=A,G,T;Individual=0,1,3;Genotype=1:2,3:3,1:3;

We can convert it to sorted BED data in the following manner:

::

  $ gvf2bed < foo.gvf 
  chr16	49291359	49291360	ID_2	.	+	dbSNP	SNV	.	ID=ID_2;Variant_seq=C,G;Individual=0,1,2,3;Genotype=0:1,0:0,1:1,0:1;zero_length_insertion=True
  chr16	49302124	49302125	ID_3	.	+	dbSNP	SNV	.	ID=ID_3;Variant_seq=C,T;Individual=0,1,3;Genotype=0:1,2:2,0:2;zero_length_insertion=True
  chr16	49302364	49302365	ID_4	.	+	dbSNP	SNV	.	ID=ID_4;Variant_seq=G;Individual=0,1;Genotype=0:0,0:0;zero_length_insertion=True
  chr16	49302699	49302700	ID_5	.	+	dbSNP	SNV	.	ID=ID_5;Variant_seq=C,T;Individual=2,3;Genotype=0:1,0:0;zero_length_insertion=True
  chr16	49303083	49303084	ID_6	.	+	dbSNP	SNV	.	ID=ID_6;Variant_seq=T,G,A;Individual=3;Genotype=1,2:;zero_length_insertion=True
  chr16	49303426	49303427	ID_8	.	+	dbSNP	SNV	.	ID=ID_8;Variant_seq=T;Individual=0;Genotype=0:0;zero_length_insertion=True
  chr16	49303595	49303596	ID_9	.	+	dbSNP	SNV	.	ID=ID_9;Variant_seq=A,G,T;Individual=0,1,3;Genotype=1:2,3:3,1:3;zero_length_insertion=True

As shown, the default usage strips the leading pragmas (``##gvf-version 1.07``, *etc.*), but adding the ``--keep-header`` option will preserve pragmas as BED elements that use ``_header`` as a chromosome name:

::

  $ gvf2bed --keep-header < foo.gvf
  _header	0	1	##gvf-version 1.07
  _header	1	2	##feature-ontology http://www.sequenceontology.org/resources/obo_files/current_release.obo
  _header	2	3	##multi-individual NA19240,NA18507,NA12878,NA19238
  _header	3	4	##genome-build NCBI B36.3
  _header	4	5	##sequence-region chr16 1 88827254
  chr16	49291359	49291360	ID_2	.	+	dbSNP	SNV	.	ID=ID_2;Variant_seq=C,G;Individual=0,1,2,3;Genotype=0:1,0:0,1:1,0:1;zero_length_insertion=True
  chr16	49302124	49302125	ID_3	.	+	dbSNP	SNV	.	ID=ID_3;Variant_seq=C,T;Individual=0,1,3;Genotype=0:1,2:2,0:2;zero_length_insertion=True
  chr16	49302364	49302365	ID_4	.	+	dbSNP	SNV	.	ID=ID_4;Variant_seq=G;Individual=0,1;Genotype=0:0,0:0;zero_length_insertion=True
  chr16	49302699	49302700	ID_5	.	+	dbSNP	SNV	.	ID=ID_5;Variant_seq=C,T;Individual=2,3;Genotype=0:1,0:0;zero_length_insertion=True
  chr16	49303083	49303084	ID_6	.	+	dbSNP	SNV	.	ID=ID_6;Variant_seq=T,G,A;Individual=3;Genotype=1,2:;zero_length_insertion=True
  chr16	49303426	49303427	ID_8	.	+	dbSNP	SNV	.	ID=ID_8;Variant_seq=T;Individual=0;Genotype=0:0;zero_length_insertion=True
  chr16	49303595	49303596	ID_9	.	+	dbSNP	SNV	.	ID=ID_9;Variant_seq=A,G,T;Individual=0,1,3;Genotype=1:2,3:3,1:3;zero_length_insertion=True

.. note:: Zero-length insertion elements are given an extra attribute called ``zero_length_insertion`` which lets a BED-to-GVF or other parser know that the element will require conversion back to a right-closed element ``[a, b]``, where ``a`` and ``b`` are equal.

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from GVF to BED. *BEDOPS supports operations on input with any coordinate indexing*, but the coordinate change made here is believed to be convenient for most end users.

.. _gvf2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how GVF columns are mapped to BED columns. We start with the first six UCSC BED columns as follows:

+---------------------------+---------------------+---------------+
| GVF field                 | BED column index    | BED field     |
+===========================+=====================+===============+
| seqid                     | 1                   | chromosome    |
+---------------------------+---------------------+---------------+
| start                     | 2                   | start         |
+---------------------------+---------------------+---------------+
| end                       | 3                   | stop          |
+---------------------------+---------------------+---------------+
| ID (via attributes)       | 4                   | id            |
+---------------------------+---------------------+---------------+
| score                     | 5                   | score         |
+---------------------------+---------------------+---------------+
| strand                    | 6                   | strand        |
+---------------------------+---------------------+---------------+

The remaining columns are mapped as follows:

+---------------------------+---------------------+---------------+
| GVF field                 | BED column index    | BED field     |
+===========================+=====================+===============+
| source                    | 7                   |               |
+---------------------------+---------------------+---------------+
| type                      | 8                   |               |
+---------------------------+---------------------+---------------+
| phase                     | 9                   |               |
+---------------------------+---------------------+---------------+
| attributes                | 10                  |               |
+---------------------------+---------------------+---------------+

When we encounter zero-length insertion elements (which are defined where the ``start`` and ``stop`` GVF field values are equivalent), the ``start`` coordinate is decremented to convert to 0-based, half-open indexing, and a ``zero_length_insertion`` attribute is added to the ``attributes`` field value.

.. _gvf2bed_downloads:

=========
Downloads
=========

* Sample GVF dataset: :download:`foo.gvf <../../../../assets/reference/file-management/conversion/reference_gvf2bed_foo.gvf>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
