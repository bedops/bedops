/*
  Author: Scott Kuehn, Shane Neph
  Date: Tue Aug 28 15:31:19 PDT 2007
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

#include <map>
#include <string>

namespace Ext {
  template < 
            typename ObjType, 
            typename IdentifierType, 
            typename ObjCreator
           >
  struct Factory {

    ObjType* create(const IdentifierType& id) {
      typename CallbackRegistry::const_iterator cbi = registry_.find(id);
      return (cbi != registry_.end()) ? cbi->second : 0;
    }

    bool registration(const IdentifierType& id, ObjCreator createFunc) { 
      return registry_.insert(std::make_pair(id, createFunc)).second; 
    }

    bool cancelation(const IdentifierType& id) {
      return registry_.erase(id) == 1; 
    }

    bool isRegistered(const IdentifierType& id) {
      return registry_.count(id) == 1;
    }

  protected:
    typedef std::map< IdentifierType, ObjCreator > CallbackRegistry;
    CallbackRegistry registry_;
  };

} // namespace Ext
