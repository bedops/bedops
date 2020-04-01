/*
  Author: Shane Neph & Scott Kuehn
  Date:   Sun Dec 13 23:50:58 PST 2009
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

#ifndef _NAMED_VISITORS_HPP
#define _NAMED_VISITORS_HPP

#include <sstream>
#include <string>
#include <vector>

#include "algorithm/visitors/BedVisitors.hpp"
#include "algorithm/visitors/NumericalVisitors.hpp"
#include "algorithm/visitors/OtherVisitors.hpp"
#include "algorithm/visitors/helpers/ProcessBedVisitorRow.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "data/bed/BedCompare.hpp"
#include "utility/OrderCompare.hpp"

// Names returned via VisitorName<...>::Name() must be unique
//  to every other specialization here.  Compiler cannot catch.

namespace Visitors {

  namespace Helpers {

    // default undefined:
    //  compiler can help catch unimplemented details
    template <typename T>
    struct VisitorName;

    template <typename A, typename B>
    struct VisitorName< Visitors::Count<A,B> > {
      static std::string Name()
        { return "count"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::CoeffVariation<A,B> > {
      static std::string Name()
        { return "cv"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::Echo<A,B> > {
      static std::string Name()
        { return "echo"; }
    };

    template <typename B>
    struct VisitorName< Visitors::Echo<Visitors::BedHelpers::PrintLength,B> > {
      static std::string Name()
        { return "echo-ref-size"; }
    };

    template <typename B>
    struct VisitorName< Visitors::Echo<Visitors::BedHelpers::PrintSpanName,B> > {
      static std::string Name()
        { return "echo-ref-name"; }
    };

    template <typename B>
    struct VisitorName< Visitors::Echo<Visitors::BedHelpers::PrintRowID, B> > {
      static std::string Name()
        { return "echo-ref-row-id"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::Indicator<A,B> > {
      static std::string Name()
        { return "indicator"; }
    };

    template <typename A, typename B, typename C>
    struct VisitorName< Visitors::RollingKthAverage<A,B,C> > {
      static std::string Name()
        { return "kth"; }
    };

    template <typename A, typename B, typename C>
    struct VisitorName< Visitors::MedianAbsoluteDeviation<A,B,C> > {
      static std::string Name()
        { return "mad"; }
    };

    template <typename A, typename B, typename C, typename D>
    struct VisitorName< Visitors::Extreme< A,B,Ordering::CompValueThenAddressGreater<C,C>,D > > {
      static std::string Name()
        { return "max"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::Average<A,B> > {
      static std::string Name()
        { return "mean"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::Median<A,B> > {
      static std::string Name()
        { return "median"; }
    };

    template <typename A, typename B, typename C, typename D>
    struct VisitorName< Visitors::Extreme< A,B,Ordering::CompValueThenAddressLesser<C, C>,D > > {
      static std::string Name()
        { return "min"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::StdDev<A,B> > {
      static std::string Name()
        { return "stdev"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::Sum<A,B> > {
      static std::string Name()
        { return "sum"; }
    };

    template <typename A, typename B, typename C>
    struct VisitorName< Visitors::TrimmedMean<A,B,C> > {
      static std::string Name()
        { return "tmean"; }
    };

    template <typename A, typename B, typename C>
    struct VisitorName< Visitors::RollingKth<A,B,C> > {
      static std::string Name()
        { return "value-at"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::Variance<A,B> > {
      static std::string Name()
        { return "variance"; }
    };


    // BED
    template <typename A, typename B>
    struct VisitorName< Visitors::BedSpecific::OvrAggregate<A,B> > {
      static std::string Name()
        { return "bases"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::BedSpecific::OvrUnique<A,B> > {
      static std::string Name()
        { return "bases-uniq"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::BedSpecific::OvrUniqueFract<A,B> > {
      static std::string Name()
        { return "bases-uniq-f"; }
    };

    template <template<class X> class A, class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapBed<A<Visitors::BedHelpers::Print>,B> > {
      static std::string Name()
        { return "echo-map"; }
    };

    template <template<class X> class A, class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapBed<A<Visitors::BedHelpers::PrintID>,B> > {
      static std::string Name()
        { return "echo-map-id"; }
    };

    template <class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapBed<Visitors::BedHelpers::PrintUniqueRangeIDs, B> > {
      static std::string Name()
        { return "echo-map-id-uniq"; }
    };

    template <class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapBed< Visitors::BedHelpers::PrintGenomicRange<Visitors::BedHelpers::PrintBED3>, B> > {
      static std::string Name()
        { return "echo-map-range"; }
    };

    template <template<class X> class A, class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapBed< A<Visitors::BedHelpers::PrintScorePrecision>, B> > {
      static std::string Name()
        { return "echo-map-score"; }
    };

    template <template<class X> class A, class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapBed<A<Visitors::BedHelpers::PrintLength>,B> > {
      static std::string Name()
        { return "echo-map-size"; }
    };

    template <class A, class B>
    struct VisitorName< Visitors::BedSpecific::EchoMapIntersectLength<A, B> > {
      static std::string Name()
        { return "echo-overlap-size"; }
    };

    template <typename B, typename C, typename D>
    struct VisitorName< Visitors::Extreme< Visitors::BedHelpers::PrintAllScorePrecision,B,Bed::ScoreThenGenomicCompareGreater<C,C>, D > > {
      static std::string Name()
        { return "max-element"; }
    };

    template <typename B, typename C, typename D>
    struct VisitorName< Visitors::Extreme< Visitors::BedHelpers::PrintAllScorePrecision,B,Ordering::CompValueThenAddressGreater<C,C>, D > > {
      static std::string Name()
        { return "max-element-rand"; }
    };

    template <typename B, typename C, typename D>
    struct VisitorName< Visitors::Extreme< Visitors::BedHelpers::PrintAllScorePrecision,B,Bed::ScoreThenGenomicCompareLesser<C,C>, D > > {
      static std::string Name()
        { return "min-element"; }
    };

    template <typename B, typename C, typename D>
    struct VisitorName< Visitors::Extreme< Visitors::BedHelpers::PrintAllScorePrecision,B,Ordering::CompValueThenAddressLesser<C, C>, D > > {
      static std::string Name()
        { return "min-element-rand"; }
    };

    template <typename A, typename B>
    struct VisitorName< Visitors::WeightedAverage<A,B> > {
      static std::string Name()
        { return "wmean"; }
    };

  } // namespace Helpers

} // Visitors

#endif // _NAMED_VISITORS_HPP
