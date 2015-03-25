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

#ifndef _BEDMAP_TYPEDEFS_HPP
#define _BEDMAP_TYPEDEFS_HPP

#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "data/bed/BedCompare.hpp"
#include "utility/Exception.hpp"

namespace BedMap {

  //==============
  // VisitorTypes
  //==============
  template <typename BaseClass>
  struct VisitorTypes {
    typedef Visitors::BedHelpers::Print ProcessAll;
    typedef Visitors::BedHelpers::Print ProcessOne;
    typedef Visitors::BedHelpers::PrintText ProcessText;
    typedef Visitors::BedHelpers::PrintLength ProcessLength;
    typedef Visitors::Helpers::Print ProcessIntegerLength;
    typedef Visitors::BedHelpers::PrintScore ProcessScore;
    typedef Visitors::BedHelpers::PrintSpanName ProcessSpanName;
    typedef Visitors::BedHelpers::PrintScorePrecision ProcessScorePrecision;
    typedef Visitors::BedHelpers::PrintColumnScorePrecision ProcessColumnScorePrecision;
    typedef Visitors::BedHelpers::PrintBED3 ProcessBED3;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessAll> ProcessRangeDelimAll;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessText> ProcessRangeDelimText;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessLength> ProcessRangeDelimLength;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessIntegerLength> ProcessRangeDelimIntersectLength;
    typedef Visitors::BedHelpers::PrintUniqueRangeText ProcessUniqueDelimText;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessScore> ProcessRangeDelimScore;
    typedef Visitors::Helpers::PrintRangeDelim<ProcessColumnScorePrecision> ProcessRangeDelimScorePrecision;
    typedef Visitors::BedHelpers::PrintGenomicRange<ProcessBED3> ProcessMapGenomicRange;

    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef Bed::CompValueThenAddressGreater MaxOrder;
    typedef Bed::CompValueThenAddressLesser MinOrder;

    typedef Visitors::BedSpecific::Average<ProcessScorePrecision, BaseClass> Average;
    typedef Visitors::BedSpecific::CoeffVariation<ProcessScorePrecision, BaseClass> CoeffVariation;
    typedef Visitors::Count<ProcessScore, BaseClass> Count;
    typedef Visitors::BedSpecific::RollingKthAverage<ProcessScorePrecision, BaseClass, Ext::ArgumentError> KthAverage;
    typedef Visitors::BedSpecific::Extreme<ProcessColumnScorePrecision, BaseClass, MaxOrder> Max;
    typedef Visitors::Indicator<ProcessScore, BaseClass> Indicator;
    typedef Visitors::BedSpecific::Median<ProcessScorePrecision, BaseClass> Median;
    typedef Visitors::BedSpecific::MedianAbsoluteDeviation<ProcessScorePrecision, BaseClass> MedianAbsoluteDeviation;
    typedef Visitors::BedSpecific::Extreme<ProcessColumnScorePrecision, BaseClass, MinOrder> Min;
    typedef Visitors::BedSpecific::StdDev<ProcessScorePrecision, BaseClass> StdDev;
    typedef Visitors::BedSpecific::Sum<ProcessScorePrecision, BaseClass> Sum;
    typedef Visitors::BedSpecific::TrimmedMean<ProcessColumnScorePrecision, BaseClass, Ext::ArgumentError> TMean;
    typedef Visitors::BedSpecific::Variance<ProcessScorePrecision, BaseClass> Variance;

    typedef Visitors::BedSpecific::Extreme<ProcessOne, BaseClass, MaxOrder> MaxElement;
    typedef Visitors::BedSpecific::Extreme<ProcessOne, BaseClass, MinOrder> MinElement;

    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimAll, BaseClass> EchoMapAll;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimText, BaseClass> EchoMapText;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimLength, BaseClass> EchoMapLength;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessMapGenomicRange, BaseClass> EchoMapRange;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessRangeDelimScorePrecision, BaseClass> EchoMapScore;
    typedef Visitors::BedSpecific::EchoMapBed<ProcessUniqueDelimText, BaseClass> EchoMapTextUnique;
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
