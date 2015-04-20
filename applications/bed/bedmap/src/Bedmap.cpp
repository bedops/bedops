/*
  Author: Scott Kuehn, Shane Neph
  Date:   Wed Oct 17 14:23:29 PDT 2007
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

#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "algorithm/WindowSweep.hpp"
#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedDistances.hpp"
#include "data/bed/GeneralBed.hpp"
#include "suite/BEDOPS.Version.hpp"
#include "utility/Assertion.hpp"
#include "utility/ByLine.hpp"
#include "utility/Exception.hpp"
#include "utility/FPWrap.hpp"
#include "utility/Typify.hpp"

#include "Input.hpp"
#include "TDefs.hpp"

namespace BedMap {

  const std::string prognm = "bedmap";
  const std::string version = BEDOPS::revision();
  const std::string authors = "Shane Neph & Scott Kuehn";
  const std::string citation = BEDOPS::citation();

  //======
  // Help
  //======
  struct Help { /* */ };
  struct Version { /* */ };

  //======================
  // Forward Declarations
  //======================
  template <typename SweepDistType, typename BedDistType, typename InputType>
  void selectSweep(const SweepDistType&,
                   const BedDistType&,
                   const InputType&);
} // namespace BedMap

//========
// main()
//========
int main(int argc, char **argv) {
  typedef BedMap::Input<Ext::UserError, BedMap::Help, BedMap::Version> InputType;
  try {
    InputType input(argc, argv);
    if ( input.isPercMap_ ) { // % overlap relative to MapType's size (signalmapish)
      Bed::PercentOverlapMapping bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input);
    } else if ( input.isPercRef_ ) { // % overlap relative to RefTypes's size (setops -e)
      Bed::PercentOverlapReference bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input);
    } else if ( input.isPercBoth_ ) { // % overlap relative to both MapType's and RefType's sizes
      Bed::PercentOverlapBoth bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input);
    } else if ( input.isExact_ ) { // must be identical coordinates
      Bed::Exact bedDist;
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input);
    } else if ( input.isPercEither_ ) { // % overlap relative to either MapType's or RefType's size
      Bed::PercentOverlapEither bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input);
    } else if ( input.isRangeBP_ ) { // buffer each reference element
      Bed::RangedDist bedDist(input.rangeBP_);
      Bed::RangedDist sweepDist(input.rangeBP_); // same as bedDist in this case
      BedMap::selectSweep(sweepDist, bedDist, input);
    } else { // require a certain amount of bp overlap
      Bed::Overlapping bedDist(input.overlapBP_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input);
    }
    return EXIT_SUCCESS;
  } catch(const BedMap::Help& h) { // show usage and exit success
    std::cout << BedMap::prognm << std::endl;
    std::cout << "  citation: " << BedMap::citation << std::endl;
    std::cout << "  version:  " << BedMap::version << std::endl;
    std::cout << "  authors:  " << BedMap::authors << std::endl;
    std::cout << BedMap::Usage() << std::endl;
    return EXIT_SUCCESS;
  } catch(const BedMap::Version& v) { // show version and exit success
    std::cout << BedMap::prognm << std::endl;
    std::cout << "  citation: " << BedMap::citation << std::endl;
    std::cout << "  version:  " << BedMap::version << std::endl;
    std::cout << "  authors:  " << BedMap::authors << std::endl;
  } catch(const BedMap::NoInput& ni) { // show usage and exit failure
    std::cerr << BedMap::prognm << std::endl;
    std::cerr << "  citation: " << BedMap::citation << std::endl;
    std::cerr << "  version:  " << BedMap::version << std::endl;
    std::cerr << "  authors:  " << BedMap::authors << std::endl;
    std::cerr << BedMap::Usage() << std::endl;
  } catch(std::string& s) {
    std::cerr << "May use bedmap --help for more help.\n" << std::endl;
    std::cerr << "Error: " << s << std::endl;
  } catch(const std::exception& stx) {
    std::cerr << "May use bedmap --help for more help.\n" << std::endl;
    std::cerr << "Error: " << stx.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown Error.  Aborting" << std::endl;
  }
  return EXIT_FAILURE;
}


