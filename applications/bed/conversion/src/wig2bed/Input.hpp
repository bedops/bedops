/*
  Author: Scott Kuehn, Shane Neph
  Date:   Tue Oct 30 11:14:40 PDT 2007
*/
//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013, 2014 Shane Neph, Scott Kuehn and Alex Reynolds
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

    extern std::string prognm();
    extern std::string citation();
    extern std::string version();
    extern std::string authors();
    
    struct Help { /* */ };
    struct Version { /* */ };
    
    struct Input {

        static std::string Usage() {
            std::stringstream usage;
            usage << "  USAGE: wig2bed <file1.wig> [file2.wig...]                                                 \n";
            usage << "        Convert the format of <file1.wig> from UCSC wiggle to UCSC BED.  All wiggle formats \n";
            usage << "          defined at: http://genome.ucsc.edu/goldenPath/help/wiggle.html are acceptable.    \n";
            usage << "        May use '-' to indicate reading from stdin.                                         \n";
            usage << "        Options:                                                                            \n";
            usage << "          --help:                   Print this message and exit successfully.               \n";
            usage << "          --keep-header:            Preserve header information as BED elements.            \n";
            usage << "          --multisplit <basename>:  A single input file may have multiple wig sections, a   \n";
            usage << "                                     user may pass in more than one file, or both may occur.\n";
            usage << "                                     With this option, every separate input goes to a       \n";
            usage << "                                     separate output, starting with <basename>.1, then      \n";
            usage << "                                     <basename>.2, and so on.                               \n";
            usage << "          --version                 Print program information and exit successfully.        \n";
            return(usage.str());
        }
        
        Input(int argc, char **argv) : basename_(""), keepHeader_(false) {
            if ( argc < 2 )
                throw(std::string("No input file argument given."));
            
            int i = 1;
            for ( ; i < argc; ++i ) { // look for --help/--version
                std::string next = argv[i];
                if ( next == "--help" )
                  throw(Help());
                if ( next == "--version" )
                  throw(Version());
            } // for
            for ( i=1 ; i < argc; ++i ) {
                std::string next = argv[i];
                if ( next == "--multisplit" ) {
                    std::string afterNext = argv[++i];
                    if (( i == argc ) || ( (i + 1) == argc ) || ( afterNext.find("--") == 0 ))
                        throw(std::string("No argument given for --multisplit <basename>"));
                    basename_ = argv[i];
                }
                else if ( next == "--keep-header" )
                    keepHeader_ = true;
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
        bool keepHeader_;
        std::vector< std::istream* > inFiles_; // expected-warning {{can be safely ignored -- reordering variables will not eliminate padding and additional memory usage is not a concern}}
    };
    
} // namespace Wig2Bed

#endif // WIG2BED_INPUT_HPP
