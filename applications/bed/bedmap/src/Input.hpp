/*
  Author: Scott Kuehn, Shane Neph
  Date:   Fri Oct 19 08:20:50 PDT 2007
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

#ifndef _BEDMAP_INPUT_HPP
#define _BEDMAP_INPUT_HPP

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "data/bed/BedCompare.hpp"
#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/helpers/NamedVisitors.hpp"
#include "utility/Assertion.hpp"
#include "utility/Typify.hpp"

#include "TDefs.hpp"

namespace BedMap {

  struct NoInput { /* */ };

  namespace details {
    struct dummyBase {
      typedef int RefType;
      typedef int MapType;
    };


    template <typename T>
    std::string name() {
      return(Visitors::Helpers::VisitorName<T>::Name());
    }
  } // details



  //=============
  // Input<T,U>:
  //=============
  template <typename ArgError, typename HelpType, typename VersionType>
  class Input {

    typedef VisitorTypes<details::dummyBase> VT;

  public:

    // Constructor
    Input(int argc, char **argv)
      : refFileName_(""), mapFileName_(""), rangeBP_(0), overlapBP_(0),
        percOvr_(0.0), isPercMap_(false), isPercRef_(false), isPercEither_(false),
        isPercBoth_(false), isRangeBP_(false), isOverlapBP_(false), isExact_(false),
        precision_(6), useScientific_(false), useMinMemory_(false), setPrec_(false), numFiles_(0),
        minRefFields_(0), minMapFields_(0), errorCheck_(false), sweepAll_(false),
        outDelim_("|"), multiDelim_(";"), fastMode_(false), rangeAlias_(false),
        chrom_("all"), skipUnmappedRows_(false), unmappedVal_("") {

      // Process user's operation options
      if ( argc <= 1 )
        throw(NoInput()); // prints usage statement and returns EXIT_FAILURE
      const std::string posIntegers = "0123456789";
      const std::string integers = "-" + posIntegers;
      const std::string reals = "." + integers;
      int argcntr = 1;
      bool hasVisitor = false;
      while ( argcntr < argc ) {
        std::string next = argv[argcntr++];
        if ( next.find("--") == std::string::npos && argc - argcntr < 2 ) // process file inputs
          break;

        Ext::Assert<ArgError>(next.find("--") == 0, "Option " + next + " does not start with '--'");
        next = next.substr(2);

        if ( next == "help" ) {
          throw(HelpType()); // prints usage statement and returns EXIT_SUCCESS
        } else if ( next == "version" ) {
          throw(VersionType()); // prints version and returns EXIT_SUCCESS
        } else if ( next == "ec" || next == "header" ) {
          // bed_check_iterator<> allows silly headers
          errorCheck_ = true;
        } else if ( next == "faster" ) {
          fastMode_ = true;
        } else if ( next == "sweep-all" ) { // --> sweep through all of second file
          sweepAll_ = true;
        } else if ( next == "unmapped-val" ) {
          Ext::Assert<ArgError>(unmappedVal_.empty(), "--unmapped-val specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No value given for --unmapped-val");
          unmappedVal_ = argv[argcntr++];
          Ext::Assert<ArgError>(unmappedVal_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where <val> expected for --unmapped-val.");
        } else if ( next == "delim" ) {
          Ext::Assert<ArgError>(outDelim_ == "|", "--delim specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No output delimiter given");
          outDelim_ = argv[argcntr++];
          Ext::Assert<ArgError>(outDelim_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where output delimiter expected.");
        } else if ( next == "chrom" ) {
          Ext::Assert<ArgError>(chrom_ == "all", "--chrom specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No chromosome name given");
          chrom_ = argv[argcntr++];
          Ext::Assert<ArgError>(chrom_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where chromosome expected.");
        } else if ( next == "multidelim" ) {
          Ext::Assert<ArgError>(multiDelim_ == ";", "--multidelim specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No multi-value column delimmiter given");
          multiDelim_ = argv[argcntr++];
          Ext::Assert<ArgError>(multiDelim_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where output delimiter expected.");
        } else if ( next == "skip-unmapped" ) {
          skipUnmappedRows_ = true;
        } else if ( next == "sci" ) {
          useScientific_ = true;
        } else if ( next == "min-memory" ) {
          useMinMemory_ = true;
        } else if ( next == "prec" ) {
          Ext::Assert<ArgError>(argcntr < argc, "No precision value given");
          Ext::Assert<ArgError>(!setPrec_, "--prec specified multiple times.");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(posIntegers) == std::string::npos,
                                "Non-positive-integer argument: " + sval + " for --prec");
          std::stringstream conv(sval);
          conv >> precision_;
          Ext::Assert<ArgError>(precision_ >= 0, "--prec value must be >= 0");
          setPrec_ = true;
        } else if ( next == "bp-ovr" ) {
          // first check that !rangeAlias_ before !isOverlapBP_
          Ext::Assert<ArgError>(!rangeAlias_, "--range and --bp-ovr detected.  Choose one.");
          Ext::Assert<ArgError>(!isOverlapBP_, "multiple --bp-ovr's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --bp-ovr");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(posIntegers) == std::string::npos,
                                "Non-positive-integer argument: " + sval + " for --bp-ovr");
          std::stringstream conv(sval);
          conv >> overlapBP_;
          Ext::Assert<ArgError>(overlapBP_ > 0, "--bp-ovr value must be > 0");
          isOverlapBP_ = true;
        } else if ( next == "range" ) {
          Ext::Assert<ArgError>(!isRangeBP_ && !rangeAlias_, "multiple --range's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --range");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(posIntegers) == std::string::npos,
                                "Non-positive-integer argument: " + sval + " for --range");
          std::stringstream conv(sval);
          conv >> rangeBP_;
          Ext::Assert<ArgError>(rangeBP_ >= 0, "--range value must be >= 0");
          isRangeBP_ = true;
          if ( 0 == rangeBP_ ) { // alias for --bp-ovr 1
            Ext::Assert<ArgError>(!rangeAlias_, "--range 0 specified multiple times.");
            Ext::Assert<ArgError>(!isOverlapBP_, "--bp-ovr and --range detected.  Choose one.");
            isRangeBP_ = false;
            isOverlapBP_ = true;
            rangeAlias_ = true;
            overlapBP_ = 1;
          }
        } else if ( next == "fraction-ref" ) {
          Ext::Assert<ArgError>(!isPercRef_, "multiple --fraction-ref's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-ref");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-ref");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-ref value must be: >0-1.0");
          isPercRef_ = true;
        } else if ( next == "fraction-map" ) {
          Ext::Assert<ArgError>(!isPercMap_, "multiple --fraction-map's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-map");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-map");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-map value must be: >0-1.0");
          isPercMap_ = true;
        } else if ( next == "fraction-either" ) {
          Ext::Assert<ArgError>(!isPercEither_, "multiple --fraction-either's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-either");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-either");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-either value must be: >0-1.0");
          isPercEither_ = true;
        } else if ( next == "fraction-both" ) {
          Ext::Assert<ArgError>(!isPercBoth_, "multiple --fraction-both's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-both");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-both");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-both value must be: >0-1.0");
          isPercBoth_ = true;
        } else if ( next == "exact" ) { // same as --fraction-both 1
          Ext::Assert<ArgError>(!isExact_, "multiple --exact's detected - use one");
          isExact_ = true;
        }
        else if ( next == details::name<typename VT::OvrAgg>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::OvrAgg>());
        else if ( next == details::name<typename VT::OvrUniq>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::OvrUniq>());
        else if ( next == details::name<typename VT::OvrUniqFract>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::OvrUniqFract>());
        else if ( next == details::name<typename VT::EchoRefAll>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoRefAll>());
        else if ( next == details::name<typename VT::EchoRefLength>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoRefLength>());
        else if ( next == details::name<typename VT::EchoRefSpan>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoRefSpan>());
        else if ( next == details::name<typename VT::EchoRefRowNumber>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoRefRowNumber>());
        else if ( next == details::name<typename VT::EchoMapAll>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapAll>());
        else if ( next == details::name<typename VT::EchoMapID>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapID>());
        else if ( next == details::name<typename VT::EchoMapUniqueID>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapUniqueID>());
        else if ( next == details::name<typename VT::EchoMapLength>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapLength>());
        else if ( next == details::name<typename VT::EchoMapIntersectLength>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapIntersectLength>());
        else if ( next == details::name<typename VT::EchoMapRange>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapRange>());
        else if ( next == details::name<typename VT::EchoMapScore>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::EchoMapScore>());
        else if ( next == details::name<typename VT::Count>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Count>());
        else if ( next == details::name<typename VT::Indicator>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Indicator>());
        else if ( next == details::name<typename VT::Max>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Max>());
        else if ( next == details::name<typename VT::MaxElementRand>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::MaxElementRand>());
        else if ( next == details::name<typename VT::MaxElementStable>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::MaxElementStable>());
        else if ( next == details::name<typename VT::Min>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Min>());
        else if ( next == details::name<typename VT::MinElementRand>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::MinElementRand>());
        else if ( next == details::name<typename VT::MinElementStable>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::MinElementStable>());
        else if ( next == details::name<typename VT::Average>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Average>());
        else if ( next == details::name<typename VT::Variance>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Variance>());
        else if ( next == details::name<typename VT::StdDev>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::StdDev>());
        else if ( next == details::name<typename VT::CoeffVariation>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::CoeffVariation>());
        else if ( next == details::name<typename VT::Sum>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Sum>());
        else if ( next == details::name<typename VT::WMean>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::WMean>());
        else if ( next == details::name<typename VT::Median>() )
          hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::Median>());
        else if ( next == details::name<typename VT::MedianAbsoluteDeviation>() ) {
          std::string sval = argv[argcntr];
          if ( sval.find_first_not_of(reals) == std::string::npos ) { // assume argument for this option
            ++argcntr;
            std::stringstream conv(sval);
            double val = -1;
            conv >> val;
            Ext::Assert<ArgError>(val > 0, "--" + details::name<typename VT::MedianAbsoluteDeviation>() + " Expect 0 < val");
            std::vector<std::string> tmpVec;
            tmpVec.push_back(sval);
            hasVisitor = addVisitor(Ext::Type2Type<typename VT::MedianAbsoluteDeviation>(), tmpVec);
          }
          else // use default multiplier for MAD
            hasVisitor = addNoArgVisitor(Ext::Type2Type<typename VT::MedianAbsoluteDeviation>());
        }
        else if ( next == details::name<typename VT::KthAverage>() ) {
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --" + details::name<typename VT::KthAverage>());
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --" + details::name<typename VT::KthAverage>());
          std::stringstream conv(sval);
          double val = -1;
          conv >> val;
          Ext::Assert<ArgError>(val >= 0 && val <= 1, "--" + details::name<typename VT::KthAverage>() + " Expect 0 <= val <= 1");
          std::vector<std::string> tmpVec;
          tmpVec.push_back(sval);
          hasVisitor = addVisitor(Ext::Type2Type<typename VT::KthAverage>(), tmpVec);
        }
        else if ( next == details::name<typename VT::TMean>() ) {
          Ext::Assert<ArgError>(argcntr < argc, "No <low> arg given for --" + details::name<typename VT::TMean>());
          std::string svalLow = argv[argcntr++];
          Ext::Assert<ArgError>(svalLow.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + svalLow + " for --" + details::name<typename VT::TMean>());

          Ext::Assert<ArgError>(argcntr < argc, "No <hi> arg given for --" + details::name<typename VT::TMean>());
          std::string svalHigh = argv[argcntr++];
          Ext::Assert<ArgError>(svalHigh.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + svalHigh + " for --" + details::name<typename VT::TMean>());

          std::stringstream convLow(svalLow), convHigh(svalHigh);
          double valLow = 100, valHigh = 100;
          convLow >> valLow; convHigh >> valHigh;
          Ext::Assert<ArgError>(valLow >= 0 && valLow <= 1,
                                "--" + details::name<typename VT::TMean>() + " Expect 0 <= low < hi <= 1");
          Ext::Assert<ArgError>(valHigh >= 0 && valHigh <= 1,
                                "--" + details::name<typename VT::TMean>() + " Expect 0 <= low < hi <= 1");
          Ext::Assert<ArgError>(valLow + valHigh <= 1,
                                "--" + details::name<typename VT::TMean>() + " Expect (low + hi) <= 1.");
          std::vector<std::string> tmpVec;
          tmpVec.push_back(svalLow); tmpVec.push_back(svalHigh);
          hasVisitor = addVisitor(Ext::Type2Type<typename VT::TMean>(), tmpVec);
        }
        else
          throw(ArgError("Unknown option: --" + next));
      } // while

      if ( !(isPercMap_ || isPercRef_ || isPercEither_ || isPercBoth_ || isRangeBP_ || isOverlapBP_ || isExact_) ) {
        // use defaults
        isOverlapBP_ = true;
        overlapBP_ = 1;
      }
      int count = isPercMap_;
      count += isPercRef_;
      count += isPercEither_;
      count += isPercBoth_;
      count += isRangeBP_;
      count += isOverlapBP_;
      count += isExact_;
      Ext::Assert<ArgError>(1 == count, "More than one overlap specification used.");

      Ext::Assert<ArgError>(hasVisitor, "No processing option specified (ie; --max).");
      Ext::Assert<ArgError>(0 <= argc - argcntr, "No files");
      Ext::Assert<ArgError>(3 == minRefFields_, "Program error: Input.hpp::minRefFields_");
      Ext::Assert<ArgError>(3 <= minMapFields_ && 5 >= minMapFields_, "Program error: Input.hpp::minMapFields_");
      Ext::Assert<ArgError>(!fastMode_ || isOverlapBP_ || isRangeBP_ || isPercBoth_ || isExact_, "--faster compatible with --range, --bp-ovr, --fraction-both, and --exact only");

      // Process files inputs
      Ext::Assert<ArgError>(2 >= argc - argcntr, "Need [one or] two input files");
      Ext::Assert<ArgError>(0 <= argc - argcntr, "Need [one or] two input files");
      numFiles_ = argc - argcntr + 1;
      Ext::Assert<ArgError>(1 <= numFiles_ && numFiles_ <= 2, "Need [one or] two input files");
      if ( 2 == numFiles_ ) {
        refFileName_ = argv[argc-2];
        mapFileName_ = argv[argc-1];
      } else { // single-file mode
        refFileName_ = argv[argc-1];
        mapFileName_ = "";
        minRefFields_ = minMapFields_;
        minMapFields_ = 0;
      }
      Ext::Assert<ArgError>(refFileName_ != "-" || mapFileName_ != "-",
                            "Cannot have stdin set for two files");
    }


  public:
    std::string refFileName_;
    std::string mapFileName_;
    std::vector<std::string> visitorNames_;
    std::vector< std::vector<std::string> > visitorArgs_;
    long rangeBP_;
    long overlapBP_;
    double percOvr_;
    bool isPercMap_;
    bool isPercRef_;
    bool isPercEither_;
    bool isPercBoth_;
    bool isRangeBP_;
    bool isOverlapBP_;
    bool isExact_;
    int precision_;
    bool useScientific_;
    bool useMinMemory_;
    bool setPrec_;
    unsigned int numFiles_;
    unsigned int minRefFields_;
    unsigned int minMapFields_;
    bool errorCheck_;
    bool sweepAll_;
    std::string outDelim_;
    std::string multiDelim_;
    bool fastMode_;
    bool rangeAlias_;
    std::string chrom_;
    bool skipUnmappedRows_;
    std::string unmappedVal_;

  private:
    struct MapFields {
      template <typename T> static unsigned int num(Ext::Type2Type<T>) { return 5; }
      static unsigned int num(Ext::Type2Type<typename VT::Count>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoMapAll>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoMapID>) { return 4; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoMapIntersectLength>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoMapLength>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoMapRange>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoMapUniqueID>) { return 4; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoRefAll>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoRefLength>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoRefSpan>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::EchoRefRowNumber>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::Indicator>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::OvrAgg>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::OvrUniq>) { return 3; }
      static unsigned int num(Ext::Type2Type<typename VT::OvrUniqFract>) { return 3; }
    };

    struct RefFields {
      template <typename T> static unsigned int num(Ext::Type2Type<T>) { return 3; }
    };

    template <typename T>
    bool addNoArgVisitor(Ext::Type2Type<T> t) {
      visitorNames_.push_back(details::name<T>());
      visitorArgs_.push_back(std::vector<std::string>());
      minMapFields_ = std::max(minMapFields_, static_cast<unsigned int>(MapFields::num(t)));
      minRefFields_ = std::max(minRefFields_, static_cast<unsigned int>(RefFields::num(t)));
      return(true);
    }

    template <typename T>
    bool addVisitor(Ext::Type2Type<T> t, const std::vector<std::string>& args) {
      visitorNames_.push_back(details::name<T>());
      visitorArgs_.push_back(args);
      minMapFields_ = std::max(minMapFields_, static_cast<unsigned int>(MapFields::num(t)));
      minRefFields_ = std::max(minRefFields_, static_cast<unsigned int>(RefFields::num(t)));
      return(true);
    }
  };



  //---------
  // Usage()
  std::string Usage() {

    typedef BedMap::VisitorTypes<details::dummyBase> VT;

    std::stringstream usage;
    usage << "                                                                                                    \n";
    usage << " USAGE: bedmap [process-flags] [overlap-option] <operation(s)...> <ref-file> [map-file]             \n";
    usage << "     Any input file must be sorted per the sort-bed utility.                                        \n";
    usage << "     The program accepts BED and Starch file formats.                                               \n";
    usage << "     You may use '-' for a BED file to indicate the input comes from stdin.                         \n";
    usage << "                                                                                                    \n";
    usage << "     Traverse <ref-file>, while applying <operation(s)> on qualified, overlapping elements from     \n";
    usage << "       <map-file>.  Output is one line for each line in <ref-file>, sent to standard output.  There \n";
    usage << "       is no limit on the number of operations you can specify to compute in one bedmap call.       \n";
    usage << "     If <map-file> is omitted, the given file is treated as both the <ref-file> and <map-file>.     \n";
    usage << "       This usage is more efficient than specifying the same file twice.                            \n";
    usage << "     Arguments may be given in any order before the input file(s).                                  \n";
    usage << "                                                                                                    \n";
    usage << "    Process Flags:                                                                                  \n";
    usage << "     --------                                                                                       \n";
    usage << "      --chrom <chromosome>  Jump to and process data for given <chromosome> only.                   \n";
    usage << "      --delim <delim>       Change output delimiter from '|' to <delim> between columns (e.g. \'\\t\').\n";
    usage << "      --ec                  Error check all input files (slower).                                   \n";
    usage << "      --faster              (advanced) Strong input assumptions are made.  Compatible with:         \n";
    usage << "                              --bp-ovr, --range, --fraction-both, and --exact overlap options only. \n";
    usage << "      --header              Accept headers (VCF, GFF, SAM, BED, WIG) in any input file.             \n";
    usage << "      --help                Print this message and exit successfully.                               \n";
    usage << "      --min-memory          Minimize memory usage (slower).                                         \n";
    usage << "      --multidelim <delim>  Change delimiter of multi-value output columns from ';' to <delim>.     \n";
    usage << "      --prec <int>          Change the post-decimal precision of scores to <int>.  0 <= <int>.      \n";
    usage << "      --sci                 Use scientific notation for score outputs.                              \n";
    usage << "      --skip-unmapped       Print no output for a row with no mapped elements.                      \n";
    usage << "      --sweep-all           Ensure <map-file> is read completely (helps to prevent broken pipes).   \n";
    usage << "      --unmapped-val <val>  Print <val> on unmapped --echo-map* and --min/max-element* operations.  \n";
    usage << "                              The default is to print nothing.                                      \n";
    usage << "      --version             Print program information.                                              \n";
    usage << "                                                                                                    \n";
    usage << "                                                                                                    \n";
    usage << "    Overlap Options (At most, one may be selected.  By default, --bp-ovr 1 is used):                \n";
    usage << "     --------                                                                                       \n";
    usage << "      --bp-ovr <int>           Require <int> bp overlap between elements of input files.            \n";
    usage << "      --exact                  First 3 fields from <map-file> must be identical to <ref-file>'s.    \n";
    usage << "      --fraction-both <val>    Both --fraction-ref <val> and --fraction-map <val> must be true to   \n";
    usage << "                                 qualify as overlapping.  Expect 0 < val <= 1.                      \n";
    usage << "      --fraction-either <val>  Either --fraction-ref <val> or --fraction-map <val> must be true to  \n";
    usage << "                                 qualify as overlapping.  Expect 0 < val <= 1.                      \n";
    usage << "      --fraction-map <val>     The fraction of the element's size from <map-file> that must overlap \n";
    usage << "                                 the element in <ref-file>.  Expect 0 < val <= 1.                   \n";
    usage << "      --fraction-ref <val>     The fraction of the element's size from <ref-file> that must overlap \n";
    usage << "                                 an element in <map-file>.  Expect 0 < val <= 1.                    \n";
    usage << "      --range <int>            Grab <map-file> elements within <int> bp of <ref-file>'s element,    \n";
    usage << "                                 where 0 <= int.  --range 0 is an alias for --bp-ovr 1.             \n";
    usage << "                                                                                                    \n";
    usage << "                                                                                                    \n";
    usage << "    Operations:  (Any number of operations may be used any number of times.)                        \n";
    usage << "     ----------                                                                                     \n";
    usage << "      SCORE:                                                                                        \n";
    usage << "       <ref-file> must have at least 3 columns and <map-file> 5 columns.                            \n";
    usage << "                                                                                                    \n";
    usage << "      --" + details::name<VT::CoeffVariation>() + "                The result of --" + details::name<VT::StdDev>() + " divided by the result of --" + details::name<VT::Average>() + ".\n";
    usage << "      --" + details::name<VT::KthAverage>() + " <val>         Generalized median. Report the value, x, such that the fraction <val>\n";
    usage << "                            of overlapping elements' scores from <map-file> is less than x,\n";
    usage << "                            and the fraction 1-<val> of scores is greater than x.  0 < val <= 1.\n";
    usage << "      --" + details::name<VT::MedianAbsoluteDeviation>() + " <mult=1>      The median absolute deviation of overlapping elements in <map-file>.\n";
    usage << "                            Multiply mad score by <mult>.  0 < mult, and mult is 1 by default.\n";
    usage << "      --" + details::name<VT::Max>() + "               The highest score from overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::MaxElementStable>() + "       A (non-random) highest-scoring and overlapping element in <map-file>.\n";
    usage << "      --" + details::name<VT::MaxElementRand>() + "  A random highest-scoring and overlapping element in <map-file>.\n";
    usage << "      --" + details::name<VT::Average>() + "              The average score from overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::Median>() + "            The median score from overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::Min>() + "               The lowest score from overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::MinElementStable>() + "       A (non-random) lowest-scoring and overlapping element in <map-file>.\n";
    usage << "      --" + details::name<VT::MinElementRand>() + "  A random lowest-scoring and overlapping element in <map-file>.\n";
    usage << "      --" + details::name<VT::StdDev>() + "             The square root of the result of --" + details::name<VT::Variance>() + ".\n";
    usage << "      --" + details::name<VT::Sum>() + "               Accumulated scores from overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::TMean>() + " <low> <hi>  The mean score from overlapping elements in <map-file>, after\n";
    usage << "                            ignoring the bottom <low> and top <hi> fractions of those scores.\n";
    usage << "                            0 <= low <= 1.  0 <= hi <= 1.  low+hi <= 1.\n";
    usage << "      --" + details::name<VT::Variance>() + "          The variance of scores from overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::WMean>() + "             Weighted mean, scaled in proportion to the coverage of the <ref-file>\n";
    usage << "                            element by each overlapping <map-file> element.\n";
    usage << "     \n";
    usage << "     ----------\n";
    usage << "      NON-SCORE:\n";
    usage << "       <ref-file> must have at least 3 columns.\n";
    usage << "       For --" + details::name<VT::EchoMapID>() + "/" + details::name<VT::EchoMapUniqueID>() + ", <map-file> must have at least 4 columns.\n";
    usage << "       For --" + details::name<VT::EchoMapScore>() + ", <map-file> must have at least 5 columns.\n";
    usage << "       For all others, <map-file> requires at least 3 columns.\n\n";
    usage << "      --" + details::name<VT::OvrAgg>() + "             The total number of overlapping bases from <map-file>.\n";
    usage << "      --" + details::name<VT::OvrUniq>() + "        The number of distinct bases from <ref-file>'s element covered by\n";
    usage << "                            overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::OvrUniqFract>() + "      The fraction of distinct bases from <ref-file>'s element covered by\n";
    usage << "                            overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::Count>() + "             The number of overlapping elements in <map-file>.\n";
    usage << "      --" + details::name<VT::EchoRefAll>() + "              Print each line from <ref-file>.\n";
    usage << "      --" + details::name<VT::EchoMapAll>() + "          List all overlapping elements from <map-file>.\n";
    usage << "      --" + details::name<VT::EchoMapID>() +  "       List IDs from all overlapping <map-file> elements.\n";
    usage << "      --" + details::name<VT::EchoMapUniqueID>() +  "  List unique IDs from overlapping <map-file> elements.\n";
    usage << "      --" + details::name<VT::EchoMapRange>() + "    Print genomic range of overlapping elements from <map-file>.\n";
    usage << "      --" + details::name<VT::EchoMapScore>() + "    List scores from overlapping <map-file> elements.\n";
    usage << "      --" + details::name<VT::EchoMapLength>() + "     List the full length of every overlapping element.\n";
    usage << "      --" + details::name<VT::EchoMapIntersectLength>() + " List lengths of overlaps.\n";
    usage << "      --" + details::name<VT::EchoRefSpan>() + "     Print the first 3 fields of <ref-file> using chrom:start-end format.\n";
    usage << "      --" + details::name<VT::EchoRefRowNumber>() + "   Print 'id-' followed by the line number of <ref-file>.\n";
    usage << "      --" + details::name<VT::EchoRefLength>() + "     Print the length of each line from <ref-file>.\n";
    usage << "      --" + details::name<VT::Indicator>() + "         Print 1 if there exists an overlapping element in <map-file>, 0 otherwise.\n";
    usage << "\n";

    return(usage.str());
  }

} // namespace BedMap

#endif // _BEDMAP_INPUT_HPP
