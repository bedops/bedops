//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchConstants.c
//=========

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

#include "data/starch/starchConstants.h"

#ifdef __cplusplus
namespace starch {
#endif

const char * kStarchBedHeaderTrack = "track";
const char * kStarchBedHeaderBrowser = "browser";
const char * kStarchBedHeaderSAM = "@";
const char * kStarchBedHeaderVCF = "##";
const char * kStarchBedGenericComment = "#";
const unsigned int kStarchFinalizeTransformTrue = 1;
const unsigned int kStarchFinalizeTransformFalse = 0;
const Boolean kStarchTrue = 1;
const Boolean kStarchFalse = 0;
const char * kStarchNullChromosome = "null";

#ifdef __cplusplus
} // namespace starch
#endif
