/*
  Author  : Shane Neph & Scott Kuehn
  Date    : Sat Aug 25 01:22:03 PDT 2007
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

#ifndef FP_WRAPPER_HPP
#define FP_WRAPPER_HPP

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
     : fp_( (file == "-") ? stdin : std::fopen(file.c_str(), mode.c_str()) ), name_(file) {
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

    inline const std::string& Name() const { return name_; }

    ~FPWrap()
      { if ( fp_ && fp_ != NULL ) std::fclose(fp_); }

  private:
    FILE* fp_;
    std::string name_;
  };

} // namespace Ext


#endif // FP_WRAPPER_HPP
