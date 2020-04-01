/*
  Author: Shane Neph, Scott Kuehn
  Date:   Thu Sep 27 10:50:39 PDT 2007
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

#ifndef SWEEP_VISITORS_HPP
#define SWEEP_VISITORS_HPP

#include "algorithm/WindowSweep.hpp"

namespace Visitors {

  // Visitor that should be inherited when using sweep() algorithm.
  template <typename Ref, typename Map = Ref>
  struct Visitor {
    typedef Ref RefType;
    typedef Map MapType;

    // interface for sweep()
    inline bool ManagesOwnMemory() const { return(false); }
    inline void OnAdd(MapType* u) { Add(u); }
    inline void OnDelete(MapType* u) { Delete(u); }
    inline void OnDone() { DoneReference(); }
    inline void OnEnd() { End(); }
    inline void OnPurge() { Purge(); }
    inline void OnStart(RefType* t) { SetReference(t); }

  public:
    Visitor() { /* */ }
    template <typename U> explicit Visitor(const U&) { /* */ }
    virtual ~Visitor() { /* */ }

    // Derived Class interface : Must be public due to MultiVisitor-type usage
    virtual void Delete(MapType*) = 0;
    virtual void Add(MapType*) = 0;
    virtual void DoneReference() = 0;
    virtual void SetReference(RefType*) { /* */ }
    virtual void End() { /* */ }
    virtual void Purge() { /* */ }
  };
 
} // namespace Visitors

#endif // SWEEP_VISITORS_HPP
