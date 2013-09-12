/*
  FILE: Formats.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Sun Aug 19 11:39:47 PDT 2007
  PROJECT: utility
  ID: $Id:$
*/

// Macro Guard
#ifndef SIMPLE_C_FORMATS_H
#define SIMPLE_C_FORMATS_H

namespace Formats {

  extern char const* Format(char const*);
  extern char const* Format(char);
  extern char const* Format(double);
  extern char const* Format(float);
  extern char const* Format(int);
  extern char const* Format(unsigned int);
  extern char const* Format(long int);
  extern char const* Format(unsigned long int);
  extern char const* Format(long long int);
  extern char const* Format(unsigned long long int);
  extern char const* Format(short);
  extern char const* Format(unsigned short);

  extern char const* Format(double d, int precision, bool scientific);

} // namespace Formats


#endif // SIMPLE_C_FORMATS_H
