/*
  Author: Scott Kuehn, Shane Neph
  Date:   Fri Oct 19 08:20:50 PDT 2007
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

#ifndef _BEDMAP_INPUT_HPP
#define _BEDMAP_INPUT_HPP

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"

namespace BedMap {

  struct NoInput { /* */ };

  enum class OpName {
    Bases, BasesUniq, BasesUniqFract, Count, CoeffVar, Echo, EchoMap,
    EchoMapRange, EchoMapScore, EchoMapSize, EchoMapText, EchoMapTextUniq,
    EchoOverlapSize, EchoRefSize, EchoRefName, Indicator, KthAvg, KthValueAt,
    Mad, Max, MaxElement, Mean, Median, Min, MinElement, Stdev, Sum, TMean,
    Variance
  };

  enum class OpNameAlias {
    EchoMapID, EchoMapIDUniq 
 };

  std::string OperationName(OpName nm) {
    std::string name;
    switch(nm) {
      case OpName::Bases:           name = "bases";              break;
      case OpName::BasesUniq:       name = "bases-uniq";         break;
      case OpName::BasesUniqFract:  name = "bases-uniq-f";       break;
      case OpName::Count:           name = "count";              break;
      case OpName::CoeffVar:        name = "cv";                 break;
      case OpName::Echo:            name = "echo";               break;
      case OpName::EchoMap:         name = "echo-map";           break;
      case OpName::EchoMapRange:    name = "echo-map-range";     break;
      case OpName::EchoMapScore:    name = "echo-map-score";     break;
      case OpName::EchoMapSize:     name = "echo-map-size";      break;
      case OpName::EchoMapText:     name = "echo-map-text";      break;
      case OpName::EchoMapTextUniq: name = "echo-map-text-uniq"; break;
      case OpName::EchoOverlapSize: name = "echo-overlap-size";  break;
      case OpName::EchoRefSize:     name = "echo-ref-size";      break;
      case OpName::EchoRefName:     name = "echo-ref-name";      break;
      case OpName::Indicator:       name = "indicator";          break;
      case OpName::KthAvg:          name = "kth";                break;
      case OpName::KthValueAt:      name = "kth-val";            break;
      case OpName::Mad:             name = "mad";                break;
      case OpName::Max:             name = "max";                break;
      case OpName::MaxElement:      name = "max-element";        break;
      case OpName::Mean:            name = "mean";               break;
      case OpName::Median:          name = "median";             break;
      case OpName::Min:             name = "min";                break;
      case OpName::MinElement:      name = "min-element";        break;
      case OpName::Stdev:           name = "stdev";              break;
      case OpName::Sum:             name = "sum";                break;
      case OpName::TMean:           name = "tmean";              break;
      case OpName::Variance:        name = "variance";           break;
      default:
        throw(Ext::ProgramError("Unmatched enum"));
    };
    return name;
  }

  std::string OperationName(OpNameAlias nm) {
    std::string name;
    switch(nm) {
      case OpNameAlias::EchoMapID:     name = "echo-map-id";      break;
      case OpNameAlias::EchoMapIDUniq: name = "echo-map-id-uniq"; break;
      default:
        throw(Ext::ProgramError("Unmatched enum alias"));
    };
    return name;
  }

  //=============
  // Input<T,U>:
  //=============
  template <typename ArgError, typename HelpType, typename VersionType>
  struct Input {
    // Constructor
    Input(int argc, char **argv)
      : refFileName_(""), mapFileName_(""), rangeBP_(0), overlapBP_(0),
        percOvr_(0.0), isPercMap_(false), isPercRef_(false), isPercEither_(false),
        isPercBoth_(false), isRangeBP_(false), isOverlapBP_(false), isExact_(false),
        precision_(6), useScientific_(false), setPrec_(false), numFiles_(0),
        minRefFields_(0), minMapFields_(0), errorCheck_(false), sweepAll_(false),
        outDelim_("|"), multiDelim_(";"), fastMode_(false), rangeAlias_(false),
        chrom_("all"), skipUnmappedRows_(false), useRest_(false) {

      // Process user's operation options
      if ( argc <= 1 )
        throw(NoInput()); // prints usage statement and returns EXIT_FAILURE
      const std::string posIntegers = "0123456789";
      const std::string integers = "-" + posIntegers;
      const std::string reals = "." + integers;
      int argcntr = 1;
      bool hasVisitor = false;
      while ( argcntr < argc ) {
        std::string nextOption = argv[argcntr++];
        int columnSelect = -1;
        if ( nextOption.find("--") == std::string::npos && argc - argcntr < 2 ) // process file inputs
          break;

        Ext::Assert<ArgError>(nextOption.find("--") == 0, "Option " + nextOption + " does not start with '--'");
        nextOption = nextOption.substr(2);

        if ( nextOption.find("=") != std::string::npos ) {
          auto column = nextOption.substr(nextOption.find("=")+1);
          Ext::Assert<ArgError>(column.find_first_not_of(posIntegers) == std::string::npos,
                                "Bad column number with option: --" + nextOption);
          std::stringstream ss; ss << column;
          ss >> columnSelect;
          Ext::Assert<ArgError>(columnSelect > 3, "Column selection must be > 3 for --" + nextOption);
          nextOption = nextOption.substr(0, nextOption.find("="));
        }

        if ( nextOption == "help" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          throw(HelpType()); // prints usage statement and returns EXIT_SUCCESS
        } else if ( nextOption == "version" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          throw(VersionType()); // prints version and returns EXIT_SUCCESS
        } else if ( nextOption == "ec" || nextOption == "header" ) {
          // bed_check_iterator<> allows silly headers
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          errorCheck_ = true;
        } else if ( nextOption == "faster" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          fastMode_ = true;
        } else if ( nextOption == "sweep-all" ) { // --> sweep through all of second file
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          sweepAll_ = true;
        } else if ( nextOption == "delim" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(outDelim_ == "|", "--delim specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No output delimiter given");
          outDelim_ = argv[argcntr++];
          Ext::Assert<ArgError>(outDelim_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where output delimiter expected.");
        } else if ( nextOption == "chrom" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(chrom_ == "all", "--chrom specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No chromosome name given");
          chrom_ = argv[argcntr++];
          Ext::Assert<ArgError>(chrom_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where chromosome expected.");
        } else if ( nextOption == "multidelim" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(multiDelim_ == ";", "--multidelim specified multiple times");
          Ext::Assert<ArgError>(argcntr < argc, "No multi-value column delimmiter given");
          multiDelim_ = argv[argcntr++];
          Ext::Assert<ArgError>(multiDelim_.find("--") != 0,
                                "Apparent option: " + std::string(argv[argcntr]) + " where output delimiter expected.");
        } else if ( nextOption == "skip-unmapped" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          skipUnmappedRows_ = true;
        } else if ( nextOption == "sci" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          useScientific_ = true;
        } else if ( nextOption == "prec" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(argcntr < argc, "No precision value given");
          Ext::Assert<ArgError>(!setPrec_, "--prec specified multiple times.");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(posIntegers) == std::string::npos,
                                "Non-positive-integer argument: " + sval + " for --prec");
          std::stringstream conv(sval);
          conv >> precision_;
          Ext::Assert<ArgError>(precision_ >= 0, "--prec value must be >= 0");
          setPrec_ = true;
        } else if ( nextOption == "bp-ovr" ) {
          // first check that !rangeAlias_ before !isOverlapBP_
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
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
        } else if ( nextOption == "range" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
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
        } else if ( nextOption == "fraction-ref" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(!isPercRef_, "multiple --fraction-ref's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-ref");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-ref");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-ref value must be: >0-1.0");
          isPercRef_ = true;
        } else if ( nextOption == "fraction-map" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(!isPercMap_, "multiple --fraction-map's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-map");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-map");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-map value must be: >0-1.0");
          isPercMap_ = true;
        } else if ( nextOption == "fraction-either" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(!isPercEither_, "multiple --fraction-either's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-either");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-either");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-either value must be: >0-1.0");
          isPercEither_ = true;
        } else if ( nextOption == "fraction-both" ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(!isPercBoth_, "multiple --fraction-both's detected");
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --fraction-both");
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --fraction-both");
          std::stringstream conv(sval);
          conv >> percOvr_;
          Ext::Assert<ArgError>(percOvr_ > 0 && percOvr_ <= 1, "--fraction-both value must be: >0-1.0");
          isPercBoth_ = true;
        } else if ( nextOption == "exact" ) { // same as --fraction-both 1
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          Ext::Assert<ArgError>(!isExact_, "multiple --exact's detected - use one");
          isExact_ = true;
        } else if ( nextOption == OperationName(OpName::Bases) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Bases, columnSelect);
        } else if ( nextOption == OperationName(OpName::BasesUniq) ) {
          hasVisitor = addColumnSelectVisitor(OpName::BasesUniq, columnSelect);
        } else if ( nextOption == OperationName(OpName::BasesUniqFract) ) {
          hasVisitor = addColumnSelectVisitor(OpName::BasesUniqFract, columnSelect);
        } else if ( nextOption == OperationName(OpName::Echo) ) {
          hasVisitor = addNoArgVisitor(OpName::Echo);
        } else if ( nextOption == OperationName(OpName::EchoRefSize) ) {
          hasVisitor = addNoArgVisitor(OpName::EchoRefSize);
        } else if ( nextOption == OperationName(OpName::EchoRefName) ) {
          hasVisitor = addNoArgVisitor(OpName::EchoRefName);
        } else if ( nextOption == OperationName(OpName::EchoMap) ) {
          hasVisitor = addNoArgVisitor(OpName::EchoMap);
        } else if ( nextOption == OperationName(OpName::EchoMapText) ) {
          hasVisitor = addColumnSelectVisitor(OpName::EchoMapText, columnSelect < 0 ? 4 : columnSelect);
        } else if ( nextOption == OperationName(OpNameAlias::EchoMapID) ) {
          hasVisitor = addColumnSelectVisitor(OpName::EchoMapText, 4);
        } else if ( nextOption == OperationName(OpName::EchoMapTextUniq) ) {
          hasVisitor = addColumnSelectVisitor(OpName::EchoMapTextUniq, columnSelect < 0 ? 4 : columnSelect);
        } else if ( nextOption == OperationName(OpNameAlias::EchoMapIDUniq) ) {
          hasVisitor = addColumnSelectVisitor(OpName::EchoMapTextUniq, 4);
        } else if ( nextOption == OperationName(OpName::EchoMapSize) ) {
          hasVisitor = addNoArgVisitor(OpName::EchoMapSize);
        } else if ( nextOption == OperationName(OpName::EchoOverlapSize) ) {
          hasVisitor = addNoArgVisitor(OpName::EchoOverlapSize);
        } else if ( nextOption == OperationName(OpName::EchoMapRange) ) {
          hasVisitor = addNoArgVisitor(OpName::EchoMapRange);
        } else if ( nextOption == OperationName(OpName::EchoMapScore) ) {
          hasVisitor = addColumnSelectVisitor(OpName::EchoMapScore, columnSelect);
        } else if ( nextOption == OperationName(OpName::Count) ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          hasVisitor = addNoArgVisitor(OpName::Count);
        } else if ( nextOption == OperationName(OpName::Indicator) ) {
          Ext::Assert<ArgError>(columnSelect < 0, "No column number allowed for option: --" + nextOption);
          hasVisitor = addNoArgVisitor(OpName::Indicator);
        } else if ( nextOption == OperationName(OpName::Max) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Max, columnSelect);
        } else if ( nextOption == OperationName(OpName::MaxElement) ) {
          hasVisitor = addColumnSelectVisitor(OpName::MaxElement, columnSelect);
        } else if ( nextOption == OperationName(OpName::Min) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Min, columnSelect);
        } else if ( nextOption == OperationName(OpName::MinElement) ) {
          hasVisitor = addColumnSelectVisitor(OpName::MinElement, columnSelect);
        } else if ( nextOption == OperationName(OpName::Mean) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Mean, columnSelect);
        } else if ( nextOption == OperationName(OpName::Variance) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Variance, columnSelect);
        } else if ( nextOption == OperationName(OpName::Stdev) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Stdev, columnSelect);
        } else if ( nextOption == OperationName(OpName::CoeffVar) ) {
          hasVisitor = addColumnSelectVisitor(OpName::CoeffVar, columnSelect);
        } else if ( nextOption == OperationName(OpName::Sum) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Sum, columnSelect);
        } else if ( nextOption == OperationName(OpName::Median) ) {
          hasVisitor = addColumnSelectVisitor(OpName::Median, columnSelect);
        } else if ( nextOption == OperationName(OpName::Mad) ) {
          std::string sval = argv[argcntr];
          if ( sval.find_first_not_of(reals) == std::string::npos ) { // assume argument for this option
            ++argcntr;
            std::stringstream conv(sval);
            double val = -1;
            conv >> val;
            Ext::Assert<ArgError>(val > 0, "--" + OperationName(OpName::Mad) + " Expect 0 < val");
            std::vector<std::string> tmpVec;
            tmpVec.push_back(sval);
            hasVisitor = addColumnSelectVisitor(OpName::Mad, columnSelect, tmpVec);
          }
          else // use default multiplier for MAD
            hasVisitor = addColumnSelectVisitor(OpName::Mad, columnSelect);
        } else if ( nextOption == OperationName(OpName::KthAvg) ) {
          Ext::Assert<ArgError>(argcntr < argc, "No arg for --" + OperationName(OpName::KthAvg));
          std::string sval = argv[argcntr++];
          Ext::Assert<ArgError>(sval.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + sval + " for --" + OperationName(OpName::KthAvg));
          std::stringstream conv(sval);
          double val = -1;
          conv >> val;
          Ext::Assert<ArgError>(val >= 0 && val <= 1, "--" + OperationName(OpName::KthAvg) + " Expect 0 <= val <= 1");
          std::vector<std::string> tmpVec;
          tmpVec.push_back(sval);
          hasVisitor = addColumnSelectVisitor(OpName::KthAvg, columnSelect, tmpVec);
        } else if ( nextOption == OperationName(OpName::TMean) ) {
          Ext::Assert<ArgError>(argcntr < argc, "No <low> arg given for --" + OperationName(OpName::TMean));
          std::string svalLow = argv[argcntr++];
          Ext::Assert<ArgError>(svalLow.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + svalLow + " for --" + OperationName(OpName::TMean));

          Ext::Assert<ArgError>(argcntr < argc, "No <hi> arg given for --" + OperationName(OpName::TMean));
          std::string svalHigh = argv[argcntr++];
          Ext::Assert<ArgError>(svalHigh.find_first_not_of(reals) == std::string::npos,
                                "Non-numeric argument: " + svalHigh + " for --" + OperationName(OpName::TMean));

          std::stringstream convLow(svalLow), convHigh(svalHigh);
          double valLow = 100, valHigh = 100;
          convLow >> valLow; convHigh >> valHigh;
          Ext::Assert<ArgError>(valLow >= 0 && valLow <= 1,
                                "--" + OperationName(OpName::TMean) + " Expect 0 <= low < hi <= 1");
          Ext::Assert<ArgError>(valHigh >= 0 && valHigh <= 1,
                                "--" + OperationName(OpName::TMean) + " Expect 0 <= low < hi <= 1");
          Ext::Assert<ArgError>(valLow + valHigh <= 1,
                                "--" + OperationName(OpName::TMean) + " Expect (low + hi) <= 1.");
          std::vector<std::string> tmpVec;
          tmpVec.push_back(svalLow); tmpVec.push_back(svalHigh);
          hasVisitor = addColumnSelectVisitor(OpName::TMean, columnSelect, tmpVec);
        } else {
          throw(ArgError("Unknown option: --" + nextOption));
        }
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
      Ext::Assert<ArgError>(3 <= minMapFields_, "Program error: Input.hpp::minMapFields_");
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
    std::vector<OpName> visitorNames_;
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
    bool useRest_;

  private:
    bool addNoArgVisitor(OpName op) {
      visitorNames_.push_back(op);
      visitorArgs_.push_back(std::vector<std::string>());
      minRefFields_ = std::max(minRefFields_, 3u);
      minMapFields_ = std::max(minMapFields_, 3u);
      useRest_ = useRest_ || op == OpName::Echo || op == OpName::EchoMap;
      return true;
    }

    bool addColumnSelectVisitor(OpName op, int column, std::vector<std::string> args = std::vector<std::string>()) {
      visitorNames_.push_back(op);
      std::stringstream ss;
      if ( column > 0 )
        ss << column;
      else // default for measurements
        ss << 5;
	  std::reverse(args.begin(), args.end());
      args.push_back(ss.str());
	  std::reverse(args.begin(), args.end());
      visitorArgs_.push_back(args);
      minRefFields_ = std::max(minRefFields_, 3u);
      minMapFields_ = std::max(minMapFields_, static_cast<unsigned int>(column > 0 ? column : 5u));
      useRest_ = true;
      return true;
    }
  };

  //---------
  // Usage()
  std::string Usage() {
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
    usage << "      --multidelim <delim>  Change delimiter of multi-value output columns from ';' to <delim>.     \n";
    usage << "      --prec <int>          Change the post-decimal precision of scores to <int>.  0 <= <int>.      \n";
    usage << "      --sci                 Use scientific notation for score outputs.                              \n";
    usage << "      --skip-unmapped       Print no output for a row with no mapped elements.                      \n";
    usage << "      --sweep-all           Ensure <map-file> is read completely (helps to prevent broken pipes).   \n";
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
    usage << "      --" + OperationName(OpName::CoeffVar) + "                The result of --" + OperationName(OpName::Stdev) + " divided by the result of --" + OperationName(OpName::Mean) + ".\n";
    usage << "      --" + OperationName(OpName::KthAvg) + " <val>         Generalized median. Report the value, x, such that the fraction <val>\n";
    usage << "                            of overlapping elements' scores from <map-file> is less than x,\n";
    usage << "                            and the fraction 1-<val> of scores is greater than x.  0 < val <= 1.\n";
    usage << "      --" + OperationName(OpName::Mad) + " <mult=1>      The median absolute deviation of overlapping elements in <map-file>.\n";
    usage << "                            Multiply mad score by <mult>.  0 < mult, and mult is 1 by default.\n";
    usage << "      --" + OperationName(OpName::Max) + "               The highest score from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::MaxElement) + "       An element with the highest score from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::Mean) + "              The average score from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::Median) + "            The median score from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::Min) + "               The lowest score from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::MinElement) + "       An element with the lowest score from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::Stdev) + "             The square root of the result of --" + OperationName(OpName::Variance) + ".\n";
    usage << "      --" + OperationName(OpName::Sum) + "               Accumulated scores from overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::TMean) + " <low> <hi>  The mean score from overlapping elements in <map-file>, after\n";
    usage << "                            ignoring the bottom <low> and top <hi> fractions of those scores.\n";
    usage << "                            0 <= low <= 1.  0 <= hi <= 1.  low+hi <= 1.\n";
    usage << "      --" + OperationName(OpName::Variance) + "          The variance of scores from overlapping elements in <map-file>.\n";
    usage << "     \n";
    usage << "     ----------\n";
    usage << "      NON-SCORE:\n";
    usage << "       <ref-file> must have at least 3 columns.\n";
    usage << "       For --" + OperationName(OpNameAlias::EchoMapID) + "/" + OperationName(OpNameAlias::EchoMapIDUniq) + ", <map-file> must have at least 4 columns.\n";
    usage << "       For --" + OperationName(OpName::EchoMapScore) + ", <map-file> must have at least 5 columns.\n";
    usage << "       For all others, <map-file> requires at least 3 columns.\n\n";
    usage << "      --" + OperationName(OpName::Bases) + "              The total number of overlapping bases from <map-file>.\n";
    usage << "      --" + OperationName(OpName::BasesUniq) + "         The number of distinct bases from <ref-file>'s element covered by\n";
    usage << "                             overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::BasesUniqFract) + "       The fraction of distinct bases from <ref-file>'s element covered by\n";
    usage << "                             overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::Count) + "              The number of overlapping elements in <map-file>.\n";
    usage << "      --" + OperationName(OpName::Echo) + "               Print each line from <ref-file>.\n";
    usage << "      --" + OperationName(OpName::EchoMap) + "           List all overlapping elements from <map-file>.\n";
    usage << "      --" + OperationName(OpNameAlias::EchoMapID) +  "        List IDs from all overlapping <map-file> elements.\n";
    usage << "      --" + OperationName(OpNameAlias::EchoMapIDUniq) +  "   List unique IDs from overlapping <map-file> elements.\n";
    usage << "      --" + OperationName(OpName::EchoMapRange) + "     Print genomic range of overlapping elements from <map-file>.\n";
    usage << "      --" + OperationName(OpName::EchoMapScore) + "     List scores from overlapping <map-file> elements.\n";
    usage << "      --" + OperationName(OpName::EchoMapSize) + "      List the full length of every overlapping element.\n";
    usage << "      --" + OperationName(OpName::EchoMapText) +  "      List column text from all overlapping <map-file> elements.\n";
    usage << "      --" + OperationName(OpName::EchoMapTextUniq) +  " List unique column text from overlapping <map-file> elements.\n";
    usage << "      --" + OperationName(OpName::EchoOverlapSize) + "  List lengths of overlaps.\n";
    usage << "      --" + OperationName(OpName::EchoRefName) + "      Print the first 3 fields of <ref-file> using chrom:start-end format.\n";
    usage << "      --" + OperationName(OpName::EchoRefSize) + "      Print the length of each line from <ref-file>.\n";
    usage << "      --" + OperationName(OpName::Indicator) + "          Print 1 if there exists an overlapping element in <map-file>, 0 otherwise.\n";
    usage << "\n";

    return usage.str();
  }

} // namespace BedMap

#endif // _BEDMAP_INPUT_HPP
