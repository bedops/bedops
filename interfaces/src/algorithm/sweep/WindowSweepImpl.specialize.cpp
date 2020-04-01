/*
  Author: Shane Neph & Scott Kuehn
  Date:   Sat Aug 11 09:35:42 PDT 2007
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

#include <cstdlib>
#include <deque>

#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedDistances.hpp"
#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "utility/AllocateIterator.hpp"

namespace WindowSweep {

  /*
    why the specialization? From BedBaseVisitor.hpp
      chr1  1   200  a  1
      chr1  10  20   b  3
      chr1  50  150  c  4

      when used with bedmap --bp-ovr 11 --count <one-file>

      row 2 has no hits, but row 1 goes with row 3's output.  Cannot assume that once
        row 2 is out of range that everything to the left of it is out of range too,
        even when looking at straight bp's.  The main issue shown here is that the
        --bp-ovr 11 is larger than row 2's range.  It can never qualify but row 1 can.
  */

  // See WindowSweep.hpp for detailed assumptions of sweep()

  //===================
  // sweep Overload1 :
  //===================
  template <
            class InputIterator,
            class EventVisitor
           >
  void sweep(InputIterator start, InputIterator end,
             Bed::Overlapping inRange, EventVisitor& visitor) {

    // Local typedefs
    typedef typename EventVisitor::RefType Type;
    typedef Type* TypePtr;
    typedef std::deque<TypePtr> WindowType;

    // Local variables
    // const bool cleanHere = !visitor.ManagesOwnMemory(); no longer useful with multivisitor
    const TypePtr zero = static_cast<TypePtr>(0);
    TypePtr bPtr = zero;
    std::size_t index = 0;
    WindowType win;
    TypePtr cache = zero;
    bool first = true;
    bool reset = true;
    InputIterator orig = start;

    // Loop through inputs
    while ( start != end || cache || !win.empty() ) {

      if ( !reset ) { // Check items falling out of range 'to the left'
        visitor.OnStart(win[index]);
        while ( !win.empty() && inRange.Map2Ref(win[0], win[index]) < 0 ) {
          if ( win[0]->length() >= inRange.ovrRequired_ )
            visitor.OnDelete(win[0]);
          Details::clean(orig, win[0]);
          win.pop_front(), --index;
        } // while
      } else { // last item in windowed buffer, reset buffer
        if ( start == end && !cache ) { // stopping condition
          visitor.OnEnd();
          while ( !win.empty() ) { // deletions belonging to NO ref
            Details::clean(orig, win[0]);
            win.pop_front();
          } // while
          break;
        }
        // we cannot call another vistor action until we add something
        //  to the new window and declare it as our new reference,
        //  which will occur when we look to add elements to the
        //  (soon to be empty) window.  Since we are here, more input
        //  exists and we are guaranteed to enter the proper inclusion
        //  code and call visitor.OnStart() before anything else.
        // 'win' stores items that need visitor.OnDelete() calls, but
        //  logically belong to the next, currently undeclared,
        //  reference.  So, we must wait a bit.
      }

      // Check for items to be included in current windowed range
      while ( cache || start != end ) {
        if ( cache ) {
          bPtr = cache;
          cache = zero;
        }
        else {
          bPtr = Details::get(start); // don't do get(start++); in case allocate_iterator
          ++start;
        }

        if ( win.empty() || reset || inRange.Ref2Map(win[index], bPtr) == 0 ) {
          if ( reset ) { // we are populating a new window
            reset = false;
            index = 0;
            visitor.OnStart(bPtr); // establish the new reference
            if ( !first )
              visitor.OnPurge(); // starting a new window; notify visitor
            first = false;

            while ( !win.empty() ) { // deletions on behalf of new ref
              if ( win[0]->length() >= inRange.ovrRequired_ )
                visitor.OnDelete(win[0]);
              Details::clean(orig, win[0]);
              win.pop_front();
            } // while
          }
          win.push_back(bPtr);
          if ( bPtr->length() >= inRange.ovrRequired_ )
            visitor.OnAdd(bPtr); // must follow 'reset' check
        }
        else { // read one passed current windowed range
          cache = bPtr;
          break;
        }
      } // while

      visitor.OnDone(); // done processing current item

      reset = (++index >= win.size());
    } // while !done

    if ( cache )
      Details::clean(orig, cache); // never given to visitor

  } // sweep() overload1

} // namespace WindowSweep
