/*
  Author: Shane Neph & Scott Kuehn
  Date:   Thu Aug 23 17:42:22 PDT 2007
*/

//
//    BEDOPS
//    Copyright (C) 2011-2015 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef CLASS_WINDOW_EXTREME_VISITOR_BED_H
#define CLASS_WINDOW_EXTREME_VISITOR_BED_H

#include <iostream>
#include <set>
#include <string>

#include "data/measurement/NaN.hpp"
#include "data/bed/BedCompare.hpp"

namespace Visitors {

  namespace BedSpecific {

    /*
      CompType requires value + address comparisons; we must use a std::set<>
       not a std::multiset<>.  Use a version of the templated
       CompValueThenAddressLesser or a similar idea.
    */
    template <
              typename Process,
              typename BaseVisitor,
              typename CompType = Bed::CompValueThenAddressLesser
             >
    struct Extreme : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef Process ProcessType;
      typedef typename BaseVisitor::RefType RefType;
      typedef typename BaseVisitor::MapType MapType;

      explicit Extreme(const ProcessType& pt = ProcessType(), std::size_t col = 5)
        : pt_(pt), col_(col), m_(CompType(col)) { /* */ }

      inline void Add(MapType* bt) {
        m_.insert(bt);
      }

      inline void Delete(MapType* bt) {
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
      std::size_t col_;
      std::set<MapType*, CompType> m_;
    };

  } // namespace BedSpecific

} // namespace Visitors


#endif // CLASS_WINDOW_EXTREME_VISITOR_BED_H
