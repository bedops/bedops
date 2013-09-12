/*
  FILE: StdevVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Oct  3 09:25:51 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

// Macro Guard
#ifndef STDEVVISITOR_HPP
#define STDEVVISITOR_HPP

// Files included
#include <cmath>

#include "data/measurement/NaN.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor
           >
  struct StdDev : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::reference_type RefType;
    typedef typename BaseClass::mapping_type MapType;

    explicit StdDev(const ProcessType& pt = ProcessType())
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
        double numer = (count_ * squareSum_) - (sum_ * sum_);
        double denom = (count_ * (count_ - 1));
        double val = std::sqrt(numer / denom);
        pt_.operator()(val);
      }
    }

    virtual ~StdDev() { /* */ }

  protected:
    ProcessType pt_;
    double sum_;
    double squareSum_;
    double count_;
  };

} // namespace Visitors

#endif // STDEVVISITOR_HPP
