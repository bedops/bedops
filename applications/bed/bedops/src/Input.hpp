/*
  Author:  Shane Neph & Scott Kuehn
  Date:    Fri Aug 13 15:00:25 PDT 2010
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

#ifndef INPUT_BEDOPS_H
#define INPUT_BEDOPS_H

#include <fstream>
#include <iostream>
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
    UNIONALL, ELEMENTOF, NOTELEMENTOF, PARTITION, CHOP };

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
  Input(int argc, char** argv) : ft_(MERGE), numFiles_(0), minFiles_(1000), allFiles_(),
                                 subsetPerc_(1), useSubsetPerc_(true), chopBP_(1),
                                 chopStaggerBP_(0), chopCutShort_(false), errorCheck_(false),
                                 lpad_(0), rpad_(0), leftMost_(0), chrSpecific_(false),
                                 chr_("all") {

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
            const std::string ints = "1234567890";
            int start = argcntr + 1;
            for ( int i = start; i < argc; ++i ) {
              std::string::size_type sz = std::string(argv[i]).size();
              if ( (argv[i][0] == '-' && sz > 1) // sz of 1 means stdin; legacy usage
                     ||
                   (std::string(argv[i]).find_first_not_of(ints) == std::string::npos)
                     ||
                   (std::string(argv[i]).find("%") != std::string::npos) ) {
                if ( std::string(argv[i]).find("--") == std::string::npos ) {
                  std::ifstream tmpfile(argv[i]);
                  if ( !tmpfile ) { // even if an integer, it could name a file input
                    setSubsetOption(argv[i]);
                    ++argcntr;
                    break;
                  } else {
                    bool isint = (std::string(argv[i]).find_first_not_of(plusints) == std::string::npos);
                    if ( isint ) {
                      std::cerr << "Warning: interpreting argument '"
                                << argv[i]
                                << "' as a file input and not as an overlap spec," << std::endl;
                      std::cerr << "         since the file exists." << std::endl;
                      std::cerr << "You can use the legacy syntax '-"
                                << argv[i]
                                << "' if you want to use it as an overlap criterion." << std::endl;
                    }
                    break;
                  }
                } else {
                  break;
                }
              } else {
                break;
              }
            } // for
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
          } else if ( ft_ == CHOP ) {
            typedef Ext::UserError UE;
            const std::string ints = "1234567890";
            int maxCount = 4, cntr = 0;
            int start = argcntr + 1;
            bool optionValueSet = false;
            bool auxOptionsSet = false;
            bool staggerSet = false;

            for ( int i = start; i < argc; ++i ) {
              if ( std::string(argv[i]) == "--stagger" ) {
                Ext::Assert<UE>(!staggerSet, "chop's --stagger suboption specified multiple times.");
                Ext::Assert<UE>(++i != argc, "No #nt value found for --stagger suboption in --chop");
                ++argcntr; // extra for required bp value that goes with --stagger
                Ext::Assert<UE>(std::string(argv[i]).find_first_not_of(ints) == std::string::npos,
                                "Invalid --stagger suboption #nt value in --chop.  Expect a +integer.");
                std::stringstream s; s << std::string(argv[i]); s >> chopStaggerBP_;
                Ext::Assert<UE>(chopStaggerBP_ > 0, "nt setting for chop's --stagger suboption must be > 0");
                staggerSet = true;
                auxOptionsSet = true;
              } else if ( std::string(argv[i]) == "-x" ) {
                Ext::Assert<UE>(!chopCutShort_, "chop's -x suboption specified multiple times.");
                chopCutShort_ = true;
                auxOptionsSet = true;
              } else if ( std::string(argv[i]).find_first_not_of(ints) == std::string::npos ) {
                Ext::Assert<UE>(!optionValueSet, "Stray integer found (invalid argument for --chop?)");
                Ext::Assert<UE>(!auxOptionsSet, "Stray integer value found: not valid for --chop");
                std::stringstream s; s << std::string(argv[i]); s >> chopBP_;
                Ext::Assert<UE>(chopBP_ > 0, "bp setting for chop must be > 0");
                optionValueSet = true;
              }
              else
                break;
              ++argcntr;
              ++cntr;
            } // for
            std::string msg = "Too many arguments for a --chop operation";
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
  int ChopChunkSize() const {
    return(chopBP_);
  }
  int ChopStaggerSize() const {
    return(chopStaggerBP_);
  }
  int ChopExcludeShort() const {
    return(chopCutShort_);
  }
  std::string Chrom() const {
    return(chr_);
  }
  bool ChrSpecific() const {
    return(chrSpecific_);
  }
  bool ComplementFullLeft() const {
    return(leftMost_);
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
    return(subsetPerc_);
  }
  bool UsePercentage() const {
    return(useSubsetPerc_);
  }

  void setSubsetOption(const std::string& str) {
    typedef Ext::UserError UE;
    const std::string nums = ".1234567890";
    const std::string ints = "1234567890";
    std::string l = str.substr(1); // allow '-' out front for legacy reasons
    std::string::size_type pos = str.find("%");
    if ( pos != std::string::npos ) {
      Ext::Assert<UE>(pos + 1 == str.size(), "Bad placement of %");
      std::string value = str.substr(0, pos); // get rid of %
      Ext::Assert<UE>(!value.empty(), "Bad % value");
      if ( value[0] == '-' ) { // support legacy -75% syntax; drop '-'
        value = value.substr(1);
        Ext::Assert<UE>(!value.empty(), "Bad % value");
      }
      Ext::Assert<UE>(value.find_first_not_of(nums) == std::string::npos,
                      "Bad: % value");
      std::stringstream conv(value);
      conv >> subsetPerc_;
      subsetPerc_ /= 100.0;
      if ( subsetPerc_ > 1 )
        throw(Ext::UserError("Expect percentage less than or equal to 100%"));
      useSubsetPerc_ = true;
      if ( subsetPerc_ == 0 ) { // 0% can match *everything*: convert to 1bp
        subsetPerc_ = 1;
        useSubsetPerc_ = false;
      }
    } else if ( str.find_first_not_of(ints) == std::string::npos ) {
      subsetPerc_ = atoi(str.c_str());
      useSubsetPerc_ = false; // pretend subsetPerc_ is # of bp
    } else if ( l.find_first_not_of(ints) == std::string::npos ) {
      // legacy
      subsetPerc_ = atoi(l.c_str());
      useSubsetPerc_ = false; // pretend subsetPerc_ is # of bp
    } else if ( str.find_first_not_of(nums) == std::string::npos ) {
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
      case 'w': case 'W':
        ft = CHOP; break;
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
    options_.insert(std::make_pair("--chop", "-w"));
  }

private:
  ModeType ft_;
  int numFiles_;
  int minFiles_;
  std::vector<std::string> allFiles_;
  double subsetPerc_;
  bool useSubsetPerc_;
  int chopBP_;
  int chopStaggerBP_;
  bool chopCutShort_;
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
    if ( mode == CHOP ) {
      msg += "        Using the -w or --chop operation requires at least 1 BED file input.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Produces windowed slices from the merged regions of all input files.\n";
      msg += "        The integer given with --chop is referred to as the chunk-size.  By default, the chunk-size is 1.\n";
      msg += "        The chunk-size must be greater than zero.\n";
      msg += "        Using --chop 100 with inputs\n\n";

      msg += example1();

      msg += "        Produces:\n";
      msg += "          chr1   10    110\n";
      msg += "          chr1   110   125\n";
      msg += "          chr1   250   350\n";
      msg += "          chr1   350   400\n";
      msg += "          chr1   2000  2100\n";
      msg += "          chr1   2100  2200\n";
      msg += "          chr1   2200  2300\n";
      msg += "          chr1   2300  2400\n";
      msg += "          chr1   2400  2500\n";
      msg += "          chr21  500   600\n";
      msg += "          chr21  600   700\n";
      msg += "          chr21  700   800\n";
      msg += "          chr21  800   900\n";
      msg += "          chr21  900   1000\n\n";

      msg += "        Additional options --stagger <nt> and -x may be used to modify --chop's behavior.\n";
      msg += "        The -x option simply excludes from output all rows that were shortened to less than chunk-size bps.\n";
      msg += "        The above output results would be modified with the -x option as follows:\n\n";

      msg += "          chr1   10    110\n";
      msg += "          chr1   250   350\n";
      msg += "          chr1   2000  2100\n";
      msg += "          chr1   2100  2200\n";
      msg += "          chr1   2200  2300\n";
      msg += "          chr1   2300  2400\n";
      msg += "          chr1   2400  2500\n";
      msg += "          chr21  500   600\n";
      msg += "          chr21  600   700\n";
      msg += "          chr21  700   800\n";
      msg += "          chr21  800   900\n";
      msg += "          chr21  900   1000\n\n";

      msg += "        The --stagger <nt> option specifies the number of nt to 'jump' in between output rows, where <nt>.\n";
      msg += "          must be greater than 0 and it must be specified along with --stagger.  Starting from the previous output\n";
      msg += "          start coordinate, it adds this number of nt to the next start coordinate output, and the end coordinate\n";
      msg += "          is adjusted accordingly.\n";
      msg += "        The --stagger <nt> specification is sometimes referred to as the step-size.\n";
      msg += "        For example, using --chop 100 --stagger 53, produces:\n\n";

      msg += "          chr1   10    110\n";
      msg += "          chr1   63    125\n";
      msg += "          chr1   116   125\n";
      msg += "          chr1   250   350\n";
      msg += "          chr1   303   400\n";
      msg += "          chr1   356   400\n";
      msg += "          chr1   2000  2100\n";
      msg += "          chr1   2053  2153\n";
      msg += "          chr1   2106  2206\n";
      msg += "          chr1   2159  2259\n";
      msg += "          chr1   2212  2312\n";
      msg += "          chr1   2265  2365\n";
      msg += "          chr1   2318  2418\n";
      msg += "          chr1   2371  2471\n";
      msg += "          chr1   2424  2500\n";
      msg += "          chr1   2477  2500\n";
      msg += "          chr21  500   600\n";
      msg += "          chr21  553   653\n";
      msg += "          chr21  606   706\n";
      msg += "          chr21  659   759\n";
      msg += "          chr21  712   812\n";
      msg += "          chr21  765   865\n";
      msg += "          chr21  818   918\n";
      msg += "          chr21  871   971\n";
      msg += "          chr21  924   1000\n";
      msg += "          chr21  977   1000\n\n";

      msg += "        Notice the differences between start coordinates differ by 53.\n";
      msg += "        The -x and --stagger <nt> options may be combined.\n";
      msg += "        For example, using --chop 100 --stagger 53 -x, produces:\n\n";

      msg += "          chr1   10    110\n";
      msg += "          chr1   250   350\n";
      msg += "          chr1   2000  2100\n";
      msg += "          chr1   2053  2153\n";
      msg += "          chr1   2106  2206\n";
      msg += "          chr1   2159  2259\n";
      msg += "          chr1   2212  2312\n";
      msg += "          chr1   2265  2365\n";
      msg += "          chr1   2318  2418\n";
      msg += "          chr1   2371  2471\n";
      msg += "          chr21  500   600\n";
      msg += "          chr21  553   653\n";
      msg += "          chr21  606   706\n";
      msg += "          chr21  659   759\n";
      msg += "          chr21  712   812\n";
      msg += "          chr21  765   865\n";
      msg += "          chr21  818   918\n";
      msg += "          chr21  871   971\n\n";

      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before chopped regions are calculated.\n";
    } else if ( mode == COMPLEMENT ) {
      msg += "        Using the -c or --complement operation requires at least 1 BED file input.\n";
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
      msg += "        Using the -d or --difference operation requires at least 2 BED file inputs.\n";
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
      msg += "        Using the -e or --element-of operation requires at least 2 BED file inputs.\n";
      msg += "        The output consists of all columns from qualifying rows of the first input file.\n";
      msg += "        -e produces exactly everything that -n does not, given the same overlap criterion.\n";
      msg += "        Reports the BED rows from file1 that overlap, by the specified percentage or number of base-pair, merged\n";
      msg += "          rows from file2, file3, etc.  By default, 100% is used as the overlap specification.\n";
      msg += "        The user may specify an overlap criterion by indicating a number of base-pair, or a percentage of the\n";
      msg += "          length of an input element.\n";
      msg += "          For example,\n";
      msg += "              bedops -e 5 file1.bed file2.bed\n";
      msg += "          will echo an input row on output where the row from file1.bed overlaps, by at least 5bp, the merged\n";
      msg += "          coordinates from file2.bed.  Similarly,\n";
      msg += "              bedops -e 50% file1.bed file2.bed\n";
      msg += "          gives a row from file1.bed that is overlapped by at least 50% of its length by merged rows in file2.bed\n\n";
      msg += example1();
      msg += "        bedops -e 1 file1.bed file2.bed produces:\n";
      msg += "          chr1  2000  2500  id-3  54  +\n\n";
      msg += "        while bedops -e 75% file1.bed file2.bed produces nothing.\n";
      msg += "        The 3rd row is overlapped by 25 bp, but has length 500.\n";
      msg += "        --range L:R pads the start/end coordinates in file2.bed to fileN.bed by L/R, without padding elements of file1.bed.\n";
      msg += "          The output is a subset of file1.bed\n";
    } else if ( mode == INTERSECTION ) {
      msg += "        Using the -i or --intersect operation requires at least 2 BED file inputs.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Reports the intervals common to all input files.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1  2100  2125\n\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before intersections are calculated.\n";
    } else if ( mode == MERGE ) {
      msg += "        Using the -m or --merge operation requires at least 1 BED file input.\n";
      msg += "        The output consists of the first 3 columns of the BED specification.\n";
      msg += "        Merges together (flattens) all disjoint, overlapping, and adjoining intervals from all input files into\n";
      msg += "          contiguous, disjoint regions.\n\n";
      msg += example1();
      msg += "        Output:\n";
      msg += "          chr1   10    125\n";
      msg += "          chr1   250   400\n";
      msg += "          chr1   2000  2500\n";
      msg += "          chr21  500   1000\n\n";
      msg += "        --range L:R pads the start/end coordinates in all input files by L/R before merged regions are calculated.\n";
    } else if ( mode == NOTELEMENTOF ) {
      msg += "        Using the -n or --not-element-of operation requires at least 2 BED file inputs.\n";
      msg += "        The output consists of all columns from qualifying rows of the first input file.\n";
      msg += "        -n produces exactly everything that -e does not, given the same overlap criterion.\n";
      msg += "        Reports the BED rows from file1 that do not overlap, by the specified percentage or number of base-pair, merged\n";
      msg += "          rows from file2, file3, etc.  By default, -100% is used as the overlap specification.\n";
      msg += "        The user may specify an overlap criterion by indicating a number of base-pair, or a percentage\n";
      msg += "          of the length of an input file row.\n";
      msg += "          For example,\n";
      msg += "              bedops -n 5 file1.bed file2.bed\n";
      msg += "          will echo an input row on output where the row from file1.bed does not overlap, by at least 5 bp, the merged\n";
      msg += "          rows from file2.bed.  Similarly,\n";
      msg += "              bedops -n 50% file1.bed file2.bed\n";
      msg += "          gives a row from file1.bed that is not overlapped, by at least 50% of its length, by merged rows from file2.bed.\n\n";
      msg += example1();
      msg += "        bedops -n 1 file1.bed file2.bed produces:\n";
      msg += "            chr1  10  100  id-1  5\n";
      msg += "            chr1  50  125\n\n";
      msg += "        while bedops -n 75% file1.bed file2.bed produces the entirety of file1.bed.\n";
      msg += "        --range L:R pads the start/end coordinates in file2.bed to fileN.bed by L/R, without padding elements of file1.bed.\n";
      msg += "          The output is a subset of file1.bed\n";
    } else if ( mode == PARTITION ) {
      msg += "        Using the -p or --partition operation requires at least 1 BED file input.\n";
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
      msg += "        Using the -s or --symmdiff operation requires at least 2 BED file inputs.\n";
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
      msg += "        Using the -u or --everything operation requires at least 1 BED file input.\n";
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
    msg += "          --range S            Pad or shrink input file(s) coordinates symmetrically by S.\n";
    msg += "                                 This is shorthand for: --range -S:S.\n";
    msg += "          --version            Print program information.\n\n";

    msg += "      Operations: (choose one of)\n";
    msg += "          -c, --complement [-L] File1 [File]*\n";
    msg += "          -d, --difference ReferenceFile File2 [File]*\n";
    msg += "          -e, --element-of [bp | percentage] ReferenceFile File2 [File]*\n";
    msg += "                 by default, -e 100% is used.  'bedops -e 1' is also popular.\n";
    msg += "          -i, --intersect File1 File2 [File]*\n";
    msg += "          -m, --merge File1 [File]*\n";
    msg += "          -n, --not-element-of [bp | percentage] ReferenceFile File2 [File]*\n";
    msg += "                 by default, -n 100% is used.  'bedops -n 1' is also popular.\n";
    msg += "          -p, --partition File1 [File]*\n";
    msg += "          -s, --symmdiff File1 File2 [File]*\n";
    msg += "          -u, --everything File1 [File]*\n";
    msg += "          -w, --chop [bp] [--stagger <nt>] [-x] File1 [File]*\n";
    msg += "                 by default, -w 1 is used with no staggering.\n";
    msg += "      \nExample: bedops --range 10 -u file1.bed\n";
    msg += "      NOTE: Only operations -e|n|u preserve all columns (no flattening)\n";
    return(msg);
  }

} // namespace BedOperations


#endif // INPUT_BEDOPS_H
