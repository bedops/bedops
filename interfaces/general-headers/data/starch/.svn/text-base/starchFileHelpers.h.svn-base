//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchFileHelpers.h
//=========

#ifndef STARCHFILEHELPERS_H
#define STARCHFILEHELPERS_H

#define STARCH_FATAL_ERROR -1

/* 
   On Darwin, file I/O is 64-bit by default (OS X 10.5 at least) so we use standard 
   types and calls 

   Via Terry Lambert <tlambert@apple.com>: 

   ``off_t is 64 bit by default in Mac OS X, so it's not necessary to use special 
     calls to obtain large file support.  You will still need to use fseeko() 
     instead of fseek(), unless you compile your code 64 bit (fseek() takes a long 
     instead of an off_t because the standard requires it).''
*/

#ifdef __APPLE__
#define fopen64 fopen
#define off64_t off_t
#define fseeko64 fseeko
#endif

FILE*   STARCH_fopen(const char *filename, 
                     const char *type);

int     STARCH_fseeko(FILE *stream, 
                     off_t offset, 
                       int whence);

int     STARCH_gzip_deflate(FILE *source, 
                            FILE *dest, 
                             int level);

#endif
