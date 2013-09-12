/*
  FILE: NaN.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Mon Sep 10 11:33:57 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef NAN_HPP
#define NAN_HPP

#include <ostream>

namespace Signal {

  struct NaN {
    typedef char const* Type;
    friend std::ostream& operator<<(std::ostream& os, const NaN&) {
      os << nan_;
      return(os);
    }

    static Type nan_;
  };

} // namespace Signal

#endif // NAN_HPP
