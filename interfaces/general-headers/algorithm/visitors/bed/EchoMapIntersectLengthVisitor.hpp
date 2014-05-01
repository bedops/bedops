/*
  FILE: EchoMapIntersectLengthVisitor.hpp
  AUTHOR: Shane Neph
  CREATE DATE: Mon Dec  9 16:07:08 PST 2013
  PROJECT: utility
  ID: $Id$
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013, 2014 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef INTERSECT_REF_VISITOR_HPP
#define INTERSECT_REF_VISITOR_HPP

#include <set>

#include "data/bed/Bed.hpp"
#include "data/bed/BedCompare.hpp"

namespace Visitors {

    namespace BedSpecific {
  
    // Collect the total number of overlapping positions
  
    template <
              typename ProcessType,
              typename BaseVisitor
             >
    struct EchoMapIntersectLength : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef typename BaseClass::reference_type T;
      typedef typename BaseClass::mapping_type V;
  
      EchoMapIntersectLength(const ProcessType& pt = ProcessType()) : refItem_(0), pt_(pt)
        { /* */ }
  
      inline void SetReference(T* t) { 
        refItem_ = t; 
      }
  
      inline void Delete(V* v) {
        win_.erase(v);
      }
  
      inline void Add(V* v) {
        win_.insert(v);
      }
  
      inline void DoneReference() {
        std::vector<long> vec;
        typename SType::const_iterator i = win_.begin();
        while ( i != win_.end() ) {
          T c = *refItem_;
          c.intersection(**i++);
          vec.push_back(c.length());
        } // while
        pt_.operator()(vec.begin(), vec.end());
      }
  
      virtual ~EchoMapIntersectLength() { }
  
    protected:
      typedef Bed::GenomicAddressCompare<V, V> Comp;
      typedef std::set<V*, Comp> SType;
  
      T* refItem_;
      ProcessType pt_;
      SType win_;
    };
  
  } // namespace BedSpecific

} // namespace Visitors

#endif // INTERSECT_REF_VISITOR_HPP
