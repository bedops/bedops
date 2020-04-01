/*
  Author: Scott Kuehn, Shane Neph
  Date:   Wed Sep  5 09:23:00 PDT 2007
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
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;

    explicit Count(const ProcessType& pt = ProcessType())
        : pt_(pt), count_(0)
      { /* */ }

    inline void Add(MapType*)
      { ++count_; }

    inline void Delete(MapType*)
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
