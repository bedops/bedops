//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchConstants.h
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

#ifndef STARCH_CONSTANTS_H
#define STARCH_CONSTANTS_H

#ifdef __cplusplus
namespace starch {
#endif

typedef int Boolean;

/* 
   Detail custom track headers
   -----------------------------------------------------------------
   cf. http://genome.ucsc.edu/goldenPath/help/customTrack.html
       http://genome.ucsc.edu/FAQ/FAQformat.html#format1

   These keywords are reserved "chromosome" names -- you *cannot* name 
   chromosomes in your BED file with any of these keywords as prefixes. 

   If you do so, then the line will not be transformed per Shane's algorithm 
   and no compression gain is accomplished (beyond the use of raw bzip2 or 
   gzip). Further still, no file offsets get determined and so there is 
   no way to search by chromosome...
*/

extern const char *       kStarchBedHeaderTrack; 
extern const char *       kStarchBedHeaderBrowser; 
extern const char *       kStarchBedHeaderSAM;
extern const char *       kStarchBedHeaderVCF;
extern const char *       kStarchBedGenericComment;

extern const unsigned int kStarchFinalizeTransformTrue;
extern const unsigned int kStarchFinalizeTransformFalse;
extern const Boolean      kStarchTrue;
extern const Boolean      kStarchFalse;
extern const char *       kStarchNullChromosome;

#ifdef __cplusplus
} // namespace starch
#endif

#endif
