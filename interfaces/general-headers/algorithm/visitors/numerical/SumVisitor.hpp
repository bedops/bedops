/*
  Author: Shane Neph & Scott Kuehn
  Date:   Mon Aug 27 10:17:36 PDT 2007
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

#ifndef CLASS_WINDOW_SUM_VISITOR_H
#define CLASS_WINDOW_SUM_VISITOR_H

#include "data/measurement/SelectMeasureType.hpp"
#include "data/measurement/NaN.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor
           >
  struct Sum : public BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;

    explicit Sum(const ProcessType& pt = ProcessType())
        : pt_(pt), sum_(0), counter_(0)
      { /* */ }

    inline void Add(MapType* bt)
      { sum_ += *bt; ++counter_; }

    inline void Delete(MapType* bt)
      { sum_ -= *bt; --counter_; }

    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( 0 < counter_ )
        pt_.operator()(sum_);
      else
        pt_.operator()(nan);
    }

    virtual ~Sum()
      { /* */ }

  protected:
    ProcessType pt_;
    typename Signal::SelectMeasure<MapType>::MeasureType sum_;
    int counter_;
  };

} // namespace Visitors

#endif // CLASS_WINDOW_SUM_VISITOR_H
