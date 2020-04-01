/*
// Author:  Shane Neph
// Date:    Sun Mar 18 15:28:01 PDT 2012
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

#ifndef SPECIAL_BED_ITERATOR_HEADERS_STARCH_CHR_SPECIFIC_MINMEM_HPP
#define SPECIAL_BED_ITERATOR_HEADERS_STARCH_CHR_SPECIFIC_MINMEM_HPP

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <istream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <sys/stat.h>

#include "algorithm/bed/FindBedRange.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "data/bed/Bed.hpp"
#include "data/starch/starchApi.hpp"
#include "suite/BEDOPS.Constants.hpp"
#include "utility/ByLine.hpp"
#include "utility/Exception.hpp"

namespace Bed {

  template <class BedType>
  class bed_check_iterator_mm;

  template <class BedType>
  class bed_check_iterator_mm<BedType*> {

  public:
    typedef Ext::UserError            Exception;
    typedef std::forward_iterator_tag iterator_category;
    typedef BedType*                  value_type;
    typedef std::ptrdiff_t            difference_type;
    typedef BedType**                 pointer;
    typedef BedType*&                 reference;
  
    static constexpr int nFields_  = BedType::NumFields;
    static constexpr bool hasRest_ = BedType::UseRest;
  
    bed_check_iterator_mm() : fp_(std::cin), _M_ok(false), _M_value(0), fn_(""), cnt_(0),
                              lastChr_(""), lastRest_(""), lastStart_(1), lastEnd_(0), nestCheck_(false),
                              maxEnd_(0), chr_(""), isStarch_(false), all_(true), archive_(0)
      { /* */ }

    bed_check_iterator_mm(std::istream& is, const std::string& filename, const std::string& chr = "all", bool nestCheck = false)
      : fp_(is), _M_ok(fp_), _M_value(0), fn_(filename), cnt_(0), lastChr_(""), lastRest_(""), lastStart_(1),
        lastEnd_(0), nestCheck_(nestCheck), maxEnd_(0), chr_(chr),
        isStarch_(false), all_(chr_ == "all"),
        archive_(0) {
  
      if ( !_M_ok )
        return;

      bool is_namedpipe = false;
      if ( fn_ != "-" && fn_ != "stdin" ) {
        struct stat st;
        if ( stat(filename.c_str(), &st) == -1 )
          throw(Exception("stat() failed on: " + fn_));
        is_namedpipe = (S_ISFIFO(st.st_mode) != 0);
      }

      isStarch_ = (fp_ && !is_namedpipe && (&is != &std::cin) && starch::Starch::isStarch(fn_));
      if ( isStarch_ ) // Starch constructor opens a stream for us
        dynamic_cast<std::ifstream&>(fp_).close(); // isStarch_ ensures fp_ is open and it's not std::cin/named pipe

      // compare pointers directly, to allow compilation with Clang/LLVM against C++11 standard
      if ( (&fp_ == &std::cin || is_namedpipe) && !all_ ) { // only BED through stdin; chromosome-specific
        // cannot 'jump' to chr_ -> stream through, line by line until we find it or eof
        Ext::ByLine bl;
        while ( (_M_ok = (fp_ && fp_ >> bl)) ) {
          ++cnt_;
          while ( !check(bl) ) {
            if ( fp_ >> bl )
              ++cnt_;
            else { /* only headers found */
              _M_ok = false;
              _M_value = static_cast<BedType*>(0);
              return;
            }
          } // while
          if ( _M_value->chrom() == chr_ )
            return;
          delete _M_value;
          _M_value = static_cast<BedType*>(0);
        } // while
        _M_value = static_cast<BedType*>(0);
        return;
      }
      if ( isStarch_ ) {
        const bool perLineUsage = true;
        const bool noHeaders = false;
        archive_ = new starch::Starch(fn_, noHeaders, perLineUsage, chr_);
        _M_ok = archive_->getArchiveRecordIter();
        if ( !_M_ok ) {
          delete archive_;
          archive_ = NULL;
        } else {
          std::string line;
          _M_ok = get_starch(line); // let starchApi deal with headers
          if ( _M_ok ) {
            ++cnt_;
            check(line);
          }
          _M_ok = (_M_ok && static_cast<bool>(_M_value) && 
                   static_cast<bool>(archive_->getArchiveRecordIter()) &&
                   !archive_->isEOF());
        }
      } else if ( !all_ ) { // BED, chromosome-specific
        // position fp_ to start of correct chromosome
        FILE* tmpf = std::fopen(fn_.c_str(), "r");
        if ( tmpf == NULL )
          throw(Exception("Internal problem opening file: " + fn_));
        std::fseek(tmpf, 0, SEEK_END);  // apparently dangerous on some platforms in binary mode -> padded nulls;
        const Bed::ByteOffset at_end = std::ftell(tmpf); // I'll assume msft is the problem until I know better
        std::rewind(tmpf);
  
        Bed::extract_details::TargetBedType* bt = new Bed::extract_details::TargetBedType(tmpf);
        if ( bt->chrom() == chr_ ) { // first entry is correct chromosome
          delete bt;
          std::rewind(tmpf);
          fp_.seekg(0, std::ios::beg);
  
          Ext::ByLine bl;
          if ( !(_M_ok && fp_ >> bl) )
            _M_ok = false;
          else {
            ++cnt_;
            while ( !check(bl) ) {
              if ( fp_ >> bl )
                ++cnt_;
              else { /* only headers found */
                _M_ok = false;
                break;
              }
            } // while
          }
          _M_ok = (_M_ok && fp_ && !fp_.eof());
        } else { // search for correct chromosome
          delete bt;
          std::rewind(tmpf);
          const bool done = false;
          std::vector<Bed::extract_details::TargetBedType*> v;
          while ( !done ) {
            bt = new Bed::extract_details::TargetBedType(tmpf);
            bt->start(std::numeric_limits<Bed::CoordType>::max()-1);
            bt->end(std::numeric_limits<Bed::CoordType>::max());
            v.push_back(bt);
            Visitors::Helpers::DoNothing nada;
            std::pair<bool, Bed::ByteOffset> lbound;
            lbound = find_bed_range(tmpf, v.begin(), v.end(), nada); // routine deletes bt for us
            v.clear(); // bt already deleted
            if ( lbound.first && lbound.second != at_end ) {
              std::fseek(tmpf, lbound.second, SEEK_SET);
              ByteOffset b = std::ftell(tmpf);
              Bed::extract_details::QueryBedType q(tmpf);
              std::fseek(tmpf, b, SEEK_SET);
              if ( q.chrom() == chr_ ) {
  
                fp_.seekg(b, std::ios::beg);
                Ext::ByLine bl;
                if ( !(_M_ok && fp_ >> bl) )
                  _M_ok = false;
                else {
                  ++cnt_;
                  while ( !check(bl) ) {
                    if ( fp_ >> bl )
                      ++cnt_;
                    else { /* only headers found */
                      _M_value = static_cast<BedType*>(0);
                      _M_ok = false;
                      break;
                    }
                  } // while
                }
                _M_ok = (_M_ok && fp_ && !fp_.eof());
                break;
              }
            }
            else {
              _M_value = static_cast<BedType*>(0);
              _M_ok = false;
              break;
            }
          } // while
        }
        std::fclose(tmpf);
      } else { // BED, process everything
        Ext::ByLine bl;
        if ( !(_M_ok && fp_ >> bl) )
          _M_ok = false;
        else {
          ++cnt_;
          while ( !check(bl) ) {
            if ( fp_ >> bl )
              ++cnt_;
            else { /* only headers found */
              _M_ok = false;
              break;
            }
          } // while
        }
      }
    }

    reference operator*() { return _M_value; }
    pointer operator->() { return &(operator*()); }

    bed_check_iterator_mm& operator++() {
      static Ext::ByLine bl;
      if ( _M_ok ) {
        if ( !isStarch_ ) { // bed
          if ( (_M_ok = (fp_ && fp_ >> bl)) ) {
            ++cnt_;
            if ( !check(bl) ) {
              std::stringstream s;
              s << cnt_;
              throw(Exception("in " + fn_ + "\nHeader found but should be at top of file.\nSee row: " + s.str()));
            } else if ( !all_ && (_M_value->chrom() != chr_) ) {
              delete _M_value;
              _M_value = static_cast<BedType*>(0);
              _M_ok = false;
            }
          }
        } else { // starch
          _M_ok = get_starch(bl);
          if ( _M_ok ) {
            ++cnt_;
            check(bl);
          }
          _M_ok = (_M_ok && static_cast<bool>(_M_value) && 
                   static_cast<bool>(archive_->getArchiveRecordIter()) &&
                   !archive_->isEOF());
        }
      }
      return *this;
    }

    bed_check_iterator_mm operator++(int)  {
      bed_check_iterator_mm __tmp = *this;
      static Ext::ByLine bl;
      if ( _M_ok ) {
        if ( !isStarch_ ) { // bed
          if ( (_M_ok = (fp_ && fp_ >> bl)) ) {
            ++cnt_;
            if ( !check(bl) ) {
              std::stringstream s;
              s << cnt_;
              throw(Exception("in " + fn_ + "\nHeader found but should be at top of file.\nSee row: " + s.str()));
            } else if ( !all_ && (_M_value->chrom() != chr_ ) ) {
              delete _M_value;
              _M_value = static_cast<BedType*>(0);
              _M_ok = false;
            }
          }
        } else { // starch
          _M_ok = get_starch(bl);
          if ( _M_ok ) {
            ++cnt_;
            check(bl);
          }
          _M_ok = (_M_ok && static_cast<bool>(_M_value) && 
                   static_cast<bool>(archive_->getArchiveRecordIter()) &&
                   !archive_->isEOF());
        }
      }
      return __tmp;
    }

    void clean() {
      if ( archive_ )
        delete archive_;
    }

    bool _M_equal(const bed_check_iterator_mm& __x) const
      { return ( (_M_ok == __x._M_ok) && (!_M_ok || &fp_ == &__x.fp_) ); }

    bool operator=(const bed_check_iterator_mm& b);

    bool has_nested() const { /* only known for Starch archives */
      if ( isStarch_ )
        return archive_->getAllChromosomesHaveNestedElement();
      return true; // assumption for BED
    }

  protected:
    std::string lowerstr(const std::string& s) {
      std::string t(s);
      for ( std::size_t i = 0; i < t.size(); ++i )
        t[i] = static_cast<char>(std::tolower(t[i]));
      return t;
    }
  
    bool isUCSCheader(const std::string& bl) {
      std::string tmp = lowerstr(bl);
      return (tmp == "browser" || tmp == "track");
    }
  
    bool get_starch(std::string& line) {
      if ( archive_ == NULL || !archive_->extractBEDLine(line) )
        return false;
      return !line.empty();
    }
  
    bool check(const std::string& bl) {
      static std::string msg = "";
      static int cmp = 0;
  
      msg.clear();
      if ( bl.empty() )
        msg = "Empty line found.";
      else if ( isUCSCheader(bl) )
        return false;
  
      // Check chromosome
      std::string::size_type marker = 0, sz = bl.size();
      while ( msg.empty() && marker < sz ) {
        if ( bl[marker] == ' ' ) {
          if ( isUCSCheader(bl.substr(0, marker)) )
            return false;
          msg = "First column should not have spaces.  Consider 'chr1' vs. 'chr1 '.  These are different names.\nsort-bed can correct this for you.";
        }
        else if ( 0 == marker && bl[marker] == '@' ) {
          return false; // SAM format header supported by starch
        }
        else if ( 0 == marker && bl[marker] == '#' ) {
          return false; // VCF format header supported by starch
        }
        else if ( bl[marker] == '\t' ) {
          if ( 0 == marker )
            msg = "First column name should not start with a tab.";
          else {
            if ( isUCSCheader(bl.substr(0, marker)) )
              return false;
            break;
          }
        }
        ++marker;
      } // while
  
      if ( msg.empty() ) {
        if ( sz <= marker )
          msg = "No tabs found in BED row.";
        else if ( marker > Bed::MAXCHROMSIZE ) { // marker is 0-based; but points to tab after chromosome name
          msg = "Chromosome name does not fit in MAXCHROMSIZE chars.\nIncrease TOKEN_CHR_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.";
          std::stringstream a, b;
          a << Bed::MAXCHROMSIZE;
          b << bl.substr(0, marker).size();
          msg += "\nMAXCHROMSIZE = " + a.str();
          msg += "; Size given = " + b.str();
        }
        else
          ++marker; // increment passed tab
      }
  
  
      // Check start coordinate
      static std::string::size_type pos = marker;
      pos = marker;
      while ( msg.empty() && marker < sz ) {
        if ( !std::isdigit(bl[marker]) ) {
          if ( bl[marker] == '\t' && pos != marker )
            break;
          else if ( bl[marker] == '\t' )
            msg += "Two or more consecutive tabs.  No start coordinate.";
          else if ( bl[marker] == '-' && marker == pos )
            msg += "Start coordinate cannot be < 0: ";
          else if ( bl[marker] == ' ' )
            msg += "Start coordinate may not contain a space: ";
          else {
            msg = "Start coordinate contains non-numeric character: ";
            msg += bl[marker];
          }
        }
        ++marker;
      } // while

      if ( msg.empty() ) {
        if ( sz <= marker )
          msg = "No tabs after start coordinate.";
        else {
          if ( marker-pos > MAX_DEC_INTEGERS )
            msg = "Sanity check failure - start coordinate has too many digits as defined by MAX_DEC_INTEGERS in BEDOPS.Constants.hpp";
          else if ( std::atof(bl.substr(pos, marker-pos).c_str()) > double(MAX_COORD_VALUE) )
            msg = "Sanity check failure - start coordinate is more than allowed by MAX_COORD_VALUE in BEDOPS.Constants.hpp";
          else if ( std::atof(bl.substr(pos, marker-pos).c_str()) > std::numeric_limits<Bed::CoordType>::max() )
            msg = "Start coordinate exceeds limits of a Bed::CoordType on your system.";
          else
            ++marker; // increment passed tab
        }
      }

      // Check end coordinate
      pos = marker;
      auto restMarker = marker;
      while ( msg.empty() && marker < sz ) {
        if ( !std::isdigit(bl[marker]) ) {
          if ( bl[marker] == '\t' && pos != marker )
            break;
          else if ( bl[marker] == '\t' )
            msg += "Two or more consecutive tabs.  No end coordinate.";
          else if ( bl[marker] == '-' && marker == pos )
            msg += "End coordinate cannot be < 0: ";
          else if ( bl[marker] == ' ' )
            msg += "End coordinate may not contain a space: ";
          else {
            msg = "End coordinate contains non-numeric character: ";
            msg += bl[marker];
          }
        }
        ++marker;
      } // while

      if ( msg.empty() ) {
        if ( sz <= marker && nFields_ > 3 ) {
          std::stringstream con; con << nFields_;
          msg = "Only 3 columns given.  Require at least " + con.str();
        }
        else {
          if ( marker-pos > MAX_DEC_INTEGERS )
            msg = "Sanity check failure - start coordinate has too many digits as defined by MAX_DEC_INTEGERS in BEDOPS.Constants.hpp";
          else if ( std::atof(bl.substr(pos, marker-pos).c_str()) > double(MAX_COORD_VALUE) )
            msg = "Sanity check failure - start coordinate is more than allowed by MAX_COORD_VALUE in BEDOPS.Constants.hpp";
          else if ( std::atof(bl.substr(pos, marker-pos).c_str()) > std::numeric_limits<Bed::CoordType>::max() )
            msg = "End coordinate exceeds limits of an Bed::CoordType on your system.";
          else
            ++marker; // increment passed tab
        }
      }

      if ( nFields_ > 3 ) { // check ID field
        pos = marker;
        while ( msg.empty() && marker < sz ) {
          if ( bl[marker] == '\t' && pos != marker )
            break;
          else if ( bl[marker] == '\t' )
            msg += "Two or more consecutive tabs.  No ID field.";
          else if ( bl[marker] == ' ' )
            msg += "ID field may not contain a space.";
  
          ++marker;
        } // while

        if ( msg.empty() ) {
          if ( sz <= marker && nFields_ > 4 ) {
            std::stringstream con; con << nFields_;
            msg = "Only 4 columns given.  Require at least " + con.str();
          }
          else if ( pos == marker )
            msg = "Fourth (id) column is empty.";
          else if ( marker-pos > Bed::MAXIDSIZE ) { // marker points at the tab after ID field, and it's 0-based
            msg = "ID field does not fit in MAXCHROMSIZE chars.\nIncrease TOKEN_ID_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.";
            std::stringstream a, b;
            a << Bed::MAXIDSIZE;
            b << bl.substr(pos, marker-pos).size();
            msg += "\nMAXIDSIZE = " + a.str();
            msg += "; Size given = " + b.str();
          }
          else
            ++marker; // increment passed tab
        }

        if ( nFields_ > 4 ) { // check measurement column
          pos = marker;
          static int decimalCount = 0;
          static int expCount = 0;
          static std::size_t expPos = 0;
          static int minusCount = 0;
          static std::size_t minusPos = 0;
          decimalCount = 0;
          expCount = 0;
          expPos = 0;
          minusCount = 0;
          minusPos = 0;
          while ( msg.empty() && marker < sz ) {
            if ( !std::isdigit(bl[marker]) ) {
              if ( bl[marker] == '\t' && pos != marker )
                break;
              else if ( bl[marker] == '\t' )
                msg = "Two or more consecutive tabs.  No measurement given.";
              else if ( bl[marker] == '.' ) {
                if ( ++decimalCount > 1 )
                  msg = "More than one decimal point in measurement field.";
                else if ( expCount > 0 )
                  msg = "Bad decimal point - part of exponent.";
              }
              else if ( bl[marker] == 'e' || bl[marker] == 'E' ) {
                if ( ++expCount > 1 )
                  msg = "Measurement value contains non-numeric character (multiple 'E' or 'e' characters detected).";
                expPos = marker;
              }
              else if ( bl[marker] == ' ' )
                msg = "Measurement value may not contain a space.";
              else if ( bl[marker] == '-' || bl[marker] == '+' ) {
                if ( marker != pos && expCount < 1 )
                  msg = "Measurement value has '-' or '+' in wrong place.";
                if ( msg.empty() && marker != pos ) {
                  if ( ++minusCount > 1 )
                    msg = "Measurement value has multiple '-' and/or '+' characters.";
                  else if ( expPos + 1 != marker )
                    msg = "Measurement value has bad '-' in the exponent.";
                  minusPos = marker;
                }
              }
              else {
                msg = "Measurement value contains non-numeric character: ";
                msg += bl[marker];
              }
            }
            ++marker;
          } // while
  
          if ( msg.empty() ) {
            if ( sz <= marker && nFields_ > 5 ) {
              std::stringstream con; con << nFields_;
              msg += "Only 5 columns given.  Require at least " + con.str();
            }
            else if ( pos == marker )
              msg = "Fifth (measure) column is empty.";
            else if ( minusPos > 0 && minusPos + 1 == marker )
              msg = "Measurement value ends with a '-'.";
            else
              ++marker; // increment passed tab
          }
  
  
          if ( nFields_ > 5 ) { // check strand column
            pos = marker;
            while ( msg.empty() && marker < sz ) {
              if ( bl[marker] != '+' && bl[marker] != '-' ) {
                if ( bl[marker] == '\t' && pos != marker )
                  break;
                else if ( bl[marker] == '\t' )
                  msg += "Two or more consecutive tabs.  No strand information given.";
                else {
                  msg = "Strand (6th) column must be '+' or '-' (with no spaces).  Received: ";
                  msg += bl[marker];
                  msg += "\nsort-bed can correct this for you.";
                }
              } else if ( marker != pos ) {
                msg += "Two or more consecutive '+' or '-'s detected.";
              }
              ++marker;
            } // while
          }
  
          if ( msg.empty() ) {
            if ( pos == marker )
              msg = "Sixth (strand) column is empty.";
            else
              ++marker; // increment passed tab.
          }
        }
      }
  
      if ( msg.empty() && hasRest_ && marker < sz && sz - marker > Bed::MAXRESTSIZE ) {
        msg = "The 'rest' of the input row (everything beyond the first ";
        std::stringstream a, b, c;
        a << Bed::MAXRESTSIZE;
        b << bl.substr(marker).size();
        c << nFields_;
        msg += c.str();
        msg += " fields) cannot fit into MAXRESTSIZE chars.\nIncrease TOKEN_REST_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.";
        msg += "\nMAXRESTSIZE = " + a.str();
        msg += "; Size given = " + b.str();
      }
  
      if ( !msg.empty() ) {
        std::stringstream s;
        s << cnt_;
        throw(Exception("in " + fn_ + "\n" + msg + "\nSee row: " + s.str()));
      }

      _M_value = new BedType(bl);
      if ( msg.empty() && !lastChr_.empty() ) {
        cmp = std::strcmp(_M_value->chrom(), lastChr_.c_str());
        if ( cmp < 0 )
          msg = "Bed file not properly sorted by first column.";
        else if ( cmp == 0 ) {
          if ( _M_value->start() < lastStart_ )
            msg = "Bed file not properly sorted by start coordinates.";
          else if ( _M_value->start() == lastStart_ ) {
            if ( _M_value->end() < lastEnd_ )
              msg = "Bed file not properly sorted by end coordinates when start coordinates are identical.";
            else if ( hasRest_ && _M_value->end() == lastEnd_ ) {
              if ( std::strcmp(bl.substr(restMarker).c_str(), lastRest_.c_str()) < 0 )
                msg = "Bed file not sorted by information following the 3rd column (columns 1-3 equal to previous row).";
            }
          }

          if ( msg.empty() && nestCheck_ ) { // _M_value->start() > lastStart_
            if ( _M_value->end() < maxEnd_ )
              msg = "Fully nested component found.";
          }
        }
      }

      if ( msg.empty() && _M_value->end() <= _M_value->start() )
        msg = "End coordinates must be greater than start coordinates.";
  
      if ( !msg.empty() ) {
        std::stringstream s;
        s << cnt_;
        throw(Exception("in " + fn_ + "\n" + msg + "\nSee row: " + s.str()));
      }
  
      lastChr_ = _M_value->chrom();
      lastStart_ = _M_value->start();
      lastEnd_ = _M_value->end();
      lastRest_ = bl.substr(restMarker);
      maxEnd_ = lastEnd_;
      return true;
    }

  private:
    std::istream& fp_;
    bool _M_ok;
    BedType* _M_value;
    std::string fn_;
    Bed::CoordType cnt_;
    std::string lastChr_, lastRest_;
    Bed::CoordType lastStart_, lastEnd_;
    bool allowHeaders_;
    bool nestCheck_;
    Bed::CoordType maxEnd_;
    std::string chr_;
    bool isStarch_;
    const bool all_;
    starch::Starch* archive_;
  };
  
  template <class BedType>
  inline bool 
  operator==(const bed_check_iterator_mm<BedType>& __x,
             const bed_check_iterator_mm<BedType>& __y) {
    return __x._M_equal(__y);
  }
  
  template <class BedType>
  inline bool 
  operator!=(const bed_check_iterator_mm<BedType>& __x,
             const bed_check_iterator_mm<BedType>& __y) {
    return !__x._M_equal(__y);
  }

} // namespace Bed

#endif // SPECIAL_BED_ITERATOR_HEADERS_STARCH_CHR_SPECIFIC_MINMEM_HPP
