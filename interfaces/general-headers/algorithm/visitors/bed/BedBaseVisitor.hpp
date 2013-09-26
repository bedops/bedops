/*
  FILE: BedBaseVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Dec. 7, 2009
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

#ifndef _BED_BASE_VISITOR_HPP
#define _BED_BASE_VISITOR_HPP

#include <list>
#include <set>

#include "algorithm/WindowSweep.hpp"
#include "data/bed/BedCompare.hpp"
#include "utility/Assertion.hpp"
#include "utility/Exception.hpp"

/*
  Bed types are not sequences of simple numbers.  WindowSweep::sweep() can
  be used with both types, but there are inherent difficulties with BED.

  Take, for instance, the properly ordered coordinates:
  Problem 1:
  chr1 0 100
  chr1 23 25
  chr1 23 107
  chr1 30 45
  chr1 31 32
  chr1 99 100
  chr1 101 107

  When the first element is evaluated, everything but the last row is part
  of the current window.  When you change to the second row, chr1 99 100 is
  no longer part of the current window.  Then it is for the 3rd row, then
  not for the 4th.  We do not account for such possibilities within sweep()
  or the main Visitor baseclass via events.

  BedBaseVisitor implements logic to deal with these problems, and issues
  events to subclasses appropriately.  Bed type visitors should inherit
  from BedBaseVisitor and use its public interface.

  The same sort of issue exists on the left end.
  Problem 2:
  chr1 0 100
  chr1 23 25
  chr1 39 100

  When the 3rd row is the current item, row 2 does not make up its window,
  but row 1 does.  Once row 2 is behind and out of scope relative to the
  current item; it never comes back into play.

  I believe memory management can no longer be passed down to derived classes
  due to buffering that goes on within this class.  I haven't thought about
  it much yet as no Visitor has yet needed its own memory management.

  I have tried optimizing things when the overlap method is based upon bp's and
   not percentages and received much better runtimes.  Unfortunately, my assumptions
   were wrong.  The bottom line is that you need to look at every element in the
   current window, both going out of range to the left and coming into range on
   the right in fixWindow().  Here is the case that fails:

  chr1  1   200  a  1
  chr1  10  20   b  3
  chr1  50  150  c  4

  when used with bedmap --bp-ovr 11 --count -

  row 2 has no hits, but row 1 goes with row 3's output.  Cannot assume that once
    row 2 is out of range that everything to the left of it is out of range too,
    even when looking at straight bp's.  The main issue shown here is that the
    --bp-ovr 11 is larger than row 2's range.  It can never qualify but row 1 can.

  All of the problems mentioned can be attributed to fully nested elements.  There
    are scenarios without fully nested elements that are problematic too without this
    BedBaseVisitor.  For example, consider using Visitors.hpp as base class with the
    bedmap application under the following scenario:
      bedmap --fraction-map 0.1 --echo --count with the following file:

        chr1	564622	564633
        chr1	564629	564637
        chr1	564634	564677
        chr1	564673	564681
        chr1	564974	565006
        chr1	564978	565023
        chr1	565007	565040
        chr1	565256	565294

   When at line 2, line 3 does not map b/c 0.1*43nt = 4nt while line 2 and line 3
     overlap by 3 nt.  But, when we go to line 3 as the reference, line 2 is an
     overlapping element that qualifies since its size is 8 nt (0.1*8nt -> any
     overlap satisfies).  Unfortunately, sweep() has already flushed out line 2
     since it has the inherent symmetry assumption:  dist(a,b) == dist(b,a).
   BedBaseVisitor deals with this scenario properly.
*/


namespace Visitors {

  template <typename BedDist, typename Ref, typename Map = Ref>
  struct BedBaseVisitor {
     typedef BedDist dist_type;
     typedef Ref reference_type;
     typedef Map mapping_type;
  
   protected:
     // friends
     template <class I, class R, class E>
     friend void WindowSweep::sweep(I, I, R&, E&);
  
     template <class I, class J, class R, class E>
     friend void WindowSweep::sweep(I, I, J, J, R&, E&, bool);
  
     // typedefs
     typedef std::set< mapping_type*, Bed::GenomicAddressCompare<mapping_type> > OrderLesser;
     typedef OrderLesser OrderCache;
     typedef OrderLesser OrderWin;
  
