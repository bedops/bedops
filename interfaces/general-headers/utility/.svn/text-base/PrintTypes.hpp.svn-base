/*
  FILE: PrintTypes.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Wed Sep  5 11:16:29 PDT 2007
  PROJECT: utility
  ID: $Id:$
*/

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
