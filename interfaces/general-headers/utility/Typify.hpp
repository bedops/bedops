/*
  FILE: Formats.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Thu Nov 29 15:28:17 PST 2007
  PROJECT: utility
  ID: $Id:$
*/


// Macro Guard
#ifndef TYPIFY_CONSTRUCTIONS_H
#define TYPIFY_CONSTRUCTIONS_H

namespace Ext {

  //============
  // Int2Type<> : typify an integer
  //   o good for overloading & specializations purposes
  //============
  template <int Val>
  struct Int2Type {
    enum { val = Val };
  };

  //=============
  // Type2Type<> : typify a type
  //   o lightweight -> good for function overloading purposes
  //   o does not require template T to be constructible
  //=============
  template <typename T>
  struct Type2Type {
    typedef T OriginalType;
  };


  //===========
  // IdType<,> : differentiate multiple type T's by an ID
  //           : this class requires T to be a user-defined type
  //===========
  template <typename T, int ID>
  struct IDType : public T {
    typedef T Type;
    enum { value = ID };
  };

} // namespace Ext

#endif // TYPIFY_CONSTRUCTIONS_H
