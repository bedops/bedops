/*
  FILE: AverageVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Thu Aug 23 17:42:22 PDT 2007
  PROJECT: windowing-visitors
  ID: $Id:$
*/

// Macro Guard
#ifndef CLASS_WINDOW_AVERAGE_VISITOR_H
#define CLASS_WINDOW_AVERAGE_VISITOR_H


// Files included
#include "data/measurement/NaN.hpp"


namespace Visitors {


  //=========
  // Average
  //=========
  template <
            typename Process,
            typename BaseVisitor
           >
  struct Average : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::reference_type T1;
    typedef typename BaseClass::mapping_type T2;

    explicit Average(const ProcessType& pt = ProcessType())
        : pt_(pt), sum_(0), counter_(0)
      { /* */ }

    inline void Add(T2* bt) {
      sum_ += *bt;
      ++counter_;
    }

    inline void Delete(T2* bt) {
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
    double sum_;
    int counter_;
  };


} // namespace Visitors

#endif // CLASS_WINDOW_AVERAGE_VISITOR_H
