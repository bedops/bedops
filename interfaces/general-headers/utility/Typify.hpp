/*
  Author: Shane Neph & Scott Kuehn
  Date:   Thu Nov 29 15:28:17 PST 2007
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

#ifndef TYPIFY_CONSTRUCTIONS_H
#define TYPIFY_CONSTRUCTIONS_H

namespace Ext {

  //============
  // Int2Type<> : typify an integer
  //   o good for overloading & specializations purposes
  //============
  template <int Val>
  struct Int2Type {
    enum { val = Val };
  };

  //=============
  // Type2Type<> : typify a type
  //   o lightweight -> good for function overloading purposes
  //   o does not require template T to be constructible
  //=============
  template <typename T>
  struct Type2Type {
    typedef T OriginalType;
  };

  //============
  // has_type<T> : is there a T::type typedef?
  // stolen and modified from
  // http://stackoverflow.com/questions/25626293/has-type-template-returns-true-for-struct-type
  //============
  template <typename T>
  struct has_type {

    template <typename C>
    static constexpr
    char test_for_type(...) { return '0'; }

    template <typename C>
    static constexpr
    double test_for_type(typename C::type const*) { return 0.0; }

    static const bool value = sizeof(test_for_type<T>(0)) == sizeof(double);
  };

} // namespace Ext

#endif // TYPIFY_CONSTRUCTIONS_H
