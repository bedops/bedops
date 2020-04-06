#!/bin/tcsh -ef

# author  : sjn and apr
# date    : Feb 2012
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

############################
# some input error checking
############################

set help = "\nUsage: starchcluster_gnuParallel [--help] [--clean] <input-bed-file> [output-starch-file]\n\n"
set help = "$help  Pass in the name of a BED file to create a Starch archive using GNU Parallel.\n\n"
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

set nm = scg.`uname -a | cut -f2 -d' '`.$$
if ( -d $nm ) then
  rm -rf $nm
endif
mkdir -p $nm

set here = `pwd`
cd $nm
if ( -s ../$originput ) then
  set input = $here/$originput
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

@ chrom_count = `bedextract --list-chr $input | awk 'END { print NR }'`

bedextract --list-chr $input | parallel "bedextract {} $input | starch - > $here/$nm/{}.starch"

@ extracted_file_count = `find $here/$nm -name '*.starch' | wc -l`
if ( $chrom_count != $extracted_file_count ) then
  printf "Error: Only some or no files were submitted to GNU Parallel successfully\n"
  exit -1
endif

##################################################
# create final starch archive and clean things up
##################################################

starchcat $here/$nm/*.starch > $output
cd $here
rm -rf $nm
if ( $clean > 0 ) then
  rm -f $originput
endif

exit 0
