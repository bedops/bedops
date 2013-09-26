/*
  FILE: PrintTypes.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Wed Sep  5 11:16:29 PDT 2007
  PROJECT: utility
  ID: $Id:$
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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

// Macro Guard
#ifndef SIMPLE_PRINT_FORMATS_H
#define SIMPLE_PRINT_FORMATS_H

// Files included
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>


namespace PrintTypes {

  namespace Details {
    template <typename T>
    struct check {
      static const bool value = 
              boost::is_arithmetic<T>::value ||
              boost::is_same< typename boost::remove_const<T>::type, char* >::value ||
              boost::is_same< typename boost::remove_const<T>::type, char const* >::value;
    };
  }

  template <typename T>
  extern typename boost::enable_if< Details::check<T>, void >::type
  Print(T);

  template <typename T>
  extern typename boost::enable_if< Details::check<T>, void >::type
  Println(T);

  template <typename T>
  extern typename boost::enable_if< boost::is_arithmetic<T>, void >::type
  Print(T, int, bool);

  template <typename T>
  extern typename boost::enable_if< boost::is_arithmetic<T>, void >::type
  Println(T, int, bool);

  template <typename T>
  extern typename boost::enable_if< Details::check<T>, void >::type
  Print(FILE* out, T t);

  template <typename T>
  extern typename boost::enable_if< Details::check<T>, void >::type
  Println(FILE* out, T t);



  template <typename T>
  extern typename boost::disable_if< Details::check<T>, void >::type
  Print(const T& t);

  template <typename T>
  extern typename boost::disable_if< Details::check<T>, void >::type
  Println(const T& t);

  template <typename T>
  extern typename boost::disable_if< Details::check<T>, void >::type
  Print(FILE* out, const T& t);

  template <typename T>
  extern typename boost::disable_if< Details::check<T>, void >::type
  Println(FILE* out, const T& t);

} // namespace PrintTypes

#include "../../src/utility/PrintTypes.cpp"

#endif // SIMPLE_PRINT_FORMATS_H
