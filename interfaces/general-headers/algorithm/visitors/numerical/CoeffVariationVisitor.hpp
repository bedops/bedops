/*
  FILE: CoeffVariation.hpp
  AUTHOR: Shane Neph, Scott Kuehn
  CREATE DATE: Tue Sep  7 22:52:08 PDT 2010
  PROJECT: utility
  ID: $Id$
*/

// Macro Guard
#ifndef _COEFF_VARIATION_HPP
#define _COEFF_VARIATION_HPP

// Files included
#include <cmath>

#include "data/measurement/NaN.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor
           >
  struct CoeffVariation : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::reference_type RefType;
    typedef typename BaseClass::mapping_type MapType;

    explicit CoeffVariation(const ProcessType& pt = ProcessType())
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
        double stdev = std::sqrt(numer / denom);
        double mean = (sum_ / count_);
        if ( mean == 0 )
          pt_.operator()(nan);
        else
          pt_.operator()(stdev/mean);
      }
    }

    virtual ~CoeffVariation() { /* */ }

  protected:
    ProcessType pt_;
    double sum_;
    double squareSum_;
    double count_;
  };

} // namespace Visitors

#endif // _COEFF_VARIATION_HPP
