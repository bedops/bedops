/*
  FILE: Input.cpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Fri Oct 19 08:20:50 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef _BEDMAP_TYPEDEFS_HPP
#define _BEDMAP_TYPEDEFS_HPP

// Files included
#include "algorithm/visitors/bed/BedBaseVisitor.hpp"
#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
#include "utility/Exception.hpp"



namespace BedMap {

  //==============
  // VisitorTypes
  //==============
  template <typename BaseClass>
  struct VisitorTypes {

    typedef Visitors::BedHelpers::Print ProcessAll;
    typedef Visitors::BedHelpers::Print ProcessOne;
    typedef Visitors::BedHelpers::PrintID ProcessID;
    typedef Visitors::BedHelpers::PrintScore ProcessScore;
    typedef Visitors::BedHelpers::PrintScorePrecision ProcessScorePrecision;
    typedef Visitors::BedHelpers::PrintBED3 ProcessBED3;
    typedef Visitors::BedHelpers::PrintSortedRangeDelim<ProcessAll> ProcessRangeDelimAll;
    typedef Visitors::BedHelpers::PrintSortedRangeDelim<ProcessID> ProcessRangeDelimID;
    typedef Visitors::BedHelpers::PrintUniqueRangeIDs ProcessUniqueDelimID;
    typedef Visitors::BedHelpers::PrintSortedRangeDelim<ProcessScore> ProcessRangeDelimScore;
    typedef Visitors::BedHelpers::PrintSortedRangeDelim<ProcessScorePrecision> ProcessRangeDelimScorePrecision;
    typedef Visitors::BedHelpers::PrintGenomicRange<ProcessBED3> ProcessMapGenomicRange;

    typedef typename BaseClass::mapping_type MapType;
    typedef Ordering::CompValueThenAddressGreater<MapType, MapType> MaxOrder;
    typedef Ordering::CompValueThenAddressLesser<MapType, MapType> MinOrder;

    typedef Visitors::Average<ProcessScorePrecision, BaseClass> Average;
    typedef Visitors::CoeffVariation<ProcessScorePrecision, BaseClass> CoeffVariation;
    typedef Visitors::Count<ProcessScore, BaseClass> Count;
    typedef Visitors::RollingKthAverage<ProcessScorePrecision, BaseClass, Ext::ArgumentError> KthAverage;
    typedef Visitors::Extreme<ProcessScorePrecision, BaseClass, MaxOrder> Max;
    typedef Visitors::Indicator<ProcessScore, BaseClass> Indicator;
    typedef Visitors::Median<ProcessScorePrecision, BaseClass> Median;
    typedef Visitors::MedianAbsoluteDeviation<ProcessScorePrecision, BaseClass> MedianAbsoluteDeviation;
    typedef Visitors::Extreme<ProcessScorePrecision, BaseClass, MinOrder> Min;
    typedef Visitors::StdDev<ProcessScorePrecision, BaseClass> StdDev;
    typedef Visitors::Sum<ProcessScorePrecision, BaseClass> Sum;
    typedef Visitors::TrimmedMean<ProcessScorePrecision, BaseClass, Ext::ArgumentError> TMeans;
    typedef Visitors::Variance<ProcessScorePrecision, BaseClass> Variance;

    typedef Visitors::Extreme<ProcessOne, BaseClass, MaxOrder> MaxElement;
    typedef Visitors::Extreme<ProcessOne, BaseClass, MinOrder> MinElement;

    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimAll, BaseClass> EchoMapAll;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimID, BaseClass> EchoMapID;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessUniqueDelimID, BaseClass> EchoMapUniqueID;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessMapGenomicRange, BaseClass> EchoMapRange;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimScorePrecision, BaseClass> EchoMapScore;
    typedef Visitors::Echo<ProcessAll, BaseClass> EchoRefAll;
    typedef Visitors::BedSpecific::OvrAggregate<ProcessScore, BaseClass> OvrAgg;
    typedef Visitors::BedSpecific::OvrUnique<ProcessScore, BaseClass> OvrUniq;
    typedef Visitors::BedSpecific::OvrUniqueFract<ProcessScorePrecision, BaseClass> OvrUniqFract;

  };

} // namespace BedMap

#endif // _BEDMAP_TYPEDEFS_HPP
