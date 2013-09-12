/*
  FILE: BedBaseVisitor.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Dec. 7, 2009
  PROJECT: utility
  ID: $Id$
*/

#ifndef _VISITOR_BED_POST_PROCESSING_
#define _VISITOR_BED_POST_PROCESSING_

// Files included
#include <set>
#include <string>

#include <boost/type_traits.hpp>

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

      explicit PrintRangeDelim(const PrintType& p = PrintType(),
                               const std::string& delim = ";")
          : Base(delim), pt_(p)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        if ( beg == end )
          return;
        pt_.operator()(*beg);
        while ( ++beg != end ) {
          Base::operator()();
          pt_.operator()(*beg);
        } // while
      }

    private:
      PrintType pt_;
    };

    //=======================
    // PrintUniqueRangeIDs()
    //  : sorting and uniquing -> will not be in genomic order
    //=======================
    struct PrintUniqueRangeIDs : private PrintDelim {
      typedef PrintDelim Base;

      explicit PrintUniqueRangeIDs(const std::string& delim = ";")
          : Base(delim)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        typedef std::set<std::string> SortType;
        if ( beg == end )
          return;
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
    };

    //=========================
    // PrintSortedRangeDelim()
    //=========================
    template <typename PrintType>
    struct PrintSortedRangeDelim : private PrintDelim {
      typedef PrintDelim Base;
      typedef PrintType PType;

      explicit PrintSortedRangeDelim(const PrintType& p = PrintType(),
                                     const std::string& delim = ";")
          : Base(delim), pt_(p)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        // It is often possible/likely that beg->end is not in an order
        //  of the original input file (sorted by sort-bed), due to
        //  issues of BED types (see BedBaseVisitor.hpp).  Put things
        //  back here.
        typedef std::set<
                      typename Iter::value_type,
                      Bed::GenomicAddressCompare<
                           typename boost::remove_pointer<typename Iter::value_type>::type,
                           typename boost::remove_pointer<typename Iter::value_type>::type
                                                >
                        > SortType; // will be consistent with sort-bed
        if ( beg == end )
          return;
        SortType srt;
        while ( beg != end )
          srt.insert(*beg++);

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

      explicit PrintGenomicRange(const PrintType& p = PrintType())
          : pt_(p)
        { /* */ }

      template <typename Iter>
      void operator()(Iter beg, Iter end) const {
        // It is often possible/likely that beg->end is not in an order
        //  of the original input file (sorted by sort-bed), due to
        //  issues of BED types (see BedBaseVisitor.hpp).
        if ( beg == end )
          return;

        typename boost::remove_pointer<typename Iter::value_type>::type val = **beg;
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
    };

  } // namespace BedHelpers

} // namespace Visitors

#endif // _VISITOR_BED_POST_PROCESSING_
