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

// Files included
#include <cstdio>
#include <string>

// Files included
#include <boost/utility/enable_if.hpp>

// Files included
#include "utility/Formats.hpp"
#include "utility/PrintTypes.hpp"


namespace PrintTypes {

  // Built-in char* and arithmetic types: Print() and Println()

  template <typename T>
  typename boost::enable_if< Details::check<T>, void >::type
  Print(T t) {
    static std::string f = Formats::Format(t);
    static char const* format = f.c_str();
    std::printf(format, t);
  }

  template <typename T>
  typename boost::enable_if< Details::check<T>, void >::type
  Println(T t) {
    static std::string end = Formats::Format(t) + std::string("\n");
    static char const* format = end.c_str();
    std::printf(format, t);
  }

  template <typename T>
  extern typename boost::enable_if< boost::is_arithmetic<T>, void >::type
  Print(T t, int precision, bool scientific) {
    std::string f = Formats::Format(t, precision, scientific);
    char const* format = f.c_str();
    std::printf(format, t);
  }

  template <typename T>
  extern typename boost::enable_if< boost::is_arithmetic<T>, void >::type
  Println(T t, int precision, bool scientific) {
    std::string end = Formats::Format(t, precision, scientific) + std::string("\n");
    char const* format = end.c_str();
    std::printf(format, t);
  }

  template <typename T>
  typename boost::enable_if< Details::check<T>, void >::type
  Print(FILE* out, T t) {
    static std::string f = Formats::Format(t);
    static char const* format = f.c_str();
    std::fprintf(out, format, t);
  }

  template <typename T>
  typename boost::enable_if< Details::check<T>, void >::type
  Println(FILE* out, T t) {
    static std::string end = Formats::Format(t) + std::string("\n");
    static char const* format = end.c_str();
    std::fprintf(out, format, t);
  }


  // Default user-defined types: Print() and Println()
  //  More specialized version can be created for other user-defined types

  template <typename T>
  typename boost::disable_if< Details::check<T>, void >::type
  Print(const T& t)
    { t.print(); }

  template <typename T>
  typename boost::disable_if< Details::check<T>, void >::type
  Println(const T& t)
    { t.println(); }

  template <typename T>
  typename boost::disable_if< Details::check<T>, void >::type
  Print(FILE* out, const T& t)
    { t.print(out); }

  template <typename T>
  typename boost::disable_if< Details::check<T>, void >::type
  Println(FILE* out, const T& t)
    { t.println(out); }

} // namespace PrintTypes
