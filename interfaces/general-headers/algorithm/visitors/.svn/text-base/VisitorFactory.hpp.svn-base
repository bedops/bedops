/*
  FILE: StatVisitor.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Thu Sep 27 16:47:55 PDT 2007
  PROJECT: utility
  ID: $Id$
*/


#ifndef VISITORFACTORY_HPP
#define VISITORFACTORY_HPP

#include <string>
#include "algorithm/Visitors.hpp"
#include "utility/Factory.hpp"
#include "utility/SingletonType.hpp"

namespace Visitors {

  template <
            typename T1,
            typename T2 = T1
            typename BaseVisitor = Visitors::Visitor<T1, T2>
           >
  struct VisitorTraits {
    typedef T1 RefType;
    typedef T2 MapType;
    typedef BaseVisitor BaseClass;
    typedef BaseClass* (*VisitorCreationFunc)();
    typedef Ext::SingletonType< Ext::Factory<BaseClass, std::string, VisitorCreationFunc> > VisitorFactory;
  };

} // namespace Visitors

#endif // VISITORFACTORY_HPP
