/*
  Author: Shane Neph & Scott Kuehn
  Date:   Thu Aug 23 17:42:22 PDT 2007
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

#ifndef CLASS_WINDOW_EXTREME_VISITOR_H
#define CLASS_WINDOW_EXTREME_VISITOR_H

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <set>
#include <string>
#include <type_traits>

#include "data/measurement/NaN.hpp"
#include "data/measurement/SelectMeasureType.hpp"
#include "utility/OrderCompare.hpp"

namespace Visitors {

  /*
    CompType requires value + address comparisons; we must use a std::set<>
     not a std::multiset<>.  Use a version of the templated
     CompValueThenAddress<> or a similar idea.
  */
  struct DoNothing {};
  struct RandTie {
    RandTie() {
      std::srand(std::time(NULL));
    }

    template <typename T, typename C>
    T* breakTie(const std::set<T*, C>& s) {
      // s is not empty if Extreme is calling
      std::vector<T*> toRand;
      bool first = true;
      typename Signal::SelectMeasure<T>::MeasureType best = 0;
      for ( auto iter = s.begin(); iter != s.end(); ++iter ) {
        if ( first ) {
          best = **iter;
          toRand.push_back(*iter);
          first = false;
        } else if ( **iter == best ) {
          toRand.push_back(*iter);
        } else {
          break;
        }
      } // for
      if ( toRand.size() == 1 )
        return toRand.back();
      std::random_shuffle(toRand.begin(), toRand.end());
      return toRand.back();
    }
  };

  template <
            typename Process,
            typename BaseVisitor,
            typename CompType = Ordering::CompValueThenAddressLesser<
                                                                     typename BaseVisitor::mapping_type,
                                                                     typename BaseVisitor::mapping_type
                                                                    >,
            typename OnTies = DoNothing
           >
  struct Extreme : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseVisitor::RefType RefType;
    typedef typename BaseVisitor::MapType MapType;

    explicit Extreme(const ProcessType& pt = ProcessType()) : pt_(pt) { /* */ }

    inline void Add(MapType* bt) {
      m_.insert(bt);
    }

    inline void Delete(MapType* bt) {
      m_.erase(bt);
    }

    void DoneReference() {
      doneReference();
    }

    virtual ~Extreme() { /* */ }

  private:

    template <typename OT=OnTies>
    inline typename std::enable_if<!std::is_same<OT, DoNothing>::value>::type
    doneReference() {
      static const Signal::NaN nan = Signal::NaN();
      static OnTies onTies;
      if ( !m_.empty() )
        pt_.operator()(onTies.breakTie(m_));
      else
        pt_.operator()(nan);
    }


    template <typename OT=OnTies>
    inline typename std::enable_if<std::is_same<OT, DoNothing>::value>::type
    doneReference() {
      static const Signal::NaN nan = Signal::NaN();
      if ( !m_.empty() )
        pt_.operator()(*m_.begin());
      else
        pt_.operator()(nan);
    }

  protected:
    ProcessType pt_;
    std::set<MapType*, CompType> m_;
  };

} // namespace Visitors


#endif // CLASS_WINDOW_EXTREME_VISITOR_H
