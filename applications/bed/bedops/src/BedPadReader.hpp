/*
  Author:  Shane Neph & Scott Kuehn
  Date:    Fri Aug 13 15:00:25 PDT 2010
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

#ifndef BED_PAD_READER_BEDOPS_H
#define BED_PAD_READER_BEDOPS_H

#include <algorithm>
#include <cmath>
#include <deque>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "data/bed/BedCompare.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "utility/Assertion.hpp"
#include "utility/ByLine.hpp"


namespace BedOperations {

template <typename T>
struct NoPtr;

template <typename T>
struct NoPtr<T*> {
  typedef T Type;
};


/*
 What to think about with padding:
    chr1  20  100
    chr1  30  50
 say padding is 40:
    chr1  0  140
    chr1  0  90
 this is no longer in sort-bed order.

  This only is a problem near zero, when lpad_ < 0.  The private member function
   getFirst() deals with everything, and should ONLY be called from the constructor or
   during chromosome changes.
*/


//==============
// BedPadReader
//==============
template <typename IterType>
struct BedPadReader {
  typedef IterType IteratorType;
  typedef typename NoPtr<typename IterType::value_type>::Type BedType;

  explicit BedPadReader(IterType iter, int lpadding, int rpadding)
           : iter_(iter), lpad_(lpadding), rpad_(rpadding),
             lastChr_(""), cache_() {
    if ( lpad_ < 0 ) {
      getFirst();
    }
  }

  BedPadReader(const BedPadReader& b); // not safe to copy due to iterators in some cases
  BedPadReader& operator=(const BedPadReader& b); // not safe to assign due to iterators in some cases

  inline bool HasNext() {
    static const IterType end;
    if ( !cache_.empty() )
      return(true);
    if ( iter_ == end )
      return(false);

    /*
    Must ReadLine() in case padding is such that all remaining elements
      in a file vaporize.
    Example:
       chr1	20	100
       chr1	30	50
       chr2	5	8

      bedops -u --pad -5:-10
    */
    BedType* tmp = ReadLine();
    if ( tmp != static_cast<BedType*>(0) ) {
      PushBack(tmp);
      return(true);
    }
    return(false);
  }

  inline void PushBack(BedType* bt) {
    cache_.push_back(bt);
  }

  inline void Remove(BedType* bt) {
    iter_.get_pool().release(bt);
  }

  inline BedType* ReadLine() {
    static const bool done = false;
    static BedType* tmp = static_cast<BedType*>(0);
    static const IterType end;

    // lpad_ may be +, rpad_ may be -.  In either case, padding may cause an element to become
    //   a non-element (vaporizes).  Nothing in the cache_ is a problem.  Only need to
    //   check when reading something new from iter_.
    while ( !done ) {
      if ( !cache_.empty() ) { // get from the cache_
        tmp = cache_.back();
        cache_.pop_back();
        if ( lpad_ < 0 ) // else, don't waste time on next unimportant assignment
          lastChr_ = tmp->chrom();
        return(tmp);
      } else if ( iter_ == end ) { // && cache_.empty()
        return(static_cast<BedType*>(0));
      } else { // read from iter_
        tmp = *iter_; // cannot post-increment here due to lpad_ < 0 && getFirst() possibility
        if ( rpad_ < 0 || lpad_ > 0 ) { // tmp->end() is an Bed::CoordType; must cast
          ++iter_;
          tmp->start(tmp->start() + lpad_);
          if ( static_cast<double>(tmp->end()) + rpad_ > tmp->start() ) {
            tmp->end(tmp->end() + rpad_);
            return(tmp);
          }
          else // tmp vaporized by padding
            Remove(tmp);
        } else if ( lpad_ < 0 ) {
          if ( tmp->chrom() != lastChr_ ) { // cache_ is empty && iter_ != end
            getFirst(); // iter_ increments dealt with in getFirst()
            tmp = cache_.back(); // next item may have changed after getFirst()
            cache_.pop_back();
            lastChr_ = tmp->chrom();
          } else { // getFirst() already dealt with any padding issues on this chromosome
            tmp->start(tmp->start() + lpad_);
            tmp->end(tmp->end() + rpad_);
            ++iter_;
          }
          return(tmp);
        } else if ( rpad_ > 0 ) {
          tmp->start(tmp->start() + lpad_);
          tmp->end(tmp->end() + rpad_);
          ++iter_;
          return(tmp);
        } else {
          ++iter_;
          return(tmp);
        }
      }
    } // while
  }

