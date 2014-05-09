//=========
// Author:  Shane Neph & Scott Kuehn
// Date:    Fri Aug 13 15:00:25 PDT 2010
// Project: bedops
// ID:      $Id$
//=========

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

// Macro Guard
#ifndef INPUT_BEDOPS_H
#define INPUT_BEDOPS_H

// Files included
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"



namespace BedOperations {


// enumerations
enum ModeType
  { MERGE, INTERSECTION, COMPLEMENT, DIFFERENCE, SYMMETRIC_DIFFERENCE,
    UNIONALL, ELEMENTOF, NOTELEMENTOF, PARTITION };

struct NoInput { /* */ };
struct HelpException { /* */ };
struct ExtendedHelpException {
  explicit ExtendedHelpException(ModeType m) : m_(m) { /* */ }
  ModeType m_;
};
struct Version { /* */ };

//=======
// Input
//=======
struct Input {

  // Constructor
  Input(int argc, char** argv) : ft_(MERGE), numFiles_(0), minFiles_(1000),
                                 allFiles_(), perc_(1), usePerc_(true),
                                 errorCheck_(false), lpad_(0), rpad_(0), leftMost_(0),
                                 chrSpecific_(false), chr_("all") {

    typedef Ext::UserError UE;

    try {
      // Basic error checking
      loadOptions();
      if ( argc <= 1 )
        throw(NoInput());

      bool hasOption = false;
      int argcntr = 1;
      const std::string plusints = "0123456789";
      bool hasRange = false;
      while ( argcntr < argc ) {
        std::string next = argv[argcntr];
        if ( next == "--ec" ) {
          errorCheck_ = true;
        } else if ( next == "--header" ) {
          errorCheck_ = true;
        } else if ( next == "--chrom" ) {
          Ext::Assert<UE>(!chrSpecific_, "--chrom specified multiple times.");
          Ext::Assert<UE>(++argcntr < argc, "No value for --chrom given.");
          chr_ = argv[argcntr];
          chrSpecific_ = (chr_ != "all");
        } else if ( next == "--range" ) {
          Ext::Assert<UE>(!hasRange, "--range specified multiple times.");
          Ext::Assert<UE>(++argcntr < argc, "No value for --range given.");
          next = argv[argcntr];
          if ( next.find(":") != std::string::npos ) {
            std::string leftVal = next.substr(0, next.find(":"));
            std::string rightVal = next.substr(next.find(":")+1);
            Ext::Assert<UE>(leftVal.size() > 0,
                            "integer expected for the 'L' value of --range L:R.");
            Ext::Assert<UE>(rightVal.size() > 0,
                            "integer expected for the 'R' value of --range L:R.");
            Ext::Assert<UE>(leftVal.find_first_not_of("-" + plusints) == std::string::npos,
                            "integer expected for the 'L' value of --range L:R.");
            Ext::Assert<UE>(rightVal.find_first_not_of("-" + plusints) == std::string::npos,
                            "integer expected for the 'R' value of --range L:R.");

            std::string::size_type t = leftVal.find_first_of("-");
            std::string::size_type u = leftVal.find_last_of("-");
            Ext::Assert<UE>(t == u, "multiple '-' signs detected for 'L' value of --range option");

            t = rightVal.find_first_of("-");
            u = rightVal.find_last_of("-");
            Ext::Assert<UE>(t == u, "multiple '-' signs detected for 'R' value of --range option");

            std::stringstream conv1(leftVal);
            conv1 >> lpad_;
            std::stringstream conv2(rightVal);
            conv2 >> rpad_;
            hasRange = true;
          } else {
            Ext::Assert<UE>(next.find_first_not_of("-" + plusints) == std::string::npos,
                            "integer value expected for --range");
            std::string::size_type t = next.find_first_of("-");
            std::string::size_type u = next.find_last_of("-");
            Ext::Assert<UE>(t == u, "multiple '-' signs detected in <val> for --range option");
            std::stringstream conv(next);
            int range;
            conv >> range;
            lpad_ = -range;
            rpad_ = range;
            hasRange = true;
          }
        } else if ( next == "--help" ) {
          throw(HelpException());
        } else if ( next.find("--help-") == 0 ) { // detailed help?
          next = next.substr(7);
          Ext::Assert<UE>(!next.empty(), "No operation argument specified as part of --help-");
          bool longOption = (next.size() > 1);
          if ( longOption ) {
            std::map<std::string, std::string>::iterator i = options_.find("--" + next);
            Ext::Assert<UE>(i != options_.end(), "Unknown operation specified with --help-: " + next);
            next = i->second;
          } else {
            Ext::Assert<UE>(next.size() == 1, "Unknown operation specified as part of --help-: " + next);
            next = "-" + next;
          }

          try {
           setModeType(next[1]);
          } catch(...) {
            throw(UE("Unknown operation argument specified as part of --help-"));
          }
          throw(ExtendedHelpException(ft_));
        } else if ( next == "--version" ) {
          throw(Version());
        } else if ( next.find("-") != 0 ) {
          break;
        } else if ( next.size() > 1 ) { // sz of 1 means stdin
          std::string::size_type startOpt = next.find_first_not_of("-");
          Ext::Assert<Ext::UserError>(startOpt != std::string::npos, "Bad option: " + next);
          Ext::Assert<Ext::UserError>(!hasOption, "More than one operation specified: " + next);
          hasOption = true;

          bool longOption = (next.find("--") == 0);
          if ( longOption ) {
            std::map<std::string, std::string>::iterator i = options_.find(next);
            Ext::Assert<UE>(i != options_.end(), "Unknown operation: " + next);
            next = i->second;
          }

          // Set up operating mode
          Ext::Assert<UE>(next.size() == 2, "Unknown operation: " + next);
          setModeType(next[1]);

          // More argument possibilities for -e, -n, and -c
          if ( ft_ == ELEMENTOF || ft_ == NOTELEMENTOF ) { // extra args?
            int maxCount = 1, cntr = 0;
            int start = argcntr + 1;
            for ( int i = start; i < argc; ++i ) {
              std::string::size_type sz = std::string(argv[i]).size();
              if ( argv[i][0] == '-' && sz > 1 ) { // sz of 1 means stdin
                if ( std::string(argv[i]).find("--") == std::string::npos ) {
                  setSubsetOption(argv[i]);
                  ++argcntr;
                } else {
                  break;
                }
              }
              else
                break;
              ++cntr;
            } // for
            std::string msg = "Expect (at most) 1 argument with -e/-n operations";
            Ext::Assert<Ext::UserError>(cntr <= maxCount, msg);
          } else if ( ft_ == COMPLEMENT ) {
            int maxCount = 1, cntr = 0;
            int start = argcntr + 1;
            for ( int i = start; i < argc; ++i ) {
              if ( std::string(argv[i]) == "-L" ) {
                setComplementOption();
                ++argcntr;
              }
              else
                break;
              ++cntr;
            } // for
            std::string msg = "-L specified multiple times with --complement";
            Ext::Assert<Ext::UserError>(cntr <= maxCount, msg);
          }
        }
        else
          break;
        ++argcntr;
      } // while

      // More basic error checking
      Ext::Assert<UE>(argcntr < argc, "No input file given.");
      Ext::Assert<UE>(hasOption, "No operation argument given.");

      // Check file input(s); ensure minimum number of files is met
      bool onlyOne = true;
      for ( int i = argcntr; i < argc; ++i ) {
        if ( std::string(argv[i]) == "-" ) {
          Ext::Assert<UE>(onlyOne, "Too many '-'");
          allFiles_.push_back("-");
          onlyOne = false;
        }
        else {
          Ext::Assert<UE>(std::string(argv[i])[0] != '-', "Bad option: " + std::string(argv[i]));
          std::ifstream check(argv[i]);
          Ext::Assert<UE>(static_cast<bool>(check), "Cannot find " + std::string(argv[i]));
          check.close();
          allFiles_.push_back(argv[i]);
        }
        ++numFiles_;
      } // for
      Ext::Assert<Ext::UserError>(numFiles_ >= minFiles_, "Not enough files");
    } catch(HelpException& he) {
      throw;
    } catch(ExtendedHelpException& ehe) {
      throw;
    } catch(UE& ie) {
      std::string msg = "Bad Input\n";
      msg += ie.what();
      Ext::UserError toThrow(msg);
      throw(toThrow);
    }
  }

