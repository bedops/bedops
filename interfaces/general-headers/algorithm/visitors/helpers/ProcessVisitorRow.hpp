/*
  Author: Shane Neph & Scott Kuehn
  Date: Dec. 7, 2009
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

#ifndef _VISITOR_POST_PROCESSING_
#define _VISITOR_POST_PROCESSING_

#include <iostream>
#include <ostream>
#include <string>

#include "data/measurement/NaN.hpp"
#include "utility/PrintTypes.hpp"

namespace Visitors {

  namespace Helpers {

    //=============
    // DoNothing()
    //=============
    struct DoNothing {
      void operator()(...) const { /* */ }
    };


    //======
    // Keep
    //======
    template <typename ValueType>
    struct Keep {
      Keep() : isNan_(false), value_(0)
        { }

      template <typename T>
      void operator()(T* t) {
        value_ = static_cast<ValueType>(*t);
        isNan_ = false;
      }

      template <typename T>
      void operator()(const T& t) {
        value_ = static_cast<ValueType>(t);
        isNan_ = false;
      }

      void operator()(const Signal::NaN&) {
        isNan_ = true;
      }

      bool isNan_;
      ValueType value_;
    };

    //=========
    // Print()
    //=========
    struct Print {
      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Print(*t);
      }

      template <typename T>
      void operator()(const T& t) const {
        PrintTypes::Print(t);
      }
    };

    //===========
    // Println()
    //===========
    struct Println {
      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Println(*t);
      }

      template <typename T>
      void operator()(const T& t) const {
        PrintTypes::Println(t);
      }
    };

    //============
    // PrintDelim
    //============
    struct PrintDelim {
      explicit PrintDelim(const std::string& delim = "|")
        : delim_(delim), isChar_(false) {
        bool isaTab = (delim == "\t" || delim == "\\t" || delim == "'\t'");
        bool isaNewline = (delim == "\n" || delim == "\\n" || delim == "'\n'");
        if ( isaTab ) {
          delim_ = "";
          delim_ += '\t';
          isChar_ = true;
        }
        else if ( isaNewline ) {
          delim_ = "";
          delim_ += '\n';
          isChar_ = true;
        } else if ( 1 == delim.size() ) {
          delim_ = "";
          delim_ += delim[0];
          isChar_ = true;
        }
      }

      explicit PrintDelim(char t) : delim_(""), isChar_(true) {
        delim_ += t;
      }

      void operator()() const {
        if ( isChar_ )
          PrintTypes::Print(delim_[0]);
        else
          PrintTypes::Print(delim_.c_str());
      }

    private:
      std::string delim_;
      bool isChar_;
    };

    //==================
    // PrintRangeDelim()
    //===================
    template <typename PrintType>
    struct PrintRangeDelim : private PrintDelim {
      typedef PrintDelim Base;
      typedef PrintType PType;

      explicit PrintRangeDelim(const PrintType& p = PrintType(),
                               const std::string& delim = ";",
                               const std::string& onEmpty = "")
          : Base(delim), pt_(p), oe_(onEmpty)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        if ( beg == end ) {
          if ( !oe_.empty() )
            PrintTypes::Print(oe_.c_str());
          return;
        }
        pt_.operator()(*beg);
        while ( ++beg != end ) {
          Base::operator()();
          pt_.operator()(*beg);
        } // while
      }

    private:
      PrintType pt_;
      std::string oe_;
    };

  } // namespace Helpers

} // namespace Visitors

#endif // _VISITOR_POST_PROCESSING_
