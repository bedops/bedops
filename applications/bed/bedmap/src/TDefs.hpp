/*
  Author: Scott Kuehn, Shane Neph
  Date:   Fri Oct 19 08:20:50 PDT 2007
*/
//
//    BEDOPS
//    Copyright (C) 2011-2016 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef _BEDMAP_TYPEDEFS_HPP
#define _BEDMAP_TYPEDEFS_HPP

#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "utility/Exception.hpp"

namespace BedMap {

  //==============
  // VisitorTypes
  //==============
  template <typename BaseClass>
  struct VisitorTypes {

    typedef Visitors::BedHelpers::Print ProcessAll;
    typedef Visitors::BedHelpers::PrintAllScorePrecision ProcessOne;
    typedef Visitors::BedHelpers::PrintID ProcessID;
    typedef Visitors::BedHelpers::PrintLength ProcessLength;
    typedef Visitors::Helpers::Print ProcessIntegerLength;
    typedef Visitors::BedHelpers::PrintScore ProcessScore;
    typedef Visitors::BedHelpers::PrintSpanName ProcessSpanName;
    typedef Visitors::BedHelpers::PrintScorePrecision ProcessScorePrecision;
    typedef Visitors::BedHelpers::PrintBED3 ProcessBED3;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessAll> ProcessRangeDelimAll;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessID> ProcessRangeDelimID;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessLength> ProcessRangeDelimLength;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessIntegerLength> ProcessRangeDelimIntersectLength;
    typedef Visitors::BedHelpers::PrintUniqueRangeIDs ProcessUniqueDelimID;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessScore> ProcessRangeDelimScore;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessScorePrecision> ProcessRangeDelimScorePrecision;
    typedef Visitors::BedHelpers::PrintGenomicRange<ProcessBED3> ProcessMapGenomicRange;

    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
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
    typedef Visitors::TrimmedMean<ProcessScorePrecision, BaseClass, Ext::ArgumentError> TMean;
    typedef Visitors::Variance<ProcessScorePrecision, BaseClass> Variance;

    typedef Visitors::Extreme<ProcessOne, BaseClass, MaxOrder> MaxElement;
    typedef Visitors::Extreme<ProcessOne, BaseClass, MinOrder> MinElement;

    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimAll, BaseClass> EchoMapAll;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimID, BaseClass> EchoMapID;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimLength, BaseClass> EchoMapLength;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessMapGenomicRange, BaseClass> EchoMapRange;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimScorePrecision, BaseClass> EchoMapScore;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessUniqueDelimID, BaseClass> EchoMapUniqueID;
    typedef Visitors::BedSpecific::EchoMapIntersectLength<ProcessRangeDelimIntersectLength, BaseClass> EchoMapIntersectLength;
    typedef Visitors::Echo<ProcessAll, BaseClass> EchoRefAll;
    typedef Visitors::Echo<ProcessLength, BaseClass> EchoRefLength;
    typedef Visitors::Echo<ProcessSpanName, BaseClass> EchoRefSpan;
    typedef Visitors::BedSpecific::OvrAggregate<ProcessScore, BaseClass> OvrAgg;
    typedef Visitors::BedSpecific::OvrUnique<ProcessScore, BaseClass> OvrUniq;
    typedef Visitors::BedSpecific::OvrUniqueFract<ProcessScorePrecision, BaseClass> OvrUniqFract;

  };

} // namespace BedMap

#endif // _BEDMAP_TYPEDEFS_HPP
