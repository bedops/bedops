BEDOPS: the fastest scalable and easily-parallelizable genome analysis toolkit
==============================================================================
BEDOPS is an open-source command-line toolkit that performs efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale. Tasks can be easily split by chromosome for distributing BEDOPS operations across a computational cluster.

+-----------------------+-----------------------+------------------------+
| |set_operations_png|  | |statistics_png|      | |file_management_png|  |
+-----------------------+-----------------------+------------------------+
| |set_operations|      | |statistics|          | |file_management|      |
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

.. |set_operations_png| image:: assets/index/set_operations.png
.. |statistics_png| image:: assets/index/statistics.png
.. |file_management_png| image:: assets/index/file_management.png

.. |set_operations| raw:: html

   <ul style="list-style-type: square; font-size:smaller; margin: 0; padding: 1;">
   <li>bedops - apply set operations on any number of BED inputs</li>
   <li>bedextract - efficiently extract BED features</li>
   <li>closest-features - matches nearest features between BED files</li>
   </ul>

.. |statistics| raw:: html

   <ul style="list-style-type: square; font-size:smaller; margin: 0; padding: 1;">
   <li>bedmap - map overlapping BED elements onto target regions, and optionally compute any number of common statistical operations</li>
   </ul>

.. |file_management| raw:: html

   <ul style="list-style-type: square; font-size:smaller; margin: 0; padding: 1;">
   <li>sort-bed - apply lexicographical sort to BED data</li>
   <li>starch and unstarch - compress and extract BED data</li>
   <li>starchcat - merge compressed archives</li>
   <li>conversion tools - convert common genomic formats to BED</li>
   </ul>
