/*
 Author : Shane Neph & Scott Kuehn
 Date   : Fri Aug 24 18:12:08 PDT 2007
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

#ifndef BED_DATA_COMPARE_H
#define BED_DATA_COMPARE_H

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
