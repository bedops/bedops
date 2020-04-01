/*
  Author: Shane Neph & Scott Kuehn
  Date:   Mon Aug 20 14:22:38 PDT 2007
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

#ifndef ROLLING_KTH_VISITOR_AVERAGE_HPP
#define ROLLING_KTH_VISITOR_AVERAGE_HPP

#include <cmath>
#include <cstdlib>
#include <set>
#include <string>

#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "algorithm/visitors/numerical/RollingKthVisitor.hpp"
#include "data/measurement/NaN.hpp"
#include "data/measurement/SelectMeasureType.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor,
            typename ExceptionType = Ext::ArgumentError
           >
  struct RollingKthAverage : RollingKth<Visitors::Helpers::DoNothing, BaseVisitor, ExceptionType> {

    typedef RollingKth<Visitors::Helpers::DoNothing, BaseVisitor, ExceptionType> BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef MapType* PtrType;

    //==============
    // Construction
    //==============
    explicit RollingKthAverage(double kth = 0.8, const ProcessType& pt = ProcessType())
        : BaseClass(kth, Visitors::Helpers::DoNothing()), pt_(pt) { /* */ }

    //====================================
    // Repositioning and Reporting Phases
    //====================================
    inline void DoneReference() {
      BaseClass::DoneReference();

      // The calculations below are based upon suggestions from wikipedia.
      //   They are different from the base class' implementation
      typedef typename Signal::SelectMeasure<MapType>::MeasureType MT;
      std::size_t size = BaseClass::scoresBuf_.size();
      std::size_t kthPosUp = static_cast<std::size_t>(std::ceil(static_cast<double>(BaseClass::kthValue_ * size)));
      std::size_t kthPosDown = static_cast<std::size_t>(std::floor(static_cast<double>(BaseClass::kthValue_ * size)));
      if ( kthPosUp > 0 ) // make zero-based
        --kthPosUp;
      if ( kthPosDown > 0 ) // make zero-based
        --kthPosDown;

      if ( size > 1 ) {
        if ( kthPosUp == kthPosDown ) { // a true integer; take average of two adjacent integers
          MT one = **BaseClass::currentMarker_;
          typename BaseClass::ScoreTypeContainer::iterator next = BaseClass::currentMarker_;
          MT two = **++next;
          pt_.operator()((one + two)/2.0);
        } else if ( BaseClass::currentAtPos_ == kthPosUp ) {
          pt_.operator()(static_cast<MT>(**BaseClass::currentMarker_));
        } else { // BaseClass::currentAtPos_ == kthPosDown; round up to kthPosUp per wikipedia
          typename BaseClass::ScoreTypeContainer::iterator next = BaseClass::currentMarker_;
          pt_.operator()(static_cast<MT>(**++next));
        }
      } else if ( 1 == size ) {
        pt_.operator()(static_cast<MT>(**BaseClass::currentMarker_));
      } else {
        static const Signal::NaN nan = Signal::NaN();
        pt_.operator()(nan);
      }
    }

    //===========
    // Cleanup()
    //===========
    virtual ~RollingKthAverage()
      { /* */ }

  protected:
    typedef Ordering::CompValueThenAddressLesser<MapType, MapType> Comp;
    typedef std::set<PtrType, Comp> ScoreTypeContainer;

  protected:
    ProcessType pt_;
  };

} // namespace Visitors

#endif // ROLLING_KTH_VISITOR_AVERAGE_HPP
