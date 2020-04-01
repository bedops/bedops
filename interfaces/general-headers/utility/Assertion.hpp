/*
  Author: Shane Neph & Scott Kuehn
  Date:   Tue Aug  7 09:12:26 PDT 2007
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

#ifndef ASSERTIONMECHANISM_H
#define ASSERTIONMECHANISM_H

#include <string>

namespace Ext {

  //==============================================================================
  // Assert<ExcType>(bool, error)
  //  A simple runtime assertion mechanism.  When assertion fails, throw ExcType.
  //==============================================================================
  template <typename ExcType>
  struct Assert {
    /* NOTE: not default constructable */
    
    // Constructor
    Assert(bool b, const std::string& msg) {
      if ( b ) return;
      ExcType e(msg);
      throw(e);
    }
    
    // Constructor overload
    Assert(bool b, const std::string& msg1, const std::string& msg2) {
      Assert<ExcType>(b, msg1 + "\n" + msg2); // Call overload
    }
  };

} // namespace Ext

#endif // ASSERTIONMECHANISM_H
