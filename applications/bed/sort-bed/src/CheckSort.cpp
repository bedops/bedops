/*
  Author: Shane Neph and Alex Reynolds
    Date: Fri Feb 13 15:05:44 PST 2015
*/
//
//    BEDOPS
//    Copyright (C) 2011-2015 Shane Neph, Scott Kuehn and Alex Reynolds
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include <cstdio>
#include <fstream>
#include <string>

#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedTypes.hpp"
#include "utility/Exception.hpp"

#include "Structures.hpp"


int
checkSort(char const **bedFileNames, unsigned int numFiles)
{
  typedef Bed::bed_check_iterator<Bed::B3Rest*> IterType;
  int rtnval = EXIT_FAILURE;
  try {
    for ( unsigned int i = 0; i < numFiles; ++i ) {
      std::ifstream infile(bedFileNames[i]);
      if ( 0 != std::strcmp(bedFileNames[i], "-") && !infile )
        throw(Ext::UserError("Unable to find: " + std::string(bedFileNames[i])));
      IterType start((std::strcmp(bedFileNames[i], "-") == 0) ? std::cin : infile,
                     bedFileNames[i]), end;
      while ( start != end )
        delete *start++;
    } // for each file
    rtnval = EXIT_SUCCESS;
  } catch(std::exception& s) {
    std::fprintf(stderr, "%s\n", s.what());
  } catch(...) {
    std::fprintf(stderr, "Unknown problem\n");
  }
  return rtnval;
}
