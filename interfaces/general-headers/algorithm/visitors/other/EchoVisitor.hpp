/*
  FILE: EchoVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Sat Nov  3 07:19:47 PDT 2007
  PROJECT: utility
  ID: $Id$
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
    typedef typename BaseVisitor::RefType RefType;
    typedef typename BaseVisitor::MapType MapType;

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
