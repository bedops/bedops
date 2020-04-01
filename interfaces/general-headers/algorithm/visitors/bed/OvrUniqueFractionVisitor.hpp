/*
  Author: Scott Kuehn, Shane Neph
  Date:   Wed Sep  5 09:33:15 PDT 2007
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

#ifndef OVR_UNIQUE_FRACT_VISITOR_HPP
#define OVR_UNIQUE_FRACT_VISITOR_HPP

#include "algorithm/visitors/bed/OvrUniqueVisitor.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"

namespace Visitors {

  namespace BedSpecific {
  
    template <
              typename Process,
              typename BaseVisitor
             >
    struct OvrUniqueFract : OvrUnique<Visitors::Helpers::Keep<double>, BaseVisitor> {
      typedef OvrUnique<Visitors::Helpers::Keep<double>, BaseVisitor> BaseClass;
      typedef Process ProcessType;

      OvrUniqueFract(const ProcessType& pt = ProcessType()) : BaseClass(Visitors::Helpers::Keep<double>()), prt_(pt)
        { /* */ }

      // Just give the fraction of refItem_'s bases covered
      inline void DoneReference() {
        BaseClass::DoneReference();
        prt_.operator()(BaseClass::pt_.value_ / BaseClass::refItem_->length());
      }

      virtual ~OvrUniqueFract() { /* */ }

    protected:
      ProcessType prt_;
    };

  } // namespace BedSpecific

} // namespace Visitors


#endif // OVR_UNIQUE_FRACT_VISITOR_HPP
