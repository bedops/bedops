/*
  Author: Shane Neph & Scott Kuehn
  Date:   Fri Aug 10 15:04:51 PDT 2007
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

#ifndef SINGLETONTYPE_H
#define SINGLETONTYPE_H

namespace Ext {
  template <typename T>
  struct SingletonType {
    static T& Instance() {
      if ( !_instance )
        _instance = new T();
      return *_instance;
    }

  private:
    SingletonType();
    ~SingletonType() {}
    SingletonType(const SingletonType&); // undefined
    SingletonType& operator=(const SingletonType&); // undefined

    void Cleanup();
    static T* _instance;
  };
} // namespace Ext

#include "../../src/utility/SingletonType.cpp"

#endif // SINGLETONTYPE_H
