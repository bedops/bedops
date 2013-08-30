BEDOPS: the fastest scalable and easily-parallelizable genome analysis toolkit
==============================================================================
BEDOPS is an open-source command-line toolkit that performs efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale. Tasks can be easily split by chromosome for distributing BEDOPS operations across a computational cluster.

+-----------------------+-----------------------+------------------------+
| |set_operations.png|  | |statistics.png|      | |file_management.png|  |
+-----------------------+-----------------------+------------------------+
| * bedops - apply set  |                       |                        |
|   operations on any   |                       |                        |
|   number of BED       |                       |                        |
|   inputs              |                       |                        |
| * bedextract -        |                       |                        |
|   efficiently extract |                       |                        |
|   BED features        |                       |                        |
| * closest-features -  |                       |                        |
|   matches nearest     |                       |                        |
|   features between    |                       |                        |
|   BED files           |                       |                        |
+-----------------------+-----------------------+------------------------+

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

.. |set_operations.png| image:: assets/index/set_operations.png
.. |statistics.png| image:: assets/index/statistics.png
.. |file_management.png| image:: assets/index/file_management.png

.. |set_operations| replace:: 

.. |statistics| replace::
   * bedmap - foo bar baz
.. |file_management| replace::
   * starch, unstarch, starchcat - compression!
