/*
  FILE: Input.cpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Tue Oct 30 11:14:40 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef WIG2BED_INPUT_HPP
#define WIG2BED_INPUT_HPP

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Wig2Bed {

struct Help { /* */ };

struct Input {

  static std::string Usage() {
    std::stringstream usage;
    usage << "  USAGE: wig2bed <file1.wig> [file2.wig...]                                                 \n";
    usage << "        Convert the format of <file1.wig> from UCSC wiggle to UCSC BED.  All wiggle formats \n";
    usage << "          defined at: http://genome.ucsc.edu/goldenPath/help/wiggle.html are acceptable.    \n";
    usage << "        May use '-' to indicate reading from stdin.                                         \n";
    usage << "        Options:                                                                            \n";
    usage << "          --help:                   Print this message and exit successfully.               \n";
    usage << "          --multisplit <basename>:  A single input file may have multiple wig sections, a   \n";
    usage << "                                     user may pass in more than one file, or both may occur.\n";
    usage << "                                     With this option, every separate input goes to a       \n";
    usage << "                                     separate output, starting with <basename>.1, then      \n";
    usage << "                                     <basename>.2, and so on.                               \n";
    return(usage.str());
  }

  Input(int argc, char **argv) : basename_("") {
    if ( argc < 2 )
      throw(std::string("No input file argument given."));

    int i = 1;
    for ( ; i < argc; ++i ) {
      std::string next = argv[i];
      if ( next == "--help" )
        throw(Help());
      else if ( next == "--multisplit" ) {
        if ( ++i == argc )
          throw(std::string("No argument given for --multisplit <basename>"));
        basename_ = argv[i];
      }
      else if ( next.find("--") == 0 )
        throw(std::string("Unknown arg: " + next));
      else
        break;
    } // for


    // The rest are, presumably, input files
    int numFiles = argc - i;
    if ( numFiles <= 0 )
      throw(std::string("No input file argument given."));

    bool usingStdin = false;
    for ( ; i < argc; ++i ) {
      std::string next = argv[i];
      if ( next == "-" ) {
        if ( usingStdin )
          throw(std::string("Cannot specify '-' more than once for reading stdin."));
        inFiles_.push_back(&std::cin);
        usingStdin = true;
      } else {
        std::ifstream* iptr = new std::ifstream(next.c_str());
        if ( !(*iptr) )
          throw(std::string("Unable to find file: ") + next);
        inFiles_.push_back(iptr);
      }
    } // for
  }

  ~Input() {
    InFileIterator ifi = inFiles_.begin();
    while ( ifi != inFiles_.end() ) {
      if ( *ifi != &std::cin )
        delete *(ifi++);
      else
        ++ifi;
    } // while
  }

  typedef std::vector< std::istream *>::const_iterator InFileIterator;
  std::string basename_;
  std::vector< std::istream* > inFiles_;
};

} // namespace Wig2Bed

#endif // WIG2BED_INPUT_HPP