     // Implement what WindowSweep::sweep() needs
     inline bool ManagesOwnMemory() const { return(false); }
  
     inline void OnStart(reference_type* t) {
       // Give derived class the new reference
       SetReference(t);
       ref_ = t;
       if ( !first_ )
         checkPurge();
       first_ = false;
     }
  
     inline void OnAdd(mapping_type* u) {
       // Add(u); Do not add until deletions done in fixWindow()
       cache_.insert(u);
     }
  
     inline void OnDelete(mapping_type* u) {
       if ( win_.find(u) == win_.end() )
         cache_.erase(u);
       else { // update
         Delete(u);
         win_.erase(u);
       }
     }
  
     void OnDone() {
       fixWindow(); // deletions before insertions
       DoneReference();
     }
  
     inline void OnEnd() {
       End();
       win_.clear();
       cache_.clear();
     }

     inline void OnPurge() {
       // bed types can violate sweep's OnPurge() checks -> see checkPurge() below
       // Checked elsewhere -> do nothing here
     }

    private:
     inline void checkPurge() {
       // Are ALL items in the window out of range of 'ref_'?
       if ( win_.empty() )
         return;
       typename OrderCache::iterator i = cache_.begin();
       while ( i != cache_.end() && dist_.Map2Ref(*i, ref_) < 0 )
         ++i;

       if ( i == cache_.end() ) {
         typename OrderWin::iterator j = win_.begin();
         while ( j != win_.end() && dist_.Map2Ref(*j, ref_) < 0 )
           ++j;
         if ( j == win_.end() ) {
           Purge();
           for ( typename OrderWin::iterator i = win_.begin(); i != win_.end(); ++i )
             Delete(*i);
           win_.clear();
           cache_.clear();
           // do not reset rest_ since the last OnStart() set the new window's start point
           //  rest_ is legite.  If user is using the single iterator version of sweep()
           //  then OnAdd() will still get called and rest_ added to the appropriate
           //  containers.
         }
       }
     }
  
   public:
     explicit BedBaseVisitor(const dist_type& d = dist_type()) : dist_(d), first_(true)
       { /* */ }
  
     virtual ~BedBaseVisitor() { /* */ }
  
     // Derived Class interface : Must be public due to MultiVisitor-type usage
     virtual void Delete(mapping_type*) = 0;
     virtual void Add(mapping_type*) = 0;
     virtual void DoneReference() = 0;
     virtual inline void SetReference(reference_type*) { /* */ }
     virtual inline void End() { /* */ }
     virtual inline void Purge() { /* */ }
  
   private:
     void fixWindow() {
       // Deletions must come before insertions for consistency with sweep()
       // Realize that, at a minimum, ref_ has changed -> all time-consuming
       //  checks are necessary
  
       // Are any items in the window really out of range of 't'?  See problem 2.
       static std::list<typename OrderWin::value_type> lst;
       typename OrderWin::iterator winIter = win_.begin(), wtmp;
       while ( winIter != win_.end() ) {
         if ( dist_.Map2Ref(*winIter, ref_) != 0 ) {
           Delete(*winIter);
//           cache_.insert(*winIter);
           lst.push_back(*winIter);
           wtmp = winIter++;
           win_.erase(wtmp);
         } else {
           ++winIter;
         }
       } // while
  
       typename OrderCache::iterator cacheIter = cache_.begin(), ctmp;
       while ( cacheIter != cache_.end() ) {
         if ( 0 == dist_.Map2Ref(*cacheIter, ref_) ) {
           Add(*cacheIter);
           win_.insert(*cacheIter);
           ctmp = cacheIter++;
           cache_.erase(ctmp);
         } else {
           ++cacheIter;
         }
       } // while
  
       cache_.insert(lst.begin(), lst.end());
       lst.clear();
     }
  
   private:
     // MUST use a set with value/address compare here instead of a std::multiset
     //  This is for deleting elements and making sure multiple rows with the
     //  same coordinates each receive Delete() calls to derived classes.
     dist_type dist_;
     reference_type* ref_;
     OrderCache cache_;
     OrderWin win_;
     bool first_;
  };

} // namespace Visitors

#endif // _BED_BASE_VISITOR_HPP
