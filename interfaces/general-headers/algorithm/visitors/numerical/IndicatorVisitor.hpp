/*
  Author: Shane Neph, Scott Kuehn
  Date:   Fri Jun 15 15:38:10 PDT 2012
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

#ifndef INDICATOR_VISITOR_HPP
#define INDICATOR_VISITOR_HPP

#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "algorithm/visitors/numerical/CountVisitor.hpp"

namespace Visitors {

  // Indicate the occurrence of overlaps
  template <
            typename Process,
            typename BaseVisitor
           >
  struct Indicator : Count<Visitors::Helpers::DoNothing, BaseVisitor> {

    typedef Count<Visitors::Helpers::DoNothing, BaseVisitor> BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;

    explicit Indicator(const ProcessType& = ProcessType())
        : BaseClass(Visitors::Helpers::DoNothing())
      { /* */ }

    inline void DoneReference() {
      pt_.operator()(BaseClass::count_ > 0 ? 1 : 0);
    }

    virtual ~Indicator() { }

  protected:
    ProcessType pt_;
  };

} // namespace Visitors

#endif // INDICATOR_VISITOR_HPP
