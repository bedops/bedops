//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchConstants.h
//=========

#ifndef STARCH_CONSTANTS_H
#define STARCH_CONSTANTS_H

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

#endif