  void Clean() {
    while ( !cache_.empty() ) {
      Remove(cache_.back());
      cache_.pop_back();
    }
  }

  void CleanAll() {
    Clean();
    BedType* tmp = static_cast<BedType*>(0);
    while ( (tmp = ReadLine()) )
      Remove(tmp);
  }

  ~BedPadReader() {
    while ( !cache_.empty() ) {
      Remove(cache_.back());
      cache_.pop_back();
    }
  }


private:

  // Only called from the constructor or on chrom changes WHEN lpad_ is < 0
  void getFirst() {
    // Only for the case where subtracting padding results in a start coordinate
    //  of zero or less.  Need to keep track of multiple items that are ties to
    //  ensure the output order is the same as the input order.
    // After separating lpad_ from rpad_, need to worry about vaporizing elements
    //  here too for the case when: lpad_ < 0 and rpad_ < lpad_
    typedef Bed::GenomicCompare<BedType, BedType> CompLess;
    typedef std::set<BedType*, CompLess> SType;
    typedef std::multiset<BedType*, CompLess> MType;
    typedef std::vector<BedType*> TieType; // helps deal with ties after padding
    MType mset;
    SType sset;
    TieType tmap;
    static BedType* const zero = static_cast<BedType*>(0);
    BedType* tmp = zero;
    static const IterType end;
    const Bed::CoordType lpd = static_cast<Bed::CoordType>(std::abs(lpad_));
    std::pair<typename SType::iterator, bool> p;
    while ( iter_ != end && (tmp = *iter_++) ) {
      if ( tmp->start() > lpd ) { // cannot be >=
        tmp->start(tmp->start() - lpd);
        if ( tmp->end() + rpad_ > tmp->start() ) {
          tmp->end(tmp->end() + rpad_);
          mset.insert(tmp);
          break; // all others meet the invariant condition: tmp->start() > lpd
        } else { // lpad_ < 0 and rpad_ < lpad_
          Remove(tmp);
          continue;
        }
      }
      if ( static_cast<double>(tmp->end()) + rpad_ <= 0 ) {
        Remove(tmp);
        continue;
      }
      tmp->start(0);
      tmp->end(tmp->end() + rpad_);
      mset.insert(tmp);
      p = sset.insert(tmp);
      if ( !p.second ) { // a tie
        if ( tmap.empty() )
          tmap.push_back(*sset.find(tmp));
        else {
          bool add = true;
          BedType* tmptr = *sset.find(tmp);
          for ( std::size_t i = tmap.size(); i > 0; --i ) {
            if ( tmap[i-1] == tmptr ) { // already in tmap?
              add = false;
              break;
            }
          } // for
          if ( add )
            tmap.push_back(tmptr);
        }
        tmap.push_back(tmp);
      }
    } // while

    cache_.clear(); // cache_ better be empty already!
    sset.clear();

    CompLess cless;
    std::size_t number = 0;
    if ( !tmap.empty() ) {
      for ( typename MType::const_iterator i = mset.begin(); i != mset.end(); ++i ) {
        if ( number >= tmap.size() ) {
          cache_.push_front(*i);
          continue;
        }

        if ( tmap[number] == *i ) {
          cache_.push_front(*i);
          ++number;
        } else if ( 0 == cless.operator()(tmap[number], *i) && 0 == cless.operator()(*i, tmap[number]) ) { // tie, but not tmap[number]
          cache_.push_front(tmap[number]); // tmap holds the proper order on ties
          ++number;
          --i; // try again - may be more than just 2 in the tie
        } else {
          cache_.push_front(*i);
        }
      } // for
    }
    else // cache_ holds the 'next' item at its back
      std::copy(mset.rbegin(), mset.rend(), std::back_inserter(cache_));
  }

private:
  IterType iter_;
  int lpad_, rpad_;
  std::string lastChr_;
  std::deque<BedType*> cache_;
};

} // namespace BedOperations


#endif // BED_PAD_READER_BEDOPS_H
