/*
  FILE: CountVisitor.hpp
  AUTHOR: Shane Neph, Scott Kuehn
  CREATE DATE: Fri Jun 15 15:38:10 PDT 2012
  PROJECT: utility
  ID: $Id$
*/

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
    typedef typename BaseClass::reference_type T;
    typedef typename BaseClass::mapping_type V;

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
