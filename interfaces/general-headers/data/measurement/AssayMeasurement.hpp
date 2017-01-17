/*
  Author: Scott Kuehn, Shane Neph
  Date:   Fri Jul 27 11:49:03 PDT 2007
*/
//
//    BEDOPS
//    Copyright (C) 2011-2017 Shane Neph, Scott Kuehn and Alex Reynolds
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

#include <limits>
#include <type_traits>

#include "utility/Formats.hpp"

#ifndef ASSAY_MEASUREMENT_HPP
#define ASSAY_MEASUREMENT_HPP

namespace Signal {
  namespace Details {

    // The main idea:  AssayMeasurement inherits from AssayMeasurementImpl<T, T>.  When T happens
    //  to be a built-in type, then the specialization <T,T> matches best over the general
    //  <T,U> AssayMeasurementImpl.  When T is not a built-in type, the specialization is
    //  invalidated from entering the overload set and the general case is inherited from.

    template <typename T, typename U>
    struct AssayMeasurementImpl; // Forward Decl

    template <typename T>
    struct AssayMeasurementImpl<T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type> {
      typedef T value_type;

      explicit AssayMeasurementImpl(T m = 0) : measurement_(m)  { }
      AssayMeasurementImpl(const AssayMeasurementImpl& a) : measurement_(a.measurement_) { /* */ }
      bool isNaN() const
        { return NL::has_quiet_NaN && measurement_ == NL::quiet_NaN(); }
      T measurement() const { return measurement_; }
      void measurement(T m) { measurement_ = m; }
      operator T() const { return measurement_; }
      AssayMeasurementImpl& operator=(T t) { measurement_ = t; return *this; }
      AssayMeasurementImpl& operator=(const AssayMeasurementImpl& a)
        { measurement_ = a.measurement_; return *this; }

      static inline char const* formatter()
        { return Formats::Format(typename std::remove_cv<T>::type()); }

    protected:
      T measurement_;
      typedef std::numeric_limits<T> NL;
    };


    // General implementation simply inherits a specialization from above
    //  Requires T::MEASURETYPE to be defined.  This is useful for building
    //  algorithms -> as long as T::MEASURETYPE is (eventually) defined to
    //  be a built in numeric type, then we will (eventually) inherit from
    //  a specialization from above, allowing an algorithm to be written in
    //  terms of fundamental types via conversion functions.
    template <typename T, typename U>
    struct AssayMeasurementImpl
      : public AssayMeasurementImpl<typename T::MEASURETYPE, typename T::MEASURETYPE> {
      typedef typename T::MEASURETYPE MEASURETYPE; // in case T is a spec of this class
      typedef AssayMeasurementImpl<MEASURETYPE, MEASURETYPE> BaseClass;

      AssayMeasurementImpl() : BaseClass() { /* */ }
      AssayMeasurementImpl(typename BaseClass::value_type t) : BaseClass(t) { /* */ }
      AssayMeasurementImpl(const AssayMeasurementImpl& m) : BaseClass(m) { /* */ }
    };

  } // namespace Signal::Details



  //==================
  // AssayMeasurement
  //==================
  template <typename T>
  struct AssayMeasurement : public Details::AssayMeasurementImpl<T, T> {
    // Internal typedefs
    typedef Details::AssayMeasurementImpl<T, T> BaseClass;
    typedef typename BaseClass::value_type value_type;

    // Construction
    AssayMeasurement() : BaseClass() { /* */ }
    explicit AssayMeasurement(value_type t) : BaseClass(t) { /* */ }
    AssayMeasurement(const AssayMeasurement& a) : BaseClass(a) { /* */ }

    // Copy-assignment
    AssayMeasurement& operator=(const AssayMeasurement& a)
      { BaseClass::measurement_ = a.measurement_; return *this; }
    AssayMeasurement& operator=(value_type t)
      { BaseClass::measurement_ = t; return *this; }

    // non-friend operators
    AssayMeasurement& operator-=(const AssayMeasurement& a)
      { BaseClass::measurement_ -= a.measurement_; return *this; }
    AssayMeasurement& operator+=(const AssayMeasurement& a)
      { BaseClass::measurement_ += a.measurement_; return *this; }
    AssayMeasurement& operator*=(const AssayMeasurement& a)
      { BaseClass::measurement_ *= a.measurement_; return *this; }
    AssayMeasurement& operator/=(const AssayMeasurement& a)
      { BaseClass::measurement_ /= a.measurement_; return *this; }
    AssayMeasurement& operator++()
      { ++BaseClass::measurement_; return *this; }
    AssayMeasurement operator++(int)
      { AssayMeasurement cpy(*this); ++(*this); return cpy; }
    AssayMeasurement& operator--()
      { --BaseClass::measurement_; return *this; }
    AssayMeasurement operator--(int)
      { AssayMeasurement cpy(*this); --(*this); return cpy; }

    // friend operator functions
    friend bool operator<(const AssayMeasurement& a, const AssayMeasurement& b)
      { return(a.measurement_ < b.measurement_); }
    friend bool operator<=(const AssayMeasurement& a, const AssayMeasurement& b)
      { return(a.measurement_ <= b.measurement_); }
    friend bool operator>(const AssayMeasurement& a, const AssayMeasurement& b)
      { return(a.measurement_ > b.measurement_); }
    friend bool operator>=(const AssayMeasurement& a, const AssayMeasurement& b)
      { return(a.measurement_ >= b.measurement_); }
    friend bool operator==(const AssayMeasurement& a, const AssayMeasurement& b)
      { return(a.measurement_ == b.measurement_); }
    friend bool operator!=(const AssayMeasurement& a, const AssayMeasurement& b)
      { return(a.measurement_ != b.measurement_); }
    friend AssayMeasurement operator-(const AssayMeasurement& a, const AssayMeasurement& b)
      { AssayMeasurement toRtn(a.measurement_ - b.measurement_); return toRtn; }
    friend AssayMeasurement operator+(const AssayMeasurement& a, const AssayMeasurement& b)
      { AssayMeasurement toRtn(a.measurement_ + b.measurement_); return toRtn; }
    friend AssayMeasurement operator*(const AssayMeasurement& a, const AssayMeasurement& b)
      { AssayMeasurement toRtn(a.measurement_ * b.measurement_); return toRtn; }
    friend AssayMeasurement operator/(const AssayMeasurement& a, const AssayMeasurement& b)
      { AssayMeasurement toRtn(a.measurement_ / b.measurement_); return toRtn; }
  };
} // namespace Signal

#endif // ASSAY_MEASUREMENT_HPP