  // Public methods
  std::string Chrom() const {
    return(chr_);
  }

  bool ChrSpecific() const {
    return(chrSpecific_);
  }

  bool ErrorCheck() const {
    return(errorCheck_);
  }
  std::string GetFileName(int i) const {
    return(allFiles_.at(i));
  }
  ModeType GetModeType() const {
    return(ft_);
  }
  int GetLeftPad() const {
    return(lpad_);
  }
  int GetRightPad() const {
    return(rpad_);
  }
  int NumberFiles() const {
    return(numFiles_);
  }
  double Threshold() const {
    return(perc_);
  }
  bool ComplementFullLeft() const {
    return(leftMost_);
  }
  bool UsePercentage() const {
    return(usePerc_);
  }

  void setSubsetOption(const std::string& str) {
    typedef Ext::UserError UE;
    std::string l = str.substr(1); // get rid of -
    std::string::size_type pos = l.find("%");
    const std::string nums = ".1234567890";
    const std::string ints = "1234567890";
    if ( pos != std::string::npos ) {
      Ext::Assert<UE>(pos + 1 == l.size(), "Bad placement of %");
      std::string value = l.substr(0, pos); // get rid of %
      Ext::Assert<UE>(!value.empty(), "Bad -% value");
      std::stringstream conv(value);
      Ext::Assert<UE>(value.find_first_not_of(nums) == std::string::npos,
                      "Bad: -% value");
      conv >> perc_;
      perc_ /= 100.0;
      if ( perc_ > 1 )
        throw(Ext::UserError("Expect percentage less than or equal to 100%"));
      usePerc_ = true;
      if ( perc_ == 0 ) { // 0% can match *everything*: convert to 1bp
        perc_ = 1;
        usePerc_ = false;
      }
    }
    else if ( l.find_first_not_of(ints) == std::string::npos ) {
      perc_ = atoi(l.c_str());
      usePerc_ = false; // pretend perc_ is # of bp
    }
    else if ( l.find_first_not_of(nums) == std::string::npos ) {
      throw(Ext::UserError("Fractional amounts require a '%' symbol (e.g.; 5.4% not 5.4 base-pair)"));
    }
    else
      throw(Ext::UserError("Unknown arg: " + str));
  }

