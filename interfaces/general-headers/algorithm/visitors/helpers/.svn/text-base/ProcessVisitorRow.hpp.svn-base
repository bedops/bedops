/*
  FILE: BedBaseVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Dec. 7, 2009
  PROJECT: utility
  ID: $Id$
*/

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
      Print(std::ostream& os = std::cout, const std::string& fieldSep = "\t")
        : os_(os), fieldSep_(fieldSep)
        { /* */ }
  
      template <typename T>
      void operator()(T* t) {
        os_ << *t;
      }

      template <typename T>
      void operator()(const T& t) {
        os_ << t;
      }
  
      template <typename T, typename U>
      void operator()(const T& t, const U& u) {
        os_ << t << fieldSep_ << u;
      }
  
      template <typename T, typename U, typename V>
      void operator()(const T& t, const U& u, const V& v) {
        os_ << t << fieldSep_ << u << fieldSep_ << v;
      }
  
    protected:
      std::ostream& os_;
      std::string fieldSep_;
    };
  
  
    //===========
    // Println()
    //===========
    struct Println {
      Println(std::ostream& os = std::cout, const std::string& fieldSep = "\t")
        : os_(os), fieldSep_(fieldSep)
        { /* */ }

      template <typename T>
      void operator()(T* t) {
        os_ << *t << std::endl;
      }
  
      template <typename T>
      void operator()(const T& t) {
        os_ << t << std::endl;
      }
  
      template <typename T, typename U>
      void operator()(const T& t, const U& u) {
        os_ << t << fieldSep_ << u << std::endl;
      }
  
      template <typename T, typename U, typename V>
      void operator()(const T& t, const U& u, const V& v) {
        os_ << t << fieldSep_ << u << fieldSep_ << v << std::endl;
      }
  
    protected:
      std::ostream& os_;
      std::string fieldSep_;
    };

    //============
    // PrintDelim
    //============
    struct PrintDelim {
      explicit PrintDelim(const std::string& delim = "|")
        : delim_(delim)
      { /* */ }

      void operator()() const {
        PrintTypes::Print(delim_.c_str());
      }

    private:
      std::string delim_;
    };

  } // namespace Helpers

} // namespace Visitors

#endif // _VISITOR_POST_PROCESSING_
