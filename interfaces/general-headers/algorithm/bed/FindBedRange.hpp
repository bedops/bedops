/*
  Author: Shane Neph & Alex Reynolds
  Date: Mon Jan 23 06:29:10 PST 2012
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

#ifndef BED_RANGE_FINDER_ALGORITHM_H
#define BED_RANGE_FINDER_ALGORITHM_H

#include <cstdio>
#include <iterator>
#include <map>
#include <utility>

#include "data/bed/BedCompare.hpp"
#include "data/bed/BedDistances.hpp"
#include "data/bed/BedTypes.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "utility/PooledMemory.hpp"

namespace Bed {

  typedef Bed::SignedCoordType ByteOffset;

  template <class BedType, std::size_t SZ>
  class bed_check_iterator;

  namespace extract_details {

    template <typename BedType>
    struct CompBed {
      bool operator()(const BedType& b1, const BedType& b2) const {
        static Bed::GenomicCompare<BedType, BedType> gc;
        return gc(&b1,&b2);
      }
    };

    template <typename BT1, typename BT2>
    bool check_overlap(BT1 const* b1, BT2 const* b2) {
      static Bed::Overlapping overlap;
      return overlap(b1, b2) == 0;
    }

    typedef Bed::B3Rest QueryBedType;   // file 1
    typedef QueryBedType TargetBedType; // file 2 -> must be same type as QueryBedType
    typedef std::map<QueryBedType, ByteOffset, CompBed<QueryBedType>> MType;

    template <typename BedType, std::size_t Sz>
    inline
    void remove(Bed::bed_check_iterator<BedType*, Sz>& b, BedType* p)
      { b.get_pool().release(p); }

    template <typename Iter, typename BedType>
    inline
    void remove(Iter, BedType* b)
      { delete b; }

  } // namespace extract_details


  //==================
  // find_bed_range() : think bedops -e -1 with file1 = qfile and titer pointing at file2
  //==================
  template <typename TargetIter, typename Op>
  std::pair<bool, ByteOffset> find_bed_range(FILE* qfile, TargetIter titer, TargetIter teof, Op op) {

    TargetIter orig = titer;
    auto reference = new extract_details::TargetBedType;
    auto last = new extract_details::QueryBedType;
    auto current = new extract_details::QueryBedType;
    Bed::Overlapping overlap, lessthan, greaterthan; // any overlap
    ByteOffset prev_pos = 0, cur_pos = 0, start_pos = 0, end_pos = 0;
    extract_details::MType bounds;

    cur_pos = std::ftell(qfile); // may not be start of file if, for ex., qfile has headers we skipped
    std::fseek(qfile, 0, SEEK_END);  // apparently dangerous on some platforms in binary mode -> padded nulls;
    const ByteOffset at_end = std::ftell(qfile); // I'll assume msft is the problem until I know better
    std::fseek(qfile, cur_pos, SEEK_SET);
    prev_pos = cur_pos;

    std::iterator_traits<FILE*>::difference_type count, step;
    bool didWork = false, first = true, donequery = false;

    while ( titer != teof ) {
      extract_details::TargetBedType* const refelement = *titer++;
      if ( donequery ) { // read through and delete items in [titer,teof) for posterity
        extract_details::remove(orig, refelement);
        continue;
      } else if ( !first && (greaterthan(last, refelement) > 0) ) { // last lower_bound is still applicable
        extract_details::remove(orig, refelement);
        continue;
      }

      // only use reference where you need to compare to starting base.  Otherwise, use refelement.
      *reference = *refelement;
      reference->end(reference->start()+1);

      start_pos = std::ftell(qfile);
      while ( true ) {
        extract_details::MType::iterator miter = bounds.upper_bound(*refelement); // define end_pos
        if ( miter == bounds.end() ) {
          end_pos = at_end;
          bounds.clear();
          break;
        } else {
          // it's possible that miter->first and refelement overlap but are not picked up in
          //   bounds.upper_bound() search b/c bounds is ordered by the analog of "<", where
          //   start coord takes precedence.  Consider when refelement is less than miter->first
          //   but refelement->end() > miter->first.start().  In such a case miter->first would
          //   not be a proper upper bound.  Remove it and try again.
          bounds.erase(bounds.begin(), miter);
          end_pos = miter->second;
          if ( extract_details::check_overlap(refelement, &miter->first) )
            bounds.erase(miter);
          else
            break;
        }
      } // while
      count = end_pos - start_pos; // how many bytes between positions
      didWork = false;

      while ( count > 0 ) {
        std::fseek(qfile, start_pos, SEEK_SET);

        step = count/2;
        std::fseek(qfile, step, SEEK_CUR);

        // find beginning of current line
        while ( std::ftell(qfile) != start_pos ) {
          if ( static_cast<char>(std::fgetc(qfile)) != '\n' ) {
            std::fseek(qfile, -2, SEEK_CUR);
          } else {
            break;
          }
        } // while

        // read in the line; incrementing to start of next QueryBedType element.
        prev_pos = std::ftell(qfile);
        current->readline(qfile);
        cur_pos = std::ftell(qfile);
        bounds.insert(std::make_pair(*current, prev_pos));

        // compare 'current' to starting base
        if ( lessthan(current, reference) < 0 ) {
          count = (end_pos - cur_pos);

          start_pos = cur_pos;
          if ( 0 == count ) {
            if ( end_pos != at_end ) {
              prev_pos = cur_pos;
              current->readline(qfile);
              cur_pos = std::ftell(qfile);
              bounds.insert(std::make_pair(*current, prev_pos));
            } else {
              prev_pos = at_end;
            }
          }
        } else {
          count = (prev_pos - start_pos);
          end_pos = prev_pos;
        }

        didWork = true;
      } // while

      // spit elements in range
      if ( didWork ) {
        while ( prev_pos != at_end && extract_details::check_overlap(current, refelement) ) {
          op(current);
          prev_pos = std::ftell(qfile);
          if ( prev_pos != at_end )
            current->readline(qfile);
        } // while
      }

      if ( refelement )
        extract_details::remove(orig, refelement);

      std::fseek(qfile, prev_pos, SEEK_SET); // because start_pos = std::ftell(qfile); on next go-around
      if ( prev_pos == at_end )
        donequery = true;
      *last = *current;
      first = false;
    } // while

    if ( reference )
      delete reference;
    if ( last )
      delete last;
    if ( current )
      delete current;

    return std::make_pair(!first, prev_pos);
  }


} // namespace Bed

#endif // BED_RANGE_FINDER_ALGORITHM_H
