/*
  Author: Scott Kuehn, Shane Neph
  Date:   Wed Oct 17 14:23:29 PDT 2007
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

#include <sys/stat.h>

#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/helpers/NamedVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "algorithm/WindowSweep.hpp"
#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "data/bed/AllocateIterator_BED_starch_minmem.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedCheckIterator_minmem.hpp"
#include "data/bed/BedDistances.hpp"
#include "data/bed/BedTypes.hpp"
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
  const std::string version = BEDOPS::version();
  const std::string authors = "Shane Neph & Scott Kuehn";
  const std::string citation = BEDOPS::citation();
  constexpr std::size_t PoolSz = 8*8*8;
  bool minimumMemory = false;

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
                   bool nestCheck,
                   const std::string& colSep,
                   const std::string& multivalColSep,
                   const std::string& onEmptyMap,
                   int precision,
                   bool useScientific,
                   bool fastMode,
                   bool sweepAll,
                   const std::string& chrom,
                   bool skipUnmappedRows,
                   const std::vector<std::string>& visitorNames,
                   const std::vector <std::vector<std::string> >& visitorArgs);

  bool checkStarchNesting(const std::string&, const std::string&);

} // namespace BedMap




//========
// main()
//========
int main(int argc, char **argv) {

  typedef BedMap::Input<Ext::UserError, BedMap::Help, BedMap::Version> InputType;

  try {
    InputType input(argc, argv);

    std::vector<std::string> visitorNames = input.visitorNames_;
    std::vector< std::vector<std::string> > visitorArgs = input.visitorArgs_;
    const int prec = input.precision_;
    const bool sci = input.useScientific_;
    BedMap::minimumMemory = input.useMinMemory_;

    // if all Starch inputs and no nested elements, then can use --faster if the
    //   overlap criterion allows it.
    const bool starchFast = !BedMap::checkStarchNesting(input.refFileName_, input.mapFileName_);
    const bool nestCheck = input.errorCheck_ && input.fastMode_;

    if ( input.isPercMap_ ) { // % overlap relative to MapType's size (signalmapish)
      Bed::PercentOverlapMapping bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, nestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, input.fastMode_,
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( input.isPercRef_ ) { // % overlap relative to RefTypes's size (setops -e)
      Bed::PercentOverlapReference bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, nestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, input.fastMode_,
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( input.isPercBoth_ ) { // % overlap relative to both MapType's and RefType's sizes
      Bed::PercentOverlapBoth bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, nestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, input.fastMode_,
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( input.isExact_ ) { // must be identical coordinates; should work fine with fully-nested elements
      Bed::Exact bedDist;
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      const bool fastMode = true;
      const bool noNestCheck = false; // safe with fully-nested elements
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, noNestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, fastMode,
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( input.isPercEither_ ) { // % overlap relative to either MapType's or RefType's size
      Bed::PercentOverlapEither bedDist(input.percOvr_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, nestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, input.fastMode_,
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
    } else if ( input.isRangeBP_ ) { // buffer each reference element
      Bed::RangedDist bedDist(input.rangeBP_);
      Bed::RangedDist sweepDist(input.rangeBP_); // same as bedDist in this case
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, nestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, (input.fastMode_ || starchFast),
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
    } else { // require a certain amount of bp overlap
      Bed::Overlapping bedDist(input.overlapBP_);
      Bed::Overlapping sweepDist(0); // dist type for sweep different from BedBaseVisitor's
      BedMap::selectSweep(sweepDist, bedDist, input.refFileName_, input.mapFileName_,
                          input.minRefFields_, input.minMapFields_, input.errorCheck_, nestCheck,
                          input.outDelim_, input.multiDelim_, input.unmappedVal_, prec, sci, (input.fastMode_ || starchFast),
                          input.sweepAll_, input.chrom_, input.skipUnmappedRows_, visitorNames, visitorArgs);
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
    return EXIT_SUCCESS;
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
  // get_pool()
  //============
  template <typename BedTypePtr>
  Ext::PooledMemory<typename std::remove_pointer<BedTypePtr>::type, PoolSz>&
  get_pool() {
    static Ext::PooledMemory<typename std::remove_pointer<BedTypePtr>::type, PoolSz> pool;
    return pool;
  }

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

    typedef typename std::remove_const<typename BaseClass::RefType>::type RefType;
    typedef Visitors::Helpers::PrintDelim PrintType;

    // Set up visitors
    PrintType processFields(columnSep);
    PrintType processRows("\n");
    typedef Visitors::MultiVisitor<PrintType, PrintType, BaseClass> MVType;
    MVType multiv(visitorGroup, dt, processFields, processRows, !skipUnmappedRows);

    if ( !errorCheck ) { // faster iterators
      // Create file handle iterators
      Ext::FPWrap<Ext::InvalidFile> refFile(refFileName);
      if ( !minimumMemory ) {
        auto& mem1 = get_pool<RefType*>();
        Bed::allocate_iterator_starch_bed<RefType*, PoolSz> refFileI(refFile, mem1, chrom), refFileEnd;

        // Do work
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
      } else { // old school minimal memory iterator
        Bed::allocate_iterator_starch_bed_mm<RefType*> refFileI(refFile, chrom), refFileEnd;

        // Do work
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
      }
    } else {
      // Create file handle iterators
      bool isStdin = (refFileName == "-");
      std::ifstream infile(refFileName.c_str());
      if ( !isStdin && !infile )
          throw(Ext::UserError("Unable to find: " + refFileName));
      if ( isStdin ) {
        if ( !minimumMemory ) {
          auto& mem1 = get_pool<RefType*>();
          Bed::bed_check_iterator<RefType*, PoolSz> refFileI(std::cin, refFileName, mem1, chrom, nestCheck);
          Bed::bed_check_iterator<RefType*, PoolSz> refFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
        } else { // old school minimum memory iterator
          Bed::bed_check_iterator_mm<RefType*> refFileI(std::cin, refFileName, chrom, nestCheck);
          Bed::bed_check_iterator_mm<RefType*> refFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
        }
      } else {
        if ( !minimumMemory ) {
          auto& mem1 = get_pool<RefType*>();
          Bed::bed_check_iterator<RefType*, PoolSz> refFileI(infile, refFileName, mem1, chrom, nestCheck);
          Bed::bed_check_iterator<RefType*, PoolSz> refFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
        } else { // old school minimum memory iterator
          Bed::bed_check_iterator_mm<RefType*> refFileI(infile, refFileName, chrom, nestCheck);
          Bed::bed_check_iterator_mm<RefType*> refFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, st, multiv);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, dt, multiv);
        }
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
                bool sweepAll,
                const std::string& columnSep,
                const std::string& chrom,
                bool skipUnmappedRows,
                std::vector<BaseClass*>& visitorGroup) {

    typedef typename std::remove_const<typename BaseClass::RefType>::type RefType;
    typedef typename std::remove_const<typename BaseClass::MapType>::type MapType;
    typedef Visitors::Helpers::PrintDelim PrintType;

    // Set up visitors
    PrintType processFields(columnSep);
    PrintType processRows("\n");
    typedef Visitors::MultiVisitor<PrintType, PrintType, BaseClass> MVType;
    MVType multiv(visitorGroup, dt, processFields, processRows, !skipUnmappedRows);

    if ( !errorCheck ) { // faster iterators
      // Create file handle iterators
      Ext::FPWrap<Ext::InvalidFile> refFile(refFileName);
      if ( !minimumMemory ) {
        auto& mem1 = get_pool<RefType*>();
        Bed::allocate_iterator_starch_bed<RefType*, PoolSz> refFileI(refFile, mem1, chrom), refFileEnd;
        Ext::FPWrap<Ext::InvalidFile> mapFile(mapFileName);
        auto& mem2 = get_pool<MapType*>();
        Bed::allocate_iterator_starch_bed<MapType*, PoolSz> mapFileI(mapFile, mem2, chrom), mapFileEnd;

        // Do work
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
      } else { // old school minimal memory iterator
        Bed::allocate_iterator_starch_bed_mm<RefType*> refFileI(refFile, chrom), refFileEnd;
        Ext::FPWrap<Ext::InvalidFile> mapFile(mapFileName);
        Bed::allocate_iterator_starch_bed_mm<MapType*> mapFileI(mapFile, chrom), mapFileEnd;

        // Do work
        if ( !fastMode )
          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
        else // no nested elements
          WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
      }
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
      if ( !minimumMemory ) {
        auto& mem1 = get_pool<RefType*>();
        auto& mem2 = get_pool<MapType*>();
        if ( isStdinRef ) {
          Bed::bed_check_iterator<RefType*, PoolSz> refFileI(std::cin, refFileName, mem1, chrom, nestCheck), refFileEnd;
          Bed::bed_check_iterator<MapType*, PoolSz> mapFileI(mfin, mapFileName, mem2, chrom, nestCheck), mapFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
        } else {
          Bed::bed_check_iterator<RefType*, PoolSz> refFileI(rfin, refFileName, mem1, chrom, nestCheck), refFileEnd;
          if ( isStdinMap ) {
            Bed::bed_check_iterator<MapType*, PoolSz> mapFileI(std::cin, mapFileName, mem2, chrom, nestCheck), mapFileEnd;
            if ( !fastMode )
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
            else // no nested elements
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
          } else {
            Bed::bed_check_iterator<MapType*, PoolSz> mapFileI(mfin, mapFileName, mem2, chrom, nestCheck), mapFileEnd;
            if ( !fastMode )
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
            else // no nested elements
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
          }
        }
      } else { // old school minimal memory iterator
        if ( isStdinRef ) {
          Bed::bed_check_iterator_mm<RefType*> refFileI(std::cin, refFileName, chrom, nestCheck), refFileEnd;
          Bed::bed_check_iterator_mm<MapType*> mapFileI(mfin, mapFileName, chrom, nestCheck), mapFileEnd;
          if ( !fastMode )
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
          else // no nested elements
            WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
        } else {
          Bed::bed_check_iterator_mm<RefType*> refFileI(rfin, refFileName, chrom, nestCheck), refFileEnd;
          if ( isStdinMap ) {
            Bed::bed_check_iterator_mm<MapType*> mapFileI(std::cin, mapFileName, chrom, nestCheck), mapFileEnd;
            if ( !fastMode )
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
            else // no nested elements
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
          } else {
            Bed::bed_check_iterator_mm<MapType*> mapFileI(mfin, mapFileName, chrom, nestCheck), mapFileEnd;
            if ( !fastMode )
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, st, multiv, sweepAll);
            else // no nested elements
              WindowSweep::sweep(refFileI, refFileEnd, mapFileI, mapFileEnd, dt, multiv, sweepAll);
          }
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
    return t;
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
                          const std::string& onEmptyMap,
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
        rtn = new typename VTypes::EchoMapAll(PT(PType(), multivalColSep, onEmptyMap));
      }
      else if ( nm == visName<typename VTypes::EchoMapLength>() ) {
        typedef typename VTypes::EchoMapLength::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapLength(PT(PType(), multivalColSep, onEmptyMap));
      } else if ( nm == visName<typename VTypes::EchoMapIntersectLength>() ) {
        typedef typename VTypes::EchoMapIntersectLength::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapIntersectLength(PT(PType(), multivalColSep, onEmptyMap));
      } else if ( nm == visName<typename VTypes::EchoMapRange>() ) {
        typedef typename VTypes::EchoMapRange::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapRange(PT(PType(), onEmptyMap));
      } else if ( nm == visName<typename VTypes::EchoRefAll>() )
        rtn = new typename VTypes::EchoRefAll;
      else if ( nm == visName<typename VTypes::EchoRefLength>() )
        rtn = new typename VTypes::EchoRefLength;
      else if ( nm == visName<typename VTypes::EchoRefSpan>() )
        rtn = new typename VTypes::EchoRefSpan;
      else if ( nm == visName<typename VTypes::EchoRefRowNumber>() )
        rtn = new typename VTypes::EchoRefRowNumber;
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
      return rtn;
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
                          const std::string& onEmptyMap,
                          int precision, bool useScientific) {

      typedef VisitorTypes<BaseVisitor> VTypes;
      BaseVisitor* rtn = SuperClass::generate(d, className, args, multivalColSep, onEmptyMap, precision, useScientific);
      if ( rtn )
        return rtn;

      // Create an instance associated with className
      std::string nm = upper(className);
      if ( nm == visName<typename VTypes::EchoMapID>() ) {
        typedef typename VTypes::EchoMapID::ProcessType PT;
        typedef typename PT::PType PType;
        rtn = new typename VTypes::EchoMapID(PT(PType(), multivalColSep, onEmptyMap));
      } else if ( nm == visName<typename VTypes::EchoMapUniqueID>() ) {
        typedef typename VTypes::EchoMapUniqueID::ProcessType PT;
        rtn = new typename VTypes::EchoMapUniqueID(PT(multivalColSep, onEmptyMap));
      }

      return rtn;
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
                          const std::string& onEmptyMap,
                          int precision, bool useScientific) {

      typedef VisitorTypes<BaseVisitor> VTypes;
      BaseVisitor* rtn = SuperClass::generate(d, className, args, multivalColSep, onEmptyMap, precision, useScientific);
      if ( rtn )
        return rtn;

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
        rtn = new typename VTypes::EchoMapScore(PT(pt, multivalColSep, onEmptyMap));
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
      else if ( nm == visName<typename VTypes::MaxElementStable>() ) {
        typedef typename VTypes::MaxElementStable::ProcessType MPT;
        rtn = new typename VTypes::MaxElementStable(MPT(precision, useScientific, onEmptyMap));
      }
      else if ( nm == visName<typename VTypes::MaxElementRand>() ) {
        typedef typename VTypes::MaxElementRand::ProcessType MPT;
        rtn = new typename VTypes::MaxElementRand(MPT(precision, useScientific, onEmptyMap));
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
      else if ( nm == visName<typename VTypes::MinElementRand>() ) {
        typedef typename VTypes::MinElementRand::ProcessType MPT;
        rtn = new typename VTypes::MinElementRand(MPT(precision, useScientific, onEmptyMap));
      }
      else if ( nm == visName<typename VTypes::MinElementStable>() ) {
        typedef typename VTypes::MinElementStable::ProcessType MPT;
        rtn = new typename VTypes::MinElementStable(MPT(precision, useScientific, onEmptyMap));
      }
      else if ( nm == visName<typename VTypes::StdDev>() )
        rtn = new typename VTypes::StdDev(pt);
      else if ( nm == visName<typename VTypes::Sum>() )
        rtn = new typename VTypes::Sum(pt);
      else if ( nm == visName<typename VTypes::TMean>() ) {
        Ext::Assert<Ext::UserError>(2 == args.size(), "Need two values with " + nm);
        std::stringstream conv1(args[0]), conv2(args[1]);
        double percentileLow = 100, percentileHigh = -100;
        conv1 >> percentileLow;
        conv2 >> percentileHigh;
        rtn = new typename VTypes::TMean(percentileLow, percentileHigh, pt);
      }
      else if ( nm == visName<typename VTypes::Variance>() )
        rtn = new typename VTypes::Variance(pt);
      else if ( nm == visName<typename VTypes::WMean>() )
        rtn = new typename VTypes::WMean(pt);

      return rtn;
    }
  };


  //===============
  // getVisitors()
  //===============
  template <typename GV, typename BedDistType>
  std::vector<typename GV::BaseClass*>
           getVisitors(GV& gv, const BedDistType& dt, const std::string& multivalColSep, const std::string& onEmptyMap,
                       int precision, bool useScientific, const std::vector<std::string>& visitorNames,
                       const std::vector< std::vector<std::string> >& visitorArgs) {

    std::vector<typename GV::BaseClass*> visitorGroup;
    std::vector<std::string>::const_iterator iter = visitorNames.begin();
    typename GV::BaseClass* bc = static_cast<typename GV::BaseClass*>(0);
    while ( iter != visitorNames.end() ) {
      bc = gv.generate(dt, *iter, visitorArgs[iter-visitorNames.begin()], multivalColSep, onEmptyMap, precision, useScientific);
      if ( !bc )
        throw(Ext::ProgramError("Unknown Operation: " + *iter + ". Program Error Detected."));
      visitorGroup.push_back(bc);
      ++iter;
    } // while
    return visitorGroup;
  }

  //==============
  // SelectBED<>
  //==============
  template <int NFields, bool UseMemPool = true>
  struct SelectBED;

  template <>
  struct SelectBED<3, true> {
    typedef Bed::BTAllRest::Bed3Type BType;
  };

  template <>
  struct SelectBED<4, true> {
    typedef Bed::BTAllRest::Bed4Type BType;
  };

  template <>
  struct SelectBED<5, true> {
    typedef Bed::BTAllRest::Bed5Type BType;
  };


  template <>
  struct SelectBED<3, false> {
    typedef Bed::BTAllRestNoPool::Bed3Type BType;
  };

  template <>
  struct SelectBED<4, false> {
    typedef Bed::BTAllRestNoPool::Bed4Type BType;
  };

  template <>
  struct SelectBED<5, false> {
    typedef Bed::BTAllRestNoPool::Bed5Type BType;
  };

  //==============
  // named_pipe()
  //==============
  bool named_pipe(const std::string& fname) {
    bool is_namedpipe = false;
    if ( !fname.empty() && fname != "-" ) {
      struct stat st;
      if ( stat(fname.c_str(), &st) == -1 )
        throw(Ext::InvalidFile("Error: stat() failed on: " + fname));
      is_namedpipe = (S_ISFIFO(st.st_mode) != 0);
    }
    return is_namedpipe;
  }

  //======================
  // checkStarchNesting()
  //======================
  bool checkStarchNesting(const std::string& f1, const std::string& f2) {
    constexpr bool NoUseMemPool = false;
    typedef SelectBED<3, NoUseMemPool>::BType BedType;

    if ( f1 == "-" || f2 == "-" || named_pipe(f1) || named_pipe(f2) )
      return true; // not applicable
    Ext::FPWrap<Ext::InvalidFile> file1(f1);
    Bed::allocate_iterator_starch_bed_mm<BedType*> a(file1);
    bool rtn = a.has_nested();
    if ( !rtn && f2 != "" ) {
      Ext::FPWrap<Ext::InvalidFile> file2(f2);
      Bed::allocate_iterator_starch_bed_mm<BedType*> b(file2);
      rtn = b.has_nested();
    }
    return rtn;
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
                 bool nestCheck,
                 const std::string& colSep,
                 const std::string& multivalColSep,
                 const std::string& onEmptyMap,
                 int precision,
                 bool useScientific,
                 const std::string& chrom,
                 bool skipUnmappedRows,
                 bool sweepAll,
                 const std::vector<std::string>& visitorNames,
                 const std::vector< std::vector<std::string> >& visitorArgs) {

    // minRefFields must be <= minMapFields
    Ext::Assert<Ext::ProgramError>(minRefFields <= minMapFields,
                                   "BedMap::callSweep() minimum fields program error detected");

    constexpr bool UseMemPool = true;
    constexpr bool NoUseMemPool = !UseMemPool;
    if ( minMapFields < 4 ) { // just need Bed3
      if ( !BedMap::minimumMemory ) {
        typedef typename SelectBED<3, UseMemPool>::BType RefType;
        typedef RefType MapType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 3> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                            ProcessMode, sweepAll, colSep, chrom, skipUnmappedRows, visitorGroup);
      } else { // v2p4p26 and earlier mode
        typedef typename SelectBED<3, NoUseMemPool>::BType RefType;
        typedef RefType MapType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 3> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                            ProcessMode, sweepAll, colSep, chrom, skipUnmappedRows, visitorGroup);
      }
    } else if ( minMapFields < 5 ) { // just need Bed4 for Map and Bed3 for Ref
      if ( !BedMap::minimumMemory ) {
        Ext::Assert<Ext::ProgramError>(minRefFields < minMapFields,
                                       "BedMap::callSweep()-2 minimum fields program error detected");
        typedef typename SelectBED<3, UseMemPool>::BType RefType;
        typedef typename SelectBED<4, UseMemPool>::BType MapType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 4> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                            ProcessMode, sweepAll, colSep, chrom, skipUnmappedRows, visitorGroup);
      } else { // v2p4p26 and earlier mode
        Ext::Assert<Ext::ProgramError>(minRefFields < minMapFields,
                                       "BedMap::callSweep()-2 minimum fields program error detected");
        typedef typename SelectBED<3, NoUseMemPool>::BType RefType;
        typedef typename SelectBED<4, NoUseMemPool>::BType MapType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 4> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                            ProcessMode, sweepAll, colSep, chrom, skipUnmappedRows, visitorGroup);
      }
    } else { // need Bed5 for Map and Bed3 for Ref
      if ( !BedMap::minimumMemory ) {
        Ext::Assert<Ext::ProgramError>(minRefFields == 3,
                                       "BedMap::callSweep()-2 minimum fields program error detected");
        typedef typename SelectBED<3, UseMemPool>::BType RefType;
        typedef typename SelectBED<5, UseMemPool>::BType MapType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 5> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                            ProcessMode, sweepAll, colSep, chrom, skipUnmappedRows, visitorGroup);
      } else { // v2p4p26 and earlier mode
        Ext::Assert<Ext::ProgramError>(minRefFields == 3,
                                       "BedMap::callSweep()-2 minimum fields program error detected");
        typedef typename SelectBED<3, NoUseMemPool>::BType RefType;
        typedef typename SelectBED<5, NoUseMemPool>::BType MapType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, MapType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 5> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, mapFileName, errorCheck, nestCheck,
                            ProcessMode, sweepAll, colSep, chrom, skipUnmappedRows, visitorGroup);
      }
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
                 bool nestCheck,
                 const std::string& colSep,
                 const std::string& multivalColSep,
                 const std::string& onEmptyMap,
                 int precision,
                 bool useScientific,
                 const std::string& chrom,
                 bool skipUnmappedRows,
                 const std::vector<std::string>& visitorNames,
                 const std::vector< std::vector<std::string> >& visitorArgs) {

    constexpr bool UseMemPool = true;
    constexpr bool NoUseMemPool = !UseMemPool;
    if ( minRefFields < 4 ) { // just need Bed3
      if ( !BedMap::minimumMemory ) {
        typedef typename SelectBED<3, UseMemPool>::BType RefType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 3> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                            ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
      } else { // v2p4p26 and earlier mode
        typedef typename SelectBED<3, NoUseMemPool>::BType RefType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 3> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                            ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
      }
    } else if ( minRefFields < 5 ) { // need Bed4
      if ( !BedMap::minimumMemory ) {
        typedef typename SelectBED<4, UseMemPool>::BType RefType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 4> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                            ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
      } else { // v2p4p26 and earlier mode
        typedef typename SelectBED<4, NoUseMemPool>::BType RefType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 4> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                            ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
      }
    } else { // need Bed5
      if ( !BedMap::minimumMemory ) {
        typedef typename SelectBED<5, UseMemPool>::BType RefType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 5> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                            ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
      } else { // v2p4p26 and earlier mode
        typedef typename SelectBED<5, NoUseMemPool>::BType RefType;
        typedef typename SelectBase<ProcessMode, BedDistType, RefType, RefType>::BaseClass BaseClass;
        BedMap::GenerateVisitors<BaseClass, 5> gv;
        std::vector<BaseClass*> visitorGroup = getVisitors(gv, dt, multivalColSep, onEmptyMap, precision,
                                                           useScientific, visitorNames, visitorArgs);
        runSweep<BaseClass>(st, dt, refFileName, errorCheck, nestCheck,
                            ProcessMode, colSep, chrom, skipUnmappedRows, visitorGroup);
      }
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
                   bool nestCheck,
                   const std::string& colSep,
                   const std::string& multivalColSep,
                   const std::string& onEmptyMap,
                   int precision,
                   bool useScientific,
                   bool fastMode,
                   bool sweepAll,
                   const std::string& chrom,
                   bool skipUnmappedRows,
                   const std::vector<std::string>& visitorNames,
                   const std::vector< std::vector<std::string> >& visitorArgs) {

    const bool SpecialMode = true;
    const bool GeneralMode = false;
    if ( fastMode && !st.Symmetric ) // symmetry is a neccessary (and sufficient if no nested elements)
      throw(Ext::ProgramError("Symmetric distance required for fast mode!"));

    if ( mapFileName.empty() ) { // single-file mode
      if ( fastMode )
        callSweep<SpecialMode>(st, dt, refFileName, minRefFields, errorCheck, nestCheck, colSep, multivalColSep, onEmptyMap,
                               precision, useScientific, chrom, skipUnmappedRows, visitorNames, visitorArgs);
      else
        callSweep<GeneralMode>(st, dt, refFileName, minRefFields, errorCheck, nestCheck, colSep, multivalColSep, onEmptyMap,
                               precision, useScientific, chrom, skipUnmappedRows, visitorNames, visitorArgs);
    } else { // dual-file mode
      if ( fastMode )
        callSweep<SpecialMode>(st, dt, refFileName, mapFileName, minRefFields, minMapFields, errorCheck,
                               nestCheck, colSep, multivalColSep, onEmptyMap, precision, useScientific,
                               chrom, skipUnmappedRows, sweepAll, visitorNames, visitorArgs);
      else
        callSweep<GeneralMode>(st, dt, refFileName, mapFileName, minRefFields, minMapFields, errorCheck,
                               nestCheck, colSep, multivalColSep, onEmptyMap, precision, useScientific,
                               chrom, skipUnmappedRows, sweepAll, visitorNames, visitorArgs);
    }
  }

} // namespace BedMap
