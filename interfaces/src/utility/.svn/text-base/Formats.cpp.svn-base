/*
  FILE: Formats.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Mon Aug 20 10:22:26 PDT 2007
  PROJECT: utility
  ID: $Id:$
*/

#include <cstdio>
#include <cstring>

#include "utility/Formats.hpp"

namespace Formats {

  char const* Format(char const*) { return("%s"); }
  char const* Format(char) { return("%c"); }
  char const* Format(double) { return("%lf"); }
  char const* Format(float) { return("%f"); }
  char const* Format(int) { return("%d"); }
  char const* Format(unsigned int) { return("%u"); }
  char const* Format(long int) { return ("%ld"); }
  char const* Format(unsigned long int) { return("%lu"); }
  char const* Format(long long int) { return ("%lld"); } /* msft doesn't conform to this standard */
  char const* Format(unsigned long long int) { return("%llu"); } /* msft doesn't conform to this standard */
  char const* Format(short) { return ("%hd"); }
  char const* Format(unsigned short) { return ("%hu"); }

  char const* Format(double, int precision, bool scientific) {
    static char prec[20];
    if ( scientific )
      std::sprintf(prec, "%%.%de", precision);
    else
      std::sprintf(prec, "%%.%dlf", precision);
    return(prec);
  }

} // namespace Formats