namespace BedMap {

  //=============
  // runSweep1(): single-file mode
  //=============
  template <typename BaseClass, typename SweepDistType, typename BedDistType, typename InputType>
  void runSweep1(const SweepDistType& st,
                 const BedDistType& dt,
                 const InputType& input,
                 std::vector<BaseClass*>& visitorGroup,
                 bool doNestCheck) {
    typedef typename BaseClass::RefType RefType;
    typedef Visitors::Helpers::PrintDelim PrintType;

    // Set up visitors
    PrintType processFields(input.outDelim_);
    PrintType processRows("\n");
    typedef Visitors::MultiVisitor<PrintType, PrintType, BaseClass> MVType;
    MVType multiv(visitorGroup, dt, processFields, processRows, !input.skipUnmappedRows_);

    if ( !input.errorCheck_ ) { // faster iterators
      // Create file handle iterators
      Ext::FPWrap<Ext::InvalidFile> refFile(input.refFileName_);
      Bed::allocate_iterator_starch_bed<RefType*> refFileI(refFile, input.chrom_), refFileEnd;

      // Do work
      if ( !input.fastMode_ )
        WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
      else // no nested elements
        WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
    } else { // --ec
//      const bool nestCheck = doNestCheck; // check for fully-nested elements if --fast and --ec
//      bool isStdin = (input.refFileName_ == "-");
//      std::ifstream infile(input.refFileName_.c_str());
//      if ( !isStdin && !infile )
//          throw(Ext::UserError("Unable to find: " + input.refFileName_));
//      if ( isStdin ) {
//        Bed::bed_check_iterator<RefType*> refFileI(std::cin, input.refFileName_, input.chrom_, nestCheck);
//        Bed::bed_check_iterator<RefType*> refFileEnd;
//        if ( !input.fastMode_ )
//          WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
//        else // no nested elements
//          WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
//      } else {
//        Bed::bed_check_iterator<RefType*> refFileI(infile, input.refFileName_, input.chrom_, nestCheck);
//        Bed::bed_check_iterator<RefType*> refFileEnd;
//        if ( !input.fastMode_ )
//          WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
//        else // no nested elements
//          WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
//      }
    }
    // multiv does cleanup
  }

