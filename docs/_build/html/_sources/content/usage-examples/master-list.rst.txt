.. _master_list:

Collapsing multiple BED files into a master list by signal
==========================================================

Given a list of five-column UCSC BED files, where scores are kept in the fifth column, we want to build a "master list" of non-overlapping elements from all the inputs. Elements that initially overlap are ranked by score, and the highest scoring element is added to the master list.

===================
BEDOPS tools in use
===================

In the following example, we want to merge hotspot peaks for five fetal adrenal tissues, picking the highest scoring element where there are overlapping peaks. We'll use a mix of :ref:`bedmap` and its ``--max-element`` operation with :ref:`bedops` set operations to accomplish this.

======
Script
======

::

  #!/bin/bash
  # author : Bob Thurman

  beds=(fAdrenal-DS12528.dhs.bed
        fAdrenal-DS15123.dhs.bed
        fAdrenal-DS17319.dhs.bed
        fAdrenal-DS17677.dhs.bed
        fAdrenal-DS20343.dhs.bed)

  out=fAdrenal.master.merge.bed

  tmpd=/tmp/tmp$$
  mkdir -p $tmpd

  ## First, union all the peaks together into a single file.
  bedlist=""
  for bed in ${beds[*]}
  do
      bedlist="$bedlist $bed"
  done    

  bedops -u $bedlist > $tmpd/tmp.bed

  ## The master list is constructed iteratively.  For each pass through
  ## the loop, elements not yet in the master list are merged into
  ## non-overlapping intervals that span the union (this is just bedops
  ## -m).  Then for each merged interval, an original element of highest
  ## score within the interval is selected to go in the master list.
  ## Anything that overlaps the selected element is thrown out, and the
  ## process then repeats.
  iters=1
  solns=""
  stop=0
  while [ $stop == 0 ]
  do
      echo "merge steps..."

      ## Condense the union into merged intervals. This klugey bit
      ## before and after the merging is because we don't want to merge
      ## regions that are simply adjacent but not overlapping
      bedops -m --range 0:-1 $tmpd/tmp.bed \
          | bedops -u --range 0:1 - \
          > $tmpd/tmpm.bed
    
      ## Grab the element with the highest score among all elements forming each interval.
      ## If multiple elements tie for the highest score, just grab one of them.
      ## Result is the current master list.  Probably don't need to sort, but do it anyway
      ## to be safe since we're not using --echo with bedmap call.
      bedmap --max-element $tmpd/tmpm.bed $tmpd/tmp.bed \
          | sort-bed - \
          > $tmpd/$iters.bed
      solns="$solns $tmpd/$iters.bed"
      echo "Adding `awk 'END { print NR }' $tmpd/$iters.bed` elements"

      ## Are there any elements that don't overlap the current master
      ## list?  If so, add those in, and repeat.  If not, we're done.
      bedops -n 1 $tmpd/tmp.bed $tmpd/$iters.bed \
         > $tmpd/tmp2.bed

      mv $tmpd/tmp2.bed $tmpd/tmp.bed
 
      if [ ! -s $tmpd/tmp.bed ]
      then
          stop=1
      fi

      ((iters++))
  done

  ## final solution
  bedops -u $solns \
     > $out

  ## Clean up
  rm -r $tmpd

  exit 0

==========
Discussion
==========

A broad array of human cell tissue hotspot data for testing this example are available for public download from the UCSC Genome Browser:

* `http://genome.ucsc.edu/cgi-bin/hgFileUi?db=hg19&g=wgEncodeUwDnase <http://genome.ucsc.edu/cgi-bin/hgFileUi?db=hg19&g=wgEncodeUwDnase>`_

This includes hotspot data for ``DS12528``, ``DS15123``, ``DS17319``, ``DS17677`` and ``DS20343`` lines.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
