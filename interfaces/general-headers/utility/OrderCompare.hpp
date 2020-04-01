/*
  Author: Shane Neph & Scott Kuehn
  Date:   Sun Aug 19 19:01:10 PDT 2007
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
