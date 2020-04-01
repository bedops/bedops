/*
  Author: Shane Neph & Scott Kuehn
  Date: Sun Aug 19 19:31:30 PDT 2007
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

#ifndef BYLINE_H
#define BYLINE_H

#include <istream>
#include <string>

//==============================================================================
// ByLine structure:
//  Simple extension of the std::string class to allow reading input by lines
//   rather than by whitespace by default.
//==============================================================================

namespace Ext {
  struct ByLine : public std::string {
    friend std::istream& operator>>(std::istream& is, ByLine& b) {
      std::getline(is, b);
      return(is);
    }
  };
} // namespace Ext

#endif // BYLINE_H

