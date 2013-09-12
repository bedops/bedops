//=========
// Author : Shane Neph & Scott Kuehn
// Date   : Fri Aug 24 18:12:08 PDT 2007
// Project: BED utilities
// ID     : $Id:$
//=========

// Macro Guard
#ifndef BED_DATA_COMPARE_H
#define BED_DATA_COMPARE_H

// Files included
#include <cstring>
#include <functional>


namespace Bed {

  // Expect predicate function objects to be defined here

  template <typename BedType1, typename BedType2 = BedType1>
  struct GenomicCompare
     : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    inline bool operator()(BedType1 const* ptr1, BedType2 const* ptr2) const {
      static int v = 0;
      if ( (v = std::strcmp(ptr1->chrom(), ptr2->chrom())) != 0 )
        return(v < 0);
      if ( ptr1->start() != ptr2->start() )
        return(ptr1->start() < ptr2->start());
      return(ptr1->end() < ptr2->end());
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct GenomicAddressCompare
     : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* ptr1, BedType2 const* ptr2) const {
      static int v = 0;
      if ( (v = std::strcmp(ptr1->chrom(), ptr2->chrom())) != 0 )
        return(v < 0);
      if ( ptr1->start() != ptr2->start() )
        return(ptr1->start() < ptr2->start());
      else if ( ptr1->end() != ptr2->end() )
        return(ptr1->end() < ptr2->end());
      return(ptr1 < ptr2);
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct InvertGenomicAddressCompare
      : private GenomicAddressCompare<BedType1, BedType2> {
    typedef GenomicAddressCompare<BedType1, BedType2> Base;
    bool operator()(BedType1 const* ptr1, BedType2 const* ptr2) const {
      return(!Base::operator()(ptr1, ptr2));
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct CoordCompare // ignoring chrom here
     : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* one, BedType2 const* two) const {
      if ( one->start() != two->start() )
        return(one->start() < two->start());
      return(one->end() < two->end());
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct CoordAddressCompare // not caring about chrom here
     : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* one, BedType2 const* two) const {
      if ( one->start() != two->start() )
        return(one->start() < two->start());
      if ( one->end() != two->end() )
        return(one->end() < two->end());
      return(one < two);
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct EndCoordAddressCompareLesser
    : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* one, BedType2 const* two) const {
      if ( one->end() != two->end() )
        return(one->end() < two->end());
      return(one < two);
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct EndCoordAddressCompareGreater
    : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* one, BedType2 const* two) const {
      if ( one->end() != two->end() )
        return(one->end() > two->end());
      return(one > two);
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct StartCoordAddressCompareLesser
    : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* one, BedType2 const* two) const {
      if ( one->start() != two->start() )
        return(one->start() < two->start());
      return(one < two);
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct StartCoordAddressCompareGreater
    : public std::binary_function<BedType1 const*, BedType2 const*, bool> {

    bool operator()(BedType1 const* one, BedType2 const* two) const {
      if ( one->start() != two->start() )
        return(one->start() > two->start());
      return(one > two);
    }
  };

  template <typename BedType1, typename BedType2 = BedType1>
  struct ScoreCompare
     : public std::binary_function<BedType1 const*, BedType2 const*, bool> {
    
    bool operator()(BedType1 const* one, BedType2 const* two) const {
      return(one->measurement() < two->measurement());
    }
  };


} // namespace Bed



#endif // BED_DATA_COMPARE_H
