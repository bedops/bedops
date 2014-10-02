/*
  Author: Shane Neph & Scott Kuehn
  Date:   Sun Aug 19 11:39:47 PDT 2007
*/
//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013, 2014 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef SIMPLE_C_FORMATS_H
#define SIMPLE_C_FORMATS_H

#include <cinttypes>

namespace Formats {

  extern char const* Format(char const*);
  extern char const* Format(char);
  extern char const* Format(double);
  extern char const* Format(float);
  extern char const* Format(int);
  extern char const* Format(unsigned int);
  extern char const* Format(long int);
  extern char const* Format(unsigned long int);
  extern char const* Format(long long int);
  extern char const* Format(unsigned long long int);
  extern char const* Format(short);
  extern char const* Format(unsigned short);
  extern char const* Format(uint64_t);

  extern char const* Format(double d, int precision, bool scientific);

} // namespace Formats


#endif // SIMPLE_C_FORMATS_H
