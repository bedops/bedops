/*
  Author: Shane Neph & Scott Kuehn
  Date:   Mon Aug 20 14:22:38 PDT 2007
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

#ifndef TMEANS_VISITOR_HPP
#define TMEANS_VISITOR_HPP

#include <cmath>
#include <cstdlib>
#include <limits>
#include <set>
#include <string>

#include "data/measurement/NaN.hpp"
#include "data/measurement/SelectMeasureType.hpp"
#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"
#include "utility/OrderCompare.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor,
            typename ExceptionType = Ext::ArgumentError
           >
  struct TrimmedMean : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef MapType* PtrType;
    typedef Ordering::CompValueThenAddressLesser<MapType, MapType> Comp;
    typedef std::set<PtrType, Comp> ScoreTypeContainer;
    typedef typename Signal::SelectMeasure<MapType>::MeasureType MT;

    //==============
    // Construction
    //==============
    explicit TrimmedMean(double lowerKth=0.2, double upperKth = 0.8, const ProcessType& pt = ProcessType())
        : lowerKth_(lowerKth), upperKth_(upperKth), lowerSum_(0), upperSum_(0), currentAtPosLower_(0),
          currentAtPosUpper_(0), doKth_(false), symmetric_(false), pt_(pt) {
      currentMarkerLower_ = scoresBuf_.end(), currentMarkerUpper_ = scoresBuf_.end();
      Ext::Assert<ExceptionType>(lowerKth_ >= 0 && lowerKth_ <= 1, "Expect 0 <= lowerKth <= 1");
      Ext::Assert<ExceptionType>(upperKth_ >= 0 && upperKth_ <= 1, "Expect 0 <= upperKth <= 1");
      const double epsilon = std::numeric_limits<double>::epsilon();
      if ( std::abs(1.0-lowerKth-upperKth) <= epsilon )
        doKth_ = true;

      if ( std::abs(lowerKth_ - upperKth_) <= epsilon ) // symmetric: trim same # elements from both ends
        symmetric_ = true;
      Ext::Assert<ExceptionType>(lowerKth + upperKth <= 1+epsilon, "Expect lowerKth + upperKth <= 1");
    }

    //======================================
    // Windowing Phase : Add() and Delete()
    //======================================
    inline void Add(PtrType ptr) {
      scoresBuf_.insert(ptr);
      if ( lowerKth_ > 0 && !doKth_ )
        add(ptr, currentMarkerLower_, currentAtPosLower_, lowerSum_);
      add(ptr, currentMarkerUpper_, currentAtPosUpper_, upperSum_);
    }

    inline void Delete(PtrType toRemove) {
      // keep in mind that you cannot be here if there is <= 1 element
      //  in the scoresBuf_ containers.
      if ( lowerKth_ > 0 && !doKth_ )
        remove(toRemove, currentMarkerLower_, currentAtPosLower_, lowerSum_);
      remove(toRemove, currentMarkerUpper_, currentAtPosUpper_, upperSum_);
      scoresBuf_.erase(toRemove);
    }

    //====================================
    // Repositioning and Reporting Phases
    //====================================
    inline void DoneReference() {
      // currentMarkerLower_ marks the last element in the subsequence to be ignored
      //       (not one passed that, but on that)
      // currentMarkerUpper_ marks the last element to be included in the output
      //       (not one passed that, but on that)
      // output mean of sequence (..] formed by currentMarkerLower_ to currentMarkerUpper_
      //
      // if doKth_ is true, then we are just reporting the element pointed to by currentMarkerUpper_
      //   following its update.  This is really a kth usage.  Bob T. found certain math packages
      //   work this way, where, for example, tmean is called with 0.3 and 0.7.  The lower and upper
      //   markers end up pointing to the same element.  That element is not part of the sequence of
      //   interest for the lower marker, but is part of the sequence of interest for the upper marker.
      //   So, another way to get the kth element.

      // guaranteed: 0 <= kth-value <= 1
      if ( scoresBuf_.empty() ) {
        static const Signal::NaN nan = Signal::NaN();
        pt_.operator()(nan);
        return;
      }

      std::size_t size = scoresBuf_.size();
      std::size_t kthPosLow = static_cast<std::size_t>(iround(lowerKth_ * size));
      std::size_t kthPosHigh = static_cast<std::size_t>(iround(upperKth_ * size));
      kthPosHigh = size - kthPosHigh;
      if ( symmetric_ ) {
        kthPosLow = std::max(kthPosLow, size - kthPosHigh);
        kthPosHigh = size - kthPosLow;
      }
      bool doLow = kthPosLow > 0;

      if ( doLow )
        --kthPosLow; // make zero-based
      if ( kthPosHigh > 0 )
        --kthPosHigh; // make zero-based

      if ( !doKth_ && doLow )
        doneRef(currentMarkerLower_, currentAtPosLower_, lowerSum_, kthPosLow);
      doneRef(currentMarkerUpper_, currentAtPosUpper_, upperSum_, kthPosHigh);

      // Spit results
      if ( doKth_ || currentAtPosUpper_ == currentAtPosLower_ )
        pt_.operator()(*currentMarkerUpper_); // single element related to kth - Bob T. thinks this is best
      else if ( doLow )
        pt_.operator()((upperSum_ - lowerSum_)/(currentAtPosUpper_ - currentAtPosLower_));
       else
         pt_.operator()(upperSum_ / (currentAtPosUpper_ + 1));
    }

    //===========
    // Cleanup()
    //===========
    virtual ~TrimmedMean()
      { /* */ }


  protected:
    inline void add(PtrType ptr, typename ScoreTypeContainer::iterator& marker, std::size_t& pos, MT& sum) {
      static Comp comp;
      if ( marker == scoresBuf_.end() ) {
        marker = scoresBuf_.begin();
        pos = 0;
        sum = *ptr;
      } else if ( comp(ptr, *marker) ) { // ptr < marker
        ++pos;
        sum += *ptr;
      }
    }

    inline void remove(PtrType ptr, typename ScoreTypeContainer::iterator& marker, std::size_t& pos, MT& sum) {
      // keep in mind that you cannot be here if there is <= 1 element
      //  in the scoresBuf_ containers.
      static Comp comp;
      if ( comp(ptr, *marker) ) { // toRemove < marker
        --pos;
        sum -= *ptr;
      }
      else if ( ptr == *marker ) { // removing marker
        sum -= *ptr;
        if ( marker != scoresBuf_.begin() ) {
          --marker;
          --pos;
        } else {
          if ( ++marker != scoresBuf_.end() )
            sum += **marker;
          // pos remains the same
        }
      }
    }

    inline void doneRef(typename ScoreTypeContainer::iterator& marker, std::size_t& pos, MT& sum, std::size_t newPos) {

      // Increment markers as needed
      while ( newPos > pos ) {
        sum += **++marker;
        ++pos;
      } // while

      // Decrement markers as needed
      while ( newPos < pos ) {
        sum -= **marker--;
        --pos;
      } // while
    }

  protected:
    const double lowerKth_;
    const double upperKth_;
    MT lowerSum_, upperSum_;
    std::size_t currentAtPosLower_, currentAtPosUpper_;
    bool doKth_, symmetric_;
    ProcessType pt_;
    ScoreTypeContainer scoresBuf_;
    typename ScoreTypeContainer::iterator currentMarkerLower_, currentMarkerUpper_;

  private:
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

#endif // TMEANS_VISITOR_HPP
