/*
  Author: Shane Neph
  Date:   Mon Jun  5 13:34:58 PDT 2017
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

#ifndef UTILS_COMPILERMATH
#define UTILS_COMPILERMATH

#include <cstddef>

namespace Ext {
  template <std::size_t Base, std::size_t L, bool b=(L<=Base)>
  struct IntLogN {
    static constexpr std::size_t value = 1 + IntLogN<Base, L/Base + (L%Base > 0)>::value;
  };

  template <std::size_t Base, std::size_t L>
  struct IntLogN<Base, L, true> {
    static constexpr std::size_t value = 1;
  };

  template <std::size_t V, std::size_t W>
  struct Pow {
    static constexpr std::size_t value = V*Pow<V, W-1>::value;
  };

  template <std::size_t V>
  struct Pow<V,1> {
    static constexpr std::size_t value = V;
  };

  template <std::size_t V>
  struct Pow<V,0> {
    static constexpr std::size_t value = 0;
  };
} // namespace Ext

#endif // UTILS_COMPILERMATH
