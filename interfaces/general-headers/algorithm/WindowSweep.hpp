/*
  Author: Shane Neph & Scott Kuehn
  Date:   Thu Nov 29 18:03:26 PST 2007
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

#ifndef WINDOWED_SWEEP_ALGORITHM_H
#define WINDOWED_SWEEP_ALGORITHM_H

#include "data/bed/BedDistances.hpp"

namespace WindowSweep {

  //================================================================
  // sweep() Overload1 : A single input iterator pair
  //  This version is meant to sweep over a single input iterator
  //   pair.  Examples usages may be for calculating trimmed means,
  //   smoothing operations, interpolation, etc.
  //================================================================
  template <
            class InputIterator,
            class RangeComp,
            class EventVisitor
           >
  void sweep(InputIterator start, InputIterator end,
             RangeComp inRange, EventVisitor& visitor);

  template <
            class InputIterator,
            class EventVisitor
           >
  void sweep(InputIterator start, InputIterator end,
             Bed::Overlapping inRange, EventVisitor& visitor);


  //=================================================================
  // sweep() Overload2 : Two pair of input iterator pairs
  //  This version is meant to sweep over two input iterator pairs.
  //   Examples usages may be for convolution calculations & mapping
  //   aggregated values from iterator pair 2 onto iterator pair 1.
  //=================================================================
  template <
            class InputIterator1,
            class InputIterator2,
            class RangeComp,
            class EventVisitor
           >
  void sweep(InputIterator1 refStart, InputIterator1 refEnd,
             InputIterator2 mapFromStart, InputIterator2 mapFromEnd,
             RangeComp inRange, EventVisitor& visitor, bool sweepMapAll = false);


  /*
    sweep() Assumptions:
    1) in terms of RangeComp(a, b):
         return 0 if a and b are 'within range' of each other
         return less than 0 if a is 'less than' b
         return greater than 0 if a is 'greater than' b

    2) in terms of RangeComp(c, c):
         return 0 : object must always be in range of itself

    3) EventVisitor::reference_type, and in the case of the 2nd
         sweep() overload, EventVisitor::mapping_type, must be
         constructible from items pointed to by the iterators
         passed into sweep().  In many cases, this is the copy
         constructor, but may be any constructor with the appropriate
         argument type.  An example could be a constructor taking
         a string, where the iterators passed in point to strings.
  */

} // namespace WindowSweep

#include "../../src/algorithm/sweep/WindowSweepImpl.cpp"
#include "../../src/algorithm/sweep/WindowSweepImpl.specialize.cpp"

#endif // WINDOWED_SWEEP_ALGORITHM_H
