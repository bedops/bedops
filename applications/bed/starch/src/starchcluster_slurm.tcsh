#!/bin/tcsh -ef

# author  : sjn and apr
# date    : 13 Sep 2016
# version : v2.4.39

#
#    BEDOPS
#    Copyright (C) 2011-2020 Shane Neph, Scott Kuehn and Alex Reynolds
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

####################################################
# cluster variables:
#  change to match your environment
####################################################

set slurm_opts = "-D $PWD -o /dev/null -e /dev/null"

############################
# some input error checking
############################

set help = "\nUsage: starchcluster_slurm [--help] [--clean] <input-bed-file> [output-starch-file]\n\n"
set help = "$help  Pass in the name of a BED file to create a starch archive using the cluster.\n\n"
set help = "$help  (stdin isn't supported through this wrapper script, but starch supports it natively.)\n\n"
set help = "$help  Add --clean to remove <input-bed-file> after starching it up.\n\n"
set help = "$help  You can pass in the name of the output starch archive to be created.\n"
set help = "$help  Otherwise, the output will have the same name as the input file, with an additional\n"
set help = "$help   '.starch' ending.  If the input file ends with '.bed', that will be stripped off.\n"

if ( $#argv == 0 ) then
  printf "$help"
  exit -1
endif

@ inputset = 0
@ clean = 0
foreach argc (`seq 1 $#argv`)
  if ( "$argv[$argc]" == "--help" ) then
    printf "$help"
    exit 0
  else if ( "$argv[$argc]" == "--clean" ) then
    @ clean = 1
  else if ( $argc == $#argv ) then
    if ( $inputset > 0 ) then
      set output = "$argv[$argc]"
    else
      set originput = "$argv[$argc]"
      set output = $originput:t.starch
    endif
    @ inputset = 1
  else if ( $inputset > 0 ) then
    printf "$help"
    printf "Multiple input files cannot be specified\n"
    exit -1
  else
    set originput = "$argv[$argc]"
    set output = $originput:t.starch
    @ inputset = 1
  endif
end

if ( $inputset == 0 ) then
  printf "No input file specified\n"
  exit -1
else if ( ! -s $originput ) then
  printf "Unable to find file: %s\n" $originput
  exit -1
else if ( "$output" == "$originput:t.starch" && "$originput:e" == "bed" ) then
  set output = "$originput:t:r.starch"
endif

###############################################################
# new working directory to keep file pileups local to this job
###############################################################

set nm = scs.`uname -a | cut -f2 -d' '`.$$
if ( -d $nm ) then
  rm -rf $nm
endif
mkdir -p $nm

set here = `pwd`
cd $nm
if ( -s ../$originput ) then
  set input = ../$originput
else
  # $originput includes absolute path
  set input = $originput
endif

# $output:h gives back $output if there is no directory information
if ( -d ../$output:h || "$output:h" == "$output" ) then
  set output = $here/$output
else if ( `echo $output | awk '{ print substr($0, 1, 1); }'` == "/" ) then
  # $output includes absolute path
else
  # $output includes non-absolute path
  set output = $here/$output
endif

#####################################################
# extract information by chromosome and starch it up
#####################################################

set files = ()
set jids = ()
@ cntr = 0
foreach chrom (`bedextract --list-chr $input`)
  set res = `sbatch $slurm_opts -J "$nm.$cntr" --wrap="module add bedops; bedextract $chrom $input | starch - > $nm/$cntr"`
  set slurm_jid = `echo $res | sed 's|^Submitted batch job ||'`
  set jids = ($jids $slurm_jid)
  set files = ($files $nm/$cntr)
  @ cntr++
end

if ( $cntr == 0 ) then
  printf "Program problem: no files were submitted to the cluster?\n"
  exit -1
endif

##################################################
# create final starch archive and clean things up
##################################################

set dependencies = `echo $jids | tr ' ' ':'`
sbatch $slurm_opts -J $nm.union --dependency=afterok:$dependencies --wrap="module add bedops; starchcat $files > $output; cd $here; rm -rf $nm; if [ $clean != 0 ]; then rm -f $originput; fi;"

exit 0
