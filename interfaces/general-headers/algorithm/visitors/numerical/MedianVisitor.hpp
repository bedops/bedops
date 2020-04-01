/*
  Author: Shane Neph & Scott Kuehn
  Date:   Mon Aug  9 18:59:15 PDT 2010
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

#ifndef ROLLING_MEDIAN_VISITOR_HPP
#define ROLLING_MEDIAN_VISITOR_HPP

#include <cmath>
#include <cstdlib>
#include <set>
#include <string>

#include "algorithm/visitors/numerical/RollingKthAverageVisitor.hpp"
#include "utility/Exception.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor,
            typename ExceptionType = Ext::ArgumentError
           >
  struct Median : RollingKthAverage<Process, BaseVisitor, ExceptionType> {

    typedef RollingKthAverage<Process, BaseVisitor, ExceptionType> BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef MapType* PtrType;

    //==============
    // Construction
    //==============
    explicit Median(const ProcessType& pt = ProcessType()) :
      BaseClass(0.5, pt) { /* */ }

    //===========
    // Cleanup()
    //===========
    virtual ~Median()
      { /* */ }
  };

} // namespace Visitors

#endif // ROLLING_MEDIAN_VISITOR_HPP
