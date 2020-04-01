/*
  Author: Scott Kuehn, Shane Neph
  Date: Fri Oct 19 08:20:50 PDT 2007
*/
//
//    BEDOPS
//    Copyright (C) 2011-2020 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef _FEATDIST_INPUT_HPP
#define _FEATDIST_INPUT_HPP

#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"

#include <set>
#include <string>

namespace FeatDist {

  struct NoInput { /* */ };

  struct HelpException { /* */ };

  struct VersionException { /* */ };

  //========
  // Input:
  //========
  struct Input {
    // Constructor
    Input(int argc, char **argv)
      : ec_(false), shortestOnly_(false), distances_(false), suppressRef_(false),
        overlaps_(true), delim_("|"), refFile_(""), nonRefFile_(""), chr_("all") {

      typedef Ext::UserError UE;
      if ( 1 == argc )
        throw(NoInput());
      int argcntr = 1;
      bool outoption = false; // may choose up to one type of non-default output option

      // [Process-Flags]
      std::string next = "";
      while ( argcntr < argc ) {
        next = argv[argcntr];
        if ( next == "--help" )
          throw(HelpException());
        else if ( next == "--version" )
          throw(VersionException());
        else if ( next == "--ec" || next == "--header" )
          ec_ = true;
        else if ( next == "--no-overlaps" )
          overlaps_ = false;
        else if ( next == "--delim" ) {
          Ext::Assert<UE>(++argcntr < argc, "No value given for --delim.");
          delim_ = argv[argcntr];
        }
        else if ( next == "--chrom" ) {
          Ext::Assert<UE>(++argcntr < argc, "No value given for --chrome.");
          chr_ = argv[argcntr];
        }
        else if ( next == "--closest" ) {
          Ext::Assert<UE>(!outoption, "Multiple output options not allowed.");
          shortestOnly_ = true;
          outoption = true;
        }
        else if ( next == "--dist" )
          distances_ = true;
        else if ( next == "--no-ref" )
          suppressRef_ = true;
        else if ( next == "--help" )
          throw(HelpException());
        else if ( next == "--shortest" ) { // silently supported for bckwd compatibility
          Ext::Assert<UE>(!outoption, "Multiple output options not allowed.");
          shortestOnly_ = true;
          outoption = true;
        } else { // the rest are two input files?
          Ext::Assert<UE>(argcntr + 2 == argc, "Unknown option: " + next + ".");
          break;
        }
        ++argcntr;
      } // while

      Ext::Assert<UE>(argcntr + 2 == argc, "Not enough input files given.");
      refFile_ = argv[argcntr++];
      nonRefFile_ = argv[argcntr];
      Ext::Assert<UE>(refFile_.find("--") != 0, "Option given where file expected: " + refFile_ + ".");
      Ext::Assert<UE>(nonRefFile_.find("--") != 0, "Option given where file expected: " + nonRefFile_ + ".");
    }

    bool AllowOverlaps() const
      { return(overlaps_); }

    std::string Chrome() const
      { return(chr_); }

    std::string Delimiter() const
      { return(delim_); }

    bool ErrorCheck() const
      { return(ec_); }

    std::string GetNonReferenceFileName() const
      { return(nonRefFile_); }

    std::string GetReferenceFileName() const
      { return(refFile_); }

    bool PrintDistances() const
      { return(distances_); }

    bool ShortestOnly() const
      { return(shortestOnly_); }

    bool SuppressReference() const
      { return(suppressRef_); }

  private:
    bool ec_;
    bool shortestOnly_;
    bool distances_, suppressRef_, overlaps_;
    std::string delim_;
    std::string refFile_, nonRefFile_;
    std::string chr_;
  };


  //---------
  // Usage()
  std::string Usage() {
    std::string msg = "\nUSAGE: closest-features [Process-Flags] <input-file> <query-file>\n";
    msg += "   All input files must be sorted per sort-bed.\n";
    msg += "   The program accepts BED and Starch file formats\n";
    msg += "   May use '-' for a file to indicate reading from standard input (BED format only).\n";
    msg += "\n";
    msg += "   For every element in <input-file>, determine the two elements from <query-file> falling\n";
    msg += "     nearest to its left and right edges (See NOTES below).  By default, echo the <input-file>\n";
    msg += "     element, followed by those left and right elements found in <query-file>.\n";
    msg += "\n";
    msg += "  Process Flags:\n";
    msg += "    --chrom <chromosome>   Jump to and process data for given <chromosome> only.\n";
    msg += "    --closest              Choose the closest element for output only.  Ties go the left element.\n";
    msg += "    --delim <delim>        Change output delimiter from '|' to <delim> between columns (e.g. \'\\t\')\n";
    msg += "    --dist                 Print the signed distances to the <input-file> element as additional\n";
    msg += "                             columns of output.  An overlapping element has a distance of 0.\n";
    msg += "    --ec                   Error check all input files (slower).\n";
    msg += "    --header               Accept headers (VCF, GFF, SAM, BED, WIG) in any input file.\n";
    msg += "    --help                 Print this message and exit successfully.\n";
    msg += "    --no-overlaps          Overlapping elements from <query-file> will not be reported.\n";
    msg += "    --no-ref               Do not echo elements from <input-file>.\n";
    msg += "    --version              Print program information.\n";
    msg += "\n";
    msg += "  NOTES:\n";
    msg += "    If an element from <query-file> overlaps the <input-file> element, its distance is zero.\n";
    msg += "      An overlapping element takes precedence over all non-overlapping elements.  This is true\n";
    msg += "      even when the overlapping element\'s edge-to-edge distance to the <input-file>\'s element\n";
    msg += "      is greater than the edge-to-edge distance from a non-overlapping element.\n";
    msg += "    Overlapping elements may be ignored completely (no precedence) with --no-overlaps.\n";
    msg += "    Elements reported as closest to the left and right edges are never the same.\n";
    msg += "    When no qualifying element from <query-file> exists as a closest feature, 'NA' is reported.\n";
    return(msg);
  }

} // namespace FeatDist

#endif // _FEATDIST_INPUT_HPP
