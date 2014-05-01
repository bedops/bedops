/*
  FILE: Visitors.hpp
  AUTHOR: Shane Neph, Scott Kuehn
  CREATE DATE: Thu Sep 27 10:50:39 PDT 2007
  PROJECT: utility
  ID: $Id$
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
