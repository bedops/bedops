/*
  FILE: SingletonType.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Fri Aug 10 15:04:51 PDT 2007
  PROJECT: utility
  ID: $Id: SingletonType.hpp 1890 2011-01-28 02:20:13Z sjn $
*/

#ifndef SINGLETONTYPE_H
#define SINGLETONTYPE_H

//============================================================================//

namespace Ext 
{
  template <typename T>
  struct SingletonType {
    static T* Instance() {
      static T t;
      return(&t);
    }
  };
}


#endif // SINGLETONTYPE_H
