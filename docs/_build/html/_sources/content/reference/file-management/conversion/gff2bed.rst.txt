.. _gff2bed:

`gff2bed`
=========

The ``gff2bed`` script converts 1-based, closed ``[start, end]`` `General Feature Format v3 <http://www.sequenceontology.org/gff3.shtml>`_ (GFF3) to sorted, 0-based, half-open ``[start-1, end)`` extended BED-formatted data.

For convenience, we also offer ``gff2starch``, which performs the extra step of creating a :ref:`Starch-formatted <starch_specification>` archive.

============
Dependencies
============

The ``gff2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``gff2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

This script is also dependent on input that follows the GFF3 specification. A GFF3-format validator is available `here <http://modencode.oicr.on.ca/cgi-bin/validate_gff3_online>`_ to ensure your input follows specification.

.. tip:: Conversion of data which are GFF-like, but which do not follow the specification can cause parsing issues. If you run into problems, please check that your input follows the GFF3 specification. Tools such as the `GFF3 Online Validator <http://genometools.org/cgi-bin/gff3validator.cgi>`_ are useful for this task.

======
Source
======

The ``gff2bed`` and ``gff2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``gff2bed`` script parses GFF3 from standard input and prints sorted BED to standard output. The ``gff2starch`` script uses an extra step to parse GFF to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

The header data of a GFF file is usually discarded, unless you add the ``--keep-header`` option. In this case, BED elements are created from these data, using the chromosome name ``_header`` to denote content. Line numbers are specified in the start and stop coordinates, and unmodified header data are placed in the fourth column (ID field).

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If sorting converted data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample GFF input called ``foo.gff`` (see the :ref:`Downloads <gff2bed_downloads>` section to grab this file). 

::

  ##gff-version 3
  chr1    Canada  exon    1300    1300    .       +       .       ID=exon00001;score=1
  chr1    USA     exon    1050    1500    .       -       0       ID=exon00002;Ontology_term="GO:0046703";Ontology_term="GO:0046704"
  chr1    Canada  exon    3000    3902    .       ?       2       ID=exon00003;score=4;Name=foo
  chr1    .       exon    5000    5500    .       .       .       ID=exon00004;Gap=M8 D3 M6 I1 M6
  chr1    .       exon    7000    9000    10      +       1       ID=exon00005;Dbxref="NCBI_gi:10727410"

We can convert it to sorted BED data in the following manner:

::

  $ gff2bed < foo.gff3 
  chr1    1049    1500    exon00002       .       -       USA     exon    0       ID=exon00002;Ontology_term="GO:0046703";Ontology_term="GO:0046704"
  chr1    1299    1300    exon00001       .       +       Canada  exon    .       ID=exon00001;score=1;zeroLengthInsertion=True
  chr1    2999    3902    exon00003       .       ?       Canada  exon    2       ID=exon00003;score=4;Name=foo
  chr1    4999    5500    exon00004       .       .       .       exon    .       ID=exon00004;Gap=M8 D3 M6 I1 M6
  chr1    6999    9000    exon00005       10      +       .       exon    1       ID=exon00005;Dbxref="NCBI_gi:10727410"

The default usage strips the leading pragma, or header (``##gff-version 3``), but adding the ``--keep-header`` option will preserve this as a BED element that uses ``_header`` as a chromosome name:

::

  $ gff2bed --keep-header < foo.gff3
  _header 0       1       ##gff-version 3
  chr1    1049    1500    exon00002       .       -       USA     exon    0       ID=exon00002;Ontology_term="GO:0046703";Ontology_term="GO:0046704"
  chr1    1299    1300    exon00001       .       +       Canada  exon    .       ID=exon00001;score=1;zero_length_insertion=True
  chr1    2999    3902    exon00003       .       ?       Canada  exon    2       ID=exon00003;score=4;Name=foo
  chr1    4999    5500    exon00004       .       .       .       exon    .       ID=exon00004;Gap=M8 D3 M6 I1 M6
  chr1    6999    9000    exon00005       10      +       .       exon    1       ID=exon00005;Dbxref="NCBI_gi:10727410"

.. note:: Zero-length insertion elements are given an extra attribute called ``zeroLengthInsertion`` which lets a BED-to-GFF or other parser know that the element will require conversion back to a right-closed element ``[a, b]``, where ``a`` and ``b`` are equal.

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from GFF3 to BED. *BEDOPS supports operations on input with any coordinate indexing*, but the coordinate change made here is believed to be convenient for most end users.

.. _gff2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how GFF3 columns are mapped to BED columns. We start with the first six UCSC BED columns as follows:

+---------------------------+---------------------+---------------+
| GFF3 field                | BED column index    | BED field     |
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
| GFF3 field                | BED column index    | BED field     |
+===========================+=====================+===============+
| source                    | 7                   |               |
+---------------------------+---------------------+---------------+
| type                      | 8                   |               |
+---------------------------+---------------------+---------------+
| phase                     | 9                   |               |
+---------------------------+---------------------+---------------+
| attributes                | 10                  |               |
+---------------------------+---------------------+---------------+

If we encounter zero-length insertion elements (which are defined where the ``start`` and ``stop`` GFF3 field values are equivalent), the ``start`` coordinate is decremented to convert to 0-based, half-open indexing, and a ``zero_length_insertion`` attribute is added to the ``attributes`` GFF3 field value.

.. _gff2bed_downloads:

=========
Downloads
=========

* Sample GFF dataset: :download:`foo.gff <../../../../assets/reference/file-management/conversion/reference_gff2bed_foo.gff>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
