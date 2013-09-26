/*
  FILE: ExtremeVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Thu Aug 23 17:42:22 PDT 2007
  PROJECT: windowing-visitors
  ID: $Id:$
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
#ifndef CLASS_WINDOW_EXTREME_VISITOR_H
#define CLASS_WINDOW_EXTREME_VISITOR_H

// Files included
#include <iostream>
#include <set>
#include <string>

// Files included
#include "data/measurement/NaN.hpp"
#include "utility/OrderCompare.hpp"


namespace Visitors {

  //=========
  // Extreme
  //=========

  /*
    CompType requires value + address comparisons; we must use a std::set<>
     not a std::multiset<>.  Use a version of the templated
     CompValueThenAddress<> or a similar idea.
  */
  template <
            typename Process,
            typename BaseVisitor,
            typename CompType = Ordering::CompValueThenAddressLesser<
                                                                     typename BaseVisitor::mapping_type,
                                                                     typename BaseVisitor::mapping_type
                                                                    >
           >
  struct Extreme : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseVisitor::reference_type T1;
    typedef typename BaseVisitor::mapping_type T2;

    explicit Extreme(const ProcessType& pt = ProcessType()) : pt_(pt) { /* */ }

    inline void Add(T2* bt) {
      m_.insert(bt);
    }

    inline void Delete(T2* bt) {
      m_.erase(bt);
    }

    inline void DoneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( !m_.empty() ) {
        pt_.operator()(*m_.begin());
      }
      else
        pt_.operator()(nan);
    }

    virtual ~Extreme() { /* */ }

  protected:
    ProcessType pt_;
    std::set<T2*, CompType> m_;
  };

} // namespace Visitors


#endif // CLASS_WINDOW_EXTREME_VISITOR_H
