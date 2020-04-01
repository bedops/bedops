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

#include "data/bed/AllocateIterator_BED_starch_minmem.hpp"
#include "data/bed/AllocateIterator_BED_starch.hpp"
#include "data/bed/BedCheckIterator.hpp"
#include "data/bed/BedCheckIterator_minmem.hpp"
#include "utility/AllocateIterator.hpp"

namespace WindowSweep {

  // See WindowSweep.hpp for detailed assumptions of sweep()

  namespace Details {

    // get() & clean() function overloads allow sweep() to be written only twice.
    //  New iterator ideas receive just another get().  For example,
    //  the speedy allocate_iterator<T> requires something special below.
    template <typename IteratorType>
    inline typename IteratorType::value_type* get(IteratorType& i)
      { return(new typename IteratorType::value_type(*i)); }

    template <typename IteratorType>
    inline void clean(IteratorType& i, typename IteratorType::value_type* p)
      { delete p; }


    // general fast iterator (non-BED)
    template <typename T>
    inline typename Ext::allocate_iterator<T*>::value_type
                                     get(Ext::allocate_iterator<T*>& i)
      { return(*i); } /* no copy via operator new here */

    template <typename T>
    inline void clean(Ext::allocate_iterator<T*>& i, T* p)
      { delete p; }


    // min memory fast iterator (v2p4p26 and older)
    template <typename T>
    inline typename Bed::allocate_iterator_starch_bed_mm<T*>::value_type
                                     get(Bed::allocate_iterator_starch_bed_mm<T*>& i)
      { return(*i); } /* no copy via operator new here */

    template <typename T>
    inline void clean(Bed::allocate_iterator_starch_bed_mm<T*>& i, T* p)
      { delete p; }


    // fastest iterator (v2p4p27 and newer)
    template <typename T, std::size_t PoolSz> // pooled memory
    inline typename Bed::allocate_iterator_starch_bed<T*, PoolSz>::value_type
                                     get(Bed::allocate_iterator_starch_bed<T*, PoolSz>& i)
      { return(*i); } /* no copy via operator new here */

    template <typename T, std::size_t PoolSz>
    inline void clean(Bed::allocate_iterator_starch_bed<T*, PoolSz>& i, T* p)
      { static auto& pool = i.get_pool(); pool.release(p); }


    // min memory error checking iterator(v2p4p26 and older)
    template <typename T>
    inline typename Bed::bed_check_iterator_mm<T*>::value_type
                                     get(Bed::bed_check_iterator_mm<T*>& i)
      { return(*i); } /* no copy via operator new here */

    template <typename T>
    inline void clean(Bed::bed_check_iterator_mm<T*>& i, T* p)
      { delete p; }


    // pooled memory error checking iterator(v2p4p27 and newer)
    template <typename T, std::size_t PoolSz>
    inline typename Bed::bed_check_iterator<T*, PoolSz>::value_type
                                     get(Bed::bed_check_iterator<T*, PoolSz>& i)
      { return(*i); } /* no copy via operator new here */

    template <typename T, std::size_t PoolSz>
    inline void clean(Bed::bed_check_iterator<T*, PoolSz>& i, T* p)
      { static auto& pool = i.get_pool(); pool.release(p); }

  } // namespace Details

