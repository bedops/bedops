/*
  Author: Shane Neph
  Date:   Jan.2017
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

#ifndef CLASS_WINDOW_WEIGHTED_AVERAGE_VISITOR_H
#define CLASS_WINDOW_WEIGHTED_AVERAGE_VISITOR_H

#include <set>

#include "data/measurement/NaN.hpp"

namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor
           >
  struct WeightedAverage : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;

    explicit WeightedAverage(const ProcessType& pt = ProcessType())
        : pt_(pt)
      { /* */ }

    inline void Add(MapType* bt) {
      m_.insert(bt);
    }

    inline void Delete(MapType* bt) {
      m_.erase(bt);
    }

    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( !m_.empty() ) {
        double value = 0;
        double weightSum = 0;
        for ( auto ptr : m_ ) {
          auto v = r_->overlap(*ptr)/static_cast<double>(r_->length());
          value +=  v * (*ptr);
          weightSum += v;
        } // for
        value /= weightSum;
        pt_.operator()(value);
      } else {
        pt_.operator()(nan);
      }
    }

    inline void End() {
      m_.clear();
      r_ = nullptr;
    }

    inline void SetReference(RefType* r) { r_ = r; }

    virtual ~WeightedAverage()
      { /* */ }

  protected:
    ProcessType pt_;
    RefType* r_;
    std::set<MapType*> m_;
  };

} // namespace Visitors

#endif // CLASS_WINDOW_WEIGHTED_AVERAGE_VISITOR_H
