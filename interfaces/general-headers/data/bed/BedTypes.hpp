/*
  Author: Scott Kuehn, Shane Neph
  Date:   Tue Aug 14 14:44:45 PDT 2007
*/

//
//    BEDOPS
//    Copyright (C) 2011-2016 Shane Neph, Scott Kuehn and Alex Reynolds
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

#ifndef BEDTYPES_HPP
#define BEDTYPES_HPP

#include "data/bed/Bed.hpp"

namespace Bed {

  /***********************************************/
  /* Typedef helper for user applications        */
  /***********************************************/
  template <bool UseNonStaticChrom, bool UseRest, typename MType = double>
  struct BedTypes {
    typedef MType                                   MeasureType;
    typedef BasicCoords<UseNonStaticChrom, UseRest> Bed3Type;
    typedef Bed4< Bed3Type, UseRest >               Bed4Type;
    typedef Bed5< Bed4Type, MeasureType, UseRest >  Bed5Type;
    typedef Bed6< Bed5Type, UseRest >               Bed6Type;
  };

  enum { Rest = true, NoRest = false, OneChrom = false, AllChrom = true };


  /***********************************************/
  /* Common typedefs                             */
  /***********************************************/
  typedef BedTypes<AllChrom, Rest, double>    BTAllRest;
  typedef BedTypes<AllChrom, NoRest, double>  BTAllNoRest;
  typedef BedTypes<OneChrom, Rest, double>    BTOneRest;
  typedef BedTypes<OneChrom, NoRest, double>  BTOneNoRest;

  typedef BTAllRest::Bed3Type   B3Rest;
  typedef BTAllNoRest::Bed3Type B3NoRest;
  typedef BTOneRest::Bed3Type   B3OneChromRest;
  typedef BTOneNoRest::Bed3Type B3OneChromNoRest;

  typedef BTAllRest::Bed4Type   B4Rest;
  typedef BTAllNoRest::Bed4Type B4NoRest;
  typedef BTOneRest::Bed4Type   B4OneChromRest;
  typedef BTOneNoRest::Bed4Type B4OneChromNoRest;

  typedef BTAllRest::Bed5Type   B5Rest;
  typedef BTAllNoRest::Bed5Type B5NoRest;
  typedef BTOneRest::Bed5Type   B5OneChromRest;
  typedef BTOneNoRest::Bed5Type B5OneChromNoRest;

  typedef BTAllRest::Bed6Type   B6Rest;
  typedef BTAllNoRest::Bed6Type B6NoRest;
  typedef BTOneRest::Bed6Type   B6OneChromRest;
  typedef BTOneNoRest::Bed6Type B6OneChromNoRest;

} // namespace Bed

#endif // BEDTYPES_HPP
