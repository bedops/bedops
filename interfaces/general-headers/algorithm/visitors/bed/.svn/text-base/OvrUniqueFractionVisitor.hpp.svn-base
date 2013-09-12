/*
  FILE: OvrUniqueVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Sep  5 09:33:15 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

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
