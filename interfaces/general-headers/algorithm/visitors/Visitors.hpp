/*
  FILE: Visitors.hpp
  AUTHOR: Shane Neph, Scott Kuehn
  CREATE DATE: Thu Sep 27 10:50:39 PDT 2007
  PROJECT: utility
  ID: $Id$
*/


#ifndef SWEEP_VISITORS_HPP
#define SWEEP_VISITORS_HPP

#include "algorithm/WindowSweep.hpp"


namespace Visitors {

  // Visitor that should be inherited when using sweep() algorithm.
  template <typename RefType, typename MapType = RefType>
  struct Visitor {
    typedef RefType reference_type;
    typedef MapType mapping_type;

  private:
    template <class I, class R, class E>
    friend void WindowSweep::sweep(I, I, R&, E&);

    template <class I, class J, class R, class E>
    friend void WindowSweep::sweep(I, I, J, J, R&, E&, bool);

    inline virtual bool ManagesOwnMemory() const { return(false); }
    inline virtual void OnAdd(mapping_type* u) { Add(u); }
    inline virtual void OnDelete(mapping_type* u) { Delete(u); }
    inline virtual void OnDone() { DoneReference(); }
    inline virtual void OnEnd() { End(); }
    inline virtual void OnPurge() { Purge(); }
    inline virtual void OnStart(reference_type* t) { SetReference(t); }

  public:
    Visitor() { /* */ }
    template <typename U> explicit Visitor(const U&) { /* */ }
    virtual ~Visitor() { /* */ }

    // Derived Class interface : Must be public due to MultiVisitor-type usage
    virtual void Delete(mapping_type*) = 0;
    virtual void Add(mapping_type*) = 0;
    virtual void DoneReference() = 0;
    virtual void SetReference(reference_type*) { /* */ }
    virtual void End() { /* */ }
    virtual void Purge() { /* */ }
  };
 
} // namespace Visitors

#endif // SWEEP_VISITORS_HPP
