/*
  Author: Shane Neph & Scott Kuehn
  Date:   Thu Aug 23 17:42:22 PDT 2007
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

#ifndef CLASS_WINDOW_AVERAGE_VISITOR_H
#define CLASS_WINDOW_AVERAGE_VISITOR_H

#include "data/measurement/NaN.hpp"
#include "data/measurement/SelectMeasureType.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor
           >
  struct Average : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;

    explicit Average(const ProcessType& pt = ProcessType())
        : pt_(pt), sum_(0), counter_(0)
      { /* */ }

    inline void Add(MapType* bt) {
      sum_ += *bt;
      ++counter_;
    }

    inline void Delete(MapType* bt) {
      sum_ -= *bt;
      --counter_;
    }

    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( counter_ > 0 )
        pt_.operator()(sum_/counter_);
      else
        pt_.operator()(nan);
    }

    inline void End() {
      sum_ = 0;
      counter_ = 0;
    }

    virtual ~Average()
      { /* */ }

  protected:
    ProcessType pt_;
    typename Signal::SelectMeasure<MapType>::MeasureType sum_;
    int counter_;
  };

} // namespace Visitors

#endif // CLASS_WINDOW_AVERAGE_VISITOR_H
