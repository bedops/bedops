BEDOPS: the fastest, highly scalable and easily-parallelizable genome analysis toolkit
======================================================================================
BEDOPS is an open-source command-line toolkit that performs highly efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale. Tasks can be easily split by chromosome for distributing whole-genome analyses across a computational cluster.

+-----------------------+-----------------------+------------------------+
| |set_operations_png|  | |statistics_png|      | |file_management_png|  |
+-----------------------+-----------------------+------------------------+
| |set_operations|      | |statistics|          | |file_management|      |
+-----------------------+-----------------------+------------------------+
| |performance_png|     |                       |                        |
+-----------------------+-----------------------+------------------------+
| |performance|         |                       |                        |
+-----------------------+-----------------------+------------------------+

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
   <li><a href="content/reference/set-operations/bedops.html"><tt>bedops</tt></a> - apply set operations on any number of BED inputs</li>
   <li><a href="content/reference/set-operations/bedextract.html"><tt>bedextract</tt></a> - efficiently extract BED features</li>
   <li><a href="content/reference/set-operations/closest-features.html"><tt>closest-features</tt></a> - matches nearest features between BED files</li>
   </ul>

.. |statistics_png| image:: assets/index/statistics.png
.. |statistics| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/reference/statistics/bedmap.html"><tt>bedmap</tt></a> - map overlapping BED elements onto target regions, and optionally compute any number of common statistical operations</li>
   </ul>

.. |file_management_png| image:: assets/index/file_management.png
.. |file_management| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/reference/file-management/sorting/sort-bed.html"><tt>sort-bed</tt></a> - apply lexicographical sort to BED data</li>
   <li><a href="content/reference/file-management/compression/starch.html"><tt>starch</tt></a> and <a href="content/reference/file-management/compression/unstarch.html"><tt>unstarch</tt></a> - compress and extract BED data</li>
   <li><a href="content/reference/file-management/compression/starchcat.html"><tt>starchcat</tt></a> - merge compressed archives</li>
   <li><a href="content/reference/file-management/conversion.html">conversion tools</a> - convert common genomic formats to BED</li>
   </ul>

.. |performance_png| image:: assets/index/performance.png
.. |performance| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/performance.html#set-operations-with-bedops">Set operations with <tt>bedops</tt></li>
   <li><a href="content/performance.html#compression-characteristics-of-starch">Compression characteristics of <tt>starch</tt></li>
   <li><a href="content/performance.html#independent-testing">Independent testing</a></li>
   </ul>
