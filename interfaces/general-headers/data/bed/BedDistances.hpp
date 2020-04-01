/*
  Author: Shane Neph & Scott Kuehn
  Date:   Tue Aug 14 15:50:16 PDT 2007
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

#ifndef BED_DISTANCE_FUNCTIONS_H
#define BED_DISTANCE_FUNCTIONS_H

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "suite/BEDOPS.Constants.hpp"

namespace Bed {

  //============
  // RangedDist
  //============
  struct RangedDist {
    enum { PercentBased = false, Symmetric = true, BP = !PercentBased, Identical = false, Ranged = true };

    explicit RangedDist(CoordType maxDist = 0)
      : maxDist_(maxDist) { /* */ }

    template <typename T1, typename T2>
    inline int Map2Ref(T1 const* t1, T2 const* t2) const
      { return(this->operator()(t1, t2)); }

    template <typename T2, typename T1>
    inline int Ref2Map(T2 const* t2, T1 const* t1) const
      { return(this->operator()(t2, t1)); }

    /* report 0 if within maxDist_; -1 if 'a' "<" 'b'; 1 otherwise */
    template <typename BedType1, typename BedType2>
    inline int operator()(BedType1 const* a, BedType2 const* b) const {
      static int v = 0;
      if ( (v = std::strcmp(a->chrom(), b->chrom())) != 0 )
        return((v > 0) ? 1 : -1);
      else if ( a->start() < b->end() )
        return((a->end() + maxDist_ > b->start()) ? 0 : -1);
      return((b->end() + maxDist_ > a->start()) ? 0 : 1);
    }

    CoordType maxDist_;
  };

  //=================
  // AsymmRangedDist
  //=================
  /* this does not work well with sweep() because of asymmetry - use bedops --range L:R
       functionality to achieve what you need.
  struct AsymmRangedDist { };
  */

  //=============
  // Overlapping
  //=============
  struct Overlapping {
    enum { PercentBased = false, Symmetric = true, BP = !PercentBased, Identical = false, Ranged = false };

    explicit Overlapping(CoordType ovrRequired = 0)
      : ovrRequired_(ovrRequired) { /* */ }

    template <typename T1, typename T2>
    inline int Ref2Map(T1 const* t1, T2 const* t2) const
      { return(this->operator()(t1, t2)); }

    template <typename T2, typename T1>
    inline int Map2Ref(T2 const* t2, T1 const* t1) const
      { return(this->operator()(t2, t1)); }

    /* report 0 if overlapping "ovrRequired";
        -1 if a "<" b, and +1 otherwise */
    template <typename BedType1, typename BedType2>
    inline int operator()(BedType1 const* a, BedType2 const* b) const {
      static int v = 0;
      if ( (v = std::strcmp(a->chrom(), b->chrom())) != 0 )
        return ((v > 0) ? 1 : -1);
      CoordType mn = std::max(a->start(), b->start());
      CoordType mx = std::min(a->end(), b->end());
      if ( mx > mn ) { // some overlap
        if ( mx-mn >= ovrRequired_ )
          return(0);
        if ( a->start() != b->start() )
          return((a->start() < b->start()) ? -1 : 1);
        if ( a->end() != b->end() )
          return((a->end() < b->end()) ? -1 : 1);
        CoordType aidx = static_cast<CoordType>(a - static_cast<BedType1 const*>(0));
        CoordType bidx = static_cast<CoordType>(b - static_cast<BedType2 const*>(0));
        return((aidx < bidx) ? -1 : 1); // a==b but still overlap < ovrRequired
      }
      return((a->start() < b->start()) ? -1 : 1); // no overlap
    }

    CoordType ovrRequired_;
  };

  //========================
  // PercentOverlapMapping : look at % overlap relative to mapType's size
  //========================
  struct PercentOverlapMapping {
    enum { PercentBased = true, Symmetric = false, BP = !PercentBased, Identical = false, Ranged = false };

    explicit PercentOverlapMapping(double perc = 1.0)
      : perc_(perc) {

      while ( perc_ > 1 ) // ie; assume 40 was suppose to be 0.4 = 40%
        perc_ /= 10.0;

      perc_ -= std::numeric_limits<double>::epsilon(); // deal with small diffs

      if ( perc_ <= 0.0 )
        perc_ = std::numeric_limits<double>::epsilon();
    }

    // report 0 if mapType overlaps refType by perc_ or more (relative to
    //   mapType's length).
    // report -1 if refType "<" mapType, otherwise report 1
    template <typename T1, typename T2>
    inline int Ref2Map(T1 const* refType, T2 const* mapType) const {

      static int v = 0;
      static double sz = 0, totalLength = 0;
      static int direction = 0;

      // check if no overlap first
      if ( (v = std::strcmp(refType->chrom(), mapType->chrom())) != 0 )
        return((v > 0) ? 1 : -1);
      else if ( refType->end() < mapType->start() )
        return(-1);
      else if ( mapType->end() < refType->start() )
        return(1);

      // overlap exists
      if ( perc_ <= std::numeric_limits<double>::epsilon() )
        return(0);

      totalLength = mapType->end() - mapType->start();
      if ( refType->start() <= mapType->start() ) {
        if ( refType->end() >= mapType->end() )
          sz = mapType->end() - mapType->start();
        else
          sz = refType->end() - mapType->start();
        direction = -1;
      }
      else { // refType->start() > mapType->start()
        if ( refType->end() >= mapType->end() )
          sz = mapType->end() - refType->start();
        else
          sz = refType->end() - refType->start();
        direction = 1;
      }

      if ( sz / totalLength >= perc_ )
        return(0);
      return(direction);
    }

    // report 0 if mapType overlaps refType by perc_ or more (relative to mapType's length)
    //  else, report -1 if mapType "<" refType or 1 otherwise
    template <typename T2, typename T1>
    inline int Map2Ref(T2 const* mapType, T1 const* refType) const {
      return(-Ref2Map(refType, mapType));
    }

  private:
    double perc_;
  };

  //==========================
  // PercentOverlapReference: looking at % overlap relative to refType's size
  //   (think bedops -e)
  //==========================
  struct PercentOverlapReference : public PercentOverlapMapping {
    enum { PercentBased = true, Symmetric = false, BP = !PercentBased, Identical = false, Ranged = false };

    explicit PercentOverlapReference(double perc = 1.0)
      : BaseClass(perc) { }

    // report 0 if refType overlaps mapType by perc or more (relative to refType's length)
    //  else, report -1 if refType "<" mapType or 1 otherwise
    template <typename T1, typename T2>
    inline int Ref2Map(T1 const* refType, T2 const* mapType) const {
      return(-BaseClass::Ref2Map(mapType, refType));
    }

    // report 0 if refType overlaps mapType by perc or more (relative to refType's length)
    //  else, report -1 if mapType "<" refType or 1 othersise
    template <typename T1, typename T2>
    inline int Map2Ref(T1 const* mapType, T2 const* refType) const {
      return(-Ref2Map(refType, mapType));
    }

  protected:
    typedef PercentOverlapMapping BaseClass;
  };

  //==========================
  // PercentOverlapEither: looking at % overlap relative to refType's OR mapType's size
  //==========================
  struct PercentOverlapEither : public PercentOverlapMapping {
    enum { PercentBased = true, Symmetric = false, BP = !PercentBased, Identical = false, Ranged = false };

    explicit PercentOverlapEither(double perc = 1.0)
      : BaseClass(perc) { }

    // report 0 if refType overlaps mapType by perc or more (relative to refType's length)
    //     OR mapType overlaps refType by perc or more (relative to mapType's length)
    //  else, report -1 if refType "<" mapType or 1 otherwise
    template <typename T1, typename T2>
    inline int Ref2Map(T1 const* refType, T2 const* mapType) const {
      int val1 = BaseClass::Ref2Map(refType, mapType);
      if ( 0 == val1 )
        return(val1);
      int val2 = -1 * BaseClass::Ref2Map(mapType, refType);
      if ( 0 == val2 )
        return(val2);
      return(val1);
    }

    // report 0 if refType overlaps mapType by perc or more (relative to refType's length)
    //     OR mapType overlaps refType by perc or more (relative to mapType's length)
    //  else, report -1 if mapType "<" refType or 1 othersise
    template <typename T1, typename T2>
    inline int Map2Ref(T1 const* mapType, T2 const* refType) const {
      return(-Ref2Map(refType, mapType));
    }

  protected:
    typedef PercentOverlapMapping BaseClass;
  };

  //==========================
  // PercentOverlapBoth: looking at % overlap relative to refType's AND mapType's size
  //==========================
  struct PercentOverlapBoth : public PercentOverlapMapping {
    enum { PercentBased = true, Symmetric = true, BP = !PercentBased, Identical = false, Ranged = false };

    explicit PercentOverlapBoth(double perc = 1.0)
      : BaseClass(perc) { }

    // report 0 if refType overlaps mapType by perc or more (relative to refType's length)
    //     AND mapType overlaps refType by perc or more (relative to mapType's length)
    //  else, report -1 if refType "<" mapType or 1 otherwise
    template <typename T1, typename T2>
    inline int Ref2Map(T1 const* refType, T2 const* mapType) const {
      int val1 = BaseClass::Ref2Map(refType, mapType);
      if ( 0 != val1 )
        return(val1);
      int val2 = -BaseClass::Ref2Map(mapType, refType);
      if ( 0 != val2 )
        return(val2);
      return(0);
    }

    // report 0 if refType overlaps mapType by perc or more (relative to refType's length)
    //     AND mapType overlaps refType by perc or more (relative to mapType's length)
    //  else, report -1 if mapType "<" refType or 1 othersise
    template <typename T1, typename T2>
    inline int Map2Ref(T1 const* mapType, T2 const* refType) const {
      return(-Ref2Map(refType, mapType));
    }

  protected:
    typedef PercentOverlapMapping BaseClass;
  };

  //==============================
  // Exact: what the name implies
  //==============================
  struct Exact {
    enum { PercentBased = false, Symmetric = true, BP = !PercentBased, Identical = true, Ranged = false };

    Exact() {}

    // report 0 if exact.
    //  else, report -1 if refType "<" mapType or 1 otherwise
    template <typename T1, typename T2>
    inline int Ref2Map(T1 const* refType, T2 const* mapType) const {
      int v = 0;
      if ( (v = std::strcmp(refType->chrom(), mapType->chrom())) != 0 )
        return v < 0 ? -1 : 1;
      else if ( refType->start() != mapType->start() )
        return refType->start() < mapType->start() ? -1 : 1;
      else if ( refType->end() != mapType->end() )
        return refType->end() < mapType->end() ? -1 : 1;
      return 0;
    }

    // invert Ref2Map's result with args switched
    template <typename T1, typename T2>
    inline int Map2Ref(T1 const* mapType, T2 const* refType) const {
      return -Ref2Map(refType, mapType);
    }
  };

} // namespace Bed

#endif // BED_DISTANCE_FUNCTIONS_H
