/*
  Author: Scott Kuehn, Shane Neph
  Date:   Wed Sep  5 09:40:33 PDT 2007
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

#ifndef OVR_AGGREGATE_VISITOR_HPP
#define OVR_AGGREGATE_VISITOR_HPP

#include <set>

#include "data/bed/BedCompare.hpp"

namespace Visitors {

    namespace BedSpecific {
  
    // Collect the total number of overlapping positions
  
    template <
              typename ProcessType,
              typename BaseVisitor
             >
    struct OvrAggregate : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef typename BaseClass::RefType RefType;
      typedef typename BaseClass::MapType MapType;
  
      OvrAggregate(const ProcessType& pt = ProcessType()) : ovr_(0), refItem_(0), pt_(pt)
        { /* */ }
  
      inline void SetReference(RefType* t) { 
        refItem_ = t; 
        ovr_ = 0;
        if ( !cache_.empty() ) {
          for ( cacheI i = cache_.begin(); i != cache_.end(); ++i )
            ovr_ += coordCompare(refItem_, *i);
        }
      }
  
      inline void Delete(MapType* v) {
        cacheI iter = cache_.find(v);
        if ( iter != cache_.end() )
          ovr_ -= coordCompare(refItem_, v);
        cache_.erase(v);
      }
  
      inline void Add(MapType* v) {
        cache_.insert(v);
        ovr_ += coordCompare(refItem_, v);
      }
  
      inline void DoneReference() {
        pt_.operator()(ovr_);
      }
  
      virtual ~OvrAggregate() { }
  
     protected:
      inline unsigned long coordCompare(RefType* t, MapType* v) {
        if ( t->start() >= v->start() ) {
          if ( v->end() > t->start() ) {
            if( v->end() > t->end() )
              return (t->end() - t->start());
            else
              return (v->end() - t->start());
          }
          else
            return 0;
        } else { // t->start() < v->start()
          if ( t->end() > v->start() ) {
            if ( v->end() < t->end() )
              return (v->end() - v->start());
            else
              return (t->end() - v->start());
          }
          else
            return 0;
        }         
      }
  
    protected:
      typedef Bed::GenomicRestAddressCompare<MapType, MapType> Comp;
      typedef std::set<MapType*, Comp> SType;
      typedef typename SType::const_iterator cacheI;
  
      unsigned long ovr_;
      RefType* refItem_;
      ProcessType pt_;
      SType cache_;
    };
  
  } // namespace BedSpecific

} // namespace Visitors

#endif // OVR_AGGREGATE_VISITOR_HPP
