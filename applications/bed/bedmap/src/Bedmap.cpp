/*
  FILE: Bedmap.cpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Oct 17 14:23:29 PDT 2007
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

#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/Visitors.hpp"
#include "algorithm/visitors/bed/BedBaseVisitor.hpp"
#include "algorithm/visitors/helpers/NamedVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "algorithm/WindowSweep.hpp"
#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedDistances.hpp"
#include "data/bed/BedTypes.hpp"
#include "data/starch/starchApi.hpp"
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
  template <typename SweepDistType, typename BedDistType>
  void selectSweep(const SweepDistType& st,
                   const BedDistType& dt,
                   const std::string& refFileName,
                   const std::string& mapFileName,
                   int minRefFields,
                   int minMapFields,
                   bool errorCheck,
                   const std::string& colSep,
                   const std::string& multivalColSep,
                   int precision,
                   bool useScientific,
                   bool fastMode,
                   const std::string& chrom,
                   bool skipUnmappedRows,
                   const std::vector<std::string>& visitorNames,
                   const std::vector <std::vector<std::string> >& visitorArgs);

} // namespace BedMap




//========
// main()
//========
int main(int argc, char **argv) {

  typedef BedMap::Input<Ext::UserError, BedMap::Help, BedMap::Version> InputType;

  try {
    InputType progInput(argc, argv);

    std::vector<std::string> visitorNames = progInput.visitorNames_;
    std::vector< std::vector<std::string> > visitorArgs = progInput.visitorArgs_;
    int prec = progInput.precision_;
    bool sci = progInput.useScientific_;
 
    if ( progInput.isPercMap_ ) { // % overlap relative to MapType's size (signalmapish)
      Bed::PercentOverlapMapping bedDist(progInput.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, progInput.refFileName_, progInput.mapFileName_,
                          progInput.minRefFields_, progInput.minMapFields_, progInput.errorCheck_,
                          progInput.outDelim_, progInput.multiDelim_, prec, sci, progInput.fastMode_,
                          progInput.chrom_, progInput.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( progInput.isPercRef_ ) { // % overlap relative to RefTypes's size (setops -e)
      Bed::PercentOverlapReference bedDist(progInput.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, progInput.refFileName_, progInput.mapFileName_,
                          progInput.minRefFields_, progInput.minMapFields_, progInput.errorCheck_,
                          progInput.outDelim_, progInput.multiDelim_, prec, sci, progInput.fastMode_,
                          progInput.chrom_, progInput.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( progInput.isPercBoth_ ) { // % overlap relative to both MapType's and RefType's sizes
      Bed::PercentOverlapBoth bedDist(progInput.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, progInput.refFileName_, progInput.mapFileName_,
                          progInput.minRefFields_, progInput.minMapFields_, progInput.errorCheck_,
                          progInput.outDelim_, progInput.multiDelim_, prec, sci, progInput.fastMode_,
                          progInput.chrom_, progInput.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( progInput.isPercEither_ ) { // % overlap relative to either MapType's or RefType's size
      Bed::PercentOverlapEither bedDist(progInput.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, progInput.refFileName_, progInput.mapFileName_,
                          progInput.minRefFields_, progInput.minMapFields_, progInput.errorCheck_,
                          progInput.outDelim_, progInput.multiDelim_, prec, sci, progInput.fastMode_,
                          progInput.chrom_, progInput.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( progInput.isRangeBP_ ) { // buffer each reference element
      Bed::RangedDist bedDist(progInput.rangeBP_);
      Bed::RangedDist sweepDist(progInput.rangeBP_); // same as bedDist in this case
      BedMap::selectSweep(sweepDist, bedDist, progInput.refFileName_, progInput.mapFileName_,
                          progInput.minRefFields_, progInput.minMapFields_, progInput.errorCheck_,
                          progInput.outDelim_, progInput.multiDelim_, prec, sci, progInput.fastMode_,
                          progInput.chrom_, progInput.skipUnmappedRows_, visitorNames, visitorArgs);
    } else { // require a certain amount of bp overlap
      Bed::Overlapping bedDist(progInput.overlapBP_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, progInput.refFileName_, progInput.mapFileName_,
                          progInput.minRefFields_, progInput.minMapFields_, progInput.errorCheck_,
                          progInput.outDelim_, progInput.multiDelim_, prec, sci, progInput.fastMode_,
                          progInput.chrom_, progInput.skipUnmappedRows_, visitorNames, visitorArgs);
    }

    return(EXIT_SUCCESS);
  } catch(const BedMap::Help& h) { // show usage and exit success
    std::cout << BedMap::prognm << std::endl;
    std::cout << "  citation: " << BedMap::citation << std::endl;
    std::cout << "  version:  " << BedMap::version << std::endl;
    std::cout << "  authors:  " << BedMap::authors << std::endl;
    std::cout << BedMap::Usage() << std::endl;
    return(EXIT_SUCCESS);
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
  } catch(const std::exception& stx) {
    std::cerr << "May use bedmap --help for more help.\n" << std::endl;
    std::cerr << "Error: " << stx.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown Error.  Aborting" << std::endl;
  }
  return(EXIT_FAILURE);
}


namespace BedMap {

  //============
  // runSweep(): single-file mode
  //============
  template <typename BaseClass, typename SweepDistType, typename BedDistType>
  void runSweep(const SweepDistType& st,
                const BedDistType& dt,
                const std::string& refFileName,
                bool errorCheck,
                bool nestCheck,
                bool fastMode,
                const std::string& columnSep,
                const std::string& chrom,
                bool skipUnmappedRows,
                std::vector<BaseClass*>& visitorGroup) {

    typedef typename BaseClass::reference_type RefType;
    typedef Visitors::Helpers::PrintDelim PrintType;

    // Set up visitors
    PrintType processFields(columnSep);
    PrintType processRows("\n");
    typedef Visitors::MultiVisitor<PrintType, PrintType, BaseClass> MVType;
    MVType multiv(visitorGroup, dt, processFields, processRows, !skipUnmappedRows);

    if ( !errorCheck ) { // faster iterators
      // Create file handle iterators
      Ext::FPWrap<Ext::InvalidFile> refFile(refFileName);
      Bed::allocate_iterator_starch_bed<RefType*> refFileI(refFile, chrom), refFileEnd;

      // Do work
      if ( !fastMode )
        WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
      else // no nested elements
        WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
    } else {
      // Create file handle iterators
      bool isStdin = (refFileName == "-");
      std::ifstream infile(refFileName.c_str());
      if ( !isStdin && !infile )
          throw(Ext::UserError("Unable to find: " + refFileName));
      if ( isStdin ) {
        Bed::bed_check_iterator<RefType*> refFileI(std::cin, refFileName, chrom, nestCheck);
        Bed::bed_check_iterator<RefType*> refFileEnd;
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
      } else {
        Bed::bed_check_iterator<RefType*> refFileI(infile, refFileName, chrom, nestCheck);
        Bed::bed_check_iterator<RefType*> refFileEnd;
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
      }
    }

    // multiv does cleanup
  }

  //============
  // runSweep(): multi-file mode
  //============
  template <typename BaseClass, typename SweepDistType, typename BedDistType>
  void runSweep(const SweepDistType& st,
                const BedDistType& dt,
                const std::string& refFileName,
                const std::string& mapFileName,
                bool errorCheck,
                bool nestCheck,
                bool fastMode,
                const std::string& columnSep,
                const std::string& chrom,
                bool skipUnmappedRows,
                std::vector<BaseClass*>& visitorGroup) {

    typedef typename BaseClass::reference_type RefType;
    typedef typename BaseClass::mapping_type MapType;
    typedef Visitors::Helpers::PrintDelim PrintType;

    // Set up visitors
    PrintType processFields(columnSep);
    PrintType processRows("\n");
    typedef Visitors::MultiVisitor<PrintType, PrintType, BaseClass> MVType;
    MVType multiv(visitorGroup, dt, processFields, processRows, !skipUnmappedRows);

    if ( !errorCheck ) { // faster iterators
      // Create file handle iterators
      Ext::FPWrap<Ext::InvalidFile> refFile(refFileName);
      Bed::allocate_iterator_starch_bed<RefType*> refFileI(refFile, chrom), refFileEnd;
      Ext::FPWrap<Ext::InvalidFile> mapFile(mapFileName);
      Bed::allocate_iterator_starch_bed<MapType*> mapFileI(mapFile, chrom), mapFileEnd;

      // Do work
      const bool noExtraWork = false;
      if ( !fastMode )
        WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, noExtraWork);
      else // no nested elements
        WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, noExtraWork);
    } else {
      // Create file handle iterators
      typedef Ext::UserError EType;
      bool isStdinRef = (refFileName == "-");
      bool isStdinMap = (mapFileName == "-");
      if ( isStdinRef && isStdinMap )
        throw(EType("Cannot have both input files set to '-'"));
      std::ifstream rfin(refFileName.c_str()), mfin(mapFileName.c_str());
      if ( !isStdinRef && !rfin )
        throw(EType("Unable to find: " + refFileName));
      if ( !isStdinMap && !mfin )
        throw(EType("Unable to find: " + mapFileName));

      // Do work
      const bool sweepAllInput = true;
      if ( isStdinRef ) {
        Bed::bed_check_iterator<RefType*> refFileI(std::cin, refFileName, chrom, nestCheck), refFileEnd;
        Bed::bed_check_iterator<MapType*> mapFileI(mfin, mapFileName, chrom, nestCheck), mapFileEnd;
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAllInput);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAllInput);
      } else {
        Bed::bed_check_iterator<RefType*> refFileI(rfin, refFileName, chrom, nestCheck), refFileEnd;
        if ( isStdinMap ) {
          Bed::bed_check_iterator<MapType*> mapFileI(std::cin, mapFileName, chrom, nestCheck), mapFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAllInput);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAllInput);
        } else {
          Bed::bed_check_iterator<MapType*> mapFileI(mfin, mapFileName, chrom, nestCheck), mapFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAllInput);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAllInput);
        }
      }
    }

    // multiv does cleanup
  }


  //=========
  // upper()
  //=========
  std::string upper(const std::string& s) {
    std::string t = s;
    for ( std::size_t i = 0; i < t.size(); ++i )
      t[i] = std::toupper(t[i]);
    return(t);
  }


  //===========
  // visName()
  //===========
  template <typename VisitorType>
  std::string visName() {
    return upper(Visitors::Helpers::VisitorName<VisitorType>::Name());
  }


  template <typename BaseClass, int NumFields>
  struct GenerateVisitors; // not implemented


  //===========================
  // GenerateVisitors<> : Bed3
  //===========================
  template <typename BaseVisitor>
  struct GenerateVisitors<BaseVisitor, 3> {

    typedef BaseVisitor BaseClass;

    template <typename DistType>
    BaseVisitor* generate(const DistType&, const std::string& className,
                          const std::vector<std::string>&,
                          const std::string& multivalColSep,
                          int precision, bool useScientific) {
  
      typedef VisitorTypes<BaseVisitor> VTypes;
      BaseVisitor* rtn = 0;
  
      // Create an instance associated with each name in classNames
      std::string nm = upper(className);
   

      if ( nm == visName<typename VTypes::Count>() )
        rtn = new typename VTypes::Count;
      else if ( nm == visName<typename VTypes::EchoMapAll>() ) {
        typedef typename VTypes::EchoMapAll::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapAll(PT(PType(), multivalColSep));
      }
      else if ( nm == visName<typename VTypes::EchoMapLength>() )
        rtn = new typename VTypes::EchoMapLength;
      else if ( nm == visName<typename VTypes::EchoMapIntersectLength>() )
        rtn = new typename VTypes::EchoMapIntersectLength;
      else if ( nm == visName<typename VTypes::EchoMapRange>() )
        rtn = new typename VTypes::EchoMapRange;
      else if ( nm == visName<typename VTypes::EchoRefAll>() )
        rtn = new typename VTypes::EchoRefAll;
      else if ( nm == visName<typename VTypes::Indicator>() )
        rtn = new typename VTypes::Indicator;
      else if ( nm == visName<typename VTypes::OvrAgg>() )
        rtn = new typename VTypes::OvrAgg;
      else if ( nm == visName<typename VTypes::OvrUniq>() )
        rtn = new typename VTypes::OvrUniq;
      else if ( nm == visName<typename VTypes::OvrUniqFract>() ) {
        typedef typename VTypes::OvrUniqFract::ProcessType PT;
        PT pt(precision, useScientific);
        rtn = new typename VTypes::OvrUniqFract(pt);
      }

      return(rtn);
    }
  };


  //===========================
  // GenerateVisitors<> : Bed4
  //===========================
  template <typename BaseVisitor>
  struct GenerateVisitors<BaseVisitor, 4>
           : GenerateVisitors<BaseVisitor, 3> {

    typedef GenerateVisitors<BaseVisitor, 3> SuperClass;

    template <typename DistType>
    BaseVisitor* generate(const DistType& d, const std::string& className,
                          const std::vector<std::string>& args,
                          const std::string& multivalColSep,
                          int precision, bool useScientific) {
  
      typedef VisitorTypes<BaseVisitor> VTypes;
      BaseVisitor* rtn = SuperClass::generate(d, className, args, multivalColSep, precision, useScientific);
      if ( rtn )
        return(rtn);

      // Create an instance associated with className
      std::string nm = upper(className);
      if ( nm == visName<typename VTypes::EchoMapID>() ) {
        typedef typename VTypes::EchoMapID::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapID(PT(PType(), multivalColSep));
      } else if ( nm == visName<typename VTypes::EchoMapUniqueID>() ) {
        typedef typename VTypes::EchoMapUniqueID::ProcessType PT;
        rtn = new typename VTypes::EchoMapUniqueID(PT(multivalColSep));
      }

      return(rtn);
    }
  };


  //===========================
  // GenerateVisitors<> : Bed5
  //===========================
  template <typename BaseVisitor>
  struct GenerateVisitors<BaseVisitor, 5>
           : GenerateVisitors<BaseVisitor, 4> {

    typedef GenerateVisitors<BaseVisitor, 4> SuperClass;

    template <typename DistType>
    BaseVisitor* generate(const DistType& d, const std::string& className,
                          const std::vector<std::string>& args,
                          const std::string& multivalColSep,
                          int precision, bool useScientific) {
  
      typedef VisitorTypes<BaseVisitor> VTypes;
      BaseVisitor* rtn = SuperClass::generate(d, className, args, multivalColSep, precision, useScientific);
      if ( rtn )
        return(rtn);

      // Create an instance associated with className
      std::string nm = upper(className);
      typedef typename VTypes::Average::ProcessType PT; // applicable to most numeric outputs
      PT pt(precision, useScientific);

      if ( nm == visName<typename VTypes::Average>() )
        rtn = new typename VTypes::Average(pt);
      else if ( nm == visName<typename VTypes::CoeffVariation>() )
        rtn = new typename VTypes::CoeffVariation(pt);
      else if ( nm == visName<typename VTypes::EchoMapScore>() ) {
        typedef typename VTypes::EchoMapScore::ProcessType PT;
        rtn = new typename VTypes::EchoMapScore(PT(pt, multivalColSep));
      }
      else if ( nm == visName<typename VTypes::KthAverage>() ) {
        Ext::Assert<Ext::UserError>(1 == args.size(), "Need a value with " + nm);
        std::stringstream conv1(args[0]);
        double percentile;
        conv1 >> percentile;
        if ( 0 == percentile ) // min faster
          rtn = new typename VTypes::Min(pt);
        else if ( 1 == percentile ) // max faster
          rtn = new typename VTypes::Max(pt);
        else
          rtn = new typename VTypes::KthAverage(percentile, pt); // does check on percentile
      }
      else if ( nm == visName<typename VTypes::Max>() )
        rtn = new typename VTypes::Max(pt);
      else if ( nm == visName<typename VTypes::MaxElement>() ) {
        typedef typename VTypes::MaxElement::ProcessType MPT;
        rtn = new typename VTypes::MaxElement(MPT());
      }
      else if ( nm == visName<typename VTypes::Median>() )
        rtn = new typename VTypes::Median(pt);
      else if ( nm == visName<typename VTypes::MedianAbsoluteDeviation>() ) {
        Ext::Assert<Ext::UserError>(1 >= args.size(), "Need 1 value with " + nm);
        if ( 1 == args.size() ) {
          std::stringstream conv1(args[0]);
          double multiplier;
          conv1 >> multiplier;
          rtn = new typename VTypes::MedianAbsoluteDeviation(pt, multiplier);
        }
        else // use default multiplier
          rtn = new typename VTypes::MedianAbsoluteDeviation(pt);
      }
      else if ( nm == visName<typename VTypes::Min>() )
        rtn = new typename VTypes::Min(pt);
      else if ( nm == visName<typename VTypes::MinElement>() ) {
        typedef typename VTypes::MinElement::ProcessType MPT;
        rtn = new typename VTypes::MinElement(MPT());
      }
      else if ( nm == visName<typename VTypes::StdDev>() )
        rtn = new typename VTypes::StdDev(pt);
      else if ( nm == visName<typename VTypes::Sum>() )
        rtn = new typename VTypes::Sum(pt);
      else if ( nm == visName<typename VTypes::TMeans>() ) {
        Ext::Assert<Ext::UserError>(2 == args.size(), "Need two values with " + nm);
        std::stringstream conv1(args[0]), conv2(args[1]);
        double percentileLow = 100, percentileHigh = -100;
        conv1 >> percentileLow;
        conv2 >> percentileHigh;
        rtn = new typename VTypes::TMeans(percentileLow, percentileHigh, pt);
      }
      else if ( nm == visName<typename VTypes::Variance>() )
        rtn = new typename VTypes::Variance(pt);

      return(rtn);
    }
  };


  //===============
  // getVisitors()
  //===============
  template <typename GV, typename BedDistType>
  std::vector<typename GV::BaseClass*>
           getVisitors(GV& gv, const BedDistType& dt, const std::string& multivalColSep,
                       int precision, bool useScientific, const std::vector<std::string>& visitorNames,
                       const std::vector< std::vector<std::string> >& visitorArgs) {

    std::vector<typename GV::BaseClass*> visitorGroup;
    std::vector<std::string>::const_iterator iter = visitorNames.begin();
    typename GV::BaseClass* bc = static_cast<typename GV::BaseClass*>(0);
    while ( iter != visitorNames.end() ) {
      bc = gv.generate(dt, *iter, visitorArgs[iter-visitorNames.begin()], multivalColSep, precision, useScientific);
      if ( !bc )
        throw(Ext::ProgramError("Unknown Operation: " + *iter + ". Program Error Detected."));
      visitorGroup.push_back(bc);
      ++iter;
    } // while
    return(visitorGroup);
  }

  //==============
  // SelectBase<> : General Case
  //==============
  template <bool ProcessMode, typename BedDistType, typename RefType, typename MapType = RefType>
  struct SelectBase {
    typedef Visitors::BedBaseVisitor<BedDistType, RefType, MapType> BaseClass;
    enum { IsFastMode = ProcessMode };
  };

  //==============
  // SelectBase<> : No fully nested components
  //==============
  template <typename BedDistType, typename RefType, typename MapType>
  struct SelectBase<true, BedDistType, RefType, MapType> {
    typedef Visitors::Visitor<RefType, MapType> BaseClass;
    enum { IsFastMode = true };
  };

  //=============
  // callSweep(): multi-file mode
  //=============
  template <bool ProcessMode, typename SweepDistType, typename BedDistType>
  void callSweep(const SweepDistType& st,
                 const BedDistType& dt,
                 const std::string& refFileName,
                 const std::string& mapFileName,
                 int minRefFields,
                 int minMapFields,
                 bool errorCheck,
                 const std::string& colSep,
                 const std::string& multivalColSep,
                 int precision,
                 bool useScientific,
                 const std::string& chrom,
                 bool skipUnmappedRows,
                 const std::vector<std::string>& visitorNames,
                 const std::vector< std::vector<std::string> >& visitorArgs) {

    // minRefFields must be <= minMapFields
    Ext::Assert<Ext::ProgramError>(minRefFields <= minMapFields,
                                   "BedMap::callSweep() minimum fields program error detected");

    const bool nestCheck = ProcessMode;
    if ( minMapFields < 4 ) { // just need Bed3
      typedef Bed::B3Rest RefType;
      typedef Bed::B3Rest MapType;
      typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
      BedMap::GenerateVisitors<BaseClass, 3> gv;
      std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, precision,
                                                         useScientific, visitorNames, visitorArgs);
      runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                          ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
    } else if ( minMapFields < 5 ) { // just need Bed4 for Map and Bed3 for Ref
      Ext::Assert<Ext::ProgramError>(minRefFields < minMapFields,
                                     "BedMap::callSweep()-2 minimum fields program error detected");
      typedef Bed::B3Rest RefType;
      typedef Bed::B4Rest MapType;
      typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
      BedMap::GenerateVisitors<BaseClass, 4> gv;
      std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, precision,
                                                         useScientific, visitorNames, visitorArgs);
      runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                          ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
    } else { // need Bed5 for Map and Bed3 for Ref
      Ext::Assert<Ext::ProgramError>(minRefFields == 3,
                                     "BedMap::callSweep()-2 minimum fields program error detected");
      typedef Bed::B3Rest RefType;
      typedef Bed::B5Rest MapType;
      typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
      BedMap::GenerateVisitors<BaseClass, 5> gv;
      std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, precision,
                                                         useScientific, visitorNames, visitorArgs);
      runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                          ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
    }
  }

  //=============
  // callSweep(): single-file mode
  //=============
  template <bool ProcessMode, typename SweepDistType, typename BedDistType>
  void callSweep(const SweepDistType& st,
                 const BedDistType& dt,
                 const std::string& refFileName,
                 int minRefFields,
                 bool errorCheck,
                 const std::string& colSep,
                 const std::string& multivalColSep,
                 int precision,
                 bool useScientific,
                 const std::string& chrom,
                 bool skipUnmappedRows,
                 const std::vector<std::string>& visitorNames,
                 const std::vector< std::vector<std::string> >& visitorArgs) {

    const bool nestCheck = ProcessMode;
    if ( minRefFields < 4 ) { // just need Bed3
      typedef Bed::B3Rest RefType;
      typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
      BedMap::GenerateVisitors<BaseClass, 3> gv;
      std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, precision,
                                                         useScientific, visitorNames, visitorArgs);
      runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                          ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
    } else if ( minRefFields < 5 ) { // need Bed4
      typedef Bed::B4Rest RefType;
      typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
      BedMap::GenerateVisitors<BaseClass, 4> gv;
      std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, precision,
                                                         useScientific, visitorNames, visitorArgs);
      runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                          ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
    } else { // need Bed5
      typedef Bed::B5Rest RefType;
      typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
      BedMap::GenerateVisitors<BaseClass, 5> gv;
      std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, precision,
                                                         useScientific, visitorNames, visitorArgs);
      runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                          ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
    }
  }


  //================
  // selectSweep():
  //================
  template <typename SweepDistType, typename BedDistType>
  void selectSweep(const SweepDistType& st,
                   const BedDistType& dt,
                   const std::string& refFileName,
                   const std::string& mapFileName,
                   int minRefFields,
                   int minMapFields,
                   bool errorCheck,
                   const std::string& colSep,
                   const std::string& multivalColSep,
                   int precision,
                   bool useScientific,
                   bool fastMode,
                   const std::string& chrom,
                   bool skipUnmappedRows,
                   const std::vector<std::string>& visitorNames,
                   const std::vector< std::vector<std::string> >& visitorArgs) {

    const bool SpecialMode = true;
    const bool GeneralMode = false;

    if ( mapFileName.empty() ) { // single-file mode
      if ( fastMode )
        callSweep<SpecialMode>(st, dt, refFileName, minRefFields, errorCheck, colSep, multivalColSep,
                               precision, useScientific, chrom, skipUnmappedRows, visitorNames, visitorArgs);
      else
        callSweep<GeneralMode>(st, dt, refFileName, minRefFields, errorCheck, colSep, multivalColSep,
                               precision, useScientific, chrom, skipUnmappedRows, visitorNames, visitorArgs);
    } else { // dual-file mode
      if ( fastMode )
        callSweep<SpecialMode>(st, dt, refFileName, mapFileName, minRefFields, minMapFields,
                               errorCheck, colSep, multivalColSep, precision, useScientific,
                               chrom, skipUnmappedRows, visitorNames, visitorArgs);
      else
        callSweep<GeneralMode>(st, dt, refFileName, mapFileName, minRefFields, minMapFields,
                               errorCheck, colSep, multivalColSep, precision, useScientific,
                               chrom, skipUnmappedRows, visitorNames, visitorArgs);
    }
  }


} // namespace BedMap
