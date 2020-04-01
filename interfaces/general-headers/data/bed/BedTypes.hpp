/*
  Author: Scott Kuehn, Shane Neph
  Date:   Tue Aug 14 14:44:45 PDT 2007
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

#ifndef BEDTYPES_HPP
#define BEDTYPES_HPP

#include "data/bed/Bed.hpp"
#include "data/bed/Bed_minmem.hpp"
#include "suite/BEDOPS.Constants.hpp"

namespace Bed {

  /***********************************************/
  /* Typedef helper for user applications        */
  /***********************************************/
  template <bool UseNonStaticChrom, bool UseRest, typename MType = Bed::MeasurementType, bool MemPool = true>
  struct BedTypes {
    typedef MType                                   MeasureType;
    typedef BasicCoords<UseNonStaticChrom, UseRest> Bed3Type;
    typedef Bed4<Bed3Type, UseRest>                 Bed4Type;
    typedef Bed5<Bed4Type, MeasureType, UseRest>    Bed5Type;
  };

  template <bool UseNonStaticChrom, bool UseRest, typename MType>
  struct BedTypes<UseNonStaticChrom, UseRest, MType, false> {
    typedef MType                                           MeasureType;
    typedef NoPool::BasicCoords<UseNonStaticChrom, UseRest> Bed3Type;
    typedef NoPool::Bed4<Bed3Type, UseRest>                 Bed4Type;
    typedef NoPool::Bed5<Bed4Type, MeasureType, UseRest>    Bed5Type;
  };

  enum { Rest = true, NoRest = false, OneChrom = false, AllChrom = true, NoPooling = false };


  /***********************************************/
  /* Common typedefs                             */
  /***********************************************/
  typedef BedTypes<AllChrom, Rest, Bed::MeasurementType>    BTAllRest;
  typedef BedTypes<AllChrom, NoRest, Bed::MeasurementType>  BTAllNoRest;
  typedef BTAllRest::Bed3Type   B3Rest;
  typedef BTAllNoRest::Bed3Type B3NoRest;

  typedef BTAllRest::Bed4Type   B4Rest;
  typedef BTAllNoRest::Bed4Type B4NoRest;

  typedef BTAllRest::Bed5Type   B5Rest;
  typedef BTAllNoRest::Bed5Type B5NoRest;


  typedef BedTypes<AllChrom, Rest, Bed::MeasurementType, NoPooling>    BTAllRestNoPool;
  typedef BedTypes<AllChrom, NoRest, Bed::MeasurementType, NoPooling>  BTAllNoRestNoPool;

  typedef BTAllRestNoPool::Bed3Type   B3RestNoPool;
  typedef BTAllNoRestNoPool::Bed3Type B3NoRestNoPool;

  typedef BTAllRestNoPool::Bed4Type   B4RestNoPool;
  typedef BTAllNoRestNoPool::Bed4Type B4NoRestNoPool;

  typedef BTAllRestNoPool::Bed5Type   B5RestNoPool;
  typedef BTAllNoRestNoPool::Bed5Type B5NoRestNoPool;
} // namespace Bed

#endif // BEDTYPES_HPP
