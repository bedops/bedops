/*
  FILE: WeightMean1.hpp
  AUTHOR: Shane Neph
  CREATE DATE: Thu Apr 24 17:01:32 PDT 2014
  PROJECT: utility
  ID: $Id$
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

#ifndef WEIGHTED_AVG_FRACTION_REF_HPP
#define WEIGHTED_AVG_FRACTION_REF_HPP

#include <set>

#include "data/measurement/NaN.hpp"


namespace Visitors {

  namespace BedSpecific {

    template <
              typename ProcessType,
              typename BaseVisitor
             >
    struct WeightedMean1 : BaseVisitor {
      typedef BaseVisitor BaseClass;
      typedef typename BaseClass::reference_type RefType;
      typedef typename BaseClass::mapping_type MapType;
      typedef typename Signal::SelectMeasure<MapType>::MeasureType MT;

      WeightedMean1(const ProcessType& pt = ProcessType()) : pt_(pt), refItem_(0)
        { /* */ }

      inline void SetReference(RefType* t) {
        refItem_ = t;
      }

      // Append an element comprised of the coords overlapping target and reference
      inline void Add(MapType* u) {
        cache_.insert(u);
      }

      inline void Delete(MapType* u) {
        cache_.erase(u);
      }

      // Weighted mean using fraction of reference element overlapped per mapping element
      inline void DoneReference() {
        if ( !cache_.empty() ) {
          // if --range is used, we don't count the extra bps (like --bases*)
          //  Use bedops --range for those types of changes.
          MT val = 0;
          bool any = false;
          for ( auto i = cache_.begin(); i != cache_.end(); ++i ) {
            MT ovr = static_cast<MT>((*i)->intersection(*refItem_).length());
            if ( ovr > 0 ) {
              any = true;
              val += (ovr * static_cast<MT>(**i));
            }
          } // for
          if ( any )
            pt_.operator()(val/refItem_->length());
          else
            pt_.operator()(Signal::NaN());
        } else {
          pt_.operator()(Signal::NaN());
        }
      }

      virtual ~WeightedMean1() { /* */ }

    protected:
      typedef std::set<MapType*> SType;

      ProcessType pt_;
      RefType* refItem_;
      SType cache_;
    };

  } // namespace BedSpecific

} // namespace Visitors

#endif // WEIGHTED_AVG_FRACTION_REF_HPP
