/*
  File: Wig2Bed.cpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Tue Oct 30 11:14:40 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include "suite/BEDOPS.Constants.hpp"
#include "suite/BEDOPS.Version.hpp"

#include "Input.hpp"

namespace {
    const std::string& prognm() {
        static std::string* ret = new std::string("wig2bed");
        return *ret;
    }
    const std::string& citation() {
        static std::string* ret = new std::string(BEDOPS::citation());
        return *ret;
    }
    const std::string& version() {
        static std::string* ret = new std::string(BEDOPS::revision());
        return *ret;
    }
    const std::string& authors() {
        static std::string* ret = new std::string("Scott Kuehn & Shane Neph");
        return *ret;
    }
} // unnamed

int
main(int argc, char **argv)
{
    bool iserror = true;
    try
        {
            Wig2Bed::Input progInput(argc, argv);
            bool multiout = !progInput.basename_.empty();
            int cntr = 1;
            bool startWrite = false;
            FILE* outfile = stdout;
            static const char* keepHeaderChrom = "_header";
            bool keepHeader = progInput.keepHeader_;

            for(Wig2Bed::Input::InFileIterator i = progInput.inFiles_.begin(); i != progInput.inFiles_.end(); i++)	  
                {
                    if ( multiout ) {
                        std::stringstream con;
                        con << progInput.basename_ << "." << cntr++;
                        std::string s = con.str();
                        outfile = std::fopen(s.c_str(), "w");
                    }
                    Bed::LineCountType line = 0;
                    Bed::LineCountType posLines = 0;
                    Bed::CoordType span = 0;
                    Bed::CoordType step = 0;
                    Bed::CoordType startPos = 0;
                    Bed::CoordType endPos = 0;
                    float score;
                    char chromBuf[20];
                    bool isFixedStep = false;
                    std::string currLine;
                    startWrite = false;
                    int keepHeaderPos = 0;
                    
                    while(std::getline(**i, currLine))
                        {
                            line++;
                            
                            /*Ignore leading white space and comments*/
                            std::size_t lineStartPos = currLine.find_first_not_of(" \t");
                            if ( lineStartPos == std::string::npos || currLine[lineStartPos] == '#' ) {
                                if ( multiout && startWrite ) {
                                    startWrite = false;
                                    std::fclose(outfile);
                                    std::stringstream con;
                                    con << progInput.basename_ << "." << cntr++;
                                    keepHeaderPos = 0;
                                    std::string s = con.str();
                                    outfile = std::fopen(s.c_str(), "w");
                                }
                                if ( keepHeader ) {
                                    std::fprintf(outfile, "%s\t%d\t%d\t%s\n", keepHeaderChrom, keepHeaderPos, (keepHeaderPos + 1), currLine.c_str());
                                    keepHeaderPos++;
                                }
                                continue;
                            }
                            currLine.assign(currLine, lineStartPos, currLine.size() - lineStartPos);

                            /* Evaluate Wig Line */
                            if ( (std::strncmp(currLine.c_str(), "track", 5) == 0) || std::strncmp(currLine.c_str(), "browser", 7) == 0 ) {
                                if ( multiout && startWrite ) {
                                    startWrite = false;
                                    std::fclose(outfile);
                                    std::stringstream con;
                                    con << progInput.basename_ << "." << cntr++;
                                    keepHeaderPos = 0;
                                    std::string s = con.str();
                                    outfile = std::fopen(s.c_str(), "w");
                                }
                                if ( keepHeader ) {
                                    std::fprintf(outfile, "%s\t%d\t%d\t%s\n", keepHeaderChrom, keepHeaderPos, (keepHeaderPos + 1), currLine.c_str());
                                    keepHeaderPos++;
                                }
                                continue;
                            }
                            else if (std::strncmp(currLine.c_str(), "variableStep", 12) == 0) 
                                {
                                    int fields = std::sscanf(currLine.c_str(), "variableStep chrom=%s span=%" SCNu64 "\n", chromBuf, &span);
                                    if (fields < 1) 
                                        {
                                            std::stringstream err;
                                            err << "Invalid variableStep header on line " << line << std::endl;
                                            throw(err.str());
                                        }
                                    if (fields == 1) 
                                        span = 1;
                                    isFixedStep = false;
                                    if ( multiout && startWrite ) {
                                        startWrite = false;
                                        std::fclose(outfile);
                                        std::stringstream con;
                                        con << progInput.basename_ << "." << cntr++;
                                        keepHeaderPos = 0;
                                        std::string s = con.str();
                                        outfile = std::fopen(s.c_str(), "w");
                                    }
                                    if ( keepHeader ) {
                                        std::fprintf(outfile, "%s\t%d\t%d\t%s\n", keepHeaderChrom, keepHeaderPos, (keepHeaderPos + 1), currLine.c_str());
                                        keepHeaderPos++;
                                    }
                                }
                            else if (std::strncmp(currLine.c_str(), "fixedStep", 9) == 0) 
                                {
                                    int fields = std::sscanf(currLine.c_str(), "fixedStep chrom=%s start=%" SCNu64 " step=%" SCNu64 " span=%" SCNu64 "\n", chromBuf, 
                                                             &startPos, &step, &span);
                                    if (fields < 3) 
                                        {
                                            std::stringstream err;
                                            err << "Invalid fixedStep header on line " << line << std::endl;
                                            throw(err.str());
                                        }
                                    if(fields == 3)
                                        span = 1;
                                    isFixedStep = true;
                                    if ( multiout && startWrite ) {
                                        startWrite = false;
                                        std::fclose(outfile);
                                        std::stringstream con;
                                        con << progInput.basename_ << "." << cntr++;
                                        keepHeaderPos = 0;
                                        std::string s = con.str();
                                        outfile = std::fopen(s.c_str(), "w");
                                    }
                                    if ( keepHeader ) {
                                        std::fprintf(outfile, "%s\t%d\t%d\t%s\n", keepHeaderChrom, keepHeaderPos, (keepHeaderPos + 1), currLine.c_str());
                                        keepHeaderPos++;
                                    }
                                }
                            else if (std::strncmp(currLine.c_str(), "chr", 3) == 0) 
                                {
                                    /*Bed Format*/
                                    posLines++;
                                    int fields = std::sscanf(currLine.c_str(), "%s\t%" SCNu64 "\t%" SCNu64 "\t%f\n", chromBuf, 
                                                             &startPos, &endPos, &score);
                                    if(fields != 4)
                                        {
                                            std::stringstream err;
                                            err << "Invalid wig line: " << line << std::endl;
                                            throw(err.str());
                                        }
                                    std::fprintf(outfile, "%s\t%" PRIu64 "\t%" PRIu64 "\tid-%" PRIu64 "\t%f\n", chromBuf, startPos-1, endPos-1, posLines, score);
                                    startWrite = true;
                                }
                            else
                                {
                                    /* Data Value */
                                    if(isFixedStep)
                                        {
                                            int fields = std::sscanf(currLine.c_str(), "%f\n", &score);
                                            if(fields != 1)
                                                {
                                                    std::stringstream err;
                                                    err << "Invalid wig line: " << line << std::endl;
                                                    throw(err.str());
                                                }
                                            posLines++;
                                            std::fprintf(outfile, "%s\t%" PRIu64 "\t%" PRIu64 "\tid-%" PRIu64 "\t%f\n", chromBuf, startPos-1, startPos+span-1, posLines, score);
                                            startPos += step;
                                            startWrite = true;
                                        }
                                    else
                                        {
                                            /* Variable step*/
                                            int fields = std::sscanf(currLine.c_str(), "%" SCNu64 "\t%f\n", &startPos, &score);
                                            if(fields !=2)
                                                {
                                                    std::stringstream err;
                                                    err << "Invalid wig line: " << line << std::endl;
                                                    throw(err.str());
                                                }
                                            posLines++;
                                            std::fprintf(outfile, "%s\t%" PRIu64 "\t%" PRIu64 "\tid-%" PRIu64 "\t%f\n", chromBuf, startPos-1, startPos+span-1, posLines, score);
                                            startWrite = true;
                                        }
                                }
                        }
                }
            iserror = false;
            if ( multiout && startWrite )
                std::fclose(outfile);
        }
    catch(Wig2Bed::Help)
        {
            std::cout << prognm() << std::endl;
            std::cout << "	citation: " + citation() << std::endl;
            std::cout << "	version:  " + version() << std::endl;
            std::cout << "	authors:  " + authors() << std::endl;
            std::cout << std::endl << Wig2Bed::Input::Usage() << std::endl << std::endl;
            iserror = false;
        }
    catch(const std::string& msg)
        {
            std::cerr << prognm() << std::endl;
            std::cerr << "  citation: " << citation() << std::endl;
            std::cerr << "  version:	" + version() << std::endl;
            std::cerr << "  authors:	" + authors() << std::endl;
            std::cerr << std::endl << Wig2Bed::Input::Usage() << std::endl << std::endl;
            std::cerr << "  " << msg << std::endl;
            iserror = true;
        }
    catch(const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
        }
    catch(...)
        {
            std::cerr << "Error:  Aborting" << std::endl;
        }

    if ( iserror )
        return(EXIT_FAILURE);
    return(EXIT_SUCCESS);
}

