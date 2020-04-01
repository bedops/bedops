/*
  Author: Scott Kuehn, Shane Neph
  Date:   Thu Sep 27 16:47:55 PDT 2007
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

#ifndef VARIANCEVISITOR_HPP
#define VARIANCEVISITOR_HPP

#include "data/measurement/NaN.hpp"
#include "data/measurement/SelectMeasureType.hpp"

namespace Visitors {
  
  template <
            typename Process,
            typename BaseVisitor
           >
  struct Variance : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef typename Signal::SelectMeasure<MapType>::MeasureType MT;

    explicit Variance(const ProcessType& pt = ProcessType())
       : pt_(pt), sum_(0), squareSum_(0), count_(0)
      { /* */ }

    inline void Add(MapType* bt) {
      sum_ += *bt; 
      squareSum_ += (*bt * *bt);
      count_++;
    }

    inline void Delete(MapType* bt) {
      sum_ -= *bt; 
      squareSum_ -= (*bt * *bt);
      count_--;
    }

    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( count_ <= 1 )
        pt_.operator()(nan);
      else {
        MT numer = (count_ * squareSum_) - (sum_ * sum_);
        MT denom = (count_ * (count_ - 1));
        pt_.operator()(numer / denom);
      }
    }

    virtual ~Variance() { /* */ }

  protected:
    ProcessType pt_;
    MT sum_;
    MT squareSum_;
    long count_;
  };

} // namespace Visitors

#endif // VARIANCEVISITOR_HPP
