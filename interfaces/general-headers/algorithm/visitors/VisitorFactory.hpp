/*
  Author: Scott Kuehn, Shane Neph
  Date:   Thu Sep 27 16:47:55 PDT 2007
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

#ifndef VISITORFACTORY_HPP
#define VISITORFACTORY_HPP

#include <string>
#include "algorithm/Visitors.hpp"
#include "utility/Factory.hpp"
#include "utility/SingletonType.hpp"

namespace Visitors {

  template <
            typename T1,
            typename T2 = T1
            typename BaseVisitor = Visitors::Visitor<T1, T2>
           >
  struct VisitorTraits {
    typedef T1 RefType;
    typedef T2 MapType;
    typedef BaseVisitor BaseClass;
    typedef BaseClass* (*VisitorCreationFunc)();
    typedef Ext::SingletonType< Ext::Factory<BaseClass, std::string, VisitorCreationFunc> > VisitorFactory;
  };

} // namespace Visitors

#endif // VISITORFACTORY_HPP
