/*
  FILE: CountVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Wed Sep  5 09:23:00 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

#ifndef COUNT_VISITOR_HPP
#define COUNT_VISITOR_HPP


namespace Visitors {

  // Count the occurrence of overlaps
  template <
            typename Process,
            typename BaseVisitor
           >
  struct Count : BaseVisitor {

    typedef BaseVisitor BaseClass;
    typedef Process ProcessType;
    typedef typename BaseClass::reference_type T;
    typedef typename BaseClass::mapping_type V;

    explicit Count(const ProcessType& pt = ProcessType())
        : pt_(pt), count_(0)
      { /* */ }

    inline void Add(V*)
      { ++count_; }

    inline void Delete(V*)
      { --count_; }

    inline void DoneReference() {
      pt_.operator()(count_);
    }

    inline void End() {
      count_ = 0;
    }

    virtual ~Count() { }

  protected:
    ProcessType pt_;
    int count_;
  };

} // namespace Visitors

#endif // COUNT_VISITOR_HPP