  //=============
  // runSweep2(): multi-file mode
  //=============
  template <typename BaseClass, typename SweepDistType, typename BedDistType, typename InputType>
  void runSweep2(const SweepDistType& st,
                 const BedDistType& dt,
                 const InputType& input,
                 std::vector<BaseClass*>& visitorGroup,
                 bool doNestCheck) {
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef Visitors::Helpers::PrintDelim PrintType;

    // Set up visitors
    PrintType processFields(input.outDelim_);
    PrintType processRows("\n");
    typedef Visitors::MultiVisitor<PrintType, PrintType, BaseClass> MVType;
    MVType multiv(visitorGroup, dt, processFields, processRows, !input.skipUnmappedRows_);

    if ( !input.errorCheck_ ) { // faster iterators
      // Create file handle iterators
      Ext::FPWrap<Ext::InvalidFile> refFile(input.refFileName_);
      Bed::allocate_iterator_starch_bed<RefType*> refFileI(refFile, input.chrom_), refFileEnd;
      Ext::FPWrap<Ext::InvalidFile> mapFile(input.mapFileName_);
      Bed::allocate_iterator_starch_bed<MapType*> mapFileI(mapFile, input.chrom_), mapFileEnd;

      // Do work
      if ( !input.fastMode_ )
        WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, input.sweepAll_);
      else // no nested elements
        WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, input.sweepAll_);
    } else { // --ec
//      typedef Ext::UserError EType;
//      const bool nestCheck = doNestCheck; // check for fully-nested elements if --fast and --ec
//      bool isStdinRef = (input.refFileName_ == "-");
//      bool isStdinMap = (input.mapFileName_ == "-");
//      if ( isStdinRef && isStdinMap )
//        throw(EType("Cannot have both input files set to '-'"));
//      std::ifstream rfin(input.refFileName_.c_str()), mfin(input.mapFileName_.c_str());
//      if ( !isStdinRef && !rfin )
//        throw(EType("Unable to find: " + input.refFileName_));
//      if ( !isStdinMap && !mfin )
//        throw(EType("Unable to find: " + input.mapFileName_));
//
//      // Do work
//      if ( isStdinRef ) {
//        Bed::bed_check_iterator<RefType*> refFileI(std::cin, input.refFileName_, input.chrom_, nestCheck), refFileEnd;
//        Bed::bed_check_iterator<MapType*> mapFileI(mfin, input.mapFileName_, input.chrom_, nestCheck), mapFileEnd;
//        if ( !input.fastMode_ )
//          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, input.sweepAll_);
//        else // no nested elements
//          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, input.sweepAll_);
//      } else {
//        Bed::bed_check_iterator<RefType*> refFileI(rfin, input.refFileName_, input.chrom_, nestCheck), refFileEnd;
//        if ( isStdinMap ) {
//          Bed::bed_check_iterator<MapType*> mapFileI(std::cin, input.mapFileName_, input.chrom_, nestCheck), mapFileEnd;
//          if ( !input.fastMode_ )
//            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, input.sweepAll_);
//          else // no nested elements
//            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, input.sweepAll_);
//        } else {
//          Bed::bed_check_iterator<MapType*> mapFileI(mfin, input.mapFileName_, input.chrom_, nestCheck), mapFileEnd;
//          if ( !input.fastMode_ )
//            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, input.sweepAll_);
//          else // no nested elements
//            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, input.sweepAll_);
//        }
//      }
    }
    // multiv does cleanup
  }

  //==================
  // GenerateVisitors
  //==================
  template <typename BaseVisitor>
  struct GenerateVisitors {

    typedef BaseVisitor BaseClass;

    BaseVisitor* generate(OpName op,
                          const std::vector<std::string>& args,
                          const std::string& multivalColSep,
                          int precision, bool useScientific) {

      typedef VisitorTypes<BaseVisitor> VTypes;
      typedef typename VTypes::Average::ProcessType MPT; // applicable to most numeric outputs
      MPT mpt(precision, useScientific);

      BaseVisitor* rtn = 0; // create instance of OpName
      if ( op == OpName::Count ) {
        rtn = new typename VTypes::Count;
      } else if ( op == OpName::EchoMap ) {
        typedef typename VTypes::EchoMapAll::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapAll(PT(PType(), multivalColSep));
      } else if ( op == OpName::EchoMapSize ) {
        rtn = new typename VTypes::EchoMapLength;
      } else if ( op == OpName::EchoOverlapSize ) {
        rtn = new typename VTypes::EchoMapIntersectLength;
      } else if ( op == OpName::EchoMapRange ) {
        rtn = new typename VTypes::EchoMapRange;
      } else if ( op == OpName::Echo ) {
        rtn = new typename VTypes::EchoRefAll;
      } else if ( op == OpName::EchoRefSize ) {
        rtn = new typename VTypes::EchoRefLength;
      } else if ( op == OpName::EchoRefName ) {
        rtn = new typename VTypes::EchoRefSpan;
      } else if ( op == OpName::Indicator ) {
        rtn = new typename VTypes::Indicator;
      } else if ( op == OpName::Bases ) {
        rtn = new typename VTypes::OvrAgg;
      } else if ( op == OpName::BasesUniq ) {
        rtn = new typename VTypes::OvrUniq;
      } else if ( op == OpName::BasesUniqFract ) {
        typedef typename VTypes::OvrUniqFract::ProcessType PT;
        PT pt(precision, useScientific);
        rtn = new typename VTypes::OvrUniqFract(pt);
      } else if ( op == OpName::EchoMapText ) {
        typedef typename VTypes::EchoMapText::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapText(PT(PType(args[0]), multivalColSep));
      } else if ( op == OpName::EchoMapTextUniq ) {
        typedef typename VTypes::EchoMapTextUnique::ProcessType PT;
        rtn = new typename VTypes::EchoMapTextUnique(PT(args[0], multivalColSep));
      } else if ( op == OpName::Mean ) {
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        rtn = new typename VTypes::Average(mpt, column);
      } else if ( op == OpName::CoeffVar ) {
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        rtn = new typename VTypes::CoeffVariation(mpt, column);
      } else if ( op == OpName::EchoMapScore ) {
        typedef typename VTypes::EchoMapScore::ProcessType PT;
        typedef typename PT::PType PType;
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        PType pt(column, precision, useScientific);
        rtn = new typename VTypes::EchoMapScore(PT(pt, multivalColSep));
      } else if ( op == OpName::KthAvg ) {
        Ext::Assert<Ext::UserError>(2 == args.size(), "Need a value with " + OperationName(op));
        std::stringstream conv1(args[0]), conv2(args[1]);
        std::size_t column; conv1 >> column;
        double percentile; conv2 >> percentile;
        if ( 0 == percentile ) // min faster
          rtn = generate(OpName::Min, args, multivalColSep, precision, useScientific);
        else if ( 1 == percentile ) // max faster
          rtn = generate(OpName::Max, args, multivalColSep, precision, useScientific);
        else
          rtn = new typename VTypes::KthAverage(percentile, mpt, column); // does check on percentile
      } else if ( op == OpName::Max ) {
        typedef typename VTypes::Max::ProcessType PType;
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        PType pt(column, precision, useScientific);
        rtn = new typename VTypes::Max(pt, column);
      } else if ( op == OpName::MaxElement ) {
        typedef typename VTypes::MaxElement::ProcessType PType;
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        PType pt;
        rtn = new typename VTypes::MaxElement(pt, column);
      } else if ( op == OpName::Median ) {
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        rtn = new typename VTypes::Median(mpt, column);
      } else if ( op == OpName::Mad ) {
        Ext::Assert<Ext::UserError>(2 >= args.size(), "1 value allowed for --" + OperationName(op));
        std::stringstream conv1(args[0]);
        std::size_t column; conv1 >> column;
        if ( 2 == args.size() ) {
          std::stringstream conv2(args[1]);
          double multiplier;
          conv2 >> multiplier;
          rtn = new typename VTypes::MedianAbsoluteDeviation(mpt, multiplier, column);
        }
        else // use default multiplier of 1
          rtn = new typename VTypes::MedianAbsoluteDeviation(mpt, 1, column);
      } else if ( op == OpName::Min ) {
        typedef typename VTypes::Min::ProcessType PType;
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        PType pt(column, precision, useScientific);
        rtn = new typename VTypes::Min(pt, column);
      } else if ( op == OpName::MinElement ) {
        typedef typename VTypes::MinElement::ProcessType PType;
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        PType pt;
        rtn = new typename VTypes::MinElement(pt, column);
      } else if ( op == OpName::Stdev ) {
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        rtn = new typename VTypes::StdDev(mpt, column);
      } else if ( op == OpName::Sum ) {
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        rtn = new typename VTypes::Sum(mpt, column);
      } else if ( op == OpName::TMean ) {
        typedef typename VTypes::TMean::ProcessType PT;
        Ext::Assert<Ext::UserError>(3 == args.size(), "Need two values with " + OperationName(op));
        std::stringstream conv1(args[0]), conv2(args[1]), conv3(args[2]);
        double percentileLow = 100, percentileHigh = -100;
        std::size_t column; conv1 >> column;
        conv2 >> percentileLow;
        conv3 >> percentileHigh;
        PT pt(column, precision, useScientific);
        rtn = new typename VTypes::TMean(percentileLow, percentileHigh, pt, column);
      } else if ( op == OpName::Variance ) {
        std::stringstream conv(args[0]);
        std::size_t column; conv >> column;
        rtn = new typename VTypes::Variance(mpt, column);
      }
      return rtn;
    }
  };

  //===============
  // getVisitors()
  //===============
  template <typename GV, typename BedDistType, typename InputType>
  std::vector<typename GV::BaseClass*>
           getVisitors(GV& gv, const BedDistType& dt, const InputType& input) {
    std::vector<typename GV::BaseClass*> visitorGroup;
    std::vector<OpName>::const_iterator iter = input.visitorNames_.begin();
    typename GV::BaseClass* bc = static_cast<typename GV::BaseClass*>(0);
    while ( iter != input.visitorNames_.end() ) {
      bc = gv.generate(*iter, input.visitorArgs_[iter-input.visitorNames_.begin()],
                       input.multiDelim_, input.precision_, input.useScientific_);
      if ( !bc )
        throw(Ext::ProgramError("Unknown Operation: " + OperationName(*iter) + ". Program Error Detected."));
      visitorGroup.push_back(bc);
      ++iter;
    } // while
    return visitorGroup;
  }

  //============
  // SelectBase : General Case
  //============
  template <bool ProcessMode, typename BedDistType>
  struct SelectBase {
    typedef Visitors::BedBaseVisitor<BedDistType, Bed::GBed, Bed::GBed> BaseClass;
    enum { IsFastMode = ProcessMode };
  };

  //============
  // SelectBase : No fully nested components
  //============
  template <typename BedDistType>
  struct SelectBase<true, BedDistType> {
    typedef Visitors::Visitor<Bed::GBed, Bed::GBed> BaseClass;
    enum { IsFastMode = true };
  };

  //==============
  // callSweep1(): single-file mode
  //==============
  template <bool ProcessMode, typename SweepDistType, typename BedDistType, typename InputType>
  void callSweep1(const SweepDistType& st,
                  const BedDistType& dt,
                  const InputType& input) {
    typedef typename SelectBase<ProcessMode, BedDistType>::BaseClass BaseClass;
    constexpr bool isFast = SelectBase<ProcessMode, BedDistType>::IsFastMode;
    BedMap::GenerateVisitors<BaseClass> gv;
    std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, input);
    runSweep1<BaseClass>(st, dt, input, visitorGroup, isFast);
  }

  //==============
  // callSweep2(): multi-file mode
  //==============
  template <bool ProcessMode, typename SweepDistType, typename BedDistType, typename InputType>
  void callSweep2(const SweepDistType& st,
                  const BedDistType& dt,
                  const InputType& input) {
    typedef typename SelectBase<ProcessMode, BedDistType>::BaseClass BaseClass;
    constexpr bool isFast = SelectBase<ProcessMode, BedDistType>::IsFastMode;
    BedMap::GenerateVisitors<BaseClass> gv;
    std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, input);
    runSweep2<BaseClass>(st, dt, input, visitorGroup, isFast);
  }

  //================
  // selectSweep():
  //================
  template <typename SweepDistType, typename BedDistType, typename InputType>
  void selectSweep(const SweepDistType& st,
                   const BedDistType& dt,
                   const InputType& input) {

    const bool SpecialMode = true;
    const bool GeneralMode = false;
    if ( input.fastMode_ && !st.Symmetric ) // symmetry is a necessary (and sufficient if no nested elements)
      throw(Ext::ProgramError("Symmetric distance required for fast mode!"));

    if ( input.mapFileName_.empty() ) { // single-file mode
      if ( input.fastMode_ )
        callSweep1<SpecialMode>(st, dt, input);
      else
        callSweep1<GeneralMode>(st, dt, input);
    } else { // dual-file mode
      if ( input.fastMode_ )
        callSweep2<SpecialMode>(st, dt, input);
      else
        callSweep2<GeneralMode>(st, dt, input);
    }
  }

} // namespace BedMap
