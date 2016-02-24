//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchFileHelpers.h
//=========

//
//    BEDOPS
//    Copyright (C) 2011-2016 Shane Neph, Scott Kuehn and Alex Reynolds
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

/*
   In Cygwin, there is no support for fseeko(). We can use fseek() on a 64-bit 
   distribution of Cygwin, because a signed long int has sufficient bits to store 
   an offset for a file greater than 2 GB. For 32-bit distributions of Cygwin, we
   do not yet have support for this arrangement.
*/

#ifdef __CYGWIN__
#define fseeko fseek
#endif

#ifdef __CYGWIN32__
#error 32-bit Cygwin compilation is unsupported due to lack of fseeko() support
#endif

#ifdef __cplusplus
namespace starch {
#endif

FILE*   STARCH_fopen(const char *filename, 
                     const char *type);

int     STARCH_fseeko(FILE *stream, 
                     off_t offset, 
                       int whence);

int     STARCH_gzip_deflate(FILE *source, 
                            FILE *dest, 
                             int level);

#ifdef __cplusplus
} // namespace starch
#endif

#endif
