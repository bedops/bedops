/*
  Author: Shane Neph, Sean Thomas
  Date:   Wed Aug 18 23:28:17 PDT 2010
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

#ifndef _MAD_VISITOR_HPP
#define _MAD_VISITOR_HPP

#include <algorithm>
#include <cmath>
#include <functional>
#include <set>
#include <vector>

#include "algorithm/visitors/numerical/RollingKthAverageVisitor.hpp"
#include "data/measurement/NaN.hpp"
#include "data/measurement/SelectMeasureType.hpp"
#include "utility/Exception.hpp"

namespace Visitors {

  namespace details {

    template <typename T>
    struct abs_diff : std::binary_function<T, T, T> {
      T operator()(const T& t1, const T& t2) const {
        T diff = t1 - t2;
        if ( diff < 0 )
          return(-diff);
        return(diff);
      }
    };

  } // details

  template <
            typename Process,
            typename BaseVisitor,
            typename ExceptionType = Ext::ArgumentError
           >
  struct MedianAbsoluteDeviation : RollingKthAverage<
             Helpers::Keep<typename Signal::SelectMeasure<typename BaseVisitor::MapType>::MeasureType>,
             BaseVisitor, ExceptionType> {

    typedef typename Signal::SelectMeasure<typename BaseVisitor::MapType>::MeasureType MT;
    typedef RollingKthAverage<Helpers::Keep<MT>, BaseVisitor, ExceptionType> BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;

    // Using default multiplier == 1 // see wikipedia
    explicit MedianAbsoluteDeviation(const ProcessType& pt = ProcessType(), double mult = 1)
       : BaseClass(0.5, Helpers::Keep<MT>()), pt_(pt), mult_(mult)
      { /* */ }

    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( BaseClass::scoresBuf_.size() <= 1 ) {
        pt_.operator()(nan);
        return;
      }

      BaseClass::DoneReference();
      MT median = BaseClass::pt_.value_;
      if ( BaseClass::pt_.isNan_ ) {
        pt_.operator()(nan);
        return;
      }

      std::vector<MT> vec;
      typename std::vector<MT>::iterator n = vec.begin();
      typedef typename BaseClass::ScoreTypeContainer::iterator IterType;
      for ( IterType i = BaseClass::scoresBuf_.begin(); i != BaseClass::scoresBuf_.end(); ++i )
        vec.push_back(static_cast<MT>(**i));

      std::transform(vec.begin(), vec.end(), vec.begin(), std::bind2nd(details::abs_diff<MT>(), median));
      std::size_t sz = vec.size();
      MT mad = 0;
      if ( sz % 2 == 0 ) {
        n = vec.begin() + static_cast<std::size_t>(sz / 2.0 - 1);
        std::nth_element(vec.begin(), n, vec.end());
        mad = static_cast<MT>(*n);
        n = vec.begin() + static_cast<std::size_t>(sz / 2.0);
        std::nth_element(vec.begin(), n, vec.end());
        mad += static_cast<MT>(*n);
        mad /= 2.0;
      } else {
        n = vec.begin() + static_cast<std::size_t>(std::floor(sz / 2.0));
        std::nth_element(vec.begin(), n, vec.end());
        mad = *n;
      }
      pt_.operator()(mad * mult_);
    }

    virtual ~MedianAbsoluteDeviation() { /* */ }

  protected:
    ProcessType pt_;
    const double mult_;
  };

} // namespace Visitors

#endif // _MAD_VISITOR_HPP
