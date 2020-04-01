/*
  Author: Shane Neph & Scott Kuehn
  Date:   Wed Sep  5 11:16:29 PDT 2007
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

#ifndef SIMPLE_PRINT_FORMATS_H
#define SIMPLE_PRINT_FORMATS_H

#include <cstdio>
#include <string>
#include <type_traits>

#include "utility/Formats.hpp"

namespace PrintTypes {

  namespace Details {
    template <typename T>
    struct check {
      static const bool value = 
              std::is_arithmetic<T>::value ||
              std::is_same<char const*, typename std::decay<T>::type>::value ||
              std::is_same<char*, typename std::decay<T>::type>::value;
    };
  }

  template <typename T>
  typename std::enable_if<Details::check<T>::value>::type
  Print(T t) {
    static std::string f = Formats::Format(t);
    static char const* format = f.c_str();
    std::printf(format, t);
  }

  template <typename T>
  typename std::enable_if<Details::check<T>::value>::type
  Println(T t) {
    static std::string end = Formats::Format(t) + std::string("\n");
    static char const* format = end.c_str();
    std::printf(format, t);
  }

  template <typename T>
  typename std::enable_if<std::is_arithmetic<T>::value>::type
  Print(T t, int precision, bool scientific) {
    std::string f = Formats::Format(t, precision, scientific);
    char const* format = f.c_str();
    std::printf(format, t);
  }

  template <typename T>
  typename std::enable_if<std::is_arithmetic<T>::value>::type
  Println(T t, int precision, bool scientific) {
    std::string end = Formats::Format(t, precision, scientific) + std::string("\n");
    char const* format = end.c_str();
    std::printf(format, t);
  }

  template <typename T>
  typename std::enable_if<Details::check<T>::value>::type
  Print(FILE* out, T t) {
    static std::string f = Formats::Format(t);
    static char const* format = f.c_str();
    std::fprintf(out, format, t);
  }

  template <typename T>
  typename std::enable_if<Details::check<T>::value>::type
  Println(FILE* out, T t) {
    static std::string end = Formats::Format(t) + std::string("\n");
    static char const* format = end.c_str();
    std::fprintf(out, format, t);
  }


  template <typename T>
  typename std::enable_if<!Details::check<T>::value>::type
  Print(const T& t)
    { t.print(); }

  template <typename T>
  typename std::enable_if<!Details::check<T>::value>::type
  Println(const T& t)
    { t.println(); }

  template <typename T>
  typename std::enable_if<!Details::check<T>::value>::type
  Print(FILE* out, const T& t)
    { t.print(out); }

  template <typename T>
  typename std::enable_if<!Details::check<T>::value>::type
  Println(FILE* out, const T& t)
    { t.println(out); }

} // namespace PrintTypes

#endif // SIMPLE_PRINT_FORMATS_H
