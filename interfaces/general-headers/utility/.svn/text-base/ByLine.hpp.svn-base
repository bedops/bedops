// File: ByLine.hpp
// Author: Shane Neph & Scott Kuehn
// Date: Sun Aug 19 19:31:30 PDT 2007
// Project: general utilities

// Macro guard
#ifndef BYLINE_H
#define BYLINE_H

// Files included
#include <istream>
#include <string>

//==============================================================================
// ByLine structure:
//  Simple extension of the std::string class to allow reading input by lines
//   rather than by whitespace by default.
//==============================================================================

namespace Ext
{
  struct ByLine : public std::string {
    friend std::istream& operator>>(std::istream& is, ByLine& b) {
      std::getline(is, b);
      return(is);
    }
  };
}

#endif // BYLINE_H

