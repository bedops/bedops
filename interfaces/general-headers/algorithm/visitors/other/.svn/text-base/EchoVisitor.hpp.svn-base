/*
  FILE: EchoVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Sat Nov  3 07:19:47 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef _ECHO_MAP_VISITOR_HPP
#define _ECHO_MAP_VISITOR_HPP



namespace Visitors {

  template <
            typename Process,
            typename BaseVisitor
           >
  struct Echo : BaseVisitor {
    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseVisitor::reference_type RefType;
    typedef typename BaseVisitor::mapping_type MapType;

    explicit Echo(const ProcessType& pt = ProcessType())
      : pt_(pt),  ref_(0)
      { /* */ }

    inline void SetReference(RefType* t) {
      ref_ = t;
    }

    inline void Add(MapType*) { /* */ }
    inline void Delete(MapType*) { /* */ }

    inline void DoneReference() {
      pt_.operator()(ref_);
    }

    virtual ~Echo() { }

  protected:
    ProcessType pt_;
    RefType* ref_;
  };

} // namespace Visitors

#endif // _ECHO_MAP_VISITOR_HPP