  void setComplementOption() {
    // assumes 0-based coordinates
    leftMost_ = true;
  }

  void setModeType(char t) {
    ModeType ft = MERGE;
    int min = 1;
    std::string badInput = "Unknown operation: -";
    badInput += t;
    switch(t) {
      case 'c': case 'C':
        ft = COMPLEMENT; break;
      case 'd': case 'D':
        ft = DIFFERENCE; ++min; break;
      case 'e': case 'E':
        ft = ELEMENTOF; ++min; break;
      case 'i': case 'I':
        ft = INTERSECTION; ++min; break;
      case 'm': case 'M':
        ft = MERGE; break;
      case 'n': case 'N':
        ft = NOTELEMENTOF; ++min; break;
      case 'p': case 'P':
        ft = PARTITION; break;
      case 's': case 'S':
        ft = SYMMETRIC_DIFFERENCE; ++min; break;
      case 'u': case 'U':
        ft = UNIONALL; break;
      default:
        throw(Ext::UserError(badInput));
    };
    ft_ = ft;
    minFiles_ = min;
  }

  void loadOptions() {
    options_.insert(std::make_pair("--complement", "-c"));
    options_.insert(std::make_pair("--difference", "-d"));
    options_.insert(std::make_pair("--element-of", "-e"));
    options_.insert(std::make_pair("--intersect", "-i"));
    options_.insert(std::make_pair("--merge", "-m"));
    options_.insert(std::make_pair("--not-element-of", "-n"));
    options_.insert(std::make_pair("--partition", "-p"));
    options_.insert(std::make_pair("--symmdiff", "-s"));
    options_.insert(std::make_pair("--everything", "-u"));
  }

private:
  ModeType ft_;
  int numFiles_;
  int minFiles_;
  std::vector<std::string> allFiles_;
  double perc_;
  bool usePerc_;
  bool errorCheck_;
  int lpad_;
  int rpad_;
  bool leftMost_;
  bool chrSpecific_;
  std::string chr_;
  std::map<std::string, std::string> options_;
};



