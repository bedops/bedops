/*
  FILE: Factory.hpp
  AUTHOR: Scott Kuehn, Shane Neph
  CREATE DATE: Tue Aug 28 15:31:19 PDT 2007
  PROJECT: utility
  ID: $Id$
*/

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

}
