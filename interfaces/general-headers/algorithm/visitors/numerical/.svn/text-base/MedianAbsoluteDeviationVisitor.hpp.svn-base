/*
  FILE: MedianAbsoluteVisitor.hpp
  AUTHOR: Sean Thomas, Shane Neph
  CREATE DATE: Wed Aug 18 23:28:17 PDT 2010
  PROJECT: utility
  ID: $Id$
*/

// Macro Guard
#ifndef _MAD_VISITOR_HPP
#define _MAD_VISITOR_HPP

// Files included
#include <algorithm>
#include <cmath>
#include <functional>
#include <set>
#include <vector>

#include "algorithm/visitors/numerical/RollingKthAverageVisitor.hpp"
#include "data/measurement/AssayMeasurement.hpp"
#include "data/measurement/NaN.hpp"
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
             Helpers::Keep<typename Signal::AssayMeasurement<typename BaseVisitor::mapping_type>::value_type>,
             BaseVisitor, ExceptionType> {

    typedef typename Signal::AssayMeasurement<typename BaseVisitor::mapping_type>::value_type VT;
    typedef RollingKthAverage<Helpers::Keep<VT>, BaseVisitor, ExceptionType> BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::reference_type RefType;
    typedef typename BaseClass::mapping_type MapType;


    // Using default multiplier == 1 // see wikipedia
    explicit MedianAbsoluteDeviation(const ProcessType& pt = ProcessType(), double mult = 1)
       : BaseClass(0.5, Helpers::Keep<VT>()), pt_(pt), mult_(mult)
      { /* */ }


    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( BaseClass::scoresBuf_.size() <= 1 ) {
        pt_.operator()(nan);
        return;
      }

      BaseClass::DoneReference();
      VT median = BaseClass::pt_.value_;
      if ( BaseClass::pt_.isNan_ ) {
        pt_.operator()(nan);
        return;
      }

      std::vector<VT> vec;
      typename std::vector<VT>::iterator n = vec.begin();
      typedef typename BaseClass::ScoreTypeContainer::iterator IterType;
      for ( IterType i = BaseClass::scoresBuf_.begin(); i != BaseClass::scoresBuf_.end(); ++i )
        vec.push_back(static_cast<VT>(**i));

      std::transform(vec.begin(), vec.end(), vec.begin(), std::bind2nd(details::abs_diff<VT>(), median));
      std::size_t sz = vec.size();
      VT mad = 0;
      if ( sz % 2 == 0 ) {
        n = vec.begin() + static_cast<std::size_t>(sz / 2.0 - 1);
        std::nth_element(vec.begin(), n, vec.end());
        mad = static_cast<VT>(*n);
        n = vec.begin() + static_cast<std::size_t>(sz / 2.0);
        std::nth_element(vec.begin(), n, vec.end());
        mad += static_cast<VT>(*n);
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
    double mult_;
  };

} // namespace Visitors

#endif // _MAD_VISITOR_HPP
