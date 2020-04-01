/*
  FILE: MultiVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Thu Sep 20 15:00:33 PDT 2007
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


#ifndef MULTIVISITOR_HPP
#define MULTIVISITOR_HPP

#include <algorithm>
#include <functional>
#include <vector>


namespace Visitors {

  template
  <
    typename ProcessFields, // belongs to MultiVisitor only; not contained visitors
    typename ProcessRows, // belongs to MultiVisitor only; not contained visitors
    typename BaseVisitor
  >
  struct MultiVisitor : BaseVisitor {
    typedef BaseVisitor BaseClass;
    typedef ProcessFields ProcessFieldType;
    typedef ProcessRows ProcessRowType;
    typedef typename BaseClass::RefType RefType;
    typedef typename BaseClass::MapType MapType;
    typedef std::vector<BaseClass*> GroupType;

    explicit MultiVisitor(GroupType& visitors,
                          const ProcessFieldType& pFields = ProcessFieldType(),
                          const ProcessRowType& pRows = ProcessRowType(),
                          bool processAll = true)
      : BaseVisitor(), t_(visitors), pFields_(pFields), pRows_(pRows),
        pAll_(processAll), cnt_(0)
      { /* */ }

    template <typename BaseDistType>
    explicit MultiVisitor(GroupType& visitors,
                          const BaseDistType& dist,
                          const ProcessFieldType& pFields = ProcessFieldType(),
                          const ProcessRowType& pRows = ProcessRowType(),
                          bool processAll = true)
      : BaseVisitor(dist), t_(visitors), pFields_(pFields), pRows_(pRows),
        pAll_(processAll), cnt_(0)
      { /* */ }

    void Add(MapType* u) {
      void (BaseClass::*memberFuncPtr)(MapType*) = &BaseClass::Add;
      std::for_each(t_.begin(), t_.end(), std::bind2nd(std::mem_fun(memberFuncPtr), u));
      ++cnt_;
    }
    
    void Delete(MapType* u) {
      void (BaseClass::*memberFuncPtr)(MapType*) = &BaseClass::Delete;
      std::for_each(t_.begin(), t_.end(), std::bind2nd(std::mem_fun(memberFuncPtr), u));
      --cnt_;
    }
    
    void DoneReference() {
      if ( !pAll_ && cnt_ == 0 )
        return;

      typename GroupType::const_iterator i = t_.begin();
      if ( i != t_.end() )
        (*i)->DoneReference();
      else
        return;

      for ( ++i; i != t_.end(); ++i ) {
        pFields_.operator()();
        (*i)->DoneReference();
      } // for
      pRows_.operator()();
    }

    void SetReference(RefType* t) {
      void (BaseClass::*memberFuncPtr)(RefType*) = &BaseClass::SetReference;
      std::for_each(t_.begin(), t_.end(), std::bind2nd(std::mem_fun(memberFuncPtr), t));
    }

    /* not used by BED visitors to date.  Pain if it is with nested elements. */
    /*
    void Purge() {
      std::for_each(t_.begin(), t_.end(), std::mem_fun(&BaseClass::Purge));
    }
    */

    void End() {
      std::for_each(t_.begin(), t_.end(), std::mem_fun(&BaseClass::End));
    }

    virtual ~MultiVisitor() {
      for ( cGtI gi = t_.begin(); gi != t_.end(); ++gi )
        delete *gi;
    }

  protected:
    typedef typename GroupType::const_iterator cGtI;
    GroupType& t_;
    ProcessFields pFields_;
    ProcessRows pRows_;
    const bool pAll_;
    long cnt_;
  };

} // namespace Visitors

#endif // MULTIVISITOR_HPP
