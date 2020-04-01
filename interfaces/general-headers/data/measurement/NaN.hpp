/*
  Author: Scott Kuehn, Shane Neph
  Date:   Mon Sep 10 11:33:57 PDT 2007
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
