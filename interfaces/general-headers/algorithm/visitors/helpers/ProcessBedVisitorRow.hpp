/*
  Author: Shane Neph & Scott Kuehn
  Date:   Dec. 7, 2009
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

#ifndef _VISITOR_BED_POST_PROCESSING_
#define _VISITOR_BED_POST_PROCESSING_

#include <set>
#include <string>
#include <type_traits>

#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "data/bed/Bed.hpp"
#include "data/bed/BedCompare.hpp"
#include "data/measurement/NaN.hpp"
#include "utility/Formats.hpp"
#include "utility/PrintTypes.hpp"

namespace Visitors {

  namespace BedHelpers {

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

      void operator()(const Signal::NaN& s) const {
        PrintTypes::Print(s.nan_);
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

      void operator()(const Signal::NaN& s) const {
        PrintTypes::Println(s.nan_);
      }
    };

    //=============
    // PrintBED3()
    //=============
    struct PrintBED3 {
      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        // Can't use t->print() since only want first 3 columns
        PrintTypes::Print(t->chrom());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->start());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->end());
      }

      template <typename T>
      void operator()(const T& t) const {
        operator()(&t);
      }

      void operator()(const Signal::NaN&) const {
        static char const* noMatch = "No-Match";
        PrintTypes::Print(noMatch);
      }
    };

    //===============
    // PrintBED3ln()
    //===============
    struct PrintBED3ln {
      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        // Can't use t->println() since only want first 3 columns
        PrintTypes::Print(t->chrom());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->start());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->end());
        PrintTypes::Print('\n');
      }

      template <typename T>
      void operator()(const T& t) const {
        operator()(&t);
      }

      void operator()(const Signal::NaN&) const {
        static char const* noMatch = "No-Match";
        PrintTypes::Print(noMatch);
      }
    };

    //==============
    // PrintScore()
    //==============
    struct PrintScore {
      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        PrintTypes::Print(t->measurement());
      }

      template <typename T>
      void operator()(const T& t) const {
        PrintTypes::Print(t);
      }

      void operator()(const Signal::NaN& s) const {
        PrintTypes::Print(s.nan_);
      }
    };

    //=======================
    // PrintScorePrecision()
    //=======================
    struct PrintScorePrecision {
      PrintScorePrecision(int precision, bool inScientific)
        : precision_(precision), scientific_(inScientific)
        { /* */ }

      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        static char const* format = Formats::Format(t->measurement(), precision_, scientific_);
        std::printf(format, t->measurement());
      }

      template <typename T>
      void operator()(const T& t) const {
        static char const* format = Formats::Format(t, precision_, scientific_);
        std::printf(format, t);
      }

      void operator()(const Signal::NaN& s) const {
        PrintTypes::Print(s.nan_);
      }

    protected:
      int precision_;
      bool scientific_;
    };

    //==========================
    // PrintAllScorePrecision()
    //==========================
    struct PrintAllScorePrecision {
      PrintAllScorePrecision(int precision, bool inScientific, const std::string& onEmpty = "")
        : precision_(precision), scientific_(inScientific), oe_(onEmpty)
        { /* */ }

      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        static char const* format = Formats::Format(t->measurement(), precision_, scientific_);
        PrintTypes::Print(t->chrom());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->start());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->end());
        PrintTypes::Print('\t');
        PrintTypes::Print(t->id());
        PrintTypes::Print('\t');
        std::printf(format, t->measurement());
        printRest(t);
      }

      template <typename T>
      void operator()(const T& t) const {
        operator()(&t);
      }

      void operator()(const Signal::NaN& s) const {
        if ( !oe_.empty() )
          PrintTypes::Print(oe_.c_str());
      }

    protected:
      template <typename T>
      typename std::enable_if<T::UseRest, void>::type printRest(T* t) const {
        // PrintTypes::Print(t->rest()); // already includes '\t' out front
        // t->full_rest() now includes t->id() and t->measurement(), exclude here
        if ( t->full_rest()[0] != '\0' && t->rest_offset() >= 0 )
          PrintTypes::Print(t->full_rest() + t->rest_offset());
      }

      template <typename T>
      typename std::enable_if<!T::UseRest, void>::type printRest(T* t) const { /* */ }

    protected:
      int precision_;
      bool scientific_;
      std::string oe_;
    };

    //================
    // PrintlnScore()
    //================
    struct PrintlnScore {
      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        PrintTypes::Println(t->measurement());
      }

      template <typename T>
      void operator()(const T& t) const {
        PrintTypes::Println(t);
      }

      void operator()(const Signal::NaN& s) const {
        PrintTypes::Println(s.nan_);
      }
    };

    //=========================
    // PrintScorePrecisionln()
    //=========================
    struct PrintScorePrecisionln {
      PrintScorePrecisionln(int precision, bool inScientific)
        : precision_(precision), scientific_(inScientific)
        { /* */ }

      template <typename T>
      void operator()(T* t) const { // could use dis/enable_if for built-ins
        static std::string f = (std::string(Formats::Format(t->measurement(), precision_, scientific_)) + "\n");
        static char const* format = f.c_str();
        std::printf(format, t->measurement());
      }

      template <typename T>
      void operator()(const T& t) const {
        static std::string f = (std::string(Formats::Format(t, precision_, scientific_)) + "\n");
        static char const* format = f.c_str();
        std::printf(format, t);
      }

      void operator()(const Signal::NaN& s) const {
        PrintTypes::Println(s.nan_);
      }

    protected:
      int precision_;
      bool scientific_;
    };

    //===========
    // PrintID()
    //===========
    struct PrintID {
      typedef std::string Type;

      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Print(t->id());
      }

      void operator()(const Signal::NaN&) const {
        static char const* noID = "No-ID";
        PrintTypes::Print(noID);
      }
    };

    //=============
    // PrintlnID()
    //=============
    struct PrintlnID {
      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Println(t->id());
      }

      void operator()(const Signal::NaN&) const {
        static char const* noID = "No-ID";
        PrintTypes::Println(noID);
      }
    };

    //===============
    // PrintLength()
    //===============
    struct PrintLength {
      typedef std::string Type;

      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Print(t->length());
      }
    };

    //=================
    // PrintlnLength()
    //=================
    struct PrintlnLength {
      typedef std::string Type;

      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Println(t->length());
      }
    };

    //=================
    // PrintSpanName()
    //=================
    struct PrintSpanName {
      template <typename T>
      void operator()(T* t) const {
        PrintTypes::Print(t->chrom());
        PrintTypes::Print(':');
        PrintTypes::Print(t->start());
        PrintTypes::Print('-');
        PrintTypes::Print(t->end());
      }
    };

    //==============
    // PrintRowID()
    //==============
    struct PrintRowID {
      template <typename T>
      void operator()(T* t) const {
        static char const* id = "id-";
        static unsigned long rowID = 0;
        static unsigned long subRowID = 0;
        static T last;
        static Bed::GenomicRestCompare<T> grc;
        if ( !grc(&last, t) && !grc(t, &last) ) { // equal
          static constexpr unsigned long sz = 1000;
          static char formatted[sz+1];
          formatted[0] = '\0';
          std::snprintf(formatted, sz, "%s%lu.%06lu", id, rowID, ++subRowID);
          PrintTypes::Print(formatted);
          return;
        }
        last = *t;
        PrintTypes::Print(id);
        PrintTypes::Print(++rowID);
        subRowID = 0;
      }
    };

    //=======================
    // PrintUniqueRangeIDs()
    //  : sorting and uniquing -> will not be in genomic order
    //=======================
    struct PrintUniqueRangeIDs : private Visitors::Helpers::PrintDelim {
      typedef Visitors::Helpers::PrintDelim Base;

      explicit PrintUniqueRangeIDs(const std::string& delim = ";", const std::string& onEmpty = "")
          : Base(delim), oe_(onEmpty)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        typedef std::set<std::string> SortType;
        if ( beg == end ) {
          if ( !oe_.empty() )
            PrintTypes::Print(oe_.c_str());
          return;
        }
        SortType srt;
        while ( beg != end ) {
          srt.insert((*beg)->id());
          ++beg;
        } // while

        typename SortType::iterator i = srt.begin();
        PrintTypes::Print(i->c_str());
        while ( ++i != srt.end() ) {
          Base::operator()();
          PrintTypes::Print(i->c_str());
        } // while
      }

    private:
      std::string oe_;
    };

    //=========================
    // PrintSortedRangeDelim()
    //=========================
    template <typename PrintType>
    struct PrintSortedRangeDelim : private Visitors::Helpers::PrintDelim {
      typedef Visitors::Helpers::PrintDelim Base;
      typedef PrintType PType;

      explicit PrintSortedRangeDelim(const PrintType& p = PrintType(),
                                     const std::string& delim = ";")
          : Base(delim), pt_(p)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        // It is possible that beg->end is not in an order of the
        //  original input file (sorted by sort-bed), due to issue of
        //  BED types (see BedBaseVisitor.hpp).  Put things back here.
        typedef std::set<
                      typename Iter::value_type,
                      Bed::GenomicAddressCompare<
                           typename std::remove_pointer<typename Iter::value_type>::type,
                           typename std::remove_pointer<typename Iter::value_type>::type
                                                >
                        > SortType; // will be consistent with sort-bed
        if ( beg == end )
          return;
        SortType srt(beg, end);

        typename SortType::iterator i = srt.begin();
        pt_.operator()(*i);
        while ( ++i != srt.end() ) {
          Base::operator()();
          pt_.operator()(*i);
        } // while
      }

    private:
      PrintType pt_;
    };

    //=====================
    // PrintGenomicRange()
    //=====================
    template <typename PrintType>
    struct PrintGenomicRange {
      typedef PrintType PType;

      explicit PrintGenomicRange(const PrintType& p = PrintType(), const std::string& onEmpty = "")
          : pt_(p), oe_(onEmpty)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        // It is often possible/likely that beg->end is not in an order
        //  of the original input file (sorted by sort-bed), due to
        //  issues of BED types (see BedBaseVisitor.hpp).
        if ( beg == end ) {
          if ( !oe_.empty() )
            PrintTypes::Print(oe_.c_str());
          return;
        }

        typename std::remove_const<typename std::remove_pointer<typename Iter::value_type>::type>::type val = **beg;
        while ( ++beg != end ) {
          if ( val.start() > (*beg)->start() )
            val.start((*beg)->start());
          if ( val.end() < (*beg)->end() )
            val.end((*beg)->end());
        } // while

        pt_.operator()(val);
      }

    private:
      PrintType pt_;
      std::string oe_;
    };

  } // namespace BedHelpers

} // namespace Visitors

#endif // _VISITOR_BED_POST_PROCESSING_
