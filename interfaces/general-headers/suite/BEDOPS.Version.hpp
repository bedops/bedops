/*
  FILE: BEDOPS.Version.hpp
  AUTHOR: Shane Neph & Alex Reynolds
  CREATE DATE: Thu Jun  2 13:21:05 PDT 2011
  PROJECT: BEDOPS suite
  ID: $Id:$
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

// Macro Guard
#ifndef REVISION_HISTORY_BEDOPS_H
#define REVISION_HISTORY_BEDOPS_H

#include "suite/BEDOPS.Constants.hpp"

#ifdef __cplusplus
namespace BEDOPS {
#endif

  static const char* version() {
    return "2.4.39 " BUILD_OPTS; // preprocessor string concatenation
  }

  static const char* citation() {
    return("http://bioinformatics.oxfordjournals.org/content/28/14/1919.abstract\n            https://doi.org/10.1093/bioinformatics/bts277");
  }

#ifdef __cplusplus
} // namespace BEDOPS
#endif

#endif // REVISION_HISTORY_BEDOPS_H
