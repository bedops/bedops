===================================================================================
BEDOPS: the fast, highly scalable and easily-parallelizable genome analysis toolkit
===================================================================================

**BEDOPS** is an open-source command-line toolkit that performs highly efficient and scalable Boolean and other set operations, statistical calculations, archiving, conversion and other management of genomic data of arbitrary scale. Tasks can be easily split by chromosome for distributing whole-genome analyses across a computational cluster. 

You can read more about **BEDOPS** and how it can be useful for your research in the :ref:`Overview <overview>` documentation, as well as in the `original manuscript <http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract>`_.

+-----------------------+-----------------------+------------------------+
| |downloads_html|                              |                        |
+-----------------------+-----------------------+------------------------+
| |linux_png|           | |macosx_png|          | |source_png|           |
+-----------------------+-----------------------+------------------------+
| |linux_downloads|     | |macosx_downloads|    | |source_downloads|     |
+-----------------------+-----------------------+------------------------+
| |reference_png|                               |                        |
+-----------------------+-----------------------+------------------------+
| |set_operations_png|  | |statistics_png|      | |file_management_png|  |
+-----------------------+-----------------------+------------------------+
| |set_operations|      | |statistics|          | |file_management|      |
+-----------------------+-----------------------+------------------------+
| |performance_png|     | |support_png|         | |other_png|            |
+-----------------------+-----------------------+------------------------+
| |performance|         | |support|             | |other|                |
+-----------------------+-----------------------+------------------------+

========
Citation
========

If you use **BEDOPS** in your research, please cite the following manuscript:

   Shane Neph, M. Scott Kuehn, Alex P. Reynolds, et al. **BEDOPS: high-performance genomic feature operations**. *Bioinformatics* (2012) 28 (14): 1919-1920. `doi: 10.1093/bioinformatics/bts277 <http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract>`_

=================
Table of contents
=================

.. toctree::
   :numbered:

   content/overview
   content/installation
   content/revision-history
   content/usage-examples
   content/performance
   content/reference
   content/summary
   content/release

.. |downloads_png| image:: assets/index/downloads_v2.png
                   :height: 45px

.. |downloads_html| raw:: html

   <img src="_images/downloads_v2.png" style="height:45px;">

.. |linux_png| image:: assets/index/linux_v2.png
               :height: 26px
.. |linux_downloads| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="https://github.com/bedops/bedops/releases/download/v2.4.3/bedops_linux_x86_64-v2.4.3.tar.bz2">x86-64 (64-bit)</a> binaries</li>
   <li><a href="https://github.com/bedops/bedops/releases/download/v2.4.3/bedops_linux_i386-v2.4.3.tar.bz2">i386 (32-bit)</a> binaries</li>
   <li><a href="content/installation.html#linux">Installation instructions</a> for Linux hosts</li>
   </ul>

.. |macosx_png| image:: assets/index/macosx_v2.png
                :height: 26px
.. |macosx_downloads| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="https://github.com/bedops/bedops/releases/download/v2.4.3/BEDOPS.2.4.3.mpkg.zip">Intel (32-/64-bit, 10.7-10.9)</a> installer package</li>
   <li><a href="content/installation.html#mac-os-x">Installation instructions</a> for Mac OS X hosts</li>
   </ul>

.. |source_png| image:: assets/index/source_v2.png
                :height: 26px
.. |source_downloads| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="https://github.com/bedops/bedops/archive/v2.4.3.tar.gz">Source code</a> (tar.gz)</li>
   <li><a href="https://github.com/bedops/bedops/archive/v2.4.3.zip">Source code</a> (zip)</li>
   <li><a href="content/installation.html#installation-via-source-code">Compilation instructions</a></li>
   </ul>

.. |reference_png| image:: assets/index/reference_v2.png
                   :height: 54px

.. |set_operations_png| image:: assets/index/set_operations_v2.png
                        :height: 130px
.. |set_operations| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/reference/set-operations/bedops.html"><tt>bedops</tt></a> - apply set operations on any number<br/> of BED inputs</li>
   <li><a href="content/reference/set-operations/bedextract.html"><tt>bedextract</tt></a> - efficiently extract BED features</li>
   <li><a href="content/reference/set-operations/closest-features.html"><tt>closest-features</tt></a> - matches nearest<br/>features between BED files</li>
   </ul>

.. |statistics_png| image:: assets/index/statistics_v2.png
                    :height: 130px
.. |statistics| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/reference/statistics/bedmap.html"><tt>bedmap</tt></a> - map overlapping BED elements onto target regions and optionally compute any number of common statistical operations</li>
   </ul>

.. |file_management_png| image:: assets/index/file_management_v2.png
                         :height: 130px
.. |file_management| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/reference/file-management/sorting/sort-bed.html"><tt>sort-bed</tt></a> - apply lexicographical sort to BED data</li>
   <li><a href="content/reference/file-management/compression/starch.html"><tt>starch</tt></a> and <a href="content/reference/file-management/compression/unstarch.html"><tt>unstarch</tt></a> - compress and extract BED data</li>
   <li><a href="content/reference/file-management/compression/starchcat.html"><tt>starchcat</tt></a> - merge compressed archives</li>
   <li><a href="content/reference/file-management/conversion.html">Conversion tools</a> - convert common genomic formats to BED</li>
   </ul>

.. |performance_png| image:: assets/index/performance.png
.. |performance| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li>Parallel <a href="content/reference/file-management/compression/parallel_bam2bed.html"><tt>bam2bed</tt></a> and <a href="content/reference/file-management/compression/parallel_bam2starch.html"><tt>bam2starch</tt></a> - parallelized conversion and compression of BAM data</li>
   <li><a href="content/performance.html#set-operations-with-bedops">Set operations with <tt>bedops</tt></li>
   <li><a href="content/performance.html#compression-characteristics-of-starch">Compression characteristics of <tt>starch</tt></li>
   <li><a href="content/performance.html#independent-testing">Independent testing</a></li>
   </ul>

.. |support_png| image:: assets/index/support.png
.. |support| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/installation.html">How to install <strong>BEDOPS</strong></a></li>
   <li><a href="content/usage-examples.html">Usage examples</a> of <strong>BEDOPS</strong> tools in action</li>
   <li><a href="http://bedops.uwencode.org/forum/"><strong>BEDOPS</strong> user forum</li>
   <li><a href="http://groups.google.com/group/bedops-discuss"><strong>BEDOPS</strong> discusssion mailing list</li>
   </ul>

.. |other_png| image:: assets/index/other.png
.. |other| raw:: html

   <ul style="list-style-type:square; font-size:smaller; margin:10px; padding:0;">
   <li><a href="content/summary.html">Table summary</a> of <strong>BEDOPS</strong> toolkit</li>
   <li><a href="content/reference/file-management/compression/starch-specification.html">Starch v2.1</a> format specification</li>
   <li><a href="content/reference/set-operations/nested-elements.html">About nested elements</a>
   <li><a href="content/revision-history.html">Revision history</a></li>
   <li><a href="content/release.html">Github release instructions</a></li>
   <li><a href="https://github.com/bedops/bedops">Github repository</a></li>
   </ul>
