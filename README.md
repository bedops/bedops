BEDOPS
====

<img src="http://bedops.readthedocs.org/en/latest/_static/logo_with_label_v2.png" align="right"/>

Shane Neph, M. Scott Kuehn, Alex P. Reynolds, et al.  
[**BEDOPS: high-performance genomic feature operations**  
*Bioinformatics* (2012) 28 (14): 1919-1920.] (http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract)

BEDOPS is a suite of tools to address common questions raised in genomic studies — mostly with regard to overlap and proximity relationships between data sets. It aims to be scalable and flexible, facilitating the efficient and accurate analysis and management of large-scale genomic data.

The suite includes tools for set and statistical operations (<tt>bedops</tt>, <tt>bedmap</tt> and <tt>closest-features</tt>) and compression of large inputs into a novel lossless format (<tt>starch</tt>) that can provide greater space savings and faster data extractions than current alternatives. We offer native support for this compression format in these and other BEDOPS tools. BEDOPS also offers logarithmic speedups in access to per-chromosome regions in sorted BED data (via <tt>bedextract</tt>, <tt>bedops</tt>, <tt>bedmap</tt> and other BEDOPS tools). These tools make whole-genome analyses "embarassingly parallel", in that per-chromosome computations can be placed onto separate work nodes, with results collated at the end in [map-reduce](http://en.wikipedia.org/wiki/MapReduce) fashion.

<table>
    <tr>
        <td>Foo</td> 
        <td>Bar</td>
    </tr>
</table>
