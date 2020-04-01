/*
  Author: Shane Neph & Scott Kuehn
  Date:   Sun Aug 19 11:39:47 PDT 2007
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

#ifndef SIMPLE_C_FORMATS_H
#define SIMPLE_C_FORMATS_H

#include <cinttypes>
#include <cstdio>
#include <type_traits>

namespace Formats {
  constexpr const char* Format(const char*) { return "%s"; }
  constexpr const char* Format(char) { return "%c"; }
  constexpr const char* Format(double) { return "%lf"; }
  constexpr const char* Format(long double) { return "%Lf"; }
  constexpr const char* Format(float) { return "%f"; }
  constexpr const char* Format(int) { return "%d"; }
  constexpr const char* Format(unsigned int) { return "%u"; }
  constexpr const char* Format(long int) { return "%ld"; }
  constexpr const char* Format(long long int) { return "%lld"; } /* msft doesn't conform to this standard */
  constexpr const char* Format(short) { return "%hd"; }
  constexpr const char* Format(unsigned short) { return "%hu"; }
  inline const char* Format(double, int precision, bool scientific) {
    static char prec[20];
    if ( scientific )
      std::sprintf(prec, "%%.%de", precision);
    else
      std::sprintf(prec, "%%.%dlf", precision);
    return(prec);
  }

  inline const char* Format(long double, int precision, bool scientific) {
    static char prec[20];
    if ( scientific )
      std::sprintf(prec, "%%.%dLe", precision);
    else
      std::sprintf(prec, "%%.%dLf", precision);
    return(prec);
  }

  namespace Details {
    template <typename T, typename U>
    struct check {
      typedef typename std::remove_reference<typename std::remove_cv<T>::type>::type type;

      static const auto value = std::is_same<type, U>::value;
      static const auto not_uint64_t = !std::is_same<type, uint64_t>::value;
    };
  } // Details

  // Overloaded functions for uint64_t and potential underlying types
  template <typename T>
  constexpr typename std::enable_if<Details::check<T, uint64_t>::value, char const*>::type
  Format(T) { return "%" PRIu64; }

  template <typename T>
  constexpr typename std::enable_if<Details::check<T, unsigned long long int>::value &&
                                    Details::check<T, unsigned long long int>::not_uint64_t, char const*>::type
  Format(T) { return "%llu"; } /* msft doesn't conform to this standard */

  template <typename T>
  constexpr typename std::enable_if<Details::check<T, unsigned long int>::value &&
                                    Details::check<T, unsigned long int>::not_uint64_t &&
                                    !Details::check<T, unsigned int>::value,
                                    char const*>::type
  Format(T) { return "%lu"; }

} // namespace Formats

#endif // SIMPLE_C_FORMATS_H