  //============
  // example1()
  //============
  std::string example1() {
      std::string ex = "        file1.bed:\n";
      ex += "          chr1  10    100   id-1  5\n";
      ex += "          chr1  50    125\n";
      ex += "          chr1  2000  2500  id-3  54  +\n";
      ex += "          \n\n";
      ex += "        file2.bed:\n";
      ex += "          chr1   250   400\n";
      ex += "          chr1   2100  2125\n";
      ex += "          chr21  500   1000\n";
      ex += "          \n\n";
      return(ex);
  }


  //=================
  // DetailedUsage()
  //=================
  std::string DetailedUsage(ModeType mode) {
    std::string msg = "\n";
    if ( mode == COMPLEMENT ) {
      msg += "        Using the -c or --complement option requires at least 1 BED file input.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Reports the intervening intervals in between all coordinates found in the input file(s).\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1  125  250\n";
      msg += "          chr1  400  2000\n\n";
      msg += "        There is no entry for chr21 since there is only 1 element and nothing 'in between' it and anything else\n";
      msg += "          (see the optional -L option that goes with -c below if this behavior is not what you want).\n\n";

      msg += "        There is one unfortunate -c side effect in that it is impossible to use a 0-based system and specify\n";
      msg += "          any BED inputs that will produce base 0 on the output, by default (for any chromosome).  For example:\n";
      msg += "            chr1  1    4\n";
      msg += "            chr1  100  110\n";
      msg += "            chr2  50   75\n";
      msg += "          will produce:\n";
      msg += "            chr1  4    100\n\n";

      msg += "          Negative coordinates are not supported so there is no way to get 'to the left of base 0' for complementing.\n\n";

      msg += "        Use of a 0-based, 1-based, or any N-based system (N >=0) is a user's choice, and bedops will support any\n";
      msg += "          of these without flaw, with the sole exception of the above. To address this 1 issue, the optional -L\n";
      msg += "          argument may be given with -c, so that:\n";
      msg += "            bedops -c -L file1.bed file2.bed\n";
      msg += "          will instead produce:\n";
      msg += "            chr1  0   1\n";
      msg += "            chr1  4   100\n";
      msg += "            chr2  0   50\n\n";

      msg += "        A user can specify any max coords (as part of inputs) to get --complement to give coordinates as far\n";
      msg += "          'to the right' as needed.  No hardcoded max genome/assembly size, per chromosome, is part of bedops.\n\n";

      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before complements are calculated.\n";
    } else if ( mode == DIFFERENCE ) {
      msg += "        Using the -d or --difference option requires at least 2 BED file inputs.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Reports the intervals found in the first file that are not present in the 2nd (or 3rd or 4th...) files.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1  10    125\n";
      msg += "          chr1  2000  2100\n";
      msg += "          chr1  2125  2500\n\n";
      msg += "        Notice the first row of output merges overlapping coordinates from file1 that do not appear in file2.\n";
      msg += "        Notice the 2nd and 3rd rows show coordinates split by file2 over chr1 2000 2500 of input file1.\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before differences are calculated.\n";
    } else if ( mode == ELEMENTOF ) {
      msg += "        Using the -e or --element-of option requires at least 2 BED file inputs.\n";
      msg += "        The output consists of all columns from qualifying rows of the first input file.\n";
      msg += "        -e produces exactly everything that -n does not, given the same overlap criterion.\n";
      msg += "        Reports the BED rows from file1 that overlap, by the specified percentage or number of base-pair, merged\n";
      msg += "          rows from file2, file3, etc.  By default, -100% is used as the overlap specification.\n";
      msg += "        The user may specify an overlap criterion by indicating a number of base-pair, or a percentage of the\n";
      msg += "          length of an input file row.\n";
      msg += "          For example, bedops -e -5 file1.bed file2.bed will produce an output row where the input row from file1.bed\n";
      msg += "          overlaps, by at least 5 base-pair, the merged rows from file2.bed.  Similarly,\n";
      msg += "              bedops -e -50% file1.bed file2.bed\n";
      msg += "          gives a row from file1.bed that is overlapped by at least 50% of its length by merged rows from file2.bed\n\n";
      msg += example1();
      msg += "        bedops -e -1 file1.bed file2.bed produces:\n";
      msg += "          chr1  2000  2500  id-3  54  +\n\n";
      msg += "        while bedops -e -75% file1.bed file2.bed produces nothing.\n";
      msg += "        The 3rd row is overlapped by 25 bp, but has length 500.\n";
      msg += "        --range L:R pads the start/end coordinates in file2.bed to fileN.bed by L/R, without padding elements of file1.bed.\n";
      msg += "          The output is a subset of file1.bed\n";
    } else if ( mode == INTERSECTION ) {
      msg += "        Using the -i or --intersect option requires at least 2 BED file inputs.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Reports the intervals common to all input files.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1  2100  2125\n\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before intersections are calculated.\n";
    } else if ( mode == MERGE ) {
      msg += "        Using the -m or --merge option requires at least 1 BED file input.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Merges together all overlapping and adjacent intervals from the input files.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1   10    125\n";
      msg += "          chr1   250   400\n";
      msg += "          chr1   2000  2500\n";
      msg += "          chr21  500   1000\n\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before merged regions are calculated.\n";
    } else if ( mode == NOTELEMENTOF ) {
      msg += "        Using the -n or --not-element-of option requires at least 2 BED file inputs.\n";
      msg += "        The output consists of all columns from qualifying rows of the first input file.\n";
      msg += "        -n produces exactly everything that -e does not, given the same overlap criterion.\n";
      msg += "        Reports the BED rows from file1 that do not overlap, by the specified percentage or number of base-pair, merged\n";
      msg += "          rows from file2, file3, etc.  By default, -100% is used as the overlap specification.\n";
      msg += "        The user may specify an overlap criterion by indicating a number of base-pair, or a percentage\n";
      msg += "          of the length of an input file row.\n";
      msg += "          For example, bedops -n -5 file1.bed file2.bed will produce an output row where the input row from file1.bed\n";
      msg += "          does not overlap, by at least 5 base-pair, the merged rows from file2.bed.  Similarly,\n";
      msg += "              bedops -n -50% file1.bed file2.bed\n";
      msg += "          gives a row from file1.bed that is not overlapped by at least 50% of its length by merged rows from file2.bed.\n\n";
      msg += example1();
      msg += "        bedops -n -1 file1.bed file2.bed produces:\n";
      msg += "            chr1  10  100  id-1  5\n";
      msg += "            chr1  50  125\n\n";
      msg += "        while bedops -n -75% file1.bed file2.bed produces the entirety of file1.bed.\n";
      msg += "        --range L:R pads the start/end coordinates in file2.bed to fileN.bed by L/R, without padding elements of file1.bed.\n";
      msg += "          The output is a subset of file1.bed\n";
    } else if ( mode == PARTITION ) {
      msg += "        Using the -p or --partition option requires at least 1 BED file input.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Breaks up inputs into disjoint (often adjacent) bed intervals.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1     10     50\n";
      msg += "          chr1     50    100\n";
      msg += "          chr1    100    125\n";
      msg += "          chr1    250    400\n";
      msg += "          chr1   2000   2100\n";
      msg += "          chr1   2100   2125\n";
      msg += "          chr1   2125   2500\n";
      msg += "          chr21   500   1000\n\n";
      msg += "        Notice that all coordinates in the original files are represented, and the total number of output rows is\n";
      msg += "          greater than the total number of input rows.  Notice that many rows have immediately adjacent coordinates.\n";
      msg += "        Note that if multiple input rows have identical coordinates, this option outputs only 1 representative.\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before partition takes place.\n";
    } else if ( mode == SYMMETRIC_DIFFERENCE ) {
      msg += "        Using the -s or --symmdiff option requires at least 2 BED file inputs.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Reports the intervals found in exactly 1 input file.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1   10    125\n";
      msg += "          chr1   250   400\n";
      msg += "          chr1   2000  2100\n";
      msg += "          chr1   2125  2500\n";
      msg += "          chr21  500   1000\n\n";
      msg += "        Notice the first row of output merges overlapping coordinates from file1 that do not appear in file2.\n";
      msg += "        Notice the 3rd and 4th rows show coordinates split by file2 over chr1 2000 2500 of input file1.\n";
      msg += "        Unlike -d, -s shows the last row of file2.bed since this interval only appears in one input file.\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before mutually exclusive regions are calculated.\n";
    } else if ( mode == UNIONALL ) {
      msg += "        Using the -u or --everything option requires at least 1 BED file input.\n";
      msg += "        The output consists of all columns from all rows of all input files.\n";
      msg += "        If multiple rows are identical, the output will consist of all of them.\n";
      msg += example1();
      msg += "        bedops -u file1.bed file2.bed produces:\n";
      msg += "            chr1   10    100  id-1 5\n";
      msg += "            chr1   50    125\n";
      msg += "            chr1   250   400\n";
      msg += "            chr1   2000  2500 id-3 54 +\n";
      msg += "            chr1   2100  2125\n";
      msg += "            chr21  500   1000\n\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R.\n";
      msg += "          This option is particularly useful for padding without the need to resort the results before further bedops calls.\n";
    } else {
      throw(Ext::ProgramError("Detailed MODE not supported"));
    }

    msg += "\n";
    msg += "        Every input file must be sorted per the sort-bed utility.\n";
    msg += "        The output is also sorted and ready for further calls to bedops.\n";
    msg += "        Coordinates are assumed to be of the form: [start,end), where 'end' is 1 bp beyond the end of the interval.\n";
    msg += "        Columns must be separated by tabs (not spaces).\n";
    msg += "        Using --ec will perform stringent error checking of all input files.  Use this if you're having troubles.\n";
    return(msg);
  }


