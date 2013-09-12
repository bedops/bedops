/*
  FILE: Assertion.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Tue Aug  7 09:12:26 PDT 2007
  PROJECT: utility
  ID: $Id: Assertion.hpp 1531 2010-07-25 03:45:25Z sjn $
*/

// Macro guard
#ifndef ASSERTIONMECHANISM_H
#define ASSERTIONMECHANISM_H

// Files included
#include <string>

namespace Ext
{

  //==============================================================================
  // Assert<ExcType>(bool, error)
  //  A simple runtime assertion mechanism.  When assertion fails, throw ExcType.
  //==============================================================================
  template <typename ExcType>
  struct Assert {
    /* NOTE: not default constructable */
    
    // Constructor
    Assert(bool b, const std::string& msg) {
      if ( b ) return;
      ExcType e(msg);
      throw(e);
    }
    
    // Constructor overload
    Assert(bool b, const std::string& msg1, const std::string& msg2) {
      Assert<ExcType>(b, msg1 + "\n" + msg2); // Call overload
    }
  };

} // namespace ext

#endif // ASSERTIONMECHANISM_H
