//=========
// Author:  Shane Neph & Scott Kuehn
// Date:    Fri Aug 13 15:00:25 PDT 2010
// Project: bedops
// ID:      $Id$
//=========


// Macro Guard
#ifndef BED_READER_FEATDIST_H
#define BED_READER_FEATDIST_H

// Files included
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

  explicit BedReader(IterType iter) : iter_(iter), cache_()
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
    while ( !cache_.empty() ) {
      delete cache_.back();
      cache_.pop_back();
    }
  }

  void CleanAll() {
    Clean();
    BedType* tmp = static_cast<BedType*>(0);
    while ( (tmp = ReadLine()) )
      delete tmp;
  }

  ~BedReader() {
    while ( !cache_.empty() ) {
      delete cache_.back();
      cache_.pop_back();
    }
  }

private:
  IterType iter_;
  std::vector<BedType*> cache_;
};

} // namespace FeatDist


#endif // BED_READER_FEATDIST_H