  //=========
  // Usage()
  //=========
  std::string Usage() {
    std::string msg = "\n";
    msg += "      USAGE: bedops [process-flags] <operation> <File(s)>*\n\n";
    msg += "          Every input file must be sorted per the sort-bed utility.\n";
    msg += "          Each operation requires a minimum number of files as shown below.\n";
    msg += "            There is no fixed maximum number of files that may be used.\n";
    msg += "          Input files must have at least the first 3 columns of the BED specification.\n";
    msg += "          The program accepts BED and Starch file formats.\n";
    msg += "          May use '-' for a file to indicate reading from standard input (BED format only).\n";
    msg += "\n";
    msg += "      Process Flags:\n";
    msg += "          --chrom <chromosome> Jump to and process data for given <chromosome> only.\n";
    msg += "          --ec                 Error check input files (slower).\n";
    msg += "          --header             Accept headers (VCF, GFF, SAM, BED, WIG) in any input file.\n";
    msg += "          --help               Print this message and exit successfully.\n";
    msg += "          --help-<operation>   Detailed help on <operation>.\n";
    msg += "                                 An example is --help-c or --help-complement\n";
    msg += "          --range L:R          Add 'L' bp to all start coordinates and 'R' bp to end\n";
    msg += "                                 coordinates. Either value may be + or - to grow or\n";
    msg += "                                 shrink regions.  With the -e/-n operations, the first\n";
    msg += "                                 (reference) file is not padded, unlike all other files.\n";
    msg += "          --range S            Pad input file(s) coordinates symmetrically by S.\n";
    msg += "                                 This is shorthand for: --range -S:S.\n";
    msg += "          --version            Print program information.\n\n";

    msg += "      Operations: (choose one of)\n";
    msg += "          -c, --complement [-L] File1 [File]*\n";
    msg += "          -d, --difference ReferenceFile File2 [File]*\n";
    msg += "          -e, --element-of [-number% | -number] ReferenceFile File2 [File]*\n";
    msg += "                 by default, -e -100% is used.  'bedops -e -1' is also popular.\n";
    msg += "          -i, --intersect File1 File2 [File]*\n";
    msg += "          -m, --merge File1 [File]*\n";
    msg += "          -n, --not-element-of [-number% | -number] ReferenceFile File2 [File]*\n";
    msg += "                 by default, -n -100% is used.  'bedops -n -1' is also popular.\n";
    msg += "          -p, --partition File1 [File]*\n";
    msg += "          -s, --symmdiff File1 File2 [File]*\n";
    msg += "          -u, --everything File1 [File]*\n\n";

    msg += "      Example: bedops --range 10 -u file1.bed\n";
    msg += "      NOTE: Only operations -e|n|u preserve all columns (no flattening)\n";
    return(msg);
  }

} // namespace BedOperations


#endif // INPUT_BEDOPS_H
