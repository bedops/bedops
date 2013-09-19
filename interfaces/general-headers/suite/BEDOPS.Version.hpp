/*
  FILE: BEDOPS.Version.hpp
  AUTHOR: Shane Neph & Alex Reynolds
  CREATE DATE: Thu Jun  2 13:21:05 PDT 2011
  PROJECT: BEDOPS suite
  ID: $Id:$
*/

// Macro Guard
#ifndef REVISION_HISTORY_BEDOPS_H
#define REVISION_HISTORY_BEDOPS_H

#ifdef __cplusplus
namespace BEDOPS {
#endif

  static const char* revision() {
    return("2.3.0");
  }

  static const char* citation() {
    return("http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract");
  }

#ifdef __cplusplus
} // namespace BEDOPS
#endif

#endif // REVISION_HISTORY_BEDOPS_H
