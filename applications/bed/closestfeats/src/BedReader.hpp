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

#ifndef BED_READER_FEATDIST_H
#define BED_READER_FEATDIST_H

#include <vector>

#include "utility/Assertion.hpp"
#include "utility/ByLine.hpp"

namespace FeatDist {

template <typename T>
struct NoPtr;

template <typename T>
struct NoPtr<T*> {
  typedef T Type;
};

//===========
// BedReader
//===========
template <typename IterType>
struct BedReader {
  typedef typename NoPtr<typename IterType::value_type>::Type BedType;

  explicit BedReader(IterType iter) : iter_(iter), orig_(iter), cache_()
    { /* */ }

  BedReader(const BedReader& b); // not safe to copy due to iterators in some cases
  BedReader& operator=(const BedReader& b); // not safe to assign due to iterators in some cases

  inline bool HasNext() {
    static const IterType end;
    return(!cache_.empty() || iter_ != end);
  }

  inline void PushBack(BedType* bt) {
    cache_.push_back(bt);
  }

  inline void PushBack(const std::list<BedType*>& c) {
    cache_.insert(cache_.end(), c.rbegin(), c.rend());
  }

  inline BedType* ReadLine() {
    static BedType* tmp = static_cast<BedType*>(0);
    static const IterType end;
    if ( !cache_.empty() ) {
      tmp = cache_.back();
      cache_.pop_back();
    }
    else if ( iter_ == end ) // && cache_.empty()
      tmp = static_cast<BedType*>(0);
    else
      tmp = *iter_++;
    return(tmp);
  }

  void Clean() {
    auto& mem = orig_.get_pool();
    while ( !cache_.empty() ) {
      mem.release(cache_.back());
      cache_.pop_back();
    }
  }

  void CleanAll() {
    Clean();
    BedType* tmp = static_cast<BedType*>(0);
    auto& mem = orig_.get_pool();
    while ( (tmp = ReadLine()) )
      mem.release(tmp);
  }

private:
  IterType iter_, orig_;
  std::vector<BedType*> cache_;
};

} // namespace FeatDist


#endif // BED_READER_FEATDIST_H
