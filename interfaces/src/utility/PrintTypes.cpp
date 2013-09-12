/*
  FILE: PrintTypes.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Wed Sep  5 11:16:29 PDT 2007
  PROJECT: utility
  ID: $Id:$
*/

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
