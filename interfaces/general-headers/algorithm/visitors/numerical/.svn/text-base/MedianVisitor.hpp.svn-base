/*
  FILE: MedianVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Mon Aug  9 18:59:15 PDT 2010
  PROJECT: windowing visitors
  ID: $Id$
*/


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
