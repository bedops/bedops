/*
  Author: Scott Kuehn, Shane Neph
  Date:   Thu Nov  8 13:31:09 PST 2007
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

#ifndef ECHO_MAP_BED_VISITOR_HPP
#define ECHO_MAP_BED_VISITOR_HPP

#include <set>

#include "data/bed/BedCompare.hpp"

namespace Visitors {

  namespace BedSpecific { // use other/EchoMapVisitor.hpp for numeric types

    template <
              typename Process,
              typename BaseVisitor
             >
    struct EchoMapBed : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef Process ProcessType;
      typedef typename BaseVisitor::RefType RefType;
      typedef typename BaseVisitor::MapType MapType;

      explicit EchoMapBed(const ProcessType& pt = ProcessType()) : pt_(pt)
        { /* */ }

      inline void Add(MapType* t) {
        win_.insert(t);
      }

      inline void Delete(MapType* t) {
        win_.erase(t);
      }

      inline void DoneReference() {
        pt_.operator()(win_.begin(), win_.end());
      }

      virtual ~EchoMapBed() { }

    private:
      typedef std::set< MapType*, Bed::GenomicRestAddressCompare<MapType, MapType> > SType; // consistent w/sort-bed
      ProcessType pt_;
      SType win_;
    };

  } // namespace BedSpecific

} // namespace Visitors


#endif // ECHO_MAP_BED_VISITOR_HPP
