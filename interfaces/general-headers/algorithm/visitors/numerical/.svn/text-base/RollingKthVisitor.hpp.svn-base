/*
  FILE: RollingKthVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Mon Aug 20 14:22:38 PDT 2007
  PROJECT: windowing visitors
  ID: $Id$
*/


// Macro Guard
#ifndef ROLLING_KTH_VISITOR_HPP
#define ROLLING_KTH_VISITOR_HPP


// Files included
#include <cmath>
#include <cstdlib>
#include <set>
#include <string>

// Files included
#include "data/measurement/AssayMeasurement.hpp"
#include "data/measurement/NaN.hpp"
#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"
#include "utility/OrderCompare.hpp"


namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor,
            typename ExceptionType = Ext::ArgumentError
           >
  struct RollingKth : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::reference_type T1;
    typedef typename BaseClass::mapping_type T2;
    typedef T2* PtrType;

    //==============
    // Construction
    //==============
    explicit RollingKth(double kth = 0.8, const ProcessType& pt = ProcessType())
        : kthValue_(kth), currentAtPos_(0), pt_(pt) {
      currentMarker_ = scoresBuf_.end();
      Ext::Assert<ExceptionType>(kth >= 0 && kth <= 1, "Expect 0 <= kth <= 1");
    }

    //======================================
    // Windowing Phase : Add() and Delete()
    //======================================
    inline void Add(PtrType ptr) {
      static Comp comp;
      scoresBuf_.insert(ptr);
      if ( currentMarker_ == scoresBuf_.end() ) {
        currentMarker_ = scoresBuf_.begin();
        currentAtPos_ = 0;
      }
      else if ( comp(ptr, *currentMarker_) ) // ptr < currentMarker_
        ++currentAtPos_;
    }

    inline void Delete(PtrType toRemove) {
      // keep in mind that you cannot be here if there is <= 1 element
      //  in the scoresBuf_ containers.
      static Comp comp;
      if ( comp(toRemove, *currentMarker_) ) // toRemove < currentMarker_
        --currentAtPos_;
      else if ( toRemove == *currentMarker_ ) { // removing currentMarker_
        if ( currentMarker_ != scoresBuf_.begin() ) {
          --currentMarker_;
          --currentAtPos_;
        }
        else {
          ++currentMarker_;
            // currentAtPos_ remains the same
        }
      }
      scoresBuf_.erase(toRemove);
    }

    //====================================
    // Repositioning and Reporting Phases
    //====================================
    inline void DoneReference() {
      // guaranteed: 0 < kthValue <= 1
      typedef typename Signal::AssayMeasurement<T2>::value_type VT;
      std::size_t size = scoresBuf_.size();
      std::size_t kthPos = static_cast<std::size_t>(iround(kthValue_ * size));
      if ( kthPos > 0 ) // make zero based
        --kthPos;

      while ( kthPos > currentAtPos_ ) { // need to increment currentMarker_
        ++currentMarker_;
        ++currentAtPos_;
      } // while incrementing

      while ( kthPos < currentAtPos_ ) { // need to decrement currentMarker_
        currentMarker_--;
        --currentAtPos_;
      } // while decrementing

      if ( size )
        pt_.operator()(static_cast<VT>(**currentMarker_));
      else {
        static const Signal::NaN nan = Signal::NaN();
        pt_.operator()(nan);
      }
    }

    //===========
    // Cleanup()
    //===========
    virtual ~RollingKth()
      { /* */ }

  protected:
    typedef Ordering::CompValueThenAddressLesser<T2, T2> Comp;
    typedef std::set<PtrType, Comp> ScoreTypeContainer;

  protected:
    double kthValue_;
    std::size_t currentAtPos_;
    ProcessType pt_;
    ScoreTypeContainer scoresBuf_;
    typename ScoreTypeContainer::iterator currentMarker_;

  protected:
    inline double iround(double d) {
      double d1 = std::ceil(d);
      return( (d >= 0) ?
              ((d1-d > 0.5) ? std::floor(d) : d1)
                       :
              ((d1-d >= 0.5) ? std::floor(d) : d1)
            );
    }
  };

} // namespace Visitors

#endif // ROLLING_KTH_VISITOR_HPP
