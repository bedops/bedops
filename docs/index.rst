BEDOPS: the fastest, highly scalable and easily-parallelizable genome analysis toolkit
======================================================================================
BEDOPS is an open-source command-line toolkit that performs highly efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale. Tasks can be easily split by chromosome for distributing BEDOPS operations across a computational cluster.

+-----------------------+-----------------------+------------------------+
| |set_operations_png|  | |statistics_png|      | |file_management_png|  |
+-----------------------+-----------------------+------------------------+
| |set_operations|      | |statistics|          | |file_management|      |
+-----------------------+-----------------------+------------------------+

========
Contents
========
.. toctree::
   :numbered:

   content/overview
   content/installation
   content/usage-examples
   content/performance
   content/reference
   content/faq

.. |set_operations_png| image:: assets/index/set_operations.png
.. |set_operations| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><tt>bedops</tt> - apply set operations on any number of BED inputs</li>
   <li><tt>bedextract</tt> - efficiently extract BED features</li>
   <li><tt>closest-features</tt> - matches nearest features between BED files</li>
   </ul>

.. |statistics_png| image:: assets/index/statistics.png
.. |statistics| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><tt>bedmap</tt> - map overlapping BED elements onto target regions, and optionally compute any number of common statistical operations</li>
   </ul>

.. |file_management_png| image:: assets/index/file_management.png
.. |file_management| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><tt>sort-bed</tt> - apply lexicographical sort to BED data</li>
   <li><tt>starch</tt> and <tt>unstarch</tt> - compress and extract BED data</li>
   <li><tt>starchcat</tt> - merge compressed archives</li>
   <li>conversion tools - convert common genomic formats to BED</li>
   </ul>
