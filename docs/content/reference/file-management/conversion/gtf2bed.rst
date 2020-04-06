.. _gtf2bed:

`gtf2bed`
=========

The ``gtf2bed`` script converts 1-based, closed ``[start, end]`` `Gene Transfer Format v2.2 <http://mblab.wustl.edu/GTF22.html>`_ (GTF2.2) to sorted, 0-based, half-open ``[start-1, end)`` extended BED-formatted data.

For convenience, we also offer ``gtf2starch``, which performs the extra step of creating a Starch-formatted archive.

============
Dependencies
============

The ``gtf2bed`` script requires :ref:`convert2bed <convert2bed>`. The ``gtf2starch`` script requires :ref:`starch <starch>`. Both dependencies are part of a typical BEDOPS installation.

This script is also dependent on input that follows the GTF 2.2 specification. A GTF-format validator is available `here <http://mblab.wustl.edu/software.html>`_ to ensure your input follows specification.

.. tip:: Conversion of data which are GTF-like, but which do not follow the specification can cause parsing issues. If you run into problems, please check that your input follows the GTF specification. 

======
Source
======

The ``gtf2bed`` and ``gtf2starch`` conversion scripts are part of the binary and source downloads of BEDOPS. See the :ref:`Installation <installation>` documentation for more details.

=====
Usage
=====

The ``gtf2bed`` script parses GTF from standard input and prints sorted BED to standard output. The ``gtf2starch`` script uses an extra step to parse GTF to a compressed BEDOPS :ref:`Starch-formatted <starch_specification>` archive, which is also directed to standard output.

.. tip:: By default, all conversion scripts now output sorted BED data ready for use with BEDOPS utilities. If you do not want to sort converted output, use the ``--do-not-sort`` option. Run the script with the ``--help`` option for more details.

.. tip:: If sorting converted data larger than system memory, use the ``--max-mem`` option to limit sort memory usage to a reasonable fraction of available memory, *e.g.*, ``--max-mem 2G`` or similar. See ``--help`` for more details.

=======
Example
=======

To demonstrate these scripts, we use a sample GTF input called ``foo.gtf`` (see the :ref:`Downloads <gtf2bed_downloads>` section to grab this file). 

::

  chr20      protein_coding  exon    9874841 9874841 .       +       .       gene_id "ENSBTAG00000020601"; transcript_id "ENSBTAT0000002.4.39"; gene_name "ZNF366";
  chr20      protein_coding  CDS     9873504 9874841 .       +       0       gene_id "ENSBTAG00000020601"; transcript_id "ENSBTAT0000002.4.39"; gene_name "ZNF366";
  chr20      protein_coding  exon    9877488 9877679 .       +       .       gene_id "ENSBTAG00000020601"; transcript_id "ENSBTAT0000002.4.39";

We can convert it to sorted BED data in the following manner:

::

  $ gtf2bed < foo.gtf
  chr20   9874840 9874841 ZNF366  .       +       protein_coding  exon    .       gene_id "ENSBTAG00000020601"; transcript_id "ENSBTAT0000002.4.39"; gene_name "ZNF366"; zero_length_insertion "True";
  chr20   9873503 9874841 ZNF366  .       +       protein_coding  CDS     0       gene_id "ENSBTAG00000020601"; transcript_id "ENSBTAT0000002.4.39"; gene_name "ZNF366";
  chr20   9877487 9877679 ENSBTAG00000020601      .       +       protein_coding  exon    .       gene_id "ENSBTAG00000020601"; transcript_id "ENSBTAT0000002.4.39";

.. tip:: After, say, performing set or statistical operations with :ref:`bedops`, :ref:`bedmap` etc., converting data back to GTF is accomplished through an ``awk`` statement that re-orders columns and shifts the coordinate index:

   ::

     $ awk '{print $1"\t"$7"\t"$8"\t"($2+1)"\t"$3"\t"$5"\t"$6"\t"$9"\t"(substr($0, index($0,$10)))}' foo_subset.bed > foo_subset.gtf

.. note:: Zero-length insertion elements are given an extra attribute called ``zero_length_insertion`` which lets a BED-to-GTF or other parser know that the element will require conversion back to a right-closed element ``[a, b]``, where ``a`` and ``b`` are equal.

.. note:: Note the conversion from 1- to 0-based coordinate indexing, in the transition from GTF to BED. *BEDOPS supports operations on input with any coordinate indexing*, but the coordinate change made here is believed to be convenient for most end users.

.. _gtf2bed_column_mapping:

==============
Column mapping
==============

In this section, we describe how GTF2.2 columns are mapped to BED columns. We start with the first six UCSC BED columns as follows:

+---------------------------+---------------------+---------------+
| GFF2.2 field              | BED column index    | BED field     |
+===========================+=====================+===============+
| seqname                   | 1                   | chromosome    |
+---------------------------+---------------------+---------------+
| start                     | 2                   | start         |
+---------------------------+---------------------+---------------+
| end                       | 3                   | stop          |
+---------------------------+---------------------+---------------+
| gene_id                   | 4                   | id            |
+---------------------------+---------------------+---------------+
| score                     | 5                   | score         |
+---------------------------+---------------------+---------------+
| strand                    | 6                   | strand        |
+---------------------------+---------------------+---------------+

The remaining columns are mapped as follows:

+---------------------------+---------------------+---------------+
| GFF2.2 field              | BED column index    | BED field     |
+===========================+=====================+===============+
| source                    | 7                   |               |
+---------------------------+---------------------+---------------+
| feature                   | 8                   |               |
+---------------------------+---------------------+---------------+
| frame                     | 9                   |               |
+---------------------------+---------------------+---------------+
| attributes                | 10                  |               |
+---------------------------+---------------------+---------------+

If present in the GTF2.2 input, the following column is also mapped:

+---------------------------+---------------------+---------------+
| GFF2.2 field              | BED column index    | BED field     |
+===========================+=====================+===============+
| comments                  | 11                  |               |
+---------------------------+---------------------+---------------+

If we encounter zero-length insertion elements (which are defined where the ``start`` and ``stop`` GFF3 field values are equivalent), the ``start`` coordinate is decremented to convert to 0-based, half-open indexing, and a ``zero_length_insertion`` attribute is added to the ``attributes`` GTF2.2 field value.

.. _gtf2bed_downloads:

=========
Downloads
=========

* Sample GTF dataset: :download:`foo.gtf <../../../../assets/reference/file-management/conversion/reference_gtf2bed_foo.gtf>`

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
