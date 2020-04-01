//=========
// Author:  Alex Reynolds & Shane Neph
// Project: starch
// File:    starchFileHelpers.c
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

#ifdef __cplusplus
#include <cassert>
#include <cstdlib>
#else
#include <assert.h>
#include <stdlib.h>
#endif
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <zlib.h>

#include "data/starch/starchFileHelpers.h"
#include "data/starch/starchHelpers.h"

#ifdef __cplusplus
namespace starch {
#endif

int 
STARCH_fseeko(FILE *stream, off_t offset, int whence) 
{
#ifdef DEBUG
    /*
    fprintf(stderr, "\n--- STARCH_fseeko() ---\n");
    fprintf(stderr, "\toffset: %" PRId64 "\twhence: %d\n\n", (int64_t) offset, whence);
    */
#endif
    int retValue = fseeko(stream, offset, whence);

    if (retValue != 0) {
        switch (retValue) {
            case EAGAIN:
                fprintf(stderr, "ERROR: EAGAIN - The O_NONBLOCK flag is set for the file descriptor underlying stream, and the process would be delayed in the write operation.\n");
                break;
            case EBADF:
                fprintf(stderr, "ERROR: EBADF - The file descriptor underlying stream is not valid.\n");
                break;
            case EFBIG:
                fprintf(stderr, "ERROR: EFBIG - The file is a regular file and an attempt was made to write at or beyond the offset maximum associated with the corresponding stream.\n");
                break;
            case EINTR:
                fprintf(stderr, "ERROR: EINTR - A signal interrupted the call.\n");
                break;
            case EINVAL:
                fprintf(stderr, "ERROR: EINVAL - The whence argument to fseeko() is invalid.\n");
                break;
            case ENOSPC:
                fprintf(stderr, "ERROR: ENOSPC - There was no free space remaining on the device containing the file.\n");
                break;
            case EOVERFLOW:
                fprintf(stderr, "ERROR: EOVERFLOW - The resulting file offset would be a value which cannot be represented correctly in an object of the requested type.\n");
                break;
            case EPIPE:
                fprintf(stderr, "ERROR: EPIPE - An attempt is made to write to a pipe or FIFO that is not open for reading by any process. A SIGPIPE signal is also sent to the process.\n");
                break;
            default:
                fprintf(stderr, "ERROR: UNKNOWN - Unknown error with fseeko() attempt (%d).\n", retValue);
        }
    }

    return retValue;    
}

FILE * 
STARCH_fopen(const char *filename, const char *type) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_fopen() ---\n");
    fprintf(stderr, "\tfilename: %s\ttype: %s\n\n", filename, type);
#endif
#ifdef __cplusplus
    FILE *fnPtr = nullptr;
#else
    FILE *fnPtr = NULL;
#endif

    fnPtr = fopen(filename, type);
    
#ifdef __cplusplus
    if ((fnPtr == nullptr) && (errno != 0)) {
#else
    if ((fnPtr == NULL) && (errno != 0)) {
#endif
        switch (errno) {
            case EACCES:
                fprintf(stderr, "ERROR: EACCES - Search permission is denied on a component of the path prefix, or the file exists and the permissions specified by mode are denied, or the file does not exist and write permission is denied for the parent directory of the file to be created\n");
                break;
            case EINTR:
                fprintf(stderr, "ERROR: EINTR - A signal was caught during the execution of fopen()\n");
                break;
            case EISDIR:
                fprintf(stderr, "ERROR: EISDIR - The named file is a directory and mode requires write access\n");
                break;
            case ELOOP:
                fprintf(stderr, "ERROR: ELOOP - Too many symbolic links were encountered in resolving pathname\n");
                break;
            case EMFILE:
                fprintf(stderr, "ERROR: EMFILE - There are {OPEN_MAX} file descriptors currently open in the calling process\n");
                break;
            case ENAMETOOLONG:
                fprintf(stderr, "ERROR: ENAMETOOLONG - The length of the filename exceeds PATH_MAX or a pathname component is longer than NAME_MAX\n");
                break;
            case ENFILE:
                fprintf(stderr, "ERROR: ENFILE - The maximum allowable number of files is currently open in the system\n");
                break;
            case ENOENT:
                fprintf(stderr, "ERROR: ENOENT - A component of the filename does not name an existing file or filename is an empty string\n");
                break;
            case ENOSPC:
                fprintf(stderr, "ERROR: ENOSPC - The directory or file system that would contain the new file cannot be expanded, the file does not exist, and it was to be created\n");
                break;
            case ENOTDIR:
                fprintf(stderr, "ERROR: ENOTDIR - A component of the path prefix is not a directory\n");
                break;
            case ENXIO:
                fprintf(stderr, "ERROR: ENXIO - The named file is a character special or block special file, and the device associated with this special file does not exist\n");
                break;
            case EOVERFLOW:
                fprintf(stderr, "ERROR: EOVERFLOW - The current value of the file position cannot be represented correctly in an object of type fpos_t\n");
                break;
            case EROFS:
                fprintf(stderr, "ERROR: EROFS - The named file resides on a read-only file system and the mode requires write access\n");
                break;
            case ENOMEM:
                fprintf(stderr, "ERROR: ENOMEM - Insufficient storage space is available\n");
                break;
            case ETXTBSY:
                fprintf(stderr, "ERROR: ETXTBSY - The file is a pure procedure (shared text) file that is being executed and mode requires write access\n");
                break;
            default:
#ifdef __cplusplus
                fprintf(stderr, "ERROR: UNKNOWN - Run into unknown file access error (%d)\n", static_cast<int>( errno ));
        }
        return nullptr;
#else
                fprintf(stderr, "ERROR: UNKNOWN - Run into unknown file access error (%d)\n", (int) errno);
        }
        return NULL;
#endif
    }

    return fnPtr;
}

int 
STARCH_gzip_deflate(FILE *source, FILE *dest, int level) 
{
#ifdef DEBUG
    fprintf(stderr, "\n--- STARCH_gzip_deflate() ---\n");
#endif
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[STARCH_Z_CHUNK + 1];
    unsigned char out[STARCH_Z_CHUNK + 1];

    /* allocate deflate state */
#ifdef __cplusplus
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
#else
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
#endif

    /* deflateInit2 allows creation of archive with gzip header, i.e. a gzip file */
    /* cf. http://www.zlib.net/manual.html */
#ifdef __cplusplus
    ret = deflateInit2cpp(&strm, level, Z_DEFLATED, (15+16), 8, Z_DEFAULT_STRATEGY);
#else
    ret = deflateInit2(&strm, level, Z_DEFLATED, (15+16), 8, Z_DEFAULT_STRATEGY);
#endif
    if (ret != Z_OK) 
        return ret;

    /* compress until end of file */
    do {
#ifdef __cplusplus
        strm.avail_in = static_cast<unsigned int>( fread(in, 1, STARCH_Z_CHUNK, source) );
#else
	strm.avail_in = (unsigned int) fread(in, 1, STARCH_Z_CHUNK, source);
#endif   

        if (ferror(source)) {
            (void) deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        
        do {
            strm.avail_out = STARCH_Z_CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            assert(ret != Z_STREAM_ERROR);
            have = STARCH_Z_CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void) deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);

    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);

    /* clean up and return */
    (void) deflateEnd(&strm);
    return Z_OK;
}

#ifdef __cplusplus
} // namespace starch
#endif
