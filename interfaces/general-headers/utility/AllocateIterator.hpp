/*
  Author:  Shane Neph & Scott Kuehn
  Date:    Tue Aug 28 09:36:24 PDT 2007
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

#ifndef SPECIAL_ALLOCATE_NEW_ITERATOR_HPP
#define SPECIAL_ALLOCATE_NEW_ITERATOR_HPP

#include <iterator>
#include <cstddef>
#include <cstdio>

namespace Ext {

  template <class _Tp>
  class allocate_iterator;

  template <class _Tp>
  class allocate_iterator<_Tp*> {

  public:
    typedef std::forward_iterator_tag iterator_category;
    typedef _Tp*                      value_type;
    typedef std::ptrdiff_t            difference_type;
    typedef _Tp**                     pointer;
    typedef _Tp*&                     reference;

    allocate_iterator() : _M_ok(false), fp_(NULL), _M_value(0) {}
    allocate_iterator(FILE* fp) /* this ASSUMES fp is open and meaningful */
      : _M_ok(fp && !std::feof(fp)), fp_(fp), _M_value(_M_ok ? new _Tp(fp) : 0)
      {
        _M_ok = (_M_ok && fp && !std::feof(fp));
        if ( !_M_ok && fp_ )
          fp_ = NULL;
      }

    reference operator*() { return _M_value; }
    pointer operator->() { return &(operator*()); }

    allocate_iterator& operator++() { 
      if ( _M_ok ) {
        _M_value = (new _Tp(fp_));
        _M_ok = !std::feof(fp_);
      }
      return *this;
    }

    allocate_iterator operator++(int)  {
      allocate_iterator __tmp = *this;
      if ( _M_ok ) {
        _M_value = (new _Tp(fp_));
        _M_ok = !std::feof(fp_);
      }
      return __tmp;
    }

    bool _M_equal(const allocate_iterator& __x) const
      { return ( (_M_ok == __x._M_ok) && (!_M_ok || fp_ == __x.fp_) ); }


  private:
    bool _M_ok;
    FILE* fp_;
    _Tp* _M_value;
  };

  template <class _Tp>
  inline bool 
  operator==(const allocate_iterator<_Tp>& __x,
             const allocate_iterator<_Tp>& __y) {
    return __x._M_equal(__y);
  }

  template <class _Tp>
  inline bool 
  operator!=(const allocate_iterator<_Tp>& __x,
             const allocate_iterator<_Tp>& __y) {
    return !__x._M_equal(__y);
  }

} // namespace Ext

#endif // SPECIAL_ALLOCATE_NEW_ITERATOR_HPP
