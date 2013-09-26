/*
  FILE: MedianVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Mon Aug  9 18:59:15 PDT 2010
  PROJECT: windowing visitors
  ID: $Id$
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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

// Macro Guard
#ifndef ROLLING_MEDIAN_VISITOR_HPP
#define ROLLING_MEDIAN_VISITOR_HPP


// Files included
#include <cmath>
#include <cstdlib>
#include <set>
#include <string>

// Files included
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
    typedef typename BaseClass::T1 T1;
    typedef typename BaseClass::T2 T2;
    typedef T2* PtrType;

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
