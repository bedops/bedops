/*
  Author: Shane Neph & Scott Kuehn
  Date:   Dec. 7, 2009
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

#ifndef _BED_BASE_VISITOR_OVERLAPPING_HPP
#define _BED_BASE_VISITOR_OVERLAPPING_HPP

#include <list>
#include <set>
#include <type_traits>

#include "algorithm/WindowSweep.hpp"
#include "data/bed/BedCompare.hpp"
#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"

/*
  See important notes in BedBaseVisitor.hpp
    Applicable part has to do with bp overlaps
*/

namespace Visitors {

  /*
    There are some optimizations that can happen if we're looking at
      straight overlaps.
  */
  template <typename BedDist, typename Ref, typename Map>
  struct BedBaseVisitor<BedDist, Ref, Map, typename std::enable_if<BedDist::BP && !BedDist::Identical>::type> {
     typedef BedDist DistType;
     typedef const Ref RefType;
     typedef const Map MapType;

  protected:
     // typedefs
     typedef std::set<MapType*, Bed::RevCoordAddressCompare<MapType>> EndLesser;
     typedef std::set<MapType*, Bed::CoordAddressCompare<MapType>> StartLesser;

  public:
     // Interface for sweep()
     inline bool ManagesOwnMemory() const { return(false); }

     inline void OnStart(RefType* t) {
       if ( t->length() < dist_.ovrRequired_ ) { // nothing qualifies
         nestedRef_ = true; // technicality
         cache_.insert(leftWin_.begin(), leftWin_.end());
         cache_.insert(rightWin_.begin(), rightWin_.end());
         auto i = win_.begin();
         while ( i != win_.end() )
           Delete(*i++);
         win_.clear();
         nestedRef_ = true;
       } else {
         static auto toDel;
         int val = 0;
         for ( auto i = leftWin_.begin(); i != leftWin_.end(); ++i ) {
           if ( (val = dist_.Map2Ref(*i, t)) < 0 ) { // forever gone
             toDel.insert(*i);
             leftWin_.erase(i);
           } else if ( 0 == val ) {
             break;
           }
         } // for
need tmpCache_ or put in cache_ and evaluate yet again
need to adjust leftWin_ and rightWin_ such that rightWin_ > ref_
shit
         for ( auto i = rightWin_.begin(); i != rightWin_.end(); ++i ) {
           if ( (val = dist_.Map2Ref(*i, t)) > 0 ) { // off the right
             tmpCache_.push_back(*i);
             toDel.insert(*i);
             rightWin_.erase(i);
           }
           else
             break;
         } // for
         toDel.clear();
       }

/*
       } else if ( !ref_ || t->end() < ref_->end() ) {
         nestedRef_ = true;
       } else {
         nestedRef_ = false;
       }
*/
       // Give derived class the new reference
       SetReference(t);
       ref_ = t;
     }

     inline void OnAdd(MapType* u) {
       // Add(u); Do not add until deletions done in fixWindow()
       //           to keep consistent with sweep
       if ( u->length() >= dist_.ovrRequired_ )
         cache_.insert(u);
     }

     inline void OnDelete(MapType* u) {
       if ( u->length() < dist_.ovrRequired_ )
         return;
       static auto winIter = win_.end();
       winIter = win_.find(u);
       if ( winIter != win_.end() ) { // update
         Delete(u);
         win_.erase(winIter);
       } else {
         cache_.erase(u);
       }
     }

     inline void OnDone() {
       if ( ref_->length() >= dist_.ovrRequired_ ) { // else, already purged win_
         // Deletions must come before insertions for consistency with sweep()  
         std::list<MapType*> lst;

         // delete stuff falling off the left, and the right if applicable
         static StartLesser toDel; // so we delete stuff in proper genomic order
         int val = 0;
         auto winIter = win_.begin();
         bool gone = false;
         while ( winIter != win_.end() ) {
           if ( (val = dist_.Map2Ref(*winIter, ref_)) < 0 ) {
             // forever gone
             toDel.insert(*winIter);
//             Delete(*winIter);
             win_.erase(winIter++);
           } else if ( 0 == val ) {
             break;
           } else if ( nestedRef_ ) { // everything now is too far to the right
             while ( winIter != win_.end() ) {
//               Delete(*winIter);
               toDel.insert(*winIter);
               lst.push_back(*winIter);
               win_.erase(winIter++);
             } // while
             gone = true;
             break;
           } else {
             break;
           }
         } // while

         // delete stuff too far to the right
         if ( nestedRef_ && !gone ) {
           auto revWinIter = win_.rbegin();
           while ( revWinIter != win_.rend() ) {
             if ( dist_.Map2Ref(*revWinIter, ref_) > 0 ) {
//               lclCache.push_front(*revWinIter++); // delete in genomic order
               lst.push_back(*revWinIter);
               toDel.insert(*revWinIter); // delete in genomic order
               win_.erase(--revWinIter.base());
               ++revWinIter;
             } else {
               break;
             }
           } // while

//           while ( !lclCache.empty() ) { // delete in genomic order
//             Delete(lclCache.front());
//             toDel.insert(lclCache.front());
//             lst.push_back(lclCache.front());
//             lclCache.pop_front();
//           } // while
         }
         for ( auto& s : toDel )
           Delete(s);

         // add stuff from cache, or remove if falling off the left
         auto cacheIter = cache_.begin();
         while ( cacheIter != cache_.end() ) {
           if ( (val = dist_.Map2Ref(*cacheIter, ref_)) == 0 ) {
             Add(*cacheIter);
             win_.insert(*cacheIter);
             cache_.erase(cacheIter++);
           } else if ( val < 0 ) { // forever gone
             cache_.erase(cacheIter++);
           } else { // out of range to the right
             break;
           }
         } // while
         cache_.insert(lst.begin(), lst.end());
         toDel.clear();
       }
       DoneReference();
     }

     inline void OnEnd() {
       End();
       win_.clear();
       cache_.clear();
     }

     inline void OnPurge() {
       // bed types can violate sweep's OnPurge() checks and they aren't really needed
     }

     explicit BedBaseVisitor(const DistType& d = DistType()) : dist_(d), ref_(0), nestedRef_(true)
       { /* */ }

     virtual ~BedBaseVisitor() { /* */ }

     // Derived Class interface : Must be public due to MultiVisitor-type usage
     virtual void Delete(MapType*) = 0;
     virtual void Add(MapType*) = 0;
     virtual void DoneReference() = 0;
     virtual inline void SetReference(RefType*) { /* */ }
     virtual inline void End() { /* */ }

   private:
     // MUST use a set with value/address compare here instead of a std::multiset
     //  This is for deleting elements and making sure multiple rows with the
     //  same coordinates each receive Delete() calls to derived classes.
     DistType dist_;
     RefType* ref_;
     bool nestedRef_;
     StartLesser cache_;
     EndLesser leftWin_; // <= ref_
     StartLesser rightWin_; // > ref_
  };

} // namespace Visitors

#endif // _BED_BASE_VISITOR_OVERLAPPING_HPP
