/*
  FILE: SelectMeasureType.hpp
  AUTHOR: Shane Neph, Scott Kuen
  CREATE DATE: Fri Jul 27 11:49:03 PDT 2007
  PROJECT: data/measurement
  ID: $Id: AssayMeasurement.hpp 1745 2010-09-13 23:39:13Z sjn $
*/

//
//    BEDOPS
//    Copyright (C) 2011, 2012, 2013 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef _MEASURE_SELECTOR_HPP
#define _MEASURE_SELECTOR_HPP

#include <boost/type_traits.hpp>

namespace Signal {
  namespace Details {
    // The main idea:  SelectMeasure inherits from SelectMeasureImpl<T, T>.  When T happens
    //  to be a built-in type, then the specialization <T,T> matches best over the general
    //  <UserDefined,U> SelectMeasureImpl.  When T is not a built-in type, the specialization is
    //  invalidated from entering the overload set and the general case is inherited from.

    template <typename T, typename U>
    struct SelectMeasureImpl; // Forward Decl

    // built-ins
    template <typename BuiltIn>
    struct SelectMeasureImpl<BuiltIn, typename boost::enable_if< boost::is_arithmetic<BuiltIn>, BuiltIn>::type> {
      typedef BuiltIn MeasureType;
    };

    // user-defined : requires MeasurementType typedef
    //   as used by SelectMeasure: UserDefined==U always, and T is not a built-in
    template <typename UserDefined, typename U>
    struct SelectMeasureImpl {
      typedef typename UserDefined::MeasurementType MeasureType;
    };
  } // namespace Details

  // General implementation simply inherits a specialization from above
  //  Requires T::MeasurementType to be defined.  This is useful for building
  //  algorithms -> as long as T::MeasurementType is (eventually) defined to
  //  be a built in numeric type, then we will (eventually) inherit from
  //  a specialization from above, allowing an algorithm to be written in
  //  terms of fundamental types via conversion functions.
  template <typename T>
  struct SelectMeasure : public Details::SelectMeasureImpl<T, T> {
    // Internal typedefs
    typedef Details::SelectMeasureImpl<T, T> BaseClass;
    typedef typename BaseClass::MeasureType MeasureType;
  };
} // namespace Signal

#endif // _MEASURE_SELECTOR_HPP
