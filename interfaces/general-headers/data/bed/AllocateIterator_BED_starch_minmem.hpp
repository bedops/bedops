/*
  Author:  Shane Neph & Alex Reynolds
  Date:    Sun Jul 22 15:49:45 PDT 2012
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

#ifndef SPECIAL_STARCH_ALLOCATE_NEW_ITERATOR_CHR_SPECIFIC_MINMEM_HPP
#define SPECIAL_STARCH_ALLOCATE_NEW_ITERATOR_CHR_SPECIFIC_MINMEM_HPP

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>

#include <sys/stat.h>

#include "algorithm/bed/FindBedRange.hpp"
#include "algorithm/visitors/helpers/ProcessVisitorRow.hpp"
#include "data/bed/Bed_minmem.hpp"
#include "data/starch/starchApi.hpp"
#include "utility/FPWrap.hpp"

namespace Bed {

  template <class BedType>
  class allocate_iterator_starch_bed_mm;

  template <class BedType>
  class allocate_iterator_starch_bed_mm<BedType*> {

  public:
    typedef std::forward_iterator_tag iterator_category;
    typedef BedType*                  value_type;
    typedef std::ptrdiff_t            difference_type;
    typedef BedType**                 pointer;
    typedef BedType*&                 reference;
  
    allocate_iterator_starch_bed_mm() : fp_(NULL), _M_ok(false), _M_value(0), is_starch_(false), all_(false), archive_(NULL) { chr_[0] = '\0'; }

    template <typename ErrorType>
    allocate_iterator_starch_bed_mm(Ext::FPWrap<ErrorType>& fp, const std::string& chr = "all") /* this ASSUMES fp is open and meaningful */
      : fp_(fp), _M_ok(fp_ && !std::feof(fp_)), _M_value(0),
        is_starch_(false),
        all_(0 == std::strcmp(chr.c_str(), "all")), archive_(NULL) {

      chr_[0] = '\0';
      std::size_t sz = std::min(chr.size(), static_cast<std::size_t>(Bed::MAXCHROMSIZE));
      std::strncpy(chr_, chr.c_str(), sz);
      chr_[sz] = '\0';

      if ( !_M_ok ) {
        if ( fp_ )
          fp_ = NULL;
        return;
      }

      bool is_namedpipe = false;
      if ( fp.Name() != "-" ) {
        struct stat st;
        if ( stat(fp.Name().c_str(), &st) == -1 )
          throw(ErrorType("Error: stat() failed on: " + fp.Name()));
        is_namedpipe = (S_ISFIFO(st.st_mode) != 0);
      }
      is_starch_ = !is_namedpipe && _M_ok && (fp_ != stdin) && starch::Starch::isStarch(fp_);

      if ( (fp_ == stdin || is_namedpipe) && !all_ ) { // BED, chrom-specific, using stdin
        // stream through until we find what we want
        while ( (_M_ok = (fp_ && !std::feof(fp_))) ) {
          _M_value = new BedType(fp_);
          if ( 0 == std::strcmp(_M_value->chrom(), chr_) ) {
            _M_ok = (fp_ && !std::feof(fp_));
            break;
          }
          delete _M_value;
        } // while
        if ( !_M_ok && fp_ )
          fp_ = NULL;
        return;
      }
  
      if ( is_starch_ ) { // starch archive can deal with all or specific chromosomes
        const bool perLineUsage = true;
        archive_ = new starch::Starch(fp_, chr_, perLineUsage);
        _M_ok = archive_->getArchiveRecordIter();
        if ( !_M_ok ) {
          fp_ = NULL;
          delete archive_;
        } else {
          _M_value = get_starch();
          _M_ok = (static_cast<bool>(_M_value) && 
                   (archive_->getArchiveRecordIter() != NULL) &&
                   !archive_->isEOF());
        }
      } else if ( !all_ ) { // BED, chromosome-specific
        // position fp_ to start of correct chromosome
        std::fseek(fp_, 0, SEEK_END);  // apparently dangerous on some platforms in binary mode -> padded nulls;
        const Bed::ByteOffset at_end = std::ftell(fp_); // I'll assume msft is the problem until I know better
        std::rewind(fp_);
  
        Bed::extract_details::TargetBedType* bt = new Bed::extract_details::TargetBedType(fp_);
        if ( 0 == std::strcmp(bt->chrom(), chr_) ) {
          delete bt;
          std::rewind(fp_);
          _M_value = new BedType(fp_);
          _M_ok = (_M_ok && fp_ && !std::feof(fp_));
          if ( !_M_ok && fp_ )
            fp_ = NULL;
        } else {
          delete bt;
          std::rewind(fp_);
          const bool done = false;
          std::vector<Bed::extract_details::TargetBedType*> v;
          while ( !done ) {
            bt = new Bed::extract_details::TargetBedType(fp_);
            bt->start(std::numeric_limits<Bed::CoordType>::max()-1);
            bt->end(std::numeric_limits<Bed::CoordType>::max());
            v.push_back(bt);
            Visitors::Helpers::DoNothing nada;
            std::pair<bool, Bed::ByteOffset> lbound;
            lbound = find_bed_range(fp_, v.begin(), v.end(), nada); // routine deletes bt for us
            v.clear(); // bt already deleted
            if ( lbound.first && lbound.second != at_end ) {
              std::fseek(fp_, lbound.second, SEEK_SET);
              ByteOffset b = std::ftell(fp_);
              Bed::extract_details::QueryBedType q(fp_);
              std::fseek(fp_, b, SEEK_SET);
              if ( 0 == std::strcmp(q.chrom(), chr_) ) {
                _M_value = new BedType(fp_);
                _M_ok = (_M_ok && fp_ && !std::feof(fp_));
                if ( !_M_ok && fp_ )
                  fp_ = NULL;
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
      } else { // BED, process everything
        _M_value = new BedType(fp_);
        _M_ok = (_M_ok && fp_ && !std::feof(fp_));
        if ( !_M_ok && fp_ )
          fp_ = NULL;
      }
    }
  
    reference operator*() { return _M_value; }
    pointer operator->() { return &(operator*()); }
  
    allocate_iterator_starch_bed_mm& operator++() { 
      if ( _M_ok ) {
        if ( !is_starch_ ) {
          _M_value = new BedType(fp_);
          _M_ok = !std::feof(fp_) && (all_ || 0 == std::strcmp(_M_value->chrom(), chr_));
          // very small leak in event that !all_ and _M_value->chrom() is not chr_
          //   too expensive to check
        } else {
          _M_value = get_starch();
          _M_ok = (static_cast<bool>(_M_value) && 
                   (archive_->getArchiveRecordIter() != NULL) &&
                   !archive_->isEOF());
        }
      }
      return *this;
    }
  
    allocate_iterator_starch_bed_mm operator++(int)  {
      allocate_iterator_starch_bed_mm __tmp = *this;
      if ( _M_ok ) {
        if ( !is_starch_ ) {
          _M_value = new BedType(fp_);
          _M_ok = !std::feof(fp_) && (all_ || 0 == std::strcmp(_M_value->chrom(), chr_));
          // very small leak in event that !all_ and _M_value->chrom() is not chr_
          //   too expensive to check
        } else {
          _M_value = get_starch();
          _M_ok = (static_cast<bool>(_M_value) && 
                   (archive_->getArchiveRecordIter() != NULL) &&
                   !archive_->isEOF());
        }
      }
      return __tmp;
    }
  
    bool _M_equal(const allocate_iterator_starch_bed_mm& __x) const {
       return (
                (_M_ok == __x._M_ok) &&
                (!_M_ok || fp_ == __x.fp_)
              ); 
    }

    bool has_nested() const { /* only known for Starch archives */
      if ( is_starch_ )
        return archive_->getAllChromosomesHaveNestedElement();
      return true; // assumption for BED
    }
  
  private:
    inline BedType* get_starch() {
      static std::string line;
      if ( archive_ == NULL || !archive_->extractBEDLine(line) )
        return(0);
      return(new BedType(line.c_str()));
    }
  
  private:
    FILE* fp_;
    bool _M_ok;
    char chr_[Bed::MAXCHROMSIZE+1];
    BedType* _M_value;
    bool is_starch_;
    const bool all_;
    starch::Starch* archive_;
  };
  
  template <class BedType>
  inline bool 
  operator==(const allocate_iterator_starch_bed_mm<BedType>& __x,
             const allocate_iterator_starch_bed_mm<BedType>& __y) {
    return __x._M_equal(__y);
  }
  
  template <class BedType>
  inline bool 
  operator!=(const allocate_iterator_starch_bed_mm<BedType>& __x,
             const allocate_iterator_starch_bed_mm<BedType>& __y) {
    return !__x._M_equal(__y);
  }

} // namespace Bed

#endif // SPECIAL_STARCH_ALLOCATE_NEW_ITERATOR_CHR_SPECIFIC_MINMEM_HPP
