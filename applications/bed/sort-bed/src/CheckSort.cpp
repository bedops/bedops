#include <cstdio>
#include <fstream>
#include <string>

#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedTypes.hpp"
#include "utility/Exception.hpp"
#include "utility/FPWrap.hpp"

#include <iostream>
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
      IterType start((std::strcmp(bedFileNames[i], "-") == 0) ? std::cin : infile, bedFileNames[i]), end, tmp;
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
