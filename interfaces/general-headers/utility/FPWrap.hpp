//==========
// Author  : Shane Neph & Scott Kuehn
// Date    : Sat Aug 25 01:22:03 PDT 2007
// Project : FILE pointer lifetime ownership
// ID      : $Id$
//==========

#ifndef FP_WRAPPER_HPP
#define FP_WRAPPER_HPP

// Files included
#include <cstddef>
#include <cstdio>
#include <string>

#include "utility/Assertion.hpp"


namespace Ext {

  template <typename IOError>
  struct FPWrap {
    FPWrap() : fp_(NULL)
      { /* */ }

    explicit FPWrap(const std::string& file, const std::string& mode = "r")
     : fp_( (file == "-") ? stdin : std::fopen(file.c_str(), mode.c_str()) ) {
      Assert<IOError>(fp_ && fp_ != NULL, "Unable to find file: " + file);
    }

    inline operator FILE*()
      { return fp_; }

    inline void Open(const std::string& file, const std::string& mode = "r") {
      if ( fp_ && fp_ != NULL )
        Close(); // file already open -> close it
      if ( file == "-" )
        fp_ = stdin;
      else
        fp_ = std::fopen(file.c_str(), mode.c_str());
      Assert<IOError>(fp_ && fp_ != NULL, "Unable to find file: " + file);
    }

    inline void Close()
      { if ( fp_ && fp_ != NULL ) std::fclose(fp_); fp_ = NULL; }

    ~FPWrap()
      { if ( fp_ && fp_ != NULL ) std::fclose(fp_); }

  private:
    FILE* fp_;
  };


} // namespace Ext


#endif // FP_WRAPPER_HPP