  //===================
  // sweep Overload1 :
  //===================
  template <
            class InputIterator,
            class RangeComp,
            class EventVisitor
           >
  void sweep(InputIterator start, InputIterator end,
             RangeComp inRange, EventVisitor& visitor) {

    // Local typedefs
    typedef typename EventVisitor::RefType Type;
    typedef Type* TypePtr;
    typedef std::deque<TypePtr> WindowType;

    // Local variables
    // const bool cleanHere = !visitor.ManagesOwnMemory(); no longer useful with multivisitor
    InputIterator orig = start;
    const TypePtr zero = static_cast<TypePtr>(0);
    TypePtr bPtr = zero;
    std::size_t index = 0;
    WindowType win;
    TypePtr cache = zero;
    bool first = true;
    bool reset = true;

    // Loop through inputs
    while ( start != end || cache || !win.empty() ) {

      if ( !reset ) { // Check items falling out of range 'to the left'
        visitor.OnStart(win[index]);
        while ( !win.empty() && inRange.Map2Ref(win[0], win[index]) < 0 ) {
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
              visitor.OnDelete(win[0]);
              Details::clean(orig, win[0]);
              win.pop_front();
            } // while
          }
          win.push_back(bPtr);
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


  //===================
  // sweep Overload2 :
  //===================
  template <
            class InputIterator1,
            class InputIterator2,
            class RangeComp,
            class EventVisitor
           >
  void sweep(InputIterator1 refStart, InputIterator1 refEnd,
             InputIterator2 mapFromStart, InputIterator2 mapFromEnd,
             RangeComp inRange, EventVisitor& visitor, bool sweepMapAll) {

    // Local typedefs
    typedef typename EventVisitor::RefType RefType;
    typedef typename EventVisitor::MapType MapType;
    typedef MapType* MapTypePtr;
    typedef std::deque<MapTypePtr> WindowType;
    typedef RefType* RefTypePtr;

    // Local variables
    // const bool cleanHere = !visitor.ManagesOwnMemory(); no longer useful with multivisitor
    const MapTypePtr zero = static_cast<MapTypePtr>(0);
    RefTypePtr rPtr;
    MapTypePtr mPtr = zero, cache = zero;
    WindowType win;
    double value = 0;
    bool willPurge = false;
    InputIterator1 rorig = refStart;
    InputIterator2 morig = mapFromStart;

    // Loop through inputs
    while ( refStart != refEnd ) {

      rPtr = Details::get(refStart); // don't do get(refStart++); in case allocate_iterator
      ++refStart;
      visitor.OnStart(rPtr);

      // See if we will be starting a new window
      willPurge = !win.empty() && (inRange.Map2Ref(win[win.size()-1], rPtr) < 0);
      if ( willPurge ) // notify visitor before deleting elements
        visitor.OnPurge();

      // Pop off items falling out of range 'to the left'
      while ( !win.empty() && inRange.Map2Ref(win[0], rPtr) < 0 ) {
        visitor.OnDelete(win[0]);
        Details::clean(morig, win[0]);
        win.pop_front();
      } // while

      // Check for items to be included in current windowed range
      while ( cache || mapFromStart != mapFromEnd ) {
        if ( cache ) {
          mPtr = cache;
          cache = zero;
        }
        else {
          mPtr = Details::get(mapFromStart); // don't do get(mapFromStart++); in case allocate_iterator
          ++mapFromStart;
        }

        if ( (value = inRange.Ref2Map(rPtr, mPtr)) == 0 ) { // within range
          win.push_back(mPtr);
          visitor.OnAdd(mPtr);
        }
        else if ( value < 0 ) { // read one passed current windowed range
          cache = mPtr;
          break;
        }
        else
          Details::clean(morig, mPtr);
      } // while
      visitor.OnDone(); // done processing current ref item
      Details::clean(rorig, rPtr);
    } // while more ref data

    visitor.OnEnd();
    while ( !win.empty() ) { // deletions belonging to NO ref
      Details::clean(morig, win[0]);
      win.pop_front();
    } // while

    if ( cache ) // never given to visitor
      Details::clean(morig, cache);

    if ( sweepMapAll ) { // read and clean remainder of map file
      while ( mapFromStart != mapFromEnd ) {
        mPtr = Details::get(mapFromStart); // don't do get(mapFromStart); in case allocate_iterator
        ++mapFromStart;
        Details::clean(morig, mPtr); // never given to visitor
      } // while
    }

  } // sweep() overload2

} // namespace WindowSweep
