/*
  FILE: OrderCompare.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Sun Aug 19 19:01:10 PDT 2007
  PROJECT: general-utilities
  ID: $Id:$
*/

// Macro Guard
#ifndef ORDERING_COMPARISONS_H
#define ORDERING_COMPARISONS_H


namespace Ordering {

  // function objects for comparing values, then addresses
  template <typename T1, typename T2=T1>
  struct CompValueThenAddressLesser
    : public std::binary_function<T1 const*, T2 const*, bool> {
      inline bool operator()(T1 const* t1, T2 const* t2) const {
        if ( *t1 != *t2 )
          return(*t1 < *t2);
        return(t1 < t2);
      }
  };

  template <typename T1, typename T2=T1>
  struct CompValueThenAddressGreater
    : public std::binary_function<T1 const*, T2 const*, bool> {
      inline bool operator()(T1 const* t1, T2 const* t2) const {
        if ( *t1 != *t2 )
          return(*t1 > *t2);
        return(t1 > t2);
      }
  };

} // namespace Ordering

#endif // ORDERING_COMPARISONS_H
